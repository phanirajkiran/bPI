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


MotorControllerBase::~MotorControllerBase() {
	delete[] m_min_thrust;
	delete[] m_max_thrust;
}

void MotorControllerBase::setMotorSpeedMin() {
	int num_motors = numMotors();
	for(int i=0; i<num_motors; ++i)
		setMotorSpeed(i, m_min_thrust[i]);
}



MotorControllerAdafruitPWM::MotorControllerAdafruitPWM(
		std::array<float, 4> min_thrust, std::array<float, 4> max_thrust,
		std::array<int, 4> channels, FuncI2CWrite func_write,
		FuncI2CRead func_read, int addr)
	: MotorControllerPWMBase(min_thrust, max_thrust),
	  I2CAdafruitPWM(func_write, func_read, addr), m_channels(channels) {
}

void MotorControllerAdafruitPWM::setThrust(float throttle,
		const Math::Vec3f& roll_pitch_yaw) {
	
	/* convert to X configuration */
	const float& roll = roll_pitch_yaw.x;
	const float& pitch = roll_pitch_yaw.y;
	const float& yaw = roll_pitch_yaw.z;
	std::array<float, 4> thrusts{{
		throttle + yaw + (+pitch -roll) * 0.5f, /* motor 0 */
		throttle - yaw + (-pitch -roll) * 0.5f, /* motor 1 */
		throttle + yaw + (-pitch +roll) * 0.5f, /* motor 2 */
		throttle - yaw + (+pitch +roll) * 0.5f, /* motor 3 */
	}};
	
	//scale to range
	for(int i=0; i<4; ++i)
		thrusts[i] = m_min_thrust[i] + (thrusts[i] + 1.f) * (m_max_thrust[i]-m_min_thrust[i])/2.f;
	
	//check max:
	float adjust = 0.f;
	for(int i=0; i<4; ++i) {
		if(thrusts[i]-adjust > m_max_thrust[i])
			adjust = thrusts[i] - m_max_thrust[i];
	}
	if(adjust > 0.f) {
		//we must not exceed the maximum, so remove thrust equally
		for(int i=0; i<4; ++i) thrusts[i] -= adjust;
	}
	//same for min:
	adjust = 0.f;
	for(int i=0; i<4; ++i) {
		if(thrusts[i]+adjust < m_min_thrust[i])
			adjust = m_min_thrust[i] - thrusts[i];
	}
	if(adjust > 0.f) {
		//we must not exceed the maximum, so add thrust equally
		for(int i=0; i<4; ++i) {
			thrusts[i] += adjust;
			if(thrusts[i] > m_max_thrust[i]) {
				//unable to satisfy both constraints...
				printk_w("Max thrust exceeded for motor %i: %.3f, max=%.3f\n",
						i, thrusts[i], m_max_thrust[i]);
				thrusts[i] = m_max_thrust[i];
			}
		}
	}

	//apply
	for(size_t i=0; i<thrusts.size(); ++i)
		setMotorSpeed(i, thrusts[i]);
}

void MotorControllerAdafruitPWM::setMotorSpeed(int motor, float speed) {
	setPWM(m_channels[motor], (uint16)(speed*4096.f));
}

float MotorControllerAdafruitPWM::getMotorSpeed(int motor) {
	return (float)getPWMDuty(m_channels[motor])/4096.f;
}
