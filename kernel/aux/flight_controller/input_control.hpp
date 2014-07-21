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

#ifndef _FLIGHT_CONTROLLER_INPUT_CONTROL_HEADER_HPP_
#define _FLIGHT_CONTROLLER_INPUT_CONTROL_HEADER_HPP_

#include <kernel/utils.h>
#include <kernel/aux/filter.hpp>
#include <kernel/interrupt.h>

#include <algorithm>


enum InputControlValue {
	InputControlValue_Yaw=0,
	InputControlValue_Pitch,
	InputControlValue_Roll,
	InputControlValue_Thrust,

	InputControlValue_Count,
};


/**
 * base class to get inputs: eg yaw, pitch, roll & thrust values.
 * it also filters (lowpass) the values
 */
template<typename T=float>
class InputControlBase {
public:
	
	typedef Filter1D<Filter1D_MovingAverage, 3, T> InputFilter;
	
	
	InputControlBase();
	
	/** update for new values: called once per main loop */
	virtual void update()=0;

	/**
	 * get current value 
	 * @param value which value to read
	 * @param out_value output value. all values are within [-1,1]
	 * @return true if value changed since last read, false otherwise
	 */
	virtual bool getControlValue(InputControlValue value, T& out_value) {
		out_value = m_values[value];
		if(m_values_changed[value]) { m_values_changed[value]=false; return true; }
		return false;
	}
protected:
	void updateValue(InputControlValue value, T data) {
		m_values[value] = m_filters[value].nextValue(std::min(T(1), std::max(T(-1), data)));
		m_values_changed[value] = true;
	}
	
	T m_values[InputControlValue_Count];
	bool m_values_changed[InputControlValue_Count];
	InputFilter m_filters[InputControlValue_Count];
};


/** 
 * input class which reads PWM values from interrupts. assumes IRQ's are
 * setup already
 */
template<typename T=float>
class InputControlPWMIRQ : public InputControlBase<T> {
public:
	InputControlPWMIRQ(int yaw_gpio_idx, int pitch_gpio_idx, int roll_gpio_idx,
			int thrust_gpio_idx);
	
	/**
	 * calibration methods: offset & scaling. for each value, first the offset
	 * is added, then it is multiplied with the scaling. values before scaling
	 * are in milliseconds.
	 * set the offset such that a zero input maps to 0, and the scaling such
	 * that the values are within [-1,1].
	 */
	void setOffset(InputControlValue value, T offset);
	void setOffsetAll(T offset);
	void setScaling(InputControlValue value, T scaling);
	void setScalingAll(T scaling);

	virtual void update();
protected:
	void updateValue(InputControlValue value, T data);

	int m_gpio_indexes[InputControlValue_Count];
	
	T m_offset[InputControlValue_Count];
	T m_scaling[InputControlValue_Count];
	
	uint m_gpio_last_timestamps[InputControlValue_Count];
};




template<typename T>
inline InputControlBase<T>::InputControlBase() {
	for(int i=0; i<InputControlValue_Count; ++i) {
		m_values_changed[i]=false;
		m_values[i] = T(0);
	}
}

template<typename T>
inline InputControlPWMIRQ<T>::InputControlPWMIRQ(int yaw_gpio_idx,
		int pitch_gpio_idx, int roll_gpio_idx, int thrust_gpio_idx) {
	for(int i=0; i<InputControlValue_Count; ++i) {
		m_gpio_indexes[i] = 0;
		m_offset[i] = T(0);
		m_scaling[i] = T(1);
		m_gpio_last_timestamps[i] = 0;
	}
	m_gpio_indexes[InputControlValue_Yaw] = yaw_gpio_idx;
	m_gpio_indexes[InputControlValue_Pitch] = pitch_gpio_idx;
	m_gpio_indexes[InputControlValue_Roll] = roll_gpio_idx;
	m_gpio_indexes[InputControlValue_Thrust] = thrust_gpio_idx;
}

template<typename T>
inline void InputControlPWMIRQ<T>::setOffset(InputControlValue value, T offset) {
	m_offset[value] = offset;
}

template<typename T>
inline void InputControlPWMIRQ<T>::setOffsetAll(T offset) {
	for(int i=0; i<InputControlValue_Count; ++i) {
		m_offset[i] = offset;
	}
}

template<typename T>
inline void InputControlPWMIRQ<T>::setScaling(InputControlValue value, T scaling) {
	m_scaling[value] = scaling;
}

template<typename T>
inline void InputControlPWMIRQ<T>::setScalingAll(T scaling) {
	for(int i=0; i<InputControlValue_Count; ++i) {
		m_scaling[i] = scaling;
	}
}

template<typename T>
inline void InputControlPWMIRQ<T>::update() {
	
	disableInterrupts();
	for(int i=0; i<InputControlValue_Count; ++i) {
		int idx = m_gpio_indexes[i];
		//this can be wrong on wrap-around. but we miss at most one pulse, so no big deal.
		if(g_irq_gpio_low_last_timestamp[idx] != m_gpio_last_timestamps[i]
			&& g_irq_gpio_low_last_timestamp[idx] > g_irq_gpio_high_last_timestamp[idx]) {
			m_gpio_last_timestamps[i] = g_irq_gpio_low_last_timestamp[idx];
			updateValue((InputControlValue)i,
				T(g_irq_gpio_low_last_timestamp[idx] - g_irq_gpio_high_last_timestamp[idx]) / T(1000));
		}
	}
	enableInterrupts();
}

template<typename T>
inline void InputControlPWMIRQ<T>::updateValue(InputControlValue value, T data) {
	InputControlBase<T>::updateValue(value, (data+m_offset[value])*m_scaling[value]);
}

#endif /* _FLIGHT_CONTROLLER_INPUT_CONTROL_HEADER_HPP_ */


