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

#include "atag.h"

#include <kernel/utils.h>
#include <kernel/mem.h>
#include <kernel/errors.h>


static uint32* atag_ptr = NULL;

static struct used_tags tags;


void readATAGRegister(void* ptr) {
	atag_ptr = (uint32*) ptr;
}


int readATAG() {
	if(!atag_ptr) return -E_NO_SUCH_RESOURCE;
	uint32* p = atag_ptr;
	mem_region region;
	struct tag_header* header = (struct tag_header*)p;

	memset(&tags, 0, sizeof(struct used_tags));

	while(header->tag != ATAG_NONE && header->size > 1) {
		void* data = p + sizeof(struct tag_header) / sizeof(uint32);
		switch(header->tag) {
			case ATAG_CORE: break;
			case ATAG_MEM: 
				if(tags.mem_count < MEM_ATAG_COUNT)
					tags.mem[tags.mem_count++] = (struct tag_mem32*)data;
				{
					struct tag_mem32* t = (struct tag_mem32*)data;
					region.type = mem_region_type_normal;
					region.start = t->start;
					region.size = t->size;
					addMemoryRegion(&region);
				}
				break;
			case ATAG_VIDEOTEXT:
				tags.videotext = (struct tag_videotext*)data;
				break;
			case ATAG_RAMDISK: break;
			case ATAG_INITRD: break;
			case ATAG_INITRD2: break;
			case ATAG_SERIAL: 
				tags.serialnr = (struct tag_serialnr*)data;
				break;
			case ATAG_REVISION:
				tags.revision = (struct tag_revision*)data;
				break;
			case ATAG_VIDEOLFB: break;
			case ATAG_CMDLINE: 
				tags.cmdline = (struct tag_cmdline*)data;
				strncpy(kernel_cmd_line, tags.cmdline->cmdline, 
						COMMAND_LINE_LEN);
				break;
			case ATAG_ACORN: break;
			case ATAG_BOARD: break;
			case ATAG_MEMCLK: break;
			default: break;
		}
		p += header->size;
		header = (struct tag_header*)p;
	}
	return SUCCESS;
}



void printATAG() {
	if(!atag_ptr) {
		printk_w("ATAG: no ATAG found\n");
		return;
	}

	printk_i("ATAG at %p:\n", atag_ptr);


	/* memory */
	printk_i(" Memory:\n");
	for(int i=0; i<tags.mem_count; ++i) {
		printk_i("  start: %#x, size: %R\n", tags.mem[i]->start, 
				tags.mem[i]->size);
	}

	/* video */
	if(tags.videotext) {
	}

	/* serial number */
	if(tags.serialnr) {
		printk_i(" SerialNr: high: %08x, low: %08x\n", tags.serialnr->high,
				tags.serialnr->low);
	}
	/* revision */
	if(tags.revision) {
		printk_i(" Revision: %i\n", tags.revision->rev);
	}

}

