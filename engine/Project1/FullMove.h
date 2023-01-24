#pragma once

#include "Move.h"

class FullMove : public Move
{
public:

	FullMove();
	FullMove(const Move& chessMove, Square sourceDuckSquare, Square targetDuckMove);

	Square sourceDuckSquare;
	Square targetDuckSquare;
};
