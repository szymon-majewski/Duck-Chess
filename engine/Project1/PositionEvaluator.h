#pragma once

#include "Position.h"

class PositionEvaluator
{
public:

	virtual double Evaluate(const Position& game) = 0;
};