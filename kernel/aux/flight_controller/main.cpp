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

#include <vector>
#include <cmath>
#include <array>
using namespace std;

#include <drivers/i2c/adafruit_pwm.hpp>
#include <drivers/i2c/mpu-6050_accel_gyro.hpp>
#include <drivers/i2c/bmp180_barometer.hpp>
#include <drivers/i2c/hmc5883l_compass.hpp>

#include <kernel/utils.h>
#include <kernel/gpio.h>
#include <kernel/math.h>
#include <kernel/serial.h>
#include <kernel/aux/filter.hpp>

#include "flight_controller.hpp"
#include "main.hpp"
#include "sensor.hpp"
#include "common.hpp"

#ifdef FLIGHT_CONTROLLER_DEBUG_MODE
# define RETURN_IF_NOT_DEBUG NOP
#else
# define RETURN_IF_NOT_DEBUG return
#endif /* FLIGHT_CONTROLLER_DEBUG_MODE */


void flight_controller_main() {
	
	FlightControllerConfig config;
	
	/* Input */
	array<int, 4> gpio_input_pins = {{11 /* yaw */, 11 /* pitch */, 11 /* roll */, 11 /* thrust */}};
	InputControlPWMIRQ<> input_control(gpio_input_pins[0], gpio_input_pins[1],
			gpio_input_pins[2], gpio_input_pins[3]);
	input_control.setupAndEnableIRQs();
	config.input_control = &input_control;
	//TODO: set calibration... (thrust value can also be a bit larger than 1)
	input_control.setOffsetAll(0.f);
	input_control.setScalingAll(1.f/20.f);
	//TODO: test PPM input: InputControlPPMSumIRQ
	
	/* input limits & handling */
	input_control.getConverter(InputControlValue_Roll).setAbsolute(0.f, DEG2RAD(45.f));
	input_control.getConverter(InputControlValue_Pitch).setAbsolute(0.f, DEG2RAD(45.f));
	input_control.getConverter(InputControlValue_Yaw).setRelative(0.f, 1.f, -M_PI, M_PI, true);
	input_control.getConverter(InputControlValue_Throttle).setAbsolute(0.f, 1.f);

	
	/* sensors */
	I2CMPU6050 accelgyro;
	accelgyro.initialize();
	//enable slave bypass for direct access to attached compass
	accelgyro.setI2CMasterModeEnabled(false);
	accelgyro.setI2CBypassEnabled(true);
	if(!accelgyro.testConnection()) {
		printk_crit("Error: no connection to Accel/Gyro\n");
		RETURN_IF_NOT_DEBUG;
	}
	SensorMPU6050GyroAccel<true> sensor_accel(accelgyro);
	SensorMPU6050GyroAccel<false> sensor_gyro(accelgyro);
	config.sensor_accel = &sensor_accel;
	config.sensor_gyro = &sensor_gyro;

	SensorBMP180Baro<> sensor_baro;
	sensor_baro.initialize();
	if(!sensor_baro.testConnection()) {
		printk_crit("Error: no connection to Barometer!\n");
		RETURN_IF_NOT_DEBUG;
	}
	sensor_baro.setControl(BMP085_MODE_PRESSURE_3);
	config.sensor_barometer = &sensor_baro;
	
	SensorHMC5883LCompass<> sensor_compass;
	sensor_compass.initialize();
	if(!sensor_compass.testConnection()) {
		printk_crit("Error: no connection to Compass!\n");
		RETURN_IF_NOT_DEBUG;
	}
	config.sensor_compass = &sensor_compass;
	
	SensorFusionMahonyAHRS sensor_fusion(1.4f, 0.02f);
	config.sensor_fusion = &sensor_fusion;
	config.altitude_cutoff_freq = 0.15f; //barometer data is very noisy!
	
	/* IMU is not mounted perfectly planar */
	config.attitude_offset = Math::Vec3f(0.026, 0.048, 0);
	
	
	/* motor output */
	int pwm_frequency = 300;
	float mfreq = (float)pwm_frequency/1000.f;
	array<float, 4> min_thrusts = {{mfreq*1.312f, mfreq*1.297f, mfreq*1.355f, mfreq*1.305f}}; /* min thrust per motor (around 1ms) */
	array<float, 4> max_thrusts = {{mfreq*(1.312f+0.8f), mfreq*(1.297f+0.8f), mfreq*(1.355f+0.8f), mfreq*(1.305f+0.8f)}}; /* max thrust per motor (around 2ms) */
	MotorControllerAdafruitPWM motor_controller(min_thrusts, max_thrusts,
		{{ 0, 1, 2, 3 }}, /* motor channel association (see also motor_controller.hpp) */
		i2cWrite, i2cRead);
	motor_controller.reset();
	if(!motor_controller.setPWMFreq(pwm_frequency)) {
		printk_crit("Error: setting PWM frequency failed\n");
		RETURN_IF_NOT_DEBUG;
	}
	config.motor_controller = &motor_controller;
	
	
	/* PID controllers */
	PID<> pid_roll( {{0.15, 0.0003, 0.002}});
	PID<> pid_pitch({{0.15, 0.0003, 0.002}});
	PID<> pid_yaw(  {{0.15, 0.0003, 0.002}});
	config.pid[FlightControllerPID_Roll] = &pid_roll;
	config.pid[FlightControllerPID_Pitch] = &pid_pitch;
	config.pid[FlightControllerPID_Yaw] = &pid_yaw;
	
	
	/* console command line */
	auto uart_writef = [](int c) { if(c=='\n') uartWrite('\r'); uartWrite(c); return 0; };
	InputOutput io(uartTryRead, uart_writef);
	CommandLine cmd_line(io, "$\x1b[32;1mbPI\x1b[0m> ");
	config.command_line = &cmd_line;
	

	/* additional stuff */
	LedBlinker led_blinker(0);
	config.led_blinker = &led_blinker;
	
	
	/* let's start flying ... */
	FlightController flight_controller(config);
	flight_controller.run();
	
}
