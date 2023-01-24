#pragma once

#include "Board.h"
#include "Piece.h"
#include "ConsolePrinterHandler.h"

class BoardPrinter : public ConsolePrinterHandler
{
protected:

	Board* board;

public:

	BoardPrinter(Board* board);
	virtual void Handle(Request request) override;
};