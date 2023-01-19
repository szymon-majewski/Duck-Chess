#include "Engine.h"
#include "StandardPositionEvaluator.h"

Engine* Engine::instance = nullptr;

Engine::Engine() {}

Engine* Engine::GetInstance()
{
	if (!instance)
	{
		instance = new Engine();	
	}
	
	return instance;
}

Engine::~Engine()
{
	delete instance->boardPrinter;
	delete instance->positionInformationPrinter;
	delete instance->position;
}

void Engine::Print()
{
	boardPrinter->Handle(consolePrinterRequest);
}

Position* Engine::GetPosition()
{
	return position;
}

void Engine::SetPosition(Position* position)
{
	delete this->position;
	delete boardPrinter;
	delete positionInformationPrinter;
	delete evaluator;

	this->position = position;

	boardPrinter = new BoardPrinter(&position->board);
	positionInformationPrinter = new PositionInformationPrinter(position);
	boardPrinter->SetSuccessor(positionInformationPrinter);

	// Setting what info is going to be displayed in console
	consolePrinterRequest = (ConsolePrinterHandler::Request)(
		(uint8_t)ConsolePrinterHandler::Request::Board |
		(uint8_t)ConsolePrinterHandler::Request::GameInformation);

	evaluator = new StandardPositionEvaluator();
}