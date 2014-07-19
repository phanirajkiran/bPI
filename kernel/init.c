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

#include <kernel/mem.h>
#include <kernel/malloc.h>
#include <kernel/mmu.h>
#include <kernel/printk.h>
#include <kernel/utils.h>
#include <kernel/panic.h>


extern int __bss_start;
extern int __bss_end;

void initZeroMemory() {
	/* init the .bss section to zero (the compiler assumes all memory in bss
	 * will be initialized to zero, so we have to do it manually here) 
	 *
	 * this must be called as early as possible.
	 *
	 * we assume the bss start & end is aligned to at least size of int
	 */
	for(int* i=&__bss_start; i!=&__bss_end; ++i)
		*i = 0;
}

void initKernel() {

	printk_i("\n++++++++++++++++++++++++\n");
	printk_i(  "  Welcome to Banana Pi  \n");
	printk_i(  "++++++++++++++++++++++++\n\n");

	/* init memory */
	initKernelMemRegions();
	initDeviceMemRegions();
	initMMU();
	if(initMalloc()) panic("failed to initialize malloc\n");

	printMemRegions();


	if(*kernel_cmd_line) {
		printk_i("Kernel Command Line: %s\n", kernel_cmd_line);
	}
}
