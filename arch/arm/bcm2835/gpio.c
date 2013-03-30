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

#define GPPUD              0x20200094
#define GPPUDCLK0          0x20200098
#define GPPUDCLK1          0x2020009C

#include <kernel/registers.h>
#include <kernel/timer.h>


void setGpioPullUpDown(int pin, int pud) {
	int32* addr_clk = (int32*)( pin >= 32 ? GPPUDCLK1 : GPPUDCLK0);
	regWrite32(GPPUD, pud);
	waitCycles(150);
	regWrite32(addr_clk, 1<<(pin & 31));
	waitCycles(150);
	regWrite32(GPPUD, 0);
	regWrite32(addr_clk, 0);
}



