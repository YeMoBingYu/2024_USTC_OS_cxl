#include "myPrintk.h"
#include "vga.h"
#include "uart.h"
#include "interrupt.h"
#include "wallClock.h"
#include "mem.h"
#include "task.h"
#include "scheduler.h"
#include "timer.h"

int startShellFlag = 0;


void pressAnyKeyToStart(void) {
	myPrintk(0x7, "\nPB211111686_ZZ >:\n");
	myPrintk(0x7, "\nEnter a number to choose the testcase:\n");
	myPrintk(0x6, "[1] - FCFS scheduling test\n");
	myPrintk(0x6, "[2] - RR   scheduling test - preemption , time slice %d seconds\n", TIME_SLICE);
	myPrintk(0x6, "[3] - SJF  scheduling test - no preemption \n");

	unsigned char input;

	while (1) {
		input = uart_get_char();
		if (input <= '3' && input >= '1')
			break;
	}
	switch (input) {
		case '1':
			sysSch.type = FCFS;
			break;
		case '2':
			sysSch.type = RR;
			break;
		case '3':
			sysSch.type = SJF;
			break;
	}
}

void osStart(void) {
	// before
	disable_interrupt();
	clear_screen();
	pressAnyKeyToStart();

	// init
	clear_screen();
	myPrintk(0x2, "Initializing the OS...\n");
	init8253();
	init8259A();
	pMemInit();
	init_timer();
	setWallClock(0, 0, 0);	

	myPrintk(0x2, "Starting the OS...\n");
	
	TaskManagerInit();
}
