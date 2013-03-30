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

/*! timer related function */


/* timer base address to 8byte timer running with 1MHz */
getTimerAddress:
	ldr r0,=0x20003004
	mov pc,lr


/* this function must not be interrupted! */
.global __udelay
__udelay:
    usec .req r1 /* arg0: delay in micro seconds (32 bit) */
	mov usec, r0
	push {lr}
	bl getTimerAddress
	start .req r2
	elapsed .req r3
	ldr start,[r0]
	loop$:
		ldr elapsed,[r0]
		sub elapsed, elapsed,start
		cmp usec,elapsed
		/* backoff some time 
		   (otherwise longer durations do not work correctly...) */
		mov r0,r0
		mov r0,r0
		mov r0,r0
		mov r0,r0
		mov r0,r0
		mov r0,r0
		mov r0,r0
		mov r0,r0
		mov r0,r0
		mov r0,r0
		mov r0,r0
		mov r0,r0
		mov r0,r0
		mov r0,r0

		bhi loop$
	.unreq start
	.unreq elapsed
	.unreq usec
	pop {pc}


