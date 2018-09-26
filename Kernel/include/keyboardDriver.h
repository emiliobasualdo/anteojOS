#ifndef _keyboardDriver_h
#define _keyboardDriver_h

#include <videoDriver.h>
#include <process.h>
#include <scheduler.h>

#define BUFFERSIZE 1024

/* gets the key pressed from stdout */
extern int getKey(void);

/* interprets the key pressed to a character and adds it to the buffer */
void keyboardInterpreter();

/* adds a character to the current buffer */
void charToBuffer(unsigned char);

/* returns the first added character in buffer */
char returnNextChar();

/* returns a boolean value: 1 if there is a new character to read */
int newToRead();

char getNextChar();

#endif
