#include <iostream>
#include "ColoringBoardPrinter.h"

ColoringBoardPrinter::ColoringBoardPrinter(BoardPrinter* boardPrinter) :
	boardPrinter(boardPrinter)
{
	consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
}

void ColoringBoardPrinter::Print() const
{
	for (int y = Board::HEIGHT - 1; y >= 0; --y)
	{
		boardPrinter->PrintRowNumber(y + 1);

		for (int x = 0; x < Board::WIDTH; ++x)
		{
			PrintPieceSymbol(y, x);
		}

		std::cout << '|' << std::endl;
	}

	boardPrinter->PrintFilesLetters();
}

void ColoringBoardPrinter::PrintPieceSymbol(int y, int x) const
{
	std::cout << "| ";
	SetConsoleTextAttribute(consoleHandle, boardPrinter->board->pieces[y][x].PieceColor() == Piece::Color::White ? WHITE_PIECES_COLOR : BLACK_PIECES_COLOR);
	std::cout << Piece::FindPieceSymbol(boardPrinter->board->pieces[y][x].GetBitPiece());
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