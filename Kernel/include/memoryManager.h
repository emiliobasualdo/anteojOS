#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "buddySystem.h"
#include <stddef.h>
#include "videoDriver.h"
#include "printf.h"

#define N_BUDDIES 8

typedef struct memoryManagerCDT
{
    buddySystem * buddies;          /** array con los buddies manejados por el manager */
    allocFn alloc;                  /** funcion para alocar la memoria */
    freeFn free;                   /** funcion para liberar memoria */
    uint64_t numBuddies;            /** tamaño del array de buddies */
} memoryManagerCDT;

typedef memoryManagerCDT * memoryManager;

memoryManager startMemoryManager(uint64_t offset, uint64_t totalMemory, allocFn alloc, freeFn free);
uint64_t requestPages(memoryManager memManager, uint64_t memSize);
void freePages(memoryManager memManager, uint64_t address);
void endMemoryManager(memoryManager memManager);
void drawMemoryManager(memoryManager memManager);

#endif