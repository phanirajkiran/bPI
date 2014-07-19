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

#include <kernel/types.h>
#include <kernel/utils.h>
#include <kernel/mmu.h>
#include <kernel/registers.h>
#include <kernel/errors.h>
#include <kernel/mem.h>
#include <kernel/panic.h>

#include <kernel/printk.h>


static mem_frame* frames;
static uint frame_count;
static uint used_frames; /* non-free pages (without reserved kernel frames) */

static uint next_free_frame;

/*
 * init all allocatable pysical frames. each frame has size PAGE_SIZE.
 * the space needed for the frames will be removed from the allocatable regions
 * & addMemoryRegion will be called for this region
 * this also aligns the allocatable regions to pages
 */
int initFrames() {

	mem_region* allocatable_reg=allocatable_mem_regions;
	int count = allocatable_mem_region_count;

	/* get the max allocatable memory address */
	ulong max_alloc_addr = 0;
	for(int i=0; i<count; ++i) {
		ulong end_addr = allocatable_reg[i].start+allocatable_reg[i].size;
		if(end_addr > max_alloc_addr) max_alloc_addr = end_addr;
	}

	/* get free space for frames */
	frame_count = getFrameIdx(max_alloc_addr) + 1;
	uint frame_mem_size = sizeof(mem_frame) * frame_count;
	const mem_region* frames_region = getPhysicalRegion(frame_mem_size, 1,
			mem_region_type_kernel);
	if(frames_region) {
		frames = (mem_frame*)frames_region->start;
	} else {
		frames = NULL;
		printk_e("Failed to find a memory region for frames\n");
		return -E_OUT_OF_MEMORY;
	}

	/* init frames */
	memset(frames, 0, sizeof(mem_frame)*frame_count);
	uint last_free_frame = -1;
	for(int i=0; i<count; ++i) {
		mem_region* r = allocatable_reg+i;
		uint start_idx = getFrameIdx(align_ptr(r->start, PAGE_SIZE));
		uint end_idx = getFrameIdx(align_ptr(r->start+r->size, PAGE_SIZE));
		for(uint k=0; k<end_idx; ++k) {
			if(last_free_frame == -1) next_free_frame = k;
			else frames[last_free_frame].next = k;
			last_free_frame = k;
		}
	}
	/* mark all non-free as reserved */
	for(int i=0; i<frame_count; ++i) {
		if(frames[i].next == 0) frames[i].next = 1<<(INT_BITS-1);
	}

	return SUCCESS;
}


void handlePageFault() {
	printk_i("got a page fault\n");
	while(1);
}

