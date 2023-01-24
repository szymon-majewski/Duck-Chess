#include "FullMove.h"

FullMove::FullMove() {}

FullMove::FullMove(const Move& move, Square sourceDuckSquare,Square targetDuckMove) :
	Move(move),
	sourceDuckSquare(sourceDuckSquare),
	targetDuckSquare(targetDuckMove) {}
