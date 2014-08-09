
THIS_FILE := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
THIS_DIR := $(dir $(THIS_FILE))
MODULES_LOC :=

#add objects & subdirectories to build

src += $(THIS_DIR)flight_controller.cpp
src += $(THIS_DIR)main.cpp
src += $(THIS_DIR)motor_controller.cpp
src += $(THIS_DIR)motor_command.cpp
src += $(THIS_DIR)sensor_fusion_mahony.cpp
src += $(THIS_DIR)sensor_fusion_madgwick.cpp
src += $(THIS_DIR)stabilize_command.cpp
#MODULES_LOC += wave/


#create output directories
_dummy := $(foreach out_dir, $(MODULES_LOC), \
	$(shell [ -d $(BUILD)/$(THIS_DIR)$(out_dir) ] || \
	$(MKDIR) $(BUILD)/$(THIS_DIR)$(out_dir)))

#include sub directories
include $(patsubst %,$(THIS_DIR)%build.mk,$(MODULES_LOC))

