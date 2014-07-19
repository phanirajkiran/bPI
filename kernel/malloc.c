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


/* memory malloc & free. the implementation assumes the MMU (paging) is disabled
 * so no page management is done here.
 */

/* we use the malloc implementation from FreeRTOS: it manages a fixed-size (defined
 * after startup), contiguous chunk of memory.
 */
void *pvPortMalloc( size_t xSize );
void vPortFree( void *pv );
void vPortInitialiseBlocks( void* start, size_t size );
size_t xPortGetFreeHeapSize( void );

static const mem_region* malloc_region;


void* kmalloc(size_t num) {
	void* ptr = pvPortMalloc(num);
	//FIXME: do better NULL-pointer handling
	if(!ptr) printk_w("WARNING: malloc returned a NULL-pointer!\n");
	return ptr;
}

void kfree(void* ptr) {
	vPortFree(ptr);
}

size_t kfreeMallocSpace() {
	return xPortGetFreeHeapSize();
}

size_t ktotalMallocSpace() {
	return malloc_region->size;
}

int initMalloc() {

	//FIXME: use all available regions (possibly non-contiguous...)
	malloc_region = getMaxPhysicalRegion(mem_region_type_malloc);
	if(!malloc_region) return -E_OUT_OF_MEMORY;
	
	vPortInitialiseBlocks((void*)malloc_region->start, (size_t)malloc_region->size);
	
	return SUCCESS;
}

