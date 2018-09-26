#include "scLib.h"

void write(char * string)
{
    syscall(1,(uint64_t) string, 0,0,0,0);
}
char getChar()
{
    return (char) syscall(2, 0, 0, 0, 0, 0);
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
int notifyExitRequest(unsigned int rdi, unsigned int rsi, const unsigned short *rdx, unsigned int rcx, unsigned int r8)
{
    return (int) syscall(11, 0, 0, 0, 0, 0);
}
int putChar(char c)
{
    return (int) syscall(12, c, 0, 0, 0, 0);
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
void userPs(char x)
{
    syscall(18, (uint64_t)x, 0, 0, 0, 0);
}
/**
 * rdi: char *name
 * rsi: uint64 isntruction
 * rdx: boolean foreground
 */
int userStartProcess(char *name, uint64_t instruct, int foreground)
{
    return (int) syscall(19, (uint64_t) name, instruct, (uint64_t) foreground, 0, 0);
}

int userKill(uint64_t pid)
{
    return syscall(20, pid,0,0, 0 ,0);
}

int userProcessBomb()
{
    return (int) syscall(21, 0, 0, 0, 0 , 0);
}

int userGetCurrentPid()
{
    return (int) syscall(22, 0, 0, 0, 0 , 0);
}
