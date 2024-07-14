#include "userInterface.h"

void myTskSJF0(void) {
    task_execute(2);
}

void myTskSJF1(void) {
    task_execute(5);
}

void myTskSJF2(void) {
    task_execute(4);
}

void myTskSJF3(void) {
    task_execute(3);
}



void initSJFCases(void) {

    int newTskTid0 = createTsk(myTskSJF0); 
    setTskPara(ARRV_TIME, 0, tcbPool[newTskTid0]->para);
    setTskPara(EXEC_TIME, 1, tcbPool[newTskTid0]->para); 

    int newTskTid1 = createTsk(myTskSJF1); 
    setTskPara(ARRV_TIME, 0, tcbPool[newTskTid1]->para);
    setTskPara(EXEC_TIME, 5, tcbPool[newTskTid1]->para); 

    int newTskTid2 = createTsk(myTskSJF2);
    setTskPara(ARRV_TIME, 1, tcbPool[newTskTid2]->para);
    setTskPara(EXEC_TIME, 4, tcbPool[newTskTid2]->para); 

    int newTskTid3 = createTsk(myTskSJF3); 
    setTskPara(ARRV_TIME, 3, tcbPool[newTskTid3]->para);
    setTskPara(EXEC_TIME, 2, tcbPool[newTskTid3]->para);


    enableTask(newTskTid1);
    enableTask(newTskTid2);
    enableTask(newTskTid3);
    enableTask(newTskTid0);

}
