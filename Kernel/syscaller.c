#include "syscaller.h"

typedef uint64_t (*func_type)();

func_type fList[] = {write, read, getHour, getMin, getSec, beep,
                               sleep, userDrawPixel, getResolutions, changeFontColour,
                               myExit, putChar, removeChar, changeBackgroundColour,
                               setCoordinates, sysMalloc, sysFree,
                               printProcess, startProcess, kill, procBomb, getCurrentPid, send, receive,
                               createMutex, kernelLock, kernelUnlock, destroyMutexKernel, sysAllocatorTest,
                               nice, kernelColumnTest };

uint64_t syscaller(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    uint64_t (*function)(uint64_t , uint64_t , uint64_t , uint64_t , uint64_t );
    function = fList[rax-1];
    return function(rdi,rsi,rdx,rcx,r8);
}

uint64_t write(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    drawString((const char *) rdi);
    return 0;
}
uint64_t read(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return (uint64_t) getNextChar();
}
uint64_t getHour(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return getTimeHour();
}
uint64_t getMin(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return getTimeMin();
}
uint64_t getSec(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return getTimeSec();
}
uint64_t beep(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    motherBeep();
    return 0;
}
uint64_t sleep(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    kernelSleep();
    return 0;
}
uint64_t userDrawPixel(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    Colour colour = {(uint8_t) rdx, (uint8_t) rcx, (uint8_t) r8};
    drawAPixelWithColour((int) rdi, (int) rsi, colour);
    return 0;
}
uint64_t getResolutions(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    *((int*)rdi) = getXResolution();
    *((int*)rsi) = getYResolution();
    return 0;
}
uint64_t changeFontColour(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    Colour col = {(uint8_t)rdi,(uint8_t)rsi,(uint8_t)rdx};
    setFontColour(col);
    return 0;
}
uint64_t myExit(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    motherBeep();
    return 0;
}
uint64_t putChar(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    drawChar((char) rdi);
    return rdi;
}
uint64_t removeChar(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    backSpace();
    return 0;
}
uint64_t changeBackgroundColour(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    Colour colour = {rdi,rsi,rdx};
    setBackgroundColour(colour);
    return 0;
}
uint64_t setCoordinates(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    setCoord(rdi, rsi);
    return 0;
}
uint64_t sysMalloc (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return (uint64_t) pageAlloc((unsigned int) rdi);
}
uint64_t sysFree (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    pageFree((void *) rdi);
    return 0;
}
uint64_t printProcess(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    if (rdi == 'q')
    {
        printProcQueues();
    }
    else if(rdi == 'p')
    {
        printSons((pPid) rsi);
    }
    else
        printAllProcs();
    return 0;
}
uint64_t startProcess(uint64_t name, uint64_t inst, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return (uint64_t) createAndExecProcess((char *) name, inst, (pPid) getCurrentProc()->pid, (boolean) rdx, DEFAULT_PRIORITY);
}
uint64_t kill(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    boolean ret = TRUE;
    for (int i = (int) rdi; i <= rsi; ++i)
    {
        ret = ret && setProcessState((pPid) i, DEAD, NO_REASON);
    }
    return ret;
}
uint64_t procBomb(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return (uint64_t) processBomb();
}
uint64_t getCurrentPid(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return (uint64_t) getCurrentProc()->pid;
}
uint64_t send(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return sendMessage((pPid)rdi, (char *)rsi, (char **)rdx, (boolean)rcx);
}
uint64_t receive(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return receiveMessage((char **) rdi, NULL, 1);
}
uint64_t createMutex(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return (uint64_t ) startMutex();
}
uint64_t kernelLock(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return (uint64_t ) lockMutex((int)rdi);
}
uint64_t kernelUnlock(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return (uint64_t ) unlockMutex((int)rdi);
}
uint64_t destroyMutexKernel(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return (uint64_t ) destroyMutexK((int)rdi);
}
uint64_t sysAllocatorTest(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    runAllocatorTest();
    return 1;
}
uint64_t nice(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return (uint64_t) setProcessPriority((pPid) rdi, (short) (int) rsi);
}
uint64_t kernelColumnTest(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    columnTest((short) rdi, (boolean) rsi);
    return TRUE;
}

