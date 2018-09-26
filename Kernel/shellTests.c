//
// Created by Emilio Basualdo on 9/24/18.
//

#include "shellTests.h"

#define PROC_COUNT 30
/*
#define COUNT MOD * 7
#define MOD 100000000
#define TIMES 4
#define PROCS 2
#define INCREMENT 7
int proc2()
{
    long long counter = 0;
    simple_printf("Soy %s pid=%d y naci\n",getCurrentProc()->name, getCurrentProc()->pid);
    while (counter++ < COUNT)
    {
        if (counter % MOD == 0)
            simple_printf("Soy %s pid=%d, llegúe a un múltiplo=%d\n",getCurrentProc()->name, getCurrentProc()->pid, counter);
    }
    return TRUE;
}
int multiProcessTest()
{
    simple_printf("\nVamos a ejecutar %d procesos simultaneamente\n", PROCS);
    simple_printf("Los %d procesos cuentan hasta %d e imprimen un mensaje cada vez que llegan a un multiplo de %d\n",PROCS,COUNT, MOD);
    simple_printf("Esto se va a hacer %d veces, y en cada ronda incrementaremos el Quantum del Round Robin en %d\n",TIMES, INCREMENT);
    simple_printf("La idea es que puedas ver como corren los procesos en distintos tiempos de Round Robin\n");
    simple_printf("!Cada vez que apretes una tecla se comenzará con la siguiente ronda. Apreta 'q' para salir!\n");
    int quantum, initQuantum;
    initQuantum = quantum = getQuantum();
    char c = 0;
    for (int j = 0; j < TIMES && c != 'q'; ++j) {
        simple_printf("\nRonda Nº:%d , Quantum=%d. Apreta cualquier tecla para comenzar esta ronda\n",j, getQuantum());
        c = getNextChar();
        for (int i = 0; i < PROCS && c != 'q'; ++i) {
            createAndExecProcess(NULL, (uint64_t) proc2, getCurrentProc()->pid, FALSE);
        }
        c = getNextChar();
        quantum+=INCREMENT;
        setQuantum(quantum);
    }
    setQuantum(initQuantum);
    return TRUE;
}
*/

int proc1()
{
    simple_printf("Hola!! soy %s y estoy creando hijos\n", getCurrentProc()->name);
    createAndExecProcess(NULL, (uint64_t) proc1, getCurrentProc()->pid, FALSE);
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
    createAndExecProcess(name, (uint64_t) proc1, getCurrentProc()->pid, FALSE);
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
    if (createAndExecProcess("f1", (uint64_t) f1, getCurrentProc()->pid, FALSE) == PID_ERROR)
    {
        simple_printf("f1: ERROR: otro == NULL\n");
        return;
    }
    if (createAndExecProcess("f2", (uint64_t) f2, getCurrentProc()->pid, FALSE) == PID_ERROR)
    {
        simple_printf("f2: ERROR: otro == NULL\n");
        return;
    }
    if (createAndExecProcess("f3", (uint64_t) f3, getCurrentProc()->pid, FALSE) == PID_ERROR)
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

pPid meuPid = 0;

static void m1()
{
    char * aux = "!";
    int i;
    for (i = 0; i < 70; i++)
    {
        simple_printf("m1 kernel msg: %s\n", aux);
        sendMessage(meuPid, aux, NULL ,1);
        aux[0]++;
    }
    simple_printf("termino m1!!\n");
}

static void m2()
{
    int j = 0;
    char * msg;
    while(j < 140)
    {
        msg = NULL;
        receiveMessage(&msg, NULL, 1);
        simple_printf("m2: %s\n", msg);
        j++;
    }
}

static void m3()
{
    char * aux = "!!";
    int i;
    for (i = 0; i < 70; i++)
    {
        simple_printf("m3 kernel msg: %s\n", aux);
        sendMessage(meuPid, aux, NULL ,1);
        aux[0]++;
    }
    simple_printf("termino m1!!\n");
}

void messageTest()
{
    meuPid = createAndExecProcess("m2", (uint64_t) m2, getCurrentProc()->pid, FALSE);
    if (meuPid == PID_ERROR)
    {
        simple_printf("m2: ERROR: otro == NULL\n");
        return;
    }

    if (createAndExecProcess("m1", (uint64_t) m1, getCurrentProc()->pid, FALSE) == PID_ERROR)
    {
        simple_printf("m1: ERROR: otro == NULL\n");
        return;
    }
    if (!createAndExecProcess("m3", (uint64_t) m3, getCurrentProc()->pid, FALSE) == PID_ERROR)
    {
        simple_printf("m3: ERROR: otro == NULL\n");
        return;
    }


}
