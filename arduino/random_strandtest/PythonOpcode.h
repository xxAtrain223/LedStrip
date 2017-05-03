#ifndef PYTHON_OPCODE_H
#define PYTHON_OPCODE_H

enum PythonOpcode {
	UNARY_POSITIVE = 0,
	UNARY_NEGATIVE,
	UNARY_NOT,
	UNARY_INVERT,
	BINARY_POWER,
	BINARY_MULTIPLY,
	BINARY_MODULO,
	BINARY_ADD,
	BINARY_SUBTRACT,
	BINARY_SUBSCR,
	BINARY_FLOOR_DIVIDE,
	BINARY_TRUE_DIVIDE,
	BINARY_LSHIFT,
	BINARY_RSHIFT,
	BINARY_AND,
	BINARY_XOR,
	BINARY_OR,
	RETURN_VALUE,
	LOAD_CONST,
	LOAD_NAME,
	CALL_FUNCTION,
	LENGTH
};

#endif //PYTHON_OPCODE_H
