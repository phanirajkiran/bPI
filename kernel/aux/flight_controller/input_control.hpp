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
	InputControlValue_Throttle,

	InputControlValue_Count,
};

/**
 * offset & scale input values to defined output values. 
 * relative and absolute values are possible: for relative, a new value is
 * always added to the old one (but first offset & scaling is applied).
 * The default is absolute w/o scaling (=1) or offset (=0)
 */
template<typename T=float>
class ValueConverter {
public:
	enum ConversionType {
		Conversion_Absolute,
		Conversion_Relative
	};
	
	/**
	 * get the next converted value
	 */
	T nextValue(T input_value);
	
	/**
	 * use relative value: first the offset is added, then it is scaled, then
	 * it is added to the previous value & range is checked. if it is out of range
	 * and wrap_around is enabled, value is shifted into the [min_val,max_val]
	 * interval.
	 */
	void setRelative(T offset, T scaling, T min_val=-1e13f, T max_val=1e13f,
			bool wrap_around=false) {
		m_conversion_type = Conversion_Relative;
		m_offset = offset;
		m_scaling = scaling;
		m_min_value = min_val;
		m_max_value = max_val;
		m_wrap_around = wrap_around;
	}

	/**
	 * use absolute value: first the offset is added, then it is scaled.
	 */
	void setAbsolute(T offset, T scaling) {
		m_conversion_type = Conversion_Absolute;
		m_offset = offset;
		m_scaling = scaling;
	}
	
	/**
	 * set the last value to a defined value (only used for relative values)
	 */
	void reset(T reset_value) {
		m_last_value = reset_value;
	}
private:
	/* configuration */
	ConversionType m_conversion_type = Conversion_Absolute;
	T m_scaling = T(1);
	T m_offset = T(0);
	T m_min_value;
	T m_max_value;
	bool m_wrap_around;

	T m_last_value = T(0);
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
	 * get current value without conversion (but filtered)
	 * @param value which value to read
	 * @param out_value output value. all values are within [-1,1]
	 * @return true if value changed since last read, false otherwise
	 */
	virtual bool getControlValueRaw(InputControlValue value, T& out_value) {
		out_value = m_values[value];
		if(m_values_changed[value]) { m_values_changed[value]=false; return true; }
		return false;
	}

	/**
	 * get current value with conversion (and filtered)
	 */
	virtual bool getControlValue(InputControlValue value, T& out_value) {
		out_value = m_values_converted[value];
		if(m_values_changed[value]) { m_values_changed[value]=false; return true; }
		return false;
	}
	
	ValueConverter<T>& getConverter(InputControlValue value) { return m_converters[value]; }
protected:
	void updateValue(InputControlValue value, T data) {
		m_values[value] = m_filters[value].nextValue(std::min(T(1), std::max(T(-1), data)));
		m_values_converted[value] = m_converters[value].nextValue(m_values[value]);
		m_values_changed[value] = true;
	}
	
	T m_values[InputControlValue_Count];
	T m_values_converted[InputControlValue_Count];
	bool m_values_changed[InputControlValue_Count];
	InputFilter m_filters[InputControlValue_Count];
	ValueConverter<T> m_converters[InputControlValue_Count];
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
	m_gpio_indexes[InputControlValue_Throttle] = thrust_gpio_idx;
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

template<typename T>
inline T ValueConverter<T>::nextValue(T input_value) {

	if(m_conversion_type == Conversion_Absolute)
		return (input_value + m_offset)*m_scaling;

	/* relative */
	T new_value = (input_value + m_offset)*m_scaling + m_last_value;
	if(m_wrap_around) {
		while(new_value < m_min_value) new_value += m_max_value-m_min_value;
		while(new_value > m_max_value) new_value -= m_max_value-m_min_value;
	} else {
		if(new_value < m_min_value)
			new_value = m_min_value;
		else if(new_value > m_max_value)
			new_value = m_max_value;
	}
	return (m_last_value = new_value);
}

#endif /* _FLIGHT_CONTROLLER_INPUT_CONTROL_HEADER_HPP_ */


