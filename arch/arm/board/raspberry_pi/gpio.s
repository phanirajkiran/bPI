/*
 * Copyright (C) 2013 Beat Küng <beat-kueng@gmx.net>
 * taken from Baking Pi: http://www.cl.cam.ac.uk/freshers/raspberrypi/tutorials/os/
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
 * GPIO functions for the BCM2835 ARM processor
 */


/* returns the base address of gpio (physical addr) */
.globl getGpioAddress
getGpioAddress:
	ldr r0,=0x20200000
	mov pc,lr




/*
 set gpio pin to a function (input/output/...)
 arg0: pin [0,53]
 arg1: function [0,7]
  000 = GPIO Pin X is an input
  001 = GPIO Pin X is an output
  100 = GPIO Pin X takes alternate function 0
  101 = GPIO Pin X takes alternate function 1
  110 = GPIO Pin X takes alternate function 2
  111 = GPIO Pin X takes alternate function 3
  011 = GPIO Pin X takes alternate function 4
  010 = GPIO Pin X takes alternate function 5
*/
.globl setGpioFunction
setGpioFunction:
    pinNum .req r0
    pinFunc .req r1
	cmp pinNum,#53		/* input validation */
	cmpls pinFunc,#7
	movhi pc,lr

	push {lr}
	mov r2,pinNum
	.unreq pinNum
	pinNum .req r2
	bl getGpioAddress
	gpioAddr .req r0

	functionLoop$:
		cmp pinNum,#9
		subhi pinNum,#10
		addhi gpioAddr,#4
		bhi functionLoop$

	add pinNum, pinNum,lsl #1
	lsl pinFunc,pinNum

	mask .req r3
	mov mask,#7					/* r3 = 111 in binary */
	lsl mask,pinNum				/* r3 = 11100..00 where the 111 is in the same position as the function in r1 */
	.unreq pinNum

	mvn mask,mask				/* r3 = 11..1100011..11 where the 000 is in the same poisiont as the function in r1 */
	oldFunc .req r2
	ldr oldFunc,[gpioAddr]		/* r2 = existing code */
	and oldFunc,mask			/* r2 = existing code with bits for this pin all 0 */
	.unreq mask

	orr pinFunc,oldFunc			/* r1 = existing code with correct bits set */
	.unreq oldFunc

	str pinFunc,[gpioAddr]
	.unreq pinFunc
	.unreq gpioAddr
	pop {pc}




.globl setGpio
setGpio:
	pinNum .req r0 		/* arg0 [0,53] */
	pinVal .req r1 		/* arg1 0 turn off, !=0 turn on */

	cmp pinNum,#53
	movhi pc,lr
	push {lr}
	mov r2,pinNum
	.unreq pinNum
	pinNum .req r2
	bl getGpioAddress
	gpioAddr .req r0

	pinBank .req r3
	lsr pinBank,pinNum,#5
	lsl pinBank,#2
	add gpioAddr,pinBank
	.unreq pinBank

	and pinNum,#31
	setBit .req r3
	mov setBit,#1
	lsl setBit,pinNum
	.unreq pinNum

	teq pinVal,#0
	.unreq pinVal
	streq setBit,[gpioAddr,#0x28]
	strne setBit,[gpioAddr,#0x1C]
	.unreq setBit
	.unreq gpioAddr
	pop {pc}



.global getGpio
getGpio:
/*
 read gpio pin
 arg0: pin [0,53]
 returns the value of the gpio: 0:pin low, !=0: pin high
 */
	pinNum .req r0 		/* arg0 [0,53] */

	cmp pinNum,#53		/* argument checking */
	movhi r0,#0
	movhi pc,lr

	push {lr}
	mov r2,pinNum
	.unreq pinNum
	pinNum .req r2
	bl getGpioAddress
	gpioAddr .req r0

	pinBank .req r3
	lsr pinBank,pinNum,#5
	lsl pinBank,#2
	add gpioAddr,pinBank
	.unreq pinBank

	and pinNum,#31
	getBit .req r3
	mov getBit,#1
	lsl getBit,pinNum
	.unreq pinNum

	ldr r0,[gpioAddr,#0x34]
	and r0,getBit
	.unreq getBit
	.unreq gpioAddr
	pop {pc}




