/******************************************************************************
*	kernel.ld
*	 by Alex Chadwick
*
*	A linker script for generation of raspberry pi kernel images.
******************************************************************************/

OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY { 
	/* we assume a start address of 0x8000 */
	ram : ORIGIN = 0x8000, LENGTH = 1M 
}
SECTIONS {
	/*
	* First and formost we need the .init section, containing the IVT.
	*/
	.init : {
		*(.init)
	} >ram
	
	/* 
	* We allow room for the ATAGs and the stack and then start our code at
	* 0x8000.
	*/
	.text : {
		*(.text)
	} >ram
	
	/* 
	* Next we put the data.
	*/
	. = ALIGN(16);
	.data : {
		*(.data)
	} >ram

	.rodata : {
		*(.rodata .rodata.*)
	} >ram

	. = ALIGN(16);
	__bss_start = .;
	.bss : {
		*(.bss)
	} >ram
	. = ALIGN(16);
	__bss_end = .;

	/*
	* Finally comes everything else. A fun trick here is to put all other 
	* sections into this section, which will be discarded by default.
	*/
	/DISCARD/ : {
		*(*)
	}
}
