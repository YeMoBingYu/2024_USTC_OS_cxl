#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "task.h"

#define FCFS 0
#define PRIO 1
#define RR 2
#define SJF 3

typedef struct scheduler {
    unsigned int type;

	myTCB * (*nextTsk)(void);
	void (*enqueueTsk)(myTCB *tsk);
	myTCB * (*dequeueTsk)(void);
	void (*schedulerInit)(void);
	void (*schedule)(void);
	void (*tick_hook)(void);
} scheduler;

extern scheduler sysSch;

void initSysSch(void);


// FCFS
myTCB * nextTskFCFS(void);
void enqueueTskFCFS(myTCB *tsk);
myTCB * dequeueTskFCFS(void);
void schedulerInitFCFS(void);
void scheduleFCFS(void);


//RR
#define TIME_SLICE 2
myTCB * nextTskRR(void);
void enqueueTskRR(myTCB *tsk);
myTCB * dequeueTskRR(void);
void schedulerInitRR(void);
void scheduleRR(void);

void preemptCurrentTaskRR_hook(void);


//SJF
myTCB * nextTskSJF(void);
void enqueueTskSJF(myTCB *tsk);
myTCB * dequeueTskSJF(void);
void schedulerInitSJF(void);
void scheduleSJF(void);

#endif