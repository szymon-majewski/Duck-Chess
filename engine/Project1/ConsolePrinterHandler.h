#pragma once

#include <memory>

#include "Definitions.h"

class ConsolePrinterHandler
{
private:

	std::shared_ptr<ConsolePrinterHandler> successor;

public:

	enum class Request : uint8_t
	{
		None = 0,
		Move = 1,
		Board = 1 << 1,
		PositionInformation = 1 << 2,
		Evaluation = 1 << 3
	};

	void SetSuccessor(const std::shared_ptr<ConsolePrinterHandler>& successor);
	virtual void Handle(Request request);

	virtual void Print() const = 0;
};