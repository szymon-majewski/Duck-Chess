#include <iostream>
#include "ColoringBoardPrinter.h"
#include "StandardBoardPrinter.h"

const std::unordered_map<BitPiece, const char*> ColoringBoardPrinter::PIECES_UNICODE_SYMBOLS =
{
	{ (uint8_t)Piece::Type::Pawn | (uint8_t)Piece::Color::Black, "\xe2\x99\x9f" },
	{ (uint8_t)Piece::Type::Pawn | (uint8_t)Piece::Color::White, "\xe2\x99\x99" },
	{ (uint8_t)Piece::Type::Knight | (uint8_t)Piece::Color::Black, "\xe2\x99\x9e" },
	{ (uint8_t)Piece::Type::Knight | (uint8_t)Piece::Color::White, "\xe2\x99\x98" },
	{ (uint8_t)Piece::Type::Bishop | (uint8_t)Piece::Color::Black, "\xe2\x99\x9d" },
	{ (uint8_t)Piece::Type::Bishop | (uint8_t)Piece::Color::White, "\xe2\x99\x97" },
	{ (uint8_t)Piece::Type::Rook | (uint8_t)Piece::Color::Black, "\xe2\x99\x9c" },
	{ (uint8_t)Piece::Type::Rook | (uint8_t)Piece::Color::White, "\xe2\x99\x96" },
	{ (uint8_t)Piece::Type::Queen | (uint8_t)Piece::Color::Black, "\xe2\x99\x9b" },
	{ (uint8_t)Piece::Type::Queen | (uint8_t)Piece::Color::White, "\xe2\x99\x95" },
	{ (uint8_t)Piece::Type::King | (uint8_t)Piece::Color::Black, "\xe2\x99\x9a"},
	{ (uint8_t)Piece::Type::King | (uint8_t)Piece::Color::White, "\xe2\x99\x94" },
	{ (uint8_t)Piece::Type::Duck | (uint8_t)Piece::Color::Both, "@" },
	{ (uint8_t)Piece::Type::None, " " }
};

ColoringBoardPrinter::ColoringBoardPrinter(Board* board) :
	BoardPrinter(board)
{
	SetConsoleOutputCP(65001);
	consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
}

void ColoringBoardPrinter::PrintPieceSymbol(int y, int x) const
{
	std::cout << "| ";
	switch (board->pieces[y][x].PieceColor())
	{
	case Piece::Color::Both:
	{
		SetConsoleTextAttribute(consoleHandle, DUCK_COLOR);
		break;
	}
	case Piece::Color::White:
	{
		SetConsoleTextAttribute(consoleHandle, WHITE_PIECES_COLOR);
		break;
	}
	case Piece::Color::Black:
	{
		SetConsoleTextAttribute(consoleHandle, BLACK_PIECES_COLOR);
		break;
	}
	}

	std::cout << PIECES_UNICODE_SYMBOLS.at(board->pieces[y][x].GetBitPiece());
	SetConsoleTextAttribute(consoleHandle, CHESSBOARD_LINES_COLOR);
	std::cout << ' ';
}