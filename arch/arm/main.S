/*
 * Copyright (C) 2013 Beat Küng <beat-kueng@gmx.net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

.section .init, "ax",%progbits ;@ must be allocatable & executable
.globl _start
_start:

	b __main


.section .text
.global __main
__main:

	;@ init stack

	mov r0,#0xD2 ;@ #(PSR_IRQ_MODE|PSR_FIQ_DIS|PSR_IRQ_DIS)
	msr cpsr_c, r0 ;@ change mode
	ldr sp, =__estack ;@ irq stack

	mov r0,#0xD3 ;@ #(PSR_SVC_MODE|PSR_FIQ_DIS|PSR_IRQ_DIS)
	msr cpsr_c, r0
	ldr sp, =__estack-0x1000 ;@ supervisor mode stack (kernel mode)


	mov r4,r2			;@ save ATAG register
	bl initZeroMemory	;@ init zero memory (bss section)

	mov r0, r4 			;@ load ATAG register
	bl readATAGRegister

	bl kmain
