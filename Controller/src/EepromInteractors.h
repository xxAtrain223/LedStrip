#ifndef EEPROMINTERACTORS_H
#define EEPROMINTERACTORS_H

#include <stdint.h>

#include "PythonInterpreter.h"

#define EEPROM_PATTERN_COUNT 8

typedef struct
{
  PyInt::Instruction patterns[EEPROM_PATTERN_COUNT][3][INSTRUCTION_ARRAY_SIZE];
  uint8_t ready;
  uint8_t currentPattern;
} DeepStorage;

void clearEeprom();
bool isEepromReady();
void resetEeprom();
uint8_t getCurrentPattern();
void setCurrentPattern(uint8_t index);
void getPattern(uint8_t index);
void setPattern(uint8_t index);

#endif//EEPROMINTERACTORS_H
