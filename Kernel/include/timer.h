#ifndef TIMEDRIVER_H_
#define TIMEDRIVER_H_

#define TICKSPERSEC 18

extern unsigned int getTime();
extern unsigned int getTimeHour();
extern unsigned int getTimeMin();
extern unsigned int getTimeSec();

/* increases the ticks count */
void timerHandler(void);

/* returns the number of seconds instead of ticks */
unsigned long ticksToSeconds(void);

/* returns the ticks count */
unsigned long ticksElapsed(void);

/* returns the ticks count in form of seconds */
unsigned long secondsElapsed(void);

#endif
