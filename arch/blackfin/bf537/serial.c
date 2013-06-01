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

#include "serial.h"

#include <kernel/registers.h>
#include <kernel/serial.h>
#include <kernel/gpio.h>

/*
 * init UART0
 * settings:
 *  baud rate: 115200
 *  8 bits, 1 stop bit, no parity, no flow control
 */
void initUart() {
	/* set up gpio */
	setGpioFunction(0, 2);
	setGpioFunction(1, 2);
	setGpioAltFunction(3, 0); //set UART0


	/* set up UART0 */
	regWrite16(BF537_UART0_GCTL, 1<<BF537_GCTL_UCEN);
	//set DLAB to change divisor
	regWrite16(BF537_UART0_LCR, 1<<BF537_LCR_DLAB);
	const uint16 divisor = (SYSTEM_CLOCK * 1e6 / 115200 / 16);
	regWrite16(BF537_UART0_DLL, divisor);
	regWrite16(BF537_UART0_DLH, divisor>>8);
	//clear DLAB
	regWrite16(BF537_UART0_LCR, (0x3 << BF537_LCR_WLS) );
}

void uartWrite(int data) {
	//check fill state
	while(regRead16Bit(BF537_UART0_LSR, BF537_LSR_THRE)==0);

	regWrite16(BF537_UART0_THR, data);
}

void uartWriteStr(char* str) {
	while(*str) {
		uartWrite(*str);
		++str;
	}
}

void uartWriteBuf(char* str, int len) {
	while(len > 0) {
		uartWrite(*str);
		++str;
		--len;
	}
}

int uartRead() {
	//check availability
	while(regRead16Bit(BF537_UART0_LSR, BF537_LSR_DR) == 0);
	
	return regRead16(BF537_UART0_RBR);
}

bool uartAvailable() {
	return regRead16Bit(BF537_UART0_LSR, BF537_LSR_DR);
}




