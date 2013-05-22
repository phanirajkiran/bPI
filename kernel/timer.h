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

/*!
 * timer related functions
 */

#ifndef TIMER_HEADER_H_
#define TIMER_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <timer_arch.h>


#ifndef ARCH_HAS_TIMER
#error "Architecture must define timer methods"
#endif /* ARCH_HAS_TIMER */


#define delay(ms) udelay((ms)*1000)


#ifdef __cplusplus
}
#endif
#endif /* TIMER_HEADER_H_ */
