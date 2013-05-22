
THIS_FILE := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
THIS_DIR := $(dir $(THIS_FILE))
MODULES_LOC :=

#add objects & subdirectories to build

src += $(THIS_DIR)main.cpp
src += $(THIS_DIR)utils.c
src += $(THIS_DIR)printk.c
src += $(THIS_DIR)endian.c
src += $(THIS_DIR)string.c
src += $(THIS_DIR)math.c
src += $(THIS_DIR)interrupt.c
src += $(THIS_DIR)mem.c
src += $(THIS_DIR)init.c
src += $(THIS_DIR)malloc.c
src += $(THIS_DIR)operator_new.cpp
MODULES_LOC += aux/
MODULES_LOC += compiler/


#create output directories
_dummy := $(foreach out_dir, $(MODULES_LOC), \
	$(shell [ -d $(BUILD)/$(THIS_DIR)$(out_dir) ] || \
	$(MKDIR) $(BUILD)/$(THIS_DIR)$(out_dir)))

#include sub directories
include $(patsubst %,$(THIS_DIR)%build.mk,$(MODULES_LOC))

