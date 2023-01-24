#pragma once

#include "Board.h"
#include "FullMove.h"

class Position
{

public:

	Board board;
	PlayerColor playerToMove;
	CastlingRights castlingRights;
	Square enPassantTarget;
	int plyClock;
	int fullMovesCount;

	// Returns winner color or PlayerColor::None if game isn'r resolved yet
	PlayerColor Update(const FullMove& move);

private:

	friend class FenParser;
	friend class Engine;
	friend class StandardPositionEvaluator;
};