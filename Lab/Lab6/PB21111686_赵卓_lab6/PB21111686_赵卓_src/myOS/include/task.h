#ifndef __TASK_H__
#define __TASK_H__

#include "userApp.h"

#define STACK_SIZE 0x1000
#define initTskBody myMain // connect initTask with myMain

// task states
#define TSK_WAITING 0
#define TSK_READY 1
#define TSK_RUNNING 2

void initTskBody(void);
void CTX_SW(void);

typedef struct tskPara {
    int priority;
    int exec_time;
    int arrv_time;
} tskPara;

// task control block TCB
typedef struct myTCB {
    int tid; // task id
    int state; // task state
    unsigned int runTime; // the time this task has been executed
    unsigned int lastScheduledTime; 
    unsigned long *stkTop; // the top of the stack
    unsigned long *stkMAX; // the MAX address of the stack, reside in the lower address
    void (*entry)(void); // the entry of task function body
    tskPara *para;
    struct myTCB *next;
} myTCB;

extern myTCB **tcbPool; // tcb pool
extern myTCB *firstFreeTCB; // the first free tcb in the pool
extern myTCB *idleTsk; // idle task pointer
extern myTCB *currentTsk; // current task pointer

extern unsigned long *BspContextBase;
extern unsigned long *BspContext;
extern unsigned int taskNum;

void idleTskBody(void);
void TaskManagerInit(void);
void tskEnd(void);

int createTsk(void (*tskBody)(void));
void destroyTsk(int tid);
void context_switch(unsigned long **prevTskStkAddr, unsigned long *nextTskStk);
void tskStart(int tid);


// taskArrv
void taskArrvQueueInit(void);
void enableTask(int tid);

//taskPara
#define MAX_PRIORITY 5
#define MAX_EXEC_TIME 20

#define PRIORITY 0
#define EXEC_TIME 1 
#define ARRV_TIME 2


void initTskPara(tskPara **para);
void setTskPara(unsigned int option, unsigned int value, tskPara *para);
unsigned int getTskPara(unsigned option, tskPara *para);

void showCurrentTskParaInfo_hook(void);

void task_execute(unsigned int wait_time);


//taskQueueFIFO
typedef struct queueNodeFIFO {
    myTCB *TCB;
    struct queueNodeFIFO *next;
} queueNodeFIFO;

typedef struct taskQueueFIFO {
    queueNodeFIFO *head;
    queueNodeFIFO *tail;
} taskQueueFIFO;

void taskQueueFIFOInit(taskQueueFIFO *queue);
int taskQueueFIFOEmpty(taskQueueFIFO *queue);
myTCB * taskQueueFIFONext(taskQueueFIFO *queue);
void taskQueueFIFOEnqueue(taskQueueFIFO *queue, myTCB *tsk);
myTCB * taskQueueFIFODequeue(taskQueueFIFO *queue);


//taskQueuePrio
typedef struct taskQueuePrio {
    myTCB **tcb;
    int tail;
    int capacity;
    int (*cmp)(const myTCB *a, const myTCB *b);
} taskQueuePrio;

void taskQueuePrioInit(taskQueuePrio **queue, int capacity, int (*cmp)(const myTCB *a, const myTCB *b));
int taskQueuePrioEmpty(taskQueuePrio *queue);
myTCB * taskQueuePrioNext(taskQueuePrio *queue);
void taskQueuePrioEnqueue(taskQueuePrio *queue, myTCB *tsk);
myTCB * taskQueuePrioDequeue(taskQueuePrio *queue);

#endif
