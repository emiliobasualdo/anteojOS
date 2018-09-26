#include "include/allocator.h"

extern uint8_t endOfKernel;

static uint64_t offset;
static size_t allocSize;
static uint64_t totalBlocks;
static uint64_t sizeForMemManager;
static uint64_t sizeForBlocksArray;
static memoryManager memManager;

static void setAllocatorForMemManager(uint64_t totalSizeLeft);
static void setSizeForBlocksArray();
static void setSizeForMemManager(uint64_t totalSize);
static uint64_t getNumOfNodes(uint64_t totalSize);
static uint64_t getNumOfBlocks(size_t size);
static void setAllocSize ();


/** calcula la cantidad de memoria total en anteojOS
 * calcula la cantidad de memoria que va a utilizar el memory manager y el array de bloques
 * calcula los offsets, para el page allocator y para el memory manager
 * inicializa el memory manager y el array de bloques
 *
 * el array de bloques se utiliza para el alloc y el free del memory manager
 * cada elemento representa el bloque en memoria que se aloca cuando se llama al allocForMemManager
 * si el bloque esta en 0, es porque esta libre
 * si esta en 1, es porque esta ocupado
 * luego si se quiere alocar en el bloque #7, la direccion es offset + 7 * allocSize
 */
boolean initializeAllocator()
{
    offset = (uint64_t) &endOfKernel;
    offset = max(offset, SCM_DATA_ADDRESS);
    uint64_t totalSizeLeft = (*((uint64_t *) 0x5020)) * 1024 * 1024 - offset;
    setAllocatorForMemManager(totalSizeLeft);
    uint64_t offsetForMemManager = offset + sizeForMemManager + sizeForBlocksArray ;
    memManager = startMemoryManager(offsetForMemManager, totalSizeLeft-sizeForMemManager-sizeForBlocksArray, allocForMemManager, freeForMemManager);
    return TRUE;
}

/** setea cada bloque en 0, indicando que el espacio esta libre */
static void setAllocatorForMemManager(uint64_t totalSizeLeft)
{
    setAllocSize();
    setSizeForMemManager(totalSizeLeft);
    setSizeForBlocksArray();
    for (uint64_t i = 0; i < totalBlocks; i++)
    {
        *((uint64_t *)(offset + i)) = 0;
    }
}

void * alloc (size_t size)
{
    return ((void *) requestPages(memManager, size));
}

void dealloc (void * address)
{
    freePages(memManager, (uint64_t) address);
}

/** calcula el tamaño en memoria necesario para reservar el array de bloques */
static void setSizeForBlocksArray()
{
    sizeForBlocksArray = sizeForMemManager / allocSize;
    totalBlocks = sizeForBlocksArray;
}

/** calcula el tamaño en memoria necesario para reservar el memory manager */
static void setSizeForMemManager(uint64_t totalSize)
{
    /** cantidad de estructuras que podria pedir en un alloc:
     * el numero total de nodos, N buddy systems y 1 memory manager
     */
    uint64_t structsNeeded = getNumOfNodes(totalSize) + N_BUDDIES + 1;
    sizeForMemManager = structsNeeded*allocSize;
}

/** calcula la cantidad de nodos que tendria el buddy system si maneja el tamaño @totalSize */
static uint64_t getNumOfNodes(uint64_t totalSize)
{
    uint64_t qtyPages = totalSize/PAGE_SIZE_KERNEL;
    qtyPages = nextLowerPowerOf2(qtyPages);               /** hacemos que sea potencia de 2 con el metodo del buddy system*/
    uint64_t qtyNodes = qtyPages;

    while (qtyPages > 0)
    {
        qtyPages /= 2;
        qtyNodes += qtyPages;
    }

    return qtyNodes;
}

static uint64_t getNumOfBlocks(size_t size)
{
    return size/allocSize + 1;
}

/** para poder hacer un free en el espacio en memoria designado al memory manager
 * necesito que los bloques que aloco sean constantes
 * por lo cual no voy a usar la variable size que me pasan como parametro
 * y uso allocSize */
void * allocForMemManager(size_t size)
{
    uint64_t addressAux =  offset + sizeForBlocksArray;
    uint64_t blocksOccupied = 0;
    uint64_t blocksToOccupy = getNumOfBlocks(size);
    bool available = true;

    for (uint64_t i=0; i<totalBlocks;)
    {
        blocksOccupied = *((uint8_t *) (offset + i));

        if (blocksOccupied == 0)
        {
            for (int j=0; j<blocksOccupied && available; j++)
            {
                if (*((uint8_t *) (offset + j * 8)) != 0)
                {
                    available=false;
                }
            }
            if (available)
            {
                *((uint8_t *) (offset + i)) = blocksToOccupy;
                addressAux += allocSize * i;
                return (void *) addressAux;
            }
            i++;
        }
        i += blocksOccupied;
    }
    return NULL;
}

/** encuentra el bloque que maneja la direccion @offsetBlock y setea el 0 */
void freeForMemManager(void * offsetBlock)
{
    uint64_t index = ((uint64_t ) (offsetBlock - offset - sizeForBlocksArray)) / allocSize;
    *((uint8_t *) (offset + index * 8)) = 0;
}

/** retorna el tamaño maximo entre las estructuras que maneja el memory manager */
static void setAllocSize ()
{
    allocSize = sizeof(buddySystemCDT);
}

void drawAllocator()
{
    drawMemoryManager(memManager);
}

void endPageAllocator()
{
    endMemoryManager(memManager);
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