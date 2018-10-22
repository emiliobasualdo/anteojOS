#ifndef PHILOSOPHERS_H
#define PHILOSOPHERS_H

#include "scLib.h"
#include "userPrintf.h"
#include "stdio.h"

#define N 5                         /** número de filósofos */
#define THINKING 2                  /** el filósofo está pensando */
#define HUNGRY 1                    /** el filósofo está buscando los tenedores */
#define EATING 0                    /** el filósofo está comiendo */
#define LEFT (phnum - 1) % N
#define RIGHT (phnum + 1) % N
#define NAME "PHIL"

int startPhilosophers(int num);

#endif
