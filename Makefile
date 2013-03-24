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


#TODO: add config file to customize toolchain, board, cflags, debug build, ...
# linux Makefile:529: -include include/config/auto.conf
#  -> generate autoconf.h header to be included in source...
#TODO: build & use include dependencies


#TOOLCHAIN ?=arm-none-eabi
TOOLCHAIN ?=		arm-linux-gnueabihf

ARCH := 			arm
BOARD := 			raspberry_pi


CFLAGS := 			-pipe -O2 -Wall -Werror=implicit-function-declaration \
					-fgnu89-inline -std=c99 -Wno-unused
CXFLAGS := 			-pipe -O2 -Wall -Werror=implicit-function-declaration \
					-fgnu89-inline -Wno-unused
INCLUDES :=			-I. -Iarch/$(ARCH) -Iarch/$(ARCH)/board/$(BOARD)


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
LIBGCC :=			$(shell $(CC) -print-libgcc-file-name)

RM := rm -rf
MKDIR := mkdir -p

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
TARGET = $(BUILD_BASE)/kernel.img

# The name of the assembler listing file to generate.
LIST = $(BUILD_BASE)/kernel.list

# The name of the map file to generate.
MAP = $(BUILD_BASE)/kernel.map

# The name of the linker script to use.
LINKER_SCRIPT = arch/$(ARCH)/board/$(BOARD)/kernel.ld

.PHONY: clean all debug rebuild

# Rule to make everything: default target.
all: $(TARGET) $(LIST)

# Rule to remake everything. Does not include clean.
rebuild: all


#include sub directories
src :=
MODULES := kernel/ \
	arch/$(ARCH)/ \
	arch/$(ARCH)/board/$(BOARD)/
#create the output directories for these modules
_dummy := $(foreach out_dir, $(MODULES), \
	$(shell [ -d $(BUILD)/$(out_dir) ] || \
	$(MKDIR) $(BUILD)/$(out_dir)))
#subdirectories (this also creates the output directories)
include $(patsubst %,%build.mk,$(MODULES))

src_asm := $(filter %.s, $(src))
obj_asm := $(patsubst %.s, $(BUILD)/%.o,$(src_asm))

src_c := $(filter %.c, $(src))
obj_c := $(patsubst %.c, $(BUILD)/%.o,$(src_c))

src_cpp := $(filter %.cpp, $(src))
obj_cpp := $(patsubst %.cpp, $(BUILD)/%.o,$(src_cpp))



# Rule to make the listing file.
$(LIST) : $(BUILD)/output.elf
	@echo " [OBJDUMP] $? > $@"; \
	$(OBJDUMP) -d $(BUILD)/output.elf > $@

# Rule to make the image file.
$(TARGET) : $(BUILD)/output.elf
	@echo " [OBJCOPY] $? > $@"; \
	$(OBJCOPY) $(BUILD)/output.elf -O binary $@

# Rule to make the elf file.
$(BUILD)/output.elf : $(obj_asm) $(obj_c) $(obj_cpp) $(LINKER_SCRIPT)
	@echo " [LD] $? > $@"; \
	$(LD) --no-undefined $(obj_asm) $(obj_c) $(obj_cpp) \
		-Map $(MAP) -o $(BUILD)/output.elf \
		-T $(LINKER_SCRIPT) $(LIBGCC) \
	|| (echo "\nCommand failed: $(LD) --no-undefined $(obj_asm) $(obj_c) $(obj_cpp) \
	-Map $(MAP) -o $(BUILD)/output.elf -T $(LINKER_SCRIPT) $(LIBGCC)" && false)


# Rule to make the object files from assembly
$(BUILD)/%.o: %.s
	@echo " [AS] $<"; \
	$(AS) $(INCLUDES) $< -o $@ \
	|| (echo "\nCommand failed: $(AS) $(INCLUDES) $< -o $@" && false)

# Rule to make the object files from c code
$(BUILD)/%.o: %.c
	@echo " [CC] $<"; \
	$(CC) -c $(INCLUDES) $(CFLAGS) $< -o $@ \
	|| (echo "\nCommand failed: $(CC) -c $(INCLUDES) $(CFLAGS) $< -o $@" && false)

# Rule to make the object files from c++ code
$(BUILD)/%.o: %.cpp
	@echo " [CP] $<"; \
	$(CX) -c $(INCLUDES) $(CXFLAGS) $< -o $@ \
	|| (echo "\nCommand failed: $(CX) -c $(INCLUDES) $(CXFLAGS) $< -o $@" && false)

# Rule to clean files.
clean : 
	-$(RM) $(BUILD)/* 
	-$(RM) $(TARGET)
	-$(RM) $(LIST)
	-$(RM) $(MAP)
