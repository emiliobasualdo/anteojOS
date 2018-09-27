#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdint.h>
#include <stddef.h>
#include "memoryManager.h"
#include "printf.h"
#include "stdbool.h"
#include "system.h"

#define PAGE_SIZE_KERNEL 256
#define SCM_DATA_ADDRESS 0x600000
/** 0x500000 + 0x100000 ---> 0x500000 se obtuvo de kernel.c y 0x100000 es la cantidad de espacio que le decidimos dar al SCM */

void * allocForMemManager(size_t size);
void freeForMemManager(void * offset);
void dealloc (void * address);
void * alloc(size_t size);
boolean initializeAllocator();
void drawAllocator();
void endPageAllocator();

#endif