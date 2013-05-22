/******************************************************************************
*	kernel.ld
*	 by Alex Chadwick
*	 adjusted by Beat Küng
*
******************************************************************************/


OUTPUT_ARCH(arm)
ENTRY(_start)


MEMORY { 
	interrupt_vector_mem (rx) : ORIGIN = 0x0, LENGTH = 0x40

	/* this defines the image load address & the max kernel size.
	 * changing the load address probably also needs an adjusted stack __estack
	 */
	ram (rwx) : ORIGIN = 0x8000, LENGTH = 1M 
}
SECTIONS {
	/*
	* .init needs to be the first section
	*/
	.init : {
		__kernel_start_addr = .;
		__estack = __kernel_start_addr;
		*(.init)
	} >ram
	
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

	/*
	 * the interrupt vector: we must know its start & end address
	 * to relocate it at runtime
	 */
	__interrupt_vector_start = LOADADDR(.interrupt_vector);
	__interrupt_vector_end = __interrupt_vector_start 
				+ SIZEOF(.interrupt_vector);
	.interrupt_vector :  {
		KEEP(*(.interrupt_vector))
	} >interrupt_vector_mem AT>ram


	.bss : {
		. = ALIGN(0x10);
		__bss_start = .;
		*(.bss .bss.*)
		. = ALIGN(0x10);
		__bss_end = .;
	} >ram

	__kernel_end_addr = .;

	/* discard everything else */
	/DISCARD/ : {
		*(*)
	}
}
