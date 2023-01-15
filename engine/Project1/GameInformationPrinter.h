#pragma once

#include "Game.h"
#include "ConsolePrinterHandler.h"

class GameInformationPrinter : public ConsolePrinterHandler
{
public:

	void PrintGameInformation(const Game& game) const;

private:

	void PrintCastlingRightsInformation(std::string color, uint8_t kingside, uint8_t queenside) const;
};