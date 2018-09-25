//
// Created by Emilio Basualdo on 9/24/18.
//

#include "shellTests.h"

#define MOD 100000000
#define PROC_COUNT 20


int proc()
{
    simple_printf("Hola!! soy %s y estoy creando hijos\n", getCurrentProc()->name);
    createAndExecProcess(NULL, (uint64_t)proc, getCurrentProc()-> pid, FALSE);
    proc();
    return -1;
}

boolean processBomb()
{
    char name[MAX_PROC_NAME];
    simple_printf("We will run a pseudo process-bomb.\n");
    simple_printf("Basicly we will create and execute %d process and see what happen.\n", PROC_COUNT);
    simple_printf("The process's code is:\n");
    simple_printf("int proc()\n"
                  "{\n"
                  "    simple_printf(\"Hola!! soy %s y estoy creando hijos\\n\", getCurrentProc()->name);\n"
                  "    createAndExecProcess(NULL, (uint64_t)proc, getCurrentProc()-> pid, FALSE);\n"
                  "    proc();\n"
                  "    return -1;\n"
                  "}");
    simple_printf("\nPress any key to start\n");
    getNextChar();
    simple_sprintf(name,"%sc","process_bomb");
    createAndExecProcess(name, (uint64_t) proc, getCurrentProc()->pid, FALSE);
    simple_printf("If we got here is because this OS is the best at handling process bombs ;)\n");
    return TRUE;
}

