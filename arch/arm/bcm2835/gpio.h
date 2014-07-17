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

#ifndef BCM2835_GPIO_HEADER_H_
#define BCM2835_GPIO_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

#define BCM2835_GPIO_BASE  (BCM2835_PERI_BASE+0x00200000)
#define BCM2835_GPIO_GPEDS0             (BCM2835_GPIO_BASE+0x40) //event detect status
#define BCM2835_GPIO_GPEDS1             (BCM2835_GPIO_BASE+0x44)
#define BCM2835_GPIO_GPREN0             (BCM2835_GPIO_BASE+0x4C) //rising edge detect enable
#define BCM2835_GPIO_GPREN1             (BCM2835_GPIO_BASE+0x50)
#define BCM2835_GPIO_GPFEN0             (BCM2835_GPIO_BASE+0x58) //falling edge detect enable
#define BCM2835_GPIO_GPFEN1             (BCM2835_GPIO_BASE+0x5C)
#define BCM2835_GPIO_GPPUD              (BCM2835_GPIO_BASE+0x94)
#define BCM2835_GPIO_GPPUDCLK0          (BCM2835_GPIO_BASE+0x98)
#define BCM2835_GPIO_GPPUDCLK1          (BCM2835_GPIO_BASE+0x9C)

#define GPIO_COUNT 54
#define GPIO_BANKS 2


#ifdef __cplusplus
}
#endif
#endif /* BCM2835_GPIO_HEADER_H_ */



