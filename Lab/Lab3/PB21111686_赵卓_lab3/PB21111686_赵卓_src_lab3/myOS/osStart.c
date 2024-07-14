#include "myPrintk.h"
#include "vga.h"
#include "i8253.h"
#include "i8259A.h"
#include "tick.h"
#include "irqs.h"
#include "wallClock.h"
void myMain(void);
void startShell(void);

void osStart(void)
{
	init8259A();
	init8253();
	tick();
	enableInterrupt();
	
    clear_screen();
    setWallClock(0, 0, 0);
    myPrintk(0x2, "START RUNNING......\n");
    myMain();
    myPrintk(0x2, "STOP RUNNING......ShutDown\n");
    while (1);
}
