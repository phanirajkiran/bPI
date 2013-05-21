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

#include "printk.h"
#include <kernel/errors.h>
#include <kernel/utils.h>



/* output */
#define PRINTK_OUTPUTS 10
static printkOutput outputs[PRINTK_OUTPUTS];
static uint output_count = 0;

int addPrintkOutput(printkOutput output) {
	if(output_count >= PRINTK_OUTPUTS) return -E_BUFFER_FULL;

	outputs[output_count++] = output;

	return SUCCESS;
}


int removePrintkOutput(printkOutput output) {
	for(int i=0; i<output_count; ++i) {
		if(output == outputs[i]) {
			for(int j=i+1; j<output_count; ++j)
				outputs[j-1] = outputs[j];
			--output_count;
			return SUCCESS;
		}
	}
	return -E_NO_SUCH_RESOURCE;
}


static inline void writeChar(char c) {

	/* call each registered function */
	for(int i=0; i<output_count; ++i)
		(*outputs[i])(c);
}




#define vtohex(v) (char)((v) > 9 ? (v)-10+'a' : (v) + '0')


/* write a list of characters & take care of padding */
static inline void writeBuf(char* buffer, int len, int min_len, char padding) {
	if(len==0) {
		++len;
		buffer[0] = '0';
	}
	while(min_len > len) {
		writeChar(padding);
		--min_len;
	}
	while(len != 0)
		writeChar(buffer[--len]);
}

static inline void writeDecimal(unsigned int i, char* buffer, int min_len, 
		char padding) {
	int len = 0;
	while(i != 0) {
		buffer[len++] = (char)((i % 10) + '0');
		i/=10;
	}
	writeBuf(buffer, len, min_len, padding);
}

static inline void writeHumanReadable(unsigned int ui, char* buffer, int min_len, 
		char padding) {
	int i = 0;

	static char* sizes[] = { "B", "KB", "MB", "GB", "TB", "PB", "EB" };

	while(true) {
		if((ui >> 10) != 0) {
			ui >>= 10;
		} else {
			writeDecimal(ui, buffer, min_len, padding);
			writeChar(' ');
			for(char* c_size = sizes[i]; *c_size; ++c_size)
				writeChar(*c_size);
			break;
		}
		++i;
	}
}



int printk(const char *format, ...) {
   va_list arg;
   int done;

   va_start (arg, format);
   done = vfprintk(format, arg);
   va_end (arg);

   return done;
}

int vfprintk(const char *format, va_list ap) {
	int ret = 0;

	/* format arguments */
	int i;
	unsigned int ui;
	char* str;
	void* ptr;
	unsigned long ptr_val;

	char buffer[32]; //this is enough for at least 64 bit numbers
	int len;
	int hex_prefix, min_len;

	while(*format) {
		if(*format=='%') {
			++format;
			//check for flags
			char padding = ' ';
			if(*format == ' ' || *format == '0') {
				padding = *format;
				++format;
			}
			hex_prefix = 0;
			if(*format == '#') {
				hex_prefix = 1;
				++format;
			}
			//check width
			min_len = 0;
			while(*format >= '0' && *format <= '9') {
				min_len = (int)(*format - '0') + min_len * 10;
				++format;
			}
			//check specifier
			switch(*format) {
			case 'd':
			case 'i':
				i = va_arg(ap, int);
				if(i<0) {
					writeChar('-');
					i=-i;
					--min_len;
				}
				writeDecimal(i, buffer, min_len, padding);
				break;

			case 'u':
				ui = va_arg(ap, unsigned int);
				writeDecimal(ui, buffer, min_len, padding);
				break;

			case 'x':
				ui = va_arg(ap, unsigned int);

				len = 0;
				while(ui != 0) {
					buffer[len++] = vtohex(ui % 16);
					ui>>=4;
				}
				if(hex_prefix == 1) {
					min_len-=2;
					writeChar('0');
					writeChar('x');
					padding = '0';
				}
				writeBuf(buffer, len, min_len, padding);
				break;

			case 'c':
				i = va_arg(ap, int);
				writeChar(i);
				break;

			case 's':
				str = va_arg(ap, char*);
				while(*str) {
					writeChar(*str);
					++str;
				}
				break;

			case 'p':
				ptr = va_arg(ap, void*);
				ptr_val = (unsigned long)ptr;
				len = 0;
				while(ptr_val != 0) {
					buffer[len++] = vtohex(ptr_val % 16);
					ptr_val>>=4;
				}
				padding = '0';
				writeChar('0');
				writeChar('x');
				min_len-=2;
				writeBuf(buffer, len, min_len, padding);
				break;

			case 'r':
				ui = va_arg(ap, unsigned int);
				writeHumanReadable(ui, buffer, min_len, padding);
				break;
			case 'R':
				ui = va_arg(ap, unsigned int);
				writeDecimal(ui, buffer, min_len, padding);
				writeChar(' '); writeChar('B'); writeChar(' '); writeChar('(');
				writeHumanReadable(ui, buffer, min_len, padding);
				writeChar(')');
				break;
			case '%':
				writeChar('%');
				break;

			default: 
				/* we want to avoid a recursive call to printk here */
				str = "(Format Error in printk!)\n";
				while(*str) {
					writeChar(*str);
					++str;
				}
				return -E_FORMAT;
			}
			++format;
			++ret;
		} else {
			writeChar(*format);
			++format;
		}
	}

	return ret;
}

