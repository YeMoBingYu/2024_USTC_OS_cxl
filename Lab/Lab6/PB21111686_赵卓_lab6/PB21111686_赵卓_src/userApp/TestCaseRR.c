#include "userInterface.h"

void myTskRR0(void) {
    task_execute(14);
}

void myTskRR1(void) {
    task_execute(4);
}

void myTskRR2(void) {
    task_execute(4);
}

void myTskRR3(void) {
    task_execute(3);
}


void initRRCases(void) {

    int newTskTid0 = createTsk(myTskRR0); 
    setTskPara(ARRV_TIME, 0, tcbPool[newTskTid0]->para);
    setTskPara(EXEC_TIME, 3, tcbPool[newTskTid0]->para); 

    int newTskTid1 = createTsk(myTskRR1); 
    setTskPara(ARRV_TIME, 1, tcbPool[newTskTid1]->para);
    setTskPara(EXEC_TIME, 3, tcbPool[newTskTid1]->para); 

    int newTskTid2 = createTsk(myTskRR2); 
    setTskPara(ARRV_TIME, 3, tcbPool[newTskTid2]->para);
    setTskPara(EXEC_TIME, 1, tcbPool[newTskTid2]->para); 

    int newTskTid3 = createTsk(myTskRR3); 
    setTskPara(ARRV_TIME, 4, tcbPool[newTskTid3]->para);
    setTskPara(EXEC_TIME, 5, tcbPool[newTskTid3]->para); 


    enableTask(newTskTid1);
    enableTask(newTskTid0);
    enableTask(newTskTid3);
    enableTask(newTskTid2);
}
