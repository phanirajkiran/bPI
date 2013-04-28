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

#include "math.h"

float sine(float x) {
	//always wrap input angle to -PI..PI
	while(x < -PI) x += 2.*PI;
	while(x >  PI) x -= 2.*PI;

	if (x < 0.)
		return (4./PI) * x + (4./(PI*PI)) * x * x;
	return (4./PI) * x - (4./(PI*PI)) * x * x;
}


