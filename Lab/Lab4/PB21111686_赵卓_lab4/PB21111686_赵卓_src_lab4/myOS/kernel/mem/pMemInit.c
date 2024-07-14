#include "../../include/myPrintk.h"
#include "../../include/mem.h"
unsigned long pMemStart; // 可用的内存的起始地址
unsigned long pMemSize;	 // 可用的大小
unsigned long pMemHandler;

void memTest(unsigned long start, unsigned long grainSize)
{
	// TODO
	/*功能：检测算法
		这一个函数对应实验讲解ppt中的第一大功能-内存检测。
		本函数的功能是检测从start开始有多大的内存可用，具体算法参照ppt检测算法的文字描述
	注意点三个：
	1、覆盖写入和读出就是往指针指向的位置写和读，不要想复杂。
	(至于为什么这种检测内存的方法可行大家要自己想一下)
	2、开始的地址要大于1M，需要做一个if判断。
	3、grainsize不能太小，也要做一个if判断

	*/
	unsigned long addr = start;
	unsigned short data;
	unsigned short *head, *tail;
	unsigned short test_1 = 0xAA55;
	unsigned short test_2 = 0x55AA;
	int sign = 0; // 检测失败标志

	// 确保start和grainSize符合要求
	if (start < 0x100000)
		start = 0x100000;
	if (grainSize < 2)
		grainSize = 2;

	// 初始化可用内存大小和起点
	pMemSize = 0;
	pMemStart = start;

	while (!sign)
	{
		sign = 0;

		head = (unsigned short *)addr;					 // grain的头2个字节
		tail = (unsigned short *)(addr + grainSize - 2); // grain的尾2个字节

		data = *head;	// grain的头2个字节
		*head = test_1; // 写入0xAA55
		if (*head != test_1)
			sign = 1;
		*head = test_2; // 写入0x55AA
		if (*head != test_2)
			sign = 1;
		*head = data; // 还原原来的值

		data = *tail;	// grain的尾2个字节
		*tail = test_1; // 写入0xAA55
		if (*tail != test_1)
			sign = 1;
		*tail = test_2; // 写入0x55AA
		if (*tail != test_2)
			sign = 1;
		*tail = data; // 还原原来的值

		if (!sign) // sign没变则检测成功，增大pMemSize和addr
		{
			addr += grainSize;
			pMemSize += grainSize;
		}
	}
	myPrintk(0x7, "MemStart: %x  \n", pMemStart);
	myPrintk(0x7, "MemSize:  %x  \n", pMemSize);
}

extern unsigned long _end; // 已用内存的尾地址

void pMemInit(void)
{
	unsigned long _end_addr = (unsigned long)&_end;
	memTest(0x100000, 0x1000);
	myPrintk(0x7, "_end:  %x  \n", _end_addr);
	if (pMemStart <= _end_addr) // 如果pMemStart在_end之前，表明占用了已用内存地址
	{
		pMemSize -= _end_addr - pMemStart;
		pMemStart = _end_addr;
	}

	// 此处选择不同的内存管理算法
	pMemHandler = dPartitionInit(pMemStart, pMemSize);
}
