//
// Created by Emilio Basualdo on 9/4/18.
//

#ifndef ANTEOJOS_ALLOC_H
#define ANTEOJOS_ALLOC_H

#include <stdint.h>
#include <printf.h>
#include <system.h>

boolean initKernelAlloc(uint64_t start);

void* my_malloc(unsigned int);

void my_free(void *);
#endif //ANTEOJOS_ALLOC_H