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

#include "audio.h"
#include "pwm.h"

#include <kernel/gpio.h>
#include <kernel/errors.h>
#include <kernel/timer.h>
#include <kernel/math.h>

#include <kernel/printk.h>


static int clock_source = 0;
static int range_bits = 0;
static uint32 audio_volume;
static int applied_sample_rate;


int initAudio(int sample_rate) {

	setGpioFunction(40, 0b100); //set to PWM0_OUT (Alt func 0)
	setGpioFunction(45, 0b100); //set to PWM1_OUT (Alt func 0)

	const int channels = (1<<0) | (1<<1); //both channels

	clock_source = 6;
	const int mash = 2;
	initPWM(clock_source, mash);
	const int clock_freq = getPWMClockFreq(clock_source); //Hz
	
	//sample_rate * range * divisor = clock_freq

	//calculate max range
	const int min_divisor = 8; //min divisor: depends on MASH setting 
							//(and seems to be wrong in the manual)
	int divisor = min_divisor;
	int range_exp = 1;
	while(range_exp <= 16 
			&& (1<<range_exp) <= clock_freq / divisor / sample_rate)
		++range_exp;
	--range_exp;
	range_bits = range_exp;

	int range = 1<<range_exp;
	divisor = clock_freq / range / sample_rate; //now get int part of divisor
	if(divisor < min_divisor ) return -E_INVALID_PARAM; //sample rate too high

	const int frac_mult = PWM_MAX_DIVISOR_FRAC + 1;
	int divisor_frac = (int)(((long long)(clock_freq - divisor*range*
				sample_rate)*frac_mult) / range / sample_rate);
	
	setPWMRange(range, channels);
	setPWMClockDivisor(divisor, divisor_frac, false);
	clearPWMFifo();
	setPWMChannels(channels);

	audio_volume = MAX_AUDIO_VOLUME;
	applied_sample_rate = sample_rate;

	printk_d("audio: init sample_rate=%i, PWM divisor=%i 0x%x/0xfff, range=2^%i\n",
			sample_rate, divisor, divisor_frac, range_exp);

	return SUCCESS;
}

/*
 * adjust a sample to output volume
 * sample: max 16 bits
 */
static inline uint32 applyVolume(uint32 sample) {
	return (sample * audio_volume) / MAX_AUDIO_VOLUME;
}


int playAudio(const wave_hdr* wave, const char* buffer, uint buffer_len) {
	if(range_bits == 0) return -E_NOT_INIT;

	printk_d("audio: playing file: bits/sample=%i, channels=%i, sample_rate=%i\n",
			wave->bits_per_sample, wave->channels, wave->samples_per_sec);

	if(wave->channels > 2) return -E_UNSUPPORTED;


	switch(wave->bits_per_sample) {
	case 16: //signed 16 bit little endian data (s16le interleaved)
		{
			int16* audio_samples16 = (int16*)buffer;
			const uint range = 16;
			for(int i=0; i<buffer_len/(sizeof(int16)/sizeof(char)); ++i) {
				uint32 value = (uint32)((int32)audio_samples16[i] +(1<<15));
				for(int k=0; k <= 2-wave->channels; ++k) {
					while(isPWMFifoFull());
					if(range_bits > range)
						PWMWriteToFifo(applyVolume(value << (range_bits-range)));
					else
						PWMWriteToFifo(applyVolume(value >> (range-range_bits)));
				}
			}
		}
		break;
	case 8: //unsigned 8 bit data (u8 interleaved)
		{
			uint8* audio_samples8 = (uint8*)buffer;
			const uint range = 8;
			for(int i=0; i<buffer_len/(sizeof(uint8)/sizeof(char)); ++i) {
				uint32 value = (uint32)(audio_samples8[i]);
				for(int k=0; k <= 2-wave->channels; ++k) {
					while(isPWMFifoFull());
					if(range_bits > range)
						PWMWriteToFifo(applyVolume(value << (range_bits-range)));
					else
						PWMWriteToFifo(applyVolume(value >> (range-range_bits)));
				}
			}
		}
		break;
	default:
		return -E_FORMAT;
	}
	return SUCCESS;
}


float playSine(int frequency, int duration_ms, float init_angle) {
	uint start = getTimestamp();
	uint duration=duration_ms*1000;
	const int channels = 2;
	float t = init_angle;
	float d = 2.*PI*(float)frequency / (float) applied_sample_rate;
	while((getTimestamp() - start) < duration) {
		uint32 value = (uint32)((sine(t)+1.)/2.*(float)(1<<range_bits));
		for(int k=0; k < channels; ++k) {
			while(isPWMFifoFull());
			PWMWriteToFifo(applyVolume(value));
		}
		t+=d;
		if(t > 2.*PI) t-=2.*PI;
	}

	return t;
}


void setAudioVolume(uint32 volume) {
	if(volume > MAX_AUDIO_VOLUME) volume=MAX_AUDIO_VOLUME;
	audio_volume = volume;
}

uint32 getAudioVolume() {
	return audio_volume;
}
