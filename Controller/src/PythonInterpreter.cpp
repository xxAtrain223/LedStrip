#include "PythonInterpreter.h"

namespace PyInt
{

    Interpreter::Interpreter()
    {
        operations[UNARY_POSITIVE] = &Interpreter::unary_positive;
        operations[UNARY_NEGATIVE] = &Interpreter::unary_negative;
        operations[UNARY_NOT] = &Interpreter::unary_not;
        operations[UNARY_INVERT] = &Interpreter::unary_invert;
        operations[BINARY_POWER] = &Interpreter::binary_power;
        operations[BINARY_MULTIPLY] = &Interpreter::binary_multiply;
        operations[BINARY_MODULO] = &Interpreter::binary_modulo;
        operations[BINARY_ADD] = &Interpreter::binary_add;
        operations[BINARY_SUBTRACT] = &Interpreter::binary_subtract;
        operations[BINARY_SUBSCR] = &Interpreter::binary_subscr;
        operations[BINARY_FLOOR_DIVIDE] = &Interpreter::binary_floor_divide;
        operations[BINARY_TRUE_DIVIDE] = &Interpreter::binary_true_divide;
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

    byte Interpreter::execute(InstructionSet set)
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

        byte rv = 0;
        for (byte i = 0; i < INSTRUCTION_ARRAY_SIZE && r_instructions[i-1].code != Opcode::RETURN_VALUE; i++)
            rv = (this->*operations[instructions[i].code])(instructions[i].arg);
        return rv;
    }

    byte Interpreter::unary_positive(byte arg)
    {
        return 0;
    }

    byte Interpreter::unary_negative(byte arg)
    {
        return 0;
    }

    byte Interpreter::unary_not(byte arg)
    {
        return 0;
    }

    byte Interpreter::unary_invert(byte arg)
    {
        return 0;
    }

    byte Interpreter::binary_power(byte arg)
    {
        return 0;
    }

    byte Interpreter::binary_multiply(byte arg)
    {
        return 0;
    }

    byte Interpreter::binary_modulo(byte arg)
    {
        return 0;
    }

    byte Interpreter::binary_add(byte arg)
    {
        stack.push(stack.pop() + stack.pop());
        return 0;
    }

    byte Interpreter::binary_subtract(byte arg)
    {
        return 0;
    }

    byte Interpreter::binary_subscr(byte arg)
    {
        return 0;
    }

    byte Interpreter::binary_floor_divide(byte arg)
    {
        return 0;
    }

    byte Interpreter::binary_true_divide(byte arg)
    {
        return 0;
    }

    byte Interpreter::binary_lshift(byte arg)
    {
        return 0;
    }

    byte Interpreter::binary_rshift(byte arg)
    {
        return 0;
    }

    byte Interpreter::binary_and(byte arg)
    {
        return 0;
    }

    byte Interpreter::binary_xor(byte arg)
    {
        return 0;
    }

    byte Interpreter::binary_or(byte arg)
    {
        return 0;
    }

    byte Interpreter::return_value(byte arg)
    {
        return stack.pop();
    }

    byte Interpreter::load_const(byte arg)
    {
        stack.push(arg);
        return 0;
    }

    byte Interpreter::load_name(byte arg)
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

    byte Interpreter::call_function(byte arg)
    {
        if (arg != 1)
        {
            return 0;
        }

        byte stackArg = stack.pop();
        byte func = stack.pop();

        byte val = 0;

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
