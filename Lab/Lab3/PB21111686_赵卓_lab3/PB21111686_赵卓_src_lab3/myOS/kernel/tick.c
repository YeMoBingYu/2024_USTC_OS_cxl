#define TICK_HOOK_VOL 10 

int system_ticks = 0;

int hook_func_num = 0;
void (*hook_list[TICK_HOOK_VOL])(void);

void setWallClockHook(void (*func)(void))
{
    hook_list[hook_func_num++] = func;
} 

void tick(void)
{

    system_ticks++;

    for (int i = 0; i < hook_func_num; i++)
        hook_list[i]();
}
