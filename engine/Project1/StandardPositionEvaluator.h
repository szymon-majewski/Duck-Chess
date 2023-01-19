#pragma once

#include "PositionEvaluator.h"

class StandardPositionEvaluator : public PositionEvaluator
{
public:

	static std::unordered_map <Piece::Type, double> piecesMaterial;
	virtual double Evaluate(const Position& game);
};