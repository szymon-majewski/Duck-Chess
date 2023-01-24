#include "Move.h"

Move::Move() :
	sourceSquare(Square::None),
	targetSquare(Square::None),
	additionalInfo(AdditionalInfo::None) {}

Move::Move(Square source, Square target, AdditionalInfo additionalInfo) :
	sourceSquare(source),
	targetSquare(target),
	additionalInfo(additionalInfo) {}
