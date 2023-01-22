#pragma once

#include "Position.h"
#include "PositionMemento.h"

class Session
{
private:
	/*DEBUG*/ public:

	PositionMemento positionMemento;
	Position position;

public:

	PlayerColor winnerColor;

	Session(const Position& position);

	void MakeMove(const Move& move);
	void UndoMove();

};