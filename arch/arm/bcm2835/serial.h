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

#ifndef BCM2835_SERIAL_HEADER_H_
#define BCM2835_SERIAL_HEADER_H_

#include "common.h"

/* hardware addresses */
#define AUX_USART_BASE     (BCM2835_PERI_BASE+0x00215000)
#define AUX_ENABLES        (AUX_USART_BASE+0x04)
#define AUX_MU_IO_REG      (AUX_USART_BASE+0x40)
#define AUX_MU_IER_REG     (AUX_USART_BASE+0x44)
#define AUX_MU_IIR_REG     (AUX_USART_BASE+0x48)
#define AUX_MU_LCR_REG     (AUX_USART_BASE+0x4C)
#define AUX_MU_MCR_REG     (AUX_USART_BASE+0x50)
#define AUX_MU_LSR_REG     (AUX_USART_BASE+0x54)
#define AUX_MU_MSR_REG     (AUX_USART_BASE+0x58)
#define AUX_MU_SCRATCH     (AUX_USART_BASE+0x5C)
#define AUX_MU_CNTL_REG    (AUX_USART_BASE+0x60)
#define AUX_MU_STAT_REG    (AUX_USART_BASE+0x64)
#define AUX_MU_BAUD_REG    (AUX_USART_BASE+0x68)


#endif /* BCM2835_SERIAL_HEADER_H_ */



