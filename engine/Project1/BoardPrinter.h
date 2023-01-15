#pragma once

#include "Board.h"
#include "Piece.h"
#include "ConsolePrinterHandler.h"

class BoardPrinter : public ConsolePrinterHandler
{
public:

	void PrintBoard(Piece board[Board::HEIGHT][Board::WIDTH]) const;

private:

	static std::string verticalLine;

	static std::string InitVerticalLine();
};