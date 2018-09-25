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

int initIPCS()
{
    int i;
    for(i = 0; i < MAXMUTEXES; i++)
    {
        mutexList[i].value = -1;
        mutexList[i].nextProcessInLine = *(createQueue(MAXINQUEUE));
    }
    messageMutex.value = 0;
    messageMutex.nextProcessInLine = *(createQueue(MAXINQUEUE));
    return TRUE;
}


/**
 * le doy el ID de su mutex
 * Si recibo -1, significa que no me lo pude "crear"
 */
int startMutex()
{
    if(positionMutexArray >= MAXMUTEXES)
    {
        int i;
        for(i = 0; i < MAXMUTEXES; i++)
        {
            if(mutexList[positionMutexArray].value == -1)
            {
                mutexList[positionMutexArray].value = 0;
                return i;
            }
        }
        return -1;
    }
    else
    {
        mutexList[positionMutexArray].value = 0;
        positionMutexArray++;
        return positionMutexArray - 1;
    }
}



int lockMutex(int mutex)
{
    if (mutex < 0 || mutex > positionMutexArray)
    {
        return -1;
    }
    if(lockMutexASM(&(mutexList[mutex].value)))
    {
        if(!isFull(&(mutexList[mutex].nextProcessInLine)))
        {
            pPid process = getCurrentProc()->pid;
            enqueue(&(mutexList[mutex].nextProcessInLine), (int)process);

            setProcessState(process, BLOCKED, MUTEX_BLOCK);

        }
    }
    return 0;
}

int unlockMutex(int mutex)
{
    if (mutex < 0 || mutex > positionMutexArray)
    {
        return -1;
    }
    else
    {
        if(!isEmpty(&(mutexList[mutex].nextProcessInLine)))
        {
            pPid process = dequeue(&(mutexList[mutex].nextProcessInLine));

            setProcessState(process, READY, MUTEX_BLOCK);

            schedulerAddProcPid(process);

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
        if(!isFull(&(messageMutex.nextProcessInLine)))
        {
            pPid process = getCurrentProc()->pid;
            enqueue(&(messageMutex.nextProcessInLine), (int)process);
            setProcessState(process, BLOCKED, MESSAGE_PASSING);
        }
        return 1;
    }
    return 0;
}

static int messageMutexUnlock()
{
    pPid process = dequeue(&(messageMutex.nextProcessInLine));
    if(process != EMPTY_QUEUE)
    {
        setProcessState(process, READY, MESSAGE_PASSING);
        schedulerAddProcPid(process);
        return 1;
    }else
    {
        unlockMutexASM(&(messageMutex.value));
    }
    return 0;
}



int destroyMutexK(int mutex)
{
    if(mutex < 0 || !isEmpty(&(mutexList[mutex].nextProcessInLine)))
    {
        return -1;
    }
    mutexList[mutex].value = -1;
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
    messageQueue * queue = my_malloc(sizeof(messageQueue));
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
    message.pidReceiver = pidReceiver;
    int length = strlen(messageBody);
    message.content = my_malloc(length+1);
    memcpy(message.content, messageBody,strlen(messageBody));
    message.content[length] = 0;
    message.pidSender = pidSender;
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
    messageNode * mNode = my_malloc(sizeof(messageNode));
    int length = strlen(message.content);
    mNode->message.content = my_malloc(length+1);
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
        message->content = my_malloc(strlen(node->message.content)+1);
        memcpy(message->content, node->message.content, strlen(node->message.content));
        message->content[length] = 0;
        queue->first = queue->first->next;
        if(queue->count == 0)
        {
            queue->first = NULL;
            queue->last = NULL;
        }
        my_free(node);
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
    int processNumber = 0;
    messageMutexLock();
    while(processNumber < MAX_PROCS && endWhile)
    {
        pcbPtr aux = getPcbPtr(processNumber);
        if (aux->pid == to)
        {
            createNewMessage(getPcbPtr(to)->postBox, from, to, body);

            if(getPcbPtr(processNumber)->postBox->count == 1) //desbloqueo el proceso pq tengo un msj
            {
                setProcessState(to, READY, MESSAGE_PASSING);
                schedulerAddProcPid(to);
            }
            endWhile = 0;
        }
        processNumber++;
    }
    //printPostBox(to);
    messageMutexUnlock();
    return;
}

static msg_t * syncSend(pPid from, pPid to, char * body)
{
    asyncSend(from, to, body);
    msg_t * msg = NULL;
    asyncReceive(msg);
    return msg;
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
uint64_t sendMessage(pPid receiver, char * content, char * answer, boolean flag)
{
    pPid pid = getCurrentProc()->pid;
    if(!flag || answer == NULL)
    {
        asyncSend(pid, receiver, content);
        return 0;
    }
    msg_t * msg = syncSend(pid, receiver, content);
    answer = msg->content;
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
    *message = my_malloc(length+1);
    memcpy((*message), aux.content, length);
    (*message)[length] = 0;
    return 0;
}
