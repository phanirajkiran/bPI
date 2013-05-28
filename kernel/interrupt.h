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

#ifndef INTERRUPT_HEADER_H_
#define INTERRUPT_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <interrupt_arch.h>

/*
 * enable all interrupts. does not enable any specific device IRQ's, but IRQ's
 * in general. can be nested.
 */
void enableInterrupts();
void disableInterrupts();

#ifdef ARCH_HAS_INTERRUPT

/*
 * test whether we are inside an interrupt handler.
 * return: 0 if not inside an interrupt handler
 */
uint inInterrupt();

void __enableInterrupts();
void __disableInterrupts();

void handleTimerIRQ();

#else
# define __enableInterrupts() while(0)
# define __disableInterrupts() while(0)
#endif


#ifdef __cplusplus
}
#endif
#endif /* INTERRUPT_HEADER_H_ */



