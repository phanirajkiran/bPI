/******************************************************************************
*	kernel.ld
*	 by Alex Chadwick
*	 adjusted by Beat Küng
*
******************************************************************************/


OUTPUT_FORMAT("elf32-bfin", "elf32-bfin", "elf32-bfin") 
OUTPUT_ARCH(bfin)
ENTRY(_start)


MEMORY { 

	/* this defines the image load address & the max kernel size.
	 * changing the load address probably also needs an adjusted stack __estack
	 */
	ram (rwx) : ORIGIN = 0x2000000, LENGTH = 1M 

	MEM_L1_SCRATCH    : ORIGIN = 0xFFB00000, LENGTH = 0x1000
}
SECTIONS {
	/*
	* .init needs to be the first section
	*/
	.init : {
		___kernel_start_addr = .;
		__kernel_start_addr = .;
		*(.init)
	} >ram
	/* put the stack in fast scratch memory */
	___estack   = ORIGIN(MEM_L1_SCRATCH) + LENGTH(MEM_L1_SCRATCH);
	
	.text : {
		. = ALIGN(0x20); /* some padding (for interrupt vector at 0) */
		*(.text .text.*)

	} >ram

	/* 
	* Next we put the data.
	*/
	.data : {
		. = ALIGN(0x10);
		*(.data .data.*)
	} >ram

	.rodata : {
		*(.rodata .rodata.*)
	} >ram

	.bss : {
		. = ALIGN(0x10);
		___bss_start = .;
		*(.bss .bss.* COMMON)
		. = ALIGN(0x10);
		___bss_end = .;
	} >ram

	___kernel_end_addr = .;

	/* discard everything else */
	/DISCARD/ : {
		*(*)
	}
}
