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
TARGET       = LedStripController
SRC          = $(TARGET).cpp Commands.cpp ws2812/light_ws2812.c Serial/Descriptors.c CmdMessenger/CmdMessenger.cpp Arduino/Millis.cpp Arduino/Stream.cpp Arduino/Print.cpp Arduino/WString.cpp $(LUFA_SRC_USB) $(LUFA_SRC_USBCLASS)
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
	python3.6 build/jumpToDfu.py; test $$? -eq 0
	touch state.dfu

state.flash: state.dfu
	sudo dfu-programmer $(MCU) flash $(TARGET).hex --force --suppress-validation; test $$? -eq 0
	touch state.flash

state.launch: state.flash
	sudo dfu-programmer $(MCU) launch --no-reset; test $$? -eq 254
	touch state.launch

upload: state.launch
	rm -f state.dfu
	rm -f state.erase
	rm -f state.flash
	rm -f state.launch
	@echo "Firmware successfully uploaded"
