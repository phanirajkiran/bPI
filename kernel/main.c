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

/*!
 * high level main kernel entry (called right after arch-specific _start
 * function).
 */

#include <kernel/utils.h>
#include <arch.h>

#include <kernel/led.h>
#include <kernel/timer.h>

#include <kernel/gpio.h>
#include <kernel/registers.h>
#include <kernel/serial.h>
#include <kernel/math.h>

#include <kernel/aux/wave/wave.h>
#include <bcm2835/pwm.h>
#include <bcm2835/audio.h>


extern int __bss_start;
extern int __bss_end;

extern int audio_data;


void initZeroMemory() {
	/* init the .bss section to zero (the compiler assumes all memory in bss
	 * will be initialized to zero, so we have to do it manually here) 
	 *
	 * this must be called as early as possible.
	 *
	 * we assume the bss start & end is aligned to at least size of int
	 */
	for(int* i=&__bss_start; i!=&__bss_end; ++i)
		*i = 0;
}



void kernelMain() {
	kernel_cmd_line[0] = 0;

	initArch();

	printk("\n++++++++++++++++++++++++\n");
	printk(  "  Welcome to Banana Pi  \n");
	printk(  "++++++++++++++++++++++++\n\n");

	if(*kernel_cmd_line) {
		printk("Kernel Command Line: %s\n", kernel_cmd_line);
	}


	/* play an audio file */
	wave_hdr* wave;
	char* audio_samples;
	int buffer_len = readWave((riff_hdr*)&audio_data, &wave, &audio_samples);
	if(buffer_len > 0) {
		if(wave->format_tag == 1) {
			printk("audio file: channels=%i, samples/sec=%i, len=%i bytes\n",
					wave->channels, wave->samples_per_sec, buffer_len);
			initAudio(wave->samples_per_sec);
			setAudioVolume(MAX_AUDIO_VOLUME);

			if(playAudio(wave, audio_samples, buffer_len) != 0) {
				printk("playAudio call failed!\n");
			}
		} else {
			printk("audio file: unrecognized format: %i\n", wave->format_tag);
		}
	} else {
		printk("failed to read audio file (%i)!\n", buffer_len);
	}



	/* main loop */

	int c=0;
	while(1) {
		//echo back
		c = uartRead();
		if(c>=0) uartWrite(c);

		//uartWrite((c++)&0xff);
		//udelay(300000);
		toggleLed(0);
	}

}



