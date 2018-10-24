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

//pipe_t * addPipeWithId(int id)
//{
//    pcbPtr process = getCurrentProc();
//    if (process == NULL)
//    {
//        return NULL;
//    }
//    int i;
//    for ( i = 0; i < FD_AMOUNT; ++i)
//    {
//        if(process->fd[i] != NULL && process->fd[i]->pipeId == id)
//        {
//            return process->fd[i];
//        }
//    }
//    for (i = 0; i < FD_AMOUNT; ++i)
//    {
//        if(process->fd[i] == NULL)
//        {
//            process->fd[i] = getPipeFromPipeList(id);
//            return process->fd[i];
//        }
//
//    }
//    return NULL;
//}


pipe_t * createPipeK()
{
    int j;
    lockMutex(pipeListMutex);
    for ( j = 0; j < MAXPIPES; ++j) {
        if(pipeList[j] == NULL){
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

            simple_printf("mutex: %d\n", pipe->mutex);
            for (int i = 0; i < 400000; ++i) {

            }

            pipe->readMutex = startMutex(1);
            simple_printf("readmutex: %d\n", pipe->readMutex);

            pipe->writeMutex = startMutex(1);
            simple_printf("writemutex: %d\n", pipe->writeMutex);

            pipeList[j] = pipe;
            unlockMutex(pipeListMutex);
            //printIpcsQueues();
            return pipe;
        }
    }
    unlockMutex(pipeListMutex);
    return NULL;
}

int writePipeK(pipe_t *pipe, char *buffer, uint64_t sizeP)
{
    simple_printf("entro al write\n");
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
            if (pipe->charsToRead >= PIPEBUFFERSIZE)//significa que el buffer esta lleno
            {
                unlockMutex(pipe->mutex);

                lockMutex(pipe->writeMutex); //espero hasta que se lea algo

                //simple_printf("Got b4 mutexlock2 %d\n", pipe->mutex);
                lockMutex(pipe->mutex);
                //simple_printf("Got past mutexlock2 %d\n", pipe->mutex);

            }
            else
            {
//                for (int j = 0;j < pipe->charsToRead;j++)
//                {
//                    simple_printf("%d: %d\n", j, pipe->buffer[j+pipe->bufferReadPosition]);
//                }
                int index = (i + (pipe->bufferWritePosition)++) % PIPEBUFFERSIZE;
                pipe->buffer[index]= buffer[i];
                pipe->charsToRead+=1;
                i++;
            }
            tryToLockMutex(pipe->writeMutex);
        }
        //drawPipeBuffer(pipe);
    }

    if(size>0 && buffer != 0)
    {
        //simple_printf("apa la papa\n");
        unlockMutex(pipe->readMutex);
    }
    //simple_printf("final del write\n");
    unlockMutex(pipe->mutex);

    return (int)size;
}

int readPipeK(pipe_t *pipe, char *buffer, uint64_t sizeP)
{
    if(pipe == NULL)
    {
        return -1;
    }
    int size = (int)sizeP;
    simple_printf("Got in read ");
    lockMutex(pipe->mutex);
    simple_printf("Not bloqued ");

    if(pipe->pipeId == STDOUT)
    {
        unlockMutex(pipe->mutex);
        return -1;
    }
    int i = 0;
    while(i < size)
    {
        if (pipe->charsToRead == 0)//significa que el buffer esta lleno
        {
            unlockMutex(pipe->mutex);

            simple_printf("entre al lockreadmutex \n");

            int a = lockMutex(pipe->readMutex);

            simple_printf("lock: %d\n", a);

            for (int j = 0; j < 4000000; ++j) {}

            simple_printf("sali del  lockreadmutex\n");

            lockMutex(pipe->mutex);


            simple_printf("no me trave con el lockmutex\n");
        }
        else
        {
            int index = (i + pipe->bufferReadPosition++) % PIPEBUFFERSIZE;
            simple_printf("entro al else\n");
            buffer[i] = pipe->buffer[index];
            pipe->charsToRead--;
            simple_printf("estoy en read\n");
            i++;
        }
        tryToLockMutex(pipe->readMutex);
        //drawPipeBuffer(pipe);
        //simple_printf("charsToRead %d %d\n", pipe->charsToRead, pipe->bufferWritePosition);
    }
    //simple_printf("sali del while\n");
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

    pipe_t * newPipe = addPipeK();

    if(p1 != NULL && p2 != NULL)
    {
        int aux = addPipeProcess();
        p1->fd[STDOUT] = aux;
        p2->fd[STDIN] = aux;
        return 1;
    }
    return 0;
}

void addStandardPipes(pPid pid) {
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