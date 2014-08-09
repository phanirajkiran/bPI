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

/** @file common definitions & configuration options for flight controller */

#ifndef _FLIGHT_CONTROLLER_COMMON_HEADER_HPP_
#define _FLIGHT_CONTROLLER_COMMON_HEADER_HPP_

/**
 * debug mode: uncomment to enable debug mode. this does the following:
 * - if a sensor is not connected, it does not lead to a fatal error and the
 *   fight controller will still run.
 * - it guarantees that the motor output is never activated or changed
 *   automatically. however it is still possible to do it manually via console.
 *   (if FLIGHT_CONTROLLER_DEBUG_MODE is set below, motors are initialized to
 *   minimum thrust)
 * 
 * this allows you to use the serial console and for example display sensor
 * values & input control values and set motor output speed manually.
 */
//#define FLIGHT_CONTROLLER_DEBUG_MODE

/** init motors after startup if defined: this is only useful in debug mode.
 *  in non-debug mode the motors are always initialized
 */
//#define FLIGHT_CONTROLLER_INIT_MOTORS


#endif /* _FLIGHT_CONTROLLER_COMMON_HEADER_HPP_ */


