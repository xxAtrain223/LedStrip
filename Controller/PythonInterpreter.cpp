#include "PythonInterpreter.h"

namespace PyInt
{

    Interpreter::Interpreter()
    {
        operations[UNARY_NEGATIVE] = &Interpreter::unary_negative;
        operations[UNARY_INVERT] = &Interpreter::unary_invert;
        operations[BINARY_MULTIPLY] = &Interpreter::binary_multiply;
        operations[BINARY_MODULO] = &Interpreter::binary_modulo;
        operations[BINARY_ADD] = &Interpreter::binary_add;
        operations[BINARY_SUBTRACT] = &Interpreter::binary_subtract;
        operations[BINARY_FLOOR_DIVIDE] = &Interpreter::binary_floor_divide;
        operations[BINARY_LSHIFT] = &Interpreter::binary_lshift;
        operations[BINARY_RSHIFT] = &Interpreter::binary_rshift;
        operations[BINARY_AND] = &Interpreter::binary_and;
        operations[BINARY_XOR] = &Interpreter::binary_xor;
        operations[BINARY_OR] = &Interpreter::binary_or;
        operations[RETURN_VALUE] = &Interpreter::return_value;
        operations[LOAD_CONST] = &Interpreter::load_const;
        operations[LOAD_NAME] = &Interpreter::load_name;
        operations[CALL_FUNCTION] = &Interpreter::call_function;

    }

    uint8_t Interpreter::execute(InstructionSet set)
    {
        Instruction *instructions = nullptr;
        switch (set)
        {
        case InstructionSet::r:
            instructions = r_instructions;
            break;
        case InstructionSet::g:
            instructions = g_instructions;
            break;
        case InstructionSet::b:
            instructions = b_instructions;
            break;
        }

        uint8_t rv = 0;
        for (uint8_t i = 0; i < INSTRUCTION_ARRAY_SIZE && r_instructions[i-1].code != Opcode::RETURN_VALUE; i++)
            rv = (this->*operations[instructions[i].code])(instructions[i].arg);
        return rv;
    }

    uint8_t Interpreter::unary_negative(uint8_t arg)
    {
        return 0;
    }

    uint8_t Interpreter::unary_invert(uint8_t arg)
    {
        return 0;
    }

    uint8_t Interpreter::binary_multiply(uint8_t arg)
    {
        uint8_t tos = stack.pop();
        uint8_t tos1 = stack.pop();
        stack.push(tos1 * tos);
        return 0;
    }

    uint8_t Interpreter::binary_modulo(uint8_t arg)
    {
        uint8_t tos = stack.pop();
        uint8_t tos1 = stack.pop();
        stack.push(tos1 % tos);
        return 0;
    }

    uint8_t Interpreter::binary_add(uint8_t arg)
    {
        uint8_t tos = stack.pop();
        uint8_t tos1 = stack.pop();
        stack.push(tos1 + tos);
        return 0;
    }

    uint8_t Interpreter::binary_subtract(uint8_t arg)
    {
        uint8_t tos = stack.pop();
        uint8_t tos1 = stack.pop();
        stack.push(tos1 - tos);
        return 0;
    }

    uint8_t Interpreter::binary_floor_divide(uint8_t arg)
    {
        uint8_t tos = stack.pop();
        uint8_t tos1 = stack.pop();
        stack.push(tos1 / tos);
        return 0;
    }

    uint8_t Interpreter::binary_lshift(uint8_t arg)
    {
        uint8_t tos = stack.pop();
        uint8_t tos1 = stack.pop();
        stack.push(tos1 << tos);
        return 0;
    }

    uint8_t Interpreter::binary_rshift(uint8_t arg)
    {
        uint8_t tos = stack.pop();
        uint8_t tos1 = stack.pop();
        stack.push(tos1 >> tos);
        return 0;
    }

    uint8_t Interpreter::binary_and(uint8_t arg)
    {
        uint8_t tos = stack.pop();
        uint8_t tos1 = stack.pop();
        stack.push(tos1 & tos);
        return 0;
    }

    uint8_t Interpreter::binary_xor(uint8_t arg)
    {
        uint8_t tos = stack.pop();
        uint8_t tos1 = stack.pop();
        stack.push(tos1 ^ tos);
        return 0;
    }

    uint8_t Interpreter::binary_or(uint8_t arg)
    {
        uint8_t tos = stack.pop();
        uint8_t tos1 = stack.pop();
        stack.push(tos1 | tos);
        return 0;
    }

    uint8_t Interpreter::return_value(uint8_t arg)
    {
        return stack.pop();
    }

    uint8_t Interpreter::load_const(uint8_t arg)
    {
        stack.push(arg);
        return 0;
    }

    uint8_t Interpreter::load_name(uint8_t arg)
    {
        switch (arg)
        {
        case Names::Time:
            stack.push(Time);
            break;
        case Names::Index:
            stack.push(Index);
            break;
        case Names::Sin:
        case Names::Cos:
            stack.push(arg);
            break;
        }

        return 0;
    }

    uint8_t Interpreter::call_function(uint8_t arg)
    {
        if (arg != 1)
        {
            return 0;
        }

        uint8_t stackArg = stack.pop();
        uint8_t func = stack.pop();

        uint8_t val = 0;

        switch (func)
        {
        case Names::Sin:
            val = Sin(stackArg);
            break;
        case Names::Cos:
            val = Cos(stackArg);
            break;
        }

        stack.push(val);

        return val;
    }
}
