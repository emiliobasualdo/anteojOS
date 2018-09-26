//
// Created by Emilio Basualdo on 9/3/18.
//
#include <process.h>
#include <scheduler.h>

/////////////////////////////////

static boolean isValidPid(pPid pid);
static void freeProcess(pcbPtr proc);
static pPid getNextPid();
static pcbPtr newProcess(char *name, uint64_t instruction, pPid parentPid, int demandPid, boolean foreground);
static pPid arrayAdd(pcbPtr pcbPtr, int demandPid);
static boolean addChildToParentList(pPid parentPid, pPid childPid);
static void procsDeathCleanUp(pcbPtr proc);
static void printProc(pcbPtr pcb);

static void reacursiveKill(pPid pid);

/** arraya de punteros a pcbs */
static pcbPtr array[MAX_PROCS]; // dinámica todo
static unsigned int arrSize;
static pPid maxPid;
static pPid nextPid; // siempre tiene el ultimo pid asignado
static char * statesNames[] = {"b", "r", "R", "B", "D"}; // lo puse aca porque en el .h me tiraba problemas de include
pcbPtr bussyWaitingProcPcb;

/**
 * Estas clase es la que se encarga del manejo de procesos.
 * Designa Pids y deshecha procesos muertos unicamente cuando
 * es necesario su pid dado que se necesite crear un proceso nuevo.
 * Mantenemos punteros a procesos guardados en un arreglo circular
 * pero saltendo siempre la posicion 0 porque alli se encuentra el
 * proceso genesis. El indice dentro del arreglo representa el
 * pid del proceso apuntado.
 * Mantenemos al proceso genesis en la primera posicion-> pid = 0
 * y este es in-matable.
 * */
pcbPtr initProcessControl(char *name, uint64_t instruction)
{
    // validamos
    if (!name || ! instruction)
    {
        simple_printf("ERROR: initProcessControl: newPcb = null");
        return NULL;
    }

    // iniciamos el array de pcbPtrs
    arrSize = MAX_PROCS;
    maxPid = arrSize-1; // cond dinamica tiene sentido, aca no todo
    // creamos el nuevo proceso
    pcbPtr init = newProcess(name, instruction, PID_ERROR, INIT_PID, FALSE);
    if (!init) {
        simple_printf("ERROR: initProcessControl: Alguno es null\n");
        freeProcess(init);
        return NULL;
    }
    bussyWaitingProcPcb = newProcess("BussyWaiting", (uint64_t) bussyWaitingProc, INIT_PID, BUSSY_WAITING, FALSE);
    nextPid = BUSSY_WAITING + 1;
    return init;
}

/**
 * Función para el usuario(no userland)
 * simplemente crea un nuevo proceso y lo guarda
 * en el arreglo de procesos, si es posbile.
 * Y se lo agrega a la lista del papa
 * */
pcbPtr createProcess(char *name, uint64_t instruction, pPid parentPid, boolean foreground)
{
    if ( !instruction)
    {
        simple_printf("ERROR: createProcess: no tiene istrución\n");
        return NULL;
    }
    if (!procExists(parentPid) )
    {
        simple_printf("ERROR: createProcess: no es valid parent pid=%d\n",parentPid);
        return NULL;
    }
    pcbPtr newPcb = newProcess(name, instruction, parentPid, PID_ERROR, foreground);
    if (!newPcb)
    {
        simple_printf("ERROR: createProcess: newPcb = null\n");
        return NULL;
    }
    //simple_printf("createProcess: ret=%d, name =%s\n", newPcb, newPcb->name);
    return newPcb;
}

/**
 * Asume que los parametros están validados
 */
static boolean addChildToParentList(pPid parentPid, pPid childPid) {
    pcbPtr parent = array[parentPid];
    if (parent->childrenCount >= MAX_CHILDREN)
    {
        //programacion defensiva;
        parent->creationLimit++;
        if(parent->creationLimit > MAX_SECURITY_LIMITAION)
        {
            IRQ_OFF;
            reacursiveKill(parentPid);
            simple_printf("!Kernel: Matamos este proceso y sus hijos porque parece malicioso para el OS!!\n");
            IRQ_RES;
            setProcessState(parentPid, DEAD, NO_REASON);
        }
        simple_printf("addChildToParentList: ERROR: parent has already reached kids limit.... stop fornicating\n");
        return FALSE;
    }
    parent->childs[parent->childrenCount++] = childPid;
    return TRUE;
}

static void reacursiveKill(pPid pid) {
    if (!procExists(pid))
        return;
    pcbPtr proc = array[pid];
    for (int i = 0; i < proc->childrenCount; ++i) {
        reacursiveKill(array[proc->childs[i]]->pid);
    }
    proc->state = DEAD;
}

/**
 * Asume que los valores ya están validados
 * */
static pcbPtr newProcess(char *name, uint64_t instruction, pPid parentPid, int demandPid, boolean foreground)
{
    pcbPtr newPcb = my_malloc(sizeof(pcb));
    if (newPcb == NULL)
    {
        simple_printf("ERROR: newProcess newPcb = NULL\n");
        return NULL;
    }

    // le damos 2 página para su heap/stack // todo <--- lista de heaps
    newPcb->heapBase = (uint64_t) my_malloc(HEAP_STACK_SIZE);
    if (!newPcb->heapBase)
    {
        simple_printf("ERROR: newProcess newPcb->heap = NULL\n");
        my_free(newPcb);
        return NULL;
    }
    // apuntamos el stack a la direccion mas alta
    newPcb->stackBase = newPcb->heapBase + HEAP_STACK_SIZE - 1 ; // todo <---- no estaré pisando 1? -1

    pPid pid = arrayAdd(newPcb, demandPid);
    if (pid == PID_ERROR){
        simple_printf("newProcess: ERROR: PID_ERROR\n");
        freeProcess(newPcb);
        return NULL;
    }

    if (!name)
        simple_sprintf(newPcb->name, "%s_%d", DEFAULT_NAME, pid );
    else
        myStrncpy(newPcb->name, name, MAX_PROC_NAME);

    newPcb->pid = pid;
    newPcb->ppid = parentPid;
    newPcb->state = BORN;
    newPcb->creationLimit = 0;
    newPcb->foreground = foreground;
    newPcb->rsp = newPcb->stackBase - sizeof(stackFrame_t) + 1;
    newPcb->childrenCount = 0;
    newPcb->postBox = createNewMessageQueue();

    newPcb->stackFrame = (stackFrame_t *) newPcb->rsp;
    newPcb->stackFrame->rdi = instruction;
    newPcb->stackFrame->rip = (uint64_t) procContainer;
    newPcb->stackFrame->cs = CS_VALUE;
    newPcb->stackFrame->rflags = RFLAGS_VALUE;
    newPcb->stackFrame->rsp = newPcb->rsp;
    newPcb->stackFrame->ss = SS_VALUE;

    if(newPcb->pid != INIT_PID)
    {
        if (!addChildToParentList(parentPid, newPcb->pid))
        {
            simple_printf("Kernel message: No pudimos agregar este proceso como hijo\n");
            freeProcess(newPcb);
            return NULL;
        }
    }

    return newPcb;
}

boolean procExists(pPid pid)
{
    return isValidPid(pid) && array[pid] != NULL;
}

boolean isValidPid(pPid pid) {
    return (pid <= maxPid && pid > INIT_PID ) || pid == NO_PARENT;
}

static void freeProcess(pcbPtr proc)  // cada ves que agrego funcionalidad, aca tengo que liberar todo
{
    if (proc)
    {
        int i;
        msg_t * msg = NULL;
        for(i = 0; proc->postBox->count > 0;i++)
        {
            dequeueMessage(proc->postBox, msg);
            my_free(msg->content);
        }
        my_free(proc->postBox);
        if (isValidPid(proc->pid))
            array[proc->pid] = NULL;
        my_free((void *) proc->heapBase);
        my_free(proc);
    }
}

/**Asumimos que array y pscbPtr no son null.
 * Si demandPID cumple con requerimientos vamos a posicionar
 * psbPtr en ese pid sin importar de lo que haya ahí
 * Hay que tener cuidad con ese metodo dado que podría
 * cambiar el padre de un proceso
 * */
static pPid arrayAdd(pcbPtr pcbPtr, int demandPid)
{
    if (demandPid != PID_ERROR && isValidPid(demandPid))
    {
        array[demandPid] = pcbPtr;
        return demandPid;
    }
    pPid ret = getNextPid();
    if(ret != PID_ERROR)
        array[ret] = pcbPtr;

    return ret;
}

/**
 * el pid no es mas que el indice en el vector
 * */
static pPid getNextPid()
{
    pPid index = nextPid;
    do
    {
        // Si está vacio o está muerto, retornamos este pid para poner algo aca
        if(array[index] == NULL || array[index]->state == DEAD)
        {
            freeProcess(array[index]);
            nextPid = (nextPid % maxPid )+1;
            return index;
        }
        index = (index % maxPid )+1;
    }while (index != nextPid);
    simple_printf("Kernel message: ERROR: no hay mas pids disponibles\n");
    return PID_ERROR; // pegue la vuelta completa
}

boolean isValidPState(int state)
{
    for (int i = BORN; i <= DEAD; ++i) {
        if(state == i){
            return TRUE;
        }
    }
    return FALSE;
}

/**
 * Al cambiar, cuando el algoritmo de scheduling lo levante, decidirá que
 * hacer respecto al estado del proceso. */
boolean setProcessState(pPid pid, pState newState, reasonT reason)
{
    if (!procExists(pid) || !isValidPState(newState)){
        simple_printf("Kernel message: ERROR: pid is not valid \n");
        return FALSE;
    }
    if (newState == DEAD)
    {
        if (pid == INIT_PID && pid != getCurrentProc()->pid) // solo theAllMighty puede matar a theAllMighty
        {
            simple_printf("Kernel message: ERROR: You cannot kill %s process\n", array[INIT_PID]->name);
            return FALSE;
        }
        else
            simple_printf("Kernel message: process %d is now dead\n",pid);
    }
    // si es de background no quiero que tenga acceso al teclado
    if (newState == BLOCKED && reason == KEYBOARD && array[pid]->foreground == FALSE)
        newState = DEAD;

    array[pid]->state = newState;
    array[pid]->blockedReason = reason;
    //simple_printf("setProcessState: proc1=%s, state=%d\n", array[pid]->name, array[pid]->state);
    if (pid == getCurrentProc()->pid && (newState == BLOCKED || newState == DEAD))
    {
        _hlt();
        // aca no se va a llegar nunca. cuando entre la interrupcion el RR va  cambiar de proceso
        // y como este está muerto no lo va a levantar.... creo
    }
    return TRUE;
}

void printAllProcs()
{
    simple_printf(" PID - NAME - STATE - FOREGROUND - CHILD_COUNT - HEAP+STACK SIZE Bytes. \n b=BORN, r=READY, R=RUNNING, B=BLOCKED, D=DEAD\n");
    Queue *queue = createQueue(MAX_PROCS);
    enqueue(queue, INIT_PID);
    pPid current = dequeue(queue);
    printProc(array[current]);
    pcbPtr child;
    do
    {
        for(int i = 0; i < array[current]->childrenCount; ++i)
        {
            child = array[array[current]->childs[i]];
            printProc(child);
            enqueue(queue, child->pid);
        }
        current = dequeue(queue);
    }while (current != PID_ERROR);
    simple_printf("\n");
}

static void printProc(pcbPtr pcb)
{
    simple_printf(" %d - %s - %s - %s - %d - %d\n", pcb->pid, pcb->name, statesNames[pcb->state], pcb->foreground == TRUE? "F":"B" , pcb->childrenCount, HEAP_STACK_SIZE);
}

void printSons(pPid parentPid)
{
    if (!procExists(parentPid))
    {
        simple_printf("Kernel message: no such process %d\n", parentPid);
        return;
    }
    int childrenCount = array[parentPid]->childrenCount;
    if(childrenCount<=0)
    {
        simple_printf("Kernel message: process with pid %d has no sons.\n", parentPid);
        return;
    }
    simple_printf("Process %s has %d son%s\n", array[parentPid]->name, childrenCount, childrenCount==1?"":"s");
    for (int i = 0; i < childrenCount; ++i) {
        printProc(array[array[parentPid]->childs[i]]);
    }
}

/**
 * Corremos los procesos en estos containers para
 * no obligar al usuario a hacer un syscall exit cada
 * ves que mata un programa
 * de esta manera nos aseguramos que el sistema no muera
 * al hacer return
 * */
int procContainer(uint64_t inst)
{
    int(*func)(void) = (int (*)(void)) inst;
    int ret = func();
    simple_printf("Kernel Message: Process %s with pid %d died, with return=%d\n",getCurrentProc()->name, getCurrentProc()->pid, ret);

    procsDeathCleanUp(getCurrentProc());
    switchToNext(); // cambio forzado de contexto sin esperar al timer tick
    simple_printf("///////////////////procExit(): si llego hasta aca estamos en problemas\n");
    return ret;
}

/**
 * Si es padre, le asignamos los hijos a theAllMighty.
 * Si es hijo, el padre de dará cuenta cuando el contador de hijo se achica.
 * La liberación del espacio ocupado por el proceso la tendrá que hacer el scheduler.
 * */
static void procsDeathCleanUp(pcbPtr proc) // todo aca creo que hay un error
{
    // si es padre...
    for (int i = 0; i < proc->childrenCount; ++i) {
        pcbPtr child = array[proc->childs[i]];
        child->ppid = INIT_PID;                     // asiganmos el hijo a INIT_PID
        addChildToParentList(INIT_PID, child->pid); // a INIT le asignamos un hijo nuevo jeje re fiesta de egresados bariloche
    }
    // si es hijo lo sacamos de la lista del padre
    pcbPtr parent = array[proc->ppid];
    int i, j;
    for (i = j = 0; i < parent->childrenCount; ++j, ++i) {
        if(parent->childs[j] == proc->pid)
        {
            i++;
        }
        parent->childs[j] = parent->childs[i];
    }
    parent->childrenCount--;
    IRQ_OFF // si o si, porque no puede suceder una sin la otra
    proc->state = DEAD;
    freeProcess(proc);
    IRQ_RES;
}

int bussyWaitingProc()
{
    //simple_printf("!!!Bussy Wainting!!\n");
    STOP;
}

pcbPtr getBussyWaitingProcPcb()
{
    //return bussyWaitingProcPcb;
    return bussyWaitingProcPcb ;//= newProcess("BussyWaiting", (uint64_t) bussyWaitingProc, INIT_PID, BUSSY_WAITING, FALSE);
}

pcbPtr getPcbPtr(pPid pid)
{
    if(isValidPid(pid))
    {
        return array[pid];
    }
    return NULL;
}

boolean isAlive(pPid pid)
{
    if (!procExists(pid))
        return FALSE;
    return array[pid]->state != DEAD;
}