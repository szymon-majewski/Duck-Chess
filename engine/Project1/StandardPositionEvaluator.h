#pragma once

#include "PositionEvaluator.h"

class StandardPositionEvaluator : public PositionEvaluator
{
public:

	StandardPositionEvaluator(Game* game);
	virtual double Evaluate();
};