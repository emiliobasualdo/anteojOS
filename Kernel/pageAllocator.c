//
// Created by Emilio Basualdo on 9/26/18.
//

#include <system.h>
#include "pageAllocator.h"

#define MAX_PAGE_REQUEST 5
#define TOTAL_MEMORY 70000000
#define PAGE_SIZE 4000
#define TOTAL_PAGES TOTAL_MEMORY/PAGE_SIZE

#define FREE -1
#define ERROR -2

static uint64_t start;
static short table[TOTAL_PAGES];

static int getPageIndex();
static void initTable();

boolean initAllocator(uint64_t firstMemPosition)
{
    start = firstMemPosition;
    initTable();
    return TRUE;
}

static void initTable()
{
    for (int i = 0; i < TOTAL_PAGES; ++i)
    {
        table[i] = FREE;
    }
}

void *requestPage()
{
    int index = getPageIndex();
    if (index == ERROR)
    {
        simple_printf("requestPage: NO MORE SPACE!");
        return ERROR;
    }
    simple_printf("allocamos la entrada %d y el puntero %d\n",index, ((index * PAGE_SIZE) + start));
    return (void *) ((index * PAGE_SIZE) + start);
}

static int getPageIndex()
{
    for (int i = 0; i < TOTAL_PAGES; ++i)
    {
        if (table[i] == FREE )
            return i;
    }
    return ERROR;
}

void freePage(void *memPtr)
{
    int entrada = (int) (((uint64_t)memPtr - start) / PAGE_SIZE);
    simple_printf("Liberamos %d\n", entrada);
    table[entrada] = FREE;
}