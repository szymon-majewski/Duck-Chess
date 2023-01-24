#include <iostream>
#include "ColoringBoardPrinter.h"
#include "StandardBoardPrinter.h"

ColoringBoardPrinter::ColoringBoardPrinter(BoardPrinter* boardPrinter) :
	boardPrinter(boardPrinter)
{
	consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
}

void ColoringBoardPrinter::Print() const
{
	for (int y = Board::HEIGHT - 1; y >= 0; --y)
	{
		((StandardBoardPrinter*)boardPrinter)->PrintRowNumber(y + 1);

		for (int x = 0; x < Board::WIDTH; ++x)
		{
			PrintPieceSymbol(y, x);
		}

		std::cout << '|' << std::endl;
	}

	((StandardBoardPrinter*)boardPrinter)->PrintFilesLetters();
}

void ColoringBoardPrinter::PrintPieceSymbol(int y, int x) const
{
	std::cout << "| ";
	switch (((StandardBoardPrinter*)boardPrinter)->board->pieces[y][x].PieceColor())
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

	std::cout << Piece::FindPieceSymbol(((StandardBoardPrinter*)boardPrinter)->board->pieces[y][x].GetBitPiece());
	SetConsoleTextAttribute(consoleHandle, CHESSBOARD_LINES_COLOR);
	std::cout << ' ';
}

void ColoringBoardPrinter::Handle(Request request)
{
	if ((Request)((uint8_t)request & (uint8_t)Request::Board) != Request::None)
	{
		Print();
	}

	ConsolePrinterHandler::Handle(request);
}