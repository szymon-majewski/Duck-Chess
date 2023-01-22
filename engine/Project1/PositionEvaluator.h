#pragma once

#include "Position.h"

class PositionEvaluator
{
public:

	virtual int32_t Evaluate(const Position& game) = 0;
};