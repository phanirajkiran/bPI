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

#ifndef BCM2835_TIMER_HEADER_H_
#define BCM2835_TIMER_HEADER_H_

#include "common.h"

#include <kernel/types.h>

#define ARM_TIMER_BASE             (BCM2835_PERI_BASE+0xb000)
#define ARM_TIMER_LOAD             (ARM_TIMER_BASE+0x400)
#define ARM_TIMER_VALUE            (ARM_TIMER_BASE+0x404)
#define ARM_TIMER_CONTROL          (ARM_TIMER_BASE+0x408)
#define ARM_TIMER_IRQ_CLR          (ARM_TIMER_BASE+0x40c)
#define ARM_TIMER_MASKED_IRQ       (ARM_TIMER_BASE+0x414)
#define ARM_TIMER_RELOAD           (ARM_TIMER_BASE+0x418)
#define ARM_TIMER_PRE_DIVIDE       (ARM_TIMER_BASE+0x41c)

/* ARM_TIMER_CONTROL register bits */
#define ARM_TIMER_CTRL_IRQ_ENAB    5
#define ARM_TIMER_CTRL_LEN         1 /* 1: use 32 bit register, 0: use 16 bits */
#define ARM_TIMER_CTRL_TIMER_ENAB  7


/*
 * schedule a timer interrupt x ms from now. does not enable the timer. this
 * will abort & reset a currently set timer irq
 */
void setNextTimerIRQ(uint ms);


#endif /* BCM2835_TIMER_HEADER_H_ */



