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

#ifndef _FLIGHT_CONTROLLER_FILTER_HEADER_HPP_
#define _FLIGHT_CONTROLLER_FILTER_HEADER_HPP_

#include <type_traits>
#include <kernel/math.h>

enum Filter1DConfig {
	Filter1D_None,
	Filter1D_MovingAverage,
	Filter1D_EMA, /** moving average with exponentially decreasing weights */
};


/**
 * a templated class to filter 1D values.
 * @param C filter type: defines which algorithm is used to filter
 * @param history_length how many old values to keep (>0). set to 1 for exponential moving average
 * @param T type of the values
 */
template<Filter1DConfig C, int history_length=1, typename T=float>
class Filter1D {
public:
	/**
	 * default constructor
	 */
	template<int dummy=0>
	Filter1D(typename std::enable_if<dummy==0 && C != Filter1D_EMA >::type* = 0) {}
	
	/** get the next (filtered) value */
	inline T nextValue(T new_value) { return new_value; }
	
};

/* moving average filter */
template<int history_length, typename T>
class Filter1D<Filter1D_MovingAverage, history_length, T> {
public:
	Filter1D() {
		for(int i=0; i<history_length; ++i) m_history[i] = T(0);
		m_sum = T(0);
	}
	inline T nextValue(T new_value) {
		m_sum -= m_history[m_index];
		m_history[m_index] = new_value;
		m_sum += new_value;
		m_index = (m_index + 1) % history_length;
		if(m_history_data_length < history_length) ++m_history_data_length;
		return m_sum / (T)m_history_data_length;
	}
private:
	T m_history[history_length];
	T m_sum;
	int m_index = 0;
	int m_history_data_length = 0;
};

/* exponential moving average filter */
template<int history_length, typename T>
class Filter1D<Filter1D_EMA, history_length, T> {
public:
	/**
	 * exponential moving average. alpha is the smoothing factor in [0,1]:
	 * higher value discards older data faster
	 */
	Filter1D(T alpha) : m_alpha(alpha) {}
	
	inline T nextValue(T new_value) {
		if(m_initial_value) {
			m_initial_value = false;
			m_prev_value = new_value;
			return new_value;
		}
		T ret_val = m_prev_value + m_alpha*(new_value-m_prev_value);
		m_prev_value = ret_val;
		return ret_val;
	}
	
	/**
	 * set cutoff frequency of the filter
	 * @param cutoff_freq_hz higher frequencies than this should be cut off
	 * @param time_step time step in seconds (!)
	 */
	void setCutoffFreq(T cutoff_freq_hz, T time_step) {
		T rc = (T)1/(2*M_PI*cutoff_freq_hz);
		m_alpha = time_step / (time_step + rc);
	}
	
	T alpha() const { return m_alpha; }
	void setAlpha(T alpha) { m_alpha = alpha; }
private:
	T m_alpha;
	bool m_initial_value=true;
	T m_prev_value;
};

#endif /* _HEADER_HPP_ */


