#include <iostream>
#include "PositionInformationPrinter.h"

PositionInformationPrinter::PositionInformationPrinter(Position* position)
{
	this->position = position;
}

void PositionInformationPrinter::Print() const
{
	std::cout << (position->playerToMove == PlayerColor::White ? "White's" : "Black's") << " move" << std::endl;
	std::cout << "Move: " << position->fullMovesCount << std::endl;

	uint8_t whiteKingside = (uint8_t)position->castlingRights & (uint8_t)CastlingRights::WhiteKingside;
	uint8_t whiteQueenside = (uint8_t)position->castlingRights & (uint8_t)CastlingRights::WhiteQueenside;
	uint8_t blackKingside = (uint8_t)position->castlingRights & (uint8_t)CastlingRights::BlackKingside;
	uint8_t blackQueenside = (uint8_t)position->castlingRights & (uint8_t)CastlingRights::BlackQueenside;

	std::cout << "Castling rights:" << std::endl;
	PrintCastlingRightsInformation("White", whiteKingside, whiteQueenside);
	PrintCastlingRightsInformation("Black", blackKingside, blackQueenside);

	std::cout << "50 move rule status: " << position->plyClock / 2 << std::endl << std::endl;
}

void PositionInformationPrinter::Handle(Request request)
{
	if ((Request)((uint8_t)request & (uint8_t)Request::PositionInformation) != Request::None)
	{
		Print();
	}

	ConsolePrinterHandler::Handle(request);
}

void PositionInformationPrinter::PrintCastlingRightsInformation(std::string color, uint8_t kingside, uint8_t queenside) const
{
	std::cout << "  " << color << ": ";

	if (!(kingside || queenside))
	{
		std::cout << '-' << std::endl;

		return;
	}

	if (kingside) { std::cout << "Kingside"; }
	if (queenside)
	{
		if (kingside) { std::cout << ", "; }

		std::cout << "Queenside";
	}
	std::cout << std::endl;
}
