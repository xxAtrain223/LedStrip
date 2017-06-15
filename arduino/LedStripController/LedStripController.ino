#include <EEPROM.h>

#include <CmdMessenger.h>
#include <FastLED.h>

#include "PythonInterpreter.h"

#define EEPROM_PATTERN_COUNT 8

#define PIN 6
#define NUM_LEDS 60
CRGB leds[NUM_LEDS];

CmdMessenger cmdMessenger = CmdMessenger(Serial);
PyInt::Interpreter interpreter;

typedef struct
{
  PyInt::Instruction patterns[EEPROM_PATTERN_COUNT][3][INSTRUCTION_ARRAY_SIZE];
  byte ready;
  byte currentPattern;
  byte currentBrightness;
} DeepStorage;

enum
{
  kPing,
  kPong,
  kUnknown,
  kAcknowledge,
  kError,
  kSetPixel,
  kFillSolid,
};

void attachCommandCallbacks()
{
  // Attach callback methods
  cmdMessenger.attach(OnUnknown);
  cmdMessenger.attach(kPing, OnPing);
  cmdMessenger.attach(kUnknown, OnUnknown);
  cmdMessenger.attach(kAcknowledge, OnAcknowledge);
  cmdMessenger.attach(kError, OnError);
  cmdMessenger.attach(kSetPixel, OnSetPixel);
  cmdMessenger.attach(kFillSolid, OnFillSolid);
}

void waitForArg()
{
  //while(!cmdMessenger.available())
  //  delay(1);
}

// ------------------  C A L L B A C K S -----------------------

void OnPing()
{
  cmdMessenger.sendCmd(kPong);
}

void OnUnknown()
{
  if (cmdMessenger.commandID() != kUnknown)
  {
    cmdMessenger.sendBinCmd(kUnknown, (uint16_t)cmdMessenger.commandID());
  }
  else
  {
    cmdMessenger.sendBinCmd(kError, kUnknown);
  }
}

void OnAcknowledge()
{
  cmdMessenger.sendBinCmd(kError, kAcknowledge);
}

void OnError()
{
  cmdMessenger.sendBinCmd(kError, kError);
}



void OnSetPixel()
{
  bool requested_ack;
  do
  {
    requested_ack = cmdMessenger.readBinArg<bool>();
  } while(!cmdMessenger.isArgOk());

  byte index;
  do
  {
    index = cmdMessenger.readBinArg<byte>();
  } while(!cmdMessenger.isArgOk());
  
  if (index >= 0 && index < NUM_LEDS)
  {
    byte r, g, b;

    do
    {
      r = cmdMessenger.readBinArg<byte>();
    } while(!cmdMessenger.isArgOk());

    do
    {
      g = cmdMessenger.readBinArg<byte>();
    } while(!cmdMessenger.isArgOk());

    do
    {
      b = cmdMessenger.readBinArg<byte>();
    } while(!cmdMessenger.isArgOk());
    
    leds[index].r = r;
    leds[index].g = g;
    leds[index].b = b;
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

void OnFillSolid()
{
  bool requested_ack = cmdMessenger.readBinArg<bool>();
  
  byte r = cmdMessenger.readBinArg<byte>();
  byte g = cmdMessenger.readBinArg<byte>();
  byte b = cmdMessenger.readBinArg<byte>();
  fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
  FastLED.show();
  FastLED.delay(0);

  if (requested_ack)
    cmdMessenger.sendBinCmd(kAcknowledge, kFillSolid);
}

// ------------------ M A I N  ----------------------

// Setup function
void setup() 
{
  // Listen on serial connection for messages from the pc
  Serial.begin(9600); 

  // Attach my application's user-defined callback methods
  attachCommandCallbacks();

  FastLED.addLeds<NEOPIXEL, PIN>(leds, NUM_LEDS);

  interpreter.Sin = (byte(*)(byte))sin8;
  interpreter.Cos = (byte(*)(byte))cos8;

  interpreter.Time = 0;
  interpreter.Index = 0;

  if (!isEepromReady())
    resetEeprom();

  getPattern(getCurrentPattern());
  FastLED.setBrightness(getCurrentBrightness());
}
// Loop function
void loop() 
{
  // Process incoming serial data, and perform callbacks
  cmdMessenger.feedinSerialData(); 
} 

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
  interpreter.r_instructions[0] = { PyInt::PythonOpcode::LOAD_CONST, 255 };
  interpreter.r_instructions[1] = { PyInt::PythonOpcode::RETURN_VALUE, 0 };

  interpreter.g_instructions[0] = { PyInt::PythonOpcode::LOAD_CONST, 255 };
  interpreter.g_instructions[1] = { PyInt::PythonOpcode::RETURN_VALUE, 0 };

  interpreter.b_instructions[0] = { PyInt::PythonOpcode::LOAD_CONST, 255 };
  interpreter.b_instructions[1] = { PyInt::PythonOpcode::RETURN_VALUE, 0 };

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
    interpreter.r_instructions[i].code = pattern[0][i].code;
    interpreter.r_instructions[i].arg = pattern[0][i].arg;

    interpreter.g_instructions[i].code = pattern[1][i].code;
    interpreter.g_instructions[i].arg = pattern[1][i].arg;

    interpreter.b_instructions[i].code = pattern[2][i].code;
    interpreter.b_instructions[i].arg = pattern[2][i].arg;
  }

  setCurrentPattern(index);
}

void setPattern(byte index)
{
  PyInt::Instruction pattern[3][INSTRUCTION_ARRAY_SIZE];
  EEPROM.get(offsetof(DeepStorage, patterns) + sizeof(PyInt::Instruction) * INSTRUCTION_ARRAY_SIZE * 3 * index, pattern);

  for (byte i = 0; i < INSTRUCTION_ARRAY_SIZE; i++)
  {
    pattern[0][i].code = interpreter.r_instructions[i].code;
    pattern[0][i].arg = interpreter.r_instructions[i].arg;

    pattern[1][i].code = interpreter.g_instructions[i].code;
    pattern[1][i].arg = interpreter.g_instructions[i].arg;

    pattern[2][i].code = interpreter.b_instructions[i].code;
    pattern[2][i].arg = interpreter.b_instructions[i].arg;
  }

  EEPROM.put(offsetof(DeepStorage, patterns) + sizeof(PyInt::Instruction) * INSTRUCTION_ARRAY_SIZE * 3 * index, pattern);
}
