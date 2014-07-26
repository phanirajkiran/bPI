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

/** @file decode PPM (Pulse Position Modulation) Sum signals on a specific GPIO
 *  pin
 */

#ifndef _PPM_DECODE_HEADER_H_
#define _PPM_DECODE_HEADER_H_

#include <kernel/types.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef struct _PPMSignal {
	uint pulse_start_timestamp;
	uint pulse_stop_timestamp; //pulse length in ms = (stop - start)/1000
} PPMSignal;


#define MAX_PPM_CHANNELS 8

extern PPMSignal g_ppm_signals[MAX_PPM_CHANNELS];


/**
 * initialize PPM decoding for a specific pin. Note that this decoder only
 * supports a single pin (and thus a single PPM signal) at a time.
 * (It does not setup GPIO pin or interrupts)
 * FIXME: does not check if it is already registered & there is no unregister!
 * @param min_sync_pulse_length_ms minimum length of the sync pulse in microseconds
 *        (4000 or 5000 is a good value)
 * @return 0 on success
 */
int setupPPMDecoder(int gpio_pin, uint min_sync_pulse_length);


#ifdef __cplusplus
}
#endif
#endif /* _PPM_DECODE_HEADER_H_ */
