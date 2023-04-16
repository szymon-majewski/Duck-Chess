#include "StandardPositionEvaluator.h"

std::unordered_map<Board::Type, Evaluation> StandardPositionEvaluator::piecesMaterial =
{
    { Board::Type::Pawns, 100 },
    { Board::Type::Knights, 300 },
    { Board::Type::Bishops, 300 },
    { Board::Type::Rooks, 500 },
    { Board::Type::Queens, 900 },
    { Board::Type::Kings, 0 }
};

std::unordered_map<StandardPositionEvaluator::TacticalFactors, Evaluation> StandardPositionEvaluator::tacticalFactorsMaterial =
{
    { StandardPositionEvaluator::TacticalFactors::KingLinedUpWithQueen, 350 }
};

Evaluation StandardPositionEvaluator::Evaluate(const Position& position)
{
    Evaluation result = 0;
    PlayerColor opponentColor = position.playerToMove == PlayerColor::White ? PlayerColor::Black : PlayerColor::White;

//    for (int y = 0; y < Board::HEIGHT; ++y)
//    {
//        for (int x = 0; x < Board::WIDTH; ++x)
//        {
//            Piece piece = position.board.pieces[y][x];

//            // Check queen lineup with king
//            bool queenLinedUpWithKing = false;

//            if (piece.GetBitPiece() == (BitPiece)((uint8_t)Piece::Type::King | (uint8_t)opponentColor))
//            {
//                for (int innerX = 0; innerX < Board::WIDTH && !queenLinedUpWithKing; ++innerX)
//                {
//                    if (position.board.pieces[y][innerX].GetBitPiece() == ((uint8_t)Piece::Type::Queen | (uint8_t)position.playerToMove))
//                    {
//                        result += tacticalFactorsMaterial[TacticalFactors::KingLinedUpWithQueen];
//                    }
//                }

//                for (int innerY = 0; innerY < Board::HEIGHT && !queenLinedUpWithKing; ++innerY)
//                {
//                    if (position.board.pieces[innerY][x].GetBitPiece() == ((uint8_t)Piece::Type::Queen | (uint8_t)position.playerToMove))
//                    {
//                        result += tacticalFactorsMaterial[TacticalFactors::KingLinedUpWithQueen];
//                    }
//                }
//            }

//            result += piecesMaterial[position.board.pieces[y][x].PieceType()] *
//                (position.board.pieces[y][x].PieceColor() == Piece::Color::White ? 1 : -1);
//        }
//    }

    return result;
}
