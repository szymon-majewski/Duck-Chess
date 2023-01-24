#include <iostream>
#include "MovePrinter.h"
#include "Engine.h"

MovePrinter::MovePrinter(FullMove* move, Board* board) :
	move(move),
	board(board) {}

void MovePrinter::Print() const
{
	int srcX;
	int srcY;
	int targetX;
	int targetY;

	SquareToBoardIndices(move->sourceSquare, srcY, srcX);
	SquareToBoardIndices(move->targetSquare, targetY, targetX);
	std::cout << MoveStringFormat(*move, board->pieces[srcY][srcX].PieceType(), board->pieces[targetY][targetX].PieceType() != Piece::Type::None) << std::endl << std::endl;
}

void MovePrinter::Handle(Request request)
{
	if ((Request)((uint8_t)request & (uint8_t)Request::Move) != Request::None && move->sourceSquare != Square::None)
	{
		Print();
	}

	ConsolePrinterHandler::Handle(request);
}