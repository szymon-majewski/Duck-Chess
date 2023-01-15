#pragma once

#include "Board.h"

class Game
{
private:
	Board board;
	PlayerColor playerToMove;
	CastlingRights castlingRights;
	Square enPassantTarget;
	int plyClock;
	int fullMovesCount;

	friend class FenParser;
	friend class Engine;
};