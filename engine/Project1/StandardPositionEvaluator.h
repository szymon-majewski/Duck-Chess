#pragma once

#include "PositionEvaluator.h"

class StandardPositionEvaluator : public PositionEvaluator
{
public:

	double Evaluate(const Game& game);
};