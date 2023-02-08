#pragma once

#include <windows.h>
#include "BoardPrinter.h"

class ColoringBoardPrinter : public BoardPrinter
{
private:

	HANDLE consoleHandle;

public:

	const int WHITE_PIECES_COLOR = 11;
	const int BLACK_PIECES_COLOR = 4;
	const int CHESSBOARD_LINES_COLOR = 15;
	const int DUCK_COLOR = 14;

	ColoringBoardPrinter(Board* board);

private:

	void PrintPieceSymbol(int y, int x) const override;
};