#include <string>
#include "FullMove.h"
#include "Board.h"
#include "MovesGenerator.h"

std::unique_ptr<std::vector<std::pair<int, int>>> GenerateSquaresWithBitPiece(const Square& startingSquare, Board::Type pieceType,
    Board::Color pieceColor, const Board& board, const Square moveSourceSquare);
void AddAmbiguousPartToMoveStringFormat(std::string& result, const std::vector<std::pair<int, int>>& otherPiecesCoords,
    int movingPieceY, int movingPieceX, const Square moveSourceSquare);

std::string SquareStringFormat(const Square& square, bool lowerCase = false)
{
    std::string result;

    char fileLetter = lowerCase ? 'a' : 'A';

    result += fileLetter + ((uint8_t)square) % Board::WIDTH;
    result += '1' + ((uint8_t)square) / Board::WIDTH;

    return result;
}

void SquareToBoardIndices(const Square& square, int& y, int& x)
{
    y = ((uint8_t)square) / Board::WIDTH;
    x = ((uint8_t)square) % Board::WIDTH;
}

Square BoardIndicesToSquare(const unsigned& y, const unsigned& x)
{
    return (Square)(y * Board::WIDTH + x);
}

std::string MoveStringFormat(const FullMove& move, const Board& board)
{
    std::string result;
    bool take = ((uint16_t)move.additionalInfo & (uint16_t)Move::captureChecker) != (uint16_t)Move::AdditionalInfo::None;
    BitBoard movingPieceBitBoard = 1ULL << move.sourceSquare;

    Board::Color movingPieceColor =
            (movingPieceBitBoard & board.bitBoards[Board::Type::All][Board::Color::White]) != 0ULL ?
            Board::Color::White : Board::Color::Black;
    Board::Type movingPieceType = board.FindBoardType(movingPieceBitBoard, movingPieceColor);

    int movingPieceY = move.sourceSquare / 8;
    int movingPieceX = move.sourceSquare % 8;

    switch (movingPieceType)
    {
        case Board::Type::Pawns:
        {
            Move::AdditionalInfo promotionType;

            if (take || ((uint16_t)move.additionalInfo & (uint16_t)Move::AdditionalInfo::EnPassant) != (uint16_t)Move::AdditionalInfo::None)
            {
                result += SquareStringFormat(move.sourceSquare, true)[0];
                result += 'x';
            }

            result += SquareStringFormat(move.targetSquare, true);

            if ((promotionType = (Move::AdditionalInfo)((uint16_t)move.additionalInfo & Move::promotionChecker)) != Move::AdditionalInfo::None)
            {
                switch (promotionType)
                {
                    case Move::AdditionalInfo::PromotionToQueen:
                    {
                        result += "=Q";
                        break;
                    }
                    case Move::AdditionalInfo::PromotionToRook:
                    {
                        result += "=R";
                        break;
                    }
                    case Move::AdditionalInfo::PromotionToKnight:
                    {
                        result += "=N";
                        break;
                    }
                    case Move::AdditionalInfo::PromotionToBishop:
                    {
                        result += "=B";
                        break;
                    }
                }
            }

            if (move.targetDuckSquare != Square::None)
            {
                result += Board::PIECES_SYMBOLS_BITPIECES_MAP.at((uint8_t)PieceType::Duck | (uint8_t)PieceColor::Both);
                result += SquareStringFormat(move.targetDuckSquare, true);
            }
            else
            {
                result += '#';
            }

            return result;
        }
        case Board::Type::Kings:
        {
            if ((Move::AdditionalInfo)((uint16_t)move.additionalInfo & Move::castlingChecker) != Move::AdditionalInfo::None)
            {
                if (move.additionalInfo == Move::AdditionalInfo::BlackKingsideCastle ||
                    move.additionalInfo == Move::AdditionalInfo::WhiteKingsideCastle)
                {
                    result += KINGSIDE_CASTLING_SYMBOL;
                }
                else
                {
                    result += QUEENSIDE_CASTLING_SYMBOL;
                }

                if (move.targetDuckSquare != Square::None)
                {
                    result += Board::PIECES_SYMBOLS_BITPIECES_MAP.at((uint8_t)PieceType::Duck | (uint8_t)PieceColor::Both);
                    result += SquareStringFormat(move.targetDuckSquare, true);
                }
                else
                {
                    result += '#';
                }

                return result;
            }
            else
            {
                result += Board::PIECES_SYMBOLS_BITPIECES_MAP.at(Board::BOARD_PIECES_TO_BITPIECES_MAP.at({ movingPieceType, Board::Color::White }));
            }

            break;
        }
        case Board::Type::Knights:
        {
            result += Board::PIECES_SYMBOLS_BITPIECES_MAP.at(Board::BOARD_PIECES_TO_BITPIECES_MAP.at({ movingPieceType, Board::Color::White }));

            BitBoard knightSquares = MovesGenerator::KNIGHT_ATTACKS[move.targetSquare];
            std::vector<std::pair<int, int>> knightSquaresCoords(CountSetBits(knightSquares) - 1);
            int i = 0;

            while (knightSquares)
            {
                int knightSquareIndex = IndexOfLSB(knightSquares);

                if (knightSquareIndex != move.sourceSquare)
                {
                    SquareToBoardIndices((Square)knightSquareIndex, knightSquaresCoords[i].first, knightSquaresCoords[i].second);
                    ++i;
                }

                RemoveLSB(knightSquares);
            }

            std::erase_if(knightSquaresCoords,
                [&board, movingPieceType, movingPieceColor](const std::pair<int, int> coords)
                {
                    //return board.pieces[coords.first][coords.second].GetBitPiece() != movingBitPiece;
                    return !CheckBit(board.bitBoards[movingPieceType][movingPieceColor], coords.first, coords.second);
                });

            AddAmbiguousPartToMoveStringFormat(result, knightSquaresCoords, movingPieceY, movingPieceX, move.sourceSquare);

            break;
        }
        default:
        {
            result += Board::PIECES_SYMBOLS_BITPIECES_MAP.at(Board::BOARD_PIECES_TO_BITPIECES_MAP.at({ movingPieceType, Board::Color::White }));

            auto otherPiecesCoords = GenerateSquaresWithBitPiece(move.targetSquare, movingPieceType, movingPieceColor, board, move.sourceSquare);
            AddAmbiguousPartToMoveStringFormat(result, *otherPiecesCoords, movingPieceY, movingPieceX, move.sourceSquare);

            break;
        }
    }

    if (take)
    {
        result += 'x';
    }

    result += SquareStringFormat(move.targetSquare, true);

    if (move.targetDuckSquare != Square::None)
    {
        result += Board::PIECES_SYMBOLS_BITPIECES_MAP.at((uint8_t)PieceType::Duck | (uint8_t)PieceColor::Both);
        result += SquareStringFormat(move.targetDuckSquare, true);
    }
    else
    {
        result += '#';
    }

    return result;
}

Square SquareIdToSquare(std::string squareId)
{
    if (squareId[0] == '-')
    {
        return Square::None;
    }
    else
    {
        squareId[0] = toupper(squareId[0]);
        return (Square)((squareId[1] - '0' - 1) * Board::WIDTH + squareId[0] - 'A' + 1);
    }
}

// PRIVATE SECTION

std::unique_ptr<std::vector<std::pair<int, int>>> GenerateSquaresWithBitPiece(const Square& startingSquare, Board::Type pieceType,
    Board::Color pieceColor, const Board& board, const Square originalCallingSquare)
{
    Square squareInDirection;
    unsigned lowBound = 0;
    unsigned highBound = MovesGenerator::DIRECTIONS_COUNT;
    auto resultSquares = std::make_unique<std::vector<std::pair<int, int>>>();
    int x;
    int y;

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

    for (unsigned d = lowBound; d < highBound; ++d)
    {
        squareInDirection = startingSquare;

        for (unsigned s = 0; s < MovesGenerator::SQUARES_TO_EDGE_COUNT[(uint8_t)startingSquare][d]; ++s)
        {
            squareInDirection = (Square)((uint8_t)squareInDirection + (int8_t)MovesGenerator::DIRECTIONS[d]);
            SquareToBoardIndices(squareInDirection, y, x);

            if (CheckBit(board.empties, y, x))
            {
                continue;
            }
            //else if (board.pieces[y][x].GetBitPiece() == bitpiece && squareInDirection != originalCallingSquare)
            else if (CheckBit(board.bitBoards[pieceType][pieceColor], y, x) && squareInDirection != originalCallingSquare)
            {
                (*resultSquares).emplace_back(std::make_pair(y, x));
                break;
            }
            else
            {
                break;
            }
        }
    }

    return resultSquares;
}

void AddAmbiguousPartToMoveStringFormat(std::string& result, const std::vector<std::pair<int, int>>& otherPiecesCoords,
    int movingPieceY, int movingPieceX, const Square moveSourceSquare)
{
    if (otherPiecesCoords.size())
    {
        bool fileRelatedExists = false;
        bool rowRelatedExists = false;

        for (const std::pair<int, int>& coords : otherPiecesCoords)
        {
            if (coords.second == movingPieceX)
            {
                fileRelatedExists = true;
            }

            if (coords.first == movingPieceY)
            {
                rowRelatedExists = true;
            }
        }

        if (!(fileRelatedExists || rowRelatedExists))
        {
            result += SquareStringFormat(moveSourceSquare, true)[0];
        }
        else
        {
            if (rowRelatedExists)
            {
                result += SquareStringFormat(moveSourceSquare, true)[0];
            }
            if (fileRelatedExists)
            {
                result += SquareStringFormat(moveSourceSquare, true)[1];
            }
        }
    }
}
