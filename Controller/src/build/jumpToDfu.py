import sys
import time
import subprocess
import serial


sys.path.append("..")
sys.path.append("../..")
sys.path.append("../../..")
from Messenger.LedStripComms import LedStripMessenger


with LedStripMessenger("/dev/LedStripController") as comms:
    comms.ping()
    comms.jumpToDfu()

    for i in range(10):
        exit_code = subprocess.call("sudo dfu-programmer atmega32u4 get family".split())
        if exit_code == 0:
            print("In DFU mode")
            sys.exit(0)
        else:
            time.sleep(0.5)

    sys.exit(1)
