#ifndef SHELLTESTS_H
#define SHELLTESTS_H

#include <printf.h>
#include <scheduler.h>
#include <system.h>
#include <keyboardDriver.h>
#include <ipc.h>
#include <roundRobin.h>
#include <videoDriver.h>

boolean processBomb();
void columnTest(short cantProcs, boolean ageing);
int proc1();
int proc2();
int multiProcessTest();
void mutexTest();
void semTest();

#endif
