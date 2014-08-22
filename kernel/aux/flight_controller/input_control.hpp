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
#include <kernel/timer.h>
#include <kernel/aux/filter.hpp>
#include <kernel/aux/delta_time.hpp>
#include <kernel/interrupt.h>
#include <kernel/gpio.h>
#include <drivers/ppm/decode.h>

#include <algorithm>


/**
 * input channels
 */
enum InputControlValue {
	InputControlValue_Yaw=0,
	InputControlValue_Pitch,
	InputControlValue_Roll,
	InputControlValue_Throttle,
	
	InputControlValue_Usr1, /* user input controls: eg for landing/flying switch */
	InputControlValue_Usr2,
	InputControlValue_Usr3,
	InputControlValue_Usr4,

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
	 * use relative value: first the offset is added, then it is scaled &
	 * multiplied with delta time in ms, then it is added to the previous value &
	 * range is checked. if it is out of range and wrap_around is enabled, value is
	 * shifted into the [min_val,max_val] interval.
	 */
	void setRelative(T offset, T scaling, T min_val=-1e13f, T max_val=1e13f,
			bool wrap_around=false) {
		m_conversion_type = Conversion_Relative;
		m_offset = offset;
		m_scaling = scaling;
		m_min_value = min_val;
		m_max_value = max_val;
		m_wrap_around = wrap_around;
		m_delta_time.reset();
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
	DeltaTime m_delta_time;

	T m_last_value = T(0);
};


/**
 * base class to get inputs: eg yaw, pitch, roll & thrust values.
 * it also filters (lowpass) the values
 */
template<typename T=float>
class InputControlBase {
public:
	
	typedef Filter1D<Filter1D_MovingAverage, 4, T> InputFilter;
	
	
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
	 * @return true if value changed since last read, false otherwise
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
 * a switch based on one of the (user) channel inputs.
 * as elsewhere the input value is assumed to be in [-1,1] and the states
 * are evenly distributed over this domain, where a larger input value is
 * a larger state.
 */
template<typename T=float>
class InputSwitch {
public:
	InputSwitch(InputControlBase<T>& input, int num_state = 2,
		InputControlValue control_value = InputControlValue_Usr1);
	
	/**
	 * get current state of the button
	 * @return state in [0, numStates()-1]
	 */
	int getState();
	
	/**
	 * switch is on iff state == numStates()-1
	 */
	bool isOn() { return getState() == m_num_states-1; }

	/**
	 * switch is off iff state == 0
	 */
	bool isOff() { return getState() == 0; }
	
	int numStates() const { return m_num_states; }
private:
	InputControlBase<T>& m_input;
	int m_num_states;
	InputControlValue m_control_value;
};


/** 
 * input class which reads PWM values from interrupts.
 */
template<typename T=float>
class InputControlPWMIRQ : public InputControlBase<T> {
public:
	/**
	 * constructor. this does NOT enable gpio irq's. set a value to -1 to disable
	 * @param yaw_gpio_idx gpio pin for yaw
	 * @param pitch_gpio_idx gpio pin for pitch
	 * @param roll_gpio_idx gpio pin for roll
	 * @param throttle_gpio_idx gpio pin for throttle
	 * @param usr1_gpio_idx gpio pin for user defined input
	 */
	InputControlPWMIRQ(int yaw_gpio_idx, int pitch_gpio_idx, int roll_gpio_idx,
			int throttle_gpio_idx, int usr1_gpio_idx=-1);
	
	/**
	 * set gpio's as input and enable edge detection & IRQ's
	 */
	void setupAndEnableIRQs();
	
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
	
	/**
	 * set gpio as input & enable edge detection
	 * @param pin GPIO pin number
	 */
	static void setupGPIOPin(int pin);
protected:
	void updateValue(InputControlValue value, T data);

	int m_gpio_indexes[InputControlValue_Count]; //GPIO pins
	
	T m_offset[InputControlValue_Count];
	T m_scaling[InputControlValue_Count];
	
	Timestamp m_gpio_last_timestamps[InputControlValue_Count];
};


/**
 * PPM sum input signal: single signal for multiple channels.
 */
template<typename T=float>
class InputControlPPMSumIRQ : public InputControlPWMIRQ<T> {
public:
	/**
	 * constructor. does not setup gpio irqs.
	 * 
	 * *_gpio_idx: define ordering of the PPM signal: which pulse is used after
	 * the sync pulse (first pulse after sync has index 0)
	 * set a value to -1 to disable
	 * 
	 * @param gpio_signal_pin the gpio pin with the PPM signal
	 * 
	 * @param yaw_gpio_idx
	 * @param pitch_gpio_idx
	 * @param roll_gpio_idx
	 * @param throttle_gpio_idx
	 * @param usr1_gpio_idx
	 */
	InputControlPPMSumIRQ(int gpio_signal_pin, int yaw_gpio_idx, int pitch_gpio_idx,
			int roll_gpio_idx, int throttle_gpio_idx, int usr1_gpio_idx=-1);

	/**
	 * set gpio's as input and enable edge detection & IRQ's
	 */
	void setupAndEnableIRQs();

	virtual void update();
private:
	int m_gpio_ppm_pin;
};


/* implementation */

template<typename T>
inline InputControlBase<T>::InputControlBase() {
	for(int i=0; i<InputControlValue_Count; ++i) {
		m_values_changed[i]=false;
		m_values[i] = T(0);
	}
}

template<typename T>
inline InputControlPWMIRQ<T>::InputControlPWMIRQ(int yaw_gpio_idx,
		int pitch_gpio_idx, int roll_gpio_idx, int throttle_gpio_idx,
		int usr1_gpio_idx) {
	for(int i=0; i<InputControlValue_Count; ++i) {
		m_gpio_indexes[i] = -1;
		m_offset[i] = T(0);
		m_scaling[i] = T(1);
		m_gpio_last_timestamps[i] = 0;
	}
	m_gpio_indexes[InputControlValue_Yaw] = yaw_gpio_idx;
	m_gpio_indexes[InputControlValue_Pitch] = pitch_gpio_idx;
	m_gpio_indexes[InputControlValue_Roll] = roll_gpio_idx;
	m_gpio_indexes[InputControlValue_Throttle] = throttle_gpio_idx;
	m_gpio_indexes[InputControlValue_Usr1] = usr1_gpio_idx;
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
		if(idx == -1) continue;
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
inline void InputControlPWMIRQ<T>::setupAndEnableIRQs() {
	for(size_t i=0; i<InputControlValue_Count; ++i) {
		if(m_gpio_indexes[i] != -1)
			setupGPIOPin(m_gpio_indexes[i]);
	}
	enableGpioIRQ();
}

template<typename T>
inline void InputControlPWMIRQ<T>::setupGPIOPin(int pin) {
	setGpioPullUpDown(pin, 0); //0=off, 1=pull-down
	setGpioFunction(pin, 0); //set as input
	setGpioEdgeDetect(pin, GPIO_BOTH_EDGES);
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
	float dt = m_delta_time.nextDeltaMilli<float>();
	T new_value = (input_value + m_offset)*m_scaling*dt + m_last_value;
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

template<typename T>
inline InputControlPPMSumIRQ<T>::InputControlPPMSumIRQ(int gpio_signal_pin,
		int yaw_gpio_idx, int pitch_gpio_idx, int roll_gpio_idx,
		int throttle_gpio_idx, int usr1_gpio_idx)
	: InputControlPWMIRQ<T>(yaw_gpio_idx, pitch_gpio_idx, roll_gpio_idx,
	  throttle_gpio_idx, usr1_gpio_idx),
	  m_gpio_ppm_pin(gpio_signal_pin) {
}

template<typename T>
inline void InputControlPPMSumIRQ<T>::setupAndEnableIRQs() {
	setupPPMDecoder(m_gpio_ppm_pin, 4500);
	InputControlPWMIRQ<T>::setupGPIOPin(m_gpio_ppm_pin);
	enableGpioIRQ();
}

template<typename T>
void InputControlPPMSumIRQ<T>::update() {
	disableInterrupts();
	for(int i=0; i<InputControlValue_Count; ++i) {
		int idx = this->m_gpio_indexes[i];
		if(idx == -1) continue;
		if(g_ppm_signals[idx].pulse_stop != this->m_gpio_last_timestamps[i]) {
			this->m_gpio_last_timestamps[i] = g_ppm_signals[idx].pulse_stop;
			this->updateValue((InputControlValue)i,
				T(g_ppm_signals[idx].pulse_stop - g_ppm_signals[idx].pulse_start) / T(1000));
		}
	}
	enableInterrupts();
}

template<typename T>
inline InputSwitch<T>::InputSwitch(InputControlBase<T>& input, int num_state,
		InputControlValue control_value)
	: m_input(input), m_num_states(num_state), m_control_value(control_value) {
}

template<typename T>
inline int InputSwitch<T>::getState() {
	T value;
	m_input.getControlValueRaw(m_control_value, value);
	int state = (int)(((value+T(1))/T(2))*T(m_num_states));
	return std::min(m_num_states-1, state);
}

#endif /* _FLIGHT_CONTROLLER_INPUT_CONTROL_HEADER_HPP_ */


