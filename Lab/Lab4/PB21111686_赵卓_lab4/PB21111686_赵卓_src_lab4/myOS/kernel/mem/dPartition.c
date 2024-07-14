#include "../../include/myPrintk.h"

// dPartition 是整个动态分区内存的数据结构
typedef struct dPartition
{
	unsigned long size;
	unsigned long firstFreeStart;
} dPartition; // 共占8个字节

#define dPartition_size ((unsigned long)0x8)

void showdPartition(struct dPartition *dp)
{
	myPrintk(0x5, "dPartition(start=0x%x, size=0x%x, firstFreeStart=0x%x)\n", dp, dp->size, dp->firstFreeStart);
}

// EMB 是每一个block的数据结构，userdata可以暂时不用管。
typedef struct EMB
{
	unsigned long size;
	union
	{
		unsigned long nextStart; // if free: pointer to next block
		unsigned long userData;	 // if allocated, belongs to user
	};
} EMB; // 共占8个字节

#define EMB_size ((unsigned long)0x8)

void showEMB(struct EMB *emb)
{
	myPrintk(0x3, "EMB(start=0x%x, size=0x%x, nextStart=0x%x)\n", emb, emb->size, emb->nextStart);
}

unsigned long dPartitionInit(unsigned long start, unsigned long totalSize)
{
	// TODO
	/*功能：初始化内存。
	1. 在地址start处，首先是要有dPartition结构体表示整个数据结构(也即句柄)。
	2. 然后，一整块的EMB被分配（以后使用内存会逐渐拆分），在内存中紧紧跟在dP后面，然后dP的firstFreeStart指向EMB。
	3. 返回start首地址(也即句柄)。
	注意有两个地方的大小问题：
		第一个是由于内存肯定要有一个EMB和一个dPartition，totalSize肯定要比这两个加起来大。
		第二个注意EMB的size属性不是totalsize，因为dPartition和EMB自身都需要要占空间。

	*/

	// 若比dP和EMB还小，不符合要求
	if (totalSize <= dPartition_size + EMB_size)
		return 0;

	// 句柄初始化
	dPartition *handle;
	handle = (dPartition *)start;
	handle->size = totalSize;
	handle->firstFreeStart = start + dPartition_size;

	// 第一个EMB初始化
	EMB *block;
	block = (EMB *)handle->firstFreeStart;
	block->size = totalSize - dPartition_size;
	block->nextStart = 0;

	// 返回句柄起点
	return start;
}

void dPartitionWalkByAddr(unsigned long dp)
{
	// TODO
	/*功能：本函数遍历输出EMB 方便调试
	1. 先打印dP的信息，可调用上面的showdPartition。
	2. 然后按地址的大小遍历EMB，对于每一个EMB，可以调用上面的showEMB输出其信息

	*/
	// 先打印句柄
	dPartition *handle;
	handle = (dPartition *)dp;
	showdPartition(handle);
	// 一个个遍历EMB打印信息
	unsigned long addr = handle->firstFreeStart;
	EMB *block;
	while (addr)
	{
		block = (EMB *)addr;
		showEMB(block);
		addr = block->nextStart;
	}
}

//=================firstfit, order: address, low-->high=====================
/**
 * return value: addr (without overhead, can directly used by user)
 **/

// 根据PPT建议，将地址8字节对齐
unsigned long align_8bytes(unsigned long addr)
{
	// 将 addr 8字节对齐
	if (addr & 1)
		addr += 1;
	if (addr & 2)
		addr += 2;
	if (addr & 4)
		addr += 4;
	return addr;
}

unsigned long dPartitionAllocFirstFit(unsigned long dp, unsigned long size)
{
	// TODO
	/*功能：分配一个空间
	1. 使用firstfit的算法分配空间，
	2. 成功分配返回首地址，不成功返回0
	3. 从空闲内存块组成的链表中拿出一块供我们来分配空间(如果提供给分配空间的内存块空间大于size，我们还将把剩余部分放回链表中)，并维护相应的空闲链表以及句柄
	注意的地方：
		1.EMB类型的数据的存在本身就占用了一定的空间。

	*/
	dPartition *handle;
	handle = (dPartition *)dp;

	if (!handle->firstFreeStart) // 无空闲内存块
		return 0;

	unsigned long size_a = align_8bytes(size);	 // 8字节对齐
	unsigned long addr = handle->firstFreeStart; // 记录当前EMB块
	unsigned long addr_before = 0;				 // 记录上一个EMB块
	EMB *block;
	EMB *block_before;

	while (addr)
	{
		block = (EMB *)addr;
		block_before = (EMB *)addr_before;

		// EMB类型数据的存在本身就占用了一定的空间，所以实际分配的空间应加上EMB的大小
		// 内存块大小足够，但剩余大小至多只能容纳一个EMB。这种情况，直接将此块分配
		if (block->size >= size_a + sizeof(unsigned long) && block->size <= size_a + sizeof(unsigned long) + EMB_size)
		{
			if (addr_before == 0)
				handle->firstFreeStart = block->nextStart; // 更改句柄
			else
				block_before->nextStart = block->nextStart; // 更改上一个EMB块的后继

			return addr + sizeof(unsigned long); // sizeof(unsigned long)这块是用来存放分配的内存的大小，用于free使用
		}

		// 内存块大小足够，且剩余大小也足够，则分配出所需大小的块，剩余部分划分为一个新的EMB块
		else if (block->size > size_a + sizeof(unsigned long) + EMB_size)
		{
			unsigned long addr_new = addr + sizeof(unsigned long) + size_a; // 新EMB的起点
			EMB *block_new = (EMB *)(addr_new);
			block_new->size = block->size - size_a - sizeof(unsigned long);
			block_new->nextStart = block->nextStart; // 新EMB的下个起点是原来EMB的下个起点
			block->size -= block_new->size;			 // 将size_a+sizeof(unsigned long)大小存入分配内存的起点

			if (addr_before == 0)
				handle->firstFreeStart = addr_new; // 更改句柄
			else
				block_before->nextStart = addr_new; // 更改上一个EMB块的后继

			return addr + sizeof(unsigned long);
		}

		addr_before = addr;
		addr = block->nextStart;
	}

	return 0; // 未找到合适内存块返回0
}

unsigned long dPartitionFreeFirstFit(unsigned long dp, unsigned long start)
{
	// TODO
	/*功能：释放一个空间
	1. 按照对应的fit的算法释放空间
	2. 注意检查要释放的start~end这个范围是否在dp有效分配范围内
		返回1 没问题
		返回0 error
	3. 需要考虑两个空闲且相邻的内存块的合并

	*/
	start = start - sizeof(unsigned long); // 分配内存的实际起点
	dPartition *handle = (dPartition *)dp;

	// 检查地址是否在有效范围内
	if ((start < dp + dPartition_size) || (start >= dp + handle->size))
		return 0;
    
	unsigned long addr = handle->firstFreeStart;
	unsigned long addr_pre = 0;	 // 记录邻近的上一个EMB块
	unsigned long addr_succ = 0; // 记录邻近的下一个EMB块
	EMB *block;

	// 寻找当前释放内存块邻近的空闲块。
	while (addr)
	{
		block = (EMB *)addr;
		if (addr < start)
			addr_pre = addr;
		else if (addr > start)
		{
			addr_succ = addr;
			break;
		}
		addr = block->nextStart;
	}

	// 释放和合并
	block = (EMB *)start;
	// 处理后继块
	if (addr_succ)
	{
		// 若后一个块和当前内存块连续，则合并，这里block->size就是当前内存块的大小，在分配时保存
		if (addr_succ == start + block->size)
		{
			EMB *block_succ = (EMB *)addr_succ;
			block->size += block_succ->size;
			block->nextStart = block_succ->nextStart;
		}
		else
			block->nextStart = addr_succ;
	}
	else
		block->nextStart = 0; // 没有后继空闲块
	// 处理前驱块						  
	if (addr_pre)
	{
		// 若前一个块和当前内存块连续，则合并
		EMB *block_pre = (EMB *)addr_pre;
		if (start == addr_pre + block_pre->size)
		{
			block_pre->size += block->size;
			block_pre->nextStart = block->nextStart;
		}
		else
			block_pre->nextStart = start;
	}
	else
		handle->firstFreeStart = start; // 没有前驱块，则和句柄相连

	return 1;
}

// 进行封装，此处默认firstfit分配算法，当然也可以使用其他fit，不限制。
unsigned long dPartitionAlloc(unsigned long dp, unsigned long size)
{
	return dPartitionAllocFirstFit(dp, size);
}

unsigned long dPartitionFree(unsigned long dp, unsigned long start)
{
	return dPartitionFreeFirstFit(dp, start);
}
