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

#include <kernel/timer.h>
#include <kernel/led.h>

#include "led_blinker.hpp"

LedBlinker::LedBlinker(int which_led) : m_which_led(which_led), m_state(1),
	m_rate_ms(50) {
}

void LedBlinker::update() {
	if(m_state != 2) return;
	Timestamp t = getTimestamp();
	if(time_after(t, m_next_toggle)) {
		toggleLed(m_which_led);
		m_next_toggle = t + m_rate_ms*1000;
	}
}

void LedBlinker::setBlinkRate(int rate_ms) {
	m_rate_ms = rate_ms;
	m_state = 2;
	m_next_toggle = getTimestamp() + m_rate_ms*1000;
}

void LedBlinker::setLedState(bool on) {
	if(on) {
		ledOn(m_which_led);
		m_state = 1;
	} else {
		ledOff(m_which_led);
		m_state = 0;
	}
}
