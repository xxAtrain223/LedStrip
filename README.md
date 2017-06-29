# LedStrip
Software to control an LED strip using higher level functions.

## Controller
This is the software that will be written to the Controller board.
Libraries:
- CmdMessenger: A messaging library for Arduino/embeded devices.
- FastLED: A fast, efficient, easy-to-use library for addressable LED strips.
- Platformio: Creates a hex file to be uploaded.

## Messenger
This is the software responsible for communicating with the Controller board.
Libraries:
- PyCmdMessenger: Python class for communication with an arduino using the CmdMessenger serial communication library.

## Uploading Firmware
This is done using DFU and DFU-programmer.
- The script jumpToDfu.py tells the controller to jump to DFU mode with a command and dfu-programmer.
- Then we erase the current flash memory, upload the firmare.hex to flash, then launch the application on the Controller.
