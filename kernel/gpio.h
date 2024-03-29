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

#ifdef __cplusplus
extern "C" {
#endif
#include "registers.h"

#define GPIO_RISING_EDGE     BIT(0)
#define GPIO_FALLING_EDGE    BIT(1)
#define GPIO_BOTH_EDGES      (GPIO_RISING_EDGE | GPIO_FALLING_EDGE)

#include <kernel/utils.h>
#include <gpio_arch.h>

int getGpioAddress();

/**
 * set a specific gpio as input/ouput/...
 * see gpio.s for specific function values
 * pin < GPIO_COUNT
 * value=0: turn off, value!=0: turn on
 */
void setGpioFunction(int pin, int function);
void setGpio(int pin, int value);

int getGpio(int pin);


/**
 * pull up/down method
 * pin < GPIO_COUNT
 * pud: 0: off, 1: pull down, 2: pull up
 */
void setGpioPullUpDown(int pin, int pud);

/**
 * enable/disable GPIO edge detection (does not change interrupts)
 * @param pin <GPIO_COUNT
 * @param edge 0, GPIO_RISING_EDGE, GPIO_FALLING_EDGE or GPIO_BOTH_EDGES
 */
void setGpioEdgeDetect(int pin, int edge);


#ifndef ARCH_HAS_GPIO

#define getGpioAddress() 0
#define setGpioFunction(i, j) NOP
#define setGpio(i, j) NOP

#define getGpio(i) 0

#define setGpioPullUpDown(i, j) NOP

#define setGpioEdgeDetect(pin, edge) NOP

#endif /* ARCH_HAS_GPIO */


#ifdef __cplusplus
}
#endif
#endif /* GPIO_HEADER_H_ */
