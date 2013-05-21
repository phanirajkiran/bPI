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

#include "malloc.h"
#include <kernel/mem.h>
#include <kernel/utils.h>


static int cur_mem_region = 0;
static ulong next_free_mem=NULL;

void* kmalloc(size_t num) {
	//this is not worthy to be called a malloc, but here we go...
	
	mem_region* reg = allocatable_mem_regions+cur_mem_region;
	while(next_free_mem + num > reg->start + reg->size) {
		//we need the next region
		if(cur_mem_region + 1 == allocatable_mem_region_count)
			return NULL; //no free memory left
		reg = allocatable_mem_regions + ++cur_mem_region;
		next_free_mem = reg->start;
	}
	void* ret = (void*)next_free_mem;
	next_free_mem += num;
	return ret;
}

void kfree(void* ptr) {
	//TODO
}


void initMalloc() {
	next_free_mem = allocatable_mem_regions[cur_mem_region].start;
	if(next_free_mem == 0) next_free_mem = 0x10;
}

