#include <iostream>
#include "BoardPrinter.h"

std::string BoardPrinter::InitVerticalLine()
{
	std::string line = "+";

	for (int i = 0; i < Board::WIDTH; ++i)
	{
		line += "---+";
	}

	return line;
}

std::string BoardPrinter::verticalLine = BoardPrinter::InitVerticalLine();

BoardPrinter::BoardPrinter(Board* board)
{
	this->board = board;
}

void BoardPrinter::Handle(Request request)
{
	if ((Request)((uint8_t)request & (uint8_t)Request::Board) != Request::None)
	{
		Print();
	}

	ConsolePrinterHandler::Handle(request);
}

void BoardPrinter::Print() const
{
	bool lightSquare = true;

	for (int y = Board::HEIGHT - 1; y >= 0; --y)
	{
		PrintRowNumber(y + 1);

		for (int x = 0; x < Board::WIDTH; ++x)
		{
			PrintPieceSymbol(y, x, lightSquare);
			lightSquare = !lightSquare;
		}

		std::cout << '|' << std::endl;
		lightSquare = !lightSquare;
	}

	PrintFilesLetters();
}

void BoardPrinter::PrintRowNumber(int y) const
{
	std::cout << "  " << verticalLine << std::endl;
	std::cout << y << ' ';
}

void BoardPrinter::PrintFilesLetters() const
{
	std::cout << "  " << verticalLine << std::endl;
	std::cout << " ";

	for (int x = 0; x < Board::WIDTH; ++x)
	{
		std::cout << "   " << (char)('A' + x);
	}

	std::cout << std::endl << std::endl;
}