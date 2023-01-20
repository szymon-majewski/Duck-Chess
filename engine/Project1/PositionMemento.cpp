#include "PositionMemento.h"

void PositionMemento::SavePosition(const Position& position)
{
	movesHistory.push(position);
}

Position PositionMemento::RevertMove()
{
	Position restoredPosition = movesHistory.top();
	
	movesHistory.pop();

	return restoredPosition;
}
