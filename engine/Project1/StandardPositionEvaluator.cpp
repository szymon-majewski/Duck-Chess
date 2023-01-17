#include "StandardPositionEvaluator.h"

std::unordered_map<Piece::Type, double> StandardPositionEvaluator::piecesMaterial =
{
	{ Piece::Type::Pawn, 1.0 },
	{ Piece::Type::Knight, 5.5 },
	{ Piece::Type::Bishop, 2.5 },
	{ Piece::Type::Rook, 3.5 },
	{ Piece::Type::Queen, 7.0 },
	{ Piece::Type::None, 0.0 }
};

StandardPositionEvaluator::StandardPositionEvaluator(Game* game) : PositionEvaluator(game) {}

double StandardPositionEvaluator::Evaluate()
{
	double result = 0.0;

	for (int y = 0; y < Board::HEIGHT; ++y)
	{
		for (int x = 0; x < Board::WIDTH; ++x)
		{
			// Duck because it has all 1 bits, Color::Both for the same reason
			result += piecesMaterial[(Piece::Type)((uint8_t)game->board.pieces[y][x].GetPiece() & (uint8_t)Piece::Type::Duck)] * 
				((Piece::Color)((uint8_t)game->board.pieces[y][x].GetPiece() & (uint8_t)Piece::Color::Both) == Piece::Color::White ? 1 : -1);
		}
	}

	return result;
}