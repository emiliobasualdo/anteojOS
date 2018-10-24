#include "messageTest.h"

int myPid = 3;
static int runningProcess = 0;
static int mutex;

char * array[] = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w",
"x","y","z",  "la", "le", "li", "lo", "lu", "Lorem", "ipsum", "dolor", "sit", "amet", "consectetur", "adipisicing", "elit", "sed", "do", "eiusmod", "tempor",
 "incididunt", "ut", "labore", "et", "dolore", "magna", "aliqua."," Ut", "enim", "ad", "minim", "veniam", "quis", "nostrud", "exercitation",
  "ullamco", "laboris", "nisi"};

static void wait()
{
    for (int i = 0; i < 5000; i++)
    {
        for (int j = 0; j < 500; j++)
        {
            for (int k = 0; k < 50; k++)
            {
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
        printF("m1 is sending msg: %s\n", array[i]);
        send(myPid, array[i], NULL ,1);
        wait();
    }
    printF("m1 Finished!!\n");
    runningProcess--;
}

static void m2()
{
    int j = 0;
    char * msg;
    while(j < 122)
    {
        msg = NULL;
        receive(&msg);
        wait();
        printF("m2 received message: %s\n", msg);
        j++;

    }
    printF("m2 Finished!!\n");
    runningProcess--;
}

static void m3()
{
    int i;
    for (i = 0; i < 61; i++)
    {
        printF("m3 is sending msg: %s\n", array[i]);
        send(myPid, array[i], NULL ,1);
        wait();
    }
    printF("m3 Finished!!\n");
    runningProcess--;
}

void messageTest()
{
    myPid = userStartProcess("m2", (uint64_t) m2, NULL, 0);
    mutex = newMutex(0);
    if (myPid == -1)
    {
        printF("m2: ERROR: other == NULL\n");
        runningProcess = 0;
        return;
    }
    runningProcess++;

    if (userStartProcess("m1", (uint64_t) m1, NULL, 0) == -1)
    {
        printF("m1: ERROR: other == NULL\n");
        runningProcess = 0;
        return;
    }
    runningProcess++;
    if (userStartProcess("m3", (uint64_t) m3, NULL, 0) == -1)
    {
        printF("m3: ERROR: other == NULL\n");
        runningProcess = 0;
        return;
    }
    runningProcess++;
    while(runningProcess > 0)
    {

    }
    printF("messageTesting Finished!\n");
}
