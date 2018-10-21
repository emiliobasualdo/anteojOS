#include <shellTests.h>

#define NAME "proc"
#define MAX 40
#define END 1000000000 // velocidad
#define CANT_BLOCKS 50
#define LEVEL (END/CANT_BLOCKS)
#define PAD 10

short cantProcess;
int xRes,yRes, xInit, yInit;
int cantRows ;
int rowHeight;
int blockWidth;


void printBlock(int, int, short priority);

void setVariables(short procsCant);

void drawLoop()
{
    //simple_printf("%d %d %s\n", getCurrentProc()->priority,getCurrentProc()->priorityType, getCurrentProc()->name);
    unsigned long long counter = 0;
    while(counter != END)
    {
        counter++;
        if(counter % LEVEL == 0) // por cada nivel imprimimos un bloque
        {
            //simple_printf("loopeando\n");
            int flag, procNum;
            kernelToInt(getCurrentProc()->name, &procNum, &flag);
            printBlock((int) ((counter / LEVEL) * 2), procNum*2 +1 , getCurrentProc()->priority);
        }
    }
    setProcessState(getCurrentProc()->pid, BLOCKED, NO_REASON);
}

void printBlock(int i, int j, short priority)
{
    int x2Init = xInit + blockWidth*i;
    int y2Init = yInit + rowHeight*j;

/*    for (int k = 0; k < blockWidth; ++k)
    {
        for (int l = 0; l < rowHeight; ++l)
        {
            drawAPixel((unsigned int) k, (unsigned int) l+y2Init);
        }
    }*/
    setCoord((unsigned int) (xInit + ((blockWidth / 2) - (CHAR_WIDTH / 2))),
             (unsigned int) (yInit + ((rowHeight / 2) - (charHeight / 2)) + j * rowHeight));
    drawChar((char) (priority + '0'));

    for (int k = 0; k < blockWidth; ++k)
    {
        for (int l = 0; l < rowHeight; ++l)
        {
            drawAPixel((unsigned int) k+x2Init, (unsigned int) l+y2Init);
        }
    }
}

void columnTest(short cantProcs, boolean ageing)
{
    if(cantProcs < 1 || cantProcs > MAX)
    {
        simple_printf("Kernel ERROR: max is %d!!\n", MAX);
        return;
    }

    clearWindow();

    char name[20];
    short priority;

    setVariables(cantProcs);

    for (int i = 0; i < cantProcs; ++i)
    {
        priority = (short) (i % PRIORITY_LEVELS);
        simple_sprintf(name,"%d-%s-%d", i, NAME, priority); // la congurencia nunca va a quedar 5
        pPid pid = createAndExecProcess(name, (uint64_t) drawLoop, getCurrentProc()->pid, FALSE, priority);
        if(!ageing)
            setProcessPriority(pid, priority);
    }
    // agregar que pueda cambiar el cuantum con teclas
    while (getNextChar() != 'q'){}
    killAllDescendants(getCurrentProc()->pid);
    clearWindow();
}

void setVariables(short procsCant)
{
    cantProcess = procsCant;
    xRes = getXResolution();
    yRes = getYResolution();
    xInit = 0;
    yInit = 0;
    cantRows = (2*cantProcess)+1;
    rowHeight = yRes/cantRows;
    blockWidth = xRes / (CANT_BLOCKS*2);
    //simple_printf("xRes=%d yRes=%d cantRows=%d rowHeight=%d blockHeight=%d\n",xRes, yRes,cantRows, rowHeight, blockHeight);
}


int proc1()
{
    simple_printf("Hola!! soy %s y estoy creando hijos\n", getCurrentProc()->name);
    createAndExecProcess(NULL, (uint64_t) proc1, getCurrentProc()->pid, FALSE, DEFAULT_PRIORITY);
    proc1();
    return -1;
}

boolean processBomb()
{
    char name[MAX_PROC_NAME];
    simple_printf("We will run a process-bomb.\n");
    simple_printf("Basically we will create and execute a recursive process and see what happen.\n");
    simple_printf("The process's code is:\n");
    simple_printf("int proc1()\n"
                  "{\n"
                  "    simple_printf(\"Hola!! soy %s y estoy creando hijos\\n\", getCurrentProc()->name);\n"
                  "    createAndExecProcess(NULL, (uint64_t) proc1, getCurrentProc()->pid, FALSE);\n"
                  "    proc1();\n"
                  "    return -1;\n"
                  "}");
    simple_printf("\nPress any key to start\n");
    getNextChar();
    getNextChar();
    simple_sprintf(name,"%sc","process_bomb");
    createAndExecProcess(name, (uint64_t) proc1, getCurrentProc()->pid, FALSE, DEFAULT_PRIORITY);
    simple_printf("If we got here is because this OS is the best at handling process bombs ;)\n");
    return TRUE;
}

int i = 0;
int mutex = 0;

static void f1()
{
    int j = 0;
    lockMutex(mutex);
    while(j++ < 100)
    {
        i++;
        simple_printf("if1 = %d\n", i);
    }
    unlockMutex(mutex);
    simple_printf("aca termina f1\n");

}
static void f2()
{
    int j = 0;
    while(j++ < 100)
    {
        lockMutex(mutex);
        i--;
        simple_printf("if2 = %d\n", i);

        unlockMutex(mutex);
    }
    simple_printf("aca termina f2\n");
}

static void f3()
{
    int j = 0;
    while(j++ < 100)
    {
        lockMutex(mutex);
        i += 2;
        simple_printf("if3 = %d\n", i);
        unlockMutex(mutex);
    }
    simple_printf("aca termina f3\n");

}

void mutexTest()
{
    mutex = startMutex(0);
    if (createAndExecProcess("f1", (uint64_t) f1, getCurrentProc()->pid, FALSE, DEFAULT_PRIORITY) == PID_ERROR)
    {
        simple_printf("f1: ERROR: otro == NULL\n");
        return;
    }
    if (createAndExecProcess("f2", (uint64_t) f2, getCurrentProc()->pid, FALSE, DEFAULT_PRIORITY) == PID_ERROR)
    {
        simple_printf("f2: ERROR: otro == NULL\n");
        return;
    }
    if (createAndExecProcess("f3", (uint64_t) f3, getCurrentProc()->pid, FALSE, DEFAULT_PRIORITY) == PID_ERROR)
    {
        simple_printf("f3: ERROR: otro == NULL\n");
        return;
    }
    long aux = 0;
    while (aux < 100000000) {

        aux++;
    }

    while(destroyMutexK(mutex) == -1)
    {
        aux = 0;
        while (aux < 45)
        {
        aux++;
        }
    }

    simple_printf("i final = %d\n", i);
}

int sem = 0;

static void s1()
{
    int j = 0;
    semWaitK(sem);
    while(j++ < 100)
    {
        i++;
        simple_printf("is1 = %d\n", i);
    }
    semPostK(sem);
    simple_printf("aca termina s1\n");

}
static void s2()
{
    int j = 0;
    while(j++ < 100)
    {
//        simple_printf("me bloquee antes del sem wait2\n");
        semWaitK(sem);
//        simple_printf("me bloquee dsps del sem wait2\n");
        i--;
        simple_printf("is2 = %d j= %d\n", i, j);
        semPostK(sem);
    }
    simple_printf("aca termina s2\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
}

static void s3()
{
    int j = 0;
    while(j++ < 100)
    {
//        simple_printf("me bloquee antes del sem wait3\n");
        semWaitK(sem);
//        simple_printf("me bloquee dsps del sem wait3\n");
        i += 2;
        simple_printf("is3 = %d j=%d\n", i, j);
        semPostK(sem);
    }
    simple_printf("aca termina s3\n");

}

void semTest()
{
    sem = semStartK(1);
    if (createAndExecProcess("s1", (uint64_t) s1, getCurrentProc()->pid, FALSE, DEFAULT_PRIORITY) == PID_ERROR)
    {
        simple_printf("s1: ERROR: otro == NULL\n");
        return;
    }
    if (createAndExecProcess("s2", (uint64_t) s2, getCurrentProc()->pid, FALSE, DEFAULT_PRIORITY) == PID_ERROR)
    {
        simple_printf("s2: ERROR: otro == NULL\n");
        return;
    }
    if (createAndExecProcess("s3", (uint64_t) s3, getCurrentProc()->pid, FALSE, DEFAULT_PRIORITY) == PID_ERROR)
    {
        simple_printf("s3: ERROR: otro == NULL\n");
        return;
    }
    long aux = 0;
    while (aux < 100000000) {

        aux++;
    }

    while(semDestroyK(mutex) == -1)
    {
        aux = 0;
        while (aux < 45)
        {
            aux++;
        }
    }

    simple_printf("i final = %d\n", i);
}
