#include "Move.h"

Move::Move(Square source, Square target, AdditionalInfo additionalInfo) :
	sourceSquare(source),
	targetSquare(target),
	additionalInfo(additionalInfo) {}
