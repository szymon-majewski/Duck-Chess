#pragma once

#include "Position.h"
#include "MoveMemento.h"

class Session
{
private:
	/*DEBUG*/ public:

	MoveMemento moveMemento;
	Position position;

public:

	PlayerColor winnerColor;

	Session(const Position& position);

	void MakeMove(const FullMove& move);
	void UndoMove();
};