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

	// Optimasation
	Evaluation materialDisparity;

	// Returns winner color or PlayerColor::None if game isn't resolved yet
	PlayerColor Update(const FullMove& move);
	void UndoMove(const std::tuple<FullMove, Square, uint8_t, CastlingRights>& revertedMove);

	Evaluation CountMaterial();

private:

	void UpdateCastling(const Move::AdditionalInfo& castlingInfo, const int& targetY, const int& targetX, const uint8_t& playerColor);

	friend class FenParser;
	friend class Engine;
	friend class StandardPositionEvaluator;
};