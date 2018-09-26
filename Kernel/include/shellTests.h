#ifndef ANTEOJOS_SHELLTESTS_H
#define ANTEOJOS_SHELLTESTS_H

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

#endif //ANTEOJOS_SHELLTESTS_H
