#include <process.h>
#include "pipes.h"

pipe_t * pipeList[MAXPIPES];

int pipeListMutex;

/** inicia los pipes en kernel */
int initPipes()
{

    int i;
    pipeListMutex = startMutex(0);          /** mutex para cambiar el array de pipes pipeList */
    simple_printf("pipeListMutex = %d\n", pipeListMutex);
    pipeList[0] = createPipeK();            /** el primer pipe es para el stdin */
    if(pipeList[0] == NULL)
    {
        return 0;
    }
    pipeList[1] = createPipeK();            /** el segundo pipe es para el stdout */
    if(pipeList[1] == NULL)
    {
        return 0;
    }
    for (i = 2; i < MAXPIPES; i++)          /** los demas pipes quedan para otros usos */
    {
        pipeList[i] = NULL;
    }

    return 1;
}

/** getter de un puntero a un pipe */
pipe_t * getPipeFromPipeList(int id)
{
    return pipeList[id];
}

/** agrega un pipe a la lista */
pipe_t * addPipeK()
{
    pcbPtr process = getCurrentProc();
    if (process == NULL)
    {
        return NULL;
    }
    int i;
    for (i = 0; i < FD_AMOUNT; ++i)             /** FD_AMOUNT es la cantidad de fd que puede tener un proceso */
    {
        if(process->fd[i] == -1)                /** si hay lugar para agregar un fd */
        {
            pipe_t * newPipe = createPipeK();
            if(newPipe != NULL)
            {
                process->fd[i] = newPipe->pipeId;
            }
            return newPipe;
        }
    }
    simple_printf("ERROR: there are no free pipes left\n");
    return NULL;
}

/** crea un pipe */
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

            pipe->bufferReadPosition = 0;          /** donde arranca a leer el buffer */

            pipe->bufferWritePosition = 0;         /** donde arranca a escribir el buffer */

            pipe->charsToRead = 0;

            pipe->pipeId = j;                       /** posicion en la lista de pipes */

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

/** escribe al pipe en * pipe el tamaño sizeP lo que hay en buffer */
int writePipeK(pipe_t *pipe, char *buffer, uint64_t sizeP)
{
    int size = (int)sizeP;

    if(pipe == NULL || buffer == NULL || size < 0)
    {
        return -1;
    }
    if(size == 0)
    {
        return 0;
    }


    lockMutex(pipe->mutex);             /** entro a la sección critica del pipe */

    if(pipe->pipeId == STDOUT)
    {
        drawString(buffer);
    }
    else
    {
        int i = 0;
        while(i < size)
        {
            if (pipe->charsToRead >= PIPEBUFFERSIZE)    /** buffer lleno y necesito que me lean el buffer */
            {
                unlockMutex(pipe->mutex);

                lockMutex(pipe->writeMutex);            /** espero a que haya menos chars para leer */

                lockMutex(pipe->mutex);                 /** entonces estoy listo para escribir */

            }
            else
            {
                int index = pipe->bufferWritePosition % PIPEBUFFERSIZE;       /** el buffer es circular */
                pipe->bufferWritePosition = (pipe->bufferWritePosition+1)%PIPEBUFFERSIZE;
                pipe->buffer[index]= buffer[i];
                pipe->charsToRead+=1;
                i++;
            }
            tryToLockMutex(pipe->writeMutex);           /** me aseguro de que el writeMutex este bloqueado para la proxima iteración */
        }
        if (pipe->charsToRead < PIPEBUFFERSIZE-1)       /** si no está lleno el buffer */
        {
            pipe->buffer[pipe->bufferWritePosition] = EOF;
        }
    }


    unlockMutex(pipe->readMutex);

    unlockMutex(pipe->mutex);

    return size;
}

/** lee de un pipe en *pipe el tamaño sizeP y lo pone en un buffer */
int readPipeK(pipe_t *pipe, char *buffer, uint64_t sizeP)
{
    if(pipe == NULL)
    {
        return -1;
    }

    int size = (int)sizeP;

    lockMutex(pipe->mutex);                 /** entramos a la seccion critica */

    if(pipe->pipeId == STDOUT)
    {
        unlockMutex(pipe->mutex);           /** no hay nada para leer en stdOUT */
        return -1;
    }

    int i = 0;
    while(i < size)
    {
        if (pipe->charsToRead == 0)             /** el buffer esta vacio */
        {

            if(pipe->pipeId == STDIN)           /** no hay nada para leer en stdin */
            {
                unlockMutex(pipe->mutex);
                lockMutexKeyboard(pipe->readMutex);     /** bloquamos el mutex hasta una interrupcion de keyboard --> esperamos a que alguien escriba */
            }
            else
            {
                unlockMutex(pipe->mutex);
                lockMutex(pipe->readMutex);         /** esperas a leer */
            }


            for (int j = 0; j < 4000000; ++j){}         /** para evitar el problema de hlt */

            lockMutex(pipe->mutex);

        }
        else
        {
            int index = pipe->bufferReadPosition % PIPEBUFFERSIZE;
            pipe->bufferReadPosition = (1+ pipe->bufferReadPosition) % PIPEBUFFERSIZE;
            buffer[i] = pipe->buffer[index];
            pipe->charsToRead--;
            i++;
        }
        tryToLockMutex(pipe->readMutex);
    }
    if(size>0)
    {
        unlockMutex(pipe->writeMutex);
    }

    unlockMutex(pipe->mutex);

    if(size == 1 && buffer[0] == EOF)
    {
        return EOF;
    }

    return size;
}

/** cierra un pipe y libera sus recursos */
int closePipeForProcK(pipe_t * pipe, pPid proc)
{
    if(pipe == NULL)
    {
        return 0;
    }
    lockMutex(pipeListMutex);

    pcbPtr process = getPcbPtr(proc);
    int i;
    for (i = 0; i < FD_AMOUNT; ++i)
    {
        if(process->fd[i] == pipe->pipeId)      /** encuentra al file descriptor */
        {
            process->fd[i] = -1;
            break;
        }
    }

    pipeList[pipe->pipeId] = NULL;
    destroyMutexK(pipe->readMutex);
    destroyMutexK(pipe->writeMutex);
    destroyMutexK(pipe->mutex);
    kernelFree(pipe);

    unlockMutex(pipeListMutex);

    return 1;
}

int closePipeK(pipe_t * pipe)
{
    if(pipe == NULL)
    {
        return 0;
    }
    lockMutex(pipeListMutex);

    pcbPtr process = getCurrentProc();
    int i;
    for (i = 0; i < FD_AMOUNT; ++i)
    {
        if(process->fd[i] == pipe->pipeId)      /** encuentra al file descriptor */
        {
            process->fd[i] = -1;
            break;
        }
    }

    pipeList[pipe->pipeId] = NULL;
    destroyMutexK(pipe->readMutex);
    destroyMutexK(pipe->writeMutex);
    destroyMutexK(pipe->mutex);
    kernelFree(pipe);

    unlockMutex(pipeListMutex);

    return 1;
}

/** hacemos un dup entre procesos --> generamos un pipe */
int dupProc(pPid pidOut, pPid pidIn)
{
    pcbPtr p1 = getPcbPtr(pidOut);
    pcbPtr p2 = getPcbPtr(pidIn);

    if(p1 != NULL && p2 != NULL)
    {
        if(p1->fd[STDOUT] != STDOUT)
        {
            closePipeK(getPipeFromPipeList(p1->fd[STDOUT]));    /** cierro el pipe que voy a cambiar */
        }
        else
        {
            p1->fd[STDOUT] = -1;
        }
        if(p2->fd[STDIN] != STDIN)
        {
            closePipeK(getPipeFromPipeList(p2->fd[STDIN]));     /** idem */
        }
        else
        {
            p2->fd[STDIN] = -1;
        }

        int aux = addPipeProcess();             /** nuevo file descriptor */
        if (aux == -1)
        {
            return 0;
        }
        p1->fd[STDOUT] = aux;
        p2->fd[STDIN] = aux;
        return 1;
    }
    return 0;
}

/** cambia el file descriptor de lectura y escritura de un proceso a stdin y stdout --> lo usamos en la shell */
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

/** addPipe que solo retorna el nuevo id */
int addPipeProcess()
{
    pipe_t * aux = addPipeK();
    if(aux == NULL)
    {
        return -1;
    }
    return aux->pipeId;
}

/**  */
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

//si 1 stdin nomas
//si 2 stdout nomas
//si no ambos
int changeToStds(pPid proc, int flag)
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
                if(process != getCurrentProc())
                {
                    closePipeForProcK(getPipeFromPipeList(process->fd[STDIN]), process->pid);
                }
                else
                {
                    closePipeK(getPipeFromPipeList(process->fd[STDIN]));    /** cierro el pipe que voy a cambiar */
                }
            }
            process->fd[STDIN] = STDIN;
            break;
        case 2:
            if(out->pipeId != STDOUT)
            {
                if(process != getCurrentProc())
                {
                    closePipeForProcK(getPipeFromPipeList(process->fd[STDOUT]), process->pid);
                }
                else
                {
                    closePipeK(getPipeFromPipeList(process->fd[STDOUT]));    /** cierro el pipe que voy a cambiar */
                }
            }
            process->fd[STDOUT] = STDOUT;
            break;
        default:
            if(in->pipeId != STDIN)
            {
                if(process != getCurrentProc())
                {
                    closePipeForProcK(getPipeFromPipeList(process->fd[STDIN]), process->pid);
                }
                else
                {
                    closePipeK(getPipeFromPipeList(process->fd[STDIN]));    /** cierro el pipe que voy a cambiar */
                }
            }
            if(out->pipeId != STDOUT)
            {
                if(process != getCurrentProc())
                {
                    closePipeForProcK(getPipeFromPipeList(process->fd[STDOUT]), process->pid);
                }
                else
                {
                    closePipeK(getPipeFromPipeList(process->fd[STDOUT]));    /** cierro el pipe que voy a cambiar */
                }
            }
            process->fd[STDIN] = STDIN;
            process->fd[STDOUT] = STDOUT;
            break;
    }
    return 1;
}