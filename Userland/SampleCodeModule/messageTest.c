#include "messageTest.h"

int myPid = 3;
static int runningProcess = 0;
static int mutex;

char * array[] = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w",
"x","y","z",  "la", "le", "li", "lo", "lu", "Lorem", "ipsum", "dolor", "sit", "amet", "consectetur", "adipisicing", "elit", "sed", "do", "eiusmod", "tempor",
 "incididunt", "ut", "labore", "et", "dolore", "magna", "aliqua."," Ut", "enim", "ad", "minim", "veniam", "quis", "nostrud", "exercitation",
  "ullamco", "laboris", "nisi"};//, "ut", "aliquip", "ex", "ea", "commodo",
//  "consequat", "Duis", "aute", "irure"};

static void wait()
{
    for (int i = 0; i < 5000; i++) {
        for (int j = 0; j < 500; j++) {
            for (int k = 0; k < 50; k++) {
                /* code */
            }
        }
    }
}

static void m1()
{
    int i;
    for (i = 0; i < 61; i++)
    {
        printF("m1 kernel msg: %s %d\n", array[i], myPid);
        send(myPid, array[i], NULL ,1);
        wait();
    }
    printF("termino m1!!\n");
    runningProcess--;
}

static void m2()
{
    int j = 0;
    char * msg;
    while(j < 122)
    {
        msg = NULL;
        //printF("num:  %d\n", j);
        receive(&msg);
        wait();
        printF("m2: %s\n", msg);
        j++;

    }
    printF("termino m2!!\n");
    runningProcess--;
}

static void m3()
{
    int i;
    for (i = 0; i < 61; i++)
    {
        printF("m3 kernel msg: %s %d\n", array[i] , myPid);
        send(myPid, array[i], NULL ,1);
        wait();
    }
    printF("termino m3!!\n");
    runningProcess--;
}

void messageTest()
{
    myPid = userStartProcess("m2", (uint64_t) m2, userGetCurrentPid());
    mutex = newMutex();
    printF("meu Pid: %d      \n", myPid);
    if (myPid == -1)
    {
        printF("m2: ERROR: otro == NULL\n");
        runningProcess = 0;
        return;
    }
    runningProcess++;

    if (userStartProcess("m1", (uint64_t) m1, userGetCurrentPid()) == -1)
    {
        printF("m1: ERROR: otro == NULL\n");
        runningProcess = 0;
        return;
    }
    runningProcess++;
    if (userStartProcess("m3", (uint64_t) m3, userGetCurrentPid()) == -1)
    {
        printF("m3: ERROR: otro == NULL\n");
        runningProcess = 0;
        return;
    }
    runningProcess++;
    while(runningProcess > 0)
    {

    }
    printF("Termino messageTesting!\n");
    return;
}
