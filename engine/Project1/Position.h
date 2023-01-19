#pragma once

#include "Board.h"

class Position
{
private:

	

public:

	Board board;
	PlayerColor playerToMove;
	CastlingRights castlingRights;
	Square enPassantTarget;
	int plyClock;
	int fullMovesCount;

private:

	friend class FenParser;
	friend class Engine;
	friend class StandardPositionEvaluator;
};