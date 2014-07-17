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

#include "i2c.hpp"

I2C::I2C(FuncI2CWrite func_write, FuncI2CRead func_read, int addr)
	: m_func_write(func_write), m_func_read(func_read), m_addr(addr) {
}

