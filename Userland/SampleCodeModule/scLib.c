#include <userPrintf.h>
#include "scLib.h"

void writeString(char *string)
{
    syscall(1,1,(uint64_t) string,(uint64_t) strlen(string),0,0);
}
void write(char * string, int length)
{
    syscall(1,1,(uint64_t) string,(uint64_t)length,0,0);
}

void read(char * string, int length)
{
    syscall(2,0,(uint64_t) string,(uint64_t)length,0,0);
}

int getChar()
{
    char aux = 0;

    syscall(2, 0, (uint64_t) (&aux), 1, 0, 0);//que create me retorne el puntero de pipe...

    return aux;
}

int getHour()
{
    return (int) syscall(3, 0, 0, 0, 0, 0);
}
int getMinute()
{
    return (int) syscall(4, 0, 0, 0, 0, 0);
}
int getSecond()
{
    return (int) syscall(5, 0, 0, 0, 0, 0);
}
void kernelBeep()
{
    syscall(6,0,0,0,0,0);
}
void sleep()
{
    syscall(7,0,0,0,0,0);
}
void userDrawPixel(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    syscall(8,(uint64_t)rdi,(uint64_t)rsi,(uint64_t)rdx,(uint64_t)rcx,(uint64_t)r8);
}
void getResolutions(unsigned int * rdi, unsigned int * rsi)
{
    syscall(9,(uint64_t)rdi,(uint64_t)rsi,0,0,0);
}
void setFontColour(uint8_t r, uint8_t g, uint8_t b)
{
    syscall(10,r,g,b, 0,0);
}
int notifyExitRequest()
{
    return (int) syscall(11, 0, 0, 0, 0, 0);
}
int putChar(char c)
{
    return (int) syscall(1,1,(uint64_t) (&c),1,0,0);
    //return (int) syscall(12, (uint64_t) c, 0, 0, 0, 0);
}
int removeChar()
{
    return (int) syscall(13, 0, 0, 0, 0, 0);
}
void setBackgroundColour(uint8_t r, uint8_t g, uint8_t b)
{
    syscall(14,r,g,b, 0,0);
}
void setCoordinates(unsigned int x, unsigned int y)
{
    syscall(15, (uint64_t)x, (uint64_t)y, 0, 0, 0);
}
void *userMalloc(uint64_t x)
{
    return (void*)syscall(16, (uint64_t)x, 0, 0, 0, 0);
}
void userFree(uint64_t x)
{
    syscall(17, (uint64_t)x, 0, 0, 0, 0);
}
void userPs(char type, int pid)
{
    syscall(18, (uint64_t)type, (uint64_t) pid, 0, 0, 0);
}
/**
 * rdi: char *name
 * rsi: uint64 isntruction
 * rdx: boolean foreground
 */
int userStartProcess(char *name, uint64_t instruct, char **argv, int argc)
{
    int pid  = (int) syscall(19, (uint64_t) name, instruct, (uint64_t) FALSE, (uint64_t) argv, (uint64_t) argc);;
    return pid;
}
int userKill(int fromPid, int toPid)
{
    return (int) syscall(20, (uint64_t) fromPid, (uint64_t) toPid, 0, 0 , 0);
}
int userProcessBomb()
{
    return (int) syscall(21, 0, 0, 0, 0 , 0);
}
int userGetCurrentPid()
{
    return (int) syscall(22, 0, 0, 0, 0 , 0);
}

int send(int receiver, char * content,  char ** answer, uint8_t flag)
{
    return (int) syscall(23, (uint64_t)receiver, (uint64_t)content, (uint64_t)(answer), (uint64_t)flag, 0);
}

int receive(char ** message)
{
    return (int) syscall(24, (uint64_t) message, 0, 0, 0 , 0);
}

int newMutex(int initValue)
{
    return (int) syscall(25, (uint64_t) initValue, 0, 0, 0 , 0);
}

int lock(int mutex)
{
    return (int) syscall(26,(uint64_t) mutex, 0, 0, 0 , 0);
}

int unlock(int mutex)
{
    return (int) syscall(27,(uint64_t) mutex, 0, 0, 0 , 0);
}

int destroyMutex(int mutex)
{
    return (int) syscall(28,(uint64_t) mutex, 0, 0, 0 , 0);
}

void kernelAllocatorTest()
{
    syscall(29, 0, 0, 0, 0, 0);
}

void kernelNice(int pid, int niceValue)
{
    syscall(30, (uint64_t) pid, (uint64_t) niceValue, 0, 0, 0);
}

void kernelColumnTest(int num, int ageing)
{
    syscall(31, (uint64_t) num, (uint64_t) ageing, 0, 0, 0);
}

void userKillAllDescendants(int pid)
{
    syscall(32, (uint64_t) pid, 0, 0, 0, 0);
}
int semStart(int amount)
{
    return (int) syscall(33, (uint64_t) amount, 0, 0, 0,0);
}

int semWait(int sem)
{
    return (int) syscall(34, (uint64_t) sem, 0, 0, 0,0);
}

int semPost(int sem)
{
    return (int) syscall(35, (uint64_t) sem, 0, 0, 0,0);
}

int semDestroy(int sem)
{
    return (int) syscall(36, (uint64_t)sem, 0, 0, 0, 0);
}

int userGetQuantum()
{
    return (int) syscall(37, 0, 0, 0, 0, 0);
}

void userSetQuantum(int pid)
{
    syscall(38, (uint64_t) pid, 0, 0, 0, 0);
}

int pipe(int p1, int p2)
{
    return (int) syscall(41, (uint64_t) p1, (uint64_t) p2, 0, 0, 0);
}
