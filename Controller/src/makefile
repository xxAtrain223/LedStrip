#
#             LUFA Library
#     Copyright (C) Dean Camera, 2017.
#
#  dean [at] fourwalledcubicle [dot] com
#           www.lufa-lib.org
#
# --------------------------------------
#         LUFA Project Makefile.
# --------------------------------------

# Run "make help" for target help.

MCU          = atmega32u4
ARCH         = AVR8
BOARD        = USBKEY
F_CPU        = 16000000
F_USB        = $(F_CPU)
OPTIMIZATION = s
TARGET       = VirtualSerial
SRC          = $(TARGET).cpp Descriptors.c millis.cpp CmdMessenger.cpp $(LUFA_SRC_USB) $(LUFA_SRC_USBCLASS)
LUFA_PATH    = ./LUFA
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -IConfig/
LD_FLAGS     =
SHELL		 = cmd.exe

# Default target
all:

# Include LUFA-specific DMBS extension modules
DMBS_LUFA_PATH ?= $(LUFA_PATH)/Build/LUFA
include $(DMBS_LUFA_PATH)/lufa-sources.mk
include $(DMBS_LUFA_PATH)/lufa-gcc.mk

# Include common DMBS build system modules
DMBS_PATH      ?= $(LUFA_PATH)/Build/DMBS/DMBS
include $(DMBS_PATH)/core.mk
include $(DMBS_PATH)/cppcheck.mk
include $(DMBS_PATH)/doxygen.mk
include $(DMBS_PATH)/dfu.mk
include $(DMBS_PATH)/gcc.mk
include $(DMBS_PATH)/hid.mk
include $(DMBS_PATH)/avrdude.mk
include $(DMBS_PATH)/atprogram.mk

#$(TARGET).c: all


state.dfu: $(TARGET).hex
	python build/jumpToDfu.py; test $$? -eq 0
	touch state.dfu

state.erase: state.dfu
	sudo dfu-programmer $(MCU) erase; test $$? -eq 5
	touch state.erase

state.flash: state.erase
	sudo dfu-programmer $(MCU) flash $(TARGET).hex --suppress-validation; test $$? -eq 0
	touch state.flash

state.launch: state.flash
	sudo dfu-programmer $(MCU) launch --no-reset; test $$? -eq 254
	touch state.launch

upload: state.launch
	rm state.dfu
	rm state.erase
	rm state.flash
	rm state.launch
	@echo "Firmware successfully uploaded"
