#ifndef SCLIB_H
#define SCLIB_H

#include <stdint.h>
#include "syscall.h"
#include "stdlib.h"

/* system call to draw a string*/
void writeString(char *string);
/* system call to draw a string if STDOUT or writeString in pipe if else*/
void write(char *, int);

/* system call to read a string if STDIN or read from pipe if else*/
void read(char *, int);

/* system call to readK the next character */
int getChar();

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

/* system call to writeK a char */
int putChar(char );

/* system call to do a backSpace */
int removeChar();

/* system call to change the background colour */
void setBackgroundColour(uint8_t , uint8_t , uint8_t );

/* system call to change coordinates */
void setCoordinates(unsigned int, unsigned int);

void * userMalloc(uint64_t x);

void userFree(uint64_t x);

/* system call to print current process states
 * x  = 0 prints Tree type
 * x != 0 prints process queues
 */
void userPs(char type, int pid);

/* not much to explain*/
int userStartProcess(char *name, uint64_t instruct, int foreground);

/* Can change between Running, Blocked, Dead*/
int userKill(int rdi, int toPid);

/* Not much to explain*/
int userProcessBomb();

/* returns the current process id*/
int userGetCurrentPid();

/*sends message in between processes. Flag changes sync, being 0 async */
int send(int receiver, char * content,  char ** answer, uint8_t flag);

/*receives message from postBox. Blocks process if postBox is empty*/
int receive(char ** message);

/*creates a mutex. Return value is the mutex Number, used to lock,unlock*/
int newMutex(int initValue);

/*tries to lock if mutex available. If not, it blocks until it is unlocked by the process that locked it*/
int lock(int mutex);

/*unlocks mutex, changing the state of the next process in the mutex line to ready*/
int unlock(int mutex);

/*makes unlockMutex not able to be used unless started*/
int destroyMutex(int mutex);

/* runs the allocator test on kernel */
void kernelAllocatorTest();

/* Changes the prioriy of the process*/
void kernelNice(int i, int i1);

void kernelColumnTest(int num, int ageing);

void userKillAllDescendants(int pid);

/* Creates a new semaphore */
int semStart(int amount);

/* Tries to pass semaphore*/
int semWait(int sem);

/* Frees semaphore*/
int semPost(int sem);

/* Destroys Semaphore*/
int semDestroy(int sem);

int userGetQuantum();

void userSetQuantum(int pid);

int pipe(int p1, int p2);
#endif
