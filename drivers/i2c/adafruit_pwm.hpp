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

#ifndef _ADAFRUIT_PWM_HEADER_H_
#define _ADAFRUIT_PWM_HEADER_H_

#include <algorithm>

#include <kernel/i2c.hpp>
#include <kernel/registers.h>

/** driver for the PCA9685 Adafruit I2C 16 PWM Board */
class I2CAdafruitPWM : public I2C {
public:
	
	/** registers */
	static const uchar MODE1 = 0x00;
	static const uchar MODE2 = 0x01;
	static const uchar SUBADR1 = 0x02;
	static const uchar SUBADR2 = 0x03;
	static const uchar SUBADR3 = 0x04;
	static const uchar PRESCALE = 0xFE;
	static const uchar LED0_ON_L = 0x06; /** 8 LSB bits of the 12bit value */
	static const uchar LED0_ON_H = 0x07; /** 4 MSB bits of the 12bit value + FULL ON bit */
	static const uchar LED0_OFF_L = 0x08;
	static const uchar LED0_OFF_H = 0x09;
	static const uchar ALLLED_ON_L = 0xFA;
	static const uchar ALLLED_ON_H = 0xFB;
	static const uchar ALLLED_OFF_L = 0xFC;
	static const uchar ALLLED_OFF_H = 0xFD;
	
	/** MODE1 bits */
	static const uchar MODE1_RESTART = BIT(7); /** restart after sleep */
	static const uchar MODE1_AI = BIT(5); /** auto increment */
	static const uchar MODE1_SLEEP = BIT(4); /** enable/disable oscillator */
	
	/** ON_H bits */
	static const uchar LED_ON_H_FULL = BIT(4);
	/** OFF_H bits */
	static const uchar LED_OFF_H_FULL = BIT(4);


	/** constructor: does not reset the device */
	I2CAdafruitPWM(FuncI2CWrite func_write, FuncI2CRead func_read, int addr=0b1000000);
	
	/** reset the device (not restart)
	 * this also enables the oscillator (PWM cannot be controlled with disabled oscillator)
	 * but PWM output is always LOW
	 */
	void reset();

	/**
	 * set the PWM frequency for all channels
	 * @param freq Frequency in Hz [40, 1000]
	 * @return true on success
	 */
	bool setPWMFreq(int freq);
	
	int getPWMFreq();
	
	/**
	 * change on and off-time of a channel.
	 * @param channel [0,15]
	 * @param on_time 12bit value, when to turn on
	 * @param off_time 12bit value, when to turn off (duty time = off_time-on_time)
	 * @return true on success
	 */
	bool setPWM(int channel, uint16 on_time, uint16 off_time);
	
	/**
	 * change duty cycle of a channel.
	 * TODO: if on-time is known to be 0, we could only set off-time and save 2 bytes
	 * @param channel [0,15]
	 * @param duty_time 12bit value -> duty cycle = duty_time/4095
	 * @return true on success
	 */
	bool setPWM(int channel, uint16 duty_time) {
		return setPWM(channel, 0, std::min((uint16)4095, duty_time));
	}
	
	/**
	 * get 'off time' (if 'on time' is 0 then this is the pulse length)
	 */
	uint16 getPWMDuty(int channel);
	
	/**
	 * set always on or always off
	 * @param channel [0,15] or -1 for all channels
	 * @return true on success
	 */
	bool setFull(int channel=-1, bool always_on = false);
private:
};


#endif /* _ADAFRUIT_PWM_HEADER_H_ */


