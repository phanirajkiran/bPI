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

#include "common.h"

#include <kernel/registers.h>
#include <kernel/timer.h>
#include <kernel/gpio.h>


void setGpioPullUpDown(int pin, int pud) {
	int32* addr_clk = (int32*)( pin >= 32 ? BCM2835_GPIO_GPPUDCLK1 : BCM2835_GPIO_GPPUDCLK0);
	regWrite32(BCM2835_GPIO_GPPUD, pud);
	waitCycles(150);
	regWrite32(addr_clk, 1<<(pin & 31));
	waitCycles(150);
	regWrite32(BCM2835_GPIO_GPPUD, 0);
	regWrite32(addr_clk, 0);
}

void setGpioEdgeDetect(int pin, int edge) {
	int32* addr_rising = (int32*)( pin >= 32 ? BCM2835_GPIO_GPREN1 : BCM2835_GPIO_GPREN0);
	if(edge & GPIO_RISING_EDGE) {
		regRMW32(addr_rising, pin & 31, 1);
	} else {
		regRMW32(addr_rising, pin & 31, 0);
	}
	int32* addr_falling = (int32*)( pin >= 32 ? BCM2835_GPIO_GPFEN1 : BCM2835_GPIO_GPFEN0);
	if(edge & GPIO_FALLING_EDGE) {
		regRMW32(addr_falling, pin & 31, 1);
	} else {
		regRMW32(addr_falling, pin & 31, 0);
	}
}

