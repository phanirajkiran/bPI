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

#include "stabilize_command.hpp"
#include <kernel/errors.h>

#include <array>
using namespace std;
using namespace Math;


CommandStabilize::CommandStabilize(CommandLine& command_line,
		FlightControllerConfig& config, const Math::Vec3f& attitude,
		Math::Vec3f& dest_roll_pitch_yaw,
		const Math::Vec3f& pid_roll_pitch_yaw_output, float* throttle_input)
	: CommandBase("stabilize",
			"Test stabilization\n"
			  "Keys: 'q' to quit, '1'-'3' pitch, roll, yaw stabilization,\n"
			  "' ' reset all speeds to 0 & turn off\n"
			  "'j'/'k' decrease/increase throttle speed by 0.01, 'n'/'m' change by 0.001\n"
			  "'w'/'e', 's'/'d', 'x'/'c' increase/decrease PID coeff P, I, D\n"
			  "'r' toggle use remote throttle input",
			command_line),
	  m_config(config), m_attitude(attitude),
	  m_dest_roll_pitch_yaw(dest_roll_pitch_yaw),
	  m_pid_roll_pitch_yaw_output(pid_roll_pitch_yaw_output),
	  m_throttle_remote(throttle_input) {
}

CommandStabilize::~CommandStabilize() {
}

void CommandStabilize::startExecute(const std::vector<std::string>& arguments) {
	m_stabilize_pitch = m_stabilize_roll = m_stabilize_yaw = false;
	m_use_throttle_remote = false;
	m_next_update = getTimestamp() + update_delay_ms*1000;
	m_last_printed_line_count = 0;
	refreshOutput();
}

int CommandStabilize::handleData() {
	int c = m_command_line.inputOutput().readByte();
	if(c == 'q') { //quit
		m_config.motor_controller->setMotorSpeedMin();
		finishExecute();
		return 0;
	}
	if(c < 0 && c != -E_WOULD_BLOCK) return c;
	
	constexpr int num_pids = 3;
	array<bool,num_pids> pids_enabled = {{
			m_stabilize_roll,
			m_stabilize_pitch,
			m_stabilize_yaw
		}};
	array<PID<>*, num_pids> pids = {{
			m_config.pid[FlightControllerPID_Roll],
			m_config.pid[FlightControllerPID_Pitch],
			m_config.pid[FlightControllerPID_Yaw]
		}};

	switch(c) {
	case ' ':
		m_throttle = -1.f;
		m_stabilize_pitch = m_stabilize_roll = m_stabilize_yaw = false;
		break;
	case 'j':
		m_throttle -= 0.01;
		break;
	case 'k':
		m_throttle += 0.01;
		break;
	case 'n':
		m_throttle -= 0.001;
		break;
	case 'm':
		m_throttle += 0.001;
		break;
	case '1':
		m_stabilize_roll = !m_stabilize_roll;
		pids[0]->reset_I();
		break;
	case '2':
		m_stabilize_pitch = !m_stabilize_pitch;
		pids[1]->reset_I();
		break;
	case '3':
		m_stabilize_yaw = !m_stabilize_yaw;
		pids[2]->reset_I();
		break;
	case 'w':
		for(int i=0; i<num_pids; ++i)
			if(pids_enabled[i]) pids[i]->kP(pids[i]->kP()-0.01f);
		break;
	case 'e':
		for(int i=0; i<num_pids; ++i)
			if(pids_enabled[i]) pids[i]->kP(pids[i]->kP()+0.01f);
		break;
	case 's':
		for(int i=0; i<num_pids; ++i)
			if(pids_enabled[i]) {
				pids[i]->kI(pids[i]->kI()-0.0002f);
				pids[i]->reset_I();
			}
		break;
	case 'd':
		for(int i=0; i<num_pids; ++i)
			if(pids_enabled[i]) {
				pids[i]->kI(pids[i]->kI()+0.0002f);
				pids[i]->reset_I();
			}
		break;
	case 'x':
		for(int i=0; i<num_pids; ++i)
			if(pids_enabled[i]) pids[i]->kD(pids[i]->kD()-0.002f);
		break;
	case 'c':
		for(int i=0; i<num_pids; ++i)
			if(pids_enabled[i]) pids[i]->kD(pids[i]->kD()+0.002f);
		break;
	case 'r':
		m_use_throttle_remote = !m_use_throttle_remote;
		break;
	}
	
	/* motor output */
	Vec3f pid_output = m_pid_roll_pitch_yaw_output;
	if(!m_stabilize_roll) pid_output.x = 0.f;
	if(!m_stabilize_pitch) pid_output.y = 0.f;
	if(!m_stabilize_yaw) pid_output.z = 0.f;
	m_config.motor_controller->setThrust(getThrottle(), pid_output);


	Timestamp cur_time = getTimestamp();
	if(time_after(cur_time, m_next_update)) {
		refreshOutput();
		m_next_update = cur_time + update_delay_ms*1000;
	}
	
	return 1;
}

void CommandStabilize::refreshOutput() {
	InputOutput& io = m_command_line.inputOutput();
	m_command_line.eraseLines(m_last_printed_line_count);

	m_last_printed_line_count = 7;

	if(m_throttle_remote) {
		io.printf("Use throttle from remote: %i ('r' to toggle)\n", m_use_throttle_remote);
		++m_last_printed_line_count;
	}
	
	//enabled
	int enabled_count = 0;
	io.printf("Stabilize:");
	if(m_stabilize_roll) { io.printf(" Roll"); ++enabled_count; }
	if(m_stabilize_pitch) { io.printf(" Pitch"); ++enabled_count; }
	if(m_stabilize_yaw) { io.printf(" Yaw"); ++enabled_count; }
	if(!enabled_count) io.printf(" None (use '1'-'3')");
	io.writeByte('\n');
	
	io.printf("Throttle: %.3f\n", getThrottle());
	
	PID<>* pid;
	pid = m_config.pid[FlightControllerPID_Roll];
	io.printf("PID Roll : P=%.5f, I=%.5f, D=%.5f\n", pid->kP(), pid->kI(), pid->kD());
	pid = m_config.pid[FlightControllerPID_Pitch];
	io.printf("PID Pitch: P=%.5f, I=%.5f, D=%.5f\n", pid->kP(), pid->kI(), pid->kD());
	pid = m_config.pid[FlightControllerPID_Yaw];
	io.printf("PID Yaw  : P=%.5f, I=%.5f, D=%.5f\n", pid->kP(), pid->kI(), pid->kD());
	
	io.printf("Dest Roll/Pitch/Yaw: %.3f, %.3f, %.3f\n",
		m_dest_roll_pitch_yaw.x, m_dest_roll_pitch_yaw.y, m_dest_roll_pitch_yaw.z);
	io.printf("Cur  Roll/Pitch/Yaw: %.3f, %.3f, %.3f\n",
		m_attitude.x, m_attitude.y, m_attitude.z);
}

float CommandStabilize::getThrottle() {
	return m_throttle_remote && m_use_throttle_remote ? *m_throttle_remote : m_throttle;
}
