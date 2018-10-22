#include "pipes.h"

pipe_t * pipeList[MAXPIPES];


void initPipes()
{
    int i;
    for (i = 0; i < MAXPIPES; i++)
    {
        pipeList[i] = NULL;
    }
}

pipe_t * create_pipeK()
{
    int j;

    for ( j = 0; j < MAXPIPES; ++j) {
        if(pipeList[j] == NULL){
            pipe_t * pipe = kernelMalloc(sizeof(pipe_t));
            pipe->buffer = kernelMalloc(PIPEBUFFERSIZE);
            pipe->bufferReadPosition = 0;
            pipe->bufferWritePosition = 0;
            pipe->charsToRead = 0;
            pipe->pipeId = j;
            pipe->mutex = newMutex(0);
            pipeList[j] = pipe;
            return pipe;
        }
    }
    return NULL;
}

int write_pipeK(pipe_t * pipe, char * buffer, uint64_t size)
{
    lockMutex(pipe->mutex);
    int i = 0;
    while(i < size)
    {
        if (pipe->charsToRead > PIPEBUFFERSIZE)//significa que el buffer esta lleno
        {
            unlockMutex(pipe->mutex);

            //ver que hacer con esto. probablemente condvar

            lockMutex(pipe->mutex);
        }
        else
        {
            pipe->buffer[(i + pipe->bufferWritePosition++) % PIPEBUFFERSIZE]= buffer[i];
            pipe->charsToRead++;
        }
        i++;
    }
    unlockMutex(pipe->mutex);
    return (int)size;
}
int read_pipeK(pipe_t * pipe, char * buffer, uint64_t size)
{
    lockMutex(pipe->mutex);
    int i = 0;
    while(i < size)
    {
        if (pipe->charsToRead == 0)//significa que el buffer esta lleno
        {
            unlockMutex(pipe->mutex);

            //ver que hacer con esto. Probablemente condvar

            lockMutex(pipe->mutex);
        }
        else
        {
            buffer[i] = pipe->buffer[(i + pipe->bufferReadPosition++) % PIPEBUFFERSIZE];
            pipe->charsToRead--;
        }
        i++;
    }
    unlockMutex(pipe->mutex);
    return (int)size;
}
int close_pipeK(pipe_t * pipe)
{

}