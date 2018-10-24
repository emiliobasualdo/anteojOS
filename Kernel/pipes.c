#include <process.h>
#include "pipes.h"

pipe_t * pipeList[MAXPIPES];

int pipeListMutex;

int initPipes()
{

    int i;
    pipeListMutex = startMutex(0);
    simple_printf("pipeListMutex = %d\n", pipeListMutex);
    for (i = 2; i < MAXPIPES; i++)
    {
        pipeList[i] = NULL;
    }
    pipeList[0] = createPipeK();//STDIN
    if(pipeList[0] == NULL)
    {
        return 0;
    }
    pipeList[1] = createPipeK();//STDOUT
    if(pipeList[1] == NULL)
    {
        return 0;
    }
    return 1;
}

pipe_t * getPipeFromPipeList(int id)
{
    return pipeList[id];
}

pipe_t * addPipeK()
{
    pcbPtr process = getCurrentProc();
    if (process == NULL)
    {
        return NULL;
    }
    int i;
    for (i = 0; i < FD_AMOUNT; ++i)
    {
        if(process->fd[i] == -1)
        {
            pipe_t * aux = createPipeK();
            if(aux != NULL)
            {
                process->fd[i] = aux->pipeId;
            }
            return aux;
        }
    }
    simple_printf("ERROR: there are no free pipes left\n");
    return NULL;
}

pipe_t * createPipeK()
{
    int j;
    lockMutex(pipeListMutex);
    for ( j = 0; j < MAXPIPES; ++j) {
        if(pipeList[j] == NULL)
        {
            pipe_t * pipe = kernelMalloc(sizeof(pipe_t));
            if (pipe == NULL)
            {
                simple_printf("Error: Malloc returned 0 on pipe creation\n");
                return NULL;
            }

            pipe->buffer = kernelMalloc(PIPEBUFFERSIZE* sizeof(char));

            pipe->bufferReadPosition = 0;

            pipe->bufferWritePosition = 0;

            pipe->charsToRead = 0;

            pipe->pipeId = j;

            pipe->mutex = startMutex(0);

            pipe->readMutex = startMutex(1);

            pipe->writeMutex = startMutex(1);

            pipeList[j] = pipe;
            unlockMutex(pipeListMutex);

            return pipe;
        }
    }
    unlockMutex(pipeListMutex);
    return NULL;
}

int writePipeK(pipe_t *pipe, char *buffer, uint64_t sizeP)
{
    int size = (int)sizeP;
    if(pipe == NULL)
    {
        return -1;
    }
    lockMutex(pipe->mutex);

    if(pipe->pipeId == STDOUT)
    {
        drawString(buffer);
    }
    else
    {
        int i = 0;
        while(i < size)
        {
            if (pipe->charsToRead >= (PIPEBUFFERSIZE-1))//significa que el buffer esta lleno
            {
                unlockMutex(pipe->mutex);

                lockMutex(pipe->writeMutex); //espero hasta que se lea algo

                lockMutex(pipe->mutex);

            }
            else
            {
                int index = (i + pipe->bufferWritePosition) % PIPEBUFFERSIZE;
                pipe->bufferWritePosition = (pipe->bufferWritePosition+1)%PIPEBUFFERSIZE;
                pipe->buffer[index]= buffer[i];
                pipe->charsToRead+=1;
                i++;
            }
            tryToLockMutex(pipe->writeMutex);
        }
        if (pipe->charsToRead < PIPEBUFFERSIZE-1)
        {
            pipe->buffer[pipe->bufferWritePosition] = EOF;
            pipe->charsToRead++;
        }
    }


    if(size>0 && buffer != 0)
    {
        unlockMutex(pipe->readMutex);
    }
    unlockMutex(pipe->mutex);

    return size;
}

int readPipeK(pipe_t *pipe, char *buffer, uint64_t sizeP)
{
    if(pipe == NULL)
    {
        return -1;
    }
    int size = (int)sizeP;
    lockMutex(pipe->mutex);

    if(pipe->pipeId == STDOUT)
    {
        unlockMutex(pipe->mutex);
        return -1;
    }
    int i = 0;
    while(i < size)
    {
        if (pipe->charsToRead == 0)//significa que el buffer esta vacio
        {
            unlockMutex(pipe->mutex);


            if(pipe->pipeId == STDIN)
                lockMutexKeyboard(pipe->readMutex);
            else
                lockMutex(pipe->readMutex);


            for (int j = 0; j < 4000000; ++j){}


            lockMutex(pipe->mutex);

            }
        else
        {
            int index = (i + pipe->bufferReadPosition) % PIPEBUFFERSIZE;
            pipe->bufferReadPosition = (1+ pipe->bufferReadPosition) % PIPEBUFFERSIZE;
            buffer[i] = pipe->buffer[index];
            pipe->charsToRead--;
            i++;
        }
        tryToLockMutex(pipe->readMutex);
    }
    if(size>0)
        unlockMutex(pipe->writeMutex);

    unlockMutex(pipe->mutex);

    return size;
}

int closePipeK(pipe_t * pipe)
{
    if(pipe == NULL)
        return 0;

    lockMutex(pipeListMutex);

    pcbPtr process = getCurrentProc();
    int i;
    for (i = 0; i < FD_AMOUNT; ++i)
    {
        if(process->fd[i] == pipe->pipeId)
        {
            process->fd[i] = -1;
            break;
        }
    }

    kernelFree(pipe->buffer);
    pipeList[pipe->pipeId] = NULL;
    destroyMutexK(pipe->readMutex);
    destroyMutexK(pipe->writeMutex);

    destroyMutexK(pipe->mutex);
    kernelFree(pipe);

    unlockMutex(pipeListMutex);

    return 1;
}

int dupProc(pPid pidOut, pPid pidIn)
{
    pcbPtr p1 = getPcbPtr(pidOut);
    pcbPtr p2 = getPcbPtr(pidIn);

    if(p1 != NULL && p2 != NULL)
    {
        closePipeK(getPipeFromPipeList(p1->fd[STDOUT]));
        closePipeK(getPipeFromPipeList(p2->fd[STDIN]));

        int aux = addPipeProcess();
        if (aux == -1)
        {
            return 1;
        }
        p1->fd[STDOUT] = aux;
        p2->fd[STDIN] = aux;
        return 1;
    }
    return 0;
}

void addStandardPipes(pPid pid)
{
    pcbPtr process = getPcbPtr(pid);
    if(process == NULL)
    {
        simple_printf("Error: Process could not be found\n");
        return;
    }
    if (process->fd[STDIN] != STDIN)
    {
        closePipeK(getPipeFromPipeList(process->fd[STDIN]));
    }
    if (process->fd[STDOUT] != STDOUT)
    {
        closePipeK(getPipeFromPipeList(process->fd[STDOUT]));
    }
    process->fd[STDIN] = STDIN;
    process->fd[STDOUT] = STDOUT;

}

void drawPipeBuffer(pipe_t * pipe)
{
    if(pipe == NULL)
        return;
    drawString("Buffer: ");
    drawString(pipe->buffer);
    drawChar('\n');
}

int addPipeProcess()
{
    pipe_t * aux = addPipeK();
    if(aux == NULL)
    {
        return -1;
    }
    return aux->pipeId;
}


int addPipeToSC()
{
    int aux = addPipeProcess();
    pcbPtr process = getCurrentProc();
    if(process == NULL || aux == -1)
    {
        return -1;
    }
    for (int i = 0; i < FD_AMOUNT; ++i)
    {
        if(process->fd[i] == aux)
        {
            return i;
        }
    }
    return -1;
}

//si 1 stdout nomas
//si 2 stdin nomas
//si no ambos
int change(pPid proc, int flag)
{
    pcbPtr process = getPcbPtr(proc);
    if(process == NULL)
    {
        return 0;
    }

    pipe_t * in = getPipeFromPipeList(process->fd[STDIN]);
    pipe_t * out = getPipeFromPipeList(process->fd[STDOUT]);
    switch (flag)
    {
        case 1:
            if(in->pipeId != STDIN)
            {
                closePipeK(in);
            }
            break;
        case 2:
            if(out->pipeId != STDOUT)
            {
                closePipeK(in);
            }

            break;
        default:

            break;
    }
}