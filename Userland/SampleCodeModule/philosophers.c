//
// Created by Emilio Basualdo on 10/19/18.
//


#include <stdint.h>
#include "philosophers.h"

#define N 5
#define MAX 40
#define THINKING 2
#define HUNGRY 1
#define EATING 0
#define LEFT (phnum + 4) % N
#define RIGHT (phnum + 1) % N
#define NAME "PHIL"

int state[MAX];
int phil[MAX];

int mutex;
int S[N];

void initArray();

void test(int phnum)
{
    if (state[phnum] == HUNGRY
        && state[LEFT] != EATING
        && state[RIGHT] != EATING) {
        // state that eating
        state[phnum] = EATING;

        sleep();
        sleep();

        printF("Philosopher %d takes fork %d and %d\n",phnum + 1, LEFT + 1, phnum + 1);

        printF("Philosopher %d is Eating\n", phnum + 1);

        // sem_post(&S[phnum]) has no effect
        // during takefork
        // used to wake up hungry philosophers
        // during putfork
        //sem_post(&S[phnum]);
    }
}

// take up chopsticks
void take_fork(int phnum) {

    //sem_wait(&mutex);

    // state that hungry
    state[phnum] = HUNGRY;

    //printf("Philosopher %d is Hungry\n", phnum + 1);

    // eat if neighbours are not eating
    test(phnum);

    //sem_post(&mutex);

    // if unable to eat wait to be signalled
    //sem_wait(&S[phnum]);

    sleep(1);
}

// put down chopsticks
void put_fork(int phnum) {

    //sem_wait(&mutex);

    // state that thinking
    state[phnum] = THINKING;

    //printf("Philosopher %d putting fork %d and %d down\n",
           //phnum + 1, LEFT + 1, phnum + 1);
    //printf("Philosopher %d is thinking\n", phnum + 1);

    test(LEFT);
    test(RIGHT);

    //sem_post(&mutex);
}

void *philospher(void *num) {

    while (1) {

        int *i = num;

        sleep(1);

        take_fork(*i);

        sleep(0);

        put_fork(*i);
    }
}

int startPhilosophers()
{

    int i;
    initArray();
    int procPid[N];
    char name[10];

    // initialize the semaphores
    mutex = newMutex(1); //sem_init(&mutex, 0, 1);

    for (i = 0; i < N; i++)
    {
        S[i] = newMutex(0);//sem_init(&S[i], 0, 0);
    }

    for (i = 0; i < N; i++)
    {
        // create philosopher processes
        userSprintf(name,"%d-%s", i, NAME);
        procPid[i] = userStartProcess(name, (uint64_t)philospher, FALSE);
        printF("Philosopher %d is thinking\n", i + 1);
    }

    char c;
    while((c = getChar()) != 'q')
    {
       // aca metemos codigo para gregar o restar philosofos
    }

    userKillAllDescendants(userGetCurrentPid());
    return 1;
}

void initArray()
{
    for (int i = 0; i < MAX; ++i)
    {
        phil[i] = i;
    }
}
