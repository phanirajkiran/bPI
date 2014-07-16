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

#ifdef __cplusplus
extern "C" {
#endif

#include <kernel/printk.h>
#include <kernel/interrupt.h>

//FIXME: make __FILE__ output optional, since this increases the compiled binary
#define panic(format, ...) \
	do { \
	disableInterrupts(); \
	printk("\n\nI'm really sorry to tell you, but... Something terrible happened:\n"); \
	printk(format, ## __VA_ARGS__); \
	printk(" (file %s:%i in %s)\n", __FILE__, __LINE__, __FUNCTION__); \
	printk("There is nothing I can do anymore...\n"); \
	while(1); \
	} while(0)


#ifdef __cplusplus
}
#endif
#endif /* PANIC_HEADER_H_ */

