#include "Arduino/EEPROM.h"
#include "EepromInteractors.h"

extern PyInt::Interpreter interp;

// Loop through the EEPROM and set every byte to 0
void clearEeprom()
{
    for (auto eeptr : EEPROM)
        eeptr.update(0);
}

// Check if the EEPROM is ready
bool isEepromReady()
{
    return EEPROM[offsetof(DeepStorage, ready)] == 42;
}

// Reset the eeprom to a valid configuration
void resetEeprom()
{
    // Fill all of the patterns with White
    interp.r_instructions[0] = { PyInt::Opcode::LOAD_CONST, 255 };
    interp.r_instructions[1] = { PyInt::Opcode::RETURN_VALUE, 0 };

    interp.g_instructions[0] = { PyInt::Opcode::LOAD_CONST, 255 };
    interp.g_instructions[1] = { PyInt::Opcode::RETURN_VALUE, 0 };

    interp.b_instructions[0] = { PyInt::Opcode::LOAD_CONST, 255 };
    interp.b_instructions[1] = { PyInt::Opcode::RETURN_VALUE, 0 };

    for (uint8_t i = 0; i < EEPROM_PATTERN_COUNT; i++)
        setPattern(i);

    // Set default current pattern
    setCurrentPattern(0);

    // Set the ready byte
    EEPROM[offsetof(DeepStorage, ready)] = 42;
}

// Get the current pattern from EEPROM
uint8_t getCurrentPattern()
{
    return EEPROM[offsetof(DeepStorage, currentPattern)];
}

// Set the current pattern in EEPROM
void setCurrentPattern(uint8_t index)
{
    EEPROM[offsetof(DeepStorage, currentPattern)] = index;
}

// Load the pattern from EEPROM into the interpreter
void getPattern(uint8_t index)
{
    PyInt::Instruction pattern[3][INSTRUCTION_ARRAY_SIZE];
    EEPROM.get(offsetof(DeepStorage, patterns) + sizeof(PyInt::Instruction) * INSTRUCTION_ARRAY_SIZE * 3 * index, pattern);

    for (uint8_t i = 0; i < INSTRUCTION_ARRAY_SIZE; i++)
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

// Save the pattern from the interpreter into EEPROM
void setPattern(uint8_t index)
{
    PyInt::Instruction pattern[3][INSTRUCTION_ARRAY_SIZE];
    EEPROM.get(offsetof(DeepStorage, patterns) + sizeof(PyInt::Instruction) * INSTRUCTION_ARRAY_SIZE * 3 * index, pattern);

    for (uint8_t i = 0; i < INSTRUCTION_ARRAY_SIZE; i++)
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
