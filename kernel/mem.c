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
#include <kernel/errors.h>
#include <kernel/utils.h>
#include <kernel/panic.h>
#include <kernel/malloc.h>


static mem_region mem_regions[MAX_MEM_REGIONS];
static int mem_region_count = 0;


mem_region allocatable_mem_regions[MAX_MEM_REGIONS];
int allocatable_mem_region_count;


int addMemoryRegion(const mem_region* reg) {
	if(mem_region_count >= MAX_MEM_REGIONS)
		return -E_BUFFER_FULL;

	memcpy(mem_regions + mem_region_count, reg, sizeof(mem_region));
	++mem_region_count;

	return SUCCESS;
}


extern char __kernel_start_addr;
extern char __kernel_end_addr;
extern char __estack;

void initKernelMemRegions() {
	
	mem_region r;
	r.type = mem_region_type_kernel;

	char* kernel_start = &__kernel_start_addr;
	char* kernel_end   = &__kernel_end_addr;
	char* stack_start  = &__estack;

	/* protect the lower memory addresses */
	r.start = 0;
	r.size = 0x400;
	addMemoryRegion(&r);

	/* kernel binary */
	r.start = (uint)kernel_start;
	r.size = (uint)kernel_end - (uint)kernel_start;
	addMemoryRegion(&r);

	/* stack */
	r.start = (uint)stack_start - MAX_STACK_SIZE;
	if((uint)stack_start < MAX_STACK_SIZE) r.start = 0;
	r.size = MAX_STACK_SIZE;
	addMemoryRegion(&r);
	
}


void finalizeMemoryRegions() {

	allocatable_mem_region_count = 0;

	/* copy all normal (allocatable) regions */
	for(int i=0; i<mem_region_count; ++i) {
		if(mem_regions[i].type == mem_region_type_normal) {
			memcpy(allocatable_mem_regions+allocatable_mem_region_count, 
					mem_regions+i, sizeof(mem_region));
			++allocatable_mem_region_count;
		}
	}

	/* exclude all non-allocatable regions */
	for(int i=0; i<mem_region_count; ++i) {
		if(mem_regions[i].type != mem_region_type_normal) {
			for(int k=0; k<allocatable_mem_region_count; ++k) {
				mem_region* a_reg = allocatable_mem_regions+k;
				if(a_reg->start >= mem_regions[i].start &&
					a_reg->start < mem_regions[i].start+mem_regions[i].size) {
					if(a_reg->start + a_reg->size <= mem_regions[i].start+
							mem_regions[i].size) { /* remove this region */
						--k;
						--allocatable_mem_region_count;
						memcpy(a_reg, allocatable_mem_regions+
							allocatable_mem_region_count, sizeof(mem_region));
					} else { /* resize region */
						a_reg->size -= mem_regions[i].start+mem_regions[i].size 
							- a_reg->start;
						a_reg->start = mem_regions[i].start+mem_regions[i].size;
					}
				} else if(a_reg->start < mem_regions[i].start &&
						a_reg->start + a_reg->size > mem_regions[i].start) {
					if(a_reg->start + a_reg->size <= mem_regions[i].start + 
							mem_regions[i].size) { /* resize region */
						a_reg->size = mem_regions[i].start - a_reg->start;
					} else { /* split region */
						if(allocatable_mem_region_count < MAX_MEM_REGIONS) {
							mem_region* r = allocatable_mem_regions
								+allocatable_mem_region_count;
							r->type = a_reg->type;
							r->start = mem_regions[i].start+mem_regions[i].size;
							r->size = a_reg->start+a_reg->size - r->start;
							++allocatable_mem_region_count;
						}
						a_reg->size = mem_regions[i].start - a_reg->start;
					}
				}
			}
		}
	}
	
	//FIXME: handle overlapping regions ?

	if(allocatable_mem_region_count == 0) 
		panic("no allocatable memory regions!");


	/* print summary */
	printk("Memory Regions:\n");
	int idx=0;
	for(int i=0; i<mem_region_count; ++i) {
		mem_region* r = mem_regions+i;
		switch(r->type) {
		case mem_region_type_reserved:
			printk(" %i: (reserved) start=0x%08x, end=0x%08x, size=0x%08x bytes\n", 
					idx++, r->start, r->start+r->size, r->size);
			break;
		case mem_region_type_kernel:
			printk(" %i: (kernel)   start=0x%08x, end=0x%08x, size=0x%08x bytes\n", 
					idx++, r->start, r->start+r->size, r->size);
			break;
		case mem_region_type_io_dev:
			printk(" %i: (IO dev)   start=0x%08x, end=0x%08x, size=0x%08x bytes\n", 
					idx++, r->start, r->start+r->size, r->size);
			break;
		case mem_region_type_normal: /* do not print: handled below */
			break;
		}
	}
	uint tot_size = 0;
	for(int i=0; i<allocatable_mem_region_count; ++i) {
		mem_region* r = allocatable_mem_regions+i;
		tot_size += r->size;
		printk(" %i: (alloc)    start=0x%08x, end=0x%08x, size=0x%08x bytes\n", 
				idx, r->start, r->start+r->size, r->size);
		++idx;
	}
	printk(" Total allocatable: %R\n", tot_size);


	initMalloc();
}


