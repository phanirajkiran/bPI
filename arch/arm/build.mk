
THIS_FILE := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
THIS_DIR := $(dir $(THIS_FILE))
MODULES_LOC :=

#add objects & subdirectories to build

src += $(THIS_DIR)arch.c
src += $(THIS_DIR)main.S
src += $(THIS_DIR)atag.c
src += $(THIS_DIR)interrupt.S
src += $(THIS_DIR)interrupt.c
src += $(THIS_DIR)mmu.c

MODULES_LOC += bcm2835/

#create output directories
_dummy := $(foreach out_dir, $(MODULES_LOC), \
	$(shell [ -d $(BUILD)/$(THIS_DIR)$(out_dir) ] || \
	$(MKDIR) $(BUILD)/$(THIS_DIR)$(out_dir)))

#include sub directories
include $(patsubst %,$(THIS_DIR)%build.mk,$(MODULES_LOC))

