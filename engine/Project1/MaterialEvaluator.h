#pragma once

#include "PositionEvaluator.h"

class MaterialEvaluator : public PositionEvaluator
{
public:

	static std::unordered_map <Piece::Type, Evaluation> piecesMaterial;

	virtual Evaluation Evaluate(const Position& game);
};