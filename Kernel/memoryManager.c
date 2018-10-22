#include "memoryManager.h"

static void startBuddies(memoryManager memManager, allocFn  alloc, freeFn free, uint64_t offset, uint64_t totalMemory);
static void drawTree(binaryTree bTree);

/** inicializa y aloca todos los recursos del memory manager */
memoryManager startMemoryManager(uint64_t offset, uint64_t totalMemory, allocFn alloc, freeFn free)
{
    if (alloc == NULL || free == NULL || offset == (uint64_t )NULL || totalMemory == 0)
    {
        return NULL;
    }

    memoryManager newMemManager = alloc(sizeof(memoryManagerCDT));
    newMemManager->free=free;
    newMemManager->alloc=alloc;
    startBuddies(newMemManager, alloc, free, offset, totalMemory);
    return newMemManager;
}

/** inicializa los buddies con la memoria disponible para cada uno
 * la memoria que "sobra" por truncar @totalMemory a potencia de 2 sera utilizada por otros buddies
 * mientras haya memoria disponible */
static void startBuddies(memoryManager memManager, allocFn  alloc, freeFn free, uint64_t offset, uint64_t totalMemory)
{
    memManager->buddies = alloc(N_BUDDIES* sizeof(buddySystem));
    uint64_t offsetAcum = offset;
    uint64_t remainingSize = totalMemory;
    uint64_t numOfBuddies = 0;
    uint64_t powerOf2 = 0;

    for (int i=0; i<N_BUDDIES && remainingSize>0; i++)
    {
        memManager->buddies[i] = startBuddySystem(remainingSize, free, alloc, offsetAcum);
        powerOf2 = nextLowerPowerOf2(remainingSize);
        remainingSize -= powerOf2;
        offsetAcum += powerOf2;
        numOfBuddies++;
    }

    memManager->numBuddies=numOfBuddies;
}

/** función que dibuja un árbol --> se utiliza sólo para debbuggear o para testeos */
static void drawTree(binaryTree bTree)
{
    if(bTree->left==NULL && bTree->right==NULL)
    {
        memorySlot mem = bTree->data;
        simple_printf("I'm a leaf! I occupy %d in a slot of %d", mem->memSize, mem->slotSize);
        switch (mem->state)
        {
            case OCCUPIED: simple_printf(" -----> I'm OCCUPIED\n");
                break;
            case FREE: simple_printf(" -----> I'm FREE\n");
                break;
            default: break;
        }
    }
    else
    {
        drawTree(bTree->right);
        drawTree(bTree->left);
    }
}

/** funcion que retorna las direcciones a las paginas donde se aloca la memoria de tamaño memSize */
uint64_t requestPages(memoryManager memManager, uint64_t memSize)
{
    uint64_t buddySize, ret = 0;
    for (int i= (int) (memManager->numBuddies - 1); i >= 0; i--)
    {
        buddySize = memManager->buddies[i]->memSize;
        if (buddySize >= memSize)
        {
            ret = allocateMemory(memManager->buddies[i]->root, memSize, memManager->alloc, memManager->buddies[i]->offset);
            if (ret != (uint64_t) NULL)
            {
                return ret;
            }
        }
    }

    return ret;
}

/** funcion que recibe el offset del espacio de memoria que se quiere liberar
 * elige el buddy que maneja esa direccion de memoria chequeando que @address se encuentre
 * entre el offset del buddy y el memSize del buddy
 */
void freePages(memoryManager memManager, uint64_t address)
{
    for (int i=0; i<memManager->numBuddies; i++)
    {
        uint64_t index = address - memManager->buddies[i]->offset;
        if (index < memManager->buddies[i]->memSize)
        {
            freeMemory(memManager->buddies[i]->root, memManager->buddies[i]->offset, memManager->buddies[i]->offset, address, memManager->free);
            return;
        }
    }
}

/** dibuja todos los arboles de los buddies del memory manager */
void drawMemoryManager(memoryManager memManager)
{
    for (int i=0; i<memManager->numBuddies; i++)
    {
        simple_printf("\nI'm buddy #%d, I handle %d bytes\nLook at my tree:\n", i+1, memManager->buddies[i]->memSize);
        drawTree(memManager->buddies[i]->root);
        simple_printf("\n");
    }
}

/** libera todos los recursos del memory manager */
void endMemoryManager(memoryManager memManager)
{
    for (int i=0; i<N_BUDDIES; i++)
    {
        memManager->free(memManager->buddies[i]);
    }
    memManager->free(memManager);
}

/**
 *    OOOO                       OOOO
 *       OO                     OO
 *        OO                   OO
 *         OOOOOOOO     OOOOOOOO
 *        OO       OOOOO        OO
 *       OO        OOOOO         OO
 *       OO        OO OO         OO
 *        OO      OO   OO       OO
 *         OOOOOOOO      OOOOOOOO
 */
