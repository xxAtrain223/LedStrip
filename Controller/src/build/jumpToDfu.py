import sys
import time
import subprocess
import serial


with serial.Serial("/dev/LedStripController") as comms:
    while True:
        try:
            comms.write(b"x\n")
        except:
            break

    for i in range(10):
        exit_code = subprocess.call("sudo dfu-programmer atmega32u4 get family".split())
        if exit_code == 0:
            print("In DFU mode")
            break
        else:
            time.sleep(0.5)
