#pragma once

// Fixed width integer types
#include <cstdint>
#include <unordered_map>
#include <string>

typedef uint8_t BitPiece;
typedef int32_t Evaluation;

const BitPiece NO_PIECE = 0U;

enum class CastlingRights : uint8_t
{
    None = 0,
    WhiteKingside = 1,
    WhiteQueenside = 1 << 1,
    BlackKingside = 1 << 2,
    BlackQueenside = 1 << 3,
    White = WhiteKingside | WhiteQueenside,
    Black = BlackKingside | BlackQueenside,
    All = White | Black
};

// These are the same values as pieces colors so they can be casted to one another (not None)
enum class PlayerColor : uint8_t
{
    None = 0,
    White = 8,
    Black = 16
};

const std::string STARTING_POSITION_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const std::string KINGSIDE_CASTLING_SYMBOL = "O-O";
const std::string QUEENSIDE_CASTLING_SYMBOL = "O-O-O";

enum Square : uint8_t
{
    A1 = 0, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    None
};

enum DirectionOffsets : int8_t
{
    North = 8,
    East = -1,
    South = -8,
    West = 1,
    NorthEast = 7,
    SouthEast = -9,
    SouthWest = -7,
    NorthWest = 9
};

enum Files : uint64_t
{
    FileH = 0x8080808080808080,
    FileG = 0x4040404040404040,
    FileF = 0x2020202020202020,
    FileE = 0x1010101010101010,
    FileD = 0x808080808080808,
    FileC = 0x404040404040404,
    FileB = 0x202020202020202,
    FileA = 0x101010101010101
};

enum Ranks : uint64_t
{
    Rank1 = 0xFF,
    Rank2 = 0xFF00,
    Rank3 = 0xFF0000,
    Rank4 = 0xFF000000,
    Rank5 = 0xFF00000000,
    Rank6 = 0xFF0000000000,
    Rank7 = 0xFF000000000000,
    Rank8 = 0xFF00000000000000
};

constexpr Evaluation NEGATIVE_INFINITY_EVALUATION = INT32_MIN + 1;
constexpr Evaluation POSITIVE_INFINITY_EVALUATION = INT32_MAX - 1;

// These exist because i want true infinite evaluations to override these, which starting best evaluations
// -- are initialized with
const Evaluation OVER_NEGATIVE_INFINITY_EVALUATION = INT32_MIN;
const Evaluation OVER_POSITIVE_INFINITY_EVALUATION = INT32_MAX;

const uint16_t MAX_GAME_LENGTH = 700U;

// Bitboards
typedef uint64_t BitBoard;

// Piece types bit representations
enum class PieceType : BitPiece
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
enum class PieceColor : BitPiece
{
    White = 8,		//01 000
    Black = 16,		//10 000
    Both = 24		//11 000
};

inline void SetBit(BitBoard& bitBoard, unsigned int bit)
{
    bitBoard |= (1ULL << bit);
}

inline void SetBit(BitBoard& bitBoard, unsigned int y, unsigned int x)
{
    bitBoard |= (1ULL << (y * 8 + x));
}

inline bool CheckBit(BitBoard bitBoard, unsigned int bit)
{
    return bitBoard & (1ULL << bit);
}

inline bool CheckBit(BitBoard bitBoard, unsigned int y, unsigned int x)
{
    return bitBoard & (1ULL << (y * 8 + x));
}

inline void ClearBit(BitBoard& bitBoard, unsigned int bit)
{
    bitBoard &= ~(1ULL << bit);
}

inline constexpr int CountSetBits(BitBoard bitBoard)
{
    return __builtin_popcountll(bitBoard);
}

inline constexpr int IndexOfLSB(BitBoard bitBoard)
{
    return __builtin_ctzll(bitBoard);
}

inline constexpr void RemoveLSB(BitBoard& bitBoard)
{
    bitBoard &= bitBoard - 1;
}
