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

unsigned MovesGenerator::squaresToEdgeCount[Board::SQUARES_COUNT][DIRECTIONS_COUNT];

MovesGenerator::MovesGenerator()
{
    for (int y = 0; y < Board::HEIGHT; ++y)
    {
        for (int x = 0; x < Board::WIDTH; ++x)
        {
            unsigned currentIndex = y * Board::WIDTH + x;

            squaresToEdgeCount[currentIndex][0] = Board::HEIGHT - y - 1;
            squaresToEdgeCount[currentIndex][1] = y;
            squaresToEdgeCount[currentIndex][2] = x;
            squaresToEdgeCount[currentIndex][3] = Board::WIDTH - x - 1;
            squaresToEdgeCount[currentIndex][4] = std::min(squaresToEdgeCount[currentIndex][0], squaresToEdgeCount[currentIndex][2]);
            squaresToEdgeCount[currentIndex][5] = std::min(squaresToEdgeCount[currentIndex][0], squaresToEdgeCount[currentIndex][3]);
            squaresToEdgeCount[currentIndex][6] = std::min(squaresToEdgeCount[currentIndex][1], squaresToEdgeCount[currentIndex][2]);
            squaresToEdgeCount[currentIndex][7] = std::min(squaresToEdgeCount[currentIndex][1], squaresToEdgeCount[currentIndex][3]);
        }
    }
}

std::unique_ptr<std::list<Move>> MovesGenerator::GenerateLegalChessMoves(const Position& position, unsigned int& numberOfCaptureMoves, bool calledByGenerateLegalMoves)
{
    std::unique_ptr<std::list<Move>> legalPiecesMoves = std::make_unique<std::list<Move>>();

    for (unsigned y = (unsigned)Ranks::Rank1; y <= (unsigned)Ranks::Rank8; ++y)
    {
        for (unsigned x = (unsigned)Files::FileA; x <= (unsigned)Files::FileH; ++x)
        {
            Piece const* currentPiece = &position.board.pieces[y][x];

            AssignSquare(currentPiece, y, x, position.playerToMove);
        }
    }

    // Knights moves
    if (playerPiecesSquares.contains(Piece::Type::Knight))
    {
        for (const Square& knightSquare : playerPiecesSquares.at(Piece::Type::Knight))
        {
            std::unique_ptr<std::list<Square>> squaresKnightCanMoveTo = GenerateAllSquaresKnightMovesTo(knightSquare);

            for (const Square& squareKnightCanMoveTo : *squaresKnightCanMoveTo)
            {
                if (emptySquares.contains(squareKnightCanMoveTo))
                {
                    legalPiecesMoves->emplace_back(Move(knightSquare, squareKnightCanMoveTo));
                }
                else if (opponentPiecesSquares.contains(squareKnightCanMoveTo))
                {
                    legalPiecesMoves->emplace_front(Move(knightSquare, squareKnightCanMoveTo, SquarePieceTypeToMoveInfo(squareKnightCanMoveTo, position.board)));
                    ++numberOfCaptureMoves;
                }
            }
        }
    }

    // Bishops moves
    GenerateLongRangePieceMoves(*legalPiecesMoves, Piece::Type::Bishop, position.board, numberOfCaptureMoves);

    // Rooks moves
    GenerateLongRangePieceMoves(*legalPiecesMoves, Piece::Type::Rook, position.board, numberOfCaptureMoves);

    // Queen moves
    GenerateLongRangePieceMoves(*legalPiecesMoves, Piece::Type::Queen, position.board, numberOfCaptureMoves);

    // King moves
    Square squareInDirection;

    for (const Square& kingSquare : playerPiecesSquares.at(Piece::Type::King))
    {
        for (unsigned d = 0; d < DIRECTIONS_COUNT; ++d)
        {
            //// One square moves
            squareInDirection = (Square)((uint8_t)kingSquare + (int8_t)DIRECTIONS[d]);

            if (squaresToEdgeCount[(uint8_t)kingSquare - 1][d] > 0)
            {
                if (emptySquares.contains(squareInDirection))
                {
                    legalPiecesMoves->emplace_back(Move(kingSquare, squareInDirection));
                }
                else if (opponentPiecesSquares.contains(squareInDirection))
                {
                    legalPiecesMoves->emplace_front(Move(kingSquare, squareInDirection, SquarePieceTypeToMoveInfo(squareInDirection, position.board)));
                    ++numberOfCaptureMoves;
                }
            }
        }

        //// Castling
        if (position.playerToMove == PlayerColor::White)
        {
            if ((CastlingRights)((uint8_t)position.castlingRights & (uint8_t)CastlingRights::WhiteKingside) != CastlingRights::None &&
                emptySquares.contains(Square::F1) && emptySquares.contains(Square::G1))
            {
                legalPiecesMoves->emplace_back(Move(kingSquare, Square::G1, Move::AdditionalInfo::WhiteKingsideCastle));
            }
            if ((CastlingRights)((uint8_t)position.castlingRights & (uint8_t)CastlingRights::WhiteQueenside) != CastlingRights::None &&
                emptySquares.contains(Square::B1) && emptySquares.contains(Square::C1) && emptySquares.contains(Square::D1))
            {
                legalPiecesMoves->emplace_back(Move(kingSquare, Square::C1, Move::AdditionalInfo::WhiteQueensideCastle));
            }
        }
        else
        {
            if ((CastlingRights)((uint8_t)position.castlingRights & (uint8_t)CastlingRights::BlackKingside) != CastlingRights::None &&
                emptySquares.contains(Square::F8) && emptySquares.contains(Square::G8))
            {
                legalPiecesMoves->emplace_back(Move(kingSquare, Square::G8, Move::AdditionalInfo::BlackKingsideCastle));
            }
            if ((CastlingRights)((uint8_t)position.castlingRights & (uint8_t)CastlingRights::BlackQueenside) != CastlingRights::None &&
                emptySquares.contains(Square::B8) && emptySquares.contains(Square::C8) && emptySquares.contains(Square::D8))
            {
                legalPiecesMoves->emplace_back(Move(kingSquare, Square::C8, Move::AdditionalInfo::BlackQueensideCastle));
            }
        }
    }

    // Pawn moves
    GeneratePawnsMoves(*legalPiecesMoves, position, numberOfCaptureMoves);

    if (!calledByGenerateLegalMoves)
    {
        emptySquares.clear();
        playerPiecesSquares.clear();
        opponentPiecesSquares.clear();
        opponentPawnsSquares.clear();
        duckSquare = Square::None;
    }

    return legalPiecesMoves;
}

std::unique_ptr<std::vector<FullMove>> MovesGenerator::GenerateLegalMoves(const Position& position)
{
    unsigned int numberOfCaptureMoves = 0;
    std::unique_ptr<std::list<Move>> legalPiecesMoves = GenerateLegalChessMoves(position, numberOfCaptureMoves, true);

    //std::sort(legalPiecesMoves.begin(), legalPiecesMoves.end(),
    //	[](const Move& move1, const Move& move2)
    //	{
    //		return move1.additionalInfo < move2.additionalInfo;
    //	});

    auto legalMoves = std::make_unique<std::vector<FullMove>>();
    (*legalMoves).reserve(legalPiecesMoves->size() * emptySquares.size() + numberOfCaptureMoves);

    //for (const Move& legalPieceMove : legalPiecesMoves)
    //{
    //	legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, legalPieceMove.sourceSquare));
    //}

    for (const Move& legalPieceMove : *legalPiecesMoves)
    {
        switch (legalPieceMove.additionalInfo)
        {
            case Move::AdditionalInfo::BlackKingsideCastle:
            case Move::AdditionalInfo::WhiteKingsideCastle:
            case Move::AdditionalInfo::BlackQueensideCastle:
            case Move::AdditionalInfo::WhiteQueensideCastle:
            {
                const auto& [rookSourceSquare, rookTargetSquare] = Move::ROOK_CASTLING_SQUARES.at(legalPieceMove.additionalInfo);

                for (const Square& emptySquare : emptySquares)
                {
                    if (emptySquare != legalPieceMove.targetSquare && emptySquare != rookTargetSquare)
                    {
                        legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, emptySquare));
                    }
                }

                legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, legalPieceMove.sourceSquare));
                legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, rookSourceSquare));

                break;
            }
            case Move::AdditionalInfo::EnPassant:
            {
                for (const Square& emptySquare : emptySquares)
                {
                    if (emptySquare != legalPieceMove.targetSquare)
                    {
                        legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, emptySquare));
                    }
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
                for (const Square& emptySquare : emptySquares)
                {
                    if (emptySquare != legalPieceMove.targetSquare)
                    {
                        legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, emptySquare));
                    }
                }

                legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, legalPieceMove.sourceSquare));

                break;
            }
        }
    }

    //std::shuffle((*legalMoves).begin(), (*legalMoves).end(), std::default_random_engine());

    /*for (const Square& emptySquare : emptySquares)
    {
        for (const Move& legalPieceMove : legalPiecesMoves)
        {
            switch (legalPieceMove.additionalInfo)
            {
                case Move::AdditionalInfo::BlackKingsideCastle:
                case Move::AdditionalInfo::WhiteKingsideCastle:
                case Move::AdditionalInfo::BlackQueensideCastle:
                case Move::AdditionalInfo::WhiteQueensideCastle:
                {
                    const std::pair<Square, Square>& rookCastlingSquares = Move::ROOK_CASTLING_SQUARES.at(legalPieceMove.additionalInfo);

                    if (emptySquare != legalPieceMove.targetSquare && emptySquare != rookCastlingSquares.second)
                    {
                        legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, emptySquare));
                    }

                    legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, legalPieceMove.sourceSquare));
                    legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, rookCastlingSquares.first));

                    break;
                }
                case Move::AdditionalInfo::EnPassant:
                {
                    if (emptySquare != legalPieceMove.targetSquare)
                    {
                        legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, emptySquare));
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
                    if (emptySquare != legalPieceMove.targetSquare)
                    {
                        legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, emptySquare));
                    }

                    legalMoves->emplace_back(FullMove(legalPieceMove, duckSquare, legalPieceMove.sourceSquare));

                    break;
                }
            }
        }
    }*/

    return legalMoves;
}

void MovesGenerator::AssignSquare(Piece const* piece, unsigned& y, unsigned& x, PlayerColor movingPlayerColor)
{
    if (piece->PieceType() == Piece::Type::None)
    {
        emptySquares.insert(BoardIndicesToSquare(y, x));
        return;
    }

    if (piece->PieceType() == Piece::Type::Duck)
    {
        duckSquare = (BoardIndicesToSquare(y, x));
        return;
    }

    if (piece->PieceColor() == (Piece::Color)movingPlayerColor)
    {
        if (!playerPiecesSquares.contains(piece->PieceType()))
        {
            playerPiecesSquares.insert(std::pair(piece->PieceType(), std::unordered_set<Square>()));
        }

        playerPiecesSquares.at(piece->PieceType()).insert(BoardIndicesToSquare(y, x));
    }
    else
    {
        opponentPiecesSquares.insert(BoardIndicesToSquare(y, x));

        if (piece->PieceType() == Piece::Type::Pawn)
        {
            opponentPawnsSquares.insert(BoardIndicesToSquare(y, x));
        }
    }
}

std::unique_ptr<std::list<Square>> MovesGenerator::GenerateAllSquaresKnightMovesTo(const Square& startingSquare)
{
    std::unique_ptr<std::list<Square>> knightSquares = std::make_unique<std::list<Square>>(std::list<Square>());

    // 2x north
    uint8_t currentDirectionOffset = (uint8_t)startingSquare + (uint8_t)DirectionOffsets::North;

    if (startingSquare < Square::A7)
    {
        if ((uint8_t)startingSquare % Board::WIDTH != 1)
        {
            knightSquares->emplace_back((Square)(currentDirectionOffset + (uint8_t)DirectionOffsets::NorthWest));
        }
        if ((uint8_t)startingSquare % Board::WIDTH != 0)
        {
            knightSquares->emplace_back((Square)(currentDirectionOffset + (uint8_t)DirectionOffsets::NorthEast));
        }
    }

    // 2x south
    currentDirectionOffset = (uint8_t)startingSquare + (uint8_t)DirectionOffsets::South;

    if (startingSquare >= Square::A3)
    {
        if ((uint8_t)startingSquare % Board::WIDTH != 1)
        {
            knightSquares->emplace_back((Square)(currentDirectionOffset + (uint8_t)DirectionOffsets::SouthWest));
        }
        if ((uint8_t)startingSquare % Board::WIDTH != 0)
        {
            knightSquares->emplace_back((Square)(currentDirectionOffset + (uint8_t)DirectionOffsets::SouthEast));
        }
    }

    // 2x west
    currentDirectionOffset = (uint8_t)startingSquare + (uint8_t)DirectionOffsets::West;

    if ((uint8_t)startingSquare % Board::WIDTH != 1 && (uint8_t)startingSquare % Board::WIDTH != 2)
    {
        if (startingSquare >= Square::A2)
        {
            knightSquares->emplace_back((Square)(currentDirectionOffset + (uint8_t)DirectionOffsets::SouthWest));
        }
        if (startingSquare < Square::A8)
        {
            knightSquares->emplace_back((Square)(currentDirectionOffset + (uint8_t)DirectionOffsets::NorthWest));
        }
    }

    // 2x east
    currentDirectionOffset = (uint8_t)startingSquare + (uint8_t)DirectionOffsets::East;

    if ((uint8_t)startingSquare % Board::WIDTH != Board::WIDTH - 1 && (uint8_t)startingSquare % Board::WIDTH != 0)
    {
        if (startingSquare >= Square::A2)
        {
            knightSquares->emplace_back((Square)(currentDirectionOffset + (uint8_t)DirectionOffsets::SouthEast));
        }
        if (startingSquare < Square::A8)
        {
            knightSquares->emplace_back((Square)(currentDirectionOffset + (uint8_t)DirectionOffsets::NorthEast));
        }
    }

    return knightSquares;
}

void MovesGenerator::GenerateLongRangePieceMoves(std::list<Move>& legalMoves, Piece::Type pieceType, const Board& board, unsigned int& numberOfCaptureMoves)
{
    if (!playerPiecesSquares.contains(pieceType))
    {
        return;
    }

    Square squareInDirection;
    unsigned lowBound = 0;
    unsigned highBound = DIRECTIONS_COUNT;

    if (pieceType == Piece::Type::Bishop)
    {
        // Starting with d = 4 (only diagonal directions)
        lowBound = 4;
    }
    else if (pieceType == Piece::Type::Rook)
    {
        // Ending with d = 4 (only horizontal and vertical directions)
        highBound = 4;
    }

    for (const Square& pieceSquare : playerPiecesSquares.at(pieceType))
    {
        for (unsigned d = lowBound; d < highBound; ++d)
        {
            squareInDirection = pieceSquare;

            for (unsigned s = 0; s < squaresToEdgeCount[(uint8_t)pieceSquare - 1][d]; ++s)
            {
                squareInDirection = (Square)((uint8_t)squareInDirection + (int8_t)DIRECTIONS[d]);

                if (emptySquares.contains(squareInDirection))
                {
                    legalMoves.emplace_back(Move(pieceSquare, squareInDirection));
                }
                else if (opponentPiecesSquares.contains(squareInDirection))
                {
                    legalMoves.emplace_front(Move(pieceSquare, squareInDirection, SquarePieceTypeToMoveInfo(squareInDirection, board)));
                    ++numberOfCaptureMoves;
                    break;
                }
                else
                {
                    break;
                }
            }
        }
    }
}

Move::AdditionalInfo MovesGenerator::SquarePieceTypeToMoveInfo(const Square& square, const Board& board)
{
    int y;
    int x;

    SquareToBoardIndices(square, y, x);

    switch (board.pieces[y][x].PieceType())
    {
        case Piece::Type::Pawn: { return Move::AdditionalInfo::CapturedPawn; }
        case Piece::Type::Knight: { return Move::AdditionalInfo::CapturedKnight; }
        case Piece::Type::Bishop: { return Move::AdditionalInfo::CapturedBishop; }
        case Piece::Type::Rook: { return Move::AdditionalInfo::CapturedRook; }
        case Piece::Type::Queen: { return Move::AdditionalInfo::CapturedQueen; }
        case Piece::Type::King: { return Move::AdditionalInfo::CapturedKing; }
        default: { return Move::AdditionalInfo::None; }
    }
}

void MovesGenerator::GeneratePawnsMoves(std::list<Move>& legalPiecesMoves, const Position& position, unsigned int& numberOfCaptureMoves)
{
    if (playerPiecesSquares.contains(Piece::Type::Pawn))
    {
        for (const Square& pawnSquare : playerPiecesSquares.at(Piece::Type::Pawn))
        {
            // White pawns
            if (position.playerToMove == PlayerColor::White)
            {
                //// One square forward
                Square northSquare = (Square)((int8_t)pawnSquare + (int8_t)DirectionOffsets::North);

                if (emptySquares.contains(northSquare))
                {
                    //// Promoting
                    if (northSquare >= Square::A8)
                    {
                        legalPiecesMoves.emplace_front(Move(pawnSquare, northSquare, Move::AdditionalInfo::PromotionToKnight));
                        legalPiecesMoves.emplace_front(Move(pawnSquare, northSquare, Move::AdditionalInfo::PromotionToBishop));
                        legalPiecesMoves.emplace_front(Move(pawnSquare, northSquare, Move::AdditionalInfo::PromotionToQueen));
                        legalPiecesMoves.emplace_front(Move(pawnSquare, northSquare, Move::AdditionalInfo::PromotionToRook));
                    }
                    else
                    {
                        legalPiecesMoves.emplace_back(Move(pawnSquare, northSquare));

                        //// Two squares forward
                        Square doubleNorthSquare = (Square)((int8_t)northSquare + (int8_t)DirectionOffsets::North);

                        //TODO: Maybe upgrade this a bit
                        if (pawnSquare >= Square::A2 && pawnSquare <= Square::H2 && emptySquares.contains(doubleNorthSquare))
                        {
                            legalPiecesMoves.emplace_back(Move(pawnSquare, doubleNorthSquare, Move::AdditionalInfo::PawnDoubleForward));
                        }
                    }
                }

                //// Takes and en passant
                Square northWestSquare = (Square)((uint8_t)pawnSquare + (int8_t)DirectionOffsets::NorthWest);
                Square northEastSquare = (Square)((uint8_t)pawnSquare + (int8_t)DirectionOffsets::NorthEast);

                if ((uint8_t)pawnSquare % Board::WIDTH != 1)
                {
                    if (opponentPiecesSquares.contains(northWestSquare))
                    {
                        Move::AdditionalInfo capturedOnNorthWest = SquarePieceTypeToMoveInfo(northWestSquare, position.board);

                        // Takes and promotes
                        if (northWestSquare >= Square::A8)
                        {
                            legalPiecesMoves.emplace_front(Move(pawnSquare, northWestSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToKnight | (uint16_t)capturedOnNorthWest)));
                            legalPiecesMoves.emplace_front(Move(pawnSquare, northWestSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToBishop | (uint16_t)capturedOnNorthWest)));
                            legalPiecesMoves.emplace_front(Move(pawnSquare, northWestSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToRook | (uint16_t)capturedOnNorthWest)));
                            legalPiecesMoves.emplace_front(Move(pawnSquare, northWestSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToQueen | (uint16_t)capturedOnNorthWest)));
                            numberOfCaptureMoves += 4;
                        }
                        else
                        {
                            legalPiecesMoves.emplace_front(Move(pawnSquare, northWestSquare, capturedOnNorthWest));
                            ++numberOfCaptureMoves;
                        }
                    }
                    else if (northWestSquare == position.enPassantTarget && northWestSquare != duckSquare)
                    {
                        legalPiecesMoves.emplace_front(Move(pawnSquare, northWestSquare, Move::AdditionalInfo::EnPassant));
                        ++numberOfCaptureMoves;
                    }
                }
                if ((uint8_t)pawnSquare % Board::WIDTH != 0)
                {
                    if (opponentPiecesSquares.contains(northEastSquare))
                    {
                        Move::AdditionalInfo capturedOnNorthEast = SquarePieceTypeToMoveInfo(northEastSquare, position.board);

                        if (northEastSquare >= Square::A8)
                        {
                            legalPiecesMoves.emplace_front(Move(pawnSquare, northEastSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToKnight | (uint16_t)capturedOnNorthEast)));
                            legalPiecesMoves.emplace_front(Move(pawnSquare, northEastSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToBishop | (uint16_t)capturedOnNorthEast)));
                            legalPiecesMoves.emplace_front(Move(pawnSquare, northEastSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToRook | (uint16_t)capturedOnNorthEast)));
                            legalPiecesMoves.emplace_front(Move(pawnSquare, northEastSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToQueen | (uint16_t)capturedOnNorthEast)));
                            numberOfCaptureMoves += 4;
                        }
                        else
                        {
                            legalPiecesMoves.emplace_front(Move(pawnSquare, northEastSquare, capturedOnNorthEast));
                            ++numberOfCaptureMoves;
                        }
                    }
                    else if (northEastSquare == position.enPassantTarget && northEastSquare != duckSquare)
                    {
                        legalPiecesMoves.emplace_front(Move(pawnSquare, northEastSquare, Move::AdditionalInfo::EnPassant));
                        ++numberOfCaptureMoves;
                    }
                }
            }
            // Black pawns
            else
            {
                //// One square forward
                Square southSquare = (Square)((int8_t)pawnSquare + (int8_t)DirectionOffsets::South);

                if (emptySquares.contains(southSquare))
                {
                    //// Promoting
                    if (southSquare < Square::A2)
                    {
                        legalPiecesMoves.emplace_front(Move(pawnSquare, southSquare, Move::AdditionalInfo::PromotionToKnight));
                        legalPiecesMoves.emplace_front(Move(pawnSquare, southSquare, Move::AdditionalInfo::PromotionToBishop));
                        legalPiecesMoves.emplace_front(Move(pawnSquare, southSquare, Move::AdditionalInfo::PromotionToRook));
                        legalPiecesMoves.emplace_front(Move(pawnSquare, southSquare, Move::AdditionalInfo::PromotionToQueen));
                    }
                    else
                    {
                        legalPiecesMoves.emplace_back(Move(pawnSquare, southSquare));

                        //// Two squares forward
                        Square doubleSouthSquare = (Square)((int8_t)southSquare + (int8_t)DirectionOffsets::South);

                        //TODO: Maybe upgrade this a bit
                        if (pawnSquare >= Square::A7 && pawnSquare <= Square::H7 && emptySquares.contains(doubleSouthSquare))
                        {
                            legalPiecesMoves.emplace_back(Move(pawnSquare, doubleSouthSquare, Move::AdditionalInfo::PawnDoubleForward));
                        }
                    }
                }

                //// Takes and en passant
                Square southWestSquare = (Square)((uint8_t)pawnSquare + (int8_t)DirectionOffsets::SouthWest);
                Square southEastSquare = (Square)((uint8_t)pawnSquare + (int8_t)DirectionOffsets::SouthEast);

                if ((uint8_t)pawnSquare % Board::WIDTH != 1)
                {
                    if (opponentPiecesSquares.contains(southWestSquare))
                    {
                        Move::AdditionalInfo capturedOnSouthWest = SquarePieceTypeToMoveInfo(southWestSquare, position.board);

                        // Takes and promotes
                        if (southWestSquare < Square::A2)
                        {
                            legalPiecesMoves.emplace_front(Move(pawnSquare, southWestSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToKnight | (uint16_t)capturedOnSouthWest)));
                            legalPiecesMoves.emplace_front(Move(pawnSquare, southWestSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToBishop | (uint16_t)capturedOnSouthWest)));
                            legalPiecesMoves.emplace_front(Move(pawnSquare, southWestSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToRook | (uint16_t)capturedOnSouthWest)));
                            legalPiecesMoves.emplace_front(Move(pawnSquare, southWestSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToQueen | (uint16_t)capturedOnSouthWest)));
                            numberOfCaptureMoves += 4;
                        }
                        else
                        {
                            legalPiecesMoves.emplace_front(Move(pawnSquare, southWestSquare, capturedOnSouthWest));
                            ++numberOfCaptureMoves;
                        }
                    }
                    else if (southWestSquare == position.enPassantTarget && southWestSquare != duckSquare)
                    {
                        legalPiecesMoves.emplace_front(Move(pawnSquare, southWestSquare, Move::AdditionalInfo::EnPassant));
                        ++numberOfCaptureMoves;
                    }
                }
                if ((uint8_t)pawnSquare % Board::WIDTH != 0)
                {
                    if (opponentPiecesSquares.contains(southEastSquare))
                    {
                        Move::AdditionalInfo capturedOnSouthEast = SquarePieceTypeToMoveInfo(southEastSquare, position.board);

                        if (southEastSquare < Square::A2)
                        {
                            legalPiecesMoves.emplace_front(Move(pawnSquare, southEastSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToKnight | (uint16_t)capturedOnSouthEast)));
                            legalPiecesMoves.emplace_front(Move(pawnSquare, southEastSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToBishop | (uint16_t)capturedOnSouthEast)));
                            legalPiecesMoves.emplace_front(Move(pawnSquare, southEastSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToRook | (uint16_t)capturedOnSouthEast)));
                            legalPiecesMoves.emplace_front(Move(pawnSquare, southEastSquare, (Move::AdditionalInfo)((uint16_t)Move::AdditionalInfo::PromotionToQueen | (uint16_t)capturedOnSouthEast)));
                            numberOfCaptureMoves += 4;
                        }
                        else
                        {
                            legalPiecesMoves.emplace_front(Move(pawnSquare, southEastSquare, capturedOnSouthEast));
                            ++numberOfCaptureMoves;
                        }
                    }
                    else if (southEastSquare == position.enPassantTarget && southEastSquare != duckSquare)
                    {
                        legalPiecesMoves.emplace_front(Move(pawnSquare, southEastSquare, Move::AdditionalInfo::EnPassant));
                        ++numberOfCaptureMoves;
                    }
                }
            }
        }
    }
}
