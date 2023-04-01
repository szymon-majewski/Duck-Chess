#include "PositionEvaluator.h"

std::unordered_map<Piece::Type, Evaluation> PositionEvaluator::piecesMaterial =
{
    { Piece::Type::Pawn, 100 },
    { Piece::Type::Knight, 550 },
    { Piece::Type::Bishop, 250 },
    { Piece::Type::Rook, 400 },
    { Piece::Type::Queen, 700 },
    { Piece::Type::None, 0 }
};
