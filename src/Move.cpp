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

const std::unordered_map<Move::AdditionalInfo, std::pair<Square, Square>> Move::KING_CASTLING_SQUARES
{
    { Move::AdditionalInfo::WhiteKingsideCastle, std::make_pair(Square::E1, Square::G1) },
    { Move::AdditionalInfo::WhiteQueensideCastle, std::make_pair(Square::E1, Square::C1) },
    { Move::AdditionalInfo::BlackKingsideCastle, std::make_pair(Square::E8, Square::G8) },
    { Move::AdditionalInfo::BlackQueensideCastle, std::make_pair(Square::E8, Square::C8) }
};

const std::unordered_map<Move::AdditionalInfo, std::pair<Square, Square>> Move::ROOK_CASTLING_SQUARES
{
    { Move::AdditionalInfo::WhiteKingsideCastle, std::make_pair(Square::H1, Square::F1) },
    { Move::AdditionalInfo::WhiteQueensideCastle, std::make_pair(Square::A1, Square::D1) },
    { Move::AdditionalInfo::BlackKingsideCastle, std::make_pair(Square::H8, Square::F8) },
    { Move::AdditionalInfo::BlackQueensideCastle, std::make_pair(Square::A8, Square::D8) }
};
