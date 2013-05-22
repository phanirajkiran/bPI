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
 * method calls emmited by the compiler. never called by the kernel itself
 */

#include <kernel/panic.h>


/*
 * called when division by 0 happens. this should never happen, it's a serious
 * error
 */
void raise() {
	panic("Thou shalt not divide by 0!\n");
}


void abort() {
	panic("abort() called\n");
}


