##
# Copyright (C) 2013 Beat Küng <beat-kueng@gmx.net>
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 3 of the License.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#


# Disable make's built-in rules.
MAKE += -RL --no-print-directory
SHELL := $(shell which sh)



TOOLCHAIN ?=		arm-none-eabi

ARCH := 			arm
BOARD := 			raspberry_pi

INSTALL_DIR ?=		.


DEFINES :=			-DBOARD_$(BOARD) -DARCH_$(ARCH)
WARNINGS :=			-Wno-unused
CFLAGS := 			-pipe -O2 -Wall -Werror=implicit-function-declaration \
					$(DEFINES) -std=c99 $(WARNINGS) \
					-fno-common -ffreestanding
CXFLAGS := 			-pipe -O2 -Wall -Werror=implicit-function-declaration \
					$(DEFINES) -std=c++11 $(WARNINGS) \
					-fno-common -ffreestanding -fno-rtti -fno-exceptions
LDFLAGS :=			-nostartfiles -ffreestanding -fno-rtti -fno-exceptions \
					-Wl,--no-undefined -Wl,--gc-sections -nostdlib \
					-lm -lstdc++ -lc -lgcc \
					#-lsupc++ # for: RTTI & exceptions
INCLUDES :=			-I. -Iarch/$(ARCH) -Iarch/$(ARCH)/board/$(BOARD)

# whether or not to generate & use include dependency files
USE_DEP_FILES :=	1


CROSS_COMPILE :=
ifdef TOOLCHAIN
	CROSS_COMPILE :=$(TOOLCHAIN)-
endif

CC :=				$(CROSS_COMPILE)gcc
CX :=				$(CROSS_COMPILE)g++
AS :=				$(CROSS_COMPILE)as
LD :=				$(CROSS_COMPILE)ld
OBJCOPY :=			$(CROSS_COMPILE)objcopy
OBJDUMP :=			$(CROSS_COMPILE)objdump
NM :=				$(CROSS_COMPILE)nm


RM := rm -rf
MKDIR := mkdir -p
COPY := cp


# build output directory. can be changed with O=<dir>
#check if output directory exists
BUILD := .
ifneq ($(O),)
BUILD := $(shell cd $(O) && /bin/pwd)
$(if $(BUILD),, \
	 $(error output directory "$(O)" does not exist))
endif
BUILD_BASE := $(BUILD)
BUILD := $(BUILD)/build



# The name of the output file to generate.
TARGET := $(BUILD_BASE)/kernel.img

# The name of the assembler listing file to generate.
LIST := $(BUILD_BASE)/kernel.disassembly

# The name of the map file to generate.
MAP := $(BUILD_BASE)/kernel.map

# The name of the linker script to use.
LINKER_SCRIPT := arch/$(ARCH)/board/$(BOARD)/kernel.ld

.PHONY: clean all debug rebuild disassembly kernel install

# default target: Rule to make the kernel 
kernel: $(TARGET)
all: $(TARGET) disassembly

# Rule to make disassembly
disassembly: $(LIST)

# Rule to remake everything. Does not include clean.
rebuild: all


#include sub directories
src :=
MODULES := kernel/ \
	arch/$(ARCH)/ \
	arch/$(ARCH)/board/$(BOARD)/ \
	drivers/ \
	3rdparty/
#create the output directories for these modules
_dummy := $(foreach out_dir, $(MODULES), \
	$(shell [ -d $(BUILD)/$(out_dir) ] || \
	$(MKDIR) $(BUILD)/$(out_dir)))
#subdirectories (this also creates the output directories)
include $(patsubst %,%build.mk,$(MODULES))

src_asm := $(filter %.S, $(src))
obj_asm := $(patsubst %.S, $(BUILD)/%_S.o,$(src_asm))

src_c := $(filter %.c, $(src))
obj_c := $(patsubst %.c, $(BUILD)/%_c.o,$(src_c))

src_cpp := $(filter %.cpp, $(src))
obj_cpp := $(patsubst %.cpp, $(BUILD)/%_cpp.o,$(src_cpp))


# dependency files
ifeq ($(strip $(USE_DEP_FILES)),1)
-include $(obj_c:.o=.d) $(obj_cpp:.o=.d) $(obj_asm:.o=.d)
$(BUILD)/%_S.d: %.S
	@$(CC) -MM -MG $(INCLUDES) $(CFLAGS) $< | \
		sed -e "s@^\(.*\)\.o:@$(dir $@)\1_S.d $(dir $@)\1_S.o:@" > $@
$(BUILD)/%_c.d: %.c
	@$(CC) -MM -MG $(INCLUDES) $(CFLAGS) $< | \
		sed -e "s@^\(.*\)\.o:@$(dir $@)\1_c.d $(dir $@)\1_c.o:@" > $@
$(BUILD)/%_cpp.d: %.cpp
	@$(CX) -MM -MG $(INCLUDES) $(CXFLAGS) $< | \
		sed -e "s@^\(.*\)\.o:@$(dir $@)\1_cpp.d $(dir $@)\1_cpp.o:@" > $@
endif # ($(USE_DEP_FILES),1)


# Rule to make the listing file (disassembly).
$(LIST) : $(BUILD)/output.elf
	@echo " [OBJDUMP] $? > $@"; \
	$(OBJDUMP) -D $(BUILD)/output.elf > $@

# Rule to make the image file.
$(TARGET) : $(BUILD)/output.elf
	@echo " [OBJCOPY] $? > $@"; \
	$(OBJCOPY) $(BUILD)/output.elf -O binary $@
	@echo "kernel image:               $(TARGET)"
	@echo "kernel load address:        0x`$(NM) $(BUILD)/output.elf | \
		awk '$$NF == "__kernel_start_addr" {print $$1}'`"

# Rule to make the elf file.
$(BUILD)/output.elf : $(obj_asm) $(obj_c) $(obj_cpp) $(LINKER_SCRIPT)
	@echo " [LD] $? > $@"; \
	$(CX) $(obj_asm) $(obj_c) $(obj_cpp) $(LDFLAGS) \
		-Wl,-Map,$(MAP) -o $(BUILD)/output.elf -Xlinker -T$(LINKER_SCRIPT) \
	|| (echo "\nCommand failed: \
$(CX) $(obj_asm) $(obj_c) $(obj_cpp) $(LDFLAGS) \
-Wl,-Map,$(MAP) -o $(BUILD)/output.elf -Xlinker -T$(LINKER_SCRIPT)" && false)

# Rule to make the object files from assembly
# we use the C-compiler because we want to use the preprocessor
$(BUILD)/%_S.o: %.S
	@echo " [AS] $<"; \
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ \
	|| (echo "\nCommand failed: $(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@" && false)

# Rule to make the object files from c code
$(BUILD)/%_c.o: %.c
	@echo " [CC] $<"; \
	$(CC) -c $(INCLUDES) $(CFLAGS) $< -o $@ \
	|| (echo "\nCommand failed: $(CC) -c $(INCLUDES) $(CFLAGS) $< -o $@" && false)

# Rule to make the object files from c++ code
$(BUILD)/%_cpp.o: %.cpp
	@echo " [CP] $<"; \
	$(CX) -c $(INCLUDES) $(CXFLAGS) $< -o $@ \
	|| (echo "\nCommand failed: $(CX) -c $(INCLUDES) $(CXFLAGS) $< -o $@" && false)

install: $(TARGET)
	$(COPY) $(TARGET) $(INSTALL_DIR)

# Rule to clean files.
clean : 
	-$(RM) $(BUILD)/* 
	-$(RM) $(TARGET) $(LIST) $(MAP)
