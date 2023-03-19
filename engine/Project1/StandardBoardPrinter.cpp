#include <iostream>
#include "StandardBoardPrinter.h"

StandardBoardPrinter::StandardBoardPrinter(Board* board) :
	BoardPrinter(board) {}

void StandardBoardPrinter::PrintPieceSymbol(int y, int x) const
{
	std::cout << "| " << Piece::FindPieceSymbol(board->pieces[y][x].GetBitPiece()) << ' ';
}