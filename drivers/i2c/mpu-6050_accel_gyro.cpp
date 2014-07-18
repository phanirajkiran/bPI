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

#include "mpu-6050_accel_gyro.hpp"
#include <kernel/utils.h>

I2CMPU6050::I2CMPU6050(int addr) 
	: MPU6050((uint8_t)addr) {
}
