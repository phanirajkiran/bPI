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

#ifndef AUX_WAVE_HEADER_H_
#define AUX_WAVE_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <kernel/types.h>

typedef struct {
    uint8 Riff[4]; /* "RIFF" magic */
    uint32 Size;    /* size of entire file in bytes minus 8 bytes (Riff & Size fields )*/
    uint8 Wave[4]; /* "WAVE" magic */
} riff_hdr;

typedef struct {
    uint8 tag[4]; /* for wave: either "fmt " (format info) or "data" (audio data) */
    uint32 length; /* number of following bytes in this tag */
} riff_tag;



typedef struct {
    uint16 format_tag; // 1==linear quantization (PCM)
    uint16 channels; //number of channels
    uint32 samples_per_sec; //sample rate (eg 44100...)
    uint32 avg_bytes_per_sec; //==samples_per_sec * channels * bits_per_sample / 8
    uint16 block_align; //==channels * bits_per_sample / 8
    uint16 bits_per_sample; //1 sample == 1 channel 
					//be careful: 8 means unsigned, 16 means signed data!
} wave_hdr;


/* returns the size of audio_data in bytes or <0 for error */

int readWave(const riff_hdr* header, wave_hdr** wave, char** audio_data);


#ifdef __cplusplus
}
#endif
#endif /* AUX_WAVE_HEADER_H_ */
