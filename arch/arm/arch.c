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

#include <arch.h>
#include <init_board.h>
#include "atag.h"
#include "interrupt_arch.h"

#include <kernel/mem.h>


void initArch() {
	if(readATAG()) {
		/* reading ATAG failed. this means we have no memory regions. we just
		 * try to add a small one 
		 */
		mem_region region;
		region.type = MEM_REGION_TYPE_NORMAL;
		region.start = 0;
		region.size = 100 * 1024 * 2024;
		addMemoryRegion(&region);
	}
	initBoard();

	/* from now on printk should work */
	printATAG();

	initKernelMemRegions();
	finalizeMemoryRegions();

	/* from now on kmalloc is usable */

	archInitInterrupts();

}
