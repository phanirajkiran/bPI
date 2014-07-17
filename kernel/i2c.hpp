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

#ifndef _I2C_CPP_HEADER_H_
#define _I2C_CPP_HEADER_H_

#include <kernel/i2c.h>

#include <functional>


/** I2C IO functions: return the length of written/read bytes */
typedef std::function<int (int addr, char* buffer, int len)> FuncI2CWrite;
typedef std::function<int (int addr, char* buffer, int len)> FuncI2CRead;


/** high-level I2C class */
class I2C {
public:
	I2C(FuncI2CWrite func_write, FuncI2CRead func_read, int addr=0);
	
	int address() const { return m_addr; }
	void setAddr(int addr) { m_addr = addr; }

	int read(char* buffer, int len) {
		return m_func_read(m_addr, buffer, len);
	}
	int write(char* buffer, int len) {
		return m_func_write(m_addr, buffer, len);
	}
	
	/** i2c_smbus_read_byte_data function: read 1 byte from a given register */
	inline int readRegister(uchar reg, uchar& out_data);

	inline int writeRegister(uchar reg, uchar data);
private:
	FuncI2CWrite m_func_write;
	FuncI2CRead m_func_read;
	int m_addr; //7/10 bit address, w/o RW bit
};

int I2C::readRegister(uchar reg, uchar& out_data) {
	if(write((char*)&reg, 1) != 1) return 0;
	return read((char*)&out_data, 1);
}

int I2C::writeRegister(uchar reg, uchar data) {
	char buffer[2];
	buffer[0] = (char)reg;
	buffer[1] = (char) data;
	return write(buffer, 2);
}
#endif /* _I2C_CPP_HEADER_H_ */


