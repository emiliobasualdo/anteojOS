#ifndef ANTEOJOS_PIPES_H
#define ANTEOJOS_PIPES_H

#include "dinamicMemory.h"
#include <stdint.h>
#include "ipc.h"
#include "ipcStructs.h"
#include "system.h"

#define MAXPIPES 100

pipe_t * addPipeK();
int addPipeToSC();
int writePipeK(pipe_t *pipe, char *buffer, uint64_t size);
int readPipeK(pipe_t *pipe, char *buffer, uint64_t size);
int closePipeK(pipe_t * pipe);
int dupProc(pPid pidOut, pPid pidIn);
int initPipes();
int changeToStds(pPid proc, int flag);

void drawPipeBuffer(pipe_t * pipe);

#endif //ANTEOJOS_PIPES_H
