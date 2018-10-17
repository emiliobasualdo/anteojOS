#include <roundRobin.h>

static boolean rrIsEmpty(rrQueue *pQueue);
static boolean changeToRespectiveQueue(rrNodePtr node);
static void printNormalQueue(rrQueue *queue);
static pcbPtr resetQueue(rrQueue *queue);
static boolean normalQueueAddNodeTail(rrNodePtr newNode, rrQueue *pQueue);
static rrNodePtr normalQueuePop(rrQueue *queue);
static boolean funcionAuxiliar(rrNodePtr node);
static void resetAllQueues();
static rrNodePtr normalQueueAddTail(pcbPtr pcbPtr, rrQueue *pQueue);
static rrNodePtr createNewNode(pcbPtr pcbPtr);
static rrQueue *getNextQueueAux();
static void changeToRespectiveStructure(rrNodePtr pNode, rrQueuePtr qsVector);
static void switchStructures();
static struct rrQueue* getNextQueue();
static rrNodePtr handleCurrent();
static rrNodePtr getNextReadyNode();
static void refreshLHPA(int priority);
static void removeNode(rrNodePtr node);
static void normalQueueRemoveNode(rrNodePtr node);
static void printStructure(rrQueue *pQueue);

/** Arrays estaticos de los vectores de colas.
* Se los accede con punteros y se usan como estructuras para
* guardad colas de procesos terminadas Ó en ejecucion */
static rrQueue queue1[PRIORITY_LEVELS];
static rrQueue queue2[PRIORITY_LEVELS];

/** Puntero a los vectores de colas, los cuales vamos a ir intercambiando*/
static rrQueuePtr runningQueques =  queue1;
static rrQueuePtr finishedQueques =  queue2;

/** Vector de contenedor de procesos para acceder a los nodos en O(k).
 * Sino lo tengo que andar buscando en O(n), y no me cuesta más que memoria
 * mantener le vector. */
static rrNodePtr process[MAX_PROCS];

/** Array estatico de colas bloqueadas*/
static rrQueue blockedArr[REASON_COUNT];

/** Flag para eficiencia de elegido. lHPA = last Higher Priority Added*/
static int lHPA = INVALID;

/** No es constante porque planeaba hacerlo cambiar en el tiempo*/
static int rrQuantum = 3;

/** El nodo corriendo en este momento*/
static rrNodePtr current;

static char *reasonNames[] = {"Keyboard", "No_Reason", "Mutex", "Message"}; // lo puse aca porque en el .h me tiraba problemas de include

/**
 * siempre vamos a asumir que scheduler nos pasa
 * referencias no nulas y validas
 * */
boolean rrInit(pcbPtr pcbPtr)
{
    simple_printf("rrInit: init\n");
    resetAllQueues();
    current = normalQueueAddTail(pcbPtr, &(runningQueques[pcbPtr->priority]));

    simple_printf("rrInit: inicializamos pid=%d, name=%s priority=%d\n", pcbPtr->pid, pcbPtr->name, pcbPtr->priority);
    printRRQueues();
    return TRUE;
}

boolean rrAddProcess(pcbPtr pcbPtr)
{
    //simple_printf("rrAddProcess: adding. vamos a imprimir despues de agregar a proc1=%s\n", pcbPtr->name);
    rrNodePtr newNode = createNewNode(pcbPtr);
    if(!newNode)
        return FALSE;

    return changeToRespectiveQueue(newNode);
}

/** Dado que RR nunca edita una priority, no chequeamos que es
 * válida, asumimos que lo es
 */
static void refreshLHPA(int priority)
{
    lHPA = priority > lHPA? priority: lHPA;
}

/** Primero cheuqemos si el current todavia tiene tiempo, si tiene lo retornamos
 * si no tiene tiempo lo metemos donde debe y buscamos otro, si no hay nada retornamos null
 */
pcbPtr rrNextAvailableProcess()
{
    //simple_printf("rrNextAvailableProcess: entrando\n");
    rrNodePtr ret = handleCurrent();
    // si no usamos current buscamos otro
    if(!ret)
    {
        //simple_printf("rrNextAvailableProcess: NOT current\n");
        ret = getNextReadyNode();
    }
    //simple_printf("rrNextAvailableProcess: returning node=%d name=%s\n",ret, ret->pcbPtr->name);
    current = ret;
    if(ret == NULL) // no es un error
    {
        //simple_printf("rrNextAvailableProcess: ret == NULL\n");
        return NULL;
    }
    directSetProcessState(ret->pcbPtr->pid, RUNNING, NO_REASON);
    return ret->pcbPtr;
}

/** Retorna el siguiente nodo a correr,
 * Sería muy erroneo que al recorrer encontremos uno bloqueado.
 * Es normal encontrar uno muerto, simplemente lo sacamos de nuestras
 * estructuras y liberamos el nodo, pero no liberamos el pcbPtr, de eso
 * se encarga process.c*/
static rrNodePtr getNextReadyNode()
{
    rrNodePtr resp;
    rrQueue *pQueue = getNextQueue();
    if(!pQueue) // estan todas vacias
    {
        //simple_printf("getNextReadyNode: no hay queues con datos\n");
        return NULL;
    }
    // Popeamos el primer nodo FCFS
    resp = normalQueuePop(pQueue);
    while (resp)
    {
        if(resp->pcbPtr->state >= BLOCKED)  // no debería pasar que haya uno blocked... dead si
        {
            changeToRespectiveQueue(resp);
            resp = normalQueuePop(pQueue);
        }
        else
            return resp;
    }
    // si llegamos aca, resp == NULL, pero no terminamos
    // puede pasar que en esta queue esten todos muertos...
    // tenemos que pasar a otra queue hasta encontrar algo o retornar null.
    simple_printf("getNextReadyNode: entramos en recursion, imprimo esto por miedo a recursion infinita\n");
    return getNextReadyNode();
}

/** Elige si seguimos corriendo el current o si lo pasamos a otra estructura*/
static rrNodePtr handleCurrent()
{
    if(current != NULL) // si, current puede ser null
    {
        // Caso BLOKED/DEAD no es un error. puede pasar:
        // 1) lo bloqeuan/matan 2) se llama a rr pero era el current
        if(current->pcbPtr->state >= BLOCKED) // todo borrar este caso
        {
            //simple_printf("handleCurrent: Current blocked/dead\n");
            changeToRespectiveQueue(current);
        }
        else
        {
            // todavia le queda para correr y no está bloqueado
            if (current->quantum < rrQuantum) // no deberia pasar que esté bloqued?
            {
                current->quantum++;
                return current;
            }
            // Ya no puede correr más -> lo pasamos a la estructura de los terminados
            if(current->quantum >= rrQuantum)
            {
                changeToRespectiveStructure(current, finishedQueques);
            }
        }
    }
    return NULL;
}

/** Hacemos el cambio entre puntero de estructuras de running y de finished*/
static void switchStructures()
{
    rrQueuePtr aux = runningQueques;
    runningQueques = finishedQueques;
    finishedQueques = aux;
    lHPA = INVALID; // porque nos fuimos pal otro lado, ya no agregamos más aca
}

/** Mete un nodo en la estructura que se le pasa, en su respectiva cola,
 * según su nivel de prioridad
 */
static void changeToRespectiveStructure(rrNodePtr pNode, rrQueuePtr qsVector)
{
    normalQueueAddNodeTail(pNode,&(qsVector[pNode->pcbPtr->priority]));
}

/** Elige detro de la estructura de running la siguiente cola a utilizar*/
static rrQueue *getNextQueueAux()
{
    if(lHPA != INVALID && !rrIsEmpty(&(runningQueques[lHPA])))
        return &(runningQueques[lHPA]);
    for (int i = 0; i < PRIORITY_LEVELS; ++i) {
        if(!rrIsEmpty(&(runningQueques[i])))
            return &(runningQueques[i]);
    }
    return NULL;
}

/** Elige detro de la estructura de running la siguiente cola a utilizar
 * Si running no tiene niguna queue, hace un switch entre estructuras para
 * levantar emepezar a levantar de la otra estructura. Si esta tampoco
 * tiene ninguna queue retorna null
 */
static struct rrQueue* getNextQueue()
{
    struct rrQueue* pQueue = getNextQueueAux();
    if(!pQueue)
    {
        switchStructures();
        pQueue = getNextQueueAux();
        if(!pQueue) // en la otra cola tampoco habia nada
            return NULL;
    }
    return pQueue;
}

/** Se encarga de mover los procesos entre sus colas */
static boolean changeToRespectiveQueue(rrNodePtr node)
{
    //simple_printf("changeToRespectiveQueue(): entrando ************************************** %s\n", node->pcbPtr->name);
    //node->pcbPtr->state representa el la cola a la cual lo voy a pasar
    switch (node->pcbPtr->state)
    {
        case BORN:
        case READY:
        case RUNNING:
            return normalQueueAddNodeTail(node, &(runningQueques[node->pcbPtr->priority]));
        case BLOCKED:
            return funcionAuxiliar(node);
        case DEAD:
            //simple_printf("changeToRespectiveQueue(): Error: case DEAD:\n");
            removeNode(node);
            return TRUE;
    }
    return TRUE;
}

/**
 * Esta función es una patch de último minuto
 * que resuelve un problema de comunicacíon que hubo
 * entre los developers de RR(yo) y el de IPCs.
 * Yo pense que YO me encargaba de TODOS los procesos bloqueados.
 * Al final no, por ende no los guardo.
 *  // todo mejorar para TP3
 */
static boolean funcionAuxiliar(rrNodePtr node)
{
    //simple_printf("funcionAuxiliar: entrando !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    if(node->pcbPtr->blockedReason == MUTEX_BLOCK || node->pcbPtr->blockedReason == MESSAGE_PASSING)
    {
        removeNode(node);
    }
    else
        normalQueueAddNodeTail(node, &blockedArr[node->pcbPtr->blockedReason]);
    return TRUE;
}

/**
 * Hay que acordarnos que esta funcion solo debería
 * ser corrida cuando ocurre una interrupcion
 * por ende no será cortada a mitad de corrida.
 * El usuario(dev) podria simplemente llamar a setProcessState,
 * pero obligaríamos a este a mantener un registro del proceso que bloqueó.
 * Si el lo desea puede usar setProcessState, esto es solo una simplificación.
 * De esta manera solo con llamar a unBlock se desbloquea
 * al que le toca.
 * Unlock no llama a setProcessState para no perder tiempo con chequeos.
 * Llama a direct directSetProcessState para modularizar esa parte.
 */
boolean rrUnblockWaiters(int reason)
{
    //simple_printf("rrUnblockWaiters: reason=%d. Imprimimos antes de despertar\n", reason);
    //printRRQueues();

    if (!validReason(reason))
    {
        simple_printf("rrUnblockWaiters: invalid Reason\n");
        return FALSE;
    }
    rrNodePtr node = normalQueuePop(&blockedArr[reason]);
    if (node == NULL)
    {
        //simple_printf("rrUnblockWaiters: NO habia nadie a despertar\n");
        return FALSE;
    }
    directSetProcessState(node->pcbPtr->pid, READY, NO_REASON);
    changeToRespectiveQueue(node);
    return TRUE;
}

/** Patch para que externamente nos avizen si un proceso cambio de estado y
 * de esa forma nosotros poder cambirlo en nuestras estructuras
 */
void rrNotifyProcessStateChange(pPid pid)
{
    //simple_printf("rrNotifyProcessStateChange: entrando -------------------------- name=%s state=%d\n", process[pid]->pcbPtr->name, process[pid]->pcbPtr->state);
    if(process[pid] == NULL)
    {
        // lo creamos y se agrega a process
        if(!createNewNode(getPcbPtr(pid)))
            return;
    }
    else // si ya está lo sacamos
        normalQueueRemoveNode(process[pid]);
    //simple_printf("rrNotifyProcessStateChange: vamos a cambiar %s\n",process[pid]->pcbPtr->name);
    changeToRespectiveQueue(process[pid]);
    // si es el current tengo que tomar una decicion
    if(process[pid] == current)
    {
        //simple_printf("rrNotifyProcessStateChange: Igual a current\n");
        current = NULL;
    }
    //printRRQueues();
}

/** Se nos notifica si un proceso cambio de prioridad para
 * moverlo dentro de nuestras estructuras
 */
void rrNotifyProcessPriorityChange(pPid pid)
{
    //si no lo tenemos en nuestras estructuras no hacemos nada
    if(process[pid] == NULL)
        return;

    // si es el que se está corriendo, no hacemos nada,
    // cuando deje de corrar será enviado a donde debe.
    // si está bloqueado/dead tampoco hacemos nada,
    // por que ahi es FIFO y no perdes tu lugar.
    if(current != process[pid] && process[pid]->pcbPtr->state < BLOCKED)
    {
        normalQueueRemoveNode(process[pid]);
        changeToRespectiveQueue(process[pid]);
    }
}


/**************************/
/** Métodos de las queues */
/**************************/
static void resetAllQueues()
{
    for (int i = 0; i < PRIORITY_LEVELS; ++i) {
        resetQueue(&(runningQueques[i]));
        resetQueue(&(finishedQueques[i]));
    }
}

static boolean rrIsEmpty(rrQueue *pQueue)
{
    return pQueue->head.next == &pQueue->tail || pQueue->head.next == NULL || pQueue->tail.prev == NULL;
}

void printRRQueues()
{
    simple_printf("Current process = %s \n", current != NULL ? current->pcbPtr->name : "NONE" );
    simple_printf("To Run queues: \n");
    printStructure(runningQueques);
    simple_printf("Finished queues: \n");
    printStructure(finishedQueques);
    simple_printf("Bloqued queues: \n");
    for (int i = 0; i < REASON_COUNT; ++i)
    {
        simple_printf("    %s blocked queue = ", reasonNames[i]);
        printNormalQueue(blockedArr + i);
    }
}

static void printStructure(rrQueue *pQueue)
{
    for (int i = 0; i < PRIORITY_LEVELS; ++i) {
        simple_printf("    %d priority level = ", i);
        printNormalQueue(pQueue + i);
    }
}

static void printNormalQueue(rrQueue *queue)
{
    if (rrIsEmpty(queue))
    {
        simple_printf("empty\n");
        return;
    }
    rrNodePtr aux = queue->head.next;
    do
    {
        simple_printf("->%s ",aux->pcbPtr->name);
        aux = aux->next;
    } while (aux != NULL && aux != &queue->tail);
    simple_printf("\n");
}

/** Crea un nodo y lo agrega a la cola*/
static rrNodePtr normalQueueAddTail(pcbPtr pcbPtr, rrQueue *pQueue)
{
    rrNodePtr newNode = createNewNode(pcbPtr);
    if (!newNode)
        return false;

    normalQueueAddNodeTail(newNode, pQueue);
    return newNode;

}

/** Agrega un nodo ya existente a la cola*/
static boolean normalQueueAddNodeTail(rrNodePtr newNode, rrQueue *pQueue)
{
    if(rrIsEmpty(pQueue))
    {
        pQueue->head.next = pQueue->tail.prev = newNode;
        newNode->prev = &pQueue->head;
        newNode->next = &pQueue->tail;
    }
    else
    {
        newNode->prev = pQueue->tail.prev;
        pQueue->tail.prev->next = newNode;
        pQueue->tail.prev = newNode;
        newNode->next = &pQueue->tail;
    }
    refreshLHPA(newNode->pcbPtr->priority);
    return TRUE;
}

static rrNodePtr createNewNode(pcbPtr pcbPtr)
{
    if(process[pcbPtr->pid] != NULL)
    {
        simple_printf("createNewNode ERROR: ya existe este nodo\n");
        return NULL;
    }
    rrNodePtr ret = kernelMalloc(sizeof(rrNode));
    if (!ret)
        return NULL;

    ret->pcbPtr = pcbPtr;
    process[pcbPtr->pid] = ret;
    ret->quantum = 0;
    return ret;
}

static pcbPtr resetQueue(rrQueue *queue)
{
    queue->head.next = NULL;
    queue->tail.prev = NULL;
    return NULL;
}

static rrNodePtr normalQueuePop(rrQueue *queue)
{
    if(rrIsEmpty(queue))
        return NULL;
    rrNodePtr node = queue->head.next;
    if (node) // todo chequear si es posible que sea null
    {
        queue->head.next = node->next;
        node->next->prev = &queue->head;
        node->next = NULL;
        node->prev = NULL;
        // si era el ultimo
        if (rrIsEmpty(queue))
            resetQueue(queue);
    }
    return node; // si es NULL lo retorna tambien
}

static void normalQueueRemoveNode(rrNodePtr node)
{
    // chueqear esto porque posta no puede ser tan simple.
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

static void removeNode(rrNodePtr node)
{
    process[node->pcbPtr->pid] = NULL;
    kernelFree(node);
}

