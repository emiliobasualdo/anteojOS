#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>
#include "stdlib.h"
#include "scLib.h"

#define MAX_BUFFER 1024

/* reads the buffer and returns the line written on stdin */
char * reaDLine(char *, unsigned int);

/*
** reads formatted input from a given buffer
*/
int scanF(char *, unsigned int, const char *,...);

/* converts a string to integer */
char * toInt(char *, int *, int *);

#endif
