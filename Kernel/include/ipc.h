#ifndef IPC_H
#define IPC_H


#include <stddef.h>
#include "process.h"
#include "ipcStructs.h"
#include "scheduler.h"
#include "printf.h"
#include "system.h"
#include "lib.h"
#include "dinamicMemory.h"

#define MAXMUTEXES 20
#define MAXINQUEUE 20

#define SND 1
#define REC 0



/**
 * Metodos Mutex y Semaforos
 */
extern int cmpandSwap(int * mutex);
extern int unlockMutexASM(int * mutex);
extern int lockMutexASM(int * mutex);


//Disponibiliza un id de Mutex / Sem
int startMutex(int initValue);
int semStart(int initValue);

//Inicializa los mutex
int initIPCS();
int lockMutex(int mutex);
int unlockMutex(int mutex);

// Semaforos
int semWait(int sem);
int semPost(int sem);

//destruye el mutex si no tiene procesos en la cola
int destroyMutexK(int mutex);
int semDestroyK(int sem);

/**
 * Metodos para crear nuevo mensaje y mandarlo
 */

//void createNewMessage(messageQueue* queue, uint64_t pidSender, uint64_t pidReceiver, char * messageBody);
uint64_t sendMessage(pPid receiver, char * content, char ** answer, boolean flag);
uint64_t receiveMessage(char ** message, void (*function)(char*), boolean flag);
#endif
