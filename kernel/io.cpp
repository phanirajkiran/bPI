/*
 * Copyright (C) 2014 Beat Küng <beat-kueng@gmx.net>
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

#include "io.hpp"
#include <kernel/errors.h>

Input::Input(FuncRead fread) : m_fread(fread) {
}

int Input::readByteBlocking() {
	int c;
	while((c=m_fread()) == -E_WOULD_BLOCK);
	return c;
}

InputBuffered::InputBuffered() : Input(0), m_next_char(0) {
}

int InputBuffered::readByte() {
	if(m_next_char >= m_buffer.length()) return -E_WOULD_BLOCK;
	return m_buffer[m_next_char++];
}

void InputBuffered::setData(const std::string& data) {
	m_buffer = data;
	reset();
}

void InputBuffered::appendData(const std::string& data) {
	m_buffer += data;
}

Output::Output(FuncWrite funcwrite) : m_fwrite(funcwrite) {
}

int Output::writeString(const std::string& str) {
	int ret;
	for(size_t i=0; i<str.length(); ++i)
		if((ret=writeByte(str[i])) < 0) return ret;
	return SUCCESS;
}


/* the following code is pretty much a copy of printk. but this makes it possible
 * to separate C++ from pure C code.
 */

#define vtohex(v) (char)((v) > 9 ? (v)-10+'a' : (v) + '0')

/* write a list of characters & take care of padding */
void Output::writeBuf(char* buffer, int len, int min_len, char padding) {
	if(len==0) {
		++len;
		buffer[0] = '0';
	}
	while(min_len > len) {
		writeByte(padding);
		--min_len;
	}
	while(len != 0)
		writeByte(buffer[--len]);
}

void Output::writeDecimal(unsigned int i, char* buffer, int min_len, 
		char padding) {
	int len = 0;
	while(i != 0) {
		buffer[len++] = (char)((i % 10) + '0');
		i/=10;
	}
	writeBuf(buffer, len, min_len, padding);
}

void Output::writeHumanReadable(unsigned int ui, char* buffer, int min_len, 
		char padding) {
	int i = 0;

	static const char* sizes[] = { "B", "KB", "MB", "GB", "TB", "PB", "EB" };

	while(true) {
		if((ui >> 10) != 0) {
			ui >>= 10;
		} else {
			writeDecimal(ui, buffer, min_len, padding);
			writeByte(' ');
			for(const char* c_size = sizes[i]; *c_size; ++c_size)
				writeByte(*c_size);
			break;
		}
		++i;
	}
}

int Output::vfprintf(const char *format, va_list ap) {
	int ret = 0;

	/* format arguments */
	int i;
	unsigned int ui;
	const char* str;
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
					writeByte('-');
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
					writeByte('0');
					writeByte('x');
					padding = '0';
				}
				writeBuf(buffer, len, min_len, padding);
				break;

			case 'c':
				i = va_arg(ap, int);
				writeByte(i);
				break;

			case 's':
				str = va_arg(ap, char*);
				while(*str) {
					writeByte(*str);
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
				writeByte('0');
				writeByte('x');
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
				writeByte(' '); writeByte('B'); writeByte(' '); writeByte('(');
				writeHumanReadable(ui, buffer, min_len, padding);
				writeByte(')');
				break;
			case '%':
				writeByte('%');
				break;

			default: 
				/* we want to avoid a recursive call to printk here */
				str = "(Format Error in printk!)\n";
				while(*str) {
					writeByte(*str);
					++str;
				}
				return -E_FORMAT;
			}
			++format;
			++ret;
		} else {
			writeByte(*format);
			++format;
		}
	}

	return ret;
}
int Output::printf(const char* format, ...) {
	   va_list arg;
	   int done;

	   va_start (arg, format);
	   done = vfprintf(format, arg);
	   va_end (arg);

	   return done;	
}

InputOutput::InputOutput(FuncRead fread, FuncWrite funcwrite)
	: Input(fread), Output(funcwrite) {
}
