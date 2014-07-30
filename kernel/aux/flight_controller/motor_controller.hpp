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
	 * @param thrust_min min output thrust in [0,1]
	 * @param thrust_max max output thrust in [0,1], 1 means full PWM duty cycle
	 */
	MotorControllerBase(float thrust_min, float thrust_max);
	
	/**
	 * set speed of one motor
	 * @param motor which motor [0, num_motors-1]
	 * @param speed in [0,1]
	 */
	virtual void setMotorSpeed(int motor, float speed) =0;
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
	
	
	/**
	 * number of motors
	 */
	virtual int numMotors() { return 4; }
protected:
	float m_thrust_min;
	float m_thrust_max;
};

/**
 * base class for a PWM motor controller
 */
class MotorControllerPWMBase : public MotorControllerBase {
public:
	
	/**
	 * @param thrust_min min output thrust in [0,1]
	 * @param thrust_max max output thrust in [0,1], 1 means full PWM duty cycle
	 */
	MotorControllerPWMBase(float thrust_min, float thrust_max);

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
	MotorControllerAdafruitPWM(float thrust_min, float thrust_max, std::array<int, 4> channels,
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



#endif /* _FLIGHT_CONTROLLER_MOTOR_CONTROLLER_HEADER_HPP_ */


