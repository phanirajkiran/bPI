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
#include <kernel/errors.h>
#include <kernel/mem.h>
#include <kernel/panic.h>

#include <kernel/printk.h>


/* init second level entries */
static void initRegionsPageTableEntries(const mem_region* regions, int count, 
		uint32* page_table_start) {

	for(int i=0; i<count; ++i) {
		const mem_region* r = regions + i;
		uint start_page = getFrameIdx(r->start);
		uint end_page = getFrameIdx(r->start+r->size);
		uint32 page_flags;
		switch(r->type) {
			case mem_region_type_normal:
			case mem_region_type_reserved:
			case mem_region_type_kernel:
			case mem_region_type_malloc:
			default:
				page_flags = (0x1<<1) | (1<<3) /* C */ | (1<<2) /* B */ 
					| (3<<4) /* AP */ /*|  (1<<10) S */;
				break;
			case mem_region_type_page_table:
				page_flags = (0x1<<1) /* | (1<<3) C */ | (1<<2) /* B */ 
					| (3<<4) /* AP */ /*|  (1<<10) S */;
				break;
			case mem_region_type_io_dev:
				page_flags = (0x1<<1)  /*| (1<<3) C */ | (1<<2) /* B */ 
					| (3<<4) /* AP */ |  (1<<10) /* S */;
				break;
		}
		for(uint k=start_page; k<end_page; ++k) {
			uint32 phys_addr = k;
			uint32* entry = page_table_start + k;
			*entry = page_flags | (phys_addr << PAGE_BITS);
		}
	}
}

#ifdef ARCH_HAS_MMU

void initMMU() {

	/* build a 1:1 page table for all used physical addresses 
	 * (physical == virtual address)
	 */

	const ulong max_addr = getMaxPhysicalAddress();

	const uint first_level_bits = 12;
	const uint second_level_bits = 8; // = 32 - PAGE_BITS - first_level_bits
	const uint entry_bits = 2; //how many bits each entry has (=4 bytes)
								//equal size for 1. & 2. level entry
	uint first_level_entries = max_addr >> (second_level_bits+PAGE_BITS);
	if(first_level_entries == 0) first_level_entries = 1;
	const uint first_level_size = first_level_entries << entry_bits;
	const uint second_level_entries = (1<<second_level_bits) *
		first_level_entries;
	const uint second_level_size = second_level_entries << entry_bits;

	const uint first_level_align = 1<<(first_level_bits+entry_bits);
	const uint second_level_align = 1<<(second_level_bits+entry_bits);

	const mem_region* first_level_region = getPhysicalRegion(first_level_size,
			first_level_align, mem_region_type_page_table);
	const mem_region* second_level_region = getPhysicalRegion(second_level_size,
			second_level_align, mem_region_type_page_table);

	if(!first_level_region || !second_level_region) 
		panic("not enough space for page table\n");

	initFrames(); //after this, the regions should not be changed anymore


	/* store page table */
	//fully initialize first level
	uint32 second_level_addr = second_level_region->start;
	for(uint i=0; i<first_level_entries; ++i) {
		uint32* entry = ((uint32*)first_level_region->start) + i;
		uint32 value = 0x1; //coarse page table, P=0, Domain=0
		value |= second_level_addr;
		*entry = value;
		second_level_addr += second_level_align;
	}
	//init second level depending on regions
	memset((void*)second_level_region->start, 0, second_level_region->size);

	//TODO: don't setup these entries here, make this on request in kmalloc...
	initRegionsPageTableEntries(allocatable_mem_regions, 
			allocatable_mem_region_count, (uint32*)second_level_region->start);

	initRegionsPageTableEntries(mem_regions, 
			mem_region_count, (uint32*)second_level_region->start);


	// setup translation base register
	__asm__ volatile(
			"mrc p15, 0, r0, c2, c0, 0;"
			"and r0, r0, %0;"
			"orr r0, r0, %1;"
			"mcr p15, 0, r0, c2, c0, 0;"
			::"r"(first_level_align-1),"r"(first_level_region->start): "r0");


	__asm__ volatile(
			"mrc p15, 0, r0, c2, c0, 2;"
			//use translation base register 0
			"bic r0, r0, #0x7;"
			"mcr p15, 0, r0, c2, c0, 2;"
			::: "r0");

	//init domain 0: domain access control register
	__asm__ volatile(
			"mov r0, #0x11;" //set domain 0 to access type 'manager'
			"mcr p15, 0, r0, c3, c0, 0;"
			::: "r0");


	//disable instruction cache
	__asm__ volatile(
			"mrc p15, 0, r0, c1, c0, 0;"
			"bic r0, r0, #0x1000;" //clear 12. bit
			"mcr p15, 0, r0, c1, c0, 0;"
			//invalidate both caches
			"eor r0,r0;"
			"mcr p15, 0, r0, c7, c7, 0;"
			::: "r0");
	

	//set XP bit of c1 Control Register
	__asm__ volatile(
			"mrc p15,0,r0,c1,c0,0;"
			"orr r0, r0, #0x800000;" //subpage AP bits disabled (ARMv6)
			"mcr p15,0,r0,c1,c0,0;"
			::: "r0");
	

	//enable MMU, data cache & instruction cache
	__asm__ volatile(
			"mrc p15, 0, r0, c1, c0, 0;"
			"orr r0, r0, #0x1;" //MMU: bit 0
			"orr r0, r0, #0x4;" //DCache: bit 2
			"orr r0, r0, #0x1000;" //ICache: bit 12
			"mcr p15, 0, r0, c1, c0, 0;"

			"mov r3, r3;"
			"mov r3, r3;"
			::: "r0");
}

#endif /* ARCH_HAS_MMU */
