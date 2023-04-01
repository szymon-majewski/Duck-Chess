#pragma once

#include "Position.h"

class PositionEvaluator
{
public:

    virtual Evaluation Evaluate(const Position& game) = 0;

    static std::unordered_map <Piece::Type, Evaluation> piecesMaterial;
};
