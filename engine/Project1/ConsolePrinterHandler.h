#pragma once

#include "Definitions.h"

class ConsolePrinterHandler
{
private:

	ConsolePrinterHandler* successor;

public:

	enum class Request : uint8_t
	{
		None = 0,
		Move = 1,
		Board = 1 << 1,
		PositionInformation = 1 << 2,
		Evaluation = 1 << 3
	};

	void SetSuccessor(ConsolePrinterHandler* successor);
	virtual void Handle(Request request);

	virtual void Print() const = 0;
};