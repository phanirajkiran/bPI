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

#include "endian.h"


int32 LE32(int32 value) {
	if(O32_HOST_ORDER == O32_LITTLE_ENDIAN) //we are on LE system
		return value;
	return REVERT_ENDIAN32(value);
}

int16 LE16(int16 value) {
	if(O32_HOST_ORDER == O32_LITTLE_ENDIAN) //we are on LE system
		return value;
	return REVERT_ENDIAN16(value);
}


