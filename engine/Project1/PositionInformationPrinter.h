#pragma once

#include "Position.h"
#include "ConsolePrinterHandler.h"

class PositionInformationPrinter : public ConsolePrinterHandler
{
private:

	Position* position;

public:

	PositionInformationPrinter(Position* game);
	virtual void Print() const;
	virtual void Handle(Request request) override;

private:

	void PrintCastlingRightsInformation(std::string color, uint8_t kingside, uint8_t queenside) const;
};