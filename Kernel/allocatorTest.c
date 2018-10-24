#include "allocatorTest.h"

static char * addresses[MAX_ADDRESS_SIZE];

static void sleep();

void runAllocatorTest()
{
    static long counterAlloc = 0, counterFree = 0;
    simple_printf("\nWELCOME TO THE ALLOCATOR TEST\n");
    simple_printf("We are going to allocate a lot of memory and see what happens\n");
    sleep();

    simple_printf("\nFirst, we will try allocating small amounts of bytes\n");
    simple_printf("We will allocate %d bytes on amounts of %d bytes\n", SMALL_AMOUNT*(MAX_ADDRESS_SIZE), SMALL_AMOUNT);
    simple_printf("After all bytes are allocated, all buddy systems will be shown as trees, from the smaller leaf up to the bigger leaf\n");
    sleep();

    int i;
    for (i=0; i<MAX_ADDRESS_SIZE; i++)
    {
        addresses[i] = kernelMalloc(SMALL_AMOUNT);
        if (addresses[i] != NULL)
        {
            counterAlloc += SMALL_AMOUNT;
            simple_printf("#%d: %d bytes allocated starting from %d\n", i, SMALL_AMOUNT, (uint64_t) addresses[i]);
        }
        else
        {
            simple_printf("#%d --> is NULL!\n", i);
            i = MAX_ADDRESS_SIZE;
        }
    }

    simple_printf("Now we'll draw the trees of the buddy systems of the memory manager\n");
    sleep();
    drawAllocator();
    simple_printf("drawing trees ......\n");
    sleep();


    simple_printf("\n\nAs %d bytes were allocated, now we will try to use that space!\n", counterAlloc);
    sleep();

    char anteojos[SMALL_AMOUNT] = "anteojOS";

    for (i=0; i<MAX_ADDRESS_SIZE; i++)
    {
        for (int j=0; j<SMALL_AMOUNT; j++)
        {
            addresses[i][j] = anteojos[j];
        }
    }

    simple_printf("We wrote anteojOS on every address obtained and now we will try to readK them:\n");
    sleep();

    for (i=0; i<MAX_ADDRESS_SIZE; i++)
    {
        simple_printf("#%d contains -----> %s\n", i, addresses[i]);
    }

    sleep();

    simple_printf("\nNow it's time to free those bytes!\n");
    sleep();


    for(i=0; i<MAX_ADDRESS_SIZE; i++)
    {
        simple_printf("#%d: freeing %d bytes at %d\n", i, SMALL_AMOUNT, addresses[i]);
        kernelFree(addresses[i]);
        counterFree +=SMALL_AMOUNT;
    }

    simple_printf("Now we'll draw the trees of the buddy systems of the memory manager\n");
    sleep();
    drawAllocator();
    sleep();


    if (counterAlloc == counterFree)
    {
        simple_printf("\nIf we are here it's because %d bytes were allocated and then freed\n", counterAlloc);
        simple_printf("\nFIRST TEST APPROVED :)\n");
    }
    else
    {
        simple_printf("\nFIRST TEST DISSAPROVED :(\n");
    }

    simple_printf("\nSecondly, we will try allocating greater amounts of bytes: from small amounts to big amounts\n");
    simple_printf("After all bytes are allocated, all buddy systems will be shown as trees, from the smaller leaf up to the bigger leaf\n");
    sleep();

    for (i=0; i<MAX_ADDRESS_SIZE; i++)
    {
        addresses[i] = kernelMalloc((size_t )(GREAT_AMOUNT * (i+1)));
        if (addresses[i] != NULL)
        {
            counterAlloc += GREAT_AMOUNT * (i+1);
            simple_printf("#%d: %d bytes allocated starting from %d\n", i, GREAT_AMOUNT * (i+1), (uint64_t) addresses[i]);
        }
        else
        {
            simple_printf("#%d --> is NULL!\n");
            i = MAX_ADDRESS_SIZE;
        }
    }


    simple_printf("Now we'll draw the trees of the buddy systems of the memory manager\n");
    sleep();
    drawAllocator();
    sleep();

    simple_printf("\n\nAs %d bytes were allocated, now we will try to use that space!\n", counterAlloc);
    sleep();

    for (i=0; i<MAX_ADDRESS_SIZE; i++)
    {
        for (int j=0; j<SMALL_AMOUNT; j++)
        {
            addresses[i][j] = anteojos[j];
        }
    }

    simple_printf("We wrote anteojOS on every address obtained and now we will try to readK them:\n");
    sleep();

    for (i=0; i<MAX_ADDRESS_SIZE; i++)
    {
        simple_printf("#%d contains -----> %s\n", i, addresses[i]);
    }

    sleep();
    simple_printf("\nNow it's time to free those bytes!\n");
    sleep();

    for(i=0; i<MAX_ADDRESS_SIZE; i++)
    {
        simple_printf("#%d: freeing %d bytes at %d\n", i, GREAT_AMOUNT * (i+1), addresses[i]);
        kernelFree(addresses[i]);
        counterFree += GREAT_AMOUNT * (i+1);
    }

    sleep();
    drawAllocator();
    sleep();

    if (counterAlloc == counterFree)
    {
        simple_printf("\nIf we are here it's because %d bytes were allocated and then freed\n", counterAlloc);
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
}
