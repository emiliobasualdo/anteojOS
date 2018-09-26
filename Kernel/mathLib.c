#include "mathLib.h"

uint64_t max (uint64_t num1, uint64_t num2)
{
    return (num1 >= num2) ? num1 : num2;
}

uint64_t min (uint64_t num1, uint64_t num2)
{
    return (num1 <= num2) ? num1 : num2;
}

/** source: https://stackoverflow.com/questions/2679815/previous-power-of-2
 * retorna la potencia de 2 mas cercana y menor o igual
 * ejemplo: 511 retorna 256 */
uint64_t nextLowerPowerOf2(uint64_t totalMemory)
{
    uint64_t x = totalMemory;
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >> 16);
    return x - (x >> 1);
}

