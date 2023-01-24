#pragma once

#include "PositionEvaluator.h"

class StandardPositionEvaluator : public PositionEvaluator
{
public:

	enum class TacticalFactors
	{
		KingLinedUpWithQueen
	};

	static std::unordered_map <Piece::Type, int32_t> piecesMaterial;
	static std::unordered_map <TacticalFactors, int32_t> tacticalFactorsMaterial;

	virtual Evaluation Evaluate(const Position& game);
};