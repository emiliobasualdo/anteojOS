//
// Created by Emilio Basualdo on 9/3/18.
//
#include <scheduler.h>


static uint64_t schedulerNextRsp(uint64_t rsp);
static boolean  schedulerAddProc(pcbPtr proc);
static pcbPtr nextProcess();

static pcbPtr current;

/**
 * Se encargará de hacer chequeos de nulos y validaciones
 * La idea es que se puedan implementar varios tipos
 * de desciciones.
 * */
pcbPtr initScheduler(char *name, uint64_t instruction)
{

    simple_printf("initScheduler : iniciando %s\n", name);
    if (!name || !instruction) {
        simple_printf("ERROR: initScheduler : pacientCero == NULL\n");
        return NULL;
    }

    pcbPtr pacientCero = initProcessControl(name, (uint64_t) instruction);

    if (rrInit(pacientCero) == FALSE) {
        simple_printf("ERROR: initScheduler : rrInit(pacientCero) == FALSE\n");
        return NULL;
    }
    current = pacientCero;
    return pacientCero;
}

uint64_t convert(uint64_t dec)
{
    if (dec == 0)
    {
        return 0;
    }
    else
    {
        return (dec % 2 + 10 * convert(dec / 2));
    }
}

void printFlags(uint64_t flags)
{
   simple_printf("\nprintFlags: FLAGSDEC= %d, FLAGSBIN= %d\n",flags, convert(flags));
}

static uint64_t schedulerNextRsp(uint64_t rsp)
{
    current->rsp = rsp; // le guardamos su ultimo rsp
    pcbPtr nextProc = nextProcess();
    return nextProc->rsp;
}

/** Si no hay ningun proceso para correr nos quedamos
 * loopeando infintamente
 */
static pcbPtr nextProcess()
{
    current = rrNextAvailableProcess();
    if (!current)
    {
        current = getBussyWaitingProcPcb();
    }
    return current;
}

uint64_t dispatcher(uint64_t rsp)
{
    uint64_t resp = schedulerNextRsp(rsp);
    return resp;
}

pcbPtr getCurrentProc()
{
    return current;
}

pPid createAndExecProcess(char *name, uint64_t instruction, pPid parent, boolean foreground)
{
    pcbPtr newProc = createProcess(name, instruction, parent, foreground);
    if(!newProc)
    {
        simple_printf("createAndExecProcess: !newProc\n");
        return PID_ERROR;
    }

    if (!schedulerAddProc(newProc))
    {
        simple_printf("createAndExecProcess: !schedulerAddProc(newProc)\n");
        return PID_ERROR;
    }
    return newProc->pid;
}

static boolean schedulerAddProc(pcbPtr proc)
{
    if(proc == NULL)
    {
        simple_printf("createAndExecProcess: ERROR: proc == NULL\n");
        return FALSE;
    }
    boolean resp = rrAddProcess(proc);
    return resp;
}

void printtt(uint64_t num)
{
    simple_printf("printt: num= %d\n", num);
}

/**
 * Fuerza un cambio de contexto sin esperar a una interrupcion
 * de timer tick.
 * Al ser forzado nunca se volverá al estado anterior, por ende
 * al usar esta funcion se debe estár seguro que no se desea retornar
 * al estado anterior.
 * Si se desea retornar al estado anterior usat la funcion haltcpu()
 */
void switchToNext()
{
    pcbPtr proc = nextProcess();
    //simple_printf("switchToNext: cambiando por: name=%s \n", proc->name);
    switchTo(proc->rsp);
    simple_printf("!!!!!Volviendo de switchToNext..... No deberiamos estar aca!!!!!\n");
}

boolean wakeUpBlocked(reasonT reason)
{
    switch (reason) // todo limpiar esto
    {
        case KEYBOARD:
            return rrUnblockWaiters(reason); // me lo agrega a la cabeza
        default:
            simple_printf("wakeUpBlocked: hubo una reason fuera de lo normla reason=%d\n", reason);
    }
    return FALSE;
}

void printProcQueues()
{
    printRRQueues();
}