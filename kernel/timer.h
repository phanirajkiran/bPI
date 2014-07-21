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
 * timer related functions
 */

#ifndef TIMER_HEADER_H_
#define TIMER_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <timer_arch.h>


#ifndef ARCH_HAS_TIMER
#error "Architecture must define timer methods"
#endif /* ARCH_HAS_TIMER */


#define delay(ms) udelay((ms)*1000)


/* from the linux kernel... use with unsigned variables! */
/*
 *	These inlines deal with timer wrapping correctly. You are 
 *	strongly encouraged to use them
 *	1. Because people otherwise forget
 *	2. Because if the timer wrap changes in future you won't have to
 *	   alter your driver code.
 *
 * time_after(a,b) returns true if the time a is after time b.
 *
 * Do this with "<0" and ">=0" to only test the sign of the result. A
 * good compiler would generate better code (and a really good compiler
 * wouldn't care). Gcc is currently neither.
 */
#define time_after(a,b)		\
	 (((int)((b) - (a)) < 0))
#define time_before(a,b)	time_after(b,a)

#define time_after_eq(a,b)	\
	 (((int)((a) - (b)) >= 0))
#define time_before_eq(a,b)	time_after_eq(b,a)

/*
 * Calculate whether a is in the range of [b, c].
 */
#define time_in_range(a,b,c) \
	(time_after_eq(a,b) && \
	 time_before_eq(a,c))

/*
 * Calculate whether a is in the range of [b, c).
 */
#define time_in_range_open(a,b,c) \
	(time_after_eq(a,b) && \
	 time_before(a,c))



/**
 * initialize a timeout variable called name, initialized to now+timeout_usec
 */
#define timeout_init(name, timeout_usec) \
	uint32 name = getTimestamp() + (timeout_usec)

/**
 * check for a timeout using a name initialized with timeout_init
 */
#define timed_out(name) \
	time_after(getTimestamp(), name)

#ifdef __cplusplus
}
#endif
#endif /* TIMER_HEADER_H_ */
