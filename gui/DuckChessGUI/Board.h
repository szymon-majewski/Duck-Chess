#pragma once

//#include "Piece.h"
#include "Definitions.h"

class Board
{
public:

    // Board dimensions
    static const int WIDTH = 8;
    static const int HEIGHT = 8;
    static const int SQUARES_COUNT = WIDTH * HEIGHT;

    BitBoard bitBoards[7][2] =
    {
        {0ULL, 0ULL},
        {0ULL, 0ULL},
        {0ULL, 0ULL},
        {0ULL, 0ULL},
        {0ULL, 0ULL},
        {0ULL, 0ULL},
        {0ULL, 0ULL}
    };
    BitBoard duck = 0ULL;
    BitBoard empties = 0ULL;

    inline void UpdateEmpties()
    {
        empties = ~(bitBoards[All][White] | bitBoards[All][Black] | duck);
    }

    enum Color : uint8_t
    {
        White = 0,
        Black = 1,
        Both = 2 // Only for map
    };

    enum Type : uint8_t
    {
        Pawns = 0,
        Bishops,
        Knights,
        Rooks,
        Queens,
        Kings,
        All,
        Duck // Only for map
    };

    struct HashPair
    {
        template <class T1, class T2>
        size_t operator()(const std::pair<T1, T2>& p) const
        {
            auto hash1 = std::hash<T1>{}(p.first);
            auto hash2 = std::hash<T2>{}(p.second);

            if (hash1 != hash2)
            {
                return hash1 ^ hash2;
            }

            // If hash1 == hash2, their XOR is zero.
            return hash1;
        }
    };

    std::unordered_map<char, BitBoard&> PIECES_SYMBOLS_BITBOARDS_MAP =
    {
        { 'p', bitBoards[Pawns][Color::Black] },
        { 'P', bitBoards[Pawns][Color::White] },
        { 'n', bitBoards[Knights][Color::Black]},
        { 'N', bitBoards[Knights][Color::White] },
        { 'b', bitBoards[Bishops][Color::Black] },
        { 'B', bitBoards[Bishops][Color::White] },
        { 'r', bitBoards[Rooks][Color::Black] },
        { 'R', bitBoards[Rooks][Color::White] },
        { 'q', bitBoards[Queens][Color::Black] },
        { 'Q', bitBoards[Queens][Color::White] },
        { 'k', bitBoards[Kings][Color::Black] },
        { 'K', bitBoards[Kings][Color::White] },
        { '@', duck }
    };

    static std::unordered_map<BitPiece, char> PIECES_SYMBOLS_BITPIECES_MAP;
    static std::unordered_map<std::pair<Type, Color>, BitPiece, HashPair> BOARD_PIECES_TO_BITPIECES_MAP;

    Board::Type FindBoardType(BitBoard pieceBitBoard, Board::Color color) const;
};
