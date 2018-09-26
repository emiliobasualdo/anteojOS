#ifndef BUDDYSYSTEM_H
#define BUDDYSYSTEM_H

#include "binaryTree.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "mathLib.h"
#include <videoDriver.h>
#include <stdbool.h>
#include <system.h>

#define FREE 0
#define OCCUPIED 1
#define SPLIT 2

typedef struct buddySystemCDT
{
    uint64_t offset;               /** indica la posicion de memoria donde se ubica el bloque de memoria */
    uint64_t memSize;              /** indica la cantidad de memoria manejada */
    freeFn free;                  /** funcion para liberar memoria */
    allocFn alloc;                 /** funcion para alocar memoria */
    binaryTree root;               /** nodo raiz del arbol del buddy system */
} buddySystemCDT;

typedef buddySystemCDT * buddySystem;

typedef struct memorySlotCDT
{
    unsigned state;                   /** indica el estado del nodo, SPLIT, FREE, OCCUPIED */
    uint64_t memSize;                 /** indica la cantidad de memoria usada dentro del slot*/
    uint64_t slotSize;               /** indica la cantidad de memoria manejada */
} memorySlotCDT;

typedef memorySlotCDT * memorySlot;

buddySystem startBuddySystem (uint64_t totalMemory, freeFn free, allocFn alloc, uint64_t offset);
uint64_t allocateMemory(binaryTree bTree, uint64_t sizeToAdd, allocFn alloc, uint64_t offset);
int8_t freeMemory(binaryTree bTree, uint64_t offset, uint64_t currOffset, uint64_t memToFreeOffset, freeFn free);
void endBuddySystem(buddySystem buddy);
uint64_t calculateInternalFragmentation(buddySystem buddy);

#endif
