#pragma once

#include "Stack.h"
#include "Definitions.h"
#include "FullMove.h"

class MoveMemento
{
public:

	class PositionData
	{
	public:

		FullMove move;
		Square enPassantTarget;
		uint8_t plyClock;
		CastlingRights castlingRights;

		PositionData(FullMove move, Square enPassantTarget, uint8_t plyClock, CastlingRights castlingRights);
	};

private:

	Stack<PositionData> moveHistory;

public:

	void SaveMove(FullMove move, Square enPassantTarget, uint8_t plyClock, CastlingRights castlingRights);
	PositionData RevertMove();
};