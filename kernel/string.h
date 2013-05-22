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
 * char* string utils
 */

#ifndef STRING_HEADER_H_
#define STRING_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

/* same methods as in standard c library */


int strncmp(const char* str1, const char* str2, size_t num);




static inline size_t strlen(const char* str) {
	const char* c = str;
	while(*c != 0) ++c;
	return (size_t)c - (size_t)str - 1;
}


#ifdef __cplusplus
}
#endif
#endif /* STRING_HEADER_H_ */

