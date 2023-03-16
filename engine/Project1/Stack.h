#pragma once

#include "Definitions.h"

template<typename T>
class Stack
{
private:

	T elements[MAX_GAME_LENGTH];

	// headIndex = -1 means that the stack is empty
	int headIndex = -1;

public:

	void Push(T element);
	T Pop();
};

template <typename T>
inline void Stack<T>::Push(T element)
{
	++headIndex;
	elements[headIndex] = element;
}

template <typename T>
inline T Stack<T>::Pop()
{
	T popped = elements[headIndex];
	--headIndex;

	return popped;
}