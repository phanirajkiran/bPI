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

#include "pwm.h"

#include <kernel/timer.h>
#include <kernel/registers.h>

#define DELAY 150

#define WAIT_FOR_BUSY_FLAG \
	udelay(DELAY); \
	while(regRead32Bit(PWM_CLK_CNTL, PWM_CLK_CNTL_BUSY)); \
	udelay(DELAY);


void initPWM(int clock_source, int mash) {
	uint32 ctl_reg = regRead32(PWM_CTL);

	clearBit(ctl_reg, PWM_CTL_MODE0); //use pwm mode
	clearBit(ctl_reg, PWM_CTL_MSEN0); //submode: use pwm
	setBit(ctl_reg, PWM_CTL_USEF0); //use fifo
	clearBit(ctl_reg, PWM_CTL_POLA0); //use default polarization
	clearBit(ctl_reg, PWM_CTL_RPTL0);

	clearBit(ctl_reg, PWM_CTL_MODE1); //use pwm mode
	clearBit(ctl_reg, PWM_CTL_MSEN1); //submode: use pwm
	setBit(ctl_reg, PWM_CTL_USEF1); //use fifo
	clearBit(ctl_reg, PWM_CTL_POLA1); //use default polarization
	clearBit(ctl_reg, PWM_CTL_RPTL1);

	regWrite32(PWM_CTL, ctl_reg);


	/* enable the PWM clock */

	udelay(DELAY);
	regWrite32(PWM_CLK_CNTL, BCM_PASSWORD | clock_source); //stop Clock
	WAIT_FOR_BUSY_FLAG;
	regWrite32(PWM_CLK_CNTL, BCM_PASSWORD
			| 0x10 | clock_source); //start Clock
	//we should not enable MASH at the same time as enabling the clock, so do it
	//in 2 steps
	udelay(DELAY);
	regWrite32(PWM_CLK_CNTL, BCM_PASSWORD | clock_source); //stop Clock
	WAIT_FOR_BUSY_FLAG;
	regWrite32(PWM_CLK_CNTL, BCM_PASSWORD
			| (mash << 9) | 0x10 | clock_source);
	udelay(DELAY);
}

uint getPWMClockFreq(int clock_source) {
	switch(clock_source) {
	case 1: return 19200e3;
	case 6: return 500e6;
	default: return 0;
	}
}

void setPWMClockDivisor(uint divisor, uint frac_part, bool restart_clock) {
	uint32 clk_cntl;
	if(restart_clock) {
		clk_cntl = regRead32(PWM_CLK_CNTL);
		clearBit(clk_cntl, PWM_CLK_CNTL_ENAB);
		udelay(DELAY);
		regWrite32(PWM_CLK_CNTL, BCM_PASSWORD | clk_cntl); //stop Clock

		WAIT_FOR_BUSY_FLAG;
	}

	regWrite32(PWM_CLK_DIV, BCM_PASSWORD | (divisor<<12) 
			| (frac_part & ((1<<12)-1)));
	

	if(restart_clock) {
		udelay(DELAY);
		regWrite32(PWM_CLK_CNTL, BCM_PASSWORD | clk_cntl
				| (1<<PWM_CLK_CNTL_ENAB)); //restore
	}
}

void setPWMRange(uint32 range, int channels) {
	if(channels & (1<<0)) regWrite32(PWM_RNG0, range);
	if(channels & (1<<1)) regWrite32(PWM_RNG1, range);
}

uint32 getPWMRange(int channel) {
	if(channel & (1<<0)) return regRead32(PWM_RNG0);
	return regRead32(PWM_RNG1);
}

void clearPWMFifo() {
	regRMW32(PWM_CTL, PWM_CTL_CLRF0, 1);
}

void setPWMChannels(int channels) {
	if(channels & (1<<0)) regRMW32(PWM_CTL, PWM_CTL_PWEN0, 1);
	if(channels & (1<<1)) regRMW32(PWM_CTL, PWM_CTL_PWEN1, 1);
}

int isPWMFifoFull() {
	return regRead32Bit(PWM_STA, PWM_STA_FULL0);
}

void PWMWriteToFifo(uint32 value) {
	regWrite32(PWM_FIFO, value);
}



