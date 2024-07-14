#include "myPrintk.h" 

void enableInterrupt(void) {
	__asm__ __volatile__("Call enable_interrupt");
}

void disableInterrupt(void) {
	__asm__ __volatile__("Call disable_interrupt");
}

void ignoreIntBody(void){

    myPrintk(0x07,"Unknown interrupt\n\0");
}
