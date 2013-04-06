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

/*
 * called when division by 0 happens. this should never happen, it's a serious
 * error
 */
void raise() {
	//TODO: panic or something...
}

char kernel_cmd_line[COMMAND_LINE_LEN];
