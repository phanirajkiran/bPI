### bPI ###

bPI is a minimal kernel, mainly for educational purposes. It was created
to run on the Raspberry Pi, but now supports multiple boards on different
platforms. Its modular design allows it to be easily ported to other boards and
platforms.


#### Features ####
- __Raspberry Pi__:
    * GPIO with IRQ's
    * Timer
    * serial: UART via GPIO pins (Baud=115200, 8N1)
    * I2C via GPIO pins
    * ATAG's: read & parse ATAG list, given by the bootloader
	* play audio via PWM (3.5 mm phone connector of the PI), play WAVE files
	  (see branch play_wave) or a single frequency
    * generic printk method (like printf)
    * interrupts: arm irq handler & timer interrupts
	* MMU & Paging: setup a virtual address space (physical == virtual
	  addresses)

- __LeanXcam__:
    * GPIO (plus LED)
    * Timer
    * serial: UART via GPIO pins (Baud=115200, 8N1)

- C++ support (with runtime information & exceptions disabled)

##### Flight Controller #####
The kernel includes a flight controller for a quadrocopter. See under
[kernel/aux/flight_controller](./kernel/aux/flight_controller/README.md) for
more details.


#### Build ####
- install an appropriate cross-compiler (eg arm-none-eabi-gcc)
- choose a cross-compiler: edit TOOLCHAIN in file Makefile
  (or use: make TOOLCHAIN=xy)
- edit ARCH & BOARD variables in Makefile to select a board to build

- `$ make`
  this will build the kernel as kernel.img
- make sure the load address is correct. If not edit the linker script kernel.ld


#### Known Issues ####
- static c++ objects: the constructor/destructor will NOT automatically be
  called on startup/exit
- Raspberry Pi: setting the kernel load address to something lower than 0x8000
  (and changing config.txt & \__estack accordingly) did not work for me. But
  setting it higher does work fine.
- exceptions are disabled, so a call like 'new int[4]' can return NULL and does
  not throw an exception


#### Miscellaneous

If you have any improvements or suggestions you are welcome to implement them
and/or contact me.


Copyright 2013-14 Beat Küng <beat-kueng@gmx.net>

