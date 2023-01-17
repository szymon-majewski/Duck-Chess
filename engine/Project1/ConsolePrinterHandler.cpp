#include "ConsolePrinterHandler.h"

void ConsolePrinterHandler::SetSuccessor(ConsolePrinterHandler* successor)
{
	this->successor = successor;
}

void ConsolePrinterHandler::Handle(Request request)
{
	if (successor)
	{
		successor->Handle(request);
	}
}