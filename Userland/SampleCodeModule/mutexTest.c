#include <scLib.h>
#include "mutexTest.h"

#define LOOPS 10

int mutex1;
int mutex2;
int variable = 0;
int procsAlive;

void proc();

void initTest(int amountOfProcs)
{
    procsAlive = amountOfProcs;
    mutex1 = newMutex(0);
    mutex2 = newMutex(1);
    printF("mutex: %d\n", mutex1);
    int procs[amountOfProcs];
    char name[10];
    int i;
    for(i = 0;i < amountOfProcs; i++)
    {
        userSprintf(name,"%s%d","m", i);
        procs[i] = createProc(name,(uint64_t) proc, NULL, 0);
        pipesToStds(procs[i], 2);
        startProc(procs[i]);
    }

    while (getChar() != 'q');
    printF("Variable despues %d loops de %d procs: %d\n", LOOPS, amountOfProcs,variable);

}

void proc()
{
    int i = 0;
    while(i< LOOPS)
    {
        lock(mutex1);

        variable++;
        printF("Soy el proc %d y acabo de incrementar la variable a %d \n", userGetCurrentPid(),variable);

        unlock(mutex1);
        i++;
    }
    printF("Soy el proc %d y termine \n", userGetCurrentPid(),variable);

}