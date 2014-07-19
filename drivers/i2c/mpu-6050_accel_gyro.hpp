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

#ifndef _MPU_6050_ACCEL_GYRO_HEADER_H_
#define _MPU_6050_ACCEL_GYRO_HEADER_H_

#include <algorithm>

#include <kernel/i2c.hpp>
#include <kernel/registers.h>
#include <3rdparty/utils/i2cdevlib/MPU6050/MPU6050.h>

/** 
 * driver for the MPU-6050 accelerometer & gyroscope Board.
 * It uses a fixed I2C read/write interface provided by the I2Cdev class
 * 
 * example usage:
 	I2CMPU6050 accelgyro;
	accelgyro.initialize();
	if(!accelgyro.testConnection())
		printk_e("Error: no connection to MPU6050 Accel/Gyro\n");
	int16_t ax, ay, az;
	int16_t gx, gy, gz;
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
 */
class I2CMPU6050 : public MPU6050 {
public:

	/**
	 * create the object, but does not initialize the device (use initialize()).
	 * address is either 0x68 or 0x69
	 */
	I2CMPU6050(int addr=0b1000000);
	
private:
};


#endif /* _MPU_6050_ACCEL_GYRO_HEADER_H_ */


