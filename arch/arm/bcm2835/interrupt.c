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

#include "interrupt.h"
#include "timer.h"

#include <kernel/registers.h>


void enableTimerIRQ() {

	/* timer */
	int32 ctrl = regRead32(ARM_TIMER_CONTROL);
	ctrl |= (1<<ARM_TIMER_CTRL_IRQ_ENAB) 
		| (1<<ARM_TIMER_CTRL_TIMER_ENAB) /* enable timer & interrupts */
		| (1<<ARM_TIMER_CTRL_LEN); /* use 32bits load register */
	regWrite32(ARM_TIMER_CONTROL, ctrl);

	/* interrupt controller */
	regWrite32(ARM_IRQ_ENABLE0, 1<<ARM_I0_TIMER);
}
void disableTimerIRQ() {
	regWrite32(ARM_IRQ_DISABLE0, 1<<ARM_I0_TIMER);
}

void archHandleTimerIRQ() {
	regWrite32(ARM_TIMER_IRQ_CLR, 1); //clear irq
}


