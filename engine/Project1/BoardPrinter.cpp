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

void BoardPrinter::PrintBoard(Piece board[Board::HEIGHT][Board::WIDTH]) const
{
	for (int y = 0; y < Board::HEIGHT; ++y)
	{
		std::cout << "  " << verticalLine << std::endl;
		std::cout << (char)('H' - y) << ' ';

		for (int x = 0; x < Board::WIDTH; ++x)
		{
			std::cout << "| " << Piece::FindPieceSymbol(board[y][x].GetPiece()) << ' ';
		}

		std::cout << '|' << std::endl;
	}

	std::cout << "  " << verticalLine << std::endl;
	std::cout << " ";
	
	for (int x = 1; x <= Board::WIDTH; ++x)
	{
		std::cout << "   " << x;
	}

	std::cout << std::endl << std::endl;
}
