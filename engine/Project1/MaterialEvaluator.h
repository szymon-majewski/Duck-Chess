#pragma once

#include "PositionEvaluator.h"

class MaterialEvaluator : public PositionEvaluator
{
public:

	static std::unordered_map <Piece::Type, int32_t> piecesMaterial;

	virtual Evaluation Evaluate(const Position& game);
};