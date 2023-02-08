#include <iostream>
#include "ColoringBoardPrinter.h"
#include "StandardBoardPrinter.h"

ColoringBoardPrinter::ColoringBoardPrinter(Board* board) :
	BoardPrinter(board)
{
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

	std::cout << Piece::FindPieceSymbol(board->pieces[y][x].GetBitPiece());
	SetConsoleTextAttribute(consoleHandle, CHESSBOARD_LINES_COLOR);
	std::cout << ' ';
}