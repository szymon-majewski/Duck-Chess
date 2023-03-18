#include <iostream>
#include "MovePrinter.h"
#include "Engine.h"

MovePrinter::MovePrinter(FullMove* move, Board* board) :
	move(move),
	board(board) {}

void MovePrinter::Print() const
{
	std::cout << MoveStringFormat(*move, *board) << std::endl << std::endl;
}

void MovePrinter::Handle(Request request)
{
	if ((Request)((uint8_t)request & (uint8_t)Request::Move) != Request::None && move->sourceSquare != Square::None)
	{
		Print();
	}

	ConsolePrinterHandler::Handle(request);
}