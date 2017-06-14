#ifndef STACK_H
#define STACK_H

typedef unsigned char byte;

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

#endif//STACK_H
