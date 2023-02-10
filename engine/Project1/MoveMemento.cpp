#include "MoveMemento.h"

void MoveMemento::SaveMove(const FullMove& move, Square enPassantTarget, uint8_t plyClock)
{
	moveHistory.push(std::tuple(move, enPassantTarget, plyClock));
}

std::tuple<FullMove, Square, uint8_t> MoveMemento::RevertMove()
{
	std::tuple<FullMove, Square, uint8_t> restoredMove = moveHistory.top();

	moveHistory.pop();

	return restoredMove;
}
