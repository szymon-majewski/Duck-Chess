#pragma once

#include "Definitions.h"

class Piece
{
public:

	// Piece types bit representations
	enum class Type : BitPiece
	{
		None = 0,		//00 000
		Pawn = 1,		//00 001
		Knight = 2,		//00 010
		Bishop = 3,		//00 011
		Rook = 4,		//00 100
		Queen = 5,		//00 101
		King = 6,		//00 110
		Duck = 7		//00 111
	};

	// Piece color bit representations
	enum class Color : BitPiece
	{
		White = 8,		//01 000
		Black = 16,		//10 000
		Both = 24		//11 000
	};

private:

	BitPiece piece;

public:

	inline const BitPiece& GetBitPiece() const { return piece; }
	inline void SetBitPiece(const BitPiece bitPiece) { piece = bitPiece; }

	Piece::Type PieceType() const;
	Piece::Color PieceColor() const;

	const static std::unordered_map<char, BitPiece> PIECES_SYMBOLS_MAP;

	static char FindPieceSymbol(BitPiece piece);
};