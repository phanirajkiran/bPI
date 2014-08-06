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

#ifndef _FLIGHT_CONTROLLER_MOTOR_CONTROLLER_HEADER_HPP_
#define _FLIGHT_CONTROLLER_MOTOR_CONTROLLER_HEADER_HPP_

#include <kernel/aux/vec3.hpp>
#include <drivers/i2c/adafruit_pwm.hpp>

#include <array>


/**
 * base class for a moter controller with 4 inputs: throttle, yaw, pitch & roll.
 * the throttle value is scaled & offset (it must be in [-1,1]) and the other
 * values need to be need to be tuned using the PID's in the flight controller.
 */
class MotorControllerBase {
public:
	/**
	 * @param min_thrust min output thrust in [0,1] (one per motor)
	 * @param max_thrust max output thrust in [0,1], 1 means full PWM duty cycle
	 */
	template<std::size_t num_motors>
	MotorControllerBase(const std::array<float, num_motors>& min_thrust,
			const std::array<float, num_motors>& max_thrust);
	virtual ~MotorControllerBase();
	
	/**
	 * set speed of one motor
	 * @param motor which motor [0, num_motors-1]
	 * @param speed in [minThrust(), maxThrust()]
	 */
	virtual void setMotorSpeed(int motor, float speed) =0;
	
	/**
	 * set motor speed for all motors to minimum
	 */
	void setMotorSpeedMin();

	/**
	 * get motor speed
	 * @param motor
	 * @return speed in [0,1]
	 */
	virtual float getMotorSpeed(int motor) =0;

	/**
	 * @param throttle assumed to be in range [-1,1]
	 * @param roll_pitch_yaw should also be in range [-1,1]
	 */
	virtual void setThrust(float throttle, const Math::Vec3f& roll_pitch_yaw)=0;
	
	
	float minThrust(int motor) const { return m_min_thrust[motor]; }
	float maxThrust(int motor) const { return m_max_thrust[motor]; }
	
	/**
	 * number of motors
	 */
	int numMotors() { return m_num_motors; }
protected:
	float* m_min_thrust;
	float* m_max_thrust;
	int m_num_motors;
};

/**
 * base class for a PWM motor controller
 */
class MotorControllerPWMBase : public MotorControllerBase {
public:
	
	/**
	 * @param thrust_min min output thrust in [0,1] (one per motor)
	 * @param thrust_max max output thrust in [0,1], 1 means full PWM duty cycle
	 */
	template<std::size_t num_motors>
	MotorControllerPWMBase(const std::array<float, num_motors>& min_thrust,
			const std::array<float, num_motors>& max_thrust);
	virtual ~MotorControllerPWMBase() {}

	/**
	 * set the PWM frequency for all channels
	 * @param freq Frequency in Hz [50, 500]
	 * @return true on success
	 */
	virtual bool setPWMFreq(int freq) =0;

	virtual int getPWMFreq() =0;
private:
};

/**
 * motor controller that uses the adafruit 16 channel PWM board via I2C.
 * motor association:
 *   X configuration:
 * 
 *         roll, front
 *   --->    ^    <---
 *   |M3|    |    |M0|
 *   <--- \  |  / --->
 *          \|/
 *  ---------*--------- pitch
 *          /|\
 *   <--- /  |  \ --->
 *   |M2|    |    |M1|
 *   --->    ^    <---
 */

class MotorControllerAdafruitPWM : public MotorControllerPWMBase, public I2CAdafruitPWM {
public:
	/**
	 * constructor: this will not setup & set PWM frequency
	 */
	MotorControllerAdafruitPWM(std::array<float, 4> min_thrust,
		std::array<float, 4> max_thrust, std::array<int, 4> channels,
		FuncI2CWrite func_write, FuncI2CRead func_read, int addr=0b1000000);

	virtual void setThrust(float throttle, const Math::Vec3f& roll_pitch_yaw);
	
	
	virtual void setMotorSpeed(int motor, float speed);
	virtual float getMotorSpeed(int motor);


	virtual bool setPWMFreq(int freq) {
		return I2CAdafruitPWM::setPWMFreq(freq);
	}

	virtual int getPWMFreq() {
		return I2CAdafruitPWM::getPWMFreq();
	}
private:
	std::array<int, 4> m_channels; //PWM channel <--> motor association
};

template<std::size_t num_motors>
inline MotorControllerBase::MotorControllerBase(
		const std::array<float, num_motors>& min_thrust,
		const std::array<float, num_motors>& max_thrust)
	: m_num_motors(num_motors) {

	m_min_thrust = new float[num_motors];
	m_max_thrust = new float[num_motors];
	for(std::size_t i=0; i<num_motors; ++i) {
		m_min_thrust[i] = min_thrust[i];
		m_max_thrust[i] = max_thrust[i];
	}
}

template<std::size_t num_motors>
inline MotorControllerPWMBase::MotorControllerPWMBase(
		const std::array<float, num_motors>& min_thrust,
		const std::array<float, num_motors>& max_thrust)
	: MotorControllerBase(min_thrust, max_thrust) {
}

#endif /* _FLIGHT_CONTROLLER_MOTOR_CONTROLLER_HEADER_HPP_ */


