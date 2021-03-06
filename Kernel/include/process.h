#ifndef PROCESOS_PROCESS_H
#define PROCESOS_PROCESS_H

#include <stdlib.h>
#include <stdio.h>
#include <system.h>
#include <videoDriver.h>
#include <defs.h>
#include <stdarg.h>
#include <queue.h>
#include "ipcStructs.h"
#include "dinamicMemory.h"

#define MAX_PROC_NAME 15
#define INIT_PID 0
#define BUSSY_WAITING (INIT_PID+1)
#define MAX_PROCS 70
#define MAX_CHILDREN 50
#define MAX_SECURITY_LIMITAION 3
#define PID_ERROR (-1)

#define HEAP_STACK_SIZE 4096
#define NO_PARENT 0
#define CS_VALUE 8
#define RFLAGS_VALUE 518
#define SS_VALUE 0
#define DEFAULT_NAME "process"
#define DO_NOT_CHANGE (-3)
#define INTERACTIVE (-2)
#define NORMAL (-1)
#define PRIORITY_LEVELS 5
#define MAX_PRIORITY 0
#define MIN_PRIORITY (PRIORITY_LEVELS-1)
#define DEFAULT_PRIORITY 1
#define FD_AMOUNT 2



typedef enum {BORN = 0, READY, RUNNING, BLOCKED, DEAD, UN_BORN = -5}pState;
typedef enum {KEYBOARD=0, NO_REASON, MESSAGE_PASSING, MUTEX_BLOCK, REASON_COUNT}reasonT;
typedef enum {NICE, MUTEX}pReason;
typedef int pPid;


typedef struct {
    uint64_t r15;  // 10
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;    // 30

    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;

    //uint64_t base; // variable para alinear el stack
} stackFrame_t;

typedef struct
{
    pPid pid;
    char name[MAX_PROC_NAME];
    pPid ppid;
    pState state;
    reasonT blockedReason;

    boolean foreground;
    short priority;           // 0 prioridad más alta, PRIORITY_LEVELS-1 la más baja
    short priorityType;

    uint64_t rsp;

    stackFrame_t *stackFrame;

    uint64_t stackBase;     // } aca entre estos dos tenemos X paginas reservadas
    uint64_t heapBase;      // } stack apunta a la parte "de arriba"/alta, heap a la parte "de abajo"/baja

    pPid childs[MAX_CHILDREN];
    unsigned int childrenCount;

    int creationLimit;
    messageQueue * postBox;
    int fd[FD_AMOUNT];
}pcb;


typedef pcb *pcbPtr;

typedef pcbPtr *pArray;


int procContainer(uint64_t inst, char **argv, int argc);
pcbPtr
createProcess(char *name, uint64_t instruction, pPid parentPid, boolean foreground, short i, char **argv, int argc);
pcbPtr initProcessControl(char *name, uint64_t instruction);
boolean procExists(pPid pid);
boolean isValidPState(int state);
boolean setProcessState(pPid pcbPtr, pState newState, reasonT reason);
pcbPtr getBussyWaitingProcPcb();
int bussyWaitingProc();
void printAllProcs();
pcbPtr getPcbPtr(pPid pid);
void printSons(pPid parentPid);
boolean validReason(int reason);
boolean directSetProcessState(pPid pid, pState newState, reasonT reason);
boolean setProcessPriority(pPid pid, short newPriority, pReason reason);
boolean reduceProcessPriority(pPid pid);
void killAllDescendants(pPid pid);


#endif //PROCESOS_PROCESS_H
