#include "PositionEvaluator.h"

std::unordered_map<Board::Type, Evaluation> PositionEvaluator::piecesMaterial =
{
    { Board::Type::Pawns, 100 },
    { Board::Type::Knights, 550 },
    { Board::Type::Bishops, 250 },
    { Board::Type::Rooks, 400 },
    { Board::Type::Queens, 700 },
    { Board::Type::Kings, 0 }
};
