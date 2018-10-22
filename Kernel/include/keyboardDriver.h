#ifndef _keyboardDriver_h
#define _keyboardDriver_h

#include <videoDriver.h>
#include <process.h>
#include <scheduler.h>

#define BUFFERSIZE 1024
#define UP -2
#define DOWN -3
#define C_UP -4
#define C_DOWN -5

/* gets the key pressed from stdout */
extern int getKey(void);

/* interprets the key pressed to a character and adds it to the buffer */
int keyboardInterpreter();

/* adds a character to the current buffer */
void charToBuffer(int);

/* returns the first added character in buffer */
int returnNextChar();

/* returns a boolean value: 1 if there is a new character to read */
int newToRead();

int getNextChar();

#endif
