#include "scheduler.h"
#include "types.h"
#include "vga.h"
#include "tick.h"
#include "task.h"
#include "timer.h"

scheduler sysSch;

void initSysSch(void) {
    switch (sysSch.type) {
        case FCFS:
            sysSch.schedulerInit = schedulerInitFCFS;
            sysSch.nextTsk = nextTskFCFS;
            sysSch.enqueueTsk = enqueueTskFCFS;
            sysSch.dequeueTsk = dequeueTskFCFS;
            sysSch.schedule = scheduleFCFS;
            sysSch.tick_hook = NULL;
            put_chars("FCFS", 0x6, VGA_SCREEN_HEIGHT - 1, VGA_SCREEN_WIDTH  / 2 - 2);
            break;
        case RR:
            sysSch.schedulerInit = schedulerInitRR;
            sysSch.nextTsk = nextTskRR;
            sysSch.enqueueTsk = enqueueTskRR;
            sysSch.dequeueTsk = dequeueTskRR;
            sysSch.schedule = scheduleRR;
            sysSch.tick_hook = preemptCurrentTaskRR_hook;
            append2HookList(preemptCurrentTaskRR_hook);
            put_chars(" RR ", 0x6, VGA_SCREEN_HEIGHT - 1, VGA_SCREEN_WIDTH  / 2 - 2);
            break;
        case SJF:
            sysSch.schedulerInit = schedulerInitSJF;
            sysSch.nextTsk = nextTskSJF;
            sysSch.enqueueTsk = enqueueTskSJF;
            sysSch.dequeueTsk = dequeueTskSJF;
            sysSch.schedule = scheduleSJF;
            sysSch.tick_hook = NULL;
            put_chars("SJF", 0x6, VGA_SCREEN_HEIGHT - 1, VGA_SCREEN_WIDTH  / 2 - 2);
            break;
    }
    sysSch.schedulerInit();
}


// FCFS
taskQueueFIFO *rdyQueueFCFS;

myTCB * nextTskFCFS(void) {
    return taskQueueFIFONext(rdyQueueFCFS);
}

void enqueueTskFCFS(myTCB *tsk) {
    taskQueueFIFOEnqueue(rdyQueueFCFS, tsk);
}

myTCB * dequeueTskFCFS(void) {
    return taskQueueFIFODequeue(rdyQueueFCFS);
}

void schedulerInitFCFS(void) {
    taskQueueFIFOInit(rdyQueueFCFS);
}

void scheduleFCFS(void) {
	while (1) {
		
		myTCB *nextTsk;
		int idleTid;

		if (taskQueueFIFOEmpty(rdyQueueFCFS)) {
			if (!idleTsk) 
				idleTid = createTsk(idleTskBody);

			nextTsk = idleTsk = tcbPool[idleTid];
		}
		else
			nextTsk = dequeueTskFCFS();

		if (nextTsk == idleTsk && currentTsk == idleTsk)
			continue; 
			
		if (currentTsk ) {
			if (currentTsk->para->exec_time * TICK_FREQ <= currentTsk->runTime || currentTsk == idleTsk)
				destroyTsk(currentTsk->tid);
			if (currentTsk == idleTsk)
				idleTsk = NULL;
		}

		nextTsk->state = TSK_RUNNING;
		currentTsk = nextTsk;
		
		context_switch(&BspContext, currentTsk->stkTop);
	}
}


// RR
taskQueueFIFO *rdyQueueRR;

myTCB * nextTskRR(void) {
    return taskQueueFIFONext(rdyQueueRR);
}

void enqueueTskRR(myTCB *tsk) {
    taskQueueFIFOEnqueue(rdyQueueRR, tsk);
}

myTCB * dequeueTskRR(void) {
    return taskQueueFIFODequeue(rdyQueueRR);
}

void schedulerInitRR(void) {
    taskQueueFIFOInit(rdyQueueRR);
}

void scheduleRR(void) {
	while (1) {

		myTCB *nextTsk;
		int idleTid;

		if (taskQueueFIFOEmpty(rdyQueueRR)) {
			if (!idleTsk) 
				idleTid = createTsk(idleTskBody);

			nextTsk = idleTsk = tcbPool[idleTid];
		}
		else
			nextTsk = dequeueTskRR();

		if (nextTsk == idleTsk && currentTsk == idleTsk) {
			continue;
		}
		
		if (currentTsk) {
			if (currentTsk->para->exec_time * TICK_FREQ <= currentTsk->runTime || currentTsk == idleTsk)
				destroyTsk(currentTsk->tid);
			if (currentTsk == idleTsk)
				idleTsk = NULL;
		}

		nextTsk->state = TSK_RUNNING;
		currentTsk = nextTsk;
		
		currentTsk->lastScheduledTime = get_current_time();
		context_switch(&BspContext, currentTsk->stkTop);
	}
}

void preemptCurrentTaskRR_hook(void) {
	if (currentTsk == idleTsk)
		return;
	
	if (currentTsk->runTime % (TIME_SLICE * TICK_FREQ) == 0 && get_current_time() > currentTsk->lastScheduledTime) {
		currentTsk->state = TSK_READY;
		enqueueTskRR(currentTsk);
		context_switch(&currentTsk->stkTop, BspContext);
	}
}

//SJF
taskQueuePrio *rdyQueueSJF;

int compare_exec_time(const myTCB *a, const myTCB *b) {
	if (getTskPara(EXEC_TIME, a->para) == getTskPara(EXEC_TIME, b->para))
		return getTskPara(ARRV_TIME, a->para) - getTskPara(ARRV_TIME, b->para);
	else
		return getTskPara(EXEC_TIME, a->para) - getTskPara(EXEC_TIME, b->para);
}

myTCB * nextTskSJF(void) {
    return taskQueuePrioNext(rdyQueueSJF);
}

void enqueueTskSJF(myTCB *tsk) {
    taskQueuePrioEnqueue(rdyQueueSJF, tsk);
}

myTCB * dequeueTskSJF(void) {
    return taskQueuePrioDequeue(rdyQueueSJF);
}

void schedulerInitSJF(void) {
    taskQueuePrioInit(&rdyQueueSJF, taskNum, compare_exec_time);
}

void scheduleSJF(void) {
	while (1) {
		
		myTCB *nextTsk;
		int idleTid;

		if (taskQueuePrioEmpty(rdyQueueSJF)) {
			if (!idleTsk) 
				idleTid = createTsk(idleTskBody);

			nextTsk = idleTsk = tcbPool[idleTid];
		}
		else
			nextTsk = dequeueTskSJF();

		if (nextTsk == idleTsk && currentTsk == idleTsk)
			continue; 
			
		if (currentTsk) {
			if (currentTsk->para->exec_time * TICK_FREQ <= currentTsk->runTime || currentTsk == idleTsk);
				destroyTsk(currentTsk->tid);
			if (currentTsk == idleTsk)
				idleTsk = NULL;
		}

		nextTsk->state = TSK_RUNNING;
		currentTsk = nextTsk;
		
		context_switch(&BspContext, currentTsk->stkTop);

	}
}
