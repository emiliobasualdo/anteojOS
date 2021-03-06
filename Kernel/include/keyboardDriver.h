#ifndef _keyboardDriver_h
#define _keyboardDriver_h

#include <videoDriver.h>
#include <process.h>
#include <scheduler.h>
#include <pipes.h>

#define BUFFERSIZE 1024
#define UP -2
#define DOWN -3

int initKeyboardDriver();

/* gets the key pressed from stdout */
extern int getKey(void);

/* interprets the key pressed to a character and adds it to the buffer */
int keyboardInterpreter();

/* adds a character to the current buffer */
void charToBuffer(unsigned char);

char getNextChar();

#endif
