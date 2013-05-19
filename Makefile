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


DEFINES :=			-DBOARD_$(BOARD) -DARCH_$(ARCH)
WARNINGS :=			-Wno-unused
CFLAGS := 			-pipe -O2 -Wall -Werror=implicit-function-declaration \
					$(DEFINES) -fgnu89-inline -std=c99 $(WARNINGS) \
					-fno-common
CXFLAGS := 			-pipe -O2 -Wall -Werror=implicit-function-declaration \
					$(DEFINES) -fgnu89-inline $(WARNINGS) \
					-fno-common
LDFLAGS :=			
INCLUDES :=			-I. -Iarch/$(ARCH) -Iarch/$(ARCH)/board/$(BOARD)

# whether or not to generate & use include dependency files
USE_DEP_FILES :=	1


CROSS_COMPILE :=
ifdef TOOLCHAIN
	CROSS_COMPILE :=$(TOOLCHAIN)-
endif

CC :=				$(CROSS_COMPILE)gcc
CX :=				$(CROSS_COMPILE)g++
AS :=				$(CROSS_COMPILE)gcc #use gcc for preprocessor
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
TARGET := $(BUILD_BASE)/kernel.img

# The name of the assembler listing file to generate.
LIST := $(BUILD_BASE)/kernel.disassembly

# The name of the map file to generate.
MAP := $(BUILD_BASE)/kernel.map

# The name of the linker script to use.
LINKER_SCRIPT := arch/$(ARCH)/board/$(BOARD)/kernel.ld

.PHONY: clean all debug rebuild disassembly

# Rule to make everything: default target.
all: $(TARGET)

# Rule to make disassembly
disassembly: $(LIST)

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

src_asm := $(filter %.S, $(src))
obj_asm := $(patsubst %.S, $(BUILD)/%.o,$(src_asm))

src_c := $(filter %.c, $(src))
obj_c := $(patsubst %.c, $(BUILD)/%.o,$(src_c))

src_cpp := $(filter %.cpp, $(src))
obj_cpp := $(patsubst %.cpp, $(BUILD)/%.o,$(src_cpp))


# dependency files
ifeq ($(strip $(USE_DEP_FILES)),1)
-include $(obj_c:.o=.d) $(obj_cpp:.o=.d) $(obj_asm:.o=.d)
$(BUILD)/%.d: %.S
	@$(CC) -MM -MG $(INCLUDES) $(CFLAGS) $< | \
		sed -e "s@^\(.*\)\.o:@$(dir $@)\1.d $(dir $@)\1.o:@" > $@
$(BUILD)/%.d: %.c
	@$(CC) -MM -MG $(INCLUDES) $(CFLAGS) $< | \
		sed -e "s@^\(.*\)\.o:@$(dir $@)\1.d $(dir $@)\1.o:@" > $@
$(BUILD)/%.d: %.cpp
	@$(CX) -MM -MG $(INCLUDES) $(CXFLAGS) $< | \
		sed -e "s@^\(.*\)\.o:@$(dir $@)\1.d $(dir $@)\1.o:@" > $@
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
	@echo "kernel load address:        0x`$(OBJDUMP) -t $(BUILD)/output.elf | \
		grep _start | grep init | cut -f 1 -d' '`"

# Rule to make the elf file.
$(BUILD)/output.elf : $(obj_asm) $(obj_c) $(obj_cpp) $(LINKER_SCRIPT)
	@echo " [LD] $? > $@"; \
	$(LD) --no-undefined $(obj_asm) $(obj_c) $(obj_cpp) $(LDFLAGS) \
		-Map $(MAP) -o $(BUILD)/output.elf \
		-T $(LINKER_SCRIPT) $(LIBGCC) \
	|| (echo "\nCommand failed: $(LD) --no-undefined $(obj_asm) $(obj_c) $(obj_cpp) \
	$(LDFLAGS) -Map $(MAP) -o $(BUILD)/output.elf -T $(LINKER_SCRIPT) $(LIBGCC)" \
	&& false)

# Rule to make the object files from assembly
$(BUILD)/%.o: %.S
	@echo " [AS] $<"; \
	$(AS) -c $(CFLAGS) $(INCLUDES) $< -o $@ \
	|| (echo "\nCommand failed: $(AS) -c $(CFLAGS) $(INCLUDES) $< -o $@" && false)

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
	-$(RM) $(TARGET) $(LIST) $(MAP)
