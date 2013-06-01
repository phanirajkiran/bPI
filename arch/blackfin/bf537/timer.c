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

#include "timer.h"
#include <arch.h>
#include <kernel/registers.h>


void initTimer() {
	/* enable timer 0 */
	regWrite16(BF537_TIMER0_CONFIG, 0x49); //IRQ disable, periodic, PWM_OUT, SCLKed, OUT PAD disabled

	regWrite32(BF537_TIMER0_PERIOD, -1);
	regWrite32(BF537_TIMER0_WIDTH, 1);

	__asm__ volatile("ssync;");

	regWrite16(BF537_TIMER_ENABLE, 1);
}


uint32 __getTimestamp() {
	return regRead32(BF537_TIMER0_COUNTER);
}


void __udelay(uint usec) {
	uint32 start = __getTimestamp();
	usec *= SYSTEM_CLOCK;
	while((__getTimestamp() - start) < usec) __nop();
}


