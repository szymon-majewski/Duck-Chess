#pragma once

#include "Board.h"

class Game
{
private:

	Board board;

public:

	PlayerColor playerToMove;
	CastlingRights castlingRights;
	Square enPassantTarget;
	int plyClock;
	int fullMovesCount;

private:

	friend class FenParser;
	friend class Engine;
};