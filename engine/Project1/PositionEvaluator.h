#pragma once

#include "Position.h"

class PositionEvaluator
{
public:

	virtual Evaluation Evaluate(const Position& game) = 0;
};