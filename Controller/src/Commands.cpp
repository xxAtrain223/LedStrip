#include "LedStripController.h"

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
    kGetBrightness,
    kGetBrightnessResult,
    kSetBrightness,
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

void cmdSetPixel()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    uint8_t index = cmdMessenger.readBinArg<byte>();
    if (index >= 0 && index < NUM_LEDS)
    {
        leds[index].r = cmdMessenger.readBinArg<byte>();
        leds[index].g = cmdMessenger.readBinArg<byte>();
        leds[index].b = cmdMessenger.readBinArg<byte>();
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
    cmdMessenger.attach(kSetPixel, cmdSetPixel);
    cmdMessenger.attach(kFillSolid, cmdFillSolid);
    cmdMessenger.attach(kJumpToDfu, cmdJumpToDfu);
}
