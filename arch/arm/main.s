.section .init
.globl _start
_start:

	b __main

.section .text
__main:
	mov sp,#0x8000		/* init stack */

	mov r4,r2			/* save ATAG register */
	bl initZeroMemory	/* init zero memory (bss section) */

	mov r0, r4 			/* load ATAG register */
	bl readATAGRegister

	bl kernelMain
