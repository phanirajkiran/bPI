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
 * this file defines multiple smaller useful functions
 */

#ifndef UTILS_HEADER_H_
#define UTILS_HEADER_H_

#include "printk.h"
#include "types.h"


#include <utils_arch.h>

#define NULL 0

/* C standard memset function */
void* memset(void* ptr, int value, size_t num);

void* memcpy(void* destination, const void* source, size_t num);

/* C standard string copy function */
char* strcpy(char* destination, const char* source);
/* C standard string copy function with max length of buffer */
char* strncpy(char* destination, const char* source, size_t num);


//TODO: assert



/* command line arguments, passed by the bootloader */
#define COMMAND_LINE_LEN 1024
extern char kernel_cmd_line[COMMAND_LINE_LEN];


#endif /* UTILS_HEADER_H_ */
