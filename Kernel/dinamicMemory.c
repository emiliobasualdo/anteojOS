#include "dinamicMemory.h"

static uint64_t getKernelPages(size_t size);

void * kernelMalloc (size_t size)
{
    /*uint64_t sizeKernelPages = getKernelPages(size)*PAGE_SIZE_KERNEL;
    return alloc(sizeKernelPages);*/
    return alloc(size);
}

/*static uint64_t getKernelPages(size_t size)
{
    return (size / PAGE_SIZE_KERNEL) + 1;
}*/

void kernelFree (void * address)
{
    dealloc(address);
}