.globl start # 一般都用start
MAGIC_ITEM_NAME = 0x1BADB002
FLAGS_ITEM_NAME = 0x00000000
CHECKSUM_ITEM_NAME = -(MAGIC_ITEM_NAME + FLAGS_ITEM_NAME)


/*此处开始，按协议标准来定义必须的multiboot header*/
.section ".multiboot_header"
.long MAGIC_ITEM_NAME 
.long FLAGS_ITEM_NAME   
.long CHECKSUM_ITEM_NAME     

.text # 进入代码段
.code32 # 32位代码
 # 这个跟第一行的声明要一致
start:
    vga:
        movl $0x2f652f48, 0xB8000    #He
	    movl $0x2f6c2f6c, 0xB8004    #ll
	    movl $0x2f202f6f, 0xB8008    #o' ' 
	    movl $0x2f6f2f77, 0xB800C    #wo
 	    movl $0x2f6c2f72, 0xB8010    #rl
	    movl $0x2f212f64, 0xB8014    #d!
	    movl $0x2f502f20, 0xB8018    #' 'P
        movl $0x2f322f42, 0xB801C    #B2
        movl $0x2f312f31, 0xB8020    #11
        movl $0x2f312f31, 0xB8024    #11
        movl $0x2f382f36, 0xB8028    #68
        movl $0x2f5f2f36, 0xB802C    #6_
        movl $0x2f682f7A, 0xB8030    #zh
        movl $0x2f6F2f61, 0xB8034    #ao
        movl $0x2f682f7A, 0xB8038    #zh
        movl $0x2f6F2f75, 0xB803C    #uo

/*根据需要初始化串口*/
/*根据需要串口输出你的字符序列，详见前面串口编程简介*/
    uart:
        movw $0x3F8, %dx
	    movb $0x48, %al  #H
	    outb %al, %dx
	    movb $0x65, %al  #e
 	    outb %al, %dx	
	    movb $0x6c, %al  #l
	    outb %al, %dx
	    movb $0x6c, %al  #l
	    outb %al, %dx
	    movb $0x6f, %al  #o
	    outb %al, %dx
	    movb $0x20, %al  #' '
	    outb %al, %dx
	    movb $0x77, %al  #w
	    outb %al, %dx
	    movb $0x6f, %al  #o
	    outb %al, %dx
        movb $0x72, %al  #r
        outb %al, %dx
	    movb $0x6c, %al  #l
	    outb %al, %dx
	    movb $0x64, %al  #d
	    outb %al, %dx
	    movb $0x21, %al  #!
	    outb %al, %dx
	    movb $0x20, %al  #' '
	    outb %al, %dx
	    movb $0x50, %al  #P
	    outb %al, %dx
	    movb $0x42, %al  #B
	    outb %al, %dx
	    movb $0x32, %al  #2
	    outb %al, %dx
	    movb $0x31, %al  #1
	    outb %al, %dx
	    movb $0x31, %al  #1
	    outb %al, %dx
	    movb $0x31, %al  #1
	    outb %al, %dx
	    movb $0x31, %al  #1
	    outb %al, %dx
	    movb $0x36, %al  #6
	    outb %al, %dx
	    movb $0x38, %al  #8
	    outb %al, %dx
	    movb $0x36, %al  #6
	    outb %al, %dx
	    movb $0x5f, %al  #_
	    outb %al, %dx
	    movb $0x7A, %al  #z
	    outb %al, %dx
	    movb $0x68, %al  #h
	    outb %al, %dx
        movb $0x61, %al  #a
	    outb %al, %dx
        movb $0x6F, %al  #o
	    outb %al, %dx
        movb $0x7A, %al  #z
	    outb %al, %dx
	    movb $0x68, %al  #h
	    outb %al, %dx
        movb $0x75, %al  #u
	    outb %al, %dx
        movb $0x6F, %al  #o
	    outb %al, %dx
/*实验结束，让计算机停机，方法：使用hlt指令，或者死循环*/
    hlt