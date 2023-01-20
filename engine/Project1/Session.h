#pragma once

#include "Position.h"
#include "PositionMemento.h"

class Session
{
private:

	PositionMemento positionMemento;
	Position position;

public:

	Session(const Position& position);

	void MakeMove(const Move& move);
	void UndoMove();
};