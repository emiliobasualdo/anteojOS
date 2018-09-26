#include <myAlloc.h>


static uint64_t startOfMem;
static uint64_t nextToReturn;

boolean initKernelAlloc(uint64_t start)
{
    simple_printf("initKernelAlloc\n");
    startOfMem = start;
    nextToReturn = start;
    return TRUE;
}

void* my_malloc(unsigned int size)
{
    void *resp = (void *) nextToReturn;
    nextToReturn += size;
    return resp;
}

void my_free(void* ptr)
{

}
