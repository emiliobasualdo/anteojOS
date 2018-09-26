#ifndef SCLIB_H
#define SCLIB_H

#include <stdint.h>
#include "syscall.h"

/* system call to draw a string */
void write(char * );

/* system call to read the next character */
char getChar();

/* system call to get the hour */
int getHour();

/* system call to get the minutes */
int  getMinute();

/* system call to get the seconds */
int  getSecond();

/* system call to make a beep sound */
void kernelBeep();

/* system call to suspend the system for a determined time */
void sleep();

/* system call to draw a pixel with a given colour */
void userDrawPixel(uint64_t , uint64_t, uint64_t , uint64_t , uint64_t );

/* system call to get the width and height screen resolutions */
void getResolutions(unsigned int * , unsigned int * );

/* system call to set a new font colour */
void setFontColour(uint8_t , uint8_t , uint8_t );

/* system call to exit and make a beep sound */
int notifyExitRequest();

/* system call to clear the screen and show a new window */
void newWindow();

/* system call to write a char */
int putChar(char );

/* system call to do a backSpace */
int removeChar();

/* system call to change the background colour */
void setBackgroundColour(uint8_t , uint8_t , uint8_t );

/* system call to change coordinates */
void setCoordinates(unsigned int, unsigned int);

void *userMalloc(uint64_t x);

void userFree(uint64_t x);

/* system call to print current process states
 * x  = 0 prints Tree type
 * x != 0 prints process queues
 */
void userPs(char type, int pid);

/* not much to explain*/
int userStartProcess(char *name, uint64_t instruct, int foreground);

/* Can change between Running, Blocked, Dead*/
int userKill(uint64_t rdi);

/* Not much to explain*/
int userProcessBomb();

/* returns the current process id*/
int userGetCurrentPid();


#endif
