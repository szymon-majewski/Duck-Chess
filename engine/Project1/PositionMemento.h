#pragma once

#include<stack>
#include "Position.h"

class PositionMemento
{
private:

	std::stack<Position> movesHistory;

public:

	void SavePosition(const Position& position);
	Position RevertMove();
};