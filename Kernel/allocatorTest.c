#include "allocatorTest.h"

static char * addresses[MAX_ADDRESS_SIZE];

static void sleep();

void runAllocatorTest()
{
    static long counterAlloc = 0, counterFree = 0;
    simple_printf("\nWELCOME TO THE ALLOCATOR TEST\n");
    simple_printf("We are going to allocate a lot of memory and see what happens\n");
    simple_printf("Press a key to continue!\n");
    getNextChar();

    simple_printf("\nFirst, we will try allocating small amounts of bytes\n");
    simple_printf("We will allocate %d bytes on amounts of %d bytes\n", SMALL_AMOUNT*(MAX_ADDRESS_SIZE), SMALL_AMOUNT);
    simple_printf("After all bytes are allocated, all buddy systems will be shown as trees, from the smaller leaf up to the bigger leaf\n");
    simple_printf("Press a key to continue!\n");
    getNextChar();

    for (int i=0; i<MAX_ADDRESS_SIZE; i++)
    {
        addresses[i] = kernelMalloc(SMALL_AMOUNT);
        if (addresses[i] != NULL)
        {
            counterAlloc += SMALL_AMOUNT;
            simple_printf("#%d: %d bytes allocated starting from at ", i, SMALL_AMOUNT);
            drawHexa((uint64_t ) addresses[i]);
            simple_printf("\n");
        }
        else
        {
            simple_printf("#%d --> is NULL!\n");
            i = MAX_ADDRESS_SIZE;
        }
    }

    sleep();
    drawAllocator();
    sleep();

    simple_printf("\n\n\n As ");
    drawHexa((uint64_t) counterAlloc);
    simple_printf(" bytes were allocated, now we will try to use that space!\n");
    sleep();

    char anteojos[SMALL_AMOUNT] = "anteojOS";

    for (int i=0; i<MAX_ADDRESS_SIZE; i++)
    {
        for (int j=0; j<SMALL_AMOUNT; j++)
        {
            addresses[i][j] = anteojos[j];
        }
    }

    simple_printf("We wrote anteojOS on every address obtained and now we will try to read them:\n");
    sleep();
    for (int i=0; i<MAX_ADDRESS_SIZE; i++)
    {
        simple_printf("#%d contains -----> %s\n", i, addresses[i]);
    }
    sleep();

    simple_printf("\nNow it's time to free those bytes!\n");
    sleep();

    for(int i=0; i<MAX_ADDRESS_SIZE; i++)
    {
        simple_printf("#%d: freeing %d bytes at ", i, SMALL_AMOUNT);
        drawHexa((uint64_t) addresses[i]);
        simple_printf("\n");
        kernelFree(addresses[i]);
        counterFree +=SMALL_AMOUNT;
    }

    sleep();
    drawAllocator();
    sleep();

    if (counterAlloc == counterFree)
    {
        simple_printf("\nIf we are here it's because ");
        drawHexa((uint64_t) counterAlloc);
        simple_printf(" bytes were allocated and then freed\n");
        simple_printf("\nFIRST TEST APPROVED :)\n");
    }
    else
    {
        simple_printf("\nFIRST TEST DISSAPROVED :(\n");
    }


    sleep();
    simple_printf("\nSecondly, we will try allocating greater amounts of bytes: from small amounts to big amounts\n");
    simple_printf("After all bytes are allocated, all buddy systems will be shown as trees, from the smaller leaf up to the bigger leaf\n");
    sleep();

    for (int i=1; i<MAX_ADDRESS_SIZE; i++)
    {
        addresses[i] = kernelMalloc((size_t )(GREAT_AMOUNT * i));
        if (addresses[i] != NULL)
        {
            counterAlloc += GREAT_AMOUNT * i;
            simple_printf("#%d: %d bytes allocated starting from at ", i, GREAT_AMOUNT * i);
            drawHexa((uint64_t ) addresses[i]);
            simple_printf("\n");
        }
        else
        {
            simple_printf("#%d --> is NULL!\n");
            i = MAX_ADDRESS_SIZE;
        }
    }

    sleep();
    drawAllocator();
    sleep();

    simple_printf("\n\n\n As ");
    drawHexa((uint64_t) counterAlloc);
    simple_printf(" bytes were allocated, now we will try to use that space!\n");
    sleep();

    for (int i=0; i<MAX_ADDRESS_SIZE; i++)
    {
        for (int j=0; j<SMALL_AMOUNT; j++)
        {
            addresses[i][j] = anteojos[j];
        }
    }

    simple_printf("We wrote anteojOS on every address obtained and now we will try to read them:\n");
    sleep();
    for (int i=0; i<MAX_ADDRESS_SIZE; i++)
    {
        simple_printf("#%d contains -----> %s\n", i, addresses[i]);
    }
    sleep();

    simple_printf("\nNow it's time to free those bytes!\n");
    sleep();

    for(int i=0; i<MAX_ADDRESS_SIZE; i++)
    {
        simple_printf("#%d: freeing %d bytes at ", i, GREAT_AMOUNT * i);
        drawHexa((uint64_t) addresses[i]);
        simple_printf("\n");
        kernelFree(addresses[i]);
        counterFree += GREAT_AMOUNT * i;
    }

    sleep();
    drawAllocator();
    sleep();

    if (counterAlloc == counterFree)
    {
        simple_printf("\nIf we are here it's because ");
        drawHexa((uint64_t) counterAlloc);
        simple_printf(" bytes were allocated and then freed\n");
        simple_printf("\nSECOND TEST APPROVED :)\n");
    }
    else
    {
        simple_printf("\nSECOND TEST DISSAPROVED :(\n");
    }
}

static void sleep()
{
    kernelSleep();
    kernelSleep();
    kernelSleep();
    kernelSleep();
    kernelSleep();
    kernelSleep();
    kernelSleep();
    kernelSleep();
    kernelSleep();
    kernelSleep();
    kernelSleep();
    kernelSleep();
    kernelSleep();
    kernelSleep();
    kernelSleep();
    kernelSleep();
    kernelSleep();
    kernelSleep();
    kernelSleep();
    kernelSleep();
    kernelSleep();
    kernelSleep();
    kernelSleep();
    kernelSleep();
}