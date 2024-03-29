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

#ifdef __cplusplus
extern "C" {
#endif

#define ARCH_HAS_TIMER

#include <kernel/types.h>

typedef uint32 Timestamp;
typedef int32 TimestampSigned;

#include <bcm2835/timer.h>

//get timer base address
int getTimerAddress();


void __udelay(uint usec);

#define udelay(usec) __udelay(usec)


void __waitCycles(uint cycles);

#define waitCycles(cycles) __waitCycles(cycles)


Timestamp __getTimestamp();


/** current timestamp in microseconds */
#define getTimestamp() __getTimestamp()

/** current timestamp in milliseconds */
static inline Timestamp getMillis() {
	return getTimestamp()/1000;
}

#ifdef __cplusplus
}
#endif
#endif /* ARCH_TIMER_HEADER_H_ */


