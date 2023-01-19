#include "Piece.h"

Piece::Type Piece::PieceType() const
{
	return (Piece::Type)(piece & (uint8_t)Piece::Type::Duck);
}

Piece::Color Piece::PieceColor() const
{
	return (Piece::Color)(piece & (uint8_t)Piece::Color::Both);
}

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
	{ 'p', (uint8_t)Piece::Type::Pawn | (uint8_t)Piece::Color::Black },
	{ 'P', (uint8_t)Piece::Type::Pawn | (uint8_t)Piece::Color::White },
	{ 'n', (uint8_t)Piece::Type::Knight | (uint8_t)Piece::Color::Black },
	{ 'N', (uint8_t)Piece::Type::Knight | (uint8_t)Piece::Color::White },
	{ 'b', (uint8_t)Piece::Type::Bishop | (uint8_t)Piece::Color::Black },
	{ 'B', (uint8_t)Piece::Type::Bishop | (uint8_t)Piece::Color::White },
	{ 'r', (uint8_t)Piece::Type::Rook | (uint8_t)Piece::Color::Black },
	{ 'R', (uint8_t)Piece::Type::Rook | (uint8_t)Piece::Color::White },
	{ 'q', (uint8_t)Piece::Type::Queen | (uint8_t)Piece::Color::Black },
	{ 'Q', (uint8_t)Piece::Type::Queen | (uint8_t)Piece::Color::White },
	{ 'k', (uint8_t)Piece::Type::King | (uint8_t)Piece::Color::Black },
	{ 'K', (uint8_t)Piece::Type::King | (uint8_t)Piece::Color::White },
	{ 'D', (uint8_t)Piece::Type::Duck | (uint8_t)Piece::Color::Both },
	{ ' ', (uint8_t)Piece::Type::None }
};