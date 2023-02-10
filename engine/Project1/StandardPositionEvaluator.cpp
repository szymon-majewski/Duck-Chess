#include "StandardPositionEvaluator.h"

std::unordered_map<Piece::Type, Evaluation> StandardPositionEvaluator::piecesMaterial =
{
	{ Piece::Type::Pawn, 100 },
	{ Piece::Type::Knight, 300 },
	{ Piece::Type::Bishop, 300 },
	{ Piece::Type::Rook, 500 },
	{ Piece::Type::Queen, 900 },
	{ Piece::Type::None, 0 }
};

std::unordered_map<StandardPositionEvaluator::TacticalFactors, Evaluation> StandardPositionEvaluator::tacticalFactorsMaterial =
{
	{ StandardPositionEvaluator::TacticalFactors::KingLinedUpWithQueen, 350 }
};

Evaluation StandardPositionEvaluator::Evaluate(const Position& position)
{
	Evaluation result = 0;
	PlayerColor opponentColor = position.playerToMove == PlayerColor::White ? PlayerColor::Black : PlayerColor::White;

	for (int y = 0; y < Board::HEIGHT; ++y)
	{
		for (int x = 0; x < Board::WIDTH; ++x)
		{
			Piece piece = position.board.pieces[y][x];

			// Check queen lineup with king
			bool queenLinedUpWithKing = false;

			if (piece.GetBitPiece() == (BitPiece)((uint8_t)Piece::Type::King | (uint8_t)opponentColor))
			{
				for (int innerX = 0; innerX < Board::WIDTH && !queenLinedUpWithKing; ++innerX)
				{
					if (position.board.pieces[y][innerX].GetBitPiece() == ((uint8_t)Piece::Type::Queen | (uint8_t)position.playerToMove))
					{
						result += tacticalFactorsMaterial[TacticalFactors::KingLinedUpWithQueen];
					}
				}

				for (int innerY = 0; innerY < Board::HEIGHT && !queenLinedUpWithKing; ++innerY)
				{
					if (position.board.pieces[innerY][x].GetBitPiece() == ((uint8_t)Piece::Type::Queen | (uint8_t)position.playerToMove))
					{
						result += tacticalFactorsMaterial[TacticalFactors::KingLinedUpWithQueen];
					}
				}
			}

			result += piecesMaterial[position.board.pieces[y][x].PieceType()] *
				(position.board.pieces[y][x].PieceColor() == Piece::Color::White ? 1 : -1);
		}
	}

	return result;
}