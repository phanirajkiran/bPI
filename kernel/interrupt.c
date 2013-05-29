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

/*
 * here are the device interrupt handlers
 */

#include <kernel/interrupt.h>
#include <kernel/types.h>
#include <kernel/printk.h>


static uint timer_irq_counter = 0;
static uint interrupts_enabled = 1; //0 means they are enabled

void handleTimerIRQ() {
	archHandleTimerIRQ();
	
	//printk("Got a timer interrupt\n");
	++timer_irq_counter;
	
	//disableTimerIRQ();
}

uint getTimerIRQCounter() {
	return timer_irq_counter;
}

void resetTimerIRQCounter() {
	timer_irq_counter = 0;
}

void enableInterrupts() {
	if(inInterrupt()) 
		return; //inside IRQ handler, interrupts are always disabled

	if(interrupts_enabled > 0) {
		if(--interrupts_enabled == 0) __enableInterrupts();
	}
}

void disableInterrupts() {
	if(inInterrupt()) 
		return; //inside IRQ handler, interrupts are always disabled

	if(++interrupts_enabled == 1)
		__disableInterrupts();
}
