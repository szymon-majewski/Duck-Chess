#pragma once

#include<stack>
#include "Position.h"

class PositionMemento
{
private:

	std::stack<Position> positionHistory;

public:

	void SavePosition(const Position& position);
	Position RevertMove();
};