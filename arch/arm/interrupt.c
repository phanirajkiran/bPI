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

#include <kernel/types.h>
#include <kernel/utils.h>
#include <kernel/interrupt.h>
#include <kernel/registers.h>


extern char __interrupt_vector_start;
extern char __interrupt_vector_end;

static uint in_interrupt = 0;


void archInitInterrupts() {
	//copy interrupt vector in place if needed: it must be placed at address 0.
	//it is possible that we overwrite the start code with this! (but it's not
	//needed anymore)
	char* interrupt_vector_start = &__interrupt_vector_start;
	char* interrupt_vector_end = &__interrupt_vector_end;
	void* destination = (void*)0; /* writing a NULL pointer :) */
	if(interrupt_vector_start) {
		memcpy(destination, interrupt_vector_start, 
			(size_t)interrupt_vector_end - (size_t)interrupt_vector_start);
	}
	//the interrupt controller does not need any specific initialization
}


void __enableInterrupts() {
	__asm__ volatile(
	"mrs r1, cpsr;"
	"bic r1, r1, #0x80;"    // enable IRQ 
	"msr cpsr_c, r1;"
	::: "r1");
}

void __disableInterrupts() {
	__asm__ volatile(
	"mrs r1, cpsr;"
	"orr r1, r1, #0x80;"    // disable IRQ
	"msr cpsr_c, r1;"
	::: "r1");
}

uint inInterrupt() {
	return in_interrupt;
}

/*
 * interrupt handler.
 * handle device interrupt & mark it as handled.
 * interrupts: disabled
 */
void irqHandler(int irq_number) {
	++in_interrupt;

	switch(irq_number) {
	case ARM_IRQ_NR_TIMER:

		handleTimerIRQ();

		break;
	default:
		printk("Got an unknown interrupt! (%i)\n", irq_number);
		break;
	}

	--in_interrupt;
}
