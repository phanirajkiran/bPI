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

#ifndef BOARD_GPIO_HEADER_H_
#define BOARD_GPIO_HEADER_H_

#define LED_GPIO 16

#define GPIO_COUNT 54

int getGpioAddress();

/* set a specific gpio as input/ouput/...
 * see gpio.s for specific function values
 * pin < GPIO_COUNT
 * value=0: turn off, value!=0: turn on
 */
void setGpioFunction(int pin, int function);
void setGpio(int pin, int value);


#endif /* BOARD_GPIO_HEADER_H_ */


