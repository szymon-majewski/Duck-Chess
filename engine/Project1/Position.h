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
	uint8_t plyClock;
	uint16_t fullMovesCount;

	// Returns winner color or PlayerColor::None if game isn't resolved yet
	PlayerColor Update(const FullMove& move);
	void UndoMove(std::tuple<FullMove, Square, uint8_t> revertedMove);

private:

	friend class FenParser;
	friend class Engine;
	friend class StandardPositionEvaluator;
};