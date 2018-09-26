#include "include/pageAllocator.h"

static uint64_t getPagesSize(size_t size);

void * pageAlloc(size_t size)
{
    uint64_t pageSize = getPagesSize(size);
    return alloc(pageSize);
}

static uint64_t getPagesSize(size_t size)
{
    return ((uint64_t)(size/PAGE_SIZE) + 1) * PAGE_SIZE;
}

void pageFree (void * address)
{
    dealloc(address);
}