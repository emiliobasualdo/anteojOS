//
// Created by Emilio Basualdo on 9/4/18.
//

#ifndef PROCESOS_SCHEDULER_H_H
#define PROCESOS_SCHEDULER_H_H

#include <roundRobin.h>
#include <stdint.h>
#include <printf.h>
#include <interrupts.h>


pcbPtr initScheduler(char *name, uint64_t instruction);
pcbPtr getCurrentProc();
uint64_t dispatcher(uint64_t rsp);
pPid createAndExecProcess(char *name, uint64_t instruction, pPid parent, boolean foreground);
void printtt(uint64_t num);
void switchToNext();
boolean wakeUpBlocked(reasonT reason);
void printProcQueues();
boolean schedulerAddProcPid(pPid proc);

#endif //PROCESOS_SCHEDULER_H_H
