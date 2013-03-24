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

#include "led_board.h"
#include <kernel/gpio.h>

void initLed(int which) {
	//we have only one led
	setGpioFunction(LED_GPIO, 1); //set LED GPIO as output
}
void initLeds() {
	for(int i=0; i<LED_COUNT; ++i) 
		initLed(i);
}

void toggleLed(int which) {
	//TODO
}

void ledOn(int which) {
	setGpio(LED_GPIO, 0);
}
void ledOff(int which) {
	setGpio(LED_GPIO, 1);
}

void setLed(int which, int value) {
	if(value) ledOn(0);
	else ledOff(0);
}
