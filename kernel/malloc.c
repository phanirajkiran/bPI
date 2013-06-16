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
#include <kernel/errors.h>
#include <kernel/printk.h>


static ulong next_free_mem=(ulong)NULL;

static const mem_region* malloc_region;


void* kmalloc(size_t num) {
	//this is not worthy to be called a malloc, but here we go...
	//TODO: use something more sophisticated
	
	if(next_free_mem + num > malloc_region->start + malloc_region->size) {
		return NULL;
	}
	void* ret = (void*)next_free_mem;
	next_free_mem += num;
	return ret;
}

void kfree(void* ptr) {
	//TODO: free memory
}


int initMalloc() {

	malloc_region = getMaxPhysicalRegion(mem_region_type_malloc);
	if(!malloc_region) return -E_OUT_OF_MEMORY;
	next_free_mem = malloc_region->start;

	return SUCCESS;
}

