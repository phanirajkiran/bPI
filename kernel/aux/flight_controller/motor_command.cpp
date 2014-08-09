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

#include "motor_command.hpp"

CommandControlMotor::CommandControlMotor(CommandLine& command_line,
		MotorControllerPWMBase& motor_controller)
	: CommandBase("motors", 
	  "Adjust motor speeds\n"
	  "Keys: 'q' to quit, '1'-'4' to select motors, ' ' reset all speeds to 0\n"
	  "'j'/'k' decrease/increase speed by 0.01, 'n'/'m' change by 0.001\n"
	  "'u'/'i' decrease/increase PWM frequency by 25\n"
	  "'x' set selected motor speeds to 1ms, 'c' set sel motors to min thrust",
	  command_line),
	  m_motor_controller(motor_controller) {

	m_selected_motors = new bool[m_motor_controller.numMotors()];
	for(int i=0; i<m_motor_controller.numMotors(); ++i)
		m_selected_motors[i] = false;
}

CommandControlMotor::~CommandControlMotor() {
	delete[] m_selected_motors;
}


void CommandControlMotor::startExecute(const std::vector<std::string>& arguments) {
	refreshOutput(false);
}

int CommandControlMotor::handleData() {
	int c = m_command_line.inputOutput().readByte();
	if(c == 'q') { //quit
		finishExecute();
		return 0;
	}
	if(c < 0 && c != -E_WOULD_BLOCK) return c;
	
	bool refresh_output = true;
	switch(c) {
	case ' ':
		for(int i=0; i<m_motor_controller.numMotors(); ++i)
			m_motor_controller.setMotorSpeed(i, 0.f);
		break;
	case 'j':
		changeSelectedMotorsSpeed(-0.01f);
		break;
	case 'k':
		changeSelectedMotorsSpeed(0.01f);
		break;
	case 'n':
		changeSelectedMotorsSpeed(-0.001f);
		break;
	case 'm':
		changeSelectedMotorsSpeed(0.001f);
		break;
	case 'u':
		changePWMFrequency(-25);
		break;
	case 'i':
		changePWMFrequency(25);
		break;

	case 'x':
		setSelectedMotorSpeed(1.f);
		break;
	case 'c':
		setSelectedMotorSpeedMin();
		break;
	default:
		refresh_output = false;
		break;
	}
	int idx = c-'1';
	if(idx >= 0 && idx < m_motor_controller.numMotors()) {
		m_selected_motors[idx] = !m_selected_motors[idx];
		refresh_output = true;
	} else if(idx == m_motor_controller.numMotors()) {
		for(int i=0; i<m_motor_controller.numMotors(); ++i)
			m_selected_motors[i] = !m_selected_motors[i];
		refresh_output = true;
	}
	if(refresh_output) refreshOutput(true);
	
	return 0;
}

void CommandControlMotor::refreshOutput(bool clear_first) {
	if(clear_first)
		m_command_line.eraseLines(2+m_motor_controller.numMotors());

	/* selected motors */
	InputOutput& io = m_command_line.inputOutput();
	io.printf("Selected Motors:");
	bool had_motor = false;
	for(int i=0; i<m_motor_controller.numMotors(); ++i) {
		if(m_selected_motors[i]) {
			io.printf(" %i", i);
			had_motor = true;
		}
	}
	if(!had_motor) io.printf(" None (use keys '1'-'%i')", m_motor_controller.numMotors());
	
	int pwm_frequency = m_motor_controller.getPWMFreq();

	io.printf("\nPWM Frequency: %i Hz\n", pwm_frequency);

	for(int i=0; i<m_motor_controller.numMotors(); ++i) {
		float speed = m_motor_controller.getMotorSpeed(i);
		io.printf("Motor %i speed: %.4f (%.3f ms)\n", i, speed,
				speed/(float)pwm_frequency*1000.f);
	}
}

void CommandControlMotor::changeSelectedMotorsSpeed(float amount) {
	for(int i=0; i<m_motor_controller.numMotors(); ++i) {
		if(m_selected_motors[i]) {
			float new_speed = m_motor_controller.getMotorSpeed(i)+amount;
			if(new_speed < 0.f) new_speed = 0.f;
			else if(new_speed > 1.f) new_speed = 1.f;
			m_motor_controller.setMotorSpeed(i, new_speed);
		}
	}
}
void CommandControlMotor::setSelectedMotorSpeed(float pulse_ms) {
	float pwm_frequency = m_motor_controller.getPWMFreq();
	float val = pulse_ms * pwm_frequency / 1000.f;
	if(val < 0.f) val = 0.f;
	if(val > 1.f) val = 1.f;
	for(int i=0; i<m_motor_controller.numMotors(); ++i) {
		if(m_selected_motors[i]) {
			m_motor_controller.setMotorSpeed(i, val);
		}
	}
}
void CommandControlMotor::setSelectedMotorSpeedMin() {
	for(int i=0; i<m_motor_controller.numMotors(); ++i) {
		if(m_selected_motors[i]) {
			m_motor_controller.setMotorSpeed(i, m_motor_controller.minThrust(i));
		}
	}
}


void CommandControlMotor::changePWMFrequency(int amount) {
	int new_freq = m_motor_controller.getPWMFreq() + amount;
	if(new_freq > 400) new_freq = 400;
	if(new_freq < 50) new_freq = 50;
	m_motor_controller.setPWMFreq(new_freq);
}
