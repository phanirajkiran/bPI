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
 * kernel panic: unrecoverable error
 */

#ifndef PANIC_HEADER_H_
#define PANIC_HEADER_H_

#include <kernel/printk.h>

#define panic(format, ...) \
	do { \
	printk("I'm really sorry to tell you, but... Something terrible happened.\n"); \
	printk(format, ## __VA_ARGS__); \
	while(1); \
	} while(0)



#endif /* PANIC_HEADER_H_ */

