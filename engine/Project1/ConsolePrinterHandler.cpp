#include "ConsolePrinterHandler.h"

void ConsolePrinterHandler::SetSuccessor(ConsolePrinterHandler* successor)
{
	this->successor = successor;
}

void ConsolePrinterHandler::Handle()
{
	if (successor)
	{
		successor->Handle();
	}
}