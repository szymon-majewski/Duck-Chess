#pragma once

#include<stack>
#include <tuple>
#include "Position.h"

class MoveMemento
{
private:

	std::stack<std::tuple<FullMove, Square, uint8_t, CastlingRights>> moveHistory;

public:

	void SaveMove(const FullMove& move, Square enPassantTarget, uint8_t plyClock, CastlingRights castlingRights);
	std::tuple<FullMove, Square, uint8_t, CastlingRights> RevertMove();
};