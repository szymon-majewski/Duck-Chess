#pragma once

// Fixed width integer types
#include <cstdint>
#include <unordered_map>

typedef uint64_t BitBoard;
typedef uint8_t BitPiece;

enum class CastlingRights : uint8_t
{
	None = 0,
	WhiteKingside = 1,
	WhiteQueenside = 1<<1,
	BlackKingside = 1<<2,
	BlackQueenside = 1<<3
};

enum class PlayerColor
{
	White,
	Black
};

const std::string STARTING_POSITION_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

enum class Square : uint8_t
{
	None = 0,
	A1 = 1, A2, A3, A4, A5, A6, A7, A8,
	B1, B2, B3, B4, B5, B6, B7, B8,
	C1, C2, C3, C4, C5, C6, C7, C8,
	D1, D2, D3, D4, D5, D6, D7, D8,
	E1, E2, E3, E4, E5, E6, E7, E8,
	F1, F2, F3, F4, F5, F6, F7, F8,
	G1, G2, G3, G4, G5, G6, G7, G8,
	H1, H2, H3, H4, H5, H6, H7, H8,
};