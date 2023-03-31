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

		PositionData() = default;
		PositionData(FullMove move, Square enPassantTarget, uint8_t plyClock, CastlingRights castlingRights);
	};

private:

	Stack<PositionData, MAX_GAME_LENGTH> moveHistory;

public:

	void SaveMove(FullMove move, Square enPassantTarget, uint8_t plyClock, CastlingRights castlingRights);
	PositionData RevertMove();
};