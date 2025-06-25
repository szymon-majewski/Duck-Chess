#pragma once

#include <array>
#include "Position.h"

class PositionEvaluator
{
public:

    virtual Evaluation Evaluate(const Position& game) = 0;

    static std::unordered_map <Board::Type, Evaluation> piecesMaterial;
    static const std::array<std::array<int8_t, Board::SQUARES_COUNT>, 6> POSITIONAL_SCORES;
};
