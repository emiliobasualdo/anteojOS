#include "dinamicMemory.h"

void * kernelMalloc (size_t size)
{
    return alloc(size);
}

void kernelFree (void * address)
{
    dealloc(address);
}
