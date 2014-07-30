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

#ifndef _FLIGHT_CONTROLLER_MOTOR_COMMAND_HEADER_HPP_
#define _FLIGHT_CONTROLLER_MOTOR_COMMAND_HEADER_HPP_

#include <kernel/aux/command_line.hpp>
#include "motor_controller.hpp"

class CommandControlMotor : public CommandBase {
public:
	CommandControlMotor(CommandLine& command_line,
			MotorControllerPWMBase& motor_controller);
	
	~CommandControlMotor();

	virtual void startExecute(const std::vector<std::string>& arguments);
	
	virtual int handleData();
private:
	void refreshOutput(bool clear_first);
	void changeSelectedMotorsSpeed(float amount);
	void changePWMFrequency(int amount);

	MotorControllerPWMBase& m_motor_controller;
	bool* m_selected_motors;
};

#endif /* _FLIGHT_CONTROLLER_MOTOR_COMMAND_HEADER_HPP_ */


