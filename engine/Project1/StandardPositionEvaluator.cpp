#include "StandardPositionEvaluator.h"

//std::unordered_map<Piece::Type, uint32_t> StandardPositionEvaluator::piecesMaterial =
//{
//	{ Piece::Type::Pawn, 100 },
//	{ Piece::Type::Knight, 550 },
//	{ Piece::Type::Bishop, 250 },
//	{ Piece::Type::Rook, 350 },
//	{ Piece::Type::Queen, 700 },
//	{ Piece::Type::None, 0 }
//};

std::unordered_map<Piece::Type, uint32_t> StandardPositionEvaluator::piecesMaterial =
{
	{ Piece::Type::Pawn, 100 },
	{ Piece::Type::Knight, 300 },
	{ Piece::Type::Bishop, 300 },
	{ Piece::Type::Rook, 500 },
	{ Piece::Type::Queen, 900 },
	{ Piece::Type::None, 0 }
};

int32_t StandardPositionEvaluator::Evaluate(const Position& position)
{
	int32_t result = 0;

	for (int y = 0; y < Board::HEIGHT; ++y)
	{
		for (int x = 0; x < Board::WIDTH; ++x)
		{
			// Duck because it has all 1 bits, Color::Both for the same reason
			result += piecesMaterial[position.board.pieces[y][x].PieceType()] *
				(position.board.pieces[y][x].PieceColor() == Piece::Color::White ? 1 : -1);
		}
	}

	return result;
}