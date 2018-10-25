//
// Created by Emilio Basualdo on 9/3/18.
//
#include <process.h>
#include <scheduler.h>

/////////////////////////////////

static boolean isValidPid(pPid pid);
static void freeProcess(pPid pid);
static pPid getNextPid();
static pPid newProcess(char *name, uint64_t instruction, pPid parentPid, int demandPid, boolean foreground, short priority,
                       char **argv, int argc);
static pPid arrayAdd(int demandPid);
static boolean addChildToParentList(pPid parentPid, pPid childPid);
static void procsDeathCleanUp(pPid proc);
static void printProc(pPid pid);
static boolean isValidPriority(int priority);
static void initChildVector(pPid pid);
static void initArray();


/** vector de pcbs, originalmetne este vector era de punteros a pcbs y estos se alocaban en rutime
 * pero luego de HORAS de debuggar problemas de pisado de memoria, la solución más rápida fue hacer
 * que el vector sea estático y listo */
static pcb array[MAX_PROCS]; // dinámica todo
static unsigned int arrSize;
static pPid maxPid;
static pPid nextPid; // siempre tiene el ultimo pid asignado
static char * statesNames[] = {"b", "r", "R", "B", "D"}; // lo puse aca porque en el .h me tiraba problemas de include
pPid bussyWaitingProcPcb;

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
    simple_printf("Kernel: initProcessControl: %s\n",name);
    // validamos
    if (!name || ! instruction)
    {
        simple_printf("ERROR: initProcessControl: newPcb = null");
        return NULL;
    }
    initArray();
    // iniciamos el array de pcbPtrs
    arrSize = MAX_PROCS;
    maxPid = arrSize-1; // cond dinamica tiene sentido, aca no todo
    // creamos el nuevo proceso
    pPid init = newProcess(name, instruction, PID_ERROR, INIT_PID, TRUE, MAX_PRIORITY, NULL, 0);
    if (init == PID_ERROR)
    {
        simple_printf("ERROR: initProcessControl: Alguno es null\n");
        return NULL;
    }
    bussyWaitingProcPcb = newProcess("BussyWaiting", (uint64_t) bussyWaitingProc, INIT_PID, BUSSY_WAITING, FALSE,
                                     MAX_PRIORITY, NULL, 0);
    nextPid = BUSSY_WAITING + 1;
    return &array[init];
}

void initArray()
{
    for (int i = 0; i < MAX_PROCS; ++i)
    {
        array[i].state = UN_BORN;
    }
}
/**
 * Función para el usuario(no userland)
 * simplemente crea un nuevo proceso y lo guarda
 * en el arreglo de procesos, si es posbile.
 * Y se lo agrega a la lista del papá
 * */
pcbPtr createProcess(char *name, uint64_t instruction, pPid parentPid, boolean foreground, short priority, char **argv,
                     int argc)
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
    if(!isValidPriority(priority))
    {
        simple_printf("ERROR: %d is not a valid priority\n",priority);
        return NULL;
    }
    pPid newPid = newProcess(name, instruction, parentPid, PID_ERROR, foreground, priority, argv, argc);
    if (newPid == PID_ERROR)
    {
        //simple_printf("ERROR: createProcess: newPcb = null\n");
        return NULL;
    }
    DEBUG //simple_printf("createProcess: pid=%d, name=%s priority=%d prioritiT=%d\n", array[newPid].pid, array[newPid].name, array[newPid].priority, array[newPid].priorityType);
    return &(array[newPid]);
}

/**
 * Asume que los parametros están validados
 */
static boolean addChildToParentList(pPid parentPid, pPid childPid)
{
    DEBUG //simple_printf(" addChildToParentList parentPid %d, childPid %d\n", parentPid,  childPid);
    pcbPtr parent = &array[parentPid];
    if (parent->childrenCount >= MAX_CHILDREN)
    {
        simple_printf("Kernel Message: ERROR: parent has already reached kids limit.... stop fornicating\n");
        return FALSE;
    }
    for (int i = 0; i < MAX_CHILDREN; ++i)
    {
        if(parent->childs[i] == PID_ERROR )
        {
            parent->childs[i] = childPid;
            parent->childrenCount++;
            return TRUE;
        }
        else if(array[parent->childs[i]].state == UN_BORN)
        {
            parent->childs[i] = childPid;
            return TRUE;
        }
        else if(array[parent->childs[i]].state == DEAD)
        {
            parent->childs[i] = childPid;
            return TRUE;
        }
    }
    for (int i = 0; i < MAX_CHILDREN; ++i)
    {
        if(parent->childs[i] == PID_ERROR )
        {
            parent->childs[i] = childPid;
            parent->childrenCount++;
            return TRUE;
        }
        else if(array[parent->childs[i]].state == UN_BORN)
        {
            parent->childs[i] = childPid;
            return TRUE;
        }
        else if(array[parent->childs[i]].state == DEAD)
        {
            procsDeathCleanUp(parent->childs[i]);
            parent->childs[i] = childPid;
            return TRUE;
        }
    }
    //parent->childs[parent->childrenCount++] = childPid;
    return FALSE;
}

/**
 * Asume que las interrupciones estan apagadas*/
void killAllDescendants(pPid pid) {
    if (!procExists(pid))
        return;
    pPid childPid;
    pcb proc = array[pid];
    for (int i = 0; i < proc.childrenCount; ++i) {
        killAllDescendants(array[proc.childs[i]].pid);
        childPid = proc.childs[i];
        if(procExists(childPid))
        {
            if(childPid == pid)
                simple_printf("ERROROROROROROOR\n");
            setProcessState(childPid,DEAD, NO_REASON);
        }
    }
}

/**
 * Asume que los valores ya están validados
 * Si la prioridad es una de las constantes la tomames como un aviso
 * de tipo de prioridad.
 * Sino se le asigna ese numero
 */
static pPid newProcess(char *name, uint64_t instruction, pPid parentPid, int demandPid, boolean foreground, short priority,
                       char **argv, int argc)
{
    DEBUG //simple_printf("Kernel: newProcess name %s parentPid %d demand %d\n",name,parentPid, demandPid);

    pPid pid = arrayAdd(demandPid);
    if (pid == PID_ERROR)
    {
        simple_printf("newProcess: ERROR: PID_ERROR\n");
        return PID_ERROR;
    }
    pcbPtr newPcb = &array[pid];

    // le damos 2 página para su heap/stack // todo <--- lista de heaps
    newPcb->heapBase = (uint64_t) kernelMalloc(HEAP_STACK_SIZE);
    if (!newPcb->heapBase)
    {
        simple_printf("ERROR: newProcess newPcb->heap = NULL\n");
        return PID_ERROR;
    }
    // apuntamos el stack a la direccion mas alta
    newPcb->stackBase = newPcb->heapBase + HEAP_STACK_SIZE - 1 ; // todo <---- no estaré pisando 1? -1

    if (!name)
        simple_sprintf(newPcb->name, "%s_%d", DEFAULT_NAME, pid );
    else
        myStrncpy(newPcb->name, name, MAX_PROC_NAME);

    DEBUG //simple_printf("cc");
    newPcb->pid = pid;
    newPcb->ppid = parentPid;
    newPcb->state = BORN;
    newPcb->creationLimit = 0;
    newPcb->foreground = foreground;
    newPcb->rsp = newPcb->stackBase - sizeof(stackFrame_t) + 1;
    newPcb->childrenCount = 0;
    initChildVector(newPcb->pid);

    DEBUG //simple_printf("%d", priority);
    if(priority == INTERACTIVE)
    {
        newPcb->priorityType = INTERACTIVE;
        newPcb->priority = MAX_PRIORITY;
    }
    else if(priority == DO_NOT_CHANGE)
    {
        newPcb->priorityType = DO_NOT_CHANGE;
        newPcb->priority = priority;
    }
    else
    {
        newPcb->priorityType = NORMAL;
        newPcb->priority = priority;
    }

    newPcb->stackFrame = (stackFrame_t *) newPcb->rsp;

    // https://aaronbloomfield->github->io/pdr/book/x86-64bit-ccc-chapter->pdf
    //To pass parameters to the subroutine, we put up to six of them into registers (in order: rdi, rsi,rdx, rcx, r8, r9)
    newPcb->stackFrame->rdi = instruction;
    newPcb->stackFrame->rsi = (uint64_t) argv;
    newPcb->stackFrame->rdx = (uint64_t) argc;

    newPcb->stackFrame->rip = (uint64_t) procContainer;
    newPcb->stackFrame->cs = CS_VALUE;
    newPcb->stackFrame->rflags = RFLAGS_VALUE;
    newPcb->stackFrame->rsp = newPcb->rsp;
    newPcb->stackFrame->ss = SS_VALUE;

    DEBUG //simple_printf("ll");
    if(newPcb->pid > BUSSY_WAITING )
    {
        DEBUG //simple_printf("mm");
        //newPcb->postBox = createNewMessageQueue();
        newPcb->fd[STDIN] = createPipeK()->pipeId;
        newPcb->fd[STDOUT] = createPipeK()->pipeId;
        DEBUG //simple_printf("aa");

    }
    if(newPcb->pid > INIT_PID)
    {
        DEBUG //simple_printf("pp");
        if (!addChildToParentList(parentPid, newPcb->pid))
        {
            DEBUG //simple_printf("aa");
            simple_printf("Kernel message: Unable to create son for process %s\n", array[parentPid].name);
            freeProcess(newPcb->pid);
            return PID_ERROR;
        }
    }
    DEBUG //simple_printf("jj\n");
    return newPcb->pid;
}
static void initChildVector(pPid pid)
{
    for (int i = 0; i < MAX_CHILDREN; ++i)
    {
        array[pid].childs[i] = PID_ERROR;
    }
}

boolean procExists(pPid pid)
{
    //return isValidPid(pid) && array[pid] != NULL;
    if(!isValidPid(pid))
    {
        //simple_printf("INVALID\n");
        return FALSE;
    }
    if(array[pid].state == UN_BORN)
    {
        //simple_printf("NULO\n");
        return FALSE;
    }
    //simple_printf("PASA\n");
    return TRUE;
}

boolean isValidPid(pPid pid) {
    return (pid <= maxPid && pid >= INIT_PID ) || pid == NO_PARENT;
}

static void freeProcess(pPid pid)  // cada ves que agrego funcionalidad, aca tengo que liberar todo
{
    if (isValidPid(pid))
    {
/*        int i;
        msg_t * msg = NULL;
        for(i = 0; proc->postBox->count > 0;i++)
        {
            dequeueMessage(proc->postBox, msg);
            if(!msg)
            {

            }
            //kernelFree(msg->content); // todo solucionar esto
        }*/
        //kernelFree(array[pid].postBox);
        kernelFree((void *) array[pid].heapBase);
        array[pid].state = UN_BORN;
    }
}

/**
 * Si demandPID cumple con requerimientos vamos a retornar
 * ese pid sin importar de lo que haya ahí
 * Hay que tener cuidad con ese metodo dado que podría
 * cambiar el padre de un proceso
 * */
static pPid arrayAdd(int demandPid)
{
    if (demandPid != PID_ERROR && isValidPid(demandPid))
    {
        return demandPid;
    }
    return getNextPid();
}

/**
 * el pid no es mas que el indice en el vector
 * */
static pPid getNextPid()
{
    pPid index = nextPid;
    do
    {
        switch (array[index].state)
        {
            // Si está vacio o está muerto, retornamos este pid para poner algo aca
            case DEAD:
                freeProcess(index); // sigo de largo
            case UN_BORN:
                nextPid = (nextPid % maxPid )+1;
                return index;
            default:
                break;
        }
        index = (index % maxPid )+1;
    }while (index != nextPid);
    simple_printf("Kernel message: ERROR: no hay mas pids disponibles\n");
    return PID_ERROR; // pegue la vuelta completa
}

boolean isValidPState(int state)
{
    return (state >= BORN && state <= DEAD);
}

/** Si es necesrio, notificamos al algoritmo de scheduling para que haga lo necesario */
boolean setProcessState(pPid pid, pState newState, reasonT reason)
{
    DEBUG //simple_printf("pid=%d, name=%s, pid=%d, newState=%d, reason=%d\n",getCurrentProc()->pid, getCurrentProc()->name, pid, newState, reason);
    DEBUG //printAllProcs();
    if (!procExists(pid) ){
        simple_printf("Kernel message:set: ERROR: pid %d is not valid \n", pid);
        return FALSE;
    }
    if (!isValidPState(newState)){
        simple_printf("Kernel message: ERROR: state %d is not valid \n", newState);
        return FALSE;
    }
    // solo 1 procesos puede estar running y el
    if(newState == RUNNING)
    {
        simple_printf("Kernel message: ERROR: only the scheduler can set a process to RUNNING \n");
    }
    // si no hay cambio no tiene sentido aplicarlo
    if(array[pid].state == newState)
        return FALSE;

    if (newState == DEAD)
    {
        if (pid == INIT_PID && pid != getCurrentProc()->pid) // solo theAllMighty puede matar a theAllMighty
        {
            simple_printf("Kernel message: ERROR: You cannot kill %s process\n", array[INIT_PID].name);
            return FALSE;
        }
        else
            simple_printf("Kernel message: process %d is now dead\n",pid);
    }
    // si es de background no quiero que tenga acceso al teclado
    if (newState == BLOCKED && reason == KEYBOARD && array[pid].foreground == FALSE)
        newState = DEAD;

    array[pid].state = newState;
    array[pid].blockedReason = reason;
    DEBUG //simple_printf("setProcessState: notificamos\n");
    schedulerNotifyProcessStateChange(pid);
    //simple_printf("setProcessState: proc=%s, state=%d\n", array[pid]->name, array[pid]->state);
    if (pid == getCurrentProc()->pid && (newState == BLOCKED || newState == DEAD))
    {
        DEBUG //simple_printf("setProcessState: blocking/Deading el current\n");
        //_hlt();
        asm volatile ("int $20");
        DEBUG //simple_printf("setProcessState: volvimos %d\n", getCurrentProc()->state);
    }
    return TRUE;
}

/** Lo cambia casi sin chequeos ni notificaciones
 * HAY QUE TENER CUIDADO CUANDO LA USAMOS!!!!!! PUEDE EXPLOTAR TODO
 */
boolean directSetProcessState(pPid pid, pState newState, reasonT reason)
{
    if (!procExists(pid)){
        simple_printf("Kernel message:direct: ERROR: pid %d is not valid , pid \n");
        return FALSE;
    }
    if(!isValidPState(newState))
    {
        simple_printf("Kernel message: ERROR: not valid state \n");
        return FALSE;
    }
    array[pid].state = newState;
    array[pid].blockedReason = reason;
    return TRUE;
}

void printAllProcs()
{
    simple_printf(" b=BORN, r=READY, R=RUNNING, B=BLOCKED, D=DEAD, N=NORMAL, I=INTERACTIVE, D=NO CHANGE\n");
    simple_printf(" PID - NAME - STATE - FOREGROUND - PRIORITY - PRIORITY STATE - RUN TIME - CHILD_COUNT - HEAP+STACK SIZE Bytes. \n");
    for (int i = 0; i < MAX_PROCS; ++i)
    {
        if(array[i].state != UN_BORN)
            printProc(array[i].pid);
        else
            return;
    }
    simple_printf("\n");
}

static void printProc(pPid pid)
{
    char priType, foreBack;
    foreBack = (char) (array[pid].foreground == TRUE ? 'F' : 'B');

    if (array[pid].priorityType == INTERACTIVE)
        priType = 'I';
    else if (array[pid].priorityType == DO_NOT_CHANGE)
        priType = 'D';
    else
        priType = 'N';

    simple_printf(" %2d - %3s - %3s - %3c - %3d - %3c - %3d - %3d - %3d\n", array[pid].pid, array[pid].name, statesNames[array[pid].state], foreBack, array[pid].priority, priType, getProcRunTime(pid), array[pid].childrenCount, HEAP_STACK_SIZE);
}

void printSons(pPid parentPid)
{
    if (!procExists(parentPid))
    {
        simple_printf("Kernel message: no such process %d\n", parentPid);
        return;
    }
    int childrenCount = array[parentPid].childrenCount;
    if(childrenCount<=0)
    {
        simple_printf("Kernel message: process with pid %d has no sons.\n", parentPid);
        return;
    }
    simple_printf("Process %s has %d son%s\n", array[parentPid].name, childrenCount, childrenCount==1?"":"s");
    for (int i = 0; i < childrenCount; ++i) {
        printProc(array[array[parentPid].childs[i]].pid);
    }
}

/**
 * Corremos los procesos en estos containers para
 * no obligar al usuario a hacer un syscall exit cada
 * ves que mata un programa
 * de esta manera nos aseguramos que el sistema no muera
 * al hacer return
 * */
int procContainer(uint64_t inst, char **argv, int argc)
{
    int(*func)(char **, int) = (int (*)(char **, int)) inst;
    int ret = func(argv,argc);
    simple_printf("pid=%d [%d]\n",getCurrentProc()->pid, ret);

    procsDeathCleanUp(getCurrentProc()->pid);
    switchToNext(); // cambio forzado de contexto sin esperar al timer tick
    simple_printf("///////////////////procExit(): si llego hasta aca estamos en problemas\n");
    return ret;
}

/**
 * Si es padre, le asignamos los hijos a theAllMighty.
 * Si es hijo, el padre de dará cuenta cuando el contador de hijo se achica.
 * La liberación del espacio ocupado por el proceso la tendrá que hacer el scheduler.
 * */
static void procsDeathCleanUp(pPid proc) // todo aca creo que hay un error
{
    // si es padre...
    for (int i = 0; i < array[proc].childrenCount; ++i)
    {
        if (procExists(array[proc].childs[i]))
        {
            if (array[array[proc].childs[i]].state < DEAD)
            {
                pPid child = array[proc].childs[i];
                if (false == addChildToParentList(INIT_PID, child))
                {
                    simple_printf("Kernel message: ERROR: a %s no le podemos asignar más hijos\n", array[INIT_PID].name);
                    setProcessState(child, DEAD, NO_REASON);// a INIT le asignamos un hijo nuevo
                }
                else
                    array[child].ppid = INIT_PID; // asiganmos el hijo a INIT_PID
            }
        }
    }
    // si es hijo NO lo sacamos de la lista del padre cosa de que siga apareciendo en el contandor
    setProcessState(proc,DEAD,NO_REASON);
}

int bussyWaitingProc()
{
    //simple_printf("!!!Bussy Wainting!!\n");
    STOP;
}

pcbPtr getBussyWaitingProcPcb()
{
    //return bussyWaitingProcPcb;
    return &array[bussyWaitingProcPcb] ;//= newProcess("BussyWaiting", (uint64_t) bussyWaitingProc, INIT_PID, BUSSY_WAITING, FALSE);
}

pcbPtr getPcbPtr(pPid pid)
{
    //simple_printf("pid %d", pid);
    if(isValidPid(pid))
    {
        return &array[pid];
    }
    return NULL;
}

boolean validReason(int reason)
{
    return reason >= 0 && reason < REASON_COUNT;
}

boolean setProcessPriority(pPid pid, short newPriority, pReason reason)
{
    if(!procExists(pid))
    {
        simple_printf("Kernel: ERROR pid %d does not exist\n", pid);
        return FALSE;
    }
    if(!isValidPriority(newPriority))
    {
        simple_printf("Kernel: ERROR invalid priority %d\n", newPriority);
        return FALSE;
    }
    if(array[pid].priorityType == INTERACTIVE || array[pid].priority == newPriority )
        return FALSE;

    // si alguien le cambio la prioridad no queremos que decisiones del kernel le bajen la prioridad
    if(reason == NICE)
        array[pid].priorityType = DO_NOT_CHANGE;

    array[pid].priority = newPriority;
    schedulerNotifyProcessPriorityChange(pid);
    return TRUE;
}

static boolean isValidPriority(int priority) {
    return (priority >= 0 && priority < PRIORITY_LEVELS) ||
            priority == INTERACTIVE ||
            priority == NORMAL ||
            priority == DO_NOT_CHANGE;
}

/** Si es posible reducimos la prioridad en 1*/
boolean reduceProcessPriority(pPid pid)
{
    if(isValidPid(pid) && array[pid].priority < MIN_PRIORITY)
    {
        // reducimos prioridad
        //int old =array[pid].priority;
        array[pid].priority++;
        //simple_printf("reducimos priordad pid=%d old_priority=%d new_priority=%d!!!\n", pid, old, array[pid].priority);
        return TRUE;
    }
    return FALSE;
}
