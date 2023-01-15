#pragma once

#include "Definitions.h"

class Piece
{
public:
	// Piece types bit representations
	enum Type : uint8_t
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
	enum Color : uint8_t
	{
		White = 8,		//01 000
		Black = 16,		//10 000
		Both = 24		//11 000
	};

private:
	BitPiece piece;

public:
	inline BitPiece GetPiece() { return piece; }
	inline void SetPiece(const BitPiece bitPiece) { piece = bitPiece; }

	const static std::unordered_map<char, BitPiece> PIECES_SYMBOLS_MAP;

	static char FindPieceSymbol(BitPiece piece);
};