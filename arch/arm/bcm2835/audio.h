/*
 * Copyright (C) 2013 Beat Küng <beat-kueng@gmx.net>
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

#ifndef BCM2835_AUDIO_HEADER_H_
#define BCM2835_AUDIO_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <kernel/aux/wave/wave.h>

#define MIN_AUDIO_VOLUME 0
#define MAX_AUDIO_VOLUME ((1u << 16)-1u)


/*
 * init audio (PWM, to GPIO 40 & 45) using 2 channels
 * sample_rate: samples per second (eg 44100)
 *
 * returns: 0 on success
 */
int initAudio(int sample_rate);


int playAudio(const wave_hdr* wave, const char* buffer, uint buffer_len);

/*
 * play sine with duration in ms.
 * init_angle is the inititial angle for the sine in rad
 * return: last played angle in [0, 2*PI]
 */
float playSine(int frequency, int duration_ms, float init_angle);


void setAudioVolume(uint32 volume);
uint32 getAudioVolume();

#ifdef __cplusplus
}
#endif
#endif /* BCM2835_AUDIO_HEADER_H_ */






