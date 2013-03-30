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

/*! this file defines functions to directly read/write hardware addresses for
 * device IO
 */

#ifndef REGISTERS_HEADER_H_
#define REGISTERS_HEADER_H_

#include "types.h"


/* read a register value */
#define regRead8(addr) (*((volatile int8*)addr))
#define regRead16(addr) (*((volatile int16*)addr))
#define regRead32(addr) (*((volatile int32*)addr))

/* read a single bit from a register: 
 * bit: 0...7/15/31
 * return value is either 0 if bit not set or !=0 if bit set
 */
#define regRead8Bit(addr, bit) (regRead8(addr) & (1<<bit))
#define regRead16Bit(addr, bit) (regRead16(addr) & (1<<bit))
#define regRead32Bit(addr, bit) (regRead32(addr) & (1<<bit))



/* write a register value */
#define regWrite8(addr, value) (*((volatile int8*)addr)) = (int8)value
#define regWrite16(addr, value) (*((volatile int16*)addr)) = (int16)value
#define regWrite32(addr, value) (*((volatile int32*)addr)) = (int32)value


/* read modify write a register bit.
 * bit: which bit to set/clear
 * value: 0 to clear bit or 1 to set bit
 */
#define regRMW8(addr, bit, value) \
	regWrite8(addr, (regRead8(addr) & ~(1 << bit)) | (value << bit))
#define regRMW16(addr, bit, value) \
	regWrite16(addr, (regRead16(addr) & ~(1 << bit)) | (value << bit))
#define regRMW32(addr, bit, value) \
	regWrite32(addr, (regRead32(addr) & ~(1 << bit)) | (value << bit))


#endif /* REGISTERS_HEADER_H_ */


