#ifndef SHELLTESTS_H
#define SHELLTESTS_H

#include <printf.h>
#include <scheduler.h>
#include <system.h>
#include <keyboardDriver.h>
#include "ipc.h"
#include <roundRobin.h>

boolean processBomb();
int proc1();
int proc2();
int multiProcessTest();
void mutexTest();
void messageTest();

#endif
