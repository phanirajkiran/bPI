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

/*!
 * high level main kernel entry (called right after arch-specific _start
 * function).
 */

#include <kernel/utils.h>
#include <arch.h>

#include <kernel/led.h>
#include <kernel/timer.h>


void kernelMain() {

	archInit();

	/* main loop */
	initLeds();
	const int duration = 80000;
	while(true) {
		toggleLed(0);
		udelay(duration);
	}
}

