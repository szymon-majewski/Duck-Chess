#pragma once

#include "Board.h"
#include "Move.h"

class Position
{

public:

	Board board;
	PlayerColor playerToMove;
	CastlingRights castlingRights;
	Square enPassantTarget;
	int plyClock;
	int fullMovesCount;

	void Udpdate(const Move& move);

private:

	static void SquareToBoardIndices(const Square& square, int& y, int& x);

	friend class FenParser;
	friend class Engine;
	friend class StandardPositionEvaluator;
};