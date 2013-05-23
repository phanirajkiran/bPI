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

#include "mem.h"
#include "common.h"

#include <kernel/mem.h>

void initDeviceMemRegions() {
	
	mem_region region;
	region.type = mem_region_type_io_dev;
	region.start = BCM2835_PERI_BASE;
	region.size = BCM2835_PERI_END-BCM2835_PERI_BASE;
	addMemoryRegion(&region);
}


