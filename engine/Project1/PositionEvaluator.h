#pragma once

#include "Game.h"

class PositionEvaluator
{
public:
	virtual double Evaluate(const Game& game) = 0;
};