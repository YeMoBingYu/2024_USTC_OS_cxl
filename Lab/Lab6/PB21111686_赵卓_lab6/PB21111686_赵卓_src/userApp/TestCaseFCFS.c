#include "userInterface.h"

void myTskFCFS0(void) {
    task_execute(4);
}

void myTskFCFS1(void) {
    task_execute(5);
}

void myTskFCFS2(void) {
    task_execute(3);
}


void initFCFSCases(void) {

    int newTskTid0 = createTsk(myTskFCFS0); 
    setTskPara(ARRV_TIME, 2, tcbPool[newTskTid0]->para);
    setTskPara(EXEC_TIME, 5, tcbPool[newTskTid0]->para); 

    int newTskTid1 = createTsk(myTskFCFS1); 
    setTskPara(ARRV_TIME, 0, tcbPool[newTskTid1]->para);
    setTskPara(EXEC_TIME, 3, tcbPool[newTskTid1]->para); 

    int newTskTid2 = createTsk(myTskFCFS2); 
    setTskPara(ARRV_TIME, 10, tcbPool[newTskTid2]->para);
    setTskPara(EXEC_TIME, 3, tcbPool[newTskTid2]->para); 

    
    enableTask(newTskTid0);
    enableTask(newTskTid1);
    enableTask(newTskTid2);
}
