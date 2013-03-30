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

#ifndef GPIO_PLATFORM_HEADER_H_
#define GPIO_PLATFORM_HEADER_H_


#ifdef BOARD_raspberry_pi
#include <bcm2835/gpio.h>
#endif /* BOARD_raspberry_pi */


/* gpio is board specific */
#include <gpio_board.h>


#ifdef BOARD_HAS_GPIO
#define PLATFORM_HAS_GPIO
#endif

#endif /* GPIO_PLATFORM_HEADER_H_ */


