#include "philosophers.h"

int state[N];                          /** arreglo para seguir el estado de todos los comensales */

int mutex;                           /** mutex para regiones críticas */
int S[N];                           /** un semáforo por filósofo */

int procPid[N];                   /** arreglo con los PID de los filósofos */

int eat();
void think();
void sleepPhil();
int getCurrentPhil(int pid);
int qtyPhilosophers;

#define MAX_FOOD 10000000000
#define MAX_PLATE (MAX_FOOD / 50) // va a comer 50 platos

/** phnum: index del filósofo, de 0 a N-1 */
void test(int phnum)
{
    if (state[phnum] == HUNGRY && state[(LEFT==-1)?qtyPhilosophers-1:LEFT] != EATING && state[RIGHT] != EATING) {

        state[phnum] = EATING;

        /*printF("%s takes fork %d and %d\n", getPhilName(phnum), (LEFT==-1)?qtyPhilosophers:LEFT + 1, phnum + 1);
        printF("%s is eating\n", getPhilName(phnum));*/
        drawDiningTable(state, qtyPhilosophers);

        unlock(S[phnum]);
    }
}

/** philosopher phnum takes up forks */
void takeFork(int phnum)
{
    lock(mutex);                                         /** entrar en la región crítica */

    state[phnum] = HUNGRY;                              /** registrar que el filósofo tiene hambre */
    //printF("Philosopher %d is hungry\n", phnum + 1);
    //drawDiningTable(state, qtyPhilosophers);

    /** eat if neighbours are not eating */
    test(phnum);                                        /** tratar de adquirir dos tenedores */

    unlock(mutex);                                  /** salir de la región crítica */

    /** if unable to eat wait to be signalled */
    lock(S[phnum]);                               /** bloquearse si no se adquirieron los tenedores */
}

/** philosopher phnum puts down forks */
void putFork(int phnum)
{
    lock(mutex);                                  /** entrar en la región crítica */

    state[phnum] = THINKING;                            /** el filósofo terminó de comer */
    /*printF("%s putting fork %d and %d down\n", getPhilName(phnum), (LEFT + 1) == 0 ? qtyPhilosophers : (LEFT + 1), phnum + 1);
    printF("%s is thinking\n", getPhilName(phnum), phnum + 1);*/
    drawDiningTable(state, qtyPhilosophers);

    test((LEFT==-1)?qtyPhilosophers-1:LEFT);            /** ver si el vecino izquierdo ahora puede comer */
    test(RIGHT);                                        /** ver si el vecino derecho ahora puede comer */

    unlock(mutex);                                     /** salir de la región crítica */
}

void * philosopher ()
{
    int aux = getCurrentPhil(userGetCurrentPid());
    int * i = &aux;
    int hungry = TRUE;
    while (hungry)
    {
        think();
        takeFork(*i);
        hungry = eat();
        putFork(*i);
    }
    return 0;
}

int startPhilosophers (int num)
{
    int i;
    char name[10];
    qtyPhilosophers = num;

    mutex = newMutex(0);

    for (i = 0; i < qtyPhilosophers; i++)
    {
        S[i] = newMutex(1);
        state[i] = THINKING;
    }

    drawDiningTableInit();
    drawDiningTable(state, qtyPhilosophers);

    for (i = 0; i < qtyPhilosophers; i++)
    {
        userSprintf(name,"%d-%s", i, NAME);
        procPid[i] = userStartProcess(name, (uint64_t) philosopher, NULL, 0);
        //printF("%s is thinking, with PID: %d\n", getPhilName(i), procPid[i]);
    }

    char c;
    while((c = (char) getChar()) != 'q')
    {
        switch (c)
        {
            case '1':
                if (qtyPhilosophers == N)
                {
                    //printF("ERROR: you've reached the maximum quantity of philosophers\n");
                }
                else
                {
                    char newPhilName[10];
                    userSprintf(newPhilName,"%d-%s", i, NAME);
                    S[qtyPhilosophers] = newMutex(0);
                    procPid[qtyPhilosophers] = userStartProcess(newPhilName, (uint64_t) philosopher, NULL, 0);
                    //printF("%s is thinking, with PID: %d\n", getPhilName(i), procPid[i]);
                    state[qtyPhilosophers] = THINKING;
                    qtyPhilosophers++;
                    drawDiningTable(state, qtyPhilosophers);
                }
                break;
            case '0':
                if (qtyPhilosophers == 2)
                {
                    //printF("ERROR: 2 philosophers are needed to perform the test\n");
                }
                else
                {
                    userKill(procPid[qtyPhilosophers-1], procPid[qtyPhilosophers-1]);
                    qtyPhilosophers--;
                    clearPlate(qtyPhilosophers);
                    drawDiningTable(state, qtyPhilosophers);
                }
                break;
            default:break;
        }
    }

    userKill(procPid[0], procPid[qtyPhilosophers-1]);
    //userKill(userGetCurrentPid(), userGetCurrentPid());
    newWindow();
    return 1;
}

void think()
{
    sleepPhil();
}

int eat()
{
    static long long counter = 0;
    while(counter < MAX_FOOD)
    {
        counter++;
        if(counter % MAX_PLATE == 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

void sleepPhil()
{
    for (int i=0; i<10000; i++)
    {
        for (int j=0; j<10000; j++)
        {}
    }
}

int getCurrentPhil(int pid)
{
    for (int i=0; i < qtyPhilosophers; i++)
    {
        if (procPid[i] == pid)
        {
            return i;
        }
    }
    return -1;
}


/** dibuja la mesa con los comensales --> qtyPhil = {0, ... , N=5} --> depende de N
void drawDiningTable(state, qtyPhilosophers)
{
    char * states[N];                          // N es la máxima cantidad de filósofos
    for (int i=0; i<qtyPhilosophers; i++)
    {
        states[i] = (state[i] == EATING ? "is eating  " : (state[i] == HUNGRY ? "is hungry  " : "is thinking"));
    }

    switch (qtyPhilosophers)
    {
        case 2:
            printF("\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "                                     ==========================\n"
                   "                                    (     %s %s     )\n"
                   "                                   (                            )\n"
                   "                                  ( %s %s         )\n"
                   "                                 (                                )\n"
                   "                                (                                  )\n"
                   "                                |                                  |\n"
                   "                                (                                  )\n"
                   "                                 (                                )\n"
                   "                                  (                              )\n"
                   "                                   (                            )\n"
                   "                                    (                          )\n"
                   "                                     ==========================\n"
                   "\n", getPhilName(0), states[0], getPhilName(1), states[1]);
            break;
        case 3:
            printF("\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "                                     ==========================\n"
                   "                                    (     %s %s     )\n"
                   "                                   (                            )\n"
                   "                                  ( %s %s         )\n"
                   "                                 (                                )\n"
                   "                                (           %s %s  )\n"
                   "                                |                                  |\n"
                   "                                (                                  )\n"
                   "                                 (                                )\n"
                   "                                  (                              )\n"
                   "                                   (                            )\n"
                   "                                    (                          )\n"
                   "                                     ==========================\n"
                   "\n", getPhilName(0), states[0], getPhilName(1), states[1], getPhilName(2), states[2]);
            break;
        case 4:
            printF("\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "                                     ==========================\n"
                   "                                    (     %s %s     )\n"
                   "                                   (                            )\n"
                   "                                  ( %s %s         )\n"
                   "                                 (                                )\n"
                   "                                (           %s %s  )\n"
                   "                                |                                  |\n"
                   "                                (                                  )\n"
                   "                                 (   %s %s           )\n"
                   "                                  (                              )\n"
                   "                                   (                            )\n"
                   "                                    (                          )\n"
                   "                                     ==========================\n"
                   "\n", getPhilName(0), states[0], getPhilName(1), states[1], getPhilName(2), states[2], getPhilName(3), states[3]);
            break;
        case 5:
            printF("\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "\n"
                   "                                     ==========================\n"
                   "                                    (     %s %s     )\n"
                   "                                   (                            )\n"
                   "                                  ( %s %s         )\n"
                   "                                 (                                )\n"
                   "                                (           %s %s  )\n"
                   "                                |                                  |\n"
                   "                                (                                  )\n"
                   "                                 (   %s %s           )\n"
                   "                                  (                              )\n"
                   "                                   (      %s %s )\n"
                   "                                    (                          )\n"
                   "                                     ==========================\n"
                   "\n", getPhilName(0), states[0], getPhilName(1), states[1], getPhilName(2), states[2], getPhilName(3), states[3], getPhilName(4), states[4]);
            break;
        default: break;
    }
}*/

char * getPhilName(int index)
{
    switch(index)
    {
        case 0: return "Kant";
        case 1: return "Socrates";
        case 2: return "Pitagoras";
        case 3: return "Platon";
        case 4: return "Descartes";
    }
    return NULL;
}