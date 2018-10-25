#include "philosophers.h"

int state[N];                          /** arreglo para seguir el estado de todos los comensales */

int mutex;                           /** mutex para regiones críticas */

int S[N];                           /** un semáforo por filósofo */

int procPid[N];                   /** arreglo con los PID de los filósofos */

int qtyPhilosophers;                /** cantidad de filosofos (menor o igual a N) */

int runningState[N];                /** estado de ejecucion de los filosofos: 1 para que corran, 0 para que dejen de correr */

void eat();
void think();
void sleepPhil();
int getCurrentPhil(int pid);

/** phnum: index del filósofo, de 0 a N-1 */
void test(int phnum)
{
    if (state[phnum] == HUNGRY && state[(LEFT==-1)?qtyPhilosophers-1:LEFT] != EATING && state[(RIGHT == qtyPhilosophers)?0:RIGHT] != EATING) {

        state[phnum] = EATING;

        drawDiningTable(state, qtyPhilosophers);

        semPost(S[phnum]);
    }
}

/** philosopher phnum takes up forks */
void takeFork(int phnum)
{
    lock(mutex);                                         /** entrar en la región crítica */

    state[phnum] = HUNGRY;                              /** registrar que el filósofo tiene hambre */

    drawDiningTable(state, qtyPhilosophers);

    /** eat if neighbours are not eating */
    test(phnum);                                        /** tratar de adquirir dos tenedores */

    unlock(mutex);                                  /** salir de la región crítica */

    /** if unable to eat wait to be signalled */
    semWait(S[phnum]);                               /** bloquearse si no se adquirieron los tenedores */
    while (state[phnum] == HUNGRY){}
}

/** philosopher phnum puts down forks */
void putFork(int phnum)
{
    lock(mutex);                                  /** entrar en la región crítica */

    state[phnum] = THINKING;                            /** el filósofo terminó de comer */

    drawDiningTable(state, qtyPhilosophers);

    test((LEFT==-1)?qtyPhilosophers-1:LEFT);            /** ver si el vecino izquierdo ahora puede comer */
    test((RIGHT == qtyPhilosophers)?0:RIGHT);           /** ver si el vecino derecho ahora puede comer */

    unlock(mutex);                                     /** salir de la región crítica */
}

void philosopher ()
{
    int i = getCurrentPhil(userGetCurrentPid());
    while (runningState[i])
    {
        think();
        takeFork(i);
        eat();
        putFork(i);
    }
}

int startPhilosophers (int num)
{
    int i;
    char name[10];
    qtyPhilosophers = num;

    mutex = newMutex(0);
    mutexToAdd = newMutex(1);

    for (i = 0; i < qtyPhilosophers; i++)
    {
        S[i] = semStart(0);
        state[i] = THINKING;
        runningState[i] = 1;
    }

    drawDiningTableInit();

    drawDiningTable(state, qtyPhilosophers);

    for (i = 0; i < qtyPhilosophers; i++)
    {
        userSprintf(name,"%d-%s", i, NAME);
        procPid[i] = createProc(name, (uint64_t) philosopher, NULL, 0);
        pipesToStds(procPid[i], 2);
        startProc(procPid[i]);
    }

    char c;
    while((c = (char) getChar()) != 'q')
    {
        switch (c)
        {
            case '1':
                if (qtyPhilosophers != N)
                {
                    while (state[0] != THINKING && state[qtyPhilosophers-1] != THINKING)
                    {}
                    if (state[0] == THINKING && state[qtyPhilosophers-1] == THINKING)
                    {
                        char newPhilName[10];
                        userSprintf(newPhilName,"%d-%s", i, NAME);
                        S[qtyPhilosophers] = semStart(0);
                        procPid[qtyPhilosophers] = createProc(newPhilName, (uint64_t) philosopher, NULL, 0);
                        state[qtyPhilosophers] = THINKING;
                        runningState[qtyPhilosophers] = 1;
                        pipesToStds(procPid[qtyPhilosophers], 2);
                        qtyPhilosophers++;
                        startProc(procPid[qtyPhilosophers-1]);
                        drawDiningTable(state, qtyPhilosophers);
                    }
                }
                break;
            case '0':
                if (qtyPhilosophers != 2)
                {
                    while (state[qtyPhilosophers-1] != THINKING && state[0] != THINKING)
                    {}
                    if (state[qtyPhilosophers-1] == THINKING && state[0] == THINKING)
                    {
                        qtyPhilosophers--;
                        runningState[qtyPhilosophers] = 0;
                        clearPlate(qtyPhilosophers);
                        drawDiningTable(state, qtyPhilosophers);
                    }
                }
                break;
            default:
                break;
        }
    }

    userKill(procPid[0], procPid[qtyPhilosophers-1]);
    newWindow();
    return 1;
}

void think()
{
    sleepPhil();
}

void eat()
{
    sleepPhil();
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
