#include "messageTest.h"

int myPid = 3;
static int runningProcess = 0;
static int mutex;
unsigned int xResM, yResM;

char * array[] = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w",
"x","y","z",  "la", "le", "li", "lo", "lu", "Lorem", "ipsum", "dolor", "sit", "amet", "consectetur", "adipisicing", "elit", "sed", "do", "eiusmod", "tempor",
 "incididunt", "ut", "labore", "et", "dolore", "magna", "aliqua."," Ut", "enim", "ad", "minim", "veniam", "quis", "nostrud", "exercitation",
  "ullamco", "laboris", "nisi"};

static void newLineMT(int pos)
{
    setCoordinates(0,pos*yResM/9);
    putChar('\n');
    setCoordinates(0,pos*yResM/9);
}

static void wait()
{
    for (int i = 0; i < 5000; i++) {
        for (int j = 0; j < 300; j++) {
            for (int k = 0; k < 50; k++) {

            }
        }
    }
}

static void m1()
{
    int i;
    for (i = 0; i < 61; i++)
    {

        send(myPid, array[i], NULL ,1);
        wait();
        newLineMT(2);
        printF("m1 is sending msg: %s\n", array[i]);
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
        receive(&msg);
        wait();
        newLineMT(5);
        printF("m2 received this message: %s\n", msg);
        j++;

    }
    printF("m2 finished!!\n");
    runningProcess--;
}

static void m3()
{
    int i;
    for (i = 0; i < 61; i++)
    {
        send(myPid, array[i], NULL ,1);
        wait();
        newLineMT(8);
        printF("m3  is sending msg: %s\n", array[i]);
    }
    printF("m3 finished!!\n");
    runningProcess--;
}

void messageTest()
{
    getResolutions(&xResM,&yResM);
    mutex = newMutex(0);
    runningProcess = 0;
    int aux;
    myPid = createProc("m2", (uint64_t) m2, NULL, 0);
    if (myPid == -1)
    {
        printF("m2: ERROR: otro == NULL\n");
        runningProcess = 0;
        return;
    }

    pipesToStds(myPid, 2);
    runningProcess++;
    startProc(myPid);

    if ((aux = createProc("m1", (uint64_t) m1, NULL, 0)) == -1)
    {
        printF("m1: ERROR: otro == NULL\n");
        runningProcess = 0;
        return;
    }

    pipesToStds(aux, 2);
    runningProcess++;
    startProc(aux);


    if ((aux = createProc("m3", (uint64_t) m3, NULL, 0)) == -1)
    {
        printF("m3: ERROR: otro == NULL\n");
        runningProcess = 0;
        return;
    }

    pipesToStds(aux, 2);
    runningProcess++;
    startProc(aux);

    newWindow();

    while(runningProcess > 0)
    {
        if(getChar() == 'q')
        {
            userKill(myPid, aux);
            wait();
            newWindow();
            return;
        }
    }
    printF("messageTesting finished successfully!\n");
}
