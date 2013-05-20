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

#include <kernel/registers.h>


void setNextTimerIRQ(uint ms) {
	uint pre_divide = 250-1; //system clock runs with 250 MHz
	//FIXME: system clock can dynamically change
	regWrite32(ARM_TIMER_PRE_DIVIDE, pre_divide); //max 10 bits wide

	regWrite32(ARM_TIMER_LOAD, ms*1000);
}
