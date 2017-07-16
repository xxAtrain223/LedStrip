#ifndef PYTHONINTERPRETER_H
#define PYTHONINTERPRETER_H

#include <stdint.h>

#define INSTRUCTION_ARRAY_SIZE 16

namespace PyInt
{
    typedef struct
    {
        uint8_t code;
        uint8_t arg;
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

    enum Opcode {
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

    template <typename T, uint8_t MAX_SIZE>
    class Stack
    {
        enum Error
        {
            NoError,
            Overflow,
            Underflow
        };

        T arr[MAX_SIZE];
        uint8_t top = 0;
        uint8_t max = MAX_SIZE;

        Error currentError = Error::NoError;

    public:
        void push(const T element)
        {
            if (!isFull())
            {
                currentError = Error::NoError;
                arr[top++] = element;
            }
            else
            {
                currentError = Error::Overflow;
            }
        }

        T pop()
        {
            if (!isEmpty())
            {
                currentError = Error::NoError;
                return arr[--top];
            }
            else
            {
                currentError = Error::Underflow;
                return T();
            }
        }

        T peek() const
        {
            if (!isEmpty())
            {
                currentError = Error::NoError;
                return arr[top - 1];
            }
            else
            {
                currentError = Error::Underflow;
                return T();
            }
        }

        bool isEmpty() const
        {
            return top == 0;
        }

        bool isFull() const
        {
            return top == max;
        }

        uint8_t count() const
        {
            return top;
        }

        Error getError()
        {
            return currentError;
        }
    };

    class Interpreter
    {
    public:
        uint8_t Time;        //Name 0
        uint8_t Index;       //Name 1
        uint8_t(*Sin)(uint8_t); //Name 2
        uint8_t(*Cos)(uint8_t); //Name 3

        Instruction r_instructions[INSTRUCTION_ARRAY_SIZE];
        Instruction g_instructions[INSTRUCTION_ARRAY_SIZE];
        Instruction b_instructions[INSTRUCTION_ARRAY_SIZE];

        Interpreter();

        uint8_t execute(InstructionSet set);

    protected:
        Stack<uint8_t, 8> stack;
        uint8_t(Interpreter::*operations[Opcode::LENGTH])(uint8_t);

        uint8_t unary_positive(uint8_t arg);
        uint8_t unary_negative(uint8_t arg);
        uint8_t unary_not(uint8_t arg);
        uint8_t unary_invert(uint8_t arg);
        uint8_t binary_power(uint8_t arg);
        uint8_t binary_multiply(uint8_t arg);
        uint8_t binary_modulo(uint8_t arg);
        uint8_t binary_add(uint8_t arg);
        uint8_t binary_subtract(uint8_t arg);
        uint8_t binary_subscr(uint8_t arg);
        uint8_t binary_floor_divide(uint8_t arg);
        uint8_t binary_true_divide(uint8_t arg);
        uint8_t binary_lshift(uint8_t arg);
        uint8_t binary_rshift(uint8_t arg);
        uint8_t binary_and(uint8_t arg);
        uint8_t binary_xor(uint8_t arg);
        uint8_t binary_or(uint8_t arg);
        uint8_t return_value(uint8_t arg);
        uint8_t load_const(uint8_t arg);
        uint8_t load_name(uint8_t arg);
        uint8_t call_function(uint8_t arg);
    };

}

#endif//PYTHONINTERPRETER_H
