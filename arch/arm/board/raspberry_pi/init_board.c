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

#include "init_board.h"
#include <kernel/led.h>
#include <kernel/serial.h>
#include <kernel/utils.h>

static void uartPrintkOutput(char c) {
	if(c=='\n') uartWrite('\r'); //serial wants CRLF for new lines
	uartWrite(c);
}

void initBoard() {
	initLeds();
	initUart();

	/* we want the printk output on the serial console */
	addPrintkOutput(&uartPrintkOutput);
}


