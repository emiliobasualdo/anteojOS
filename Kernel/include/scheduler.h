#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <roundRobin.h>
#include <stdint.h>
#include <printf.h>
#include <interrupts.h>


pcbPtr initScheduler(char *name, uint64_t instruction);
pcbPtr getCurrentProc();
uint64_t dispatcher(uint64_t rsp);
pPid createAndExecProcess(char *name, uint64_t instruction, pPid parent, boolean foreground, short priority, char **argv,
                          int argc);
void printtt(uint64_t num);
void switchToNext();
boolean wakeUpBlocked(reasonT reason);
void printProcQueues();
boolean schedulerAddProcPid(pPid proc);
void schedulerNotifyProcessStateChange(pPid pid);
void schedulerNotifyProcessPriorityChange(pPid pid);
unsigned long long getProcRunTime(pPid pid);
pPid createNotExecProcess(char *name, uint64_t instruction, pPid parent, boolean foreground, short priority, char **argv,
                          int argc);
boolean execProc(pPid pid);

#endif
