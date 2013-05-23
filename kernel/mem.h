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
 * this file handles usable & unusable memory regions (pysical addresses).
 * these will be used for kmalloc, so before calling kmalloc, setup the memory
 * regions.
 */

#ifndef MEM_HEADER_H_
#define MEM_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <kernel/types.h>
#include <mem_arch.h>

typedef enum {
	mem_region_type_normal = 0, /* normal memory: usable for malloc */
	mem_region_type_reserved, /* cannot be used for allocation */
	mem_region_type_kernel, /* used by the kernel: stack or kernel image */
	mem_region_type_io_dev /* IO device regions */
} mem_region_type;

typedef struct {
	uint start; /* region start in bytes */
	uint size;
	mem_region_type type;
} mem_region;


#define MAX_MEM_REGIONS 20
extern mem_region allocatable_mem_regions[MAX_MEM_REGIONS];
extern int allocatable_mem_region_count;


/* add a memory region. NORMAL regions must not overlap */
int addMemoryRegion(const mem_region* reg);

/* add memory regions occupied by the kernel: code & stacks */
void initKernelMemRegions();

/* call this after adding all memory regions */
void finalizeMemoryRegions();


#ifdef __cplusplus
}
#endif
#endif /* MEM_HEADER_H_ */

