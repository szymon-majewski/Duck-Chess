#include "Board.h"

std::unordered_map<BitPiece, char> Board::PIECES_SYMBOLS_BITPIECES_MAP =
{
    { (uint8_t)PieceType::Pawn | (uint8_t)PieceColor::Black, 'p' },
    { (uint8_t)PieceType::Pawn | (uint8_t)PieceColor::White, 'P' },
    { (uint8_t)PieceType::Knight | (uint8_t)PieceColor::Black, 'n' },
    { (uint8_t)PieceType::Knight | (uint8_t)PieceColor::White, 'N' },
    { (uint8_t)PieceType::Bishop | (uint8_t)PieceColor::Black, 'b' },
    { (uint8_t)PieceType::Bishop | (uint8_t)PieceColor::White, 'B' },
    { (uint8_t)PieceType::Rook | (uint8_t)PieceColor::Black, 'r' },
    { (uint8_t)PieceType::Rook | (uint8_t)PieceColor::White, 'R' },
    { (uint8_t)PieceType::Queen | (uint8_t)PieceColor::Black, 'q' },
    { (uint8_t)PieceType::Queen | (uint8_t)PieceColor::White, 'Q' },
    { (uint8_t)PieceType::King | (uint8_t)PieceColor::Black, 'k' },
    { (uint8_t)PieceType::King | (uint8_t)PieceColor::White, 'K' },
    { (uint8_t)PieceType::Duck | (uint8_t)PieceColor::Both, '@' },
    { (uint8_t)PieceType::None, ' ' }
};

std::unordered_map<std::pair<Board::Type, Board::Color>, BitPiece, Board::HashPair> Board::BOARD_PIECES_TO_BITPIECES_MAP =
{
    { { Board::Type::Pawns, Board::Color::White }, (uint8_t)PieceType::Pawn | (uint8_t)PieceColor::White },
    { { Board::Type::Pawns, Board::Color::Black }, (uint8_t)PieceType::Pawn | (uint8_t)PieceColor::Black },
    { { Board::Type::Knights, Board::Color::White }, (uint8_t)PieceType::Knight | (uint8_t)PieceColor::White },
    { { Board::Type::Knights, Board::Color::Black }, (uint8_t)PieceType::Knight | (uint8_t)PieceColor::Black },
    { { Board::Type::Bishops, Board::Color::White }, (uint8_t)PieceType::Bishop | (uint8_t)PieceColor::White },
    { { Board::Type::Bishops, Board::Color::Black }, (uint8_t)PieceType::Bishop | (uint8_t)PieceColor::Black },
    { { Board::Type::Rooks, Board::Color::White }, (uint8_t)PieceType::Rook | (uint8_t)PieceColor::White },
    { { Board::Type::Rooks, Board::Color::Black }, (uint8_t)PieceType::Rook | (uint8_t)PieceColor::Black },
    { { Board::Type::Queens, Board::Color::White }, (uint8_t)PieceType::Queen | (uint8_t)PieceColor::White },
    { { Board::Type::Queens, Board::Color::Black }, (uint8_t)PieceType::Queen | (uint8_t)PieceColor::Black },
    { { Board::Type::Kings, Board::Color::White }, (uint8_t)PieceType::King | (uint8_t)PieceColor::White },
    { { Board::Type::Kings, Board::Color::Black }, (uint8_t)PieceType::King | (uint8_t)PieceColor::Black },
    { { Board::Type::Duck, Board::Color::Both }, (uint8_t)PieceType::Duck | (uint8_t)PieceColor::Both }
};

Board::Type Board::FindBoardType(BitBoard pieceBitBoard, Board::Color color) const
{
    for (int i = 0; i < 6; ++i)
    {
        if ((pieceBitBoard & bitBoards[i][color]))
        {
            return (Board::Type)i;
        }
    }
}
