#pragma once

#include "PositionEvaluator.h"

class MaterialEvaluator : public PositionEvaluator
{
public:

    virtual Evaluation Evaluate(const Position& game) override;
};
