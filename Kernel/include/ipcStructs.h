#ifndef IPCSTRUCTS_H
#define IPCSTRUCTS_H

#include <stdint.h>
#include <queue.h>

typedef int pPid;

typedef struct pipe_t {

    int pipeId;
    char buffer[PIPEBUFFERSIZE];
    int bufferReadPosition;
    int bufferWritePosition;
    int charsToRead;
    int mutex;
    int readMutex;
    int writeMutex;

} pipe_t;

typedef struct message_t
{
    uint64_t id;
    pPid pidSender;
    pPid pidReceiver;
    char * content;
} msg_t;

typedef struct mutex_t
{
    int value;
    Queue * nextProcessInLine;
}mutex_t;

typedef struct messageNode
{
    msg_t message;
    struct messageNode * next;
} messageNode;

typedef struct messageQueue
{
    int count;
    messageNode * first;
    messageNode * last;
} messageQueue;

typedef mutex_t sem_t ;

/**
 * Metodos y structs para la cola de mensajes
 */

messageQueue * createNewMessageQueue();
int isEmptyMessageQueue(messageQueue* queue);
void enqueueMessage(messageQueue* queue, msg_t message);
void dequeueMessage(messageQueue* queue, msg_t * message);

/**
 * impresion
 */

void printIpcsQueues();

/**
 * Pipes
 */
pipe_t * getPipeFromPipeList(int id);
pipe_t * createPipeK();
void addStandardPipes(pPid pid);
int addPipeProcess();
#endif
