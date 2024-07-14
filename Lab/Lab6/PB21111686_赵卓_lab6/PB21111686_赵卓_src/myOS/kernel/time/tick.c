#include "tick.h"

#define MAX_HOOK_FUNC_NUM 32

unsigned int kernel_tick_times = 0;

int hook_func_num = 0;
void (*hook_list[MAX_HOOK_FUNC_NUM])(void); 

void append2HookList(void (*func)(void)) {
    hook_list[hook_func_num++] = func;
}

void tick(void) {

    kernel_tick_times++;

    for (int i = 0; i < hook_func_num; i++)
    	hook_list[i]();
}

unsigned int get_tick_times() {
    return kernel_tick_times;
}