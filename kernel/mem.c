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
#include <kernel/mmu.h>


mem_region mem_regions[MAX_MEM_REGIONS];
int mem_region_count = 0;


mem_region allocatable_mem_regions[MAX_MEM_REGIONS];
int allocatable_mem_region_count;


static int handleNewRegion(int mem_region_idx, int alloc_mem_region_idx) {

	mem_region* a_reg = allocatable_mem_regions+alloc_mem_region_idx;
	mem_region* r = mem_regions+mem_region_idx;
	int ret = 0;
	if(a_reg->start >= r->start && a_reg->start < r->start+r->size) {
		if(a_reg->start + a_reg->size <= r->start + r->size) { /* remove this region */
			--allocatable_mem_region_count;
			memcpy(a_reg, allocatable_mem_regions+
					allocatable_mem_region_count, sizeof(mem_region));
		} else { /* resize region */
			a_reg->size -= r->start+r->size - a_reg->start;
			a_reg->start = r->start+r->size;
		}
		ret = 1;
	} else if(a_reg->start < r->start && a_reg->start + a_reg->size > r->start) {
		if(a_reg->start + a_reg->size <= r->start + r->size) { /* resize region */
			a_reg->size = r->start - a_reg->start;
		} else { /* split region */
			if(allocatable_mem_region_count < MAX_MEM_REGIONS) {
				mem_region* new_reg = allocatable_mem_regions
					+ allocatable_mem_region_count;
				new_reg->type = a_reg->type;
				new_reg->start = r->start+r->size;
				new_reg->size = a_reg->start+a_reg->size - new_reg->start;
				++allocatable_mem_region_count;
			}
			a_reg->size = r->start - a_reg->start;
		}
		ret = 1;
	}
	return ret;
}

inline static void alignRegionToPageSize(mem_region* r) {
#ifdef HAS_MMU
		// align region to PAGE_SIZE
		ulong aligned = align_ptr(r->start, PAGE_SIZE);
		if(aligned != r->start) {
			aligned -= PAGE_SIZE;
			r->size += r->start - aligned;
			r->start = aligned; 
		}
		r->size = align_ptr(r->size, PAGE_SIZE);
#endif
}

static void checkRegionsOverlapping(int reg_idx) {
	mem_region* r = mem_regions + reg_idx;
	for(int i=0; i<mem_region_count; ++i) {
		mem_region* a_reg = mem_regions + i;
		if(reg_idx != i) {
			if(a_reg->start >= r->start && a_reg->start < r->start+r->size) {
				if(a_reg->start + a_reg->size <= r->start + r->size) { /* remove */
					--mem_region_count;
					memcpy(a_reg, mem_regions +
							mem_region_count, sizeof(mem_region));
					--i;
				} else { /* resize region */
					a_reg->size -= r->start+r->size - a_reg->start;
					a_reg->start = r->start+r->size;
				}
			} else if(a_reg->start < r->start &&
					a_reg->start + a_reg->size > r->start) {
				if(a_reg->start + a_reg->size <= r->start + r->size) { /* resize */
					a_reg->size = r->start - a_reg->start;
				} else { /* remove r */
					--mem_region_count;
					memcpy(r, mem_regions +
							mem_region_count, sizeof(mem_region));
					return;
				}
			}
		}
	}
}

int addMemoryRegion(const mem_region* reg) {
	int ret;
	if(reg->type == mem_region_type_normal) { //allocatable region
		if(allocatable_mem_region_count >= MAX_MEM_REGIONS)
			return -E_BUFFER_FULL;

		memcpy(allocatable_mem_regions + allocatable_mem_region_count, reg,
				sizeof(mem_region));
		ret = allocatable_mem_region_count++;
		for(int k=allocatable_mem_region_count-1;
				k<allocatable_mem_region_count; ++k) {
			for(int i=0; i<mem_region_count; ++i)
				if(handleNewRegion(i, k)) --i;
		}

	} else {
		if(mem_region_count >= MAX_MEM_REGIONS)
			return -E_BUFFER_FULL;

		mem_region* r = mem_regions + mem_region_count;
		memcpy(r, reg, sizeof(mem_region));
		ret = mem_region_count++;

		alignRegionToPageSize(r);
		checkRegionsOverlapping(ret);

		for(int i=mem_region_count-1; i<mem_region_count; ++i) {
			for(int k=0; k<allocatable_mem_region_count; ++k)
				if(handleNewRegion(i, k)) --k;
		}

	}
	return ret;
}


extern char __kernel_start_addr;
extern char __kernel_end_addr;
extern char __estack; /* end of the stack */

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
	r.start = (ulong)kernel_start;
	r.size = (ulong)kernel_end - (ulong)kernel_start;
	addMemoryRegion(&r);

	/* stack */
	r.start = (ulong)stack_start - MAX_STACK_SIZE;
	if((ulong)stack_start < MAX_STACK_SIZE) r.start = 0;
	r.size = MAX_STACK_SIZE;
	addMemoryRegion(&r);
	
}



void printMemRegions() {
	printk_i("Memory Regions:\n");
	uint tot_size = 0; //FIXME: use ulong -> printk format
	int idx=0;
	for(int i=0; i<mem_region_count; ++i) {
		mem_region* r = mem_regions+i;
		const char* type = NULL;
		switch(r->type) {
		case mem_region_type_reserved:
			type = "(reserved)"; break;
		case mem_region_type_kernel:
			type = "(kernel)  "; break;
		case mem_region_type_io_dev:
			type = "(IO dev)  "; break;
		case mem_region_type_page_table:
			type = "(page tbl)"; break;
		case mem_region_type_malloc:
			type = "(malloc)  ";
			tot_size += r->size; break;
		case mem_region_type_normal: /* do not print: handled below */
			break;
		}
		if(type) 
			printk_i(" %02i: %s start=0x%08x, end=0x%08x, size=0x%08x bytes\n", 
					idx++, type, r->start, r->start+r->size, r->size);
	}
	for(int i=0; i<allocatable_mem_region_count; ++i) {
		mem_region* r = allocatable_mem_regions+i;
		tot_size += r->size;
		printk_i(" %02i: (alloc)    start=0x%08x, end=0x%08x, size=0x%08x bytes\n", 
				idx, r->start, r->start+r->size, r->size);
		++idx;
	}
	printk_i(" Total (m)allocatable: %R\n", tot_size);

}


const mem_region* getPhysicalRegion(ulong size, ulong alignment,
		mem_region_type type) {
	for(int i=0; i<allocatable_mem_region_count; ++i) {
		mem_region* alloc_reg = allocatable_mem_regions + i;
		ulong align_start = align_ptr(alloc_reg->start, alignment);
		if(alloc_reg->size >= align_start - alloc_reg->start + size) {
			mem_region reg;
			reg.start = align_start;
			reg.size = size;
			reg.type = type;
			int idx = addMemoryRegion(&reg);
			return idx < 0 ? NULL : mem_regions+idx;
		}
	}
	return NULL;
}

const mem_region* getMaxPhysicalRegion(mem_region_type type) {
	if(allocatable_mem_region_count == 0) return NULL;

	mem_region* max_region=allocatable_mem_regions;
	for(int i=1; i<allocatable_mem_region_count; ++i) {
		mem_region* r = allocatable_mem_regions+i;
		if(r->size > max_region->size) max_region = r;
	}
	max_region->type = type;

	int idx_ret = addMemoryRegion(max_region);
	return idx_ret < 0 ? NULL : mem_regions + idx_ret;
}


ulong getMaxPhysicalAddress() {
	ulong max_addr = 0;
	for(int i=0; i<allocatable_mem_region_count; ++i) {
		ulong end_addr = allocatable_mem_regions[i].start + 
			allocatable_mem_regions[i].size;
		if(end_addr > max_addr) max_addr = end_addr;
	}
	for(int i=0; i<mem_region_count; ++i) {
		ulong end_addr = mem_regions[i].start + mem_regions[i].size;
		if(end_addr > max_addr) max_addr = end_addr;
	}

	return max_addr;
}

int getMaxStackSize() {
	return MAX_STACK_SIZE;
}

int getCurrentStackSize() {
	int dummy;
	char* stack_start  = &__estack;
	/* FIXME: this assumes no virtual memory is used: virtual addr == phys addr */
	char* stack_current = (char*)&dummy;
	return (int)(stack_start - stack_current);
}

