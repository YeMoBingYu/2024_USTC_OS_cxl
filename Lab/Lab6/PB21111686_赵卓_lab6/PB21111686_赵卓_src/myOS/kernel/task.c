#include "task.h"
#include "timer.h"
#include "myPrintk.h"
#include "kmalloc.h"
#include "types.h"
#include "tick.h"
#include "interrupt.h"
#include "scheduler.h"


unsigned long **prevTSK_StackPtrAddr;
unsigned long *nextTSK_StackPtr;
myTCB **tcbPool; 
myTCB *firstFreeTCB; 
myTCB *idleTsk; 
myTCB *currentTsk; 

unsigned long *BspContextBase;
unsigned long *BspContext;
unsigned int taskNum;


void context_switch(unsigned long **prevTskStkAddr, unsigned long *nextTskStk) {
	prevTSK_StackPtrAddr = prevTskStkAddr;
	nextTSK_StackPtr = nextTskStk;
	CTX_SW();
}

void tskEnd(void);


void stack_init(unsigned long **stk, void (*task)(void)) {
	*(*stk)-- = (unsigned long)tskEnd; 

	*(*stk)-- = (unsigned long)task;

	*(*stk)-- = (unsigned long)0x0202; 

	*(*stk)-- = (unsigned long)0xAAAAAAAA; 
	*(*stk)-- = (unsigned long)0xCCCCCCCC; 
	*(*stk)-- = (unsigned long)0xDDDDDDDD; 
	*(*stk)-- = (unsigned long)0xBBBBBBBB; 
	*(*stk)-- = (unsigned long)0x44444444; 
	*(*stk)-- = (unsigned long)0x55555555; 
	*(*stk)-- = (unsigned long)0x66666666; 
	**stk     = (unsigned long)0x77777777; 
}

int createTsk(void (*tskBody)(void)) {
	if (!firstFreeTCB)
		return -1;
	
	myTCB *newTsk = firstFreeTCB;
	firstFreeTCB = firstFreeTCB->next;

	initTskPara(&newTsk->para);
	newTsk->runTime = 0;
	newTsk->entry = tskBody;
	newTsk->lastScheduledTime = 0;
	newTsk->stkMAX = (unsigned long *)kmalloc(STACK_SIZE);
	if (!newTsk->stkMAX)
		return -1;	
	newTsk->stkTop = newTsk->stkMAX + STACK_SIZE - 1;
	
	stack_init(&newTsk->stkTop, tskBody);

	return newTsk->tid;
}


void destroyTsk(int tid) {
	kfree((unsigned long)tcbPool[tid]->stkMAX); 
	kfree((unsigned long)tcbPool[tid]->para);
	tcbPool[tid]->runTime = 0;
	tcbPool[tid]->lastScheduledTime = 0;
	tcbPool[tid]->entry = NULL;
	tcbPool[tid]->state = TSK_WAITING;
	tcbPool[tid]->stkMAX = NULL;
	tcbPool[tid]->stkTop = NULL;
	tcbPool[tid]->next = firstFreeTCB;
	firstFreeTCB = tcbPool[tid];
}


void tskStart(int tid) {
	tcbPool[tid]->state = TSK_READY;
	sysSch.enqueueTsk(tcbPool[tid]);
}

void tskEnd(void) {
	context_switch(&currentTsk->stkTop, BspContext);
}

void schedule(void) {
	sysSch.schedule();
}


void startMultitask(void) {
	BspContextBase = (unsigned long *)kmalloc(10 * STACK_SIZE);
	BspContext = BspContextBase + STACK_SIZE - 1;
	currentTsk = NULL;
	schedule();
}

void idleTskBody(void) {
	myPrintk(0x8, "*********************************\n");
	myPrintk(0x8, "*              IDLE             *\n");
	myPrintk(0x8, "*********************************\n");
}

void updateCurrentTskRunTime_hook(void) {
	disable_interrupt();
	currentTsk->runTime++;
	enable_interrupt();
}

void TaskManagerInit(void) {

	switch (sysSch.type) {
		case FCFS:
			taskNum = FCFS_TASK_NUM + 2;
			break;
		case PRIO:
			taskNum = PRIO_TASK_NUM + 2;
			break;
		case RR:
			taskNum = RR_TASK_NUM + 2;
			break;
		case SJF:
			taskNum = SJF_TASK_NUM + 2;
			break;
	}

	myTCB *prevTCB = NULL;
	tcbPool = (myTCB **)kmalloc(taskNum * sizeof(myTCB *));
	for (int i = 0; i < taskNum; i++) {
		tcbPool[i] = (myTCB *)kmalloc(sizeof(myTCB));
		tcbPool[i]->tid = i;
		tcbPool[i]->entry = NULL;
		tcbPool[i]->state = TSK_WAITING;
		tcbPool[i]->stkMAX = NULL;
		tcbPool[i]->stkTop = NULL;
		tcbPool[i]->next = NULL;
		tcbPool[i]->para = NULL;
		tcbPool[i]->runTime = 0;
		tcbPool[i]->lastScheduledTime = 0;
		if (prevTCB) 
			prevTCB->next = tcbPool[i];
		prevTCB = tcbPool[i];
	}
	firstFreeTCB = tcbPool[0];

	initSysSch();
	taskArrvQueueInit();

	int initTid = createTsk(initTskBody);
	idleTsk = NULL;
	tskStart(initTid);
	enable_interrupt();

	append2HookList(updateCurrentTskRunTime_hook);
	append2HookList(showCurrentTskParaInfo_hook);
	
	startMultitask();
}

taskQueuePrio *arrvQueue;

int compare_arrv_time(const myTCB *a, const myTCB *b) {
    return getTskPara(ARRV_TIME, a->para) - getTskPara(ARRV_TIME, b->para);
}

void startArrivedTask_hook(void) {
	if (taskQueuePrioEmpty(arrvQueue))
		return;

	myTCB *nextTask = taskQueuePrioNext(arrvQueue);
	if (get_current_time() >= getTskPara(ARRV_TIME, nextTask->para)) {
		tskStart(nextTask->tid);
		taskQueuePrioDequeue(arrvQueue);
	}
}

void taskArrvQueueInit(void) {
	taskQueuePrioInit(&arrvQueue, taskNum, compare_arrv_time);
	append2HookList(startArrivedTask_hook);
}

void enableTask(int tid) {
	if (tcbPool[tid]->para->arrv_time == 0)
		tskStart(tid);
	else
		taskQueuePrioEnqueue(arrvQueue, tcbPool[tid]);
}

extern int startShellFlag;

void task_execute(unsigned int wait_time) {
	while (currentTsk->runTime < getTskPara(EXEC_TIME, currentTsk->para) * TICK_FREQ);
}

void showCurrentTskParaInfo_hook(void) {
	if (get_tick_times() % TICK_FREQ != 0 || startShellFlag)
		return;

	if (currentTsk == idleTsk) 
		return;

	myPrintk(0x7, "**************************************\n");
	myPrintk(0x7, "   TID : %-2d        \n", currentTsk->tid);
	myPrintk(0x7, "   Arrive Time   : %-2d    \n", getTskPara(ARRV_TIME, currentTsk->para));
	myPrintk(0x7, "   Execute Time  : %-2d / %2d \n", (currentTsk->runTime + 50) / TICK_FREQ, getTskPara(EXEC_TIME, currentTsk->para));
	myPrintk(0x7, "**************************************\n");
}

void initTskPara(tskPara **para) {
	*para = (tskPara *)kmalloc(sizeof(tskPara));
	(*para)->arrv_time = 0;
	(*para)->exec_time = 0;
	(*para)->priority = 0;
}

void setTskPara(unsigned int option, unsigned int value, tskPara *para) {
	switch (option) {
		case PRIORITY:
			para->priority = value > MAX_PRIORITY ? MAX_PRIORITY : value;
			break;
		case EXEC_TIME:
			para->exec_time = value > MAX_EXEC_TIME ? MAX_EXEC_TIME : value;
			break;
		case ARRV_TIME:
			para->arrv_time = value;
			break;
		default:
			break;
	}
}

unsigned int getTskPara(unsigned int option, tskPara *para) {
	switch (option) {
		case PRIORITY:
			return para->priority;
		case EXEC_TIME:
			return para->exec_time;
		case ARRV_TIME:
			return para->arrv_time;
		default:
			return 0;
	}
}

void taskQueueFIFOInit(taskQueueFIFO *queue) {
    queue->head = queue->tail = NULL;
}

int taskQueueFIFOEmpty(taskQueueFIFO *queue) {
    return (queue->head == NULL && queue->tail == NULL);
}

myTCB * taskQueueFIFONext(taskQueueFIFO *queue) {
	if (taskQueueFIFOEmpty(queue))
		return NULL;
	else
    	return queue->head->TCB;
}

void taskQueueFIFOEnqueue(taskQueueFIFO *queue, myTCB *tsk) {
    queueNodeFIFO *newNode = (queueNodeFIFO *)kmalloc(sizeof(queueNodeFIFO));
    if (!newNode)
        return;

    newNode->TCB = tsk;
    newNode->next = NULL;

    disable_interrupt();
    if (taskQueueFIFOEmpty(queue))
        queue->head = queue->tail = newNode;
    else {
        queue->tail->next = newNode;
        queue->tail = newNode;
    }
    enable_interrupt();
}

myTCB * taskQueueFIFODequeue(taskQueueFIFO *queue) {
    if (taskQueueFIFOEmpty(queue))
        return NULL;

    else {
        queueNodeFIFO *node = queue->head;
        myTCB *task = node->TCB;

        disable_interrupt();
        if (queue->head == queue->tail)
            queue->head = queue->tail = NULL;
        else
            queue->head = node->next;
        enable_interrupt();
        
        kfree((unsigned long)node);

        return task;
    }
}

void taskQueuePrioInit(taskQueuePrio **queue, int capacity, int (*cmp)(const myTCB *a, const myTCB *b)) {
    *queue = (taskQueuePrio *)kmalloc(sizeof(taskQueuePrio));
    (*queue)->tcb = (myTCB **)kmalloc((capacity + 1) * sizeof(myTCB *));
    (*queue)->tail = 0;
    (*queue)->capacity = capacity;
    (*queue)->cmp = cmp;
}

int taskQueuePrioEmpty(taskQueuePrio *queue) {
    int bool = (queue->tail == 0);
    return bool;
}

myTCB * taskQueuePrioNext(taskQueuePrio *queue) {
    if (taskQueuePrioEmpty(queue))
		return NULL;
	else
    	return queue->tcb[1];
}

void taskQueuePrioEnqueue(taskQueuePrio *queue, myTCB *tsk) {

    disable_interrupt();
    int i = ++queue->tail;
    while (queue->cmp(queue->tcb[i / 2], tsk) > 0) {
        queue->tcb[i] = queue->tcb[i / 2];
        i = i / 2;
    }
    queue->tcb[i] = tsk;
    enable_interrupt();
}

myTCB * taskQueuePrioDequeue(taskQueuePrio *queue) {
    if (taskQueuePrioEmpty(queue))
        return NULL;

    disable_interrupt();
    myTCB *task = queue->tcb[1];
    queue->tail--;
    int i = 2;
    while (i <= queue->tail) {

        if (i < queue->tail && queue->cmp(queue->tcb[i], queue->tcb[i + 1]) > 0)
            i++;

        if (i <= queue->tail && queue->cmp(queue->tcb[queue->tail + 1], queue->tcb[i]) > 0)
            queue->tcb[i / 2] = queue->tcb[i];
        else
            break;

        i *= 2;
    }
    queue->tcb[i / 2] = queue->tcb[queue->tail + 1];
    enable_interrupt();
    return task;
}