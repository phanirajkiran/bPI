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
 * GPIO stuff, if the board supports it
 */

#ifndef GPIO_HEADER_H_
#define GPIO_HEADER_H_

#include <gpio_arch.h>

int getGpioAddress();

/* set a specific gpio as input/ouput/...
 * see gpio.s for specific function values
 * pin < GPIO_COUNT
 * value=0: turn off, value!=0: turn on
 */
void setGpioFunction(int pin, int function);
void setGpio(int pin, int value);

int getGpio(int pin);


/* pull up/down method
 * pin < GPIO_COUNT
 * pud: 0: off, 1: pull down, 2: pull up
 */
void setGpioPullUpDown(int pin, int pud);


#ifndef ARCH_HAS_GPIO

#define getGpioAddress() 0
#define setGpioFunction(i, j) while(0);
#define setGpio(i, j) while(0);

#define getGpio(i) 0

#define setGpioPullUpDown(i, j) while(0);

#endif /* ARCH_HAS_GPIO */



#endif /* GPIO_HEADER_H_ */
