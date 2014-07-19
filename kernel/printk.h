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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

typedef void (*printkOutput)(char c);


/* add & remove printk outputs (not checked for duplicates) */
int addPrintkOutput(printkOutput output);
int removePrintkOutput(printkOutput output);

enum LogLevel {
	LogLevel_debug = 0,
	LogLevel_all = LogLevel_debug,
	LogLevel_info,
	LogLevel_warn,
	LogLevel_error,
	LogLevel_critical,
};

extern enum LogLevel g_log_level; /** current log level: log all levels equal or above this level */

/**
 *  printk: similar to the printf function in C
 * returns the number of printed arguments or a negative error number
 * 
 * @param level log level
 * @param format
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
 * r		human readable size of bytes in decimal, unsigned int (eg 50 Mb)
 * R		human readable size of bytes with exact num of bytes, unsigned int
 * 			(eg 4535 bytes (4 Kb))
 * %		print a single %
 */
int printk(enum LogLevel level, const char *format, ...);

/** some convenience methods */
#define printk_d(format, ...) printk(LogLevel_debug, format, ## __VA_ARGS__)
#define printk_i(format, ...) printk(LogLevel_info, format, ## __VA_ARGS__)
#define printk_w(format, ...) printk(LogLevel_warn, format, ## __VA_ARGS__)
#define printk_e(format, ...) printk(LogLevel_error, format, ## __VA_ARGS__)
#define printk_crit(format, ...) printk(LogLevel_critical, format, ## __VA_ARGS__)

int vfprintk(enum LogLevel level, const char *format, va_list ap);


#ifdef __cplusplus
}
#endif
#endif /* PRINTK_HEADER_H_ */
