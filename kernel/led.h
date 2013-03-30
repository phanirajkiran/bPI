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


#include <led_platform.h>

inline void initLed(int which);
inline void initLeds();

inline void toggleLed(int which);

inline void ledOn(int which);
inline void ledOff(int which);

/*! set led: 
 * value=0: off, value!=0: on
 */
inline void setLed(int which, int value);
inline int getLed(int which);

#ifndef PLATFORM_HAS_LED

#define initLed(i) while(0);
#define initLeds() while(0);
#define toggleLed(i) while(0);
#define ledOn(i) while(0);
#define ledOff(i) while(0);
#define setLed(i, j) while(0);
#define getLed(i) while(0);

#endif /* PLATFORM_HAS_LED */



#endif /* LED_HEADER_H_ */
