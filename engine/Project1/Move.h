#pragma once

#include "Definitions.h"

class Move
{
public:

	enum class AdditionalInfo : uint16_t
	{
		None = 0,
		PromotionToQueen = 1,
		PromotionToRook = 1 << 1,
		PromotionToKnight = 1 << 2,
		PromotionToBishop = 1 << 3,
		EnPassant = 1 << 4,
		WhiteKingsideCastle = 1 << 5,
		WhiteQueensideCastle = 1 << 6,
		BlackKingsideCastle = 1 << 7,
		BlackQueensideCastle = 1 << 8,
		PawnDoubleForward = 1 << 9,
		CapturedPawn = 1 << 10,
		CapturedKnight = 1 << 11,
		CapturedBishop = 1 << 12,
		CapturedRook = 1 << 13,
		CapturedQueen = 1 << 14,
		CapturedKing = 1 << 15
	};

	static uint16_t captureChecker;
	static uint16_t promotionChecker;
	static uint16_t castlingChecker;

	Move();
	Move(Square source, Square target, AdditionalInfo additionalInfo = AdditionalInfo::None);

	Square sourceSquare;
	Square targetSquare;
	AdditionalInfo additionalInfo;
};