#pragma once

#include <array>
#include "PositionEvaluator.h"

class StandardPositionEvaluator : public PositionEvaluator
{
public:

    enum class TacticalFactors
    {
        KingLinedUpWithQueen
    };

    static std::unordered_map <TacticalFactors, Evaluation> tacticalFactorsMaterial;

    virtual Evaluation Evaluate(const Position& game) override;
};
