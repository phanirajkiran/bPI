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
 * UART
 */

#ifndef SERIAL_HEADER_H_
#define SERIAL_HEADER_H_

#include <kernel/types.h>
#include <serial_arch.h>


void initUart();

/* write 8bits to uart (blocking)
 * data must be 8bits (lsb bits are used) 
 */ 
void uartWrite(int data);

/* write a 0 terminated string to uart */
void uartWriteStr(char* str);

void uartWriteBuf(char* str, int len);

/* read 8 bits, (blocking), returns -1 on error */
int uartRead();

/* are uart data available to read? */
bool uartAvailable();


#ifndef ARCH_HAS_SERIAL

#define initUart() while(0);

#define uartWrite(i) while(0);
#define uartWriteStr(i) while(0);
#define uartWriteBuf(i, j) while(0);

#define uartRead() (-1)

#define uartAvailable() false

#endif /* ARCH_HAS_SERIAL */



#endif /* SERIAL_HEADER_H_ */

