#pragma once

#include <list>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <array>
#include "Move.h"
#include "FullMove.h"
#include "Position.h"

class MovesGenerator
{

public:
    std::unique_ptr<std::list<Move>> GenerateLegalChessMoves(const Position& position, unsigned int& numberOfCaptureMoves);
    std::unique_ptr<std::vector<FullMove>> GenerateLegalMoves(const Position& position);
    std::unique_ptr<std::vector<FullMove>> GenerateLegalCaptures(const Position& position);

    const static int PRIORITIES_COUNT = 7;
    const static int PRIORITY_LAST = PRIORITIES_COUNT - 1;
    const std::unordered_map<Move::AdditionalInfo, uint8_t> MoveInfoToListIndex
    {
        {Move::AdditionalInfo::CapturedKing, 0},
        {Move::AdditionalInfo::CapturedQueen, 1},
        {Move::AdditionalInfo::CapturedKnight, 2},
        {Move::AdditionalInfo::CapturedRook, 3},
        {Move::AdditionalInfo::CapturedBishop, 4},
        {Move::AdditionalInfo::CapturedPawn, 5},
        {Move::AdditionalInfo::None, 6}
    };

    std::unique_ptr<std::array<std::list<Move>, PRIORITIES_COUNT>> GenerateLegalChessMovesSorted(const Position& position, unsigned int& numberOfCaptureMoves);

    constexpr static unsigned DIRECTIONS_COUNT = 8;
    constexpr static DirectionOffsets DIRECTIONS[DIRECTIONS_COUNT] =
    {
        DirectionOffsets::North,
        DirectionOffsets::South,
        DirectionOffsets::West,
        DirectionOffsets::East,
        DirectionOffsets::NorthWest,
        DirectionOffsets::NorthEast,
        DirectionOffsets::SouthWest,
        DirectionOffsets::SouthEast,
    };

    static constexpr auto SQUARES_TO_EDGE_COUNT
    {
        []() constexpr
        {
            std::array<std::array<unsigned, DIRECTIONS_COUNT>, Board::SQUARES_COUNT> result;

            for (int y = 0; y < Board::HEIGHT; ++y)
            {
                for (int x = 0; x < Board::WIDTH; ++x)
                {
                    unsigned currentIndex = y * Board::WIDTH + x;

                    // Squares to edge
                    result[currentIndex][0] = Board::HEIGHT - y - 1;
                    result[currentIndex][1] = y;
                    result[currentIndex][2] = Board::WIDTH - x - 1;
                    result[currentIndex][3] = x;
                    result[currentIndex][4] = std::min(result[currentIndex][0], result[currentIndex][2]);
                    result[currentIndex][5] = std::min(result[currentIndex][0], result[currentIndex][3]);
                    result[currentIndex][6] = std::min(result[currentIndex][1], result[currentIndex][2]);
                    result[currentIndex][7] = std::min(result[currentIndex][1], result[currentIndex][3]);
                }
            }

            return result;
        }()
    };


    static constexpr auto KNIGHT_ATTACKS
    {
        []() constexpr
        {
            BitBoard currentSquare;
            std::array<BitBoard, Board::SQUARES_COUNT> result;

            for (int y = 0; y < Board::HEIGHT; ++y)
            {
                for (int x = 0; x < Board::WIDTH; ++x)
                {
                    unsigned currentIndex = y * Board::WIDTH + x;
                    currentSquare = 1ULL << currentIndex;

                    // Knight attacks
                    result[currentIndex] =
                        ((currentSquare << 17) & ~((uint64_t)Ranks::Rank1 | (uint64_t)Ranks::Rank2 | (uint64_t)Files::FileA))  |   // North, north, west
                        ((currentSquare >> 15) & ~((uint64_t)Ranks::Rank7 | (uint64_t)Ranks::Rank8 | (uint64_t)Files::FileA))  |   // South, south, west
                        ((currentSquare << 15) & ~((uint64_t)Ranks::Rank1 | (uint64_t)Ranks::Rank2 | (uint64_t)Files::FileH))  |   // North, north, east
                        ((currentSquare >> 17) & ~((uint64_t)Ranks::Rank7 | (uint64_t)Ranks::Rank8 | (uint64_t)Files::FileH))  |   // South, south, east
                        ((currentSquare << 10) & ~((uint64_t)Files::FileA | (uint64_t)Files::FileB | (uint64_t)Ranks::Rank1))  |   // West, west, north
                        ((currentSquare >> 6) & ~((uint64_t)Files::FileA | (uint64_t)Files::FileB | (uint64_t)Ranks::Rank8))   |   // West, west, south
                        ((currentSquare << 6) & ~((uint64_t)Files::FileG | (uint64_t)Files::FileH | (uint64_t)Ranks::Rank1))   |   // East, east, north
                        ((currentSquare >> 10) & ~((uint64_t)Files::FileG | (uint64_t)Files::FileH | (uint64_t)Ranks::Rank8));     // East, east, south
                }
            }

            return result;
        }()
    };

    static constexpr auto KING_ATTACKS
    {
        []() constexpr
        {
            BitBoard currentSquare;
            std::array<BitBoard, Board::SQUARES_COUNT> result;

            for (int y = 0; y < Board::HEIGHT; ++y)
            {
                for (int x = 0; x < Board::WIDTH; ++x)
                {
                    unsigned currentIndex = y * Board::WIDTH + x;
                    currentSquare = 1ULL << currentIndex;

                    // King attacks
                    result[currentIndex] =
                        ((currentSquare << 8) & ~(uint64_t)Ranks::Rank1) |
                        ((currentSquare >> 8) & ~(uint64_t)Ranks::Rank8) |
                        ((currentSquare << 1) & ~(uint64_t)Files::FileA) |
                        ((currentSquare >> 1) & ~(uint64_t)Files::FileH) |
                        ((currentSquare << 7) & ~((uint64_t)Ranks::Rank1 | (uint64_t)Files::FileH)) |
                        ((currentSquare << 9) & ~((uint64_t)Ranks::Rank1 | (uint64_t)Files::FileA)) |
                        ((currentSquare >> 7) & ~((uint64_t)Ranks::Rank8 | (uint64_t)Files::FileA)) |
                        ((currentSquare >> 9) & ~((uint64_t)Ranks::Rank8 | (uint64_t)Files::FileH));
                }
            }

            return result;
        }()
    };

    const std::unordered_map<Board::Type, Move::AdditionalInfo> CAPTURED_PIECE_TYPE
    {
        { Board::Type::Pawns, Move::AdditionalInfo::CapturedPawn },
        { Board::Type::Bishops, Move::AdditionalInfo::CapturedBishop },
        { Board::Type::Knights, Move::AdditionalInfo::CapturedKnight },
        { Board::Type::Rooks, Move::AdditionalInfo::CapturedRook },
        { Board::Type::Queens, Move::AdditionalInfo::CapturedQueen },
        { Board::Type::Kings, Move::AdditionalInfo::CapturedKing }
    };

    Move::AdditionalInfo SquarePieceTypeToMoveInfo(BitBoard pieceMoveToOpponentsSquare, const Board& board, Board::Color opponentsColor);

private:

    void GeneratePawnsMoves(std::unique_ptr<std::list<Move>>& legalPiecesMoves, const Position& position, Board::Color movingPiecesColor, unsigned int& numberOfCaptureMoves);

    const static std::array<uint64_t, Board::SQUARES_COUNT> BISHOP_MAGIC_NUMBERS;
    const static std::array<uint64_t, Board::SQUARES_COUNT> ROOK_MAGIC_NUMBERS;
    const static std::array<uint8_t, Board::SQUARES_COUNT> BISHOP_RELEVANT_SQUARES;
    const static std::array<uint8_t, Board::SQUARES_COUNT> ROOK_RELEVANT_SQUARES;
    const static std::array<BitBoard, Board::SQUARES_COUNT> ROOK_ATTACKS_MASKS;
    const static std::array<BitBoard, Board::SQUARES_COUNT> BISHOP_ATTACKS_MASKS;
    static std::array<BitBoard, Board::SQUARES_COUNT> InitRookAttacksMasks();
    static std::array<BitBoard, Board::SQUARES_COUNT> InitBishopAttacksMasks();
    static BitBoard RookAttacksWithBlockers(uint8_t sourceY, uint8_t sourceX, BitBoard blockers);
    static BitBoard BishopAttacksWithBlockers(uint8_t sourceY, uint8_t sourceX, BitBoard blockers);
    static std::array<std::array<BitBoard, 4096>, Board::SQUARES_COUNT> InitInternalRookAttacks();
    static std::array<std::array<BitBoard, 512>, Board::SQUARES_COUNT> InitInternalBishopAttacks();
    const static std::array<std::array<BitBoard, 4096>, Board::SQUARES_COUNT> INTERNAL_ROOK_ATTACKS;
    const static std::array<std::array<BitBoard, 512>, Board::SQUARES_COUNT> INTERNAL_BISHOP_ATTACKS;

    const static std::array<std::array<BitBoard, 4096>, Board::SQUARES_COUNT> ROOK_ATTACKS;
    const static std::array<std::array<BitBoard, 512>, Board::SQUARES_COUNT> BISHOP_ATTACKS;

    static BitBoard GetRookAttacks(uint8_t square, BitBoard blockers);
    static BitBoard GetBishopAttacks(uint8_t square, BitBoard blockers);
};
