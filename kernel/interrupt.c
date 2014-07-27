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

/*
 * here are the device interrupt handlers
 */

#include <kernel/interrupt.h>
#include <kernel/types.h>
#include <kernel/timer.h>
#include <kernel/printk.h>
#include <kernel/gpio.h>

volatile uint g_irq_gpio_high_counter[GPIO_COUNT];
volatile Timestamp g_irq_gpio_high_last_timestamp[GPIO_COUNT];
volatile uint g_irq_gpio_low_counter[GPIO_COUNT];
volatile Timestamp g_irq_gpio_low_last_timestamp[GPIO_COUNT];

static uint timer_irq_counter = 0;
static uint interrupts_enabled = 1; //0 means they are enabled

#define MAX_GPIO_IRQ_EVENT_HANDLERS 3
static GpioIrqEventHandler gpio_irq_event_handlers[MAX_GPIO_IRQ_EVENT_HANDLERS];
static int gpio_irq_event_handler_count = 0;


void handleTimerIRQ() {
	archHandleTimerIRQ();
	
	//printk_i("Got a timer interrupt\n");
	++timer_irq_counter;
	
	//disableTimerIRQ();
}

uint getTimerIRQCounter() {
	return timer_irq_counter;
}

void resetTimerIRQCounter() {
	timer_irq_counter = 0;
}

void handleGpioIRQ() {
	archHandleGpioIRQ();
	
}
void handleGpioIRQPin(int pin, int value) {
	if(value) {
		++g_irq_gpio_high_counter[pin];
		g_irq_gpio_high_last_timestamp[pin] = getTimestamp();
	} else {
		++g_irq_gpio_low_counter[pin];
		g_irq_gpio_low_last_timestamp[pin] = getTimestamp();
	}
	//call event handlers
	for(int i=0; i<gpio_irq_event_handler_count; ++i) {
		(*gpio_irq_event_handlers[i])(pin, value);
	}
}

void enableInterrupts() {
	if(inInterrupt()) 
		return; //inside IRQ handler, interrupts are always disabled

	if(interrupts_enabled > 0) {
		if(--interrupts_enabled == 0) __enableInterrupts();
	}
}

void disableInterrupts() {
	if(inInterrupt()) 
		return; //inside IRQ handler, interrupts are always disabled

	if(++interrupts_enabled == 1)
		__disableInterrupts();
}

int registerGpioIrqEventHandler(GpioIrqEventHandler handler) {
	if(gpio_irq_event_handler_count >= MAX_GPIO_IRQ_EVENT_HANDLERS)
		return -E_BUFFER_FULL;
	gpio_irq_event_handlers[gpio_irq_event_handler_count++] = handler;
	return 0;
}
