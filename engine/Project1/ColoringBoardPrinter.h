#pragma once

#include <windows.h>
#include "BoardPrinter.h"

class ColoringBoardPrinter : public BoardPrinter
{
private:

	HANDLE consoleHandle;

	const static std::unordered_map<Piece::Type, const char*> PIECES_UNICODE_SYMBOLS;

public:

	const unsigned short WHITE_PIECES_COLOR = 0xF;
	const unsigned short BLACK_PIECES_COLOR = 0x0;
	const unsigned short LIGHT_SQUARES_COLOR = 0xD0;
	const unsigned short DARK_SQUARES_COLOR = 0x50;
	const unsigned short CHESSBOARD_LINES_COLOR = 0xF;
	const unsigned short DUCK_COLOR = 0xE;

	ColoringBoardPrinter(Board* board);

private:

	void PrintPieceSymbol(int y, int x, bool lightSquare) const override;
};