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

#ifndef _FLIGHT_CONTROLLER_FLIGHT_CONTROLLER_HEADER_HPP_
#define _FLIGHT_CONTROLLER_FLIGHT_CONTROLLER_HEADER_HPP_

#include <kernel/aux/command_line.hpp>
#include <kernel/aux/led_blinker.hpp>

#include "sensor.hpp"
#include "motor_controller.hpp"
#include "input_control.hpp"
#include "pid.hpp"

#include <kernel/aux/filter.hpp>


enum FlightControllerPID {
	FlightControllerPID_Roll=0,
	FlightControllerPID_Pitch,
	FlightControllerPID_Yaw,
	
	//TODO: altitude controller?


	FlightControllerPID_Count
};

struct FlightControllerConfig {
	SensorBase<>* sensor_gyro = NULL;
	SensorBase<>* sensor_accel = NULL;
	SensorBase<>* sensor_compass = NULL;
	SensorBase<>* sensor_barometer = NULL;

	MotorControllerBase* motor_controller = NULL;

	InputControlBase<>* input_control = NULL;
	
	CommandLine* command_line = NULL;
	
	LedBlinker* led_blinker = NULL;

	PID<>* pid[FlightControllerPID_Count];
	
	float pid_integrator_cutoff_freq = 25.f; //[Hz]
};


/**
 * flight controller with the main loop.
 * 
 * contains:
 * - sensor readout
 * - attitude calculation
 * - PID controller stabilization
 * - reading RC input
 * - motor speed update
 * - a command line for diagnosis & changing things
 * 
 * requirements:
 * - sensor outputs are as follows:
 *   - barometer: 1 float [m] -> altitude
 *   - mag: 3 float [gauss]
 *   - accel: 3 float [m/s^2]
 *   - gyro: 3 float [rad/s]
 * - coordinate system: RHS
 *   - x == roll (positive = front)
 *   - y == pitch (positive = left)
 *   - z == yaw (positive = up)
 */
class FlightController {
public:

	/**
	 * the sensors must be setup & configured properly
	 */
	FlightController(FlightControllerConfig& config);
	~FlightController();
	
	/** main loop */
	void run();
private:
	
	enum State {
		State_init,
		State_landed,
		State_flying,
	};
	State m_state;
	
	FlightControllerConfig& m_config;

	//min delay timestamps for each sensor
	template<typename T>
	struct SensorData {
		uint next_readout;
		T sensor_data;
	};
	
	SensorData<float> m_data_baro;
	SensorData<Math::Vec3f> m_data_compass;
	SensorData<Math::Vec3f> m_data_accel;
	SensorData<Math::Vec3f> m_data_gyro;
	
	/**
	 * read new sensor data
	 * @return true if new data read
	 */
	template<typename T>
	static bool updateSensor(SensorBase<>& sensor, SensorData<T>& sensor_data);
};


template<typename T>
inline bool FlightController::updateSensor(SensorBase<>& sensor, SensorData<T>& sensor_data) {
	uint cur_timestamp = getTimestamp();
	if(time_after(cur_timestamp, sensor_data.next_readout)) {
		sensor_data.next_readout = cur_timestamp + sensor.minMeasurementDelayMicro();
		sensor.getMeasurement(sensor_data.sensor_data);
		return true;
	}
	return false;
}

#endif /* _FLIGHT_CONTROLLER_FLIGHT_CONTROLLER_HEADER_HPP_ */


