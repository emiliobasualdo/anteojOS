#include <process.h>
#include "syscaller.h"

typedef uint64_t (*func_type)();

func_type fList[] = {writeK, readK, getHour, getMin, getSec, beep,
                               sleep, userDrawPixel, getResolutions, changeFontColour,
                               myExit, putChar, removeChar, changeBackgroundColour,
                               setCoordinates, sysMalloc, sysFree,
                               printProcess, kernelCreateAndExProcess, kill, procBomb, getCurrentPid, send, receive,
                               createMutex, kernelLock, kernelUnlock, destroyMutexKernel, sysAllocatorTest,
                               nice, kernelColumnTest,kernelKillAllDescendants, kernelCreateSemaphore, kernelSemWait,
                               kernelSemPost, kernelSemDestroy, kernelGetQuantum, kernelSetQuantum, openPipe, closeK, pipeK,
                                kernelCreateProcess, kernelStartProcess, pipesToStdsK, drawCharInULND};


uint64_t syscaller(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    uint64_t (*function)(uint64_t , uint64_t , uint64_t , uint64_t , uint64_t );
    function = fList[rax-1];
    return function(rdi,rsi,rdx,rcx,r8);
}
uint64_t writeK(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    int fd = (int)rdi;
    char * buffer = (char *) rsi;
    int length = (int) rdx;

    pcbPtr process = getCurrentProc();

    pipe_t * pipe = getPipeFromPipeList(process->fd[fd]);

    return (uint64_t) writePipeK(pipe, buffer, (uint64_t)length);

}
uint64_t readK(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    int fd = (int)rdi;
    char * buffer = (char *) rsi;
    int length = (int) rdx;
    if(length <= 0)
    {
        return 0;
    }
    pcbPtr process = getCurrentProc();
    pipe_t * pipe = getPipeFromPipeList(process->fd[fd]);
    return (uint64_t) readPipeK(pipe, buffer, (uint64_t) length);
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
    pcbPtr process = getCurrentProc();
    if(process == NULL ||process->fd[STDOUT] != STDOUT)
    {
        return 1;
    }
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
    setCoord((unsigned int) rdi, (unsigned int) rsi);
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
        printProcQueues();
    else if(rdi == 'p')
        printSons((pPid) rsi);
    else
        printAllProcs();
    return 0;
}
uint64_t kernelCreateAndExProcess(uint64_t name, uint64_t inst, uint64_t pid, uint64_t argv, uint64_t argc)
{
    *((int*)pid) = createAndExecProcess((char *) name, inst, (pPid) getCurrentProc()->pid, (boolean) FALSE,
            DEFAULT_PRIORITY, (char **) argv, (int) argc);
    //simple_printf("pid %d\n",pid);
    return (uint64_t)pid ;
}

uint64_t kill(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    int from, to;
    from = (int) rdi;
    to = (int) rsi;
    if(from < 0 || (from > to))
        return FALSE;

    boolean ret = TRUE;
    for (int i = from; i <= rsi; ++i)
    {
        ret = ret && setProcessState(i, DEAD, NO_REASON);
    }
    return (uint64_t) ret;
}
uint64_t procBomb(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return (uint64_t) processBomb();
}
uint64_t getCurrentPid(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    *((int*)rdi) = getCurrentProc()->pid;
    return TRUE;
}
uint64_t send(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return sendMessage((pPid)rdi, (char *)rsi, (char **)rdx, (boolean)rcx);
}
uint64_t receive(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return receiveMessage((char **) rdi, NULL, 1);
}
uint64_t createMutex(uint64_t rdi, uint64_t id, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    *((int*)id) = startMutex((int) rdi);
    return id;
}
uint64_t kernelLock(uint64_t rdi, uint64_t resp, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    simple_printf("mutex in kernelLock: %d\n", (int)rdi);
    *((int*)resp) = lockMutex((int)rdi);
    return resp;

}
uint64_t kernelUnlock(uint64_t rdi, uint64_t resp, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    *((int*)resp) = unlockMutex((int)rdi);
    return resp;
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
uint64_t kernelKillAllDescendants(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    killAllDescendants((pPid) rdi);
    return 1;
}
uint64_t kernelCreateSemaphore(uint64_t rdi, uint64_t id, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    *((int*)id) = semStartK((int)rdi);
    return 1;
}
uint64_t kernelSemWait(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return (uint64_t)semWaitK((int)rdi);
}
uint64_t kernelSemPost(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return (uint64_t)semPostK((int)rdi);
}
uint64_t kernelSemDestroy(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return (uint64_t)semDestroyK((int)rdi);
}
uint64_t kernelGetQuantum(uint64_t ret, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    *((int*)ret) = getQuantum();
    return 1;
}
uint64_t kernelSetQuantum(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    setQuantum((int) rdi);
    return 1;
}
uint64_t openPipe(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return (uint64_t) addPipeToSC();
}
uint64_t closeK(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    int pipe = (int) rdi;
    if(pipe < 0 ||pipe >= FD_AMOUNT)
    {
        return 0;
    }
    pipe_t * aux = getPipeFromPipeList(getCurrentProc()->fd[pipe]);
    return (uint64_t)closePipeK(aux);
}
uint64_t pipeK(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return (uint64_t)dupProc((pPid) rdi, (pPid) rsi);
}

uint64_t kernelCreateProcess(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    pPid aux = createNotExecProcess((char *) rdi, rsi, getCurrentProc()->pid, FALSE, DEFAULT_PRIORITY, (char **) rdx, (int) rcx);
    simple_printf("pidK: %d\n",aux);
    *(int*)(r8) = aux;
    return (uint64_t) 1;
}
uint64_t kernelStartProcess(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return (uint64_t) execProc(*((int*)rdi));
}
uint64_t pipesToStdsK(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    return (uint64_t) changeToStds((pPid) rdi, (int)rsi);
}
uint64_t drawCharInULND(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8)
{
    drawChar((char) rdi);
    return 0;
}
