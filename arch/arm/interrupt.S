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

#define __ASSEMBLY__
#include "interrupt_arch.h"


.section .interrupt_vector, "ax",%progbits ;@ must be allocatable & executable
.global _interrupt_vector
_interrupt_vector:
    b   __reset       ;@ 0x00000000 reset
    b   __handler     ;@ 0x00000004 undefined instruction
    b   __swiHandler  ;@ 0x00000008 software interrupt swi
    b   __handler     ;@ 0x0000000C prefetch abort
    b   __dataFault   ;@ 0x00000010 data abort
    b   __handler     ;@ 0x00000014 ?
    b   __irqHandler  ;@ 0x00000018 irq
    b   __handler     ;@ 0x000000


.section .text


.macro  get_irqnr_preamble, base, tmp
	ldr	\base, =ARMCTRL_IC_BASE
.endm

.macro	get_irqnr_and_base, irqnr, irqstat, base, tmp
	/* get masked status */
	ldr	\irqstat, [\base, #(ARM_IRQ_PEND0 - ARMCTRL_IC_BASE)]
	mov	\irqnr, #(ARM_IRQ0_BASE + 31)
	and	\tmp, \irqstat, #0x300		 @ save bits 8 and 9
	/* clear bits 8 and 9, and test */
	bics	\irqstat, \irqstat, #0x300
	bne	1010f

	tst	\tmp, #0x100
	ldrne	\irqstat, [\base, #(ARM_IRQ_PEND1 - ARMCTRL_IC_BASE)]
	movne \irqnr, #(ARM_IRQ1_BASE + 31)
	@ Mask out the interrupts also present in PEND0 - see SW-5809
	bicne \irqstat, #((1<<7) | (1<<9) | (1<<10))
	bicne \irqstat, #((1<<18) | (1<<19))
	bne	1010f

	tst	\tmp, #0x200
	ldrne \irqstat, [\base, #(ARM_IRQ_PEND2 - ARMCTRL_IC_BASE)]
	movne \irqnr, #(ARM_IRQ2_BASE + 31)
	@ Mask out the interrupts also present in PEND0 - see SW-5809
	bicne \irqstat, #((1<<21) | (1<<22) | (1<<23) | (1<<24) | (1<<25))
	bicne \irqstat, #((1<<30))
	beq 1020f

1010:
	@ For non-zero x, LSB(x) = 31 - CLZ(x^(x-1))
	@ N.B. CLZ is an ARM5 instruction.
	sub	\tmp, \irqstat, #1
	eor	\irqstat, \irqstat, \tmp
	clz	\tmp, \irqstat
	sub	\irqnr, \tmp

1020:	@ EQ will be set if no irqs pending

.endm

__reset:
	b __main

__handler:
	b __reset

__swiHandler:
	b __reset

;@ main interrupt handler. handle interrupt with interrupts disabled
__irqHandler:
	sub lr,lr,#4
	stmfd sp!,{r0-r3,r12,lr}  ;@ safe registers on stack

	;@ get interrupt number
	get_irqnr_preamble r1, r3
	get_irqnr_and_base r0, r2, r1, r3

	;@ handle the interrupt & clear device interrupt register:
	bl irqHandler

	ldmfd sp!,{r0-r3,r12,pc}^


;@ page fault handler
__dataFault:
	sub lr,lr,#8

	;@ TODO: setup an abort-mode stack or switch to another mode ...
	ldr sp, =10000000

	stmfd sp!,{r0-r3,lr}  ;@ safe registers on stack

	;@ handle the interrupt & clear device interrupt register:
	bl handlePageFault

	ldmfd sp!,{r0-r3,pc}^

