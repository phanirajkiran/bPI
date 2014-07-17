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
#include <kernel/gpio.h>


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
void archHandleGpioIRQ() {
	int pin=0;
	for(int bank=0; bank<GPIO_BANKS; ++bank) {
		uint32 status = regRead32(BCM2835_GPIO_GPEDS0 + 4*bank);
		for(int bit=0; bit<32 && pin < GPIO_COUNT; ++bit) {
			if(status & (1<<bit)) {
				regWrite32(BCM2835_GPIO_GPEDS0 + 4*bank, 1<<bit); //clear
				handleGpioIRQPin(pin, getGpio(pin));
			}
			++pin;
		}
	}
}

void enableGpioIRQ() {
	regWrite32(ARM_IRQ_ENABLE2, 1<<ARM_I2_GPIO_ANY);
}
void disableGpioIRQ() {
	regWrite32(ARM_IRQ_DISABLE2, 1<<ARM_I2_GPIO_ANY);
}


