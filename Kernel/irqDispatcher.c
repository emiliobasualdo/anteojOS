#include <stdint.h>
#include <keyboardDriver.h>
#include <videoDriver.h>
#include <timer.h>

static void int_20();
static void int_21();

void irqDispatcher(uint64_t irq)
{
	switch (irq)
	{
		case 0:
			int_20();
			break;
		case 1:
			int_21();
			break;
		default:break;
	}
}

void int_20() // todo esto no lo estoy usando, solucionar
{
	drawChar('.');
	timerHandler();
}

void int_21()
{
	if (keyboardInterpreter())
	{
		wakeUpBlocked(KEYBOARD);
	}
}
