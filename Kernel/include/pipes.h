#ifndef ANTEOJOS_PIPES_H
#define ANTEOJOS_PIPES_H

#include "ipcStructs.h"

typedef struct pipe_t {

    int pipeId; //si es que necesito

    char * buffer;
    int bufferPosition;
    int bufferSize;

    mutex_t mutex;

} pipe_t;

#endif //ANTEOJOS_PIPES_H
