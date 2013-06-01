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

.section .init, "ax",%progbits // must be allocatable & executable
.globl _start
_start:

	jump __main


.section .text
.global __main
__main:

	R7 = 0;
	LC0 = R7;
	LC1 = R7;
	L0 = R7;
	L1 = R7;
	L2 = R7;
	L3 = R7;

	CSYNC;
 
	// init stack
	SP.L=___estack - 12;
	SP.H=___estack - 12;
	FP = SP;
	SP += -12;


	call.x _initZeroMemory	// init zero memory (bss section)

	call.x _kmain

    nop;
__end:
	IDLE;
	JUMP __end;
