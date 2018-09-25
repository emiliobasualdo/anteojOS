//
// Created by Emilio Basualdo on 9/4/18.

#include <roundRobin.h>

static boolean rrIsEmpty(rrQueue *pQueue);
static boolean roundQueueAddTail(pcbPtr pcbPtr, rrQueue *roundQueue);
static boolean validReason(reasonT reason);
static void changeToRespectiveQueue(rrNodePtr node);
static void printRoundQueue(rrQueue *queue);
static void printNormalQueue(rrQueue *queue);
pcbPtr resetQueue(rrQueue *queue);
static boolean normalQueueAddNodeTail(rrNodePtr node, rrQueue *queue);
static boolean roundQueueAddNodeTail(rrNodePtr, rrQueue *);
void roundQueueRemoveNode(rrNodePtr prevNode, rrNodePtr curNode, rrQueue *roundQueue);
static boolean roundQueueAddNodeHead(rrNodePtr node, rrQueue *roundQueue);

rrNodePtr normalQueuePop(rrQueue *queue);

/** Array estatico de la cola circular de los que están corriendo*/
static rrQueue runningQueue;

/** Array estatico de colas bloqueadas*/
static rrQueue blockedArr[REASON_COUNT];

static unsigned long rrQuantum = 4;

static char *reasonNames[] = {"Keyboard", "No_Reason", "Mutex", "Message"}; // lo puse aca porque en el .h me tiraba problemas de include

/**
 * siempre vamos a asumir que scheduler nos pasa
 * referencias no nulas y validas
 * */
boolean rrInit(pcbPtr pcbPtr)
{
    simple_printf("rrInit: init\n");
    rrNodePtr newNode = my_malloc(sizeof(rrNode));

    runningQueue.tail = runningQueue.current = runningQueue.head = newNode;
    newNode->pcbPtr = pcbPtr;
    newNode->next = newNode;
    newNode->quantum = 0;
    runningQueue.count = 1 ;
    simple_printf("rrInit: inicializamos con current=head=tail=pid=%d, name=%s\n", pcbPtr->pid, pcbPtr->name);
    printRRQueues();
    return TRUE;
}

static boolean rrIsEmpty(rrQueue *pQueue)
{
    return pQueue->count == 0;
}

static boolean roundQueueAddTail(pcbPtr pcbPtr, rrQueue *roundQueue)
{
    rrNodePtr newNode = my_malloc(sizeof(rrNode));
    if (newNode == NULL)
    {
        simple_printf("roundQueueAddTail: ERROR: newNode == NULL\n");
        return FALSE;
    }
    //simple_printf("roundQueueAddTail: pcbPtr=%d, pcbName=%s, newNode=%d , roundQueue=%d, runningQueue=%d\n", pcbPtr, pcbPtr->name, newNode, roundQueue, &runningQueue);
    newNode->pcbPtr = pcbPtr;
    newNode->quantum = 0;
    return roundQueueAddNodeTail(newNode, roundQueue);
}

static boolean roundQueueAddNodeTail(rrNodePtr node, rrQueue *roundQueue)
{
    if (rrIsEmpty(roundQueue))
    {
        simple_printf("roundQueueAddNodeTail: pareciera estar vacia\n");
        resetQueue(roundQueue);
        roundQueue->head = roundQueue->tail = roundQueue->current = node;
    }
    roundQueue->tail->next = node;
    roundQueue->tail = node;
    node->next = roundQueue->current;
    roundQueue->count++;
    //simple_printf("\nroundQueueAddNodeTail: node->next=%d roundQueue=%d\n", node->next, roundQueue);
    //simple_printf("\nroundQueueAddNodeTail: tail=%d current=%d,tail->Next=%d  count=%d\n", roundQueue->tail, roundQueue->current, roundQueue->tail->next,  roundQueue->count);
    return TRUE;
}

static boolean roundQueueAddNodeHead(rrNodePtr node, rrQueue *roundQueue)
{
    if (rrIsEmpty(roundQueue))
    {
        //simple_printf("roundQueueAddNodeHead: rrEmpty\n");
        resetQueue(roundQueue);
        roundQueue->head = roundQueue->tail = roundQueue->current = node;
        roundQueue->head->next = roundQueue->head;
    }
    else
    {
        node->next = roundQueue->current->next;
        roundQueue->current->next = node;
    }
    roundQueue->count++;
    //simple_printf("roundQueueAddNodeHead: done.... imprimimos\n");
    //printRRQueues();
    return TRUE;
}

static boolean normalQueueAddNodeTail(rrNodePtr node, rrQueue *queue)
{
    node->next = NULL;
    if(rrIsEmpty(queue))
    {
        queue->head = queue->tail = node;
        queue->count = 1;
        return TRUE;
    }

    queue->count++;
    queue->tail->next = node;
    queue->tail = node;
    return TRUE;
}

void printRRQueues()
{
    simple_printf("Running queue  = ");
    printRoundQueue(&runningQueue);
    for (int i = 0; i < REASON_COUNT; ++i)
    {
        simple_printf("%s blocked queue = ", reasonNames[i]);
        printNormalQueue(blockedArr + i);
    }
}

/**
 * Hay distintas funciones porque alguna menjan el head y otras no
 */
static void printRoundQueue(rrQueue *queue)
{
    if (!queue->current)
    {
        simple_printf("empty\n");
        return;
    }
    rrNodePtr aux = queue->current;
    do
    {
        simple_printf("->%s ",aux->pcbPtr->name);
        aux = aux->next;
    } while (aux != queue->current);
    simple_printf("\n");
}

/**
 * Hay distintas funciones porque alguna menjan el head y otras no
 */
static void printNormalQueue(rrQueue *queue)
{
    if (!queue->head)
    {
        simple_printf("empty\n");
        return;
    }
    rrNodePtr aux = queue->head;
    do
    {
        simple_printf("->%s ",aux->pcbPtr->name);
        aux = aux->next;
    } while (aux != NULL);
    simple_printf("\n");
}

boolean rrAddProcess(pcbPtr pcbPtr)
{
    //simple_printf("rrAddProcess: adding. vamos a imprimir despues de agregar a proc=%s\n", pcbPtr->name);
    boolean resp = roundQueueAddTail(pcbPtr, &runningQueue);
    //printRRQueues();
    return resp;
}

/**
 * Levanta cualquiera que esté BORN,READY o RUNNING.
 * Los BORN, READY los pone en RUNNING.
 * Los BLOCKED los saltea
 * Los DEAD los saca y libera nuestro nodo, el pcbPtr no es mi responsabilidad
 * Si no hay otro retorna el current
 */
pcbPtr rrNextAvailableProcess()
{
    if(rrIsEmpty(&runningQueue))
    {
        //simple_printf("rrNextAvailableProcess: Running queue esta vacia\n");
        return NULL;
    }
    // si no cumplió su cuota que siga
    if(runningQueue.current->quantum < rrQuantum && runningQueue.current->pcbPtr->state < BLOCKED)
    {
        //simple_printf("rrNextAvailableProcess: adentro y SIGUE corriendo\n");
        runningQueue.current->quantum++;
        return runningQueue.current->pcbPtr;
    }
    else
    {
        //simple_printf("rrNextAvailableProcess: adentro y TERMINO de correr\n");
        runningQueue.current->quantum = 0;
    }
    // no son null por el rrIsEmpty()
    rrNodePtr prevNode = runningQueue.current; // el que venia corriendo hasta recien
    rrNodePtr curNode = runningQueue.current->next; // el que apunta el de recien

    do
    {
        switch (curNode->pcbPtr->state)
        {
            case BORN:
            case READY:// los pasamos al estado running y los retornamos
                setProcessState(curNode->pcbPtr->pid, RUNNING, NO_REASON);
            case RUNNING: // los retornamos
                runningQueue.tail = prevNode;
                runningQueue.current = curNode;
                return runningQueue.current->pcbPtr;
            case BLOCKED: // lo mandamos a donde debería estar y lo sacamos de la lista
            case DEAD: // lo scamos de la lista
                roundQueueRemoveNode(prevNode, curNode, &runningQueue);
                changeToRespectiveQueue(curNode);

                if (runningQueue.count == 0)
                    curNode = NULL;
                else // prevNode no es updateado porque sigue siendo el prev
                    curNode = prevNode->next;

                break;
            default: // que se yo, por las dudas, que se joda
                roundQueueRemoveNode(prevNode, curNode, &runningQueue);
                break;
        }
    } while (curNode != NULL);
    //simple_printf("Returning NULL\n");
    return NULL;
}

/**
 * Lo saca pero no lo libera
 */
void roundQueueRemoveNode(rrNodePtr prevNode, rrNodePtr curNode, rrQueue *roundQueue) {

    if (roundQueue->count == 0)
        return;

    if (roundQueue->count == 1)
    {
        resetQueue(roundQueue);
        return;
    }

    prevNode->next = curNode->next;

    if (roundQueue->head == curNode)
        roundQueue->head = curNode->next;

    if (roundQueue->current == curNode)
        roundQueue->current = curNode->next;

    if (roundQueue->tail == curNode)
        roundQueue->tail = prevNode;

    roundQueue->count--;

}

pcbPtr resetQueue(rrQueue *queue) {
    queue->head = NULL;
    queue->current = NULL;
    queue->tail = NULL;
    queue->count = 0;
    return NULL;
}

/**
 * Se encarga de mover los procesos entre sus colas
 * Por ahora le vamos a dar prioridad a los porcesos que
 * estuvieron bloqueados si mucha pregunta*/
static void changeToRespectiveQueue(rrNodePtr node)
{
    //node->pcbPtr->state representa el la cola a la cual lo voy a pasar
    switch (node->pcbPtr->state)
    {
        case BORN:
        case READY:
        case RUNNING:
            roundQueueAddNodeHead(node, &runningQueue);
            break;
        case BLOCKED:
            normalQueueAddNodeTail(node, &blockedArr[node->pcbPtr->blockedReason]);
            break;
        case DEAD:
            my_free(node);
            break;
    }
}

/**
 * Hay que acordarnos que esta funcion solo debería
 * ser corrida cuando ocurre una interrupcion
 * por ende no será cortada a mitad de corrida
 */
boolean rrUnblockWaiters(reasonT reason)
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
    setProcessState(node->pcbPtr->pid, READY, NO_REASON);
    changeToRespectiveQueue(node);
    return TRUE;
}

rrNodePtr normalQueuePop(rrQueue *queue) {
    rrNodePtr node = queue->head;
    if (node)
    {
        queue->head = node->next;
        node->next = NULL;
        queue->count--;
        // si era el ultimo
        if (queue->head == NULL || queue->count == 0)
            resetQueue(queue);
    }
    return node; // si es NULL lo retorna tambien
}

static boolean validReason(reasonT reason)
{
    return reason >= 0 && reason < REASON_COUNT;
}
