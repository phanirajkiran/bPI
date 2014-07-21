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

#include "motor_controller.hpp"

#include <kernel/printk.h>


MotorControllerBase::MotorControllerBase(float thrust_min, float thrust_max)
	: m_thrust_min(thrust_min), m_thrust_max(thrust_max) {
}


MotorControllerAdafruitPWM::MotorControllerAdafruitPWM(float thrust_min,
		float thrust_max, std::array<int, 4> channels, FuncI2CWrite func_write,
		FuncI2CRead func_read, int addr)
	: MotorControllerBase(thrust_min, thrust_max),
	  I2CAdafruitPWM(func_write, func_read, addr), m_channels(channels) {
}

void MotorControllerAdafruitPWM::setThrust(float throttle,
		const Math::Vec3f& roll_pitch_yaw) {
	
	//TODO: add thrust offset value??

	/* convert to X configuration */
	const float& roll = roll_pitch_yaw.x;
	const float& pitch = roll_pitch_yaw.y;
	const float& yaw = roll_pitch_yaw.z;
	std::array<float, 4> thrusts{{
		throttle - yaw + (+pitch -roll) * 0.5f, /* motor 0 */
		throttle + yaw + (-pitch -roll) * 0.5f, /* motor 1 */
		throttle - yaw + (-pitch +roll) * 0.5f, /* motor 2 */
		throttle + yaw + (+pitch +roll) * 0.5f, /* motor 3 */
	}};
	
	//scale to range
	for(int i=0; i<4; ++i)
		thrusts[i] = m_thrust_min + (thrusts[i] + 1.f) * (m_thrust_max-m_thrust_min)/2.f;
	
	//check max:
	float max_val = m_thrust_min;
	for(int i=0; i<4; ++i)
		if(thrusts[i] > max_val) max_val = thrusts[i];
	if(max_val > m_thrust_max) {
		//we must not exceed the maximum, so remove thrust equally
		for(int i=0; i<4; ++i) thrusts[i] -= max_val - m_thrust_max;
	}
	//same for min:
	float min_val = m_thrust_max;
	for(int i=0; i<4; ++i)
		if(thrusts[i] < min_val) min_val = thrusts[i];
	if(min_val < m_thrust_min) {
		//we must not exceed the minimum, so add thrust equally
		for(int i=0; i<4; ++i) {
			thrusts[i] += m_thrust_min - min_val;
			if(thrusts[i] > m_thrust_max) {
				printk_w("Max thrust exceeded for motor %i: %.3f, max=%.3f\n",
						i, thrusts[i], m_thrust_max);
				thrusts[i] = m_thrust_max;
			}
		}
	}

	//apply
	for(int i=0; i<4; ++i)
		setPWM(m_channels[i], (uint16)(thrusts[i]*4096.f));
	
}
