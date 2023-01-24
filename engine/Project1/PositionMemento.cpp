#include "PositionMemento.h"

void PositionMemento::SavePosition(const Position& position)
{
	positionHistory.push(position);
}

Position PositionMemento::RevertMove()
{
	Position restoredPosition = positionHistory.top();
	
	positionHistory.pop();

	return restoredPosition;
}
