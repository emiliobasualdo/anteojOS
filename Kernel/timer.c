#include <timer.h>

static unsigned long long ticks = 0;

void timerHandler()
{
    ticks++;
}

unsigned long ticksToSeconds()
{
    return ticks / TICKSPERSEC; // 18 ticks por segundo => 0.018 ticks por milisegundo
}

unsigned long ticksElapsed()
{
    return ticks;
}

unsigned long secondsElapsed()
{
    return ticksToSeconds();
}
