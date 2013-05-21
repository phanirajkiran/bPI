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

#ifndef LED_HEADER_H_
#define LED_HEADER_H_


#include <led_arch.h>

void initLed(int which);
void initLeds();

void toggleLed(int which);

void ledOn(int which);
void ledOff(int which);

/*! set led: 
 * value=0: off, value!=0: on
 */
void setLed(int which, int value);
int getLed(int which);

#ifndef ARCH_HAS_LED

#define initLed(i) while(0);
#define initLeds() while(0);
#define toggleLed(i) while(0);
#define ledOn(i) while(0);
#define ledOff(i) while(0);
#define setLed(i, j) while(0);
#define getLed(i) 0

#endif /* ARCH_HAS_LED */



#endif /* LED_HEADER_H_ */
