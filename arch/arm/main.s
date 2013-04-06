.section .init
.globl _start
_start:

	b __main

.section .text
__main:
	mov sp,#0x8000		/* init stack */

	mov r0, r2 			/* load ATAG register */
	bl readATAGRegister

	bl kernelMain
