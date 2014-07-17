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
 * I2C interface
 */

#ifndef I2C_HEADER_H_
#define I2C_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <kernel/types.h>
#include <kernel/errors.h>
#include <kernel/utils.h>
#include <i2c_arch.h>


void initI2C();

/** read i2c data from a slave
 * @param addr 7bit (w/o RW bit)
 * @param buf output buffer (length >= len)
 * @param len length of data to read
 * @return number of bytes read
 */
int i2cRead(int addr, char* buf, int len);

/** write i2c data from a slave
 * @param addr 7bit (w/o RW bit)
 * @param buf data to write (length >= len)
 * @param len length of data to write
 * @return number of bytes written
 */
int i2cWrite(int addr, char* buf, int len);

#ifndef ARCH_HAS_I2C

#define initI2C() NOP

#define i2cRead(addr, buf, len) (0)
#define i2cWrite(addr, buf, len) (0)

#endif /* ARCH_HAS_I2C */



#ifdef __cplusplus
}
#endif
#endif /* I2C_HEADER_H_ */

