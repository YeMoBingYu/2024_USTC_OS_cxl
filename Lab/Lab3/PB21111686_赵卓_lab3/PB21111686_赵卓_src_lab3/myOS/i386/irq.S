.text
.code32
_start:
	.globl enable_interrupt
enable_interrupt:
	sti
	ret
	
	.globl disable_interrupt
disable_interrupt:
	cli
	ret
