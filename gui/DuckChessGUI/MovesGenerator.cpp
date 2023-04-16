#include <algorithm>
#include "MovesGenerator.h"

#include <random>

extern void SquareToBoardIndices(const Square& square, int& y, int& x);
extern Square BoardIndicesToSquare(const unsigned& y, const unsigned& x);

const DirectionOffsets MovesGenerator::DIRECTIONS[DIRECTIONS_COUNT] =
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

std::unique_ptr<std::list<Move>> MovesGenerator::GenerateLegalChessMoves(const Position& position, unsigned int& numberOfCaptureMoves)
{
    std::unique_ptr<std::list<Move>> legalPiecesMoves = std::make_unique<std::list<Move>>();

    Board::Color movingPiecesColor = position.playerToMove == PlayerColor::White ? Board::Color::White : Board::Color::Black;
    Board::Color opponentsPiecesColor = (Board::Color)(1 - movingPiecesColor);

    // Knights moves
    BitBoard movingKnights = position.board.bitBoards[Board::Type::Knights][movingPiecesColor];

    while (movingKnights)
    {
        int currentKnightIndex = IndexOfLSB(movingKnights);
        BitBoard currentKnightAttacks = KNIGHT_ATTACKS[currentKnightIndex];
        BitBoard knightMovesToEmpties = currentKnightAttacks & position.board.empties;
        BitBoard knightMovesToOpponentsSquare = currentKnightAttacks & position.board.bitBoards[Board::Type::All][opponentsPiecesColor];

        while(knightMovesToEmpties)
        {
            legalPiecesMoves->emplace_back(Move((Square)currentKnightIndex, (Square)IndexOfLSB(knightMovesToEmpties)));
            RemoveLSB(knightMovesToEmpties);
        }

        while(knightMovesToOpponentsSquare)
        {
            int knightMoveToOpponentsSquareIndex = IndexOfLSB(knightMovesToOpponentsSquare);
            BitBoard knightMoveToOpponentsSquare = 1ULL << knightMoveToOpponentsSquareIndex;
            legalPiecesMoves->emplace_front(Move((Square)currentKnightIndex, (Square)knightMoveToOpponentsSquareIndex,
                                           SquarePieceTypeToMoveInfo(knightMoveToOpponentsSquare, position.board, opponentsPiecesColor)));
            ++numberOfCaptureMoves;
            RemoveLSB(knightMovesToOpponentsSquare);
        }

        RemoveLSB(movingKnights);
    }

    // Bishops moves
    GenerateLongRangePieceMoves(legalPiecesMoves, Board::Type::Bishops, movingPiecesColor, position.board, numberOfCaptureMoves);

    // Rooks moves
    GenerateLongRangePieceMoves(legalPiecesMoves, Board::Type::Rooks, movingPiecesColor, position.board, numberOfCaptureMoves);

    // Queen moves
    GenerateLongRangePieceMoves(legalPiecesMoves, Board::Type::Queens, movingPiecesColor, position.board, numberOfCaptureMoves);

    // King moves
    //// One square moves
    int kingIndex = IndexOfLSB(position.board.bitBoards[Board::Type::Kings][movingPiecesColor]);
    BitBoard movingKingAttacks = KING_ATTACKS[kingIndex];
    BitBoard kingMovesToEmpties = movingKingAttacks & position.board.empties;
    BitBoard kingMovesToOpponentsSquare = movingKingAttacks & position.board.bitBoards[Board::Type::All][opponentsPiecesColor];

    while(kingMovesToEmpties)
    {
        legalPiecesMoves->emplace_back(Move((Square)kingIndex, (Square)IndexOfLSB(kingMovesToEmpties)));
        RemoveLSB(kingMovesToEmpties);
    }

    while(kingMovesToOpponentsSquare)
    {
        int kingMoveToOpponentsSquareIndex = IndexOfLSB(kingMovesToOpponentsSquare);
        BitBoard kingMoveToOpponentsSquare = 1ULL << kingMoveToOpponentsSquareIndex;
        legalPiecesMoves->emplace_front(Move((Square)kingIndex, (Square)kingMoveToOpponentsSquareIndex,
                                       SquarePieceTypeToMoveInfo(kingMoveToOpponentsSquare, position.board, opponentsPiecesColor)));
        ++numberOfCaptureMoves;
        RemoveLSB(kingMovesToOpponentsSquare);
    }

    //// Castling
    if (position.playerToMove == PlayerColor::White)
    {
        if ((CastlingRights)((uint8_t)position.castlingRights & (uint8_t)CastlingRights::WhiteKingside) != CastlingRights::None &&
             CheckBit(position.board.empties, Square::F1) && CheckBit(position.board.empties, Square::G1))
        {
            legalPiecesMoves->emplace_front(Move((Square)kingIndex, Square::G1, Move::AdditionalInfo::WhiteKingsideCastle));
        }
        if ((CastlingRights)((uint8_t)position.castlingRights & (uint8_t)CastlingRights::WhiteQueenside) != CastlingRights::None &&
            CheckBit(position.board.empties, Square::B1) && CheckBit(position.board.empties, Square::C1) && CheckBit(position.board.empties, Square::D1))
        {
            legalPiecesMoves->emplace_front(Move((Square)kingIndex, Square::C1, Move::AdditionalInfo::WhiteQueensideCastle));
        }
    }
    else
    {
        if ((CastlingRights)((uint8_t)position.castlingRights & (uint8_t)CastlingRights::BlackKingside) != CastlingRights::None &&
            CheckBit(position.board.empties, Square::F8) && CheckBit(position.board.empties, Square::G8))
        {
            legalPiecesMoves->emplace_front(Move((Square)kingIndex, Square::G8, Move::AdditionalInfo::BlackKingsideCastle));
        }
        if ((CastlingRights)((uint8_t)position.castlingRights & (uint8_t)CastlingRights::BlackQueenside) != CastlingRights::None &&
            CheckBit(position.board.empties, Square::B8) && CheckBit(position.board.empties, Square::C8) && CheckBit(position.board.empties, Square::D8))
        {
            legalPiecesMoves->emplace_front(Move((Square)kingIndex, Square::C8, Move::AdditionalInfo::BlackQueensideCastle));
        }
    }

    // Pawn moves
    GeneratePawnsMoves(legalPiecesMoves, position, movingPiecesColor, numberOfCaptureMoves);

    return legalPiecesMoves;
}

std::unique_ptr<std::vector<FullMove>> MovesGenerator::GenerateLegalMoves(const Position& position)
{
    unsigned int numberOfCaptureMoves = 0;
    std::unique_ptr<std::list<Move>> legalPiecesMoves = GenerateLegalChessMoves(position, numberOfCaptureMoves);

    //std::sort(legalPiecesMoves.begin(), legalPiecesMoves.end(),
    //	[](const Move& move1, const Move& move2)
    //	{
    //		return move1.additionalInfo < move2.additionalInfo;
    //	});

    auto legalMoves = std::make_unique<std::vector<FullMove>>();
    (*legalMoves).reserve(legalPiecesMoves->size() * CountSetBits(position.board.empties) + numberOfCaptureMoves);

    //for (const Move& legalPieceMove : legalPiecesMoves)
    //{
    //	legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, legalPieceMove.sourceSquare));
    //}

    Square duckSquare;
    if (position.board.duck)
    {
        duckSquare = (Square)IndexOfLSB(position.board.duck);
    }
    else
    {
        duckSquare = Square::None;
    }

    BitBoard emptiesCopy;

    for (const Move& legalPieceMove : *legalPiecesMoves)
    {
        emptiesCopy = position.board.empties;

        switch (legalPieceMove.additionalInfo)
        {
        case Move::AdditionalInfo::BlackKingsideCastle:
        case Move::AdditionalInfo::WhiteKingsideCastle:
        case Move::AdditionalInfo::BlackQueensideCastle:
        case Move::AdditionalInfo::WhiteQueensideCastle:
        {
            const auto& [rookSourceSquare, rookTargetSquare] = Move::ROOK_CASTLING_SQUARES.at(legalPieceMove.additionalInfo);

            while (emptiesCopy)
            {
                int currentEmptyIndex = IndexOfLSB(emptiesCopy);

                if (currentEmptyIndex != legalPieceMove.targetSquare && currentEmptyIndex != rookTargetSquare)
                {
                    legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, (Square)currentEmptyIndex));
                }

                RemoveLSB(emptiesCopy);
            }

            legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, legalPieceMove.sourceSquare));
            legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, rookSourceSquare));

            break;
        }
        case Move::AdditionalInfo::EnPassant:
        {
            while (emptiesCopy)
            {
                int currentEmptyIndex = IndexOfLSB(emptiesCopy);

                if (currentEmptyIndex != legalPieceMove.targetSquare)
                {
                    legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, (Square)currentEmptyIndex));
                }

                RemoveLSB(emptiesCopy);
            }

            legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, legalPieceMove.sourceSquare));

            if (position.playerToMove == PlayerColor::White)
            {
                legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, (Square)((uint8_t)legalPieceMove.targetSquare + (int8_t)DirectionOffsets::South)));
            }
            else
            {
                legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, (Square)((uint8_t)legalPieceMove.targetSquare + (int8_t)DirectionOffsets::North)));
            }

            break;
        }
        default:
        {
            while (emptiesCopy)
            {
                int currentEmptyIndex = IndexOfLSB(emptiesCopy);

                if (currentEmptyIndex != legalPieceMove.targetSquare)
                {
                    legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, (Square)currentEmptyIndex));
                }

                RemoveLSB(emptiesCopy);
            }

            legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, legalPieceMove.sourceSquare));

            break;
        }
        }
    }

    return legalMoves;
}

void MovesGenerator::GenerateLongRangePieceMoves(std::unique_ptr<std::list<Move>>& legalMoves, Board::Type pieceType, Board::Color movingPiecesColor, const Board& board, unsigned int& numberOfCaptureMoves)
{
    Board::Color opponentsPiecesColor = (Board::Color)(1 - movingPiecesColor);
    uint8_t squareInDirection;
    unsigned lowBound = 0;
    unsigned highBound = DIRECTIONS_COUNT;

    if (pieceType == Board::Type::Bishops)
    {
        // Starting with d = 4 (only diagonal directions)
        lowBound = 4;
    }
    else if (pieceType == Board::Type::Rooks)
    {
        // Ending with d = 4 (only horizontal and vertical directions)
        highBound = 4;
    }

    BitBoard movingPieces = board.bitBoards[pieceType][movingPiecesColor];

    while (movingPieces)
    {
        int currentPieceIndex = IndexOfLSB(movingPieces);

        for (unsigned d = lowBound; d < highBound; ++d)
        {
            squareInDirection = currentPieceIndex;

            for (unsigned s = 0; s < SQUARES_TO_EDGE_COUNT[currentPieceIndex][d]; ++s)
            {
                squareInDirection += (int8_t)DIRECTIONS[d];

                if (CheckBit(board.empties, squareInDirection))
                {
                    legalMoves->emplace_back(Move((Square)currentPieceIndex, (Square)squareInDirection));
                }
                else if (CheckBit(board.bitBoards[Board::Type::All][opponentsPiecesColor], squareInDirection))
                {
                    legalMoves->emplace_front(Move((Square)currentPieceIndex, (Square)squareInDirection, SquarePieceTypeToMoveInfo(1ULL << squareInDirection, board, opponentsPiecesColor)));
                    ++numberOfCaptureMoves;
                    break;
                }
                else
                {
                    break;
                }
            }
        }

        RemoveLSB(movingPieces);
    }
}

Move::AdditionalInfo MovesGenerator::SquarePieceTypeToMoveInfo(BitBoard pieceMoveToOpponentsSquare, const Board& board, Board::Color opponentsColor)
{
    for (int i = 0; i < 6; ++i)
    {
        if ((pieceMoveToOpponentsSquare & board.bitBoards[i][opponentsColor]) != 0ULL)
        {
            return CAPTURED_PIECE_TYPE.at((Board::Type)i);
        }
    }
}

void MovesGenerator::GeneratePawnsMoves(std::unique_ptr<std::list<Move>>& legalPiecesMoves, const Position& position, Board::Color movingPiecesColor, unsigned int& numberOfCaptureMoves)
{
    Board::Color opponentsPiecesColor = (Board::Color)(1 - movingPiecesColor);
    BitBoard movingPawns = position.board.bitBoards[Board::Type::Pawns][movingPiecesColor];
    //BitBoard currentPawn;
    int8_t forwardDirection;
    uint8_t forwardSquare;
    uint8_t doubleForwardSquare;
    uint8_t forwardWestSquare;
    uint8_t forwardEastSquare;
    Ranks doubleForwardRank;
    Ranks promotionCandidateRank;

    if (movingPiecesColor == Board::Color::White)
    {
        forwardDirection = DirectionOffsets::North;
        doubleForwardRank = Ranks::Rank2;
        promotionCandidateRank = Ranks::Rank7;
    }
    else
    {
        forwardDirection = DirectionOffsets::South;
        doubleForwardRank = Ranks::Rank7;
        promotionCandidateRank = Ranks::Rank2;
    }

    while (movingPawns)
    {
        int currentPawnIndex = IndexOfLSB(movingPawns);

        forwardSquare = currentPawnIndex + forwardDirection;

        // Moving one square forward
        if (CheckBit(position.board.empties, forwardSquare))
        {
            // Promotions
            if (CheckBit(promotionCandidateRank, currentPawnIndex))
            {
                legalPiecesMoves->emplace_front(Move((Square)currentPawnIndex, (Square)forwardSquare, Move::AdditionalInfo::PromotionToKnight));
                legalPiecesMoves->emplace_front(Move((Square)currentPawnIndex, (Square)forwardSquare, Move::AdditionalInfo::PromotionToBishop));
                legalPiecesMoves->emplace_front(Move((Square)currentPawnIndex, (Square)forwardSquare, Move::AdditionalInfo::PromotionToQueen));
                legalPiecesMoves->emplace_front(Move((Square)currentPawnIndex, (Square)forwardSquare, Move::AdditionalInfo::PromotionToRook));
            }
            else
            {
                legalPiecesMoves->emplace_back(Move((Square)currentPawnIndex, (Square)forwardSquare));

                // Moving two squares forward
                doubleForwardSquare = forwardSquare + forwardDirection;

                if (CheckBit(doubleForwardRank, currentPawnIndex) && CheckBit(position.board.empties, doubleForwardSquare))
                {
                    legalPiecesMoves->emplace_back(Move((Square)currentPawnIndex, (Square)doubleForwardSquare, Move::AdditionalInfo::PawnDoubleForward));
                }
            }
        }

        // Takes and en passant

        if (!CheckBit(Files::FileA, currentPawnIndex))
        {
            forwardWestSquare = forwardSquare + DirectionOffsets::East;

            if (CheckBit(position.board.bitBoards[Board::Type::All][opponentsPiecesColor], forwardWestSquare))
            {
                Move::AdditionalInfo capturedOnForwardWest = SquarePieceTypeToMoveInfo(1ULL << forwardWestSquare, position.board, opponentsPiecesColor);

                if (CheckBit(promotionCandidateRank, currentPawnIndex))
                {
                    legalPiecesMoves->emplace_front(Move((Square)currentPawnIndex, (Square)forwardWestSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToKnight | (uint16_t)capturedOnForwardWest)));
                    legalPiecesMoves->emplace_front(Move((Square)currentPawnIndex, (Square)forwardWestSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToBishop | (uint16_t)capturedOnForwardWest)));
                    legalPiecesMoves->emplace_front(Move((Square)currentPawnIndex, (Square)forwardWestSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToRook | (uint16_t)capturedOnForwardWest)));
                    legalPiecesMoves->emplace_front(Move((Square)currentPawnIndex, (Square)forwardWestSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToQueen | (uint16_t)capturedOnForwardWest)));
                    numberOfCaptureMoves += 4;
                }
                else
                {
                    legalPiecesMoves->emplace_front(Move((Square)currentPawnIndex, (Square)forwardWestSquare, capturedOnForwardWest));
                    ++numberOfCaptureMoves;
                }
            }
            else if (forwardWestSquare == position.enPassantTarget && forwardWestSquare != IndexOfLSB(position.board.duck))
            {
                legalPiecesMoves->emplace_front(Move((Square)currentPawnIndex, (Square)forwardWestSquare, Move::AdditionalInfo::EnPassant));
                ++numberOfCaptureMoves;
            }
        }
        if (!CheckBit(Files::FileH, currentPawnIndex))
        {
            forwardEastSquare = forwardSquare + DirectionOffsets::West;

            if (CheckBit(position.board.bitBoards[Board::Type::All][opponentsPiecesColor], forwardEastSquare))
            {
                Move::AdditionalInfo capturedOnForwardEast = SquarePieceTypeToMoveInfo(1ULL << forwardEastSquare, position.board, opponentsPiecesColor);

                if (CheckBit(promotionCandidateRank, currentPawnIndex))
                {
                    legalPiecesMoves->emplace_front(Move((Square)currentPawnIndex, (Square)forwardEastSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToKnight | (uint16_t)capturedOnForwardEast)));
                    legalPiecesMoves->emplace_front(Move((Square)currentPawnIndex, (Square)forwardEastSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToBishop | (uint16_t)capturedOnForwardEast)));
                    legalPiecesMoves->emplace_front(Move((Square)currentPawnIndex, (Square)forwardEastSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToRook | (uint16_t)capturedOnForwardEast)));
                    legalPiecesMoves->emplace_front(Move((Square)currentPawnIndex, (Square)forwardEastSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToQueen | (uint16_t)capturedOnForwardEast)));
                    numberOfCaptureMoves += 4;
                }
                else
                {
                    legalPiecesMoves->emplace_front(Move((Square)currentPawnIndex, (Square)forwardEastSquare, capturedOnForwardEast));
                    ++numberOfCaptureMoves;
                }
            }
            else if (forwardEastSquare == position.enPassantTarget && forwardEastSquare != IndexOfLSB(position.board.duck))
            {
                legalPiecesMoves->emplace_front(Move((Square)currentPawnIndex, (Square)forwardEastSquare, Move::AdditionalInfo::EnPassant));
                ++numberOfCaptureMoves;
            }
        }

        RemoveLSB(movingPawns);
    }
}
