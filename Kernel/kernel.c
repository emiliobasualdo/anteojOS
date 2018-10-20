#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <videoDriver.h>
#include <idtLoader.h>
#include <timer.h>
#include <scheduler.h>
#include <system.h>
#include <sleep.h>
#include <syscaller.h>
#include <ipc.h>
#include <shellTests.h>
#include <allocator.h>
#include <dinamicMemory.h>
#include <pageAllocator.h>
#include <shellTests.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;


static const uint64_t PageSize = 0x1000;

static void * sampleCodeModuleAddress = (void*)0x400000;
static void * sampleDataModuleAddress = (void*)0x500000;


void clearBSS(void * bssAddress, uint64_t bssSize)
{
    memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
    return (void*)(
            (uint64_t)&endOfKernel
            + PageSize * 8				//The size of the stack itself, 32KiB
            - sizeof(uint64_t)			//Begin at the top of the stack
    );
}

void * initializeKernelBinary() // todo ver clearBSS
{
    char buffer[10];

    ncPrint("[x64BareBones]");
    ncNewline();

    ncPrint("CPU Vendor:");
    ncPrint(cpuVendor(buffer));
    ncNewline();

    ncPrint("[Loading modules]");
    ncNewline();
    void * moduleAddresses[] = {
            sampleCodeModuleAddress,
            sampleDataModuleAddress
    };

    loadModules(&endOfKernelBinary, moduleAddresses);
    ncPrint("[Done]");
    ncNewline();
    ncNewline();

    ncPrint("[Initializing kernel's binary]");
    ncNewline();

    clearBSS(&bss, &endOfKernel - &bss); // todo esto vale la pena copiar?

    ncPrint("  text: 0x");
    ncPrintHex((uint64_t)&text);
    ncNewline();
    ncPrint("  rodata: 0x");
    ncPrintHex((uint64_t)&rodata);
    ncNewline();
    ncPrint("  data: 0x");
    ncPrintHex((uint64_t)&data);
    ncNewline();
    ncPrint("  bss: 0x");
    ncPrintHex((uint64_t)&bss);
    ncNewline();

    ncPrint("[Done]");
    ncNewline();
    ncNewline();
    return getStackBase();
}

void theAllMighty()
{
    simple_printf("The all mighty\n");
//    if (createAndExecProcess("shell", (uint64_t) sampleCodeModuleAddress, getCurrentProc()->pid, TRUE, INTERACTIVE) == PID_ERROR)
//    {
//        simple_printf("theAllMighty: ERROR: shell == NULL\n");
//        return;
//    }
    if (createAndExecProcess("p1", (uint64_t) semTest, getCurrentProc()->pid, TRUE, INTERACTIVE) == PID_ERROR)
    {
        simple_printf("theAllMighty: ERROR: shell == NULL\n");
        return;
    }
    simple_printf("theAllMighty: cargando IDT\n");
    loadIDT();
    setProcessState(getCurrentProc()->pid, BLOCKED, NO_REASON);
    simple_printf("theAllMighty: despues de bloquearse\n");
    simple_printf("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
    simple_printf("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");

    STOP;
}

int main()
{
    if (!initializeAllocator())
    {
        simple_printf("kernel: ERROR: initKernelAlloc retornó FALSE\n");
        return 0;
    }
    if(!initIPCS()) {
        simple_printf("kernel: ERROR: initIPCs retornó FALSE\n");
        return 0;
    }
    pcbPtr pacientCero = initScheduler("theAllMighty", (uint64_t) theAllMighty);
    if (!pacientCero)
    {
        simple_printf("kernel: ERROR: initScheduler retornó FALSE\n");
        return 0;
    }
    simple_printf("kernel: switchdinggg\n");
    switchToNext();
    simple_printf("kernel: volviendo de switch\n");

    simple_printf("\nX////////////////////////////XX\n");
    simple_printf("\nX////////////////////////////XX\n");
    return 0;
}