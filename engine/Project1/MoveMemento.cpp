#include "MoveMemento.h"

void MoveMemento::SaveMove(const FullMove& move, Square enPassantTarget, uint8_t plyClock, CastlingRights castlingRights)
{
	moveHistory.push(std::tuple(move, enPassantTarget, plyClock, castlingRights));
}

std::tuple<FullMove, Square, uint8_t, CastlingRights> MoveMemento::RevertMove()
{
	std::tuple<FullMove, Square, uint8_t, CastlingRights> restoredMove = moveHistory.top();

	moveHistory.pop();

	return restoredMove;
}
