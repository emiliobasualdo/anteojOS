#ifndef ROUNDROBIN_H
#define ROUNDROBIN_H

#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <system.h>
#include <dinamicMemory.h>

#define INVALID -1

typedef struct rrNode *rrNodePtr;

typedef struct rrNode{
    pcbPtr pcbPtr;
    rrNodePtr next;
    rrNodePtr prev;
    unsigned long quantum;
}rrNode;

typedef struct rrQueue{
    rrNode head;
    rrNode tail;
}rrQueue;

typedef rrQueue *rrQueuePtr;

boolean rrInit(pcbPtr pcbPtr);
boolean rrAddProcess(pcbPtr pcbPtr);
pcbPtr rrNextAvailableProcess();
boolean rrUnblockWaiters(int reason);
void printRRQueues();
void rrNotifyProcessStateChange(pPid pid);
void rrNotifyProcessPriorityChange(pPid pid);

#endif
