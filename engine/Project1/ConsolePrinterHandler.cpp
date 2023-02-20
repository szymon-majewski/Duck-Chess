#include "ConsolePrinterHandler.h"

void ConsolePrinterHandler::SetSuccessor(const std::shared_ptr<ConsolePrinterHandler>& successor)
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