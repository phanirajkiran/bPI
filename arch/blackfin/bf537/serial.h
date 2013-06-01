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

#ifndef BF537_SERIAL_HEADER_H_
#define BF537_SERIAL_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"


#define BF537_UART0_LCR              0xFFC0040C
/* LCR bits */
#define BF537_LCR_WLS                0 /* word lenth select (2 bits wide) */
#define BF537_LCR_STB                2 /* stop bits (0=1 stop bit, 1=2 stop bits */
#define BF537_LCR_PEN                3 /* parity bit enable */
#define BF537_LCR_DLAB               7 /* divisor latch access */

#define BF537_UART0_LSR              0xFFC00414
/* LSR bits */
#define BF537_LSR_DR                 0 /* data ready */
#define BF537_LSR_THRE               5 /* transmit empty */

#define BF537_UART0_THR              0xFFC00400 /* transmit register (if DLAB=0) */
#define BF537_UART0_RBR              0xFFC00400 /* receive register (if DLAB=0) */


#define BF537_UART0_DLL              0xFFC00400 /* low byte of divisor (if DLAB=1) */
#define BF537_UART0_DLH              0xFFC00404 /* high byte of divisor (if DLAB=1) */


#define BF537_UART0_GCTL             0xFFC00424
/* GCTL bits */
#define BF537_GCTL_UCEN              0 /* set to enable UART controller */



#ifdef __cplusplus
}
#endif
#endif /* BF537_SERIAL_HEADER_H_ */



