#define INSTRUCTION_ARRAY_SIZE 16
typedef unsigned char byte;

namespace PyInt
{
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

  template <typename T, byte MAX_SIZE>
  class Stack
  {
    T arr[MAX_SIZE];
    byte top = 0;
    byte max = MAX_SIZE;
  
  public:
    void push(const T element)
    {
      arr[top++] = element;
    }
  
    T pop()
    {
      return arr[--top];
    }
  
    T peek() const
    {
      return arr[top - 1];
    }
  
    bool isEmpty() const
    {
      return top == 0;
    }
  
    bool isFull() const
    {
      return top == max;
    }
  
    byte count() const
    {
      return top;
    }
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
