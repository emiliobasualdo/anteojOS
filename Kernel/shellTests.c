#include <shellTests.h>



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
    mutex = startMutex();
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
        simple_printf("f2: ERROR: otro == NULL\n");
        return;
    }
    long ifff = 0;
    while (ifff < 100000000) {

        ifff++;
    }

    while(destroyMutexK(mutex) == -1)
    {
        ifff = 0;
        while (ifff < 45)
        {
        ifff++;
        }
    }

    simple_printf("i final = %d\n", i);
}
