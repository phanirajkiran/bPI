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
 * init uart
 * TxD1: GPIO pin 14
 * RxD1: GPIO pin 15
 * settings:
 *  baud rate: 115200
 *  8 bits, 1 stop bit, no parity, no flow control
 */
void initUart() {
	/* set up gpio */
	setGpioPullUpDown(14, 0);
	setGpioPullUpDown(15, 0);
	setGpioFunction(14, 0b010); //TxD1
	setGpioFunction(15, 0b010); //RxD1

	/* init uart */
	regRMW32(AUX_ENABLES, 0, 1); //enable uart (SPI uses the same register)
	
	regWrite32(AUX_MU_LCR_REG, 0x3); //set to 8 bit mode (0.bit) 
									 // & 1 stop bit (3.bit)
	regWrite32(AUX_MU_MCR_REG, 0x0);
	regWrite32(AUX_MU_IER_REG, 0x0);
	regWrite32(AUX_MU_IIR_REG, 0x0); //disable interrupts

	
	/* set baud rate to 115200:
	 * baud rate register = sys_clk / (8*baud_rate) - 1
	 *  where sys_clk is system clock, defaults to 250 MHz
	 */
	regWrite16(AUX_MU_BAUD_REG, 270);


	regWrite32(AUX_MU_CNTL_REG, 0x3); //enable receiver & transmitter

}

void uartWrite(int data) {
	//check fill state
	while(regRead32Bit(AUX_MU_LSR_REG, 5)==0); //wait until fifo not full

	regWrite32(AUX_MU_IO_REG, data);
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
	while(regRead32Bit(AUX_MU_LSR_REG, 0) == 0);
	
	return regRead32(AUX_MU_IO_REG);
}

bool uartAvailable() {
	return regRead32Bit(AUX_MU_LSR_REG, 0);
}




