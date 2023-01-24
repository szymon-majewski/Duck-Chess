#include "MaterialEvaluator.h"

std::unordered_map<Piece::Type, int32_t> MaterialEvaluator::piecesMaterial =
{
	{ Piece::Type::Pawn, 100 },
	{ Piece::Type::Knight, 550 },
	{ Piece::Type::Bishop, 250 },
	{ Piece::Type::Rook, 400 },
	{ Piece::Type::Queen, 700 },
	{ Piece::Type::None, 0 }
};

Evaluation MaterialEvaluator::Evaluate(const Position& position)
{
	Evaluation result = 0;

	for (int y = 0; y < Board::HEIGHT; ++y)
	{
		for (int x = 0; x < Board::WIDTH; ++x)
		{
			result += piecesMaterial[position.board.pieces[y][x].PieceType()] *
				(position.board.pieces[y][x].PieceColor() == Piece::Color::White ? 1 : -1);
		}
	}

	return result;
}
