#include "StandardPositionEvaluator.h"

std::unordered_map<StandardPositionEvaluator::TacticalFactors, Evaluation> StandardPositionEvaluator::tacticalFactorsMaterial =
{
    { StandardPositionEvaluator::TacticalFactors::KingLinedUpWithQueen, 350 }
};

Evaluation StandardPositionEvaluator::Evaluate(const Position& position)
{
    Evaluation result = position.materialDisparity;
    BitBoard currentBitBoard;
    int currentPieceSquare;

    for (int i = 0; i < 6; ++i)
    {
        currentBitBoard = position.board.bitBoards[i][Board::Color::Black];

        while (currentBitBoard)
        {
            currentPieceSquare = IndexOfLSB(currentBitBoard);
            result -= POSITIONAL_SCORES[i][currentPieceSquare];
            RemoveLSB(currentBitBoard);
        }

        currentBitBoard = position.board.bitBoards[i][Board::Color::White];

        while (currentBitBoard)
        {
            currentPieceSquare = IndexOfLSB(currentBitBoard);
            result += POSITIONAL_SCORES[i][64 - currentPieceSquare];
            RemoveLSB(currentBitBoard);
        }
    }

    return result;
}
