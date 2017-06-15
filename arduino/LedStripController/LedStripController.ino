#include <EEPROM.h>

#include <CmdMessenger.h>
#include <FastLED.h>
#include <avr/wdt.h>

#include "PythonInterpreter.h"

#define EEPROM_PATTERN_COUNT 8

#define PIN 6
#define NUM_LEDS 60
CRGB leds[NUM_LEDS];

PyInt::Interpreter interp;
CmdMessenger cmdMessenger = CmdMessenger(Serial);

bool calculateColors = true;

void setup() {
    Serial.begin(9600);

    attachCommandCallbacks();

    FastLED.addLeds<NEOPIXEL, PIN>(leds, NUM_LEDS);

    interp.Sin = (byte(*)(byte))sin8;
    interp.Cos = (byte(*)(byte))cos8;

    interp.Time = 0;
    interp.Index = 0;

    if (!isEepromReady())
        resetEeprom();

    getPattern(getCurrentPattern());
    FastLED.setBrightness(getCurrentBrightness());
}

void loop() {
    cmdMessenger.feedinSerialData();

    if (calculateColors)
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            interp.Index = i;
            leds[i].r = interp.execute(PyInt::InstructionSet::r);
            leds[i].g = interp.execute(PyInt::InstructionSet::g);
            leds[i].b = interp.execute(PyInt::InstructionSet::b);
        }
        
        interp.Time = (interp.Time + 1) % 256;
    }

    FastLED.show();
    FastLED.delay(0);
}

#pragma region CmdCallbacks
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
    cmdMessenger.attach(kGetBrightness, cmdGetBrightness);
    cmdMessenger.attach(kSetBrightness, cmdSetBrightness);
    cmdMessenger.attach(kSetPixel, cmdSetPixel);
    cmdMessenger.attach(kFillSolid, cmdFillSolid);
    cmdMessenger.attach(kClearEeprom, cmdClearEeprom);
    cmdMessenger.attach(kIsEepromReady, cmdIsEepromReady);
    cmdMessenger.attach(kResetEeprom, cmdResetEeprom);
    cmdMessenger.attach(kReturnEeprom, cmdReturnEeprom);
    cmdMessenger.attach(kJumpToDfu, cmdJumpToDfu);
}

void unknownCommand()
{
    cmdMessenger.sendBinCmd(kError, kUnknown);
}

void cmdPing()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    if (requested_ack)
        cmdMessenger.sendBinCmd(kAcknowledge, kPing);

    cmdMessenger.sendBinCmd(kPingResult, kPong);
}

void cmdPauseCalculations()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    calculateColors = false;

    if (requested_ack)
        cmdMessenger.sendBinCmd(kAcknowledge, kPauseCalculations);
}

void cmdResumeCalculations()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    calculateColors = true;

    if (requested_ack)
        cmdMessenger.sendBinCmd(kAcknowledge, kResumeCalculations);
}

void uploadPattern(PyInt::InstructionSet set)
{
    byte opcode = 0, arg = 0;
    PyInt::Instruction* instructions = NULL;

    switch (set)
    {
    case PyInt::InstructionSet::r:
        instructions = (PyInt::Instruction*)interp.r_instructions;
        break;
    case PyInt::InstructionSet::g:
        instructions = (PyInt::Instruction*)interp.g_instructions;
        break;
    case PyInt::InstructionSet::b:
        instructions = (PyInt::Instruction*)interp.b_instructions;
        break;
    default:
        break;
    }

    for (byte i = 0; i < INSTRUCTION_ARRAY_SIZE && opcode != PyInt::Opcode::RETURN_VALUE; i++)
    {
        instructions[i].code = cmdMessenger.readBinArg<byte>();
        instructions[i].arg = cmdMessenger.readBinArg<byte>();
    }
}

void cmdUploadRedPattern()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    uploadPattern(PyInt::InstructionSet::r);

    if (requested_ack)
        cmdMessenger.sendBinCmd(kAcknowledge, kUploadRedPattern);
}

void cmdUploadGreenPattern()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    uploadPattern(PyInt::InstructionSet::g);

    if (requested_ack)
        cmdMessenger.sendBinCmd(kAcknowledge, kUploadGreenPattern);
}

void cmdUploadBluePattern()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    uploadPattern(PyInt::InstructionSet::b);

    if (requested_ack)
        cmdMessenger.sendBinCmd(kAcknowledge, kUploadBluePattern);
}

void cmdSavePattern()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    byte index = cmdMessenger.readBinArg<byte>();
    setPattern(index);

    if (requested_ack)
        cmdMessenger.sendBinCmd(kAcknowledge, kSavePattern);
}

void cmdLoadPattern()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    byte index = cmdMessenger.readBinArg<byte>();
    getPattern(index);

    if (requested_ack)
        cmdMessenger.sendBinCmd(kAcknowledge, kLoadPattern);
}

void cmdGetBrightness()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    if (requested_ack)
        cmdMessenger.sendBinCmd(kAcknowledge, kGetBrightness);

    cmdMessenger.sendBinCmd(kGetBrightnessResult, getCurrentBrightness());
}

void cmdSetBrightness()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    byte brightness = cmdMessenger.readBinArg<byte>();
    setCurrentBrightness(brightness);

    if (requested_ack)
        cmdMessenger.sendBinCmd(kAcknowledge, kSetBrightness);
}

void cmdSetPixel()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    byte index = cmdMessenger.readBinArg<byte>();
    if (index >= 0 && index < NUM_LEDS)
    {
        leds[index].r = cmdMessenger.readBinArg<byte>();
        leds[index].g = cmdMessenger.readBinArg<byte>();
        leds[index].b = cmdMessenger.readBinArg<byte>();
        FastLED.show();
        FastLED.delay(0);
    }
    else
    {
        cmdMessenger.sendBinCmd(kError, kSetPixel);
        return;
    }

    if (requested_ack)
        cmdMessenger.sendBinCmd(kAcknowledge, kSetPixel);
}

void cmdFillSolid()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    byte r, g, b;
    r = cmdMessenger.readBinArg<byte>();
    g = cmdMessenger.readBinArg<byte>();
    b = cmdMessenger.readBinArg<byte>();
    fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
    FastLED.show();
    FastLED.delay(0);

    if (requested_ack)
        cmdMessenger.sendBinCmd(kAcknowledge, kFillSolid);
}

void cmdClearEeprom()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    clearEeprom();

    if (requested_ack)
        cmdMessenger.sendBinCmd(kAcknowledge, kClearEeprom);
}

void cmdIsEepromReady()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    if (requested_ack)
        cmdMessenger.sendBinCmd(kAcknowledge, kIsEepromReady);

    cmdMessenger.sendBinCmd(kIsEepromReadyResult, isEepromReady());
}

void cmdResetEeprom()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    resetEeprom();

    if (requested_ack)
        cmdMessenger.sendBinCmd(kAcknowledge, kResetEeprom);
}

void cmdReturnEeprom()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    if (requested_ack)
        cmdMessenger.sendBinCmd(kAcknowledge, kReturnEeprom);

    cmdMessenger.sendCmdStart(kReturnEepromResult);
    for (uint16_t i; i < EEPROM.length(); i++)
        cmdMessenger.sendCmdBinArg((byte)EEPROM[i]);
    cmdMessenger.sendCmdEnd();

}

void cmdJumpToDfu()
{
    bool requested_ack = cmdMessenger.readBinArg<bool>();

    if (requested_ack)
        cmdMessenger.sendBinCmd(kAcknowledge, kJumpToDfu);
    
    
	TIMSK0 = 0;
	TCCR0B = 0;
	cli();
	MCUSR |= (1 << WDRF);
	wdt_enable(WDTO_15MS);
	while(1) {}
}
#pragma endregion

#pragma region EepromInteractors
typedef struct
{
  PyInt::Instruction patterns[EEPROM_PATTERN_COUNT][3][INSTRUCTION_ARRAY_SIZE];
  byte ready;
  byte currentPattern;
  byte currentBrightness;
} DeepStorage;

void clearEeprom()
{
    for (auto eeptr : EEPROM)
        eeptr.update(0);
}

bool isEepromReady()
{
    return EEPROM[offsetof(DeepStorage, ready)] == 42;
}

void resetEeprom()
{
    interp.r_instructions[0] = { PyInt::Opcode::LOAD_CONST, 255 };
    interp.r_instructions[1] = { PyInt::Opcode::RETURN_VALUE, 0 };

    interp.g_instructions[0] = { PyInt::Opcode::LOAD_CONST, 255 };
    interp.g_instructions[1] = { PyInt::Opcode::RETURN_VALUE, 0 };

    interp.b_instructions[0] = { PyInt::Opcode::LOAD_CONST, 255 };
    interp.b_instructions[1] = { PyInt::Opcode::RETURN_VALUE, 0 };

    for (byte i = 0; i < EEPROM_PATTERN_COUNT; i++)
        setPattern(i);

    setCurrentPattern(0);
    setCurrentBrightness(255);

    EEPROM[offsetof(DeepStorage, ready)] = 42;
}

byte getCurrentPattern()
{
    return EEPROM[offsetof(DeepStorage, currentPattern)];
}

void setCurrentPattern(byte index)
{
    EEPROM[offsetof(DeepStorage, currentPattern)] = index;
}

byte getCurrentBrightness()
{
    return EEPROM[offsetof(DeepStorage, currentBrightness)];
}

void setCurrentBrightness(byte brightness)
{
    EEPROM[offsetof(DeepStorage, currentBrightness)] = brightness;
    FastLED.setBrightness(brightness);
}

void getPattern(byte index)
{
    PyInt::Instruction pattern[3][INSTRUCTION_ARRAY_SIZE];
    EEPROM.get(offsetof(DeepStorage, patterns) + sizeof(PyInt::Instruction) * INSTRUCTION_ARRAY_SIZE * 3 * index, pattern);

    for (byte i = 0; i < INSTRUCTION_ARRAY_SIZE; i++)
    {
        interp.r_instructions[i].code = pattern[0][i].code;
        interp.r_instructions[i].arg = pattern[0][i].arg;

        interp.g_instructions[i].code = pattern[1][i].code;
        interp.g_instructions[i].arg = pattern[1][i].arg;

        interp.b_instructions[i].code = pattern[2][i].code;
        interp.b_instructions[i].arg = pattern[2][i].arg;
    }

    setCurrentPattern(index);
}

void setPattern(byte index)
{
    PyInt::Instruction pattern[3][INSTRUCTION_ARRAY_SIZE];
    EEPROM.get(offsetof(DeepStorage, patterns) + sizeof(PyInt::Instruction) * INSTRUCTION_ARRAY_SIZE * 3 * index, pattern);

    for (byte i = 0; i < INSTRUCTION_ARRAY_SIZE; i++)
    {
        pattern[0][i].code = interp.r_instructions[i].code;
        pattern[0][i].arg = interp.r_instructions[i].arg;

        pattern[1][i].code = interp.g_instructions[i].code;
        pattern[1][i].arg = interp.g_instructions[i].arg;

        pattern[2][i].code = interp.b_instructions[i].code;
        pattern[2][i].arg = interp.b_instructions[i].arg;
    }

    EEPROM.put(offsetof(DeepStorage, patterns) + sizeof(PyInt::Instruction) * INSTRUCTION_ARRAY_SIZE * 3 * index, pattern);
}
#pragma endregion
