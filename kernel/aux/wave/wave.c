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

#include "wave.h"

#include <kernel/endian.h>
#include <kernel/errors.h>
#include <kernel/string.h>


int check_hdr(const riff_hdr* riff_header) {

	const char* riff_id = "RIFF";
	const char* wave_id = "WAVE";
	for(int i=0; i<4; ++i) {
		if(riff_id[i] != riff_header->Riff[i]
				|| wave_id[i] != riff_header->Wave[i]) {
			return -E_FORMAT;
		}
	}

    return SUCCESS;
}





int readWave(const riff_hdr* header, wave_hdr** wave, char** audio_data) {
	if(check_hdr(header) == SUCCESS) {
		char* buffer = (char*)header;
		buffer += sizeof(riff_hdr);
		riff_tag* fmt_header = (riff_tag*)buffer;

		const char fmt[]="fmt ";
		if(strncmp(fmt, (char*)fmt_header->tag, 4)==0) {
			buffer+=sizeof(riff_tag);
			*wave = (wave_hdr*)buffer;
			//convert endian of wave header
			(*wave)->format_tag = LE16((*wave)->format_tag);
			(*wave)->channels = LE16((*wave)->channels);
			(*wave)->samples_per_sec = LE32((*wave)->samples_per_sec);
			(*wave)->avg_bytes_per_sec = LE32((*wave)->avg_bytes_per_sec);
			(*wave)->block_align = LE16((*wave)->block_align);
			(*wave)->bits_per_sample = LE16((*wave)->bits_per_sample);
			buffer+=fmt_header->length; //usually: sizeof(wave_hdr)
			riff_tag* data_header = (riff_tag*)buffer;
			const char data[]="data";
			if(strncmp(data, (char*)data_header->tag, 4)==0) {
				buffer+=sizeof(riff_tag);
				*audio_data = buffer;
				return LE32(data_header->length);
			}
		}
	}
	return -E_FORMAT;
}
