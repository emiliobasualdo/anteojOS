#ifndef ANTEOJOS_PIPES_H
#define ANTEOJOS_PIPES_H

#include "dinamicMemory.h"
#include <stdint.h>
#include "ipc.h"

#define MAXPIPES 10
#define PIPEBUFFERSIZE 4096

typedef struct pipe_t {

    int pipeId;
    char * buffer;
    int bufferReadPosition;
    int bufferWritePosition;
    int charsToRead;
    int mutex;

} pipe_t;


pipe_t * create_pipeK();
int write_pipeK(pipe_t * pipe, char * buffer, uint64_t size);
int read_pipeK(pipe_t * pipe, char * buffer, uint64_t size);
int close_pipeK(pipe_t * pipe);

#endif //ANTEOJOS_PIPES_H
