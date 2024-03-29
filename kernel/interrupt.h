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

#ifndef INTERRUPT_HEADER_H_
#define INTERRUPT_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <kernel/utils.h>
#include <kernel/timer.h>
#include <interrupt_arch.h>

/*
 * enable all interrupts. does not enable any specific device IRQ's, but IRQ's
 * in general. can be nested.
 */
void enableInterrupts();
void disableInterrupts();


extern volatile uint g_irq_gpio_high_counter[]; //high is the value after it was read
extern volatile Timestamp g_irq_gpio_high_last_timestamp[];
extern volatile uint g_irq_gpio_low_counter[];
extern volatile Timestamp g_irq_gpio_low_last_timestamp[];

/** gpio IRQ callback handler */
typedef void(*GpioIrqEventHandler)(int pin, int value);


#ifdef ARCH_HAS_INTERRUPT

/*
 * test whether we are inside an interrupt handler.
 * return: 0 if not inside an interrupt handler
 */
uint inInterrupt();

void __enableInterrupts();
void __disableInterrupts();

void handleTimerIRQ();
void handleGpioIRQ();

/**
 * register a callback handler to process gpio IRQ events.
 * handler will be called in IRQ context!
 * @return 0 on success, <0 error otherwise
 */
int registerGpioIrqEventHandler(GpioIrqEventHandler handler);

/**
 * handle an IRQ for a GPIO pin. called from arch specific gpio IRQ handler
 * (inside IRQ context)
 */
void handleGpioIRQPin(int pin, int value);

#else
# define __enableInterrupts() NOP
# define __disableInterrupts() NOP
# define archHandleTimerIRQ() NOP
# define archHandleGpioIRQ() NOP
# define inInterrupt() 0
#endif


#ifdef __cplusplus
}
#endif
#endif /* INTERRUPT_HEADER_H_ */



