#pragma once

#include "PositionEvaluator.h"

class StandardPositionEvaluator : public PositionEvaluator
{
public:

	static std::unordered_map <Piece::Type, uint32_t> piecesMaterial;
	virtual Evaluation Evaluate(const Position& game);
};