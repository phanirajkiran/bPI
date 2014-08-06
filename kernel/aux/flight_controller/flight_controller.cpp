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

#include "flight_controller.hpp"
#include "motor_command.hpp"
#include "stabilize_command.hpp"
#include "common.hpp"
#include <kernel/aux/vec3.hpp>
#include <kernel/aux/delta_time.hpp>

using namespace Math;
using namespace std;


FlightController::FlightController(FlightControllerConfig& config)
	: m_config(config) {
}

FlightController::~FlightController() {
}

void FlightController::run() {
	
	LedBlinker* led_blinker = m_config.led_blinker;
	if(led_blinker) led_blinker->setLedState(true);
	
	m_state = State_init;
	Timestamp init_delay_timestamp = getTimestamp() + 800 * 1000; //startup delay (time to settle)
	m_config.sensor_fusion->enableFastConvergence(true);
	
	/* frequency counter */
	uint hz_counter = 0, current_frequency = 0, current_frequency_filtered=0;
	Filter1D<Filter1D_MovingAverage, 20, uint> frequency_filter;
	Timestamp timer_every_second = getTimestamp() + 1000*1000;
	uint seconds_counter = 0;

	/* flight variables */
	Vec3f input_roll_pitch_yaw(0.f);
	float input_throttle=0.f;
	float* input_data[InputControlValue_Count];
	input_data[InputControlValue_Roll] = &input_roll_pitch_yaw.x;
	input_data[InputControlValue_Pitch] = &input_roll_pitch_yaw.y;
	input_data[InputControlValue_Yaw] = &input_roll_pitch_yaw.z;
	input_data[InputControlValue_Throttle] = &input_throttle;
	ASSERT(InputControlValue_Count == 4);
	Vec3f attitude(0.f); //roll, pitch, yaw
	Vec3f pid_roll_pitch_yaw_output;
	int loop_counter = 0;
	DeltaTime delta_time_pid, delta_time_sensor_fusion;
	
	m_data_baro.next_readout = m_data_compass.next_readout =
		m_data_accel.next_readout = m_data_gyro.next_readout = getTimestamp();
	
	/* add more commands */
	if(m_config.command_line) {
		auto freq_cmd = [&current_frequency, &current_frequency_filtered](
				const vector<string>& arguments, InputOutput& io) {
			io.printf("Current update frequency: %i Hz, average: %i Hz\n",
					current_frequency, current_frequency_filtered);
		};
		m_config.command_line->addTestCommand(freq_cmd, "freq", "print main loop update frequency");
		int cmd_print_rate = 100; //[ms]
		bool clear_output = true;
		CommandWatchValues* watch_sensor_cmd = new CommandWatchValues("sensors",
				*m_config.command_line, cmd_print_rate, clear_output);
		watch_sensor_cmd->addValue("altitude", m_data_baro.sensor_data);
		watch_sensor_cmd->addValue("compass", m_data_compass.sensor_data);
		watch_sensor_cmd->addValue("gyro", m_data_gyro.sensor_data);
		watch_sensor_cmd->addValue("accel", m_data_accel.sensor_data);
		CommandWatchValues* watch_attitude_cmd = new CommandWatchValues("attitude",
				*m_config.command_line, cmd_print_rate, clear_output);
		watch_attitude_cmd->addValue("attitude", attitude);
		CommandWatchValues* watch_inputs_cmd = new CommandWatchValues("inputs",
				*m_config.command_line, cmd_print_rate, clear_output);
		watch_inputs_cmd->addValue("roll-pitch-yaw", input_roll_pitch_yaw);
		watch_inputs_cmd->addValue("throttle", input_throttle);
		//motor speed tests
		CommandControlMotor* motor_cmd = new CommandControlMotor(*m_config.command_line,
			*m_config.motor_controller);
		m_config.command_line->addCommand(*motor_cmd);

		auto init_motors_cmd = [this](
				const vector<string>& arguments, InputOutput& io) {
			this->initMotors();
		};
		m_config.command_line->addTestCommand(init_motors_cmd, "initmotors",
			"initialize motors");
		
		CommandStabilize* stabilize_cmd = new CommandStabilize(
			*m_config.command_line, m_config, attitude, input_roll_pitch_yaw,
			pid_roll_pitch_yaw_output);
		m_config.command_line->addCommand(*stabilize_cmd);
	}

	
	/* main loop */
	
	while(1) {
		/* update sensor data */
		int got_sensor_data = 
			updateSensor(*m_config.sensor_barometer, m_data_baro) +
			updateSensor(*m_config.sensor_compass, m_data_compass) +
			updateSensor(*m_config.sensor_accel, m_data_accel) +
			updateSensor(*m_config.sensor_gyro, m_data_gyro);
		
		//TODO: filter sensor data
		
		
		if(got_sensor_data) {
			m_config.sensor_fusion->update(m_data_gyro.sensor_data, m_data_accel.sensor_data,
					m_data_compass.sensor_data, delta_time_sensor_fusion.nextDeltaMilli<float>(), attitude);
		}
		//TODO: count frequency for got_sensor_data

		
		/* input */
		m_config.input_control->update();
		for(int i=0; i<InputControlValue_Count; ++i) {
			float val;
			if(m_config.input_control->getControlValue((InputControlValue)i, val)) {
				printk_i("got new input date for value %i: %.5f\n", i, val);
				*input_data[i] = val;
			}
		}
		
		//calc PID's
		float dt = delta_time_pid.nextDeltaMilli<float>();
		Vec3f error = input_roll_pitch_yaw - attitude;
		pid_roll_pitch_yaw_output.x = m_config.pid[FlightControllerPID_Roll]->get_pid(error.x, dt);
		pid_roll_pitch_yaw_output.y = m_config.pid[FlightControllerPID_Pitch]->get_pid(error.y, dt);
		pid_roll_pitch_yaw_output.z = m_config.pid[FlightControllerPID_Yaw]->get_pid(error.z, dt);
		

		switch(m_state) {
		case State_init:
			if(time_after(getTimestamp(), init_delay_timestamp)) {
				led_blinker->setBlinkRate(600);
				loop_counter = 0;
				m_config.sensor_fusion->enableFastConvergence(false);
#ifdef FLIGHT_CONTROLLER_DEBUG_MODE
				m_state = State_debug;
#else
				initMotors();
				printk_i("FlightController: changing to State_landed\n");
				m_state = State_landed;
#endif /* FLIGHT_CONTROLLER_DEBUG_MODE */
			}
			break;
		case State_debug:
			/* nothing to do: we never change to another state */
			break;
		case State_landed:
			
			if(input_throttle < -0.4) {
				led_blinker->setBlinkRate(300);
				m_state = State_flying;
				/* reset values */
				for(int i=0; i<FlightControllerPID_Count; ++i) {
					m_config.pid[i]->reset_I();
				}
				//reset relative input values
				m_config.input_control->getConverter(InputControlValue_Roll).reset(attitude.x);
				m_config.input_control->getConverter(InputControlValue_Pitch).reset(attitude.y);
				m_config.input_control->getConverter(InputControlValue_Yaw).reset(attitude.z);
				m_config.input_control->getConverter(InputControlValue_Throttle).reset(0.f);
				
				printk_i("FlightController: changing to State_flying :)\n");
			}
			break;
		case State_flying:
			
			//calc motor updates
			m_config.motor_controller->setThrust(input_throttle, pid_roll_pitch_yaw_output);
			
			break;
		}

		if(led_blinker) led_blinker->update();
		if(m_config.command_line) m_config.command_line->handleData();
		

		++hz_counter;
		Timestamp cur_timestamp = getTimestamp();
		if(time_after(cur_timestamp, timer_every_second)) {
			/* stuff that needs to be done once per second */
			++seconds_counter;

			//update current frequency
			current_frequency = hz_counter;
			current_frequency_filtered = frequency_filter.nextValue(hz_counter);
			hz_counter = 0;
			timer_every_second = cur_timestamp + 1000*1000;
			
			
			//update PID cutoff frequency
			printk_d("setting cutoff frequency: current freq=%i\n", current_frequency);
			for(int i=0; i<FlightControllerPID_Count; ++i) {
				m_config.pid[i]->d_filter().setCutoffFreq(m_config.pid_integrator_cutoff_freq,
						1.f/(float)current_frequency);
			}
			
			if(m_state == State_landed && seconds_counter % 5 == 0) {
				printk_d("In Landed state: waiting for start condition: input_thrust=%.3f < %.3f\n",
						input_throttle, -0.4);
			}
		
		}
		
	}
	
}
void FlightController::initMotors() {
	printk_i("initializing motors...");
	//FIXME: it would be better to calibrate the ESC here by first setting
	//long pulse (2ms) for about 2 sec then short pulse (1ms) for some seconds.
	//but this does not seem to work
	m_config.motor_controller->setMotorSpeedMin();
	delay(3500);
	printk_i(" done\n");
}
