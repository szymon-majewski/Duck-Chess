#pragma once

#include <windows.h>
#include "BoardPrinter.h"

class ColoringBoardPrinter : public BoardPrinter
{
private:

	HANDLE consoleHandle;

	const static std::unordered_map<BitPiece, const char*> PIECES_UNICODE_SYMBOLS;

public:

	const unsigned short WHITE_PIECES_COLOR = 11;
	const unsigned short BLACK_PIECES_COLOR = 4;
	const unsigned short CHESSBOARD_LINES_COLOR = 15;
	const unsigned short DUCK_COLOR = 14;

	ColoringBoardPrinter(Board* board);

private:

	void PrintPieceSymbol(int y, int x) const override;
};