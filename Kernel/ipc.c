#include <stdbool.h>
#include "ipc.h"

/**
 * TODO checkear loops, tres procesos esperando mensajes
 * TODO meter un timeout: tiempo para ver si se recibe o no el mensaje
 */

/**
 * MUTEX
 * La primera posicion del array de mutexes es el bloque del scheduler
 */
static mutex_t mutexList[MAXMUTEXES];
int positionMutexArray = 0;

static mutex_t messageMutex;

static pPid blockedByReceive[MAXINQUEUE];
static int numberBlockedReceiveArray = 0;

static sem_t semList[MAXSEMAPHORES];
int positionSemArray = 0;
static mutex_t semMutex;

int initIPCS()
{
    int i;
    for(i = 0; i < MAXMUTEXES; i++)
    {
        mutexList[i].value = -1;
        mutexList[i].nextProcessInLine = NULL;
        semList[i].value = 0;
        semList[i].nextProcessInLine = NULL;
    }
    semMutex.value = 0;
    semMutex.nextProcessInLine = createQueue(MAXINQUEUE);
    messageMutex.value = 0;
    messageMutex.nextProcessInLine = createQueue(MAXINQUEUE);
    for(i = 0;i < MAXINQUEUE;i++)
    {
        blockedByReceive[i] = 0;
    }
    return TRUE;
}

static void changeBlockedReceiveArray(pPid pid, int state)
{
    int i = 0;
    if(state)
    {
        numberBlockedReceiveArray--;
        while(blockedByReceive[i] != pid)
        {
            i++;
        }
        blockedByReceive[i] = 0;
    }
    else
    {
        numberBlockedReceiveArray++;
        while(blockedByReceive[i] == 0)
        {
            i++;
        }
        blockedByReceive[i] = pid;
    }
}

static void printMutexList(int mutex)
{
    if(mutex < 0 || mutex > positionMutexArray || mutexList[mutex].value == -1)
    {
        return;
    }
    simple_printf("Mutex %d Queue = ", mutex);
    int i;
    for(i = 0; i < mutexList[mutex].nextProcessInLine->size; i++)
    {
        simple_printf("-> %d ",mutexList[mutex].nextProcessInLine->array[(mutexList[mutex].nextProcessInLine->front+i)%MAXINQUEUE]);
    }
    simple_printf("\n");
}

static void printSemList(int sem)
{
    if (sem < 0 || sem > positionSemArray || semList[sem].nextProcessInLine == NULL)
    {
        return;
    }
    int i;
    for(i = 0; i < semList[sem].nextProcessInLine->size; i++)
    {
        simple_printf("-> %d ",semList[sem].nextProcessInLine->array[(semList[sem].nextProcessInLine->front+i)%MAXINQUEUE]);
    }
    simple_printf("processses in list: %d\n", semList[sem].nextProcessInLine->size);

}

void printIpcsQueues()
{
    int i;
    for(i = 0; i < positionMutexArray; i++)
    {
        if(mutexList[i].value == 1 || mutexList[i].value == 0)
        {
            printMutexList(i);
        }
    }

    simple_printf("Message Mutex Queue = ");
    for(i = 0; i <= messageMutex.nextProcessInLine->size; i++)
    {
        simple_printf("-> %d ",messageMutex.nextProcessInLine->array[(messageMutex.nextProcessInLine->front+i)%MAXINQUEUE]);
    }
    simple_printf("\n");

    simple_printf("Sem Mutex Queue = ");
    for(i = 0; i <= semMutex.nextProcessInLine->size; i++)
    {
        simple_printf("-> %d ",semMutex.nextProcessInLine->array[(semMutex.nextProcessInLine->front+i)%MAXINQUEUE]);
    }
    simple_printf("\n");

    simple_printf("Received Bloqued List = ");
    int aux = numberBlockedReceiveArray;
    for(i = 0; i < MAXINQUEUE && aux; i++)
    {
        if(blockedByReceive[i] != 0)
        {
            simple_printf("-> %d ",blockedByReceive[i]);
            aux--;
        }
    }
    simple_printf("\n");
}
/**
 * MUTEX
 */

/**
 * le doy el ID de su mutex
 * Si recibo -1, significa que no me lo pude "crear"
 */
int startMutex(int initValue)
{
    if(initValue != 1 && initValue != 0)
    {
        return -1;
    }
    if(positionMutexArray >= MAXMUTEXES)
    {
        int i;
        for(i = 0; i < MAXMUTEXES; i++)
        {
            if(mutexList[positionMutexArray].value == -1)
            {
                mutexList[positionMutexArray].value = initValue;
                mutexList[positionMutexArray].nextProcessInLine = createQueue(MAXINQUEUE);
                return i;
            }
        }
        return -1;
    }
    else
    {
        mutexList[positionMutexArray].value = initValue;
        mutexList[positionMutexArray].nextProcessInLine = createQueue(MAXINQUEUE);
        positionMutexArray++;
        return positionMutexArray - 1;
    }
}
//equivalente a lock pero no bloquea al proceso(sirve para los fd)
int tryToLockMutex(int mutex)
{
    pPid process = getCurrentProc()->pid;
    if (mutex < 0 || mutex > positionMutexArray || mutexList[mutex].value == -1)
    {
        return -1;
    }
    return lockMutexASM(&(mutexList[mutex].value));
}

int lockMutex(int mutex)
{
    pPid process = getCurrentProc()->pid;
    if (mutex < 0 || mutex > positionMutexArray || mutexList[mutex].value == -1)
    {
        return -1;
    }
    if(lockMutexASM(&(mutexList[mutex].value)))
    {
        if(!isFull(mutexList[mutex].nextProcessInLine))
        {
            enqueue(mutexList[mutex].nextProcessInLine, process);

            setProcessState(process, BLOCKED, MUTEX_BLOCK);

        }
    }
    return 0;
}

int unlockMutex(int mutex)
{
    if (mutex < 0 || mutex > positionMutexArray || mutexList[mutex].value == -1)
    {
        return -1;
    }
    else
    {
        if(!isEmpty(mutexList[mutex].nextProcessInLine))
        {
            pPid process = dequeue(mutexList[mutex].nextProcessInLine);

            setProcessState(process, READY, MUTEX_BLOCK);

            setProcessPriority(process, MAX_PRIORITY);
            //printMutexList(mutex);
        }
        else
        {
            unlockMutexASM(&(mutexList[mutex].value));
        }
    }
    return 0;
}
/**
 * retorna 1 si ya estaba bloqueado y 0 si no
 */
static int messageMutexLock()
{
    if(lockMutexASM(&(messageMutex.value)))
    {
        if(!isFull(messageMutex.nextProcessInLine))
        {
            pPid process = getCurrentProc()->pid;
            enqueue(messageMutex.nextProcessInLine, (int)process);
            setProcessState(process, BLOCKED, MESSAGE_PASSING);
        }
        return 1;
    }
    return 0;
}

static int messageMutexUnlock()
{
    pPid process = dequeue(messageMutex.nextProcessInLine);
    if(process != EMPTY_QUEUE)
    {
        setProcessState(process, READY, MESSAGE_PASSING);
        return 1;
    }else
    {
        unlockMutexASM(&(messageMutex.value));
    }
    return 0;
}

static int semMutexLock()
{
    if(lockMutexASM(&(semMutex.value)))
    {
        if(!isFull(semMutex.nextProcessInLine))
        {
            pPid process = getCurrentProc()->pid;
            enqueue(semMutex.nextProcessInLine, (int)process);
            setProcessState(process, BLOCKED, MESSAGE_PASSING);
        }
        return 1;
    }
    return 0;
}

static int semMutexUnlock()
{
    pPid process = dequeue(semMutex.nextProcessInLine);
    if(process != EMPTY_QUEUE)
    {
        setProcessState(process, READY, MESSAGE_PASSING);
        return 1;
    }else
    {
        unlockMutexASM(&(semMutex.value));
    }
    return 0;
}


int destroyMutexK(int mutex)
{
    if(mutex < 0 || mutexList[mutex].value == -1 || mutexList[mutex].value == 1)
    {
        return -1;
    }
    kernelFree(mutexList[mutex].nextProcessInLine);
    mutexList[mutex].value = -1;
    return 1;
}

/**
 * SEMAFOROS
 */

int semStartK(int initValue)
{
    if(positionMutexArray >= MAXSEMAPHORES)
    {
        int i;
        for(i = 0; i < MAXSEMAPHORES; i++)
        {
            if(semList[positionSemArray].nextProcessInLine == NULL)
            {
                semList[positionSemArray].value = initValue;
                semList[positionSemArray].nextProcessInLine = createQueue(MAXINQUEUE);
                return i;
            }
        }
        return -1;
    }
    else
    {
        semList[positionSemArray].value = initValue;
        semList[positionSemArray].nextProcessInLine = createQueue(MAXINQUEUE);
        positionSemArray++;
        return positionSemArray - 1;
    }
}

int semWaitK(int sem)
{
    pPid process = getCurrentProc()->pid;
    if (sem < 0 || sem > positionSemArray || semList[sem].nextProcessInLine == NULL)
    {
        return -1;
    }

    semMutexLock();


    if(semList[sem].value>=0)
    {
        semList[sem].value--;
        semMutexUnlock();
        return 0;
    }
    if(!isFull(semList[sem].nextProcessInLine))
    {
        enqueue(semList[sem].nextProcessInLine, process);
        setProcessState(process, BLOCKED, MUTEX_BLOCK);
    }
    semMutexUnlock();
    return 0;
}

int semPostK(int sem)
{
    if (sem < 0 || sem > positionSemArray || semList[sem].nextProcessInLine == NULL) {
        return -1;
    }
    semMutexLock();

    if(isEmpty(semList[sem].nextProcessInLine))
    {
        semList[sem].value++;
    }
    else
    {
        pPid process = dequeue(semList[sem].nextProcessInLine);

        setProcessState(process, READY, MUTEX_BLOCK);
        setProcessPriority(process, MAX_PRIORITY);
    }
    semMutexUnlock();
    return 0;
}

int semDestroyK(int sem)
{
    if(sem < 0 || semList[sem].nextProcessInLine == NULL || !isEmpty(semList[sem].nextProcessInLine))
    {
        return -1;
    }
    kernelFree(semList[sem].nextProcessInLine);
    semList[sem].nextProcessInLine = NULL;
    return 1;
}

/**
 * MENSAJES
 */

static uint64_t id = 0;

static boolean messageAlreadyInQueue(messageQueue *queue, msg_t message);
static boolean sameMessage(msg_t message, msg_t message1);
static void asyncReceive(msg_t * message);

messageQueue * createNewMessageQueue()
{
    messageQueue * queue = kernelMalloc(sizeof(messageQueue));
    queue->first = NULL;
    queue->last = NULL;
    queue->count = 0;
    return queue;
}

/**
 * Recibe el contenido de un mensaje, lo crea y lo pushea al queue
 * @param queue
 * @param pidSender
 * @param pidReceiver
 * @param messageBody
 */
static void createNewMessage(messageQueue* queue, uint64_t pidSender, uint64_t pidReceiver, char * messageBody)
{
    msg_t message;
    message.pidReceiver = (int)pidReceiver;
    int length = strlen(messageBody);
    message.content = kernelMalloc(length+1);
    memcpy(message.content, messageBody,strlen(messageBody));
    message.content[length] = 0;
    message.pidSender = (int)pidSender;
    if(!messageAlreadyInQueue(queue, message))
    {
        message.id = id++;
        enqueueMessage(queue, message);
    }
}

static boolean sameMessage(msg_t msg1, msg_t msg2)
{
    return msg1.pidSender == msg2.pidSender && msg1.pidReceiver == msg2.pidReceiver && strcmp(msg1.content, msg2.content);
}

static boolean messageAlreadyInQueue(messageQueue * queue, msg_t message)
{
    boolean flag = 0;
    if(queue != NULL)
    {
        int queueSize = queue->count;
        messageNode *curr = queue->first;
        while(queueSize >1 && !flag)
        {
            if (sameMessage(message, curr->message))
            {
                flag = 1;
            }
            curr = curr->next;
            queueSize--;
        }
    }
    return flag;
}

int isEmptyMessageQueue(messageQueue* queue)
{
    return queue->count == 0;
}

void enqueueMessage(messageQueue* queue, msg_t message)
{
    if(queue == NULL)
    {
        return;
    }
    messageNode * mNode = kernelMalloc(sizeof(messageNode));
    int length = strlen(message.content);
    mNode->message.content = kernelMalloc(length+1);
    mNode->message.content[length] = 0;
    mNode->next = NULL;
    mNode->message.pidSender = message.pidSender;
    mNode->message.pidReceiver = message.pidReceiver;
    memcpy(mNode->message.content, message.content, length);
    if(isEmptyMessageQueue(queue))
    {
        queue->first = mNode;
    }
    else
    {
        queue->last->next = mNode;
    }
    queue->last = mNode;
    queue->count++;

    return;
}

void dequeueMessage(messageQueue* queue, msg_t * message)
{
    if(queue == NULL || isEmptyMessageQueue(queue))
    {
        return;
    }
    else
    {
        queue->count--;
        messageNode * node = queue->first;
        message->pidSender = node->message.pidSender;
        message->pidReceiver = node->message.pidReceiver;
        int length = strlen(node->message.content);
        message->content = kernelMalloc(strlen(node->message.content)+1);
        memcpy(message->content, node->message.content, strlen(node->message.content));
        message->content[length] = 0;
        queue->first = queue->first->next;
        if(queue->count == 0)
        {
            queue->first = NULL;
            queue->last = NULL;
        }
        kernelFree(node);
    }
}

void printPostBox(pPid pid)
{
    pcbPtr puntero = getPcbPtr(pid);
    int i;
    simple_printf("num msjs: %d. Content msg pid %d: ",puntero->postBox->count, pid);
    messageNode * nodo = puntero->postBox->first;
    for(i = 0;i < puntero->postBox->count;i++)
    {
        simple_printf("%d) %s ", i, nodo->message.content);
        nodo = nodo->next;
    }
    simple_printf("\n");
}


static void asyncSend(pPid from, pPid to, char * body)
{
    boolean endWhile = 1;
    int processNumber = 1;
    messageMutexLock();
    while(processNumber < MAX_PROCS && endWhile)
    {
        pcbPtr aux = getPcbPtr(processNumber);
        if (aux->pid == to)
        {
            createNewMessage(getPcbPtr(to)->postBox, from, to, body);

            if(aux->postBox->count == 1 && aux->state != DEAD) //desbloqueo el proceso pq tengo un msj
            {
                if(aux->state == BLOCKED && aux->blockedReason == MESSAGE_PASSING)
                {
                    changeBlockedReceiveArray(to, SND);
                    setProcessState(to, READY, MESSAGE_PASSING);
                }
            }
            endWhile = 0;
        }
        processNumber++;
    }
    //printPostBox(to);
    messageMutexUnlock();
    return;
}

static void syncSend(pPid from, pPid to, char * body, msg_t * answer)
{
    asyncSend(from, to, body);
    asyncReceive(answer);
    return;
}


/**
 * Retorno NULL si tengo algun problema
 */
static void asyncReceive(msg_t * message)
{
    pPid pid = getCurrentProc()->pid;
    pPid comparePid;

    boolean endWhile = 1;
    int processNumber = 1;
    while(processNumber < MAX_PROCS && endWhile)
    {
        if ((comparePid = getPcbPtr(processNumber)->pid) == pid)
        {
            if(isEmptyMessageQueue(getPcbPtr(processNumber)->postBox))
            {
                changeBlockedReceiveArray(pid, REC);
                setProcessState(pid, BLOCKED, MESSAGE_PASSING);
            }
            dequeueMessage(getPcbPtr(processNumber)->postBox, message);
            endWhile = 0;
        }
        processNumber++;
    }
    return;
}

static void syncReceive(msg_t * message, void (*function)(char*))
{
    asyncReceive(message);
    (*function)(message->content);
    asyncSend(message->pidReceiver, message->pidSender, message->content);
}

/*
 * si flag = 0 async
 * retorno NULL si async
 */
uint64_t sendMessage(pPid receiver, char * content, char ** answer, boolean flag)
{
    pPid pid = getCurrentProc()->pid;
    if(!flag || answer == NULL)
    {
        asyncSend(pid, receiver, content);
        return 1;
    }
    msg_t aux;
    syncSend(pid, receiver, content, &aux);
    int length = strlen(aux.content);
    *answer = kernelMalloc(length+1);
    memcpy((*answer),aux.content, length);
    (*answer)[length] = 0;
    return 0;
}

uint64_t receiveMessage(char ** message, void (*function)(char*), boolean flag)//dejo el receiver por si quiero recibir un mssj especifico de otro proceso
{
    msg_t aux;
    if(function == NULL && !flag)
    {
        syncReceive(&aux, function);
        return 0;
    }
    asyncReceive(&aux);
    int length = strlen(aux.content);
    *message = kernelMalloc(length+1);
    memcpy((*message), aux.content, length);
    (*message)[length] = 0;
    return 1;
}
