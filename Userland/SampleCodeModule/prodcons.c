#include "prodcons.h"


static void producer();
static void consumer();
static int createProducers(int numProducers);
static int createConsumers(int numConsumers);
static void decreaseConsumers();
static void decreaseProducers();
static void killEveryone();
static void printAnteojOS();
static char createAnteojOS();
static void waitToPrint();

//static anteojOS buffer[BUFFER_SIZE_MAX];
static char buffer[BUFFER_SIZE_MAX];
static int amountOfProductsOnHold = 0;
static int run = true;
static int mutex;
static int cons[MAXPROC];
static int numberOfCons = 0;
static int numberOfProds = 0;
static int prod[MAXPROC];

int producerConsumer(int numProducers, int numConsumers)
{
    mutex = newMutex(0);

    int aux = createProducers(numProducers);
    if (aux == 0)
    {
        return aux;
    }
    aux = createConsumers(numConsumers);
    if (aux == 0)
    {
        return aux;
    }

    while (run)//TODO imprimir error si llego al limite
    {
        int c = getChar();
        switch (c)
        {
            case '1':
                if(numberOfProds < MAXPROC)
                {
                    int pid = userStartProcess("producer", (uint64_t) producer, userGetCurrentPid());
                    prod[numberOfProds] = pid;
                    numberOfProds++;
                    printF("cantidad de prods: %d , pid: %d\n", numberOfProds, pid);
                }
                break;
            case '2':
                if(numberOfCons < MAXPROC)
                {
                    int pid = userStartProcess("consumer", (uint64_t) consumer, userGetCurrentPid());
                    cons[numberOfCons] = pid;
                    numberOfCons++;
                    printF("cantidad de cons: %d , pid: %d\n", numberOfCons, pid);
                }
                break;
            case '3':
                decreaseProducers();
                break;
            case '4':
                decreaseConsumers();
                break;
            case 'q':
                run = false;
                break;
            default: break;
        }
        c = 0;
    }

    killEveryone();
    destroyMutex(mutex);

    return 1;
}

static void producer()
{
    while(run)
    {
        //printF("prod, %d\n", userGetCurrentPid());
        lock(mutex);
        //printF("prod, %d\n", userGetCurrentPid());
        //send para deslockear
        int value = rand() % 2 + 1;
        while(value > 0)
        {
            if (amountOfProductsOnHold < BUFFER_SIZE_MAX)
            {
                buffer[amountOfProductsOnHold] = createAnteojOS();
                amountOfProductsOnHold++;
            }
            value--;
        }
        printAnteojOS();
        unlock(mutex);
    }
}

static void consumer()
{
    while(run)
    {
        //printF("cons, %d\n", userGetCurrentPid());
        //receive para lockear
        lock(mutex);
        int value = rand() % 2 + 1;
        while(value > 0)
        {
            if (amountOfProductsOnHold > 0)
            {
                buffer[amountOfProductsOnHold] = 0;

                amountOfProductsOnHold--;
            }
            value--;
        }
        printAnteojOS();
        unlock(mutex);
    }
}

static int createProducers(int numProducers)
{
    int currPID;
    int prodsCreated=(numProducers>MAXPROC)?MAXPROC:numProducers;
    for (int i=0; i < prodsCreated; i++)
    {
        currPID = userStartProcess("producer", (uint64_t) producer, userGetCurrentPid());
        if (currPID == -1)          // todo chequear
        {
            return 0;
        }
        prod[numberOfProds++] = currPID;
    }
    return 1;
}

static int createConsumers(int numConsumers)
{
    int currPID = 0;
    int consCreated=(numConsumers>MAXPROC)?MAXPROC:numConsumers;

    for (int i=0; i<consCreated; i++)
    {
        currPID = userStartProcess("consumer", (uint64_t) consumer, userGetCurrentPid());
        if (currPID == -1)          // todo chequear
        {
            return 0;
        }
        cons[numberOfCons++] = currPID;
    }
    return 1;
}

// TODO repito codigo con los kills

static void decreaseConsumers()
{
    if (numberOfCons == 0)
    {
        return;
    }
    userKill(cons[numberOfCons],cons[numberOfCons]);
    cons[numberOfCons--] = 0;
}

static void decreaseProducers()
{
    if (numberOfProds == 0)
    {
        return;
    }
    userKill(prod[numberOfProds],prod[numberOfProds]);
    prod[numberOfProds--] = 0;
}

static void killEveryone()
{
    while(numberOfProds)
    {
        decreaseProducers();
    }
    while (numberOfCons)
    {
        decreaseConsumers();
    }
}

static char createAnteojOS()
{
    return 'a';
}
static void waitToPrint()
{
    for (int i = 0; i < 5000; i++) {
        for (int j = 0; j < 500; j++) {
            for (int k = 0; k < 50; k++) {
                /* code */
            }
        }
    }
}

static void printAnteojOS()
{
    waitToPrint();

    for (int i=0; i<amountOfProductsOnHold; i++)
    {
        printF("%c ", buffer[i]);
    }
    printF("\n");
}
//
// static anteojOS createAnteojOS()
// {
//     anteojOS ret = userMalloc(sizeof(anteojosCDT));
//     ret->anteojos1 = ANTEOJOS1;
//     ret->anteojos2 = ANTEOJOS2;
//     ret->anteojos3 = ANTEOJOS3;
//     ret->anteojos4 = ANTEOJOS4;
//     ret->anteojos5 = ANTEOJOS5;
//     ret->anteojos6 = ANTEOJOS6;
//     ret->anteojos7 = ANTEOJOS7;
//     ret->anteojos8 = ANTEOJOS8;
//     ret->anteojos9 = ANTEOJOS9;
//     return ret;
// }
//
// static void printAnteojOS()
// {
//     anteojOS currAnteojOS;
//
//     for (int i=0; i<amountOfProductsOnHold; i++)
//     {
//         currAnteojOS = buffer[i];
//         simple_printf("%s\n", currAnteojOS->anteojos1);
//         simple_printf("%s\n", currAnteojOS->anteojos2);
//         simple_printf("%s\n", currAnteojOS->anteojos3);
//         simple_printf("%s\n", currAnteojOS->anteojos4);
//         simple_printf("%s\n", currAnteojOS->anteojos5);
//         simple_printf("%s\n", currAnteojOS->anteojos6);
//         simple_printf("%s\n", currAnteojOS->anteojos7);
//         simple_printf("%s\n", currAnteojOS->anteojos8);
//         simple_printf("%s\n", currAnteojOS->anteojos9);
//     }
// }
