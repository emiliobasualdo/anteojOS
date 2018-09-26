#ifndef ROUNDROBIN_H
#define ROUNDROBIN_H

#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <system.h>
#include "dinamicMemory.h"

typedef struct rrNode *rrNodePtr;

typedef struct rrNode{
    pcbPtr pcbPtr;
    struct rrNode *next;
    unsigned long quantum;
}rrNode;

typedef struct rrQueue{
    rrNodePtr head;
    rrNodePtr tail;
    rrNodePtr current;
    long unsigned count;
}rrQueue;

boolean rrInit(pcbPtr pcbPtr);
boolean rrAddProcess(pcbPtr pcbPtr);
pcbPtr rrNextAvailableProcess();
boolean rrUnblockWaiters(reasonT reason);
void printRRQueues();
void setQuantum(int newQuantum);
int getQuantum();

#endif
