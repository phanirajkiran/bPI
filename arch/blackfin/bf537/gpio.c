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

#include <kernel/gpio.h>
#include <kernel/registers.h>


#define PORTF_FER			0xFFC03200	/* Port F Function Enable Register (Alternate/Flag*) */
#define PORTG_FER			0xFFC03204	/* Port G Function Enable Register (Alternate/Flag*) */
#define PORTH_FER			0xFFC03208	/* Port H Function Enable Register (Alternate/Flag*) */
#define BFIN_PORT_MUX			0xFFC0320C	/* Port Multiplexer Control Register */

#define PORTFIO					0xFFC00700	/* Port F I/O Pin State Specify Register */
#define PORTGIO					0xFFC01500	/* Port G I/O Pin State Specify Register */
#define PORTHIO					0xFFC01700	/* Port H I/O Pin State Specify Register */


typedef struct {
	unsigned short data;
	unsigned short dummy1;
	unsigned short data_clear;
	unsigned short dummy2;
	unsigned short data_set;
	unsigned short dummy3;
	unsigned short toggle;
	unsigned short dummy4;
	unsigned short maska;
	unsigned short dummy5;
	unsigned short maska_clear;
	unsigned short dummy6;
	unsigned short maska_set;
	unsigned short dummy7;
	unsigned short maska_toggle;
	unsigned short dummy8;
	unsigned short maskb;
	unsigned short dummy9;
	unsigned short maskb_clear;
	unsigned short dummy10;
	unsigned short maskb_set;
	unsigned short dummy11;
	unsigned short maskb_toggle;
	unsigned short dummy12;
	unsigned short dir;
	unsigned short dummy13;
	unsigned short polar;
	unsigned short dummy14;
	unsigned short edge;
	unsigned short dummy15;
	unsigned short both;
	unsigned short dummy16;
	unsigned short inen;
} gpio_port_t;

static gpio_port_t* const gpio_array[] = {
	(gpio_port_t *) PORTFIO,
	(gpio_port_t *) PORTGIO,
	(gpio_port_t *) PORTHIO,
};


static unsigned short * const port_fer[] = {
	(unsigned short *) PORTF_FER,
	(unsigned short *) PORTG_FER,
	(unsigned short *) PORTH_FER,
};

/*
 * set which alternative function to use
 * bit:
 * 4: value=1: enable UART1
 *    value=0: enable TMR6, TMR7
 *
 * see Hardware Reference Manual p868
 */
void setGpioAltFunction(int bit, int value) {
	regRMW16(BFIN_PORT_MUX, bit, value);
}


/*
 * funcion: 0 for input, 1 for output, 2 for alternative function
 * for alternative function: first call setGpioAltFunction
 */
void setGpioFunction(int pin, int function) {
	int port = pin >> 4;
	pin = pin & 0xf;
	switch(function) {
	case 0: //input
		regRMW16(port_fer[port], pin, 0); //disable alternative func
		regRMW16(&(gpio_array[port]->dir), pin, 0); //set as input
		regRMW16(&(gpio_array[port]->inen), pin, 1); //enable input
		break;
	case 1: //output
		regRMW16(port_fer[port], pin, 0); //disable alternative func
		regRMW16(&(gpio_array[port]->inen), pin, 0); //disable input
		regRMW16(&(gpio_array[port]->dir), pin, 1); //set as output
		break;
	case 2: //alternative function
		regRMW16(&(gpio_array[port]->inen), pin, 0); //disable input
		regRMW16(port_fer[port], pin, 1); //enable alternative func
		break;
	}
}
void setGpio(int pin, int value) {
	int port = pin >> 4;
	pin = pin & 0xf;
	if(value) regWrite16(&(gpio_array[port]->data_set), 1<<pin);
	else regWrite16(&(gpio_array[port]->data_clear), 1<<pin);
}

int getGpio(int pin) {
	int port = pin / 16;
	pin = pin % 16;
	return regRead16Bit(&(gpio_array[port]->data), pin);
}

