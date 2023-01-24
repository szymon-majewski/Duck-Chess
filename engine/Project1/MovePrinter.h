#pragma once

#include "ConsolePrinterHandler.h"
#include "FullMove.h"
#include "Piece.h"
#include "Board.h"

extern std::string MoveStringFormat(const FullMove& move, Piece::Type movingPieceType, bool take);
extern void SquareToBoardIndices(const Square& square, int& y, int& x);

class MovePrinter : public ConsolePrinterHandler
{
private:

	FullMove* move;
	Board* board;

public:

	MovePrinter(FullMove* move, Board* board);

	virtual void Print() const;
	virtual void Handle(Request request) override;
};