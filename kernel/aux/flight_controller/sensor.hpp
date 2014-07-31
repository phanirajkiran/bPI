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
#include <kernel/math.h>
#include <kernel/aux/vec3.hpp>

#include <drivers/i2c/bmp180_barometer.hpp>
#include <drivers/i2c/hmc5883l_compass.hpp>
#include <drivers/i2c/mpu-6050_accel_gyro.hpp>

// acceleration due to gravity in m/s/s
#define GRAVITY_MSS 9.80665f


/** base class for a sensor: it returns either a vector with 3 values or a single
 * value of type T.
 * The sensor must operate in a continous mode or reading a measurement should
 * trigger the next measurement.
 */
template<typename T=float>
class SensorBase {
public:
	
	/** get sensor data */
	virtual bool getMeasurement(T& val) { return false; }

	/**
	 * get a 3D sensor value.
	 * coordinate system: RHS
	 *   - x == roll (points towards front)
	 *   - y == pitch (points towards right (starboard))
	 *   - z == yaw (points towards down)
	 * @param val measurement output value
	 * @return true when value was set, false otherwise & val not changed
	 *         (eg. reading problem or output val not supported)
	 */
	virtual bool getMeasurement(Math::Vec3<T>& val) { return false; }


	/** get the minimum delay between two getMeasurement calls in microseconds */
	virtual uint minMeasurementDelayMicro() { return 0; }
	
	int minMeasurementDelayMilli() { return minMeasurementDelayMicro()/1000; }
protected:
};


/* wrappers for different sensors */

template<typename T=float>
class SensorBMP180Baro : public SensorBase<T>, public I2CBMP085 {
public:
	void initialize();
	
	//altitude in meters
	virtual bool getMeasurement(T& val);
	virtual uint minMeasurementDelayMicro();
};

template<typename T=float>
class SensorHMC5883LCompass : public SensorBase<T>, public I2CHMC5883L {
public:
	void initialize();

	//direction of north pole
	virtual bool getMeasurement(Math::Vec3<T>& val);
	virtual uint minMeasurementDelayMicro();
};


template<bool use_accel, typename T=float>
class SensorMPU6050GyroAccel : public SensorBase<T> {
public:
	SensorMPU6050GyroAccel(I2CMPU6050& sensor);

	//either acceleration in [m/s^2] or gyroscope in [rad/s]
	virtual bool getMeasurement(Math::Vec3<T>& val);
	virtual uint minMeasurementDelayMicro();
private:
	I2CMPU6050& m_sensor;
};


template<typename T>
void SensorBMP180Baro<T>::initialize() {
	I2CBMP085::initialize();
	//nothing additional to do
}

template<typename T>
inline bool SensorBMP180Baro<T>::getMeasurement(T& val) {
	// read calibrated pressure value in Pascals (Pa)
	float pressure = getPressure();
	// calculate absolute altitude in meters based on known pressure
	// (may pass a second "sea level pressure" parameter here,
	// otherwise uses the standard value of 101325 Pa)
	val = (T)getAltitude(pressure);

	setControl(BMP085_MODE_PRESSURE_3); //setup next read
	return true;
}

template<typename T>
inline uint SensorBMP180Baro<T>::minMeasurementDelayMicro() {
	return getMeasureDelayMicroseconds();
}

template<typename T>
void SensorHMC5883LCompass<T>::initialize() {
	I2CHMC5883L::initialize();
	setMode(HMC5883L_MODE_SINGLE);
	setSampleAveraging(3); //8 samples averaging
	setDataRate(6); //75Hz
	//setGain(); //TODO: change/dynamically adjust gain??
}
template<typename T>
inline bool SensorHMC5883LCompass<T>::getMeasurement(Math::Vec3<T>& val) {
	int16_t mx, my, mz;
	getHeading(&mx, &my, &mz);
	if(mx == -4096 || my == -4096 || mz == -4096) {
		printk_w("Warning: Compass got invalid/overflow measurement -> gain too high?\n");
		return false;
	}
	
	val.x = (T)my;
	val.y = (T)mx;
	val.z = -(T)mz;
	
	return true;
}

template<typename T>
inline uint SensorHMC5883LCompass<T>::minMeasurementDelayMicro() {
	return 13334/2; //75 Hz (times 2 because the reading can be out of sync)
}

template<bool use_accel, typename T>
inline SensorMPU6050GyroAccel<use_accel, T>::SensorMPU6050GyroAccel(I2CMPU6050& sensor)
	: m_sensor(sensor) {

	delay(5); //make sure device was woken up
	if(use_accel) {
		m_sensor.setFullScaleAccelRange(MPU6050_ACCEL_FS_8); //8g -> affects scaling below!
	} else {
		m_sensor.setDLPFMode(2); //lowpass filter
		m_sensor.setRate(0); //0=1kHz, 1=500Hz, 2=333Hz, ...
			//when changing rate, also change minMeasurementDelayMicro
		m_sensor.setFullScaleGyroRange(MPU6050_GYRO_FS_2000); //2000deg/s -> affects scaling below!
	}
}

template<bool use_accel, typename T>
inline bool SensorMPU6050GyroAccel<use_accel, T>::getMeasurement(Math::Vec3<T>& val) {
	if(use_accel) {
		int16_t ax, ay, az;
		m_sensor.getAcceleration(&ax, &ay, &az);
		const T accel_scale = (GRAVITY_MSS / 4096.0f); //4096 LSB/g for +-8g
		val.x = (T)ay * accel_scale;
		val.y = (T)ax * accel_scale;
		val.z = (T)-az * accel_scale;
	} else {
		int16_t gx, gy, gz;
		m_sensor.getRotation(&gx, &gy, &gz);
		//convert to rad/s
		const T gyro_scale = (T)(M_PI/180. / 16.4); //16.4 LSB/deg/s +-2000deg/s
		val.x = (T)gy * gyro_scale;
		val.y = (T)gx * gyro_scale;
		val.z = (T)-gz * gyro_scale;
	}
	return true;
}

template<bool use_accel, typename T>
inline uint SensorMPU6050GyroAccel<use_accel, T>::minMeasurementDelayMicro() {
	return 1000; //1 kHz
}

#endif /* _FLIGHT_CONTROLLER_SENSOR_HEADER_HPP_ */


