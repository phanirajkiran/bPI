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

#ifndef _FLIGHT_CONTROLLER_STABILIZE_COMMAND_HEADER_HPP_
#define _FLIGHT_CONTROLLER_STABILIZE_COMMAND_HEADER_HPP_

#include <kernel/aux/command_line.hpp>
#include <kernel/aux/vec3.hpp>
#include "flight_controller.hpp"

/**
 * command to test stabilization
 */
class CommandStabilize : public CommandBase {
public:
	CommandStabilize(CommandLine& command_line,
			FlightControllerConfig& config,
			const Math::Vec3f& attitude,
			Math::Vec3f& dest_roll_pitch_yaw,
			const Math::Vec3f& pid_roll_pitch_yaw_output);
	
	~CommandStabilize();

	virtual void startExecute(const std::vector<std::string>& arguments);
	
	virtual int handleData();
private:
	void refreshOutput(bool clear);
	
	FlightControllerConfig& m_config;
	const Math::Vec3f& m_attitude;
	Math::Vec3f& m_dest_roll_pitch_yaw;
	const Math::Vec3f& m_pid_roll_pitch_yaw_output;
	bool m_stabilize_roll = false;
	bool m_stabilize_pitch = false;
	bool m_stabilize_yaw = false;
	float m_throttle = -1.f;
	Timestamp m_next_update;
	
	static constexpr float update_delay_ms = 100;
};

#endif /* _FLIGHT_CONTROLLER_STABILIZE_COMMAND_HEADER_HPP_ */


