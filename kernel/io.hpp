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

/*! 
 * This file contains base classes for convenient IO
 */

#ifndef _IO_HEADER_H_
#define _IO_HEADER_H_

#define IO_PRINTF_SUPPORT_FLOAT /** comment to avoid float */


#include <functional>
#include <string>
#include <stdarg.h>


/* Output function: write one byte per call. returns <0 on error */
typedef std::function<int (int)> FuncWrite;

/* Input function write one byte (can be blocking), returns <0 on error */
typedef std::function<int ()> FuncRead;


class Input {
public:
	Input(FuncRead fread);
	virtual ~Input() {}
	
	virtual int readByte() { return m_fread(); }
	int readByteBlocking();
	
	//TODO: non-blocking sscanf

protected:
	FuncRead m_fread;
};

/* input that reads from a buffered string */
class InputBuffered : public Input {
public:
	InputBuffered();
	virtual ~InputBuffered() {}
	
	virtual int readByte();
	

	/* set source data to read from */
	void setData(const std::string& data);
	void appendData(const std::string& data);

	void reset() { m_next_char = 0; }
private:
	std::string m_buffer;
	size_t m_next_char;
};


class Output {
public:
	Output(FuncWrite funcwrite);
	
	int writeByte(int c) { return m_fwrite(c); }
	int writeString(const std::string& str);
	int printf(const char *format, ...);

protected:
	int vfprintf(const char *format, va_list ap);

	inline void writeBuf(char* buffer, int len, int min_len, char padding);
	inline void writeDecimal(unsigned int i, char* buffer, int min_len, char padding);
	inline void writeHumanReadable(unsigned int ui, char* buffer, int min_len, char padding);
	inline void writeFloat(double n, char* buffer, int min_len, int decimal_places, char padding);
	
	FuncWrite m_fwrite;
};


class InputOutput : public Input, public Output {
public:
	InputOutput(FuncRead fread, FuncWrite fwrite);
	virtual ~InputOutput() {}
private:
};

#endif /* _IO_HEADER_H_ */


