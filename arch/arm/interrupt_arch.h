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

#ifndef INTERRUPT_ARCH_HEADER_H_
#define INTERRUPT_ARCH_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif


#define ARCH_HAS_INTERRUPT

#ifndef __ASSEMBLY__
#include <kernel/types.h>

/*
 * setup and init the interrupts, but leave interrupts disabled.
 * also no device IRQ will be enabled
 */
void archInitInterrupts();

void archHandleTimerIRQ();
void archHandleGpioIRQ();

uint getTimerIRQCounter();
void resetTimerIRQCounter();

#endif /* __ASSEMBLY__ */

#include <bcm2835/interrupt.h>


#ifdef __cplusplus
}
#endif
#endif /* INTERRUPT_ARCH_HEADER_H_ */




