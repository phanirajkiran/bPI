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
	uint hz_counter = 0, current_frequency = 0;
	uint attitude_hz_counter = 0, current_attitude_frequency = 0;
	Timestamp timer_every_second = getTimestamp() + 1000*1000;
	uint seconds_counter = 0;

	/* flight variables */
	Vec3f input_roll_pitch_yaw(0.f);
	float input_throttle=0.f, altitude_filtered;
	Filter1D<Filter1D_EMA> altitude_filter(0.8);
	float* input_data[InputControlValue_Count];
	memset(input_data, 0, sizeof(float*)*InputControlValue_Count);
	input_data[InputControlValue_Roll] = &input_roll_pitch_yaw.x;
	input_data[InputControlValue_Pitch] = &input_roll_pitch_yaw.y;
	input_data[InputControlValue_Yaw] = &input_roll_pitch_yaw.z;
	input_data[InputControlValue_Throttle] = &input_throttle;
	Vec3f attitude(0.f); //roll, pitch, yaw
	Vec3f pid_roll_pitch_yaw_output;
	DeltaTime delta_time_pid, delta_time_sensor_fusion;
	bool landing_notice_printed = false;
	
	m_data_baro.next_readout = m_data_compass.next_readout =
		m_data_accel.next_readout = m_data_gyro.next_readout = getTimestamp();
	
	/* add more commands */
	if(m_config.command_line) {
		auto freq_cmd = [&current_frequency, &current_attitude_frequency](
				const vector<string>& arguments, InputOutput& io) {
			io.printf("Current loop frequency    : %i Hz\n", current_frequency);
			io.printf("Current attitude frequency: %i Hz\n", current_attitude_frequency);
		};
		m_config.command_line->addTestCommand(freq_cmd, "freq", "print main loop update frequency");
		int cmd_print_rate = 100; //[ms]
		bool clear_output = true;
		CommandWatchValues* watch_sensor_cmd = new CommandWatchValues("sensors",
				*m_config.command_line, cmd_print_rate, clear_output);
		watch_sensor_cmd->addValue("altitude", m_data_baro.sensor_data);
		watch_sensor_cmd->addValue("altitude-filtered", altitude_filtered);
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
			pid_roll_pitch_yaw_output, &input_throttle);
		m_config.command_line->addCommand(*stabilize_cmd);
	}
	
	// state switching
	auto switchState = [&] (State new_state) {
		switch(new_state) {
		case State_landed:
			led_blinker->setBlinkRate(600);
			landing_notice_printed = false;
#ifdef FLIGHT_CONTROLLER_DEBUG_MODE
			new_state = State_debug;
			printk_i("FlightController: changing to State_debug\n");
#else
			m_config.motor_controller->setMotorSpeedMin();
			printk_i("FlightController: changing to State_landed (motor control turned off)\n");
#endif /* FLIGHT_CONTROLLER_DEBUG_MODE */
			break;
		case State_manual:
			led_blinker->setBlinkRate(100);
			printk_i("FlightController: changing to State_manual\n");
			break;
		case State_flying:
			led_blinker->setBlinkRate(300);
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
			break;
		default: panic("Error: unhandled flying state %i!\n", (int)new_state);
		break;
		}
		m_state = new_state;
	};

	
	/* main loop */
	
	while(1) {
		/* update sensor data */
		int got_sensor_data = 
			updateSensor(*m_config.sensor_barometer, m_data_baro) +
			updateSensor(*m_config.sensor_compass, m_data_compass) +
			updateSensor(*m_config.sensor_accel, m_data_accel) +
			updateSensor(*m_config.sensor_gyro, m_data_gyro);
		/*
		 * A note on sensor filtering:
		 * The current sensor fusion algorithm (Mahony) is very good at filtering,
		 * so no sensor-prefiltering is needed (plus the accel/gyro has a built-in
		 * LP filter). But if another sensor fusion alg is used, this probably
		 * becomes necessary: use EMA, around 25-30Hz (integrate into SensorBase?)
		 */
		
		
		
		if(got_sensor_data) {
			float dt = delta_time_sensor_fusion.nextDeltaMilli<float>();
			m_config.sensor_fusion->update(m_data_gyro.sensor_data, m_data_accel.sensor_data,
					m_data_compass.sensor_data, dt, attitude);
			attitude += m_config.attitude_offset;
			altitude_filtered = altitude_filter.nextValue(m_data_baro.sensor_data);
			++attitude_hz_counter;
		}

		
		/* input */
		m_config.input_control->update();
		for(int i=0; i<InputControlValue_Count; ++i) {
			if(!input_data[i]) continue;
			float val;
			if(m_config.input_control->getControlValue((InputControlValue)i, val)) {
				*input_data[i] = val;
			}
		}
		
		//calc PID's
		float dt = delta_time_pid.nextDeltaMilli<float>();
		Vec3f error = input_roll_pitch_yaw - attitude;
		for(int i=0; i<3; ++i) {
			if(error[i] > M_PI) error[i] -= 2.*M_PI;
			else if(error[i] < -M_PI) error[i] += 2.*M_PI;
		}
		pid_roll_pitch_yaw_output.x = m_config.pid[FlightControllerPID_Roll]->get_pid(error.x, dt);
		pid_roll_pitch_yaw_output.y = m_config.pid[FlightControllerPID_Pitch]->get_pid(error.y, dt);
		pid_roll_pitch_yaw_output.z = m_config.pid[FlightControllerPID_Yaw]->get_pid(error.z, dt);
		

		switch(m_state) {
		case State_init:
			if(time_after(getTimestamp(), init_delay_timestamp)) {
				m_config.sensor_fusion->enableFastConvergence(false);
#if defined(FLIGHT_CONTROLLER_INIT_MOTORS) || !defined(FLIGHT_CONTROLLER_DEBUG_MODE)
				initMotors();
#endif
				switchState(State_landed);
			}
			break;
		case State_debug:
			/* nothing to do: we never change to another state */
			break;
		case State_landed:
			
			if(m_config.input_switch_flying->isOn()) {
				switchState(State_flying);
			} else if(m_config.input_switch_flying->numStates() == 3 &&
					m_config.input_switch_flying->getState() == 1) {
				switchState(State_manual);
			}
			break;
		case State_manual:
			
			if(m_config.input_switch_flying->isOff()) {
				switchState(State_landed);
			} else if(m_config.input_switch_flying->isOn()) {
				switchState(State_flying);
			} else {
				//set motor output
				Vec3f raw_input;
				m_config.input_control->getControlValueRaw(InputControlValue_Roll, raw_input.x);
				m_config.input_control->getControlValueRaw(InputControlValue_Pitch, raw_input.y);
				m_config.input_control->getControlValueRaw(InputControlValue_Yaw, raw_input.z);
				m_config.motor_controller->setThrust(input_throttle, raw_input);
			}
			break;
			
		case State_flying:
			
			if(m_config.input_switch_flying->isOff()) {
				switchState(State_landed);
			} else if(m_config.input_switch_flying->numStates() == 3 &&
					m_config.input_switch_flying->getState() == 1) {
				switchState(State_manual);
			} else {
				//set motor output
				m_config.motor_controller->setThrust(input_throttle, pid_roll_pitch_yaw_output);
			}
			
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
			hz_counter = 0;
			current_attitude_frequency = attitude_hz_counter;
			attitude_hz_counter = 0;
			timer_every_second = cur_timestamp + 1000*1000;
			
			
			//update PID cutoff frequency
			for(int i=0; i<FlightControllerPID_Count; ++i) {
				m_config.pid[i]->d_filter().setCutoffFreq(m_config.pid_integrator_cutoff_freq,
						1.f/(float)current_frequency);
			}
			altitude_filter.setCutoffFreq(m_config.altitude_cutoff_freq,
					1.f/(float)current_attitude_frequency);
			
			if(m_state == State_landed && seconds_counter % 5 == 0 && !landing_notice_printed) {
				landing_notice_printed = true;
				printk_d("In Landed state: waiting for flying switch to be enabled...\n");
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
