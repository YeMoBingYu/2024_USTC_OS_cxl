#ifndef __TICK_H__
#define __TICK_H___

#include "i8253.h"
#include "i8259A.h"

#define HZ_INTERRUPT 100

extern int system_ticks;
void tick(void);
void setWallClockHook(void (*func)(void));

#endif