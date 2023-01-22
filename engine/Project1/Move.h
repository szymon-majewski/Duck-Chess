#pragma once

#include "Definitions.h"

class Move
{
public:

	enum class AdditionalInfo : uint8_t
	{
		None,
		PromotionToQueen,
		PromotionToRook,
		PromotionToKnight,
		PromotionToBishop,
		EnPassant,
		WhiteKingsideCastle,
		WhiteQueensideCastle,
		BlackKingsideCastle,
		BlackQueensideCastle,
		PawnDoubleForward
	};

	Move(Square source, Square target, AdditionalInfo additionalInfo = AdditionalInfo::None);

	Square sourceSquare;
	Square targetSquare;
	AdditionalInfo additionalInfo;
};