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

#ifndef _DELTA_TIME_HEADER_HPP_
#define _DELTA_TIME_HEADER_HPP_

#include <kernel/timer.h>

/**
 * helper class to calculate delta times
 */
class DeltaTime {
public:
	DeltaTime() { m_last_timestamp = getTimestamp(); }
	
	/**
	 * @return microseconds since last delta call (or since object constructed)
	 */
	Timestamp nextDeltaMicro() {
		Timestamp timestamp = getTimestamp();
		Timestamp dt = (timestamp - m_last_timestamp);
		m_last_timestamp = timestamp;
		return dt;
	}
	
	/**
	 * same as nextDeltaMicro but in milliseconds
	 */
	template<typename T>
	T nextDeltaMilli() {
		return (T)nextDeltaMicro()/(T)1000;
	}
	
	/**
	 * reset to current time
	 */
	void reset() {
		m_last_timestamp = getTimestamp();
	}
private:
	Timestamp m_last_timestamp;
};


#endif /* _DELTA_TIME_HEADER_HPP_ */


