#pragma once

#include "BoardPrinter.h"

class StandardBoardPrinter : public BoardPrinter
{
public:

	StandardBoardPrinter(Board* board);

	virtual void Print() const;

private:

	static std::string verticalLine;
	static std::string InitVerticalLine();

	void PrintRowNumber(int y) const;
	void PrintPieceSymbol(int y, int x) const;
	void PrintFilesLetters() const;

	friend class ColoringBoardPrinter;
};