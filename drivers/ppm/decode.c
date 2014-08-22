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

#include "decode.h"

#include <kernel/interrupt.h>

PPMSignal g_ppm_signals[MAX_PPM_CHANNELS];

static volatile int registered_gpio_pin;
static volatile int current_ppm_channel;
static volatile uint sync_pulse_length; //in microseconds
static volatile Timestamp last_pulse_start;


/** callback to handle IRQ's from GPIOs */
void PPMGpioIRQPinHandler(int pin, int value);



int setupPPMDecoder(int gpio_pin, uint min_sync_pulse_length) {
	//initialize
	sync_pulse_length = min_sync_pulse_length;
	current_ppm_channel = -1;
	
	registered_gpio_pin = gpio_pin;
	return registerGpioIrqEventHandler(PPMGpioIRQPinHandler);
}


void PPMGpioIRQPinHandler(int pin, int value) {
	if(pin != registered_gpio_pin) return;

	if(value) { //pulse start
		last_pulse_start = g_irq_gpio_high_last_timestamp[pin];
	} else { //pulse end
		Timestamp cur_time = g_irq_gpio_low_last_timestamp[pin];
		if(cur_time - last_pulse_start > sync_pulse_length) {
			current_ppm_channel = 0;
		} else if(current_ppm_channel >= 0 && current_ppm_channel < MAX_PPM_CHANNELS) {
			g_ppm_signals[current_ppm_channel].pulse_start = last_pulse_start;
			g_ppm_signals[current_ppm_channel].pulse_stop = cur_time;
			++current_ppm_channel;
		}
	}
}
