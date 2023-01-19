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

void BoardPrinter::Print() const
{
	for (int y = Board::HEIGHT - 1; y >= 0; --y)
	{
		std::cout << "  " << verticalLine << std::endl;
		std::cout << y + 1 << ' ';

		for (int x = 0; x < Board::WIDTH; ++x)
		{
			std::cout << "| " << Piece::FindPieceSymbol(board->pieces[y][x].GetBitPiece()) << ' ';
		}

		std::cout << '|' << std::endl;
	}

	std::cout << "  " << verticalLine << std::endl;
	std::cout << " ";
	
	for (int x = 0; x < Board::WIDTH; ++x)
	{
		std::cout << "   " << (char)('A' + x);
	}

	std::cout << std::endl << std::endl;
}

void BoardPrinter::Handle(Request request)
{
	if ((Request)((uint8_t)request & (uint8_t)Request::Board) != Request::None)
	{
		Print();
	}

	ConsolePrinterHandler::Handle(request);
}
