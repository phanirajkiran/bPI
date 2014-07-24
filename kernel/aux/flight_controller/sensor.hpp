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

#ifndef _FLIGHT_CONTROLLER_SENSOR_HEADER_HPP_
#define _FLIGHT_CONTROLLER_SENSOR_HEADER_HPP_

#include <kernel/utils.h>
#include <kernel/aux/vec3.hpp>

#include <drivers/i2c/bmp180_barometer.hpp>
#include <drivers/i2c/hmc5883l_compass.hpp>
#include <drivers/i2c/mpu-6050_accel_gyro.hpp>


/** base class for a sensor: it returns either a vector with 3 values or a single
 * value of type T.
 * The sensor must operate in a continous mode or reading a measurement should
 * trigger the next measurement.
 */
template<typename T=float>
class SensorBase {
public:
	
	/** get sensor data */
	virtual void getMeasurement(T& val) = 0;
	/**
	 * get a 3D sensor value.
	 * coordinate system: RHS
	 *   - x == roll (points towards front)
	 *   - y == pitch (points towards right (starboard))
	 *   - z == yaw (points towards down)
	 * @param val measurement output value
	 */
	virtual void getMeasurement(Math::Vec3<T>& val) = 0;
	
	/** get the minimum delay between two getMeasurement calls in microseconds */
	virtual uint minMeasurementDelayMicro() { return 0; }
	
	int minMeasurementDelayMilli() { return minMeasurementDelayMicro()/1000; }
protected:
};


/* wrappers for different sensors */

template<typename T=float>
class SensorBMP180Baro : public SensorBase<T>, public I2CBMP085 {
public:
	//altitude in meters
	virtual void getMeasurement(T& val);
	virtual void getMeasurement(Math::Vec3<T>& val) { ASSERT(false); }
	virtual uint minMeasurementDelayMicro();
};

template<typename T=float>
class SensorHMC5883LCompass : public SensorBase<T>, public I2CHMC5883L {
public:
	virtual void getMeasurement(T& val) { ASSERT(false); }
	//direction of north pole in [gauss]
	virtual void getMeasurement(Math::Vec3<T>& val);
	virtual uint minMeasurementDelayMicro();
};


template<bool use_accel, typename T=float>
class SensorMPU6050GyroAccel : public SensorBase<T> {
public:
	SensorMPU6050GyroAccel(I2CMPU6050& sensor);

	virtual void getMeasurement(T& val) { ASSERT(false); }
	//either acceleration in [m/s^2] or gyroscope in [rad/s]
	virtual void getMeasurement(Math::Vec3<T>& val);
	virtual uint minMeasurementDelayMicro();
private:
	I2CMPU6050& m_sensor;
};


template<typename T>
inline void SensorBMP180Baro<T>::getMeasurement(T& val) {
	// read calibrated pressure value in Pascals (Pa)
	float pressure = getPressure();
	// calculate absolute altitude in meters based on known pressure
	// (may pass a second "sea level pressure" parameter here,
	// otherwise uses the standard value of 101325 Pa)
	val = getAltitude(pressure);

	setControl(BMP085_MODE_PRESSURE_3); //setup next read
}

template<typename T>
inline uint SensorBMP180Baro<T>::minMeasurementDelayMicro() {
	return getMeasureDelayMicroseconds();
}

template<typename T>
inline void SensorHMC5883LCompass<T>::getMeasurement(Math::Vec3<T>& val) {
	int16_t mx, my, mz;
	getHeading(&mx, &my, &mz);
	//TODO: conversion, -4096 check, ... two's complement?
	
	//TODO: coordinate system
}

template<typename T>
inline uint SensorHMC5883LCompass<T>::minMeasurementDelayMicro() {
	return 13334/2; //75 Hz (times 2 because the reading can be out of sync)
}

template<bool use_accel, typename T>
inline SensorMPU6050GyroAccel<use_accel, T>::SensorMPU6050GyroAccel(I2CMPU6050& sensor)
	: m_sensor(sensor) {
}

template<bool use_accel, typename T>
inline void SensorMPU6050GyroAccel<use_accel, T>::getMeasurement(Math::Vec3<T>& val) {
	if(use_accel) {
		int16_t ax, ay, az;
		m_sensor.getAcceleration(&ax, &ay, &az);
	} else {
		int16_t gx, gy, gz;
		m_sensor.getRotation(&gx, &gy, &gz);
	}
	//TODO: convert & coord system....
	
}

template<bool use_accel, typename T>
inline uint SensorMPU6050GyroAccel<use_accel, T>::minMeasurementDelayMicro() {
	return 1000; //1 kHz
}

#endif /* _FLIGHT_CONTROLLER_SENSOR_HEADER_HPP_ */


