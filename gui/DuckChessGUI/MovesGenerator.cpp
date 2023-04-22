#include <algorithm>
#include "MovesGenerator.h"

#include <random>

extern void SquareToBoardIndices(const Square& square, int& y, int& x);
extern Square BoardIndicesToSquare(const unsigned& y, const unsigned& x);

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

    // Long range pieces moves
    BitBoard blockers = position.board.bitBoards[Board::Type::All][movingPiecesColor] |
                        position.board.bitBoards[Board::Type::All][opponentsPiecesColor] |
                        position.board.duck;
    // Bishops moves
    //GenerateLongRangePieceMoves(legalPiecesMoves, Board::Type::Bishops, movingPiecesColor, position.board, numberOfCaptureMoves);

    BitBoard movingPieces = position.board.bitBoards[Board::Type::Bishops][movingPiecesColor];

    while (movingPieces)
    {
        int currentPieceIndex = IndexOfLSB(movingPieces);
        BitBoard bishopMoves = GetBishopAttacks(currentPieceIndex, blockers);
        bishopMoves &= ~(position.board.bitBoards[Board::Type::All][movingPiecesColor] | position.board.duck);
        BitBoard movesToEmpties = bishopMoves & position.board.empties;
        BitBoard movesTakes = bishopMoves & position.board.bitBoards[Board::Type::All][opponentsPiecesColor];

        while(movesToEmpties)
        {
            legalPiecesMoves->emplace_back(Move((Square)currentPieceIndex, (Square)IndexOfLSB(movesToEmpties)));
            RemoveLSB(movesToEmpties);
        }

        while(movesTakes)
        {
            int moveTakeIndex = IndexOfLSB(movesTakes);
            BitBoard moveTake = 1ULL << moveTakeIndex;
            legalPiecesMoves->emplace_front(Move((Square)currentPieceIndex, (Square)moveTakeIndex,
                                           SquarePieceTypeToMoveInfo(moveTake, position.board, opponentsPiecesColor)));
            ++numberOfCaptureMoves;
            RemoveLSB(movesTakes);
        }

        RemoveLSB(movingPieces);
    }


    // Rooks moves
    //GenerateLongRangePieceMoves(legalPiecesMoves, Board::Type::Rooks, movingPiecesColor, position.board, numberOfCaptureMoves);

    movingPieces = position.board.bitBoards[Board::Type::Rooks][movingPiecesColor];

    while (movingPieces)
    {
        int currentPieceIndex = IndexOfLSB(movingPieces);
        BitBoard rookMoves = GetRookAttacks(currentPieceIndex, blockers);
        rookMoves &= ~(position.board.bitBoards[Board::Type::All][movingPiecesColor] | position.board.duck);
        BitBoard movesToEmpties = rookMoves & position.board.empties;
        BitBoard movesTakes = rookMoves & position.board.bitBoards[Board::Type::All][opponentsPiecesColor];

        while(movesToEmpties)
        {
            legalPiecesMoves->emplace_back(Move((Square)currentPieceIndex, (Square)IndexOfLSB(movesToEmpties)));
            RemoveLSB(movesToEmpties);
        }

        while(movesTakes)
        {
            int moveTakeIndex = IndexOfLSB(movesTakes);
            BitBoard moveTake = 1ULL << moveTakeIndex;
            legalPiecesMoves->emplace_front(Move((Square)currentPieceIndex, (Square)moveTakeIndex,
                                           SquarePieceTypeToMoveInfo(moveTake, position.board, opponentsPiecesColor)));
            ++numberOfCaptureMoves;
            RemoveLSB(movesTakes);
        }

        RemoveLSB(movingPieces);
    }

    // Queen moves
    //GenerateLongRangePieceMoves(legalPiecesMoves, Board::Type::Queens, movingPiecesColor, position.board, numberOfCaptureMoves);

    movingPieces = position.board.bitBoards[Board::Type::Queens][movingPiecesColor];

    while (movingPieces)
    {
        int currentPieceIndex = IndexOfLSB(movingPieces);
        BitBoard queenMoves = GetRookAttacks(currentPieceIndex, blockers) | GetBishopAttacks(currentPieceIndex, blockers);
        queenMoves &= ~(position.board.bitBoards[Board::Type::All][movingPiecesColor] | position.board.duck);
        BitBoard movesToEmpties = queenMoves & position.board.empties;
        BitBoard movesTakes = queenMoves & position.board.bitBoards[Board::Type::All][opponentsPiecesColor];

        while(movesToEmpties)
        {
            legalPiecesMoves->emplace_back(Move((Square)currentPieceIndex, (Square)IndexOfLSB(movesToEmpties)));
            RemoveLSB(movesToEmpties);
        }

        while(movesTakes)
        {
            int moveTakeIndex = IndexOfLSB(movesTakes);
            BitBoard moveTake = 1ULL << moveTakeIndex;
            legalPiecesMoves->emplace_front(Move((Square)currentPieceIndex, (Square)moveTakeIndex,
                                           SquarePieceTypeToMoveInfo(moveTake, position.board, opponentsPiecesColor)));
            ++numberOfCaptureMoves;
            RemoveLSB(movesTakes);
        }

        RemoveLSB(movingPieces);
    }

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
    auto legalPiecesMoves = GenerateLegalChessMovesSorted(position, numberOfCaptureMoves);

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

    // Moves with captures of everything but pawns
    for (int pieceID = 0; pieceID < 5; ++pieceID)
    {
        for (const Move& legalPieceMove : (*legalPiecesMoves)[pieceID])
        {
            emptiesCopy = position.board.empties;

            // Removing target square from potential duck squares
            ResetBit(emptiesCopy, legalPieceMove.targetSquare);

            while (emptiesCopy)
            {
                int currentEmptyIndex = IndexOfLSB(emptiesCopy);

                legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, (Square)currentEmptyIndex));
                RemoveLSB(emptiesCopy);
            }

            legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, legalPieceMove.sourceSquare));
        }
    }

    // Moves with captures of pawns
    for (const Move& legalPieceMove : (*legalPiecesMoves)[5])
    {
        emptiesCopy = position.board.empties;

        // Removing target square from potential duck squares
        ResetBit(emptiesCopy, legalPieceMove.targetSquare);

        if (legalPieceMove.additionalInfo == Move::AdditionalInfo::EnPassant)
        {
            while (emptiesCopy)
            {
                int currentEmptyIndex = IndexOfLSB(emptiesCopy);

                legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, (Square)currentEmptyIndex));
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
        }
        else
        {
            while (emptiesCopy)
            {
                int currentEmptyIndex = IndexOfLSB(emptiesCopy);

                legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, (Square)currentEmptyIndex));
                RemoveLSB(emptiesCopy);
            }

            legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, legalPieceMove.sourceSquare));
        }
    }

    // Rest of the moves
    for (const Move& legalPieceMove : (*legalPiecesMoves)[6])
    {
        emptiesCopy = position.board.empties;

        // Removing target square from potential duck squares
        ResetBit(emptiesCopy, legalPieceMove.targetSquare);

        if ((uint16_t)legalPieceMove.additionalInfo & Move::castlingChecker)
        {
            const auto& [rookSourceSquare, rookTargetSquare] = Move::ROOK_CASTLING_SQUARES.at(legalPieceMove.additionalInfo);

            // Removing rook target square from potential duck squares
            ResetBit(emptiesCopy, rookTargetSquare);

            while (emptiesCopy)
            {
                int currentEmptyIndex = IndexOfLSB(emptiesCopy);

                legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, (Square)currentEmptyIndex));
                RemoveLSB(emptiesCopy);
            }

            legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, legalPieceMove.sourceSquare));
            legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, rookSourceSquare));
        }
        else
        {
            while (emptiesCopy)
            {
                int currentEmptyIndex = IndexOfLSB(emptiesCopy);

                legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, (Square)currentEmptyIndex));
                RemoveLSB(emptiesCopy);
            }

            legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, legalPieceMove.sourceSquare));
        }
    }

    return legalMoves;
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

const std::array<uint64_t, Board::SQUARES_COUNT> MovesGenerator::BISHOP_MAGIC_NUMBERS =
{
    0x89a1121896040240ULL,
    0x2004844802002010ULL,
    0x2068080051921000ULL,
    0x62880a0220200808ULL,
    0x4042004000000ULL,
    0x100822020200011ULL,
    0xc00444222012000aULL,
    0x28808801216001ULL,
    0x400492088408100ULL,
    0x201c401040c0084ULL,
    0x840800910a0010ULL,
    0x82080240060ULL,
    0x2000840504006000ULL,
    0x30010c4108405004ULL,
    0x1008005410080802ULL,
    0x8144042209100900ULL,
    0x208081020014400ULL,
    0x4800201208ca00ULL,
    0xf18140408012008ULL,
    0x1004002802102001ULL,
    0x841000820080811ULL,
    0x40200200a42008ULL,
    0x800054042000ULL,
    0x88010400410c9000ULL,
    0x520040470104290ULL,
    0x1004040051500081ULL,
    0x2002081833080021ULL,
    0x400c00c010142ULL,
    0x941408200c002000ULL,
    0x658810000806011ULL,
    0x188071040440a00ULL,
    0x4800404002011c00ULL,
    0x104442040404200ULL,
    0x511080202091021ULL,
    0x4022401120400ULL,
    0x80c0040400080120ULL,
    0x8040010040820802ULL,
    0x480810700020090ULL,
    0x102008e00040242ULL,
    0x809005202050100ULL,
    0x8002024220104080ULL,
    0x431008804142000ULL,
    0x19001802081400ULL,
    0x200014208040080ULL,
    0x3308082008200100ULL,
    0x41010500040c020ULL,
    0x4012020c04210308ULL,
    0x208220a202004080ULL,
    0x111040120082000ULL,
    0x6803040141280a00ULL,
    0x2101004202410000ULL,
    0x8200000041108022ULL,
    0x21082088000ULL,
    0x2410204010040ULL,
    0x40100400809000ULL,
    0x822088220820214ULL,
    0x40808090012004ULL,
    0x910224040218c9ULL,
    0x402814422015008ULL,
    0x90014004842410ULL,
    0x1000042304105ULL,
    0x10008830412a00ULL,
    0x2520081090008908ULL,
    0x40102000a0a60140ULL
};

const std::array<uint64_t, Board::SQUARES_COUNT> MovesGenerator::ROOK_MAGIC_NUMBERS =
{
    0xa8002c000108020ULL,
    0x6c00049b0002001ULL,
    0x100200010090040ULL,
    0x2480041000800801ULL,
    0x280028004000800ULL,
    0x900410008040022ULL,
    0x280020001001080ULL,
    0x2880002041000080ULL,
    0xa000800080400034ULL,
    0x4808020004000ULL,
    0x2290802004801000ULL,
    0x411000d00100020ULL,
    0x402800800040080ULL,
    0xb000401004208ULL,
    0x2409000100040200ULL,
    0x1002100004082ULL,
    0x22878001e24000ULL,
    0x1090810021004010ULL,
    0x801030040200012ULL,
    0x500808008001000ULL,
    0xa08018014000880ULL,
    0x8000808004000200ULL,
    0x201008080010200ULL,
    0x801020000441091ULL,
    0x800080204005ULL,
    0x1040200040100048ULL,
    0x120200402082ULL,
    0xd14880480100080ULL,
    0x12040280080080ULL,
    0x100040080020080ULL,
    0x9020010080800200ULL,
    0x813241200148449ULL,
    0x491604001800080ULL,
    0x100401000402001ULL,
    0x4820010021001040ULL,
    0x400402202000812ULL,
    0x209009005000802ULL,
    0x810800601800400ULL,
    0x4301083214000150ULL,
    0x204026458e001401ULL,
    0x40204000808000ULL,
    0x8001008040010020ULL,
    0x8410820820420010ULL,
    0x1003001000090020ULL,
    0x804040008008080ULL,
    0x12000810020004ULL,
    0x1000100200040208ULL,
    0x430000a044020001ULL,
    0x280009023410300ULL,
    0xe0100040002240ULL,
    0x200100401700ULL,
    0x2244100408008080ULL,
    0x8000400801980ULL,
    0x2000810040200ULL,
    0x8010100228810400ULL,
    0x2000009044210200ULL,
    0x4080008040102101ULL,
    0x40002080411d01ULL,
    0x2005524060000901ULL,
    0x502001008400422ULL,
    0x489a000810200402ULL,
    0x1004400080a13ULL,
    0x4000011008020084ULL,
    0x26002114058042ULL
};

const std::array<uint8_t, Board::SQUARES_COUNT> MovesGenerator::BISHOP_RELEVANT_SQUARES =
{
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};

const std::array<uint8_t, Board::SQUARES_COUNT> MovesGenerator::ROOK_RELEVANT_SQUARES =
{
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};

std::array<BitBoard, Board::SQUARES_COUNT> MovesGenerator::InitRookAttacksMasks()
{
    std::array<BitBoard, Board::SQUARES_COUNT> result;

    for (int y = 0; y < Board::HEIGHT; ++y)
    {
        for (int x = 0; x < Board::WIDTH; ++x)
        {
            unsigned currentIndex = y * Board::WIDTH + x;

            result[currentIndex] = 0ULL;

            // West
            for (int i = x - 1; i > 0; --i)
            {
                result[currentIndex] |= (1ULL << (y * Board::WIDTH + i));
            }
            // East
            for (int i = x + 1; i < 7; ++i)
            {
                result[currentIndex] |= (1ULL << (y * Board::WIDTH + i));
            }
            // North
            for (int i = y + 1; i < 7; ++i)
            {
                result[currentIndex] |= (1ULL << (i * Board::WIDTH + x));
            }
            // South
            for (int i = y - 1; i > 0; --i)
            {
                result[currentIndex] |= (1ULL << (i * Board::WIDTH + x));
            }
        }
    }

    return result;
}

const std::array<BitBoard, Board::SQUARES_COUNT> MovesGenerator::ROOK_ATTACKS_MASKS = InitRookAttacksMasks();

std::array<BitBoard, Board::SQUARES_COUNT> MovesGenerator::InitBishopAttacksMasks()
{
    std::array<BitBoard, Board::SQUARES_COUNT> result;

    for (int y = 0; y < Board::HEIGHT; ++y)
    {
        for (int x = 0; x < Board::WIDTH; ++x)
        {
            unsigned currentIndex = y * Board::WIDTH + x;

            result[currentIndex] = 0ULL;

            // North-west
            for (int file = x + 1, rank = y + 1; file < 7 && rank < 7; ++file, ++rank)
            {
                result[currentIndex] |= (1ULL << (rank * Board::WIDTH + file));
            }
            // North-east
            for (int file = x - 1, rank = y + 1; file > 0 && rank < 7; --file, ++rank)
            {
                result[currentIndex] |= (1ULL << (rank * Board::WIDTH + file));
            }
            // South-west
            for (int file = x + 1, rank = y - 1; file < 7 && rank > 0; ++file, --rank)
            {
                result[currentIndex] |= (1ULL << (rank * Board::WIDTH + file));
            }
            // South-east
            for (int file = x - 1, rank = y - 1; file > 0 && rank > 0; --file, --rank)
            {
                result[currentIndex] |= (1ULL << (rank * Board::WIDTH + file));
            }
        }
    }

    return result;
}

const std::array<BitBoard, Board::SQUARES_COUNT> MovesGenerator::BISHOP_ATTACKS_MASKS = InitBishopAttacksMasks();

BitBoard MovesGenerator::RookAttacksWithBlockers(uint8_t sourceY, uint8_t sourceX, BitBoard blockers)
{
    BitBoard attacks = 0ULL;
    BitBoard currentSquare;

    // West
    for (int i = sourceX - 1; i >= 0; --i)
    {
        currentSquare = sourceY * Board::WIDTH + i;
        attacks |= (1ULL << currentSquare);

        if ((1ULL << currentSquare) & blockers) { break; }
    }
    // East
    for (int i = sourceX + 1; i <= 7; ++i)
    {
        currentSquare = sourceY * Board::WIDTH + i;
        attacks |= (1ULL << currentSquare);

        if ((1ULL << currentSquare) & blockers) { break; }
    }
    // North
    for (int i = sourceY + 1; i <= 7; ++i)
    {
        currentSquare = i * Board::WIDTH + sourceX;
        attacks |= (1ULL << currentSquare);

        if ((1ULL << currentSquare) & blockers) { break; }
    }
    // South
    for (int i = sourceY - 1; i >= 0; --i)
    {
        currentSquare = i * Board::WIDTH + sourceX;
        attacks |= (1ULL << currentSquare);

        if ((1ULL << currentSquare) & blockers) { break; }
    }

    return attacks;
}

BitBoard MovesGenerator::BishopAttacksWithBlockers(uint8_t sourceY, uint8_t sourceX, BitBoard blockers)
{
    BitBoard attacks = 0ULL;
    BitBoard currentSquare;

    // North-west
    for (int file = sourceX + 1, rank = sourceY + 1; file <= 7 && rank <= 7; ++file, ++rank)
    {
        currentSquare = rank * Board::WIDTH + file;
        attacks |= (1ULL << currentSquare);

        if ((1ULL << currentSquare) & blockers) { break; }
    }
    // North-east
    for (int file = sourceX - 1, rank = sourceY + 1; file >= 0 && rank <= 7; --file, ++rank)
    {
        currentSquare = rank * Board::WIDTH + file;
        attacks |= (1ULL << currentSquare);

        if ((1ULL << currentSquare) & blockers) { break; }
    }
    // South-west
    for (int file = sourceX + 1, rank = sourceY - 1; file <= 7 && rank >= 0; ++file, --rank)
    {
        currentSquare = rank * Board::WIDTH + file;
        attacks |= (1ULL << currentSquare);

        if ((1ULL << currentSquare) & blockers) { break; }
    }
    // South-east
    for (int file = sourceX - 1, rank = sourceY - 1; file >= 0 && rank >= 0; --file, --rank)
    {
        currentSquare = rank * Board::WIDTH + file;
        attacks |= (1ULL << currentSquare);

        if ((1ULL << currentSquare) & blockers) { break; }
    }

    return attacks;
}

std::array<std::array<BitBoard, 4096>, Board::SQUARES_COUNT> MovesGenerator::InitInternalRookAttacks()
{
    std::array<std::array<BitBoard, 4096>, Board::SQUARES_COUNT> result;

    for (int i = 0; i < Board::SQUARES_COUNT; ++i)
    {
        int maskBitsCount = CountSetBits(ROOK_ATTACKS_MASKS[i]);
        int blockersVariationsCount = 1 << maskBitsCount;

        for (int j = 0; j < blockersVariationsCount; ++j)
        {
            BitBoard attackMask = ROOK_ATTACKS_MASKS[i];
            BitBoard blockers = 0ULL;
            int index = 0;

            while (attackMask)
            {
                int square = IndexOfLSB(attackMask);

                if (j & (1 << index))
                {
                    blockers |= (1ULL << square);
                }

                ++index;
                RemoveLSB(attackMask);
            }

            BitBoard attacks = RookAttacksWithBlockers(i / 8, i % 8, blockers);

            uint64_t magicIndex = (blockers * ROOK_MAGIC_NUMBERS[i]) >> (64 - ROOK_RELEVANT_SQUARES[i]);
            result[i][magicIndex] = attacks;
        }
    }

    return result;
}

std::array<std::array<BitBoard, 512>, Board::SQUARES_COUNT> MovesGenerator::InitInternalBishopAttacks()
{
    std::array<std::array<BitBoard, 512>, Board::SQUARES_COUNT> result;

    for (int i = 0; i < Board::SQUARES_COUNT; ++i)
    {
        int maskBitsCount = CountSetBits(BISHOP_ATTACKS_MASKS[i]);
        int blockersVariationsCount = 1 << maskBitsCount;

        for (int j = 0; j < blockersVariationsCount; ++j)
        {
            BitBoard attackMask = BISHOP_ATTACKS_MASKS[i];
            BitBoard blockers = 0ULL;
            int index = 0;

            while (attackMask)
            {
                int square = IndexOfLSB(attackMask);

                if (j & (1 << index))
                {
                    blockers |= (1ULL << square);
                }

                ++index;
                RemoveLSB(attackMask);
            }

            BitBoard attacks = BishopAttacksWithBlockers(i / 8, i % 8, blockers);

            uint64_t magicIndex = (blockers * BISHOP_MAGIC_NUMBERS[i]) >> (64 - BISHOP_RELEVANT_SQUARES[i]);
            result[i][magicIndex] = attacks;
        }
    }

    return result;
}

const std::array<std::array<BitBoard, 4096>, Board::SQUARES_COUNT> MovesGenerator::ROOK_ATTACKS = InitInternalRookAttacks();
const std::array<std::array<BitBoard, 512>, Board::SQUARES_COUNT> MovesGenerator::BISHOP_ATTACKS = InitInternalBishopAttacks();

BitBoard MovesGenerator::GetRookAttacks(uint8_t square, BitBoard blockers)
{
    blockers &= ROOK_ATTACKS_MASKS[square];
    blockers *= ROOK_MAGIC_NUMBERS[square];
    blockers >>= 64 - ROOK_RELEVANT_SQUARES[square];

    return ROOK_ATTACKS[square][blockers];
}

BitBoard MovesGenerator::GetBishopAttacks(uint8_t square, BitBoard blockers)
{
    blockers &= BISHOP_ATTACKS_MASKS[square];
    blockers *= BISHOP_MAGIC_NUMBERS[square];
    blockers >>= 64 - BISHOP_RELEVANT_SQUARES[square];

    return BISHOP_ATTACKS[square][blockers];
}

std::unique_ptr<std::array<std::list<Move>, MovesGenerator::PRIORITIES_COUNT>> MovesGenerator::GenerateLegalChessMovesSorted(const Position& position, unsigned int& numberOfCaptureMoves)
{
    std::unique_ptr<std::array<std::list<Move>, PRIORITIES_COUNT>> legalPiecesMoves = std::make_unique<std::array<std::list<Move>, 7>>();

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
            (*legalPiecesMoves)[PRIORITY_LAST].emplace_back(Move((Square)currentKnightIndex, (Square)IndexOfLSB(knightMovesToEmpties)));
            RemoveLSB(knightMovesToEmpties);
        }

        while(knightMovesToOpponentsSquare)
        {
            int knightMoveToOpponentsSquareIndex = IndexOfLSB(knightMovesToOpponentsSquare);
            BitBoard knightMoveToOpponentsSquare = 1ULL << knightMoveToOpponentsSquareIndex;
            Move::AdditionalInfo capturedPiece = SquarePieceTypeToMoveInfo(knightMoveToOpponentsSquare, position.board, opponentsPiecesColor);
            (*legalPiecesMoves)[MoveInfoToListIndex.at(capturedPiece)].emplace_front(Move((Square)currentKnightIndex, (Square)knightMoveToOpponentsSquareIndex,
                                           capturedPiece));
            ++numberOfCaptureMoves;
            RemoveLSB(knightMovesToOpponentsSquare);
        }

        RemoveLSB(movingKnights);
    }

    // Long range pieces moves
    BitBoard blockers = position.board.bitBoards[Board::Type::All][movingPiecesColor] |
                        position.board.bitBoards[Board::Type::All][opponentsPiecesColor] |
                        position.board.duck;
    // Bishops moves
    //GenerateLongRangePieceMoves(legalPiecesMoves, Board::Type::Bishops, movingPiecesColor, position.board, numberOfCaptureMoves);

    BitBoard movingPieces = position.board.bitBoards[Board::Type::Bishops][movingPiecesColor];

    while (movingPieces)
    {
        int currentPieceIndex = IndexOfLSB(movingPieces);
        BitBoard bishopMoves = GetBishopAttacks(currentPieceIndex, blockers);
        bishopMoves &= ~(position.board.bitBoards[Board::Type::All][movingPiecesColor] | position.board.duck);
        BitBoard movesToEmpties = bishopMoves & position.board.empties;
        BitBoard movesTakes = bishopMoves & position.board.bitBoards[Board::Type::All][opponentsPiecesColor];

        while(movesToEmpties)
        {
            (*legalPiecesMoves)[PRIORITY_LAST].emplace_back(Move((Square)currentPieceIndex, (Square)IndexOfLSB(movesToEmpties)));
            RemoveLSB(movesToEmpties);
        }

        while(movesTakes)
        {
            int moveTakeIndex = IndexOfLSB(movesTakes);
            BitBoard moveTake = 1ULL << moveTakeIndex;
             Move::AdditionalInfo capturedPiece = SquarePieceTypeToMoveInfo(moveTake, position.board, opponentsPiecesColor);
            (*legalPiecesMoves)[MoveInfoToListIndex.at(capturedPiece)].emplace_front(Move((Square)currentPieceIndex, (Square)moveTakeIndex,
                                           capturedPiece));
            ++numberOfCaptureMoves;
            RemoveLSB(movesTakes);
        }

        RemoveLSB(movingPieces);
    }


    // Rooks moves
    //GenerateLongRangePieceMoves(legalPiecesMoves, Board::Type::Rooks, movingPiecesColor, position.board, numberOfCaptureMoves);

    movingPieces = position.board.bitBoards[Board::Type::Rooks][movingPiecesColor];

    while (movingPieces)
    {
        int currentPieceIndex = IndexOfLSB(movingPieces);
        BitBoard rookMoves = GetRookAttacks(currentPieceIndex, blockers);
        rookMoves &= ~(position.board.bitBoards[Board::Type::All][movingPiecesColor] | position.board.duck);
        BitBoard movesToEmpties = rookMoves & position.board.empties;
        BitBoard movesTakes = rookMoves & position.board.bitBoards[Board::Type::All][opponentsPiecesColor];

        while(movesToEmpties)
        {
            (*legalPiecesMoves)[PRIORITY_LAST].emplace_back(Move((Square)currentPieceIndex, (Square)IndexOfLSB(movesToEmpties)));
            RemoveLSB(movesToEmpties);
        }

        while(movesTakes)
        {
            int moveTakeIndex = IndexOfLSB(movesTakes);
            BitBoard moveTake = 1ULL << moveTakeIndex;
             Move::AdditionalInfo capturedPiece = SquarePieceTypeToMoveInfo(moveTake, position.board, opponentsPiecesColor);
            (*legalPiecesMoves)[MoveInfoToListIndex.at(capturedPiece)].emplace_front(Move((Square)currentPieceIndex, (Square)moveTakeIndex,
                                           capturedPiece));
            ++numberOfCaptureMoves;
            RemoveLSB(movesTakes);
        }

        RemoveLSB(movingPieces);
    }

    // Queen moves
    //GenerateLongRangePieceMoves(legalPiecesMoves, Board::Type::Queens, movingPiecesColor, position.board, numberOfCaptureMoves);

    movingPieces = position.board.bitBoards[Board::Type::Queens][movingPiecesColor];

    while (movingPieces)
    {
        int currentPieceIndex = IndexOfLSB(movingPieces);
        BitBoard queenMoves = GetRookAttacks(currentPieceIndex, blockers) | GetBishopAttacks(currentPieceIndex, blockers);
        queenMoves &= ~(position.board.bitBoards[Board::Type::All][movingPiecesColor] | position.board.duck);
        BitBoard movesToEmpties = queenMoves & position.board.empties;
        BitBoard movesTakes = queenMoves & position.board.bitBoards[Board::Type::All][opponentsPiecesColor];

        while(movesToEmpties)
        {
            (*legalPiecesMoves)[PRIORITY_LAST].emplace_back(Move((Square)currentPieceIndex, (Square)IndexOfLSB(movesToEmpties)));
            RemoveLSB(movesToEmpties);
        }

        while(movesTakes)
        {
            int moveTakeIndex = IndexOfLSB(movesTakes);
            BitBoard moveTake = 1ULL << moveTakeIndex;
            Move::AdditionalInfo capturedPiece = SquarePieceTypeToMoveInfo(moveTake, position.board, opponentsPiecesColor);
            (*legalPiecesMoves)[MoveInfoToListIndex.at(capturedPiece)].emplace_front(Move((Square)currentPieceIndex, (Square)moveTakeIndex,
                                           capturedPiece));
            ++numberOfCaptureMoves;
            RemoveLSB(movesTakes);
        }

        RemoveLSB(movingPieces);
    }

    // King moves
    //// One square moves
    int kingIndex = IndexOfLSB(position.board.bitBoards[Board::Type::Kings][movingPiecesColor]);
    BitBoard movingKingAttacks = KING_ATTACKS[kingIndex];
    BitBoard kingMovesToEmpties = movingKingAttacks & position.board.empties;
    BitBoard kingMovesToOpponentsSquare = movingKingAttacks & position.board.bitBoards[Board::Type::All][opponentsPiecesColor];

    while(kingMovesToEmpties)
    {
        (*legalPiecesMoves)[PRIORITY_LAST].emplace_back(Move((Square)kingIndex, (Square)IndexOfLSB(kingMovesToEmpties)));
        RemoveLSB(kingMovesToEmpties);
    }

    while(kingMovesToOpponentsSquare)
    {
        int kingMoveToOpponentsSquareIndex = IndexOfLSB(kingMovesToOpponentsSquare);
        BitBoard kingMoveToOpponentsSquare = 1ULL << kingMoveToOpponentsSquareIndex;
        Move::AdditionalInfo capturedPiece = SquarePieceTypeToMoveInfo(kingMoveToOpponentsSquare, position.board, opponentsPiecesColor);
        (*legalPiecesMoves)[MoveInfoToListIndex.at(capturedPiece)].emplace_front(Move((Square)kingIndex, (Square)kingMoveToOpponentsSquareIndex,
                                       capturedPiece));
        ++numberOfCaptureMoves;
        RemoveLSB(kingMovesToOpponentsSquare);
    }

    //// Castling
    if (position.playerToMove == PlayerColor::White)
    {
        if ((CastlingRights)((uint8_t)position.castlingRights & (uint8_t)CastlingRights::WhiteKingside) != CastlingRights::None &&
             CheckBit(position.board.empties, Square::F1) && CheckBit(position.board.empties, Square::G1))
        {
            (*legalPiecesMoves)[PRIORITY_LAST].emplace_front(Move((Square)kingIndex, Square::G1, Move::AdditionalInfo::WhiteKingsideCastle));
        }
        if ((CastlingRights)((uint8_t)position.castlingRights & (uint8_t)CastlingRights::WhiteQueenside) != CastlingRights::None &&
            CheckBit(position.board.empties, Square::B1) && CheckBit(position.board.empties, Square::C1) && CheckBit(position.board.empties, Square::D1))
        {
            (*legalPiecesMoves)[PRIORITY_LAST].emplace_front(Move((Square)kingIndex, Square::C1, Move::AdditionalInfo::WhiteQueensideCastle));
        }
    }
    else
    {
        if ((CastlingRights)((uint8_t)position.castlingRights & (uint8_t)CastlingRights::BlackKingside) != CastlingRights::None &&
            CheckBit(position.board.empties, Square::F8) && CheckBit(position.board.empties, Square::G8))
        {
            (*legalPiecesMoves)[PRIORITY_LAST].emplace_front(Move((Square)kingIndex, Square::G8, Move::AdditionalInfo::BlackKingsideCastle));
        }
        if ((CastlingRights)((uint8_t)position.castlingRights & (uint8_t)CastlingRights::BlackQueenside) != CastlingRights::None &&
            CheckBit(position.board.empties, Square::B8) && CheckBit(position.board.empties, Square::C8) && CheckBit(position.board.empties, Square::D8))
        {
            (*legalPiecesMoves)[PRIORITY_LAST].emplace_front(Move((Square)kingIndex, Square::C8, Move::AdditionalInfo::BlackQueensideCastle));
        }
    }

    // Pawn moves
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
                (*legalPiecesMoves)[PRIORITY_LAST].emplace_front(Move((Square)currentPawnIndex, (Square)forwardSquare, Move::AdditionalInfo::PromotionToKnight));
                (*legalPiecesMoves)[PRIORITY_LAST].emplace_front(Move((Square)currentPawnIndex, (Square)forwardSquare, Move::AdditionalInfo::PromotionToBishop));
                (*legalPiecesMoves)[PRIORITY_LAST].emplace_front(Move((Square)currentPawnIndex, (Square)forwardSquare, Move::AdditionalInfo::PromotionToQueen));
                (*legalPiecesMoves)[PRIORITY_LAST].emplace_front(Move((Square)currentPawnIndex, (Square)forwardSquare, Move::AdditionalInfo::PromotionToRook));
            }
            else
            {
                (*legalPiecesMoves)[PRIORITY_LAST].emplace_back(Move((Square)currentPawnIndex, (Square)forwardSquare));

                // Moving two squares forward
                doubleForwardSquare = forwardSquare + forwardDirection;

                if (CheckBit(doubleForwardRank, currentPawnIndex) && CheckBit(position.board.empties, doubleForwardSquare))
                {
                    (*legalPiecesMoves)[PRIORITY_LAST].emplace_back(Move((Square)currentPawnIndex, (Square)doubleForwardSquare, Move::AdditionalInfo::PawnDoubleForward));
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
                    (*legalPiecesMoves)[MoveInfoToListIndex.at(capturedOnForwardWest)].emplace_front(Move((Square)currentPawnIndex, (Square)forwardWestSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToKnight | (uint16_t)capturedOnForwardWest)));
                    (*legalPiecesMoves)[MoveInfoToListIndex.at(capturedOnForwardWest)].emplace_front(Move((Square)currentPawnIndex, (Square)forwardWestSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToBishop | (uint16_t)capturedOnForwardWest)));
                    (*legalPiecesMoves)[MoveInfoToListIndex.at(capturedOnForwardWest)].emplace_front(Move((Square)currentPawnIndex, (Square)forwardWestSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToRook | (uint16_t)capturedOnForwardWest)));
                    (*legalPiecesMoves)[MoveInfoToListIndex.at(capturedOnForwardWest)].emplace_front(Move((Square)currentPawnIndex, (Square)forwardWestSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToQueen | (uint16_t)capturedOnForwardWest)));
                    numberOfCaptureMoves += 4;
                }
                else
                {
                    (*legalPiecesMoves)[MoveInfoToListIndex.at(capturedOnForwardWest)].emplace_front(Move((Square)currentPawnIndex, (Square)forwardWestSquare, capturedOnForwardWest));
                    ++numberOfCaptureMoves;
                }
            }
            else if (forwardWestSquare == position.enPassantTarget && forwardWestSquare != IndexOfLSB(position.board.duck))
            {
                (*legalPiecesMoves)[MoveInfoToListIndex.at(Move::AdditionalInfo::CapturedPawn)].emplace_front(Move((Square)currentPawnIndex, (Square)forwardWestSquare, Move::AdditionalInfo::EnPassant));
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
                    (*legalPiecesMoves)[MoveInfoToListIndex.at(capturedOnForwardEast)].emplace_front(Move((Square)currentPawnIndex, (Square)forwardEastSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToKnight | (uint16_t)capturedOnForwardEast)));
                    (*legalPiecesMoves)[MoveInfoToListIndex.at(capturedOnForwardEast)].emplace_front(Move((Square)currentPawnIndex, (Square)forwardEastSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToBishop | (uint16_t)capturedOnForwardEast)));
                    (*legalPiecesMoves)[MoveInfoToListIndex.at(capturedOnForwardEast)].emplace_front(Move((Square)currentPawnIndex, (Square)forwardEastSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToRook | (uint16_t)capturedOnForwardEast)));
                    (*legalPiecesMoves)[MoveInfoToListIndex.at(capturedOnForwardEast)].emplace_front(Move((Square)currentPawnIndex, (Square)forwardEastSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToQueen | (uint16_t)capturedOnForwardEast)));
                    numberOfCaptureMoves += 4;
                }
                else
                {
                    (*legalPiecesMoves)[MoveInfoToListIndex.at(capturedOnForwardEast)].emplace_front(Move((Square)currentPawnIndex, (Square)forwardEastSquare, capturedOnForwardEast));
                    ++numberOfCaptureMoves;
                }
            }
            else if (forwardEastSquare == position.enPassantTarget && forwardEastSquare != IndexOfLSB(position.board.duck))
            {
                (*legalPiecesMoves)[MoveInfoToListIndex.at(Move::AdditionalInfo::CapturedPawn)].emplace_front(Move((Square)currentPawnIndex, (Square)forwardEastSquare, Move::AdditionalInfo::EnPassant));
                ++numberOfCaptureMoves;
            }
        }

        RemoveLSB(movingPawns);
    }

    return legalPiecesMoves;
}
