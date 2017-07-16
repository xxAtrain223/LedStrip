#include "LedStripController.h"
#include "PythonInterpreter.h"
#include "EepromInteractors.h"
#include "EEPROM.h"

enum {
    kAcknowledge,
    kError,
    kUnknown,
    kPing,
    kPingResult,
    kPong,
    kPauseCalculations,
    kResumeCalculations,
    kUploadRedPattern,
    kUploadGreenPattern,
    kUploadBluePattern,
    kSavePattern,
    kLoadPattern,
    kGetPixel,
    kGetPixelResult,
    kSetPixel,
    kFillSolid,
    kClearEeprom,
    kIsEepromReady,
    kIsEepromReadyResult,
    kResetEeprom,
    kReturnEeprom,
    kReturnEepromResult,
    kJumpToDfu
};

void unknownCommand()
{
    cmdMessenger.sendCmdStart((uint8_t)kError);
    cmdMessenger.sendCmdBinArg((uint8_t)kUnknown);
    cmdMessenger.sendCmdBinArg((uint8_t)cmdMessenger.commandID());
    cmdMessenger.sendCmdEnd();
}

void cmdPing()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    if (requested_ack)
        cmdMessenger.sendBinCmd((uint8_t)kAcknowledge, (uint8_t)kPing);

    cmdMessenger.sendBinCmd((uint8_t)kPingResult, (uint8_t)kPong);

    for (int i = 0; i < 3; i++)
    {
        PORTD = 0x08;
        delay(50);
        PORTD = 0x00;
        delay(50);
    }
}

void cmdPauseCalculations()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    CalculateColors = false;

    if (requested_ack)
        cmdMessenger.sendBinCmd((uint8_t)kAcknowledge, (uint8_t)kPauseCalculations);
}

void cmdResumeCalculations()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    CalculateColors = true;

    if (requested_ack)
        cmdMessenger.sendBinCmd((uint8_t)kAcknowledge, (uint8_t)kResumeCalculations);
}

void uploadPattern(PyInt::InstructionSet set)
{
    //uint8_t opcode = 0, arg = 0;
    PyInt::Instruction* instructions = NULL;

    if (set == PyInt::InstructionSet::r)
        instructions = (PyInt::Instruction*)interp.r_instructions;
    else if (set == PyInt::g)
        instructions = (PyInt::Instruction*)interp.g_instructions;
    else if (set == PyInt::b)
        instructions = (PyInt::Instruction*)interp.b_instructions;

    for (uint8_t i = 0; i < INSTRUCTION_ARRAY_SIZE && (i == 0 || instructions[i-1].code != PyInt::RETURN_VALUE); i++)
    {
        instructions[i].code = cmdMessenger.readBinArg<uint8_t>();
        instructions[i].arg = cmdMessenger.readBinArg<uint8_t>();
    }
}

void cmdUploadRedPattern()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    uploadPattern(PyInt::r);

    if (requested_ack)
        cmdMessenger.sendBinCmd((uint8_t)kAcknowledge, (uint8_t)kUploadRedPattern);
}

void cmdUploadGreenPattern()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    uploadPattern(PyInt::g);

    if (requested_ack)
        cmdMessenger.sendBinCmd((uint8_t)kAcknowledge, (uint8_t)kUploadGreenPattern);
}

void cmdUploadBluePattern()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    uploadPattern(PyInt::b);

    if (requested_ack)
        cmdMessenger.sendBinCmd((uint8_t)kAcknowledge, (uint8_t)kUploadBluePattern);
}

void cmdSavePattern()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    uint8_t index = cmdMessenger.readBinArg<uint8_t>();
    setPattern(index);

    if (requested_ack)
        cmdMessenger.sendBinCmd((uint8_t)kAcknowledge, (uint8_t)kSavePattern);
}

void cmdLoadPattern()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    uint8_t index = cmdMessenger.readBinArg<uint8_t>();
    getPattern(index);

    if (requested_ack)
        cmdMessenger.sendBinCmd((uint8_t)kAcknowledge, (uint8_t)kLoadPattern);
}

void cmdGetPixel()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    uint8_t index = cmdMessenger.readBinArg<uint8_t>();

    if (requested_ack)
        cmdMessenger.sendBinCmd((uint8_t)kAcknowledge, (uint8_t)kGetPixel);

    if (index >= 0 && index < NUM_LEDS)
    {
        cmdMessenger.sendCmdStart((uint8_t)kGetPixelResult);
        cmdMessenger.sendCmdBinArg((uint8_t)leds[index].r);
        cmdMessenger.sendCmdBinArg((uint8_t)leds[index].g);
        cmdMessenger.sendCmdBinArg((uint8_t)leds[index].b);
        cmdMessenger.sendCmdEnd();
    }
    else
    {
        cmdMessenger.sendBinCmd((uint8_t)kError, (uint8_t)kSetPixel);
        return;
    }
}

void cmdSetPixel()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    uint8_t index = cmdMessenger.readBinArg<uint8_t>();
    if (index >= 0 && index < NUM_LEDS)
    {
        leds[index].r = cmdMessenger.readBinArg<uint8_t>();
        leds[index].g = cmdMessenger.readBinArg<uint8_t>();
        leds[index].b = cmdMessenger.readBinArg<uint8_t>();
        ws2812_setleds(leds, NUM_LEDS);
    }
    else
    {
        cmdMessenger.sendBinCmd((uint8_t)kError, (uint8_t)kSetPixel);
        return;
    }

    if (requested_ack)
        cmdMessenger.sendBinCmd((uint8_t)kAcknowledge, (uint8_t)kSetPixel);
}

void cmdFillSolid()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    uint8_t r = cmdMessenger.readBinArg<uint8_t>();
    uint8_t g = cmdMessenger.readBinArg<uint8_t>();
    uint8_t b = cmdMessenger.readBinArg<uint8_t>();

    for (uint8_t i = 0; i < NUM_LEDS; i++)
    {
        leds[i].r = r;
        leds[i].g = g;
        leds[i].b = b;
    }

    ws2812_setleds(leds, NUM_LEDS);

    if (requested_ack)
        cmdMessenger.sendBinCmd((uint8_t)kAcknowledge, (uint8_t)kFillSolid);
}

void cmdClearEeprom()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    clearEeprom();

    if (requested_ack)
        cmdMessenger.sendBinCmd((uint8_t)kAcknowledge, (uint8_t)kClearEeprom);
}

void cmdIsEepromReady()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    if (requested_ack)
        cmdMessenger.sendBinCmd((uint8_t)kAcknowledge, (uint8_t)kIsEepromReady);

    cmdMessenger.sendBinCmd((uint8_t)kIsEepromReadyResult, isEepromReady());
}

void cmdResetEeprom()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    resetEeprom();

    if (requested_ack)
        cmdMessenger.sendBinCmd((uint8_t)kAcknowledge, (uint8_t)kResetEeprom);
}

void cmdReturnEeprom()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    if (requested_ack)
        cmdMessenger.sendBinCmd((uint8_t)kAcknowledge, (uint8_t)kReturnEeprom);

    cmdMessenger.sendCmdStart((uint8_t)kReturnEepromResult);
    for (uint16_t i = 0; i < EEPROM.length(); i++)
        cmdMessenger.sendCmdBinArg((uint8_t)EEPROM[i]);
    cmdMessenger.sendCmdEnd();

}

void cmdJumpToDfu()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    if (requested_ack)
        cmdMessenger.sendBinCmd((uint8_t)kAcknowledge, (uint8_t)kJumpToDfu);


    TIMSK0 = 0;
    TCCR0B = 0;
    cli();
    MCUSR |= (1 << WDRF);
    wdt_enable(WDTO_15MS);
    while(true) {}
}

void attachCommandCallbacks()
{
    cmdMessenger.attach(unknownCommand);
    cmdMessenger.attach(kPing, cmdPing);
    cmdMessenger.attach(kPauseCalculations, cmdPauseCalculations);
    cmdMessenger.attach(kResumeCalculations, cmdResumeCalculations);
    cmdMessenger.attach(kUploadRedPattern, cmdUploadRedPattern);
    cmdMessenger.attach(kUploadGreenPattern, cmdUploadGreenPattern);
    cmdMessenger.attach(kUploadBluePattern, cmdUploadBluePattern);
    cmdMessenger.attach(kSavePattern, cmdSavePattern);
    cmdMessenger.attach(kLoadPattern, cmdLoadPattern);
    cmdMessenger.attach(kGetPixel, cmdGetPixel);
    cmdMessenger.attach(kSetPixel, cmdSetPixel);
    cmdMessenger.attach(kFillSolid, cmdFillSolid);
    cmdMessenger.attach(kClearEeprom, cmdClearEeprom);
    cmdMessenger.attach(kIsEepromReady, cmdIsEepromReady);
    cmdMessenger.attach(kResetEeprom, cmdResetEeprom);
    cmdMessenger.attach(kReturnEeprom, cmdReturnEeprom);
    cmdMessenger.attach(kJumpToDfu, cmdJumpToDfu);
}
