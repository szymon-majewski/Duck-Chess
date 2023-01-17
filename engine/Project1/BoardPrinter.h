#pragma once

#include "Board.h"
#include "Piece.h"
#include "ConsolePrinterHandler.h"

class BoardPrinter : public ConsolePrinterHandler
{
private:

	Board* board;

public:

	BoardPrinter(Board* board);
	virtual void Print() const;
	virtual void Handle(Request request) override;

private:

	static std::string verticalLine;

	static std::string InitVerticalLine();
};