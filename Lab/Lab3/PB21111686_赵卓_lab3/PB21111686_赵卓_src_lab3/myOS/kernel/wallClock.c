#include "tick.h"
#include "vga.h"

int HH, MM, SS;

void displayWallClock(void)
{
    char str[12];

    str[0] = '0' + HH / 10;
    str[1] = '0' + HH % 10;
    str[2] = ' ';
    str[3] = ':';
    str[4] = ' ';
    str[5] = '0' + MM / 10;
    str[6] = '0' + MM % 10;
    str[7] = ' ';
    str[8] = ':';
    str[9] = ' ';
    str[10] = '0' + SS / 10;
    str[11] = '0' + SS % 10;

    append2screen_clock(str, 0x2f, 25 - 1, 80 - 12);
}

void updateWallClock(void)
{

    if (system_ticks % HZ_INTERRUPT != 0)
        return;

    SS = (SS + 1) % 60;

    if (SS == 0)
        MM = (MM + 1) % 60;

    if (MM == 0 && SS == 0)
        HH = (HH + 1) % 24;

    displayWallClock();
}

void setWallClock(int h, int m, int s)
{

    if (h < 0)
        HH = 0;
    else if (h > 23)
        HH = 23;
    else
        HH = h;

    if (m < 0)
        MM = 0;
    else if (m > 59)
        MM = 59;
    else
        MM = m;

    if (s < 0)
        SS = 0;
    else if (s > 59)
        SS = 59;
    else
        SS = s;

    setWallClockHook(updateWallClock);

    displayWallClock();
}

void getWallClock(int *h, int *m, int *s)
{
    *h = HH;
    *m = MM;
    *s = SS;
}
