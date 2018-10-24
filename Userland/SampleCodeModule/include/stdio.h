#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>
#include "stdlib.h"
#include "scLib.h"

#define MAX_BUFFER 1024
#define MAX_INT_DIGIT 20
#define FALSE 0
#define TRUE !FALSE

/* reads the buffer and returns the line written on myStdin */
char * reaDLine(char *, unsigned int);

/*
** reads formatted input from a given buffer
*/
int scanF(char *, unsigned int, const char *,...);

/* converts a string to integer */
char * toInt(char *, int *, int *);

int getNum();

void strncpy(char *to, const char * from, int n);
#endif
