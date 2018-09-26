#ifndef PRODCONS_H
#define PRODCONS_H

#include "userPrintf.h"
#include <stdbool.h>
#include "stdlib.h"

#define BUFFER_SIZE_MAX 50//512             // TODO ver bien esto
#define ANTEOJOS1     "OOOO                       OOOO"
#define ANTEOJOS2     "   OO                     OO   "
#define ANTEOJOS3     "    OO                   OO    "
#define ANTEOJOS4     "     OOOOOOOO     OOOOOOOO     "
#define ANTEOJOS5     "    OO       OOOOO       OO    "
#define ANTEOJOS6     "   OO        OOOOO        OO   "
#define ANTEOJOS7     "   OO        OO OO        OO   "
#define ANTEOJOS8     "    OO      OO   OO      OO    "
#define ANTEOJOS9     "     OOOOOOOO     OOOOOOOO     "
#define ANTEOJOSY 9
#define ANTEOJOSX 32
#define MAXPROC 40

typedef struct anteojosCDT
{
    char anteojos1[ANTEOJOSX];
    char anteojos2[ANTEOJOSX];
    char anteojos3[ANTEOJOSX];
    char anteojos4[ANTEOJOSX];
    char anteojos5[ANTEOJOSX];
    char anteojos6[ANTEOJOSX];
    char anteojos7[ANTEOJOSX];
    char anteojos8[ANTEOJOSX];
    char anteojos9[ANTEOJOSX];

} anteojosCDT;

typedef anteojosCDT * anteojOS;

int producerConsumer(int numProducers, int numConsumers);

/**
 *    OOOO                       OOOO
 *       OO                     OO
 *        OO                   OO
 *         OOOOOOOO     OOOOOOOO
 *        OO       OOOOO        OO
 *       OO        OOOOO         OO
 *       OO        OO OO         OO
          OO      OO   OO       OO
           OOOOOOOO      OOOOOOOO

 */

#endif
