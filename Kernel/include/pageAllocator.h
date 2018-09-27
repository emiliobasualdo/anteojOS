#ifndef PAGEALLOCATOR_H
#define PAGEALLOCATOR_H

#include "allocator.h"

#define PAGE_SIZE 4096                  /** --> = 4 KiB */

void * pageAlloc(size_t size);
void pageFree (void * address);

#endif