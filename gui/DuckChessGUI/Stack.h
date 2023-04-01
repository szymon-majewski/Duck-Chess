#pragma once

#include "Definitions.h"

template<typename T, unsigned int N>
class Stack
{
private:

    T elements[N];

    // headIndex = -1 means that the stack is empty
    int headIndex = -1;

public:

    void Push(T element);
    T Pop();
};

template<typename T, unsigned int N>
inline void Stack<T, N>::Push(T element)
{
    ++headIndex;
    elements[headIndex] = element;
}

template<typename T, unsigned int N>
inline T Stack<T, N>::Pop()
{
    T popped = elements[headIndex];
    --headIndex;

    return popped;
}
