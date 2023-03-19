#pragma once

#include "Board.h"
#include "Piece.h"
#include "ConsolePrinterHandler.h"

class BoardPrinter : public ConsolePrinterHandler
{
protected:

	Board* board;

public:

	void Print() const;
	BoardPrinter(Board* board);
	virtual void Handle(Request request) override;

	void PrintRowNumber(int y) const;
	void PrintFilesLetters() const;
	virtual void PrintPieceSymbol(int y, int x) const = 0;

private:

	static std::string verticalLine;
	static std::string InitVerticalLine();
};