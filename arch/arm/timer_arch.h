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

#ifndef ARCH_TIMER_HEADER_H_
#define ARCH_TIMER_HEADER_H_

#define ARCH_HAS_TIMER

//get timer base address
int getTimerAddress();


void __udelay(uint usec);

#define udelay(usec) __udelay(usec)


void __waitCycles(uint cycles);

#define waitCycles(cycles) __waitCycles(cycles)


//TODO: getTimestamp


#endif /* ARCH_TIMER_HEADER_H_ */


