#include <Arduino.h>

#include "Stack.h"

#include "PythonOpcode.h"
#include "TrigFunctions.h"

#define INSTRUCTION_ARRAY_SIZE 16

namespace PyInt
{
	typedef struct
	{
		byte code;
		byte arg;
	} Instruction;

	enum Names
	{
		Time = 0,
		Index,
		Sin,
		Cos
	};

	enum InstructionSet
	{
		r = 0,
		g,
		b
	};

	class Interpreter
	{
	public:
		byte Time;        //Name 0
		byte Index;       //Name 1
		byte(*Sin)(byte); //Name 2
		byte(*Cos)(byte); //Name 3

		Instruction r_instructions[INSTRUCTION_ARRAY_SIZE];
		Instruction g_instructions[INSTRUCTION_ARRAY_SIZE];
		Instruction b_instructions[INSTRUCTION_ARRAY_SIZE];

		Interpreter();

		byte execute(InstructionSet set);

	protected:
		Stack<byte, 8> stack;
		byte(Interpreter::*operations[PythonOpcode::LENGTH])(byte);

		byte unary_positive(byte arg);
		byte unary_negative(byte arg);
		byte unary_not(byte arg);
		byte unary_invert(byte arg);
		byte binary_power(byte arg);
		byte binary_multiply(byte arg);
		byte binary_modulo(byte arg);
		byte binary_add(byte arg);
		byte binary_subtract(byte arg);
		byte binary_subscr(byte arg);
		byte binary_floor_divide(byte arg);
		byte binary_true_divide(byte arg);
		byte binary_lshift(byte arg);
		byte binary_rshift(byte arg);
		byte binary_and(byte arg);
		byte binary_xor(byte arg);
		byte binary_or(byte arg);
		byte return_value(byte arg);
		byte load_const(byte arg);
		byte load_name(byte arg);
		byte call_function(byte arg);
	};

}
