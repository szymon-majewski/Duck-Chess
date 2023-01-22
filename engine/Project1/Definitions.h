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

// These are the same values as pieces colors so they can be casted to one another (not None)
enum class PlayerColor
{
	None = 0,
	White = 8,
	Black = 16
};

const std::string STARTING_POSITION_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

enum class Square : uint8_t
{
	None = 0,
	A1 = 1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A8, B8, C8, D8, E8, F8, G8, H8
};

enum class DirectionOffsets : int8_t
{
	North = 8,
	East = 1,
	South = -8,
	West = -1,
	NorthEast = 9,
	SouthEast = -7,
	SouthWest = -9,
	NorthWest = 7
};

enum class Files : uint8_t
{
	FileA = 0,
	FileB,
	FileC,
	FileD,
	FileE,
	FileF,
	FileG,
	FileH
};

enum class Ranks : uint8_t
{
	Rank1 = 0,
	Rank2,
	Rank3,
	Rank4,
	Rank5,
	Rank6,
	Rank7,
	Rank8
};

const int32_t NEGATIVE_INFINITY_EVALUATION = INT32_MIN;
const int32_t POSITIVE_INFINITY_EVALUATION = INT32_MAX;