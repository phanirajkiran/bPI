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

#ifndef BCM2835_PWM_HEADER_H_
#define BCM2835_PWM_HEADER_H_

#include "common.h"

#include <kernel/types.h>


#define PWM_BASE                  0x2020C000
#define PWM_CTL                   (PWM_BASE+0x0)
#define PWM_STA                   (PWM_BASE+0x4)
#define PWM_RNG0                  (PWM_BASE+0x10)
#define PWM_FIFO                  (PWM_BASE+0x18)
#define PWM_RNG1                  (PWM_BASE+0x20)

/* PWM bits */
#define PWM_CTL_PWEN0             0 /* set bit: enable channel */
#define PWM_CTL_MODE0             1 /* choose mode: clear for PWM, set for 
                                       serial */
#define PWM_CTL_RPTL0             2 /* set bit: repeat last data in fifo, when 
									   fifo empty */
#define PWM_CTL_MSEN0             7 /* select submode if MODE0=0 */
#define PWM_CTL_USEF0             5 /* use fifo if set */
#define PWM_CTL_POLA0             4 /* invert polarization if set */

#define PWM_CTL_PWEN1             8
#define PWM_CTL_MODE1             9
#define PWM_CTL_RPTL1             10
#define PWM_CTL_MSEN1             15
#define PWM_CTL_USEF1             13
#define PWM_CTL_POLA1             12

#define PWM_CTL_CLRF0             6 /* set to clear the fifo */


#define PWM_STA_FULL0             0
#define PWM_STA_RERR1             3


/* PWM clock
 * these addresses are taken from wiringPi, I cannot find these in the manual :(
 */
#define PWM_CLK_BASE              0x20101000
#define	PWM_CLK_CNTL              (PWM_CLK_BASE+0xa0)
#define	PWM_CLK_DIV	              (PWM_CLK_BASE+0xa4)

/* PWM clock bits */
#define PWM_CLK_CNTL_BUSY         7
#define PWM_CLK_CNTL_ENAB         4


#define PWM_MAX_DIVISOR_FRAC      0xfff


/*
 * init the PWM to be used for FIFO, but does not enable the channels or set the
 * range.
 * clock_source: 1: oscillator (19.2 MHz)
 *               4: PLLA per (not working ??)
 *               5: PLLC per (not working ??)
 *               6: PLLD per
 * mash: 0: use only int for divisor (no MASH)
 *       1: 1-stage MASH
 *       2: 2-stage MASH
 *       3: 3-stage MASH
 */
void initPWM(int clock_source, int mash);

/*
 * get clock frequency in Hz
 */
uint getPWMClockFreq(int clock_source);

/*
 * set clock divisor
 * divisor: min: 2, max: 0xfff
 * frac_part: (only if MASH!=0) min: 0, max: 0xfff
 * restart_clock: whether to restart clock (avoid glitches)
 */
void setPWMClockDivisor(uint divisor, uint frac_part, bool restart_clock);

/*
 * set value range of PWM
 * channels: LSB: channel 0, ... (max 2 channels)
 */
void setPWMRange(uint32 range, int channels);

uint32 getPWMRange(int channel);

/*
 * enable/disable channels
 * channels: LSB: channel 0, ... (max 2 channels)
 */
inline void setPWMChannels(int channels);

inline void clearPWMFifo();


/* check if fifo full: return 0 if not full */
inline int isPWMFifoFull();

/* non-blocking writ to fifo (must check that fifo is not full) */
inline void PWMWriteToFifo(uint32 value);


#endif /* BCM2835_PWM_HEADER_H_ */




