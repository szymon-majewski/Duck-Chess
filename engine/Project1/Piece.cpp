#include "Piece.h"

char Piece::FindPieceSymbol(BitPiece piece)
{
	for (auto& it : Piece::PIECES_SYMBOLS_MAP)
	{
		if (it.second == piece)
		{
			return it.first;
		}
	}
}

const std::unordered_map<char, BitPiece> Piece::PIECES_SYMBOLS_MAP =
{
	{ 'p', Piece::Type::Pawn | Piece::Color::Black },
	{ 'P', Piece::Type::Pawn | Piece::Color::White },
	{ 'n', Piece::Type::Knight | Piece::Color::Black },
	{ 'N', Piece::Type::Knight | Piece::Color::White },
	{ 'b', Piece::Type::Bishop | Piece::Color::Black },
	{ 'B', Piece::Type::Bishop | Piece::Color::White },
	{ 'r', Piece::Type::Rook | Piece::Color::Black },
	{ 'R', Piece::Type::Rook | Piece::Color::White },
	{ 'q', Piece::Type::Queen | Piece::Color::Black },
	{ 'Q', Piece::Type::Queen | Piece::Color::White },
	{ 'k', Piece::Type::King | Piece::Color::Black },
	{ 'K', Piece::Type::King | Piece::Color::White },
	{ '@', Piece::Type::Duck | Piece::Color::Both },
	{ ' ', Piece::Type::None }
};