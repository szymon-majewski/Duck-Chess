#include <iostream>
#include "StandardBoardPrinter.h"

std::string StandardBoardPrinter::InitVerticalLine()
{
	std::string line = "+";

	for (int i = 0; i < Board::WIDTH; ++i)
	{
		line += "---+";
	}

	return line;
}

std::string StandardBoardPrinter::verticalLine = StandardBoardPrinter::InitVerticalLine();

StandardBoardPrinter::StandardBoardPrinter(Board* board) :
	BoardPrinter(board) {}

void StandardBoardPrinter::Print() const
{
	for (int y = Board::HEIGHT - 1; y >= 0; --y)
	{
		PrintRowNumber(y + 1);

		for (int x = 0; x < Board::WIDTH; ++x)
		{
			PrintPieceSymbol(y, x);
		}

		std::cout << '|' << std::endl;
	}

	PrintFilesLetters();
}

void StandardBoardPrinter::PrintPieceSymbol(int y, int x) const
{
	std::cout << "| " << Piece::FindPieceSymbol(board->pieces[y][x].GetBitPiece()) << ' ';
}

void StandardBoardPrinter::PrintRowNumber(int y) const
{
	std::cout << "  " << verticalLine << std::endl;
	std::cout << y << ' ';
}

void StandardBoardPrinter::PrintFilesLetters() const
{
	std::cout << "  " << verticalLine << std::endl;
	std::cout << " ";

	for (int x = 0; x < Board::WIDTH; ++x)
	{
		std::cout << "   " << (char)('A' + x);
	}

	std::cout << std::endl << std::endl;
}