#ifndef IPCSTRUCTS_H
#define IPCSTRUCTS_H

#include <stdint.h>
#include "queue.h"

typedef int pPid;

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

#endif
