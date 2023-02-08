#pragma once

#include "BoardPrinter.h"

class StandardBoardPrinter : public BoardPrinter
{
public:

	StandardBoardPrinter(Board* board);

private:

	virtual void PrintPieceSymbol(int y, int x) const override;
};