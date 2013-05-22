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

#include "utils.h"


void* memset(void* ptr, int value, size_t num) {
	char* end = (char*)ptr + num;
	char v = value & 0xff;
	char* p = (char*)ptr;
	while(p != end) {
		*p = v;
		++p;
	}
	return ptr;
}

void* memcpy(void* destination, const void* source, size_t num) {
	char* end = (char*)destination + num;
	char* ret = (char*)destination;
	char* s = (char*)source;
	char* d = (char*)destination;
	while(d != end)
		*(d++) = *(s++);
	return ret;
}

char* strcpy(char* destination, const char* source) {
	char* ret = destination;
	while(*source) {
		*destination = *source;
		++destination;
		++source;
	}
	*destination = 0;
	return ret;
}

char* strncpy(char* destination, const char* source, size_t num) {
	char* ret = destination;
	if(num==0) return ret;
	--num;
	while(*source && num) {
		*destination = *source;
		++destination;
		++source;
		--num;
	}
	*destination = 0;
	return ret;
}

void* memmove(void * destination, const void * source, size_t num) {
	void* ret = destination;
	const char* src = (const char*)source;
	char* dst = (char*)destination;
	char* end = (char*)dst + num;
	if(destination < source) {
		while(dst != end)
			*(dst++) = *(src++);
	} else if(source < destination) {
		src += num;
		while(dst != end)
			*(--end) = *(--src);
	}

	return ret;
}

int memcmp(const void * ptr1, const void * ptr2, size_t num) {
	const uchar* p1=(const uchar*) ptr1;
	const uchar* p2=(const uchar*) ptr2;
	const uchar* end = p1+num;
	while(p1 != end) {
		if(*p1 > *p2) return 1;
		else if(*p1 < *p2) return -1;
		++p1;
		++p2;
	}

	return 0;
}

char kernel_cmd_line[COMMAND_LINE_LEN];
