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

#include "adafruit_pwm.hpp"
#include <kernel/utils.h>

I2CAdafruitPWM::I2CAdafruitPWM(FuncI2CWrite func_write, FuncI2CRead func_read, int addr)
	: I2C(func_write, func_read, addr) {
}

void I2CAdafruitPWM::reset() {
	char buffer[2];
	buffer[0] = MODE1;
	buffer[1] = MODE1_AI; /* enable auto increment */
	write(buffer, 2);
	udelay(500); //wait for oscillator to be on
}

bool I2CAdafruitPWM::setPWMFreq(int freq) {
	ASSERT(freq>=40 && freq<=1000);

	int prescale_val = 25e6; //internal clock is 25MHz
	prescale_val /= 4096;
	prescale_val = prescale_val/freq - 1;
	uchar old_mode;
	if(readRegister(MODE1, old_mode) != 1) return false;
	//disable oscillator
	uchar new_mode = (old_mode & 0x7f) | MODE1_SLEEP;
	if(writeRegister(MODE1, new_mode) != 2) return false;
	if(writeRegister(PRESCALE, (uchar)prescale_val) != 2) return false;
	if(writeRegister(MODE1, old_mode) != 2) return false; //reenable
	udelay(500);
	if(writeRegister(MODE1, old_mode | MODE1_RESTART) != 2) return false; //restart PWM

	return true;
}
int I2CAdafruitPWM::getPWMFreq() {
	char buffer[1];
	buffer[0] = PRESCALE;
	if(write(buffer, 1) != 1) return 0;
	if(read(buffer, 1) != 1) return 0;
	int prescale = (uchar)buffer[0];
	return 25e6/4096 / (prescale+1);
}

bool I2CAdafruitPWM::setPWM(int channel, uint16 on_time, uint16 off_time) {
	char buffer[5];
	buffer[0] = LED0_ON_L + 4*channel;
	buffer[1] = (char)(on_time & 0xff);
	buffer[2] = (char)(on_time >> 8);
	buffer[3] = (char)(off_time & 0xff);
	buffer[4] = (char)(off_time >> 8);
	return write(buffer, 5) == 5;
}

uint16 I2CAdafruitPWM::getPWMDuty(int channel) {
	char buffer[2];
	buffer[0] = LED0_OFF_L + 4*channel;
	if(write(buffer, 1) != 1) return 0;
	if(read(buffer, 2) != 2) return 0;
	return (uint16)buffer[0] | ((uint16)(buffer[1]&0xf)<<8);
}
bool I2CAdafruitPWM::setFull(int channel, bool always_on) {
	if(always_on) {
		char buffer[5];
		buffer[0] = channel == -1 ? ALLLED_ON_L : (LED0_ON_L + 4*channel);
		buffer[1] = 0;
		buffer[2] = LED_ON_H_FULL;
		buffer[3] = 0;
		buffer[4] = 0;
		return write(buffer, 5) == 5;
	} else {
		char buffer[2];
		buffer[0] = channel == -1 ? ALLLED_OFF_H : (LED0_OFF_H + 4*channel);
		buffer[1] = LED_OFF_H_FULL;
		return write(buffer, 2) == 2;
	}
}
