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
 * the printf of the kernel
 */

#ifndef PRINTK_HEADER_H_
#define PRINTK_HEADER_H_

#include <stdarg.h>

/////////////////////////////////////////////////////
//TODO: extend this
typedef struct {
} Outputs;


/* printk: similar to the printf function in C
 * returns the number of printed arguments or a negative error number
 *
 * supported formats:
 * %[flags][width]specifier 
 *
 * flags:
 * (space)	pad with spaces (default padding)
 * 0		pad with 0 (this is forced for hex output with 0x)
 * #		for hex: preceed with 0x
 *
 * width:
 * (number)	minimum number of characters to be printed
 *
 * specifier:
 * d or i	signed int
 * u		unsigned int
 * x		unsigned int in hex
 * c		char
 * s		string (null terminated)
 * p		pointer address (in hex)
 * %		print a single %
 */
int printk(const char *format, ...);


int vfprintk(Outputs output, const char *format, va_list ap);

#endif /* PRINTK_HEADER_H_ */
