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
#include <kernel/interrupt.h>

#include <kernel/aux/wave/wave.h>
#include <bcm2835/pwm.h>
#include <bcm2835/audio.h>


extern int __bss_start;
extern int __bss_end;

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



	setNextTimerIRQ(300);
	
	enableTimerIRQ();
	enableInterrupts();


	while(1) {
		const int delay_ms = 1000;
		printk("got %i timer irqs in %i ms\n", getTimerIRQCounter(), delay_ms);
		resetTimerIRQCounter();
		delay(delay_ms);
	}

	
	/* play a sine */
	/*
	initAudio(44100);
	setAudioVolume(MAX_AUDIO_VOLUME);

	float angle=0.;
	while(1) {
		int start_freq = 30;
		int end_freq = 800;
		for(int i=start_freq; i<end_freq; i+=1)
			angle=playSine(i, 10, angle);
		for(int i=end_freq; i>start_freq; i-=1)
			angle=playSine(i, 10, angle);
	}
	*/


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



