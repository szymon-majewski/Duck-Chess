#pragma once

#include "PositionEvaluator.h"

class StandardPositionEvaluator : public PositionEvaluator
{
public:

    enum class TacticalFactors
    {
        KingLinedUpWithQueen
    };

    static std::unordered_map <Piece::Type, Evaluation> piecesMaterial;
    static std::unordered_map <TacticalFactors, Evaluation> tacticalFactorsMaterial;

    virtual Evaluation Evaluate(const Position& game);
};
