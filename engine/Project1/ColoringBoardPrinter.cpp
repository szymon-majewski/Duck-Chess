#include <iostream>
#include "ColoringBoardPrinter.h"
#include "StandardBoardPrinter.h"

const std::unordered_map<Piece::Type, const char*> ColoringBoardPrinter::PIECES_UNICODE_SYMBOLS =
{
	{ Piece::Type::Pawn, "\xe2\x99\x9f" },
	{ Piece::Type::Knight, "\xe2\x99\x9e" },
	{ Piece::Type::Bishop, "\xe2\x99\x9d" },
	{ Piece::Type::Rook, "\xe2\x99\x9c" },
	{ Piece::Type::Queen, "\xe2\x99\x9b" },
	{ Piece::Type::King, "\xe2\x99\x9a"},
	{ Piece::Type::Duck, "@" },
	{ Piece::Type::None, " " }
};

ColoringBoardPrinter::ColoringBoardPrinter(Board* board) :
	BoardPrinter(board)
{
	SetConsoleOutputCP(65001);
	consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
}

void ColoringBoardPrinter::PrintPieceSymbol(int y, int x, bool lightSquare) const
{
	std::cout << "|";

	switch (board->pieces[y][x].PieceColor())
	{
		case Piece::Color::Both:
		{
			SetConsoleTextAttribute(consoleHandle, DUCK_COLOR | (lightSquare ? LIGHT_SQUARES_COLOR : DARK_SQUARES_COLOR));
			break;
		}
		case Piece::Color::White:
		{
			SetConsoleTextAttribute(consoleHandle, WHITE_PIECES_COLOR | (lightSquare ? LIGHT_SQUARES_COLOR : DARK_SQUARES_COLOR));
			break;
		}
		case Piece::Color::Black:
		{
			SetConsoleTextAttribute(consoleHandle, BLACK_PIECES_COLOR | (lightSquare ? LIGHT_SQUARES_COLOR : DARK_SQUARES_COLOR));
			break;
		}
		default:
		{
			SetConsoleTextAttribute(consoleHandle, lightSquare ? LIGHT_SQUARES_COLOR : DARK_SQUARES_COLOR);
			break;
		}
	}

	std::cout << ' ' << PIECES_UNICODE_SYMBOLS.at(board->pieces[y][x].PieceType()) << ' ';

	SetConsoleTextAttribute(consoleHandle, CHESSBOARD_LINES_COLOR);
}