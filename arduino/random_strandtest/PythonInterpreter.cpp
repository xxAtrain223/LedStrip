#include "PythonInterpreter.h"
#include "TrigFunctions.h"

namespace PyInt
{

	Interpreter::Interpreter()
	{
		operations[UNARY_POSITIVE] = &unary_positive;
		operations[UNARY_NEGATIVE] = &unary_negative;
		operations[UNARY_NOT] = &unary_not;
		operations[UNARY_INVERT] = &unary_invert;
		operations[BINARY_POWER] = &binary_power;
		operations[BINARY_MULTIPLY] = &binary_multiply;
		operations[BINARY_MODULO] = &binary_modulo;
		operations[BINARY_ADD] = &binary_add;
		operations[BINARY_SUBTRACT] = &binary_subtract;
		operations[BINARY_SUBSCR] = &binary_subscr;
		operations[BINARY_FLOOR_DIVIDE] = &binary_floor_divide;
		operations[BINARY_TRUE_DIVIDE] = &binary_true_divide;
		operations[BINARY_LSHIFT] = &binary_lshift;
		operations[BINARY_RSHIFT] = &binary_rshift;
		operations[BINARY_AND] = &binary_and;
		operations[BINARY_XOR] = &binary_xor;
		operations[BINARY_OR] = &binary_or;
		operations[RETURN_VALUE] = &return_value;
		operations[LOAD_CONST] = &load_const;
		operations[LOAD_NAME] = &load_name;
		operations[CALL_FUNCTION] = &call_function;
	}

	byte Interpreter::execute(InstructionSet set)
	{
		byte rv = 0;
		switch (set)
		{
		case InstructionSet::r:
			for (byte i = 0; i < INSTRUCTION_ARRAY_SIZE && r_instructions[i-1].code != PythonOpcode::RETURN_VALUE; i++)
				rv = (this->*operations[r_instructions[i].code])(r_instructions[i].arg);
			break;
		case InstructionSet::g:
			for (byte i = 0; i < INSTRUCTION_ARRAY_SIZE && g_instructions[i-1].code != PythonOpcode::RETURN_VALUE; i++)
				rv = (this->*operations[g_instructions[i].code])(g_instructions[i].arg);
			break;
		case InstructionSet::b:
			for (byte i = 0; i < INSTRUCTION_ARRAY_SIZE && b_instructions[i-1].code != PythonOpcode::RETURN_VALUE; i++)
				rv = (this->*operations[b_instructions[i].code])(b_instructions[i].arg);
			break;
		}
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
