#ifndef ROUNDROBIN_H
#define ROUNDROBIN_H

#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <system.h>
#include <dinamicMemory.h>
#include <timer.h>

#define MAX_TURNS_PER_PRIORITY 5

typedef struct rrNode *rrNodePtr;

typedef struct rrNode{
    pcbPtr pcbPtr;
    rrNodePtr next;
    rrNodePtr prev;
    int quantum;
    unsigned long long rrTurns; // Cantidadd de veces que estuvo en procesador
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
unsigned long long getRRRunTime(pPid pid);
void setQuantum(int newQuantum);
int getQuantum();

#endif
