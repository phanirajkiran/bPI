/*
 * Copyright (C) 2014 Beat Küng <beat-kueng@gmx.net>
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

#include "i2c.h"

#include <kernel/registers.h>
#include <kernel/i2c.h>
#include <kernel/gpio.h>

static inline int initTransfer(bool read, int addr, int len);

#define OPERATION_TIMEOUT 50000 //in usec

#define readI2CReg(reg) regRead32(BCM2835_I2C1_BASE + (reg))
#define writeI2CReg(reg, val) regWrite32(BCM2835_I2C1_BASE + (reg), (val))


void initI2C() {
	/* set up gpio */
	setGpioFunction(2, 0b100); //SDA1
	setGpioFunction(3, 0b100); //SCL1

	/* init i2c */
	uint32 i2c_freq = 400; //[kHz], default is 100kHz
	uint32 divider = 250000 / i2c_freq; //core_clk is nominally 250 MHz
								//(data sheet seems to be wrong)
	/*
	 * Per the datasheet, the register is always interpreted as an even
	 * number, by rounding down. In other words, the LSB is ignored. So,
	 * if the LSB is set, increment the divider to avoid any issue.
	 */
	if (divider & 1)
		divider++;
	writeI2CReg(BCM2835_I2C_DIV, divider);
	writeI2CReg(BCM2835_I2C_C, 0);
}

int i2cRead(int addr, char* buf, int len) {
	if(initTransfer(true, addr, len)) return 0;
	
	timeout_init(timeout, OPERATION_TIMEOUT);

	for(int i=0; i<len; ++i) {
		uint32 val = readI2CReg(BCM2835_I2C_S);
		if (!(val & BCM2835_I2C_S_RXD)) {
			//check & reset errors
			writeI2CReg(BCM2835_I2C_S, val);
			if (val & (BCM2835_I2C_S_CLKT | BCM2835_I2C_S_ERR)) {
				return i;
			}

			//wait until FIFO non-empty
			while(!(readI2CReg(BCM2835_I2C_S) & BCM2835_I2C_S_RXD) && !timed_out(timeout));
			if(timed_out(timeout)) {
				printk_w("Warning: I2C read from address %i timed out\n", addr);
				return -1;
			}
		}
		buf[i] = readI2CReg(BCM2835_I2C_FIFO);
	}
	
	//wait until finished
	while(!(readI2CReg(BCM2835_I2C_S) & BCM2835_I2C_S_DONE) && !timed_out(timeout));
	if(timed_out(timeout)) {
		printk_w("Warning: I2C read from address %i timed out\n", addr);
		return -1;
	}
	writeI2CReg(BCM2835_I2C_S, BCM2835_I2C_S_DONE);
	writeI2CReg(BCM2835_I2C_C, BCM2835_I2C_C_CLEAR);
	return len;
}

int i2cWrite(int addr, char* buf, int len) {
	if(initTransfer(false, addr, len)) return 0;
	
	timeout_init(timeout, OPERATION_TIMEOUT);
	
	for(int i=0; i<len; ++i) {
		uint32 val = readI2CReg(BCM2835_I2C_S);
		if (!(val & BCM2835_I2C_S_TXD)) {
			//check & reset errors
			writeI2CReg(BCM2835_I2C_S, val);
			if (val & (BCM2835_I2C_S_CLKT | BCM2835_I2C_S_ERR)) {
				return i;
			}
			
			//wait until FIFO non-full
			while(!(readI2CReg(BCM2835_I2C_S) & BCM2835_I2C_S_TXD) && !timed_out(timeout));
			if(timed_out(timeout)) {
				printk_w("Warning: I2C write to address %i timed out\n", addr);
				return -1;
			}
		}
		writeI2CReg(BCM2835_I2C_FIFO, (uint32)buf[i]);
	}
	
	//wait until finished
	while(!(readI2CReg(BCM2835_I2C_S) & BCM2835_I2C_S_DONE) && !timed_out(timeout));
	if(timed_out(timeout)) {
		printk_w("Warning: I2C write to address %i timed out\n", addr);
		return -1;
	}
	writeI2CReg(BCM2835_I2C_S, BCM2835_I2C_S_DONE);
	writeI2CReg(BCM2835_I2C_C, BCM2835_I2C_C_CLEAR);
	return len;
}

int initTransfer(bool read, int addr, int len) {
	writeI2CReg(BCM2835_I2C_C, BCM2835_I2C_C_CLEAR);

	uint32 c = 0;
	if (read)
		c = BCM2835_I2C_C_READ;

	c |= BCM2835_I2C_C_ST | BCM2835_I2C_C_I2CEN;

	writeI2CReg(BCM2835_I2C_A, addr);
	writeI2CReg(BCM2835_I2C_DLEN, len);
	writeI2CReg(BCM2835_I2C_C, c);

	uint32 val = readI2CReg(BCM2835_I2C_S);
	//check & reset errors
	if(val & (BCM2835_I2C_S_CLKT | BCM2835_I2C_S_ERR)) {
		writeI2CReg(BCM2835_I2C_S, val | BCM2835_I2C_S_DONE);
		writeI2CReg(BCM2835_I2C_C, BCM2835_I2C_C_CLEAR);
		return -1;
	}
	return 0;
}

