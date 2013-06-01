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

#ifndef BF537_TIMER_HEADER_H_
#define BF537_TIMER_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

#define BF537_TIMER_ENABLE             0xFFC00680

#define BF537_TIMER0_CONFIG            0xFFC00600
#define BF537_TIMER0_COUNTER           0xFFC00604
#define BF537_TIMER0_PERIOD            0xFFC00608
#define BF537_TIMER0_WIDTH             0xFFC0060C



#include <kernel/types.h>
/* current timestamp in microseconds */
uint32 __getTimestamp();
#define getTimestamp() (__getTimestamp() / SYSTEM_CLOCK)


void initTimer();

#ifdef __cplusplus
}
#endif
#endif /* BF537_TIMER_HEADER_H_ */




