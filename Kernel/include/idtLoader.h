#ifndef _idtLoader_H_
#define _idtLoader_H_

#include <stdint.h>
#include <defs.h>
#include <interrupts.h>
#include <sysCallHandler.h>
#include <system.h>
#include <system.h>

/* loads the interrupt handlers to de idt */
void loadIDT();

#endif
