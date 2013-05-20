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

#include <interrupt_arch.h>


#ifdef ARCH_HAS_INTERRUPT

/*
 * enable all interrupts. does not enable any specific device IRQ's, but IRQ's
 * in general
 */
inline void enableInterrupts();

inline void handleTimerIRQ();

#else
# define enableInterrupts() while(0)
#endif

#endif /* INTERRUPT_HEADER_H_ */



