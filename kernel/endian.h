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


#ifndef ENDIAN_HEADER_H_
#define ENDIAN_HEADER_H_


#include "types.h"


#include <limits.h>
#if CHAR_BIT != 8
#error "unsupported char size"
#endif


enum
{
    O32_LITTLE_ENDIAN = 0x03020100ul,
    O32_BIG_ENDIAN = 0x00010203ul,
    O32_PDP_ENDIAN = 0x01000302ul
};

static const union { unsigned char bytes[4]; uint32 value; } o32_host_order =
    { { 0, 1, 2, 3 } };

#define O32_HOST_ORDER (o32_host_order.value)


/* convert a native value <--> little endian */
inline int32 LE32(int32 value);
inline int16 LE16(int16 value);


#define REVERT_ENDIAN32(value) \
	(((value>>24) & 0xff) | \
	((value>>8)   & 0xff00) | \
	((value<<8)   & 0xff0000) | \
	((value<<24)  & 0xff000000))

#define REVERT_ENDIAN16(value) \
	((value>>8)   & 0xff) | \
	((value<<8)   & 0xff00) 

#endif /* ENDIAN_HEADER_H_ */


