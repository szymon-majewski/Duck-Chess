#pragma once

#include <windows.h>
#include "BoardPrinter.h"

class ColoringBoardPrinter : public ConsolePrinterHandler
{
private:

	BoardPrinter* boardPrinter;
	HANDLE consoleHandle;

public:

	const int WHITE_PIECES_COLOR = 11;
	const int BLACK_PIECES_COLOR = 4;
	const int CHESSBOARD_LINES_COLOR = 15;

	ColoringBoardPrinter(BoardPrinter* boardPrinter);

	virtual void Print() const;
	virtual void Handle(Request request) override;

private:

	void PrintPieceSymbol(int y, int x) const;
};