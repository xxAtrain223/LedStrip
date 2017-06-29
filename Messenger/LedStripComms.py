from PyCmdMessenger import CmdMessenger
from PyCmdMessenger import ArduinoBoard

from enum import Enum
import dis
import signal

import logging

if __name__ == "__main__":                # Stupid python stuff
    from ourlogging import setup_logging  # This is the main script
else:
    from .ourlogging import setup_logging # This script is being imported
setup_logging(__file__)
logger = logging.getLogger(__name__)

from time import sleep, time

class DelayedKeyboardInterrupt(object):
    def __enter__(self):
        self.signal_received = False
        self.old_handler = signal.getsignal(signal.SIGINT)
        signal.signal(signal.SIGINT, self.handler)

    def handler(self, signal, frame):
        self.signal_received = (signal, frame)
        logging.info('SIGINT received. Delaying KeyboardInterrupt.')

    def __exit__(self, type, value, traceback):
        signal.signal(signal.SIGINT, self.old_handler)
        if self.signal_received:
            self.old_handler(*self.signal_received)

class LedStripMessenger(object):
    def __init__(self, port):
        self.arduino = ArduinoBoard(port, timeout=5.0, enable_dtr=False)
        self.commands = [["kAcknowledge", "i"],
                         ["kError", "i*"],
                         ["kUnknown", ""],
                         ["kPing", "?"],
                         ["kPingResult", "i"],
                         ["kPong", ""],
                         ["kPauseCalculations", "?"],
                         ["kResumeCalculations", "?"],
                         ["kUploadRedPattern", "?b*"],
                         ["kUploadGreenPattern", "?b*"],
                         ["kUploadBluePattern", "?b*"],
                         ["kSavePattern", "?b"],
                         ["kLoadPattern", "?b"],
                         ["kGetBrightness", "?"],
                         ["kGetBrightnessResult", "b"],
                         ["kSetBrightness", "?b"],
                         ["kSetPixel", "?bbbb"],
                         ["kFillSolid", "?bbb"],
                         ["kClearEeprom", "?"],
                         ["kIsEepromReady", "?"],
                         ["kIsEepromReadyResult", "?"],
                         ["kResetEeprom", "?"],
                         ["kReturnEeprom", "?"],
                         ["kReturnEepromResult", "b*"],
                         ["kJumpToDfu", "?"]]
        self.messenger = CmdMessenger(self.arduino, self.commands)

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self.close()

    def send(self, request_ack, has_response, command, *args, arg_formats=None):
        logger.info("Sending {0:s}".format(command))
        with DelayedKeyboardInterrupt():
            self.messenger.send(command, *(request_ack, *args), arg_formats=arg_formats)
            if request_ack:
                acknowledgement = self.messenger.receive()
            if has_response:
                response = self.messenger.receive()
        
        if request_ack:
            try:
                assert (acknowledgement[0] == "kAcknowledge" and self.commands[acknowledgement[1][0]][0] == command)
                logger.info("Received kAcknowledge for {}".format(command))
            except (AssertionError, TypeError):
                logger.error("Acknowledgement error")
                logger.error("Actual response was {}.".format(repr(acknowledgement)))
                if acknowledgement is not None:
                    logger.error("Acknowledged Command was {}.".format(self.commands[acknowledgement[1][0]][0]))
                logger.error("Command was {}.".format(command))

        if has_response:
            try:
                assert (response[0][:-6] == command and response[0][-6:] == "Result")
            except (AssertionError, TypeError):
                logger.error("Response error")
                logger.error("Actual response command was {}".format(repr(response)))
                logger.error("Command was {0:s}.".format(command))

        if has_response and response is not None:
            return response[1]

    def close(self):
        self.arduino.close()

    def ping(self, attempts = 5):
        for i in range(attempts):
            response = self.send(False, True, "kPing")
            if response is not None and self.commands[response[0]][0] == "kPong":
                logger.info("Received kPong")
                break
            else:
                logger.error("Ping response was not pong")
                logger.error("Actual response command was {}".format(repr(response)))
                if i < attempts:
                    logger.error("Trying again... {} tries remaining".format(attempts - i - 1))

    def uploadPattern(self, r_pattern, g_pattern, b_pattern):
        PythonOpcode = Enum("PythonOpcode",
            '''
            UNARY_POSITIVE
            UNARY_NEGATIVE
            UNARY_NOT
            UNARY_INVERT
            BINARY_POWER
            BINARY_MULTIPLY
            BINARY_MODULO
            BINARY_ADD
            BINARY_SUBTRACT
            BINARY_SUBSCR
            BINARY_FLOOR_DIVIDE
            BINARY_TRUE_DIVIDE
            BINARY_LSHIFT
            BINARY_RSHIFT
            BINARY_AND
            BINARY_XOR
            BINARY_OR
            RETURN_VALUE
            LOAD_CONST
            LOAD_NAME
            CALL_FUNCTION
        ''')

        def compilePattern(pattern):
            bytecode = compile(pattern, "<string>", "eval")
            rv = []

            for i in dis.Bytecode(bytecode):
                argval = i.argval

                if type(argval) is str:
                    if argval == "time":
                        argval = 0
                    elif argval == "index":
                        argval = 1
                    elif argval == "sin":
                        argval = 2
                    elif argval == "cos":
                        argval = 3
                elif argval is None:
                    argval = 0
                else:
                    argval = int(argval)    

                rv.append(PythonOpcode[i.opname].value - 1)
                rv.append(argval)

            if len(rv) > 16:
                logger.error("Instruction set is greater than allowed, 16.")
                logger.error("Actual length is {}.".format(len(rv)))
                rv = []

            return rv

        self.pauseCalculations()
        self.send(True, False, "kUploadRedPattern", *compilePattern(r_pattern))
        self.send(True, False, "kUploadGreenPattern", *compilePattern(g_pattern))
        self.send(True, False, "kUploadBluePattern", *compilePattern(b_pattern))
        self.resumeCalculations()
        logger.info("Pattern uploaded")

    def savePattern(self, index):
        self.send(True, False, "kSavePattern", index)

    def loadPattern(self, index):
        self.send(True, False, "kLoadPattern", index)

    def pauseCalculations(self):
        self.send(True, False, "kPauseCalculations")

    def resumeCalculations(self):
        self.send(True, False, "kResumeCalculations")

    def getBrightness(self):
        return self.send(True, True, "kGetBrightness")[0]

    def setBrightness(self, brightness):
        self.send(True, False, "kSetBrightness", brightness)

    def setPixel(self, index, r, g, b):
        self.send(False, False, "kSetPixel", index, r, g, b)

    def fillSolid(self, r, g, b):
        self.send(False, False, "kFillSolid", r, g, b)

    def setDeltaHue(self, deltaHue):
        self.send(True, False, "kSetDeltaHue", deltaHue)

    def clearEeprom(self):
        self.send(True, False, "kClearEeprom")

    def isEepromReady(self):
        ready = self.send(True, True, "kIsEepromReady")[0]

        if ready:
            logger.info("Eeprom is ready")
        else:
            logger.error("Eeprom is not ready")

        return ready

    def resetEeprom(self):
        self.send(True, False, "kResetEeprom")

    def returnEeprom(self):
        eeprom = self.send(True, True, "kReturnEeprom")

        for i, byte in enumerate(eeprom):
            s = ""
            if i < 2 * 16 * 3 * 8:
                if i % (2 * 16) == 0:
                    s += "Pattern[{}]".format(int(i / (2 * 16 * 3)))
                    if i / (2 * 16) % 3 == 0:
                        s += '["r"]'
                    elif i / (2 * 16) % 3 == 1:
                        s += '["g"]'
                    elif i / (2 * 16) % 3 == 2:
                        s += '["b"]'

            elif i == 768:
                s += "ready"
            elif i == 769:
                s += "currentPattern"
            elif i == 770:
                s += "currentBrightness"

            logger.info("{0:4d}: {1:3d}{2:s}".format(i, byte, " //" + s if s != "" else ""))

        return eeprom

    def jumpToDfu(self):
        self.send(False, False, "kJumpToDfu")

if __name__ == "__main__":
    with LedStripMessenger("/dev/LedStripController") as comm:
        comm.ping()
        
        op = 8
        if op == 0:
            comm.isEepromReady()
            comm.clearEeprom()
            comm.isEepromReady()
            comm.resetEeprom()
            comm.isEepromReady()
        
        elif op == 1:
            comm.uploadPattern(
                "sin(time + index + 0)",
                "sin(time + index + 85)",
                "sin(time + index + 170)"
            )

            comm.savePattern(0)

        elif op == 2:
            comm.uploadPattern(
                "255",
                "255",
                "255"
            )

            comm.savePattern(1)

        elif op == 3:
            comm.loadPattern(0)

        elif op == 4:
            comm.setBrightness(255)

        elif op == 5:
            comm.returnEeprom()

        elif op == 6:
            comm.pauseCalculations()
            comm.fillSolid(0, 0, 0)
            
            i = 1
            dir = 1
            while True:
                if i - dir * 2 >= 0:
                    comm.setPixel(i - dir * 2, 0, 0, 0)

                comm.setPixel(i - 1, 255, 0, 0)
                comm.setPixel(i, 255, 0, 0)
                comm.setPixel(i + 1, 255, 0, 0)

                i += dir
                if i >= 58:
                    dir = -1
                elif i <= 1:
                    dir = 1

        elif op == 7:
            comm.jumpToDfu()
