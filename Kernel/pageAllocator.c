//
// Created by Emilio Basualdo on 9/26/18.
//

#include <system.h>
#include "pageAllocator.h"

uint64_t start;
boolean initAllocator(uint64_t firstMemPosition)
{
    start = firstMemPosition;


    return TRUE;
}