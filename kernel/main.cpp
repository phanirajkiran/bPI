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
#include <kernel/malloc.h>

#include <kernel/aux/command_line.hpp>

#include <kernel/aux/wave/wave.h>
#include <bcm2835/pwm.h>
#include <bcm2835/audio.h>

#include <kernel/compiler/icxxabi.h>



extern "C" /* we need this so that kmain can be called from C/asm */
void kmain() {

	kernel_cmd_line[0] = 0;
	initArch();


	setNextTimerIRQ(300);
	
	//enableTimerIRQ();
	enableInterrupts();


	/*
	while(1) {
		const int delay_ms = 1000;
		printk("got %i timer irqs in %i ms\n", getTimerIRQCounter(), delay_ms);
		resetTimerIRQCounter();
		toggleLed(0);
		delay(delay_ms);
	}
	*/
	
	/* test command line */
	auto uart_writef = [](int c) { if(c=='\n') uartWrite('\r'); uartWrite(c); return 0; };
	InputOutput io(uartTryRead, uart_writef);
	CommandLine cmd_line(io, "$bPI> ");
	int counter = 0;
	while(1) {
		cmd_line.handleData();
		udelay(5000);
		if(++counter == 20) {
			toggleLed(0);
			counter = 0;
		}
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
		if(c>=0) {
			uartWrite(c);
			if(c=='\r') uartWrite('\n');
		}

		//uartWrite((c++)&0xff);
		//delay(300);
		toggleLed(0);
	}

	/* call all static descructors */
	__cxa_finalize(0);
}


