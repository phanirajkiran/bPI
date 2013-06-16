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


#ifndef TYPES_HEADER_H_
#define TYPES_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char      uchar;
typedef unsigned short     ushort;
typedef unsigned int       uint;
typedef unsigned long      ulong;

/* size_t type */
#include <stddef.h>


/* c lib includes */
#if !defined(__cpluscplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <limits.h>

#define INT_BITS (sizeof(int)*CHAR_BIT)


#include <types_arch.h>



#ifdef __cplusplus
}
#endif
#endif /* TYPES_HEADER_H_ */

