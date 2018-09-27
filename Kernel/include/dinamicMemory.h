#ifndef DINAMICMEMORY_H
#define DINAMICMEMORY_H

#include "allocator.h"

void * kernelMalloc (size_t size);

void kernelFree (void * address);

#endif