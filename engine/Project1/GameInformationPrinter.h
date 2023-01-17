#pragma once

#include "Game.h"
#include "ConsolePrinterHandler.h"

class GameInformationPrinter : public ConsolePrinterHandler
{
private:

	Game* game;

public:

	GameInformationPrinter(Game* game);
	virtual void Print() const;
	virtual void Handle(Request request) override;

private:

	void PrintCastlingRightsInformation(std::string color, uint8_t kingside, uint8_t queenside) const;
};