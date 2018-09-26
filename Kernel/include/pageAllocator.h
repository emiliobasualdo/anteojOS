//
// Created by Emilio Basualdo on 9/26/18.
//

#ifndef ANTEOJOS_PAGEALLOCATOR_H
#define ANTEOJOS_PAGEALLOCATOR_H

#include <system.h>

boolean initAllocator(uint64_t firstMemPosition);

void *requestPage();

void freePage(void *memPtr);
#endif //ANTEOJOS_PAGEALLOCATOR_H
