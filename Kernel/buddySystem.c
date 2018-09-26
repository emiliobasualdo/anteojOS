#include "buddySystem.h"

static boolean splitTree(binaryTree bTree, allocFn alloc);
static uint8_t mergeChildren (binaryTree bTree, freeFn free);
static memorySlot createMemSlot(uint64_t slotSize, uint64_t memSize, allocFn alloc);
static void endBuddySystemRec (binaryTree bTree, freeFn free);
static uint64_t calculateInternalFragmentationRec(binaryTree bTree);

/**
 * para manejar memoria nos basamos en el buddy system
 * O(log(n)), n = memoria total que maneja el buddy
 * link de youtube que explica como funciona: --> https://www.youtube.com/watch?v=1pCC6pPAtio
 */
buddySystem startBuddySystem (uint64_t totalMemory, freeFn free, allocFn alloc, uint64_t offset)
{
    if ( totalMemory == 0 || offset == NULL || alloc == NULL || free == NULL)
    {
        return NULL;
    }

    buddySystem newBSystem = alloc(sizeof(buddySystemCDT));
    if (newBSystem == NULL)
    {
        return NULL;
    }

    newBSystem->offset=offset;
    newBSystem->alloc=alloc;
    newBSystem->free=free;

    /**
     * calculo la cantidad que voy a manejar con potencias de 2
     * ejemplo: si me dicen de manejar 511 bytes, voy a manejar 256 bytes
     */
    newBSystem->memSize=nextLowerPowerOf2(totalMemory);

    memorySlot totalMemorySlot = createMemSlot(newBSystem->memSize, 0, alloc);
    if (totalMemorySlot == NULL)
    {
        return NULL;
    }

    newBSystem->root = createBTree(totalMemorySlot, alloc);

    return newBSystem;
}

/** crea una instancia de la estructura del memory slot para que sea la data del nodo del arbol */
static memorySlot createMemSlot(uint64_t slotSize, uint64_t memSize, allocFn alloc)
{
    memorySlot newMemSlot = alloc(sizeof(memorySlotCDT));
    if (newMemSlot == NULL)
    {
        return NULL;
    }
    newMemSlot->slotSize=slotSize;
    newMemSlot->memSize=memSize;
    newMemSlot->state=FREE;
    return newMemSlot;
}

/** funcion recursiva para alocar memoria
 * retorna la direccion de memoria donde se aloco
 * o NULL si no habia lugar */
uint64_t allocateMemory(binaryTree bTree, uint64_t sizeToAdd, allocFn alloc, uint64_t currOffset)
{
    memorySlot currMemorySlot = bTree->data;
    if (sizeToAdd > currMemorySlot->slotSize)
    {
        return (uint64_t ) NULL;
    }

    if (currMemorySlot->state == SPLIT)
    {
        uint64_t ret = allocateMemory(bTree->right, sizeToAdd, alloc, currOffset);
        if (ret == (uint64_t)NULL)
        {
            return allocateMemory(bTree->left, sizeToAdd, alloc, currOffset+(currMemorySlot->slotSize/2));
        }
        else
        {
            return ret;
        }
    }

    if (currMemorySlot->state == FREE)
    {
        if (currMemorySlot->slotSize/2 < sizeToAdd)
        {
            if (sizeToAdd > currMemorySlot->slotSize)
            {
                return (uint64_t) NULL;
            }
            currMemorySlot->memSize = sizeToAdd;
            currMemorySlot->state=OCCUPIED;
            return currOffset;
        }
        else
        {
            if (!splitTree(bTree, alloc))
            {
                return (uint64_t ) NULL;
            }
            return allocateMemory(bTree, sizeToAdd, alloc, currOffset);
        }
    }
    return (uint64_t ) NULL;        /** state = OCCUPIED */
}

static boolean splitTree(binaryTree bTree, allocFn alloc)
{
    memorySlot currMemSlot = bTree->data;
    currMemSlot->state=SPLIT;

    memorySlot memSlotLeft = createMemSlot(currMemSlot->slotSize / 2, 0, alloc);
    if (memSlotLeft == NULL)
    {
        return false;
    }
    binaryTree bTreeLeft = createBTree(memSlotLeft, alloc);

    memorySlot memSlotRight = createMemSlot(currMemSlot->slotSize / 2, 0, alloc);
    if (memSlotRight == NULL)
    {
        return false;
    }
    binaryTree bTreeRight = createBTree(memSlotRight, alloc);

    bTree->left = bTreeLeft;
    bTree->right = bTreeRight;
    return true;
}

int8_t freeMemory(binaryTree bTree, uint64_t offset, uint64_t currOffset, uint64_t memToFreeOffset, freeFn free)
{
    if (currOffset < offset)
    {
        return 0;
    }

    memorySlot currMemSlot = bTree->data;

    if (currOffset == memToFreeOffset && currMemSlot->state == OCCUPIED)
    {
        currMemSlot->state=FREE;
        currMemSlot->memSize=0;
        return 1;
    }
    if (currMemSlot->state == SPLIT)
    {
        int8_t freed;

        if (memToFreeOffset < ((currMemSlot->slotSize / 2) + currOffset))
        {
            freed = freeMemory(bTree->right, offset, currOffset, memToFreeOffset, free);
        }
        else
        {
            freed = freeMemory(bTree->left, offset, currOffset + (currMemSlot->slotSize/2), memToFreeOffset, free);
        }
        if (freed == 1)
        {
            return mergeChildren(bTree, free);
        }
        return freed;
    }
    return -1;      /** o esta libre o esta ocupada pero no en el offset que me pasaron */
}

static uint8_t mergeChildren (binaryTree bTree, freeFn free)
{
    memorySlot memSlotLeft = bTree->left->data;
    memorySlot memSlotRight = bTree->right->data;
    memorySlot memSlot = bTree->data;

    if (memSlotLeft->state == FREE && memSlotRight->state == FREE)
    {
        memSlot->state=FREE;
        freeBTree(bTree->left, free);       /** este free libera el memSlot tambien */
        freeBTree(bTree->right, free);
        bTree->left=NULL;
        bTree->right=NULL;
        return 1;
    }
    return 0;
}

void endBuddySystem(buddySystem buddy)
{
    if (buddy != NULL)
    {
        endBuddySystemRec(buddy->root, buddy->free);
        buddy->free(buddy);
    }
}

static void endBuddySystemRec (binaryTree bTree, freeFn free)
{
    if (bTree->left == NULL && bTree->right == NULL)
    {
        free(bTree);
        return;
    }

    endBuddySystemRec(bTree->left, free);
    endBuddySystemRec(bTree->right, free);
}

uint64_t calculateInternalFragmentation(buddySystem buddy)
{
    return calculateInternalFragmentationRec(buddy->root);
}

static uint64_t calculateInternalFragmentationRec(binaryTree bTree)
{
    memorySlot currMemSlot = bTree->data;
    if (bTree->right== NULL && bTree->left==NULL)
    {
        return currMemSlot->slotSize - currMemSlot->memSize;
    }

    return calculateInternalFragmentationRec(bTree->left) + calculateInternalFragmentationRec(bTree->right);
}