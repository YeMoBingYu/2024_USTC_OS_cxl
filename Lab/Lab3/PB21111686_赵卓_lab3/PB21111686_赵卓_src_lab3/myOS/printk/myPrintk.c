#include "uart.h"
#include "vga.h"
#include "vsprintf.h"
#include <stdarg.h>


char kBuf[400];
int myPrintk(int color, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int count = vsprintf(kBuf, format, args);
    va_end(args);
    append2screen(kBuf, color);
    return count;
}

char uBuf[400];
int myPrintf(int color, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int count = vsprintf(uBuf, format, args);
    va_end(args);
    append2screen(uBuf, color);
    return count;
}