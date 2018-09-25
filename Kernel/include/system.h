//
// Created by Emilio Basualdo on 9/4/18.
//

#ifndef PROCESOS_SYSTEM_H
#define PROCESOS_SYSTEM_H

#include <printf.h>

#define asm __asm__
#define volatile __volatile__

#define IRQ_OFF { asm volatile ("cli"); } //<- https://github.com/stevej/osdev/tree/master/kernel
#define IRQ_RES { asm volatile ("sti"); }
#define HALT    { asm volatile ("hlt"); }
#define STOP while (1) { HALT; }

#define FALSE 0
#define TRUE !FALSE
typedef int boolean;

char *myStrncpy(char *dst, const char*src, unsigned int n);
int strcmp(char *, char *);
int strlen(char *);
#endif //PROCESOS_SYSTEM_H
