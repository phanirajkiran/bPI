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

/*! this reads the ARM ATAG information, provided by the bootloader.
 * the start address should be given in the register r2, the _start function
 * takes care of this
 *
 * the table consists of several tags until ATAG_NONE.
 * each tag consists of:
 * 	- size (32bits) of tag, including size
 * 	- tag identification (ATAG_x)
 * 	- tag specific stuff
 */

#ifndef ATAG_ARCH_HEADER_H_
#define ATAG_ARCH_HEADER_H_

#include <kernel/utils.h>

/* called by _start to store the r2 address, that will be used later to init
 * ATAG list */
void readATAGRegister(void* ptr);


/* parse & read atag's. this does not move the memory, so the memory at *ptr
 * must not be overwritten afterwards, if the tags will be used later.
 * - initializes the command line, included via kernel/utils.h
 */
void readATAG();


/* output atag info's via printk */
void printATAG();


/*
 * taken from: linux/include/asm/setup.h
 */


/* The list ends with an ATAG_NONE node. */
#define ATAG_NONE 0x00000000

struct tag_header {
	uint32 size;
	uint32 tag;
};

/* The list must start with an ATAG_CORE node */
#define ATAG_CORE 0x54410001

struct tag_core {
	uint32 flags;	/* bit 0 = read-only */
	uint32 pagesize;
	uint32 rootdev;
};

/* it is allowed to have multiple ATAG_MEM nodes */
#define ATAG_MEM 0x54410002

struct tag_mem32 {
	uint32	size;
	uint32	start;	/* physical start address */
};

/* VGA text type displays */
#define ATAG_VIDEOTEXT 0x54410003

struct tag_videotext {
	uint8	x;
	uint8	y;
	uint16	video_page;
	uint8	video_mode;
	uint8	video_cols;
	uint16	video_ega_bx;
	uint8	video_lines;
	uint8	video_isvga;
	uint16	video_points;
};

/* describes how the ramdisk will be used in kernel */
#define ATAG_RAMDISK 0x54410004

struct tag_ramdisk {
	uint32 flags;	/* bit 0 = load, bit 1 = prompt */
	uint32 size;	/* decompressed ramdisk size in _kilo_ bytes */
	uint32 start;	/* starting block of floppy-based RAM disk image */
};

/* describes where the compressed ramdisk image lives (virtual address) */
/*
* this one accidentally used virtual addresses - as such,
* its depreciated.
*/
#define ATAG_INITRD 0x54410005

/* describes where the compressed ramdisk image lives (physical address) */
#define ATAG_INITRD2 0x54420005

struct tag_initrd {
	uint32 start;	/* physical start address */
	uint32 size;	/* size of compressed ramdisk image in bytes */
};

/* board serial number. "64 bits should be enough for everybody" */
#define ATAG_SERIAL 0x54410006

struct tag_serialnr {
	uint32 low;
	uint32 high;
};

/* board revision */
#define ATAG_REVISION 0x54410007

struct tag_revision {
	uint32 rev;
};

/* initial values for vesafb-type framebuffers. see struct screen_info
* in include/linux/tty.h
*/
#define ATAG_VIDEOLFB 0x54410008

struct tag_videolfb {
	uint16	lfb_width;
	uint16	lfb_height;
	uint16	lfb_depth;
	uint16	lfb_linelength;
	uint32	lfb_base;
	uint32	lfb_size;
	uint8	red_size;
	uint8	red_pos;
	uint8	green_size;
	uint8	green_pos;
	uint8	blue_size;
	uint8	blue_pos;
	uint8	rsvd_size;
	uint8	rsvd_pos;
};

/* command line: \0 terminated string */
#define ATAG_CMDLINE 0x54410009

struct tag_cmdline {
	char	cmdline[1];	/* this is the minimum size */
};

/* acorn RiscPC specific information */
#define ATAG_ACORN 0x41000101

struct tag_acorn {
	uint32 memc_control_reg;
	uint32 vram_pages;
	uint8 sounddefault;
	uint8 adfsdrives;
};

/* TI OMAP specific information */
#define ATAG_BOARD 0x414f4d50

struct tag_omap {
	uint8 data[0];
};

/* footbridge memory clock, see arch/arm/mach-footbridge/arch.c */
#define ATAG_MEMCLK 0x41000402

struct tag_memclk {
	uint32 fmemclk;
};

struct tag {
	struct tag_header hdr;
	union {
		struct tag_core	core;
		struct tag_mem32	mem;
		struct tag_videotext	videotext;
		struct tag_ramdisk	ramdisk;
		struct tag_initrd	initrd;
		struct tag_serialnr	serialnr;
		struct tag_revision	revision;
		struct tag_videolfb	videolfb;
		struct tag_cmdline	cmdline;

		/*
		 * Acorn specific
		 */
		struct tag_acorn	acorn;

		/*
		 * OMAP specific
		 */
		struct tag_omap omap;

		/*
		 * DC21285 specific
		 */
		struct tag_memclk	memclk;
	} u;
};

#define MEM_ATAG_COUNT 10 //there can be multiple memory tags

struct used_tags {
	struct tag_mem32* mem[MEM_ATAG_COUNT];
	int mem_count;
	struct tag_videotext* videotext;
	struct tag_serialnr* serialnr;
	struct tag_revision* revision;
	struct tag_cmdline* cmdline;
};


#endif /* ATAG_ARCH_HEADER_H_ */



