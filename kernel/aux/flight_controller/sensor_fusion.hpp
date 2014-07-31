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

#ifndef _FLIGHT_CONTROLLER_SENSOR_FUSION_HEADER_HPP_
#define _FLIGHT_CONTROLLER_SENSOR_FUSION_HEADER_HPP_

#include <kernel/aux/vec3.hpp>
#include <kernel/utils.h>

/**
 * sensor fusion: calculate attitude from an IMU/MARG sensor. currently only
 * 9 dof (mag+gyro+accel) is supported.
 */
class SensorFusionBase {
public:
	
	/**
	 * update attitude from new sensor values.
	 * @param gyro gyroscope sensor data in [rad/s]
	 * @param accel acceleration sensor data (unit does not matter)
	 * @param mag compass sensor data (unit does not matter)
	 * @param dt timestep in [ms]
	 * @param attitude returned attitude: roll, pitch & yaw [rad]
	 * 	               roll & yaw are in [-pi,pi], pitch is in [-pi/2,pi/2]
	 */
	virtual void update(const Math::Vec3f& gyro, const Math::Vec3f& accel,
			const Math::Vec3f& mag, float dt, Math::Vec3f& attitude)=0;
protected:
	static inline void quaternionToRollPitchYaw(float q0, float q1, float q2, float q3,
			float& roll, float& pitch, float& yaw);
	
	/**
	 * calculate x^(-1/2)
	 */
	static inline float invSqrt(float x);
};

void SensorFusionBase::quaternionToRollPitchYaw(float q0, float q1, float q2, float q3,
			float& roll, float& pitch, float& yaw) {
	/*
	//euler angle sequence: 3,2,1 (see [diebel2006])
	roll =  atan2(-2.f*q1*q2 + 2.f*q0*q3, q1*q1+q0*q0-q3*q3-q2*q2);
	pitch = asin(2.f*(q1*q3+q0*q2));
	yaw =   atan2(-2.f*q2*q3+2.f*q0*q1, q3*q3-q2*q2-q1*q1+q0*q0);
	//*/
	
	//euler angle sequence 1,2,3 (see [diebel2006])
	roll = atan2(2.f*q2*q3 + 2.f*q0*q1, q3*q3-q2*q2-q1*q1+q0*q0);
	pitch = -asin(2.f*q1*q3-2.f*q0*q2);
	yaw = atan2(2.f*q1*q2 + 2.f*q0*q3, q1*q1 + q0*q0 - q3*q3 - q2*q2);
	//pitch & yaw seem to be inverted...
	pitch = -pitch;
	yaw = -yaw;
	
}

// Fast inverse square-root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
float SensorFusionBase::invSqrt(float x) {
	typedef int32 CAN_ALIAS long_alias;
	typedef float CAN_ALIAS float_alias;

	float halfx = 0.5f * x;
	float_alias y = x;
	long_alias i = *(long_alias*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float_alias*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}



/**
 * sensor fusion class based on Mahony's AHRS quaternion algorithm
 */
class SensorFusionMahonyAHRS : public SensorFusionBase {
public:
	/**
	 * @param Kp p coefficient for PI controller
	 * @param Ki i coefficient for PI controller
	 */
	SensorFusionMahonyAHRS(float Kp=0.5f, float Ki=0.f);

	virtual void update(const Math::Vec3f& gyro, const Math::Vec3f& accel,
			const Math::Vec3f& mag, float dt, Math::Vec3f& attitude);
private:
	inline void MahonyAHRSupdate(float gx, float gy, float gz,
			float ax, float ay, float az, float mx, float my, float mz, float dt);
	inline void MahonyAHRSupdateIMU(float gx, float gy, float gz,
			float ax, float ay, float az, float dt);

	float twoKp, twoKi;
	float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;					// quaternion of sensor frame relative to auxiliary frame
	float integralFBx = 0.0f,  integralFBy = 0.0f, integralFBz = 0.0f;	// integral error terms scaled by Ki
};


/**
 * sensor fusion class based on Madgwick's AHRS algorithm
 * 
 * FIXME: this method seems not to work AT ALL (-> wrong coordinate system input?)
 */
class SensorFusionMadgwickAHRS : public SensorFusionBase {
public:
	SensorFusionMadgwickAHRS(float beta=0.1f);

	virtual void update(const Math::Vec3f& gyro, const Math::Vec3f& accel,
			const Math::Vec3f& mag, float dt, Math::Vec3f& attitude);
private:
	inline void MadgwickAHRSupdate(float gx, float gy, float gz,
			float ax, float ay, float az, float mx, float my, float mz, float dt);
	inline void MadgwickAHRSupdateIMU(float gx, float gy, float gz,
			float ax, float ay, float az, float dt);

	float beta;
	float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;					// quaternion of sensor frame relative to auxiliary frame
};

#endif /* _FLIGHT_CONTROLLER_SENSOR_FUSION_HEADER_HPP_ */


