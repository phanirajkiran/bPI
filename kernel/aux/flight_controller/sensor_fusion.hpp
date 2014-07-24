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
};

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
	/**
	 * calculate x^(-1/2)
	 */
	inline float invSqrt(float x);

	float twoKp, twoKi;
	float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;					// quaternion of sensor frame relative to auxiliary frame
	float integralFBx = 0.0f,  integralFBy = 0.0f, integralFBz = 0.0f;	// integral error terms scaled by Ki
};

#endif /* _FLIGHT_CONTROLLER_SENSOR_FUSION_HEADER_HPP_ */


