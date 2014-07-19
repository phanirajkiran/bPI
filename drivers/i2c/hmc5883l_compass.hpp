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

#ifndef _HMC5883L_COMPASS_HEADER_H_
#define _HMC5883L_COMPASS_HEADER_H_

#include <algorithm>

#include <kernel/i2c.hpp>
#include <kernel/registers.h>
#include <3rdparty/utils/i2cdevlib/HMC5883L/HMC5883L.h>

/** driver for the HMC5883L compass Board.
 * It uses a fixed I2C read/write interface provided by the I2Cdev class
 * 
 * example usage:
 	I2CHMC5883L mag;
	int16_t mx, my, mz;
	mag.initialize();
	mag.getHeading(&mx, &my, &mz);
	printk_i("mag: %i, %i, %i\n", (int)mx, (int)my, (int)mz);
	// To calculate heading in degrees. 0 degree indicates North
	float heading = atan2(my, mx);
	if(heading < 0)
	  heading += 2 * M_PI;
	printk_i(" heading: %i degrees\n", (int)(heading * 180/M_PI));
 */
class I2CHMC5883L : public HMC5883L {
public:
	
	I2CHMC5883L(int addr=0b0011110);

private:
};


#endif /* _HMC5883L_COMPASS_HEADER_H_ */


