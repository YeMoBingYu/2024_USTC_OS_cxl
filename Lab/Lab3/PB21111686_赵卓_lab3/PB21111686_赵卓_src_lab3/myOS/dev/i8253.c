#include "io.h"
#define I8253 1193180
#define HZ_INTERRUPT 100

void init8253(void)
{
	short temp = 1193180 / 100; 
	outb(0x43, 0x34);
	outb(0x40, (unsigned char)temp);		
	outb(0x40, (unsigned char)(temp >> 8)); 

	outb(0x21, inb(0x21) & 0xFE);
}
