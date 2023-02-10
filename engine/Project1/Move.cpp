#include "Move.h"

uint16_t Move::captureChecker = (uint16_t)AdditionalInfo::CapturedPawn |
								(uint16_t)AdditionalInfo::CapturedKnight |
								(uint16_t)AdditionalInfo::CapturedBishop |
								(uint16_t)AdditionalInfo::CapturedRook |
								(uint16_t)AdditionalInfo::CapturedQueen |
								(uint16_t)AdditionalInfo::CapturedKing;

uint16_t Move::promotionChecker = (uint16_t)AdditionalInfo::PromotionToQueen | 
							      (uint16_t)AdditionalInfo::PromotionToRook |
							      (uint16_t)AdditionalInfo::PromotionToKnight |
							      (uint16_t)AdditionalInfo::PromotionToBishop;

uint16_t Move::castlingChecker = (uint16_t)AdditionalInfo::WhiteKingsideCastle |
								 (uint16_t)AdditionalInfo::WhiteQueensideCastle |
								 (uint16_t)AdditionalInfo::BlackKingsideCastle |
								 (uint16_t)AdditionalInfo::BlackQueensideCastle;

Move::Move() :
	sourceSquare(Square::None),
	targetSquare(Square::None),
	additionalInfo(AdditionalInfo::None) {}

Move::Move(Square source, Square target, AdditionalInfo additionalInfo) :
	sourceSquare(source),
	targetSquare(target),
	additionalInfo(additionalInfo) {}
