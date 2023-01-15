#include <iostream>
#include "GameInformationPrinter.h"

void GameInformationPrinter::PrintGameInformation(const Game& game) const
{
	std::cout << (game.playerToMove == PlayerColor::White ? "White's" : "Black's") << " move" << std::endl;
	std::cout << "Move: " << game.fullMovesCount << std::endl;

	uint8_t whiteKingside = (uint8_t)game.castlingRights & (uint8_t)CastlingRights::WhiteKingside;
	uint8_t whiteQueenside = (uint8_t)game.castlingRights & (uint8_t)CastlingRights::WhiteQueenside;
	uint8_t blackKingside = (uint8_t)game.castlingRights & (uint8_t)CastlingRights::BlackKingside;
	uint8_t blackQueenside = (uint8_t)game.castlingRights & (uint8_t)CastlingRights::BlackQueenside;

	PrintCastlingRightsInformation("White", whiteKingside, whiteQueenside);
	PrintCastlingRightsInformation("Black", blackKingside, blackQueenside);

	std::cout << "50 move rule status: " << game.plyClock / 2;
}

void GameInformationPrinter::PrintCastlingRightsInformation(std::string color, uint8_t kingside, uint8_t queenside) const
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
