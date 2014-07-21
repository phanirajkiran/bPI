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

#ifndef _LED_BLINKER_HEADER_HPP_
#define _LED_BLINKER_HEADER_HPP_

#include <kernel/types.h>

/**
 * a simple LED blinker class based on polling
 */
class LedBlinker {
public:
	/** constructor: does not change the current state */
	LedBlinker(int which_led);
	
	/** update state: call this inside a main loop */
	void update();
	
	/**
	 * set blinking mode: led will be toggled every rate_ms millisecond.
	 */
	void setBlinkRate(int rate_ms);
	
	/**
	 * set eigher full on or full off
	 * @param on
	 */
	void setLedState(bool on);
	
private:
	int m_which_led;
	int m_state; //0=off, 1=on, 2=blink
	int m_rate_ms;
	uint m_next_toggle;
};



#endif /* _LED_BLINKER_HEADER_HPP_ */


