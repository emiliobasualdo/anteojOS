//
// Created by Emilio Basualdo on 9/4/18.
//

#ifndef PROCESOS_PDOUBLYLINKEDLIST_H
#define PROCESOS_PDOUBLYLINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <system.h>
#include <myAlloc.h>

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

#endif //PROCESOS_PDOUBLYLINKEDLIST_H
