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

#ifndef _BMP180_BAROMETER_HEADER_H_
#define _BMP180_BAROMETER_HEADER_H_

#include <algorithm>

#include <kernel/i2c.hpp>
#include <kernel/registers.h>
#include <3rdparty/utils/i2cdevlib/BMP085/BMP085.h>

/** driver for the BMP180/BMP058 barometer Board.
 * It uses a fixed I2C read/write interface provided by the I2Cdev class
 * 
 * example usage:
 	BMP085 barometer;
	float temperature;
	float pressure;
	float altitude;
	int32_t lastMicros;
	barometer.initialize();

	if(!barometer.testConnection()) {
		printk("Error: no connection to BMP058 barometer!\n");
	} else {
		barometer.setControl(BMP085_MODE_TEMPERATURE);
		// wait appropriate time for conversion (4.5ms delay)
		lastMicros = getTimestamp();
		while (getTimestamp() - lastMicros < barometer.getMeasureDelayMicroseconds());
		// read calibrated temperature value in degrees Celsius
		temperature = barometer.getTemperatureC();
		// request pressure (3x oversampling mode, high detail, 23.5ms delay)
		barometer.setControl(BMP085_MODE_PRESSURE_3);
		while (getTimestamp() - lastMicros < barometer.getMeasureDelayMicroseconds());

		// read calibrated pressure value in Pascals (Pa)
		pressure = barometer.getPressure();

		// calculate absolute altitude in meters based on known pressure
		// (may pass a second "sea level pressure" parameter here,
		// otherwise uses the standard value of 101325 Pa)
		altitude = barometer.getAltitude(pressure);

		printk("T: %i, P: %i, Alt: %i\n", (int)temperature, (int)pressure, (int)altitude);
	}
 */
class I2CBMP085 : public BMP085 {
public:

	I2CBMP085(int addr=0b1110111);

private:
};


#endif /* _BMP180_BAROMETER_HEADER_H_ */


