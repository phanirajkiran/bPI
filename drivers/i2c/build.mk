
THIS_FILE := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
THIS_DIR := $(dir $(THIS_FILE))
MODULES_LOC :=

#add objects & subdirectories to build

src += $(THIS_DIR)adafruit_pwm.cpp
src += $(THIS_DIR)bmp180_barometer.cpp
src += $(THIS_DIR)hmc5883l_compass.cpp
src += $(THIS_DIR)mpu-6050_accel_gyro.cpp
#MODULES_LOC += aux/


#create output directories
_dummy := $(foreach out_dir, $(MODULES_LOC), \
	$(shell [ -d $(BUILD)/$(THIS_DIR)$(out_dir) ] || \
	$(MKDIR) $(BUILD)/$(THIS_DIR)$(out_dir)))

#include sub directories
include $(patsubst %,$(THIS_DIR)%build.mk,$(MODULES_LOC))

