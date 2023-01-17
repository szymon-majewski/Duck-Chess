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
	delete instance->gameInformationPrinter;
	delete instance->game;
}

void Engine::Print()
{
	boardPrinter->Handle(consolePrinterRequest);
}

Game* Engine::GetGame()
{
	return game;
}

void Engine::SetGame(Game* game)
{
	delete this->game;
	delete boardPrinter;
	delete gameInformationPrinter;
	delete evaluator;

	this->game = game;

	boardPrinter = new BoardPrinter(&game->board);
	gameInformationPrinter = new GameInformationPrinter(game);
	boardPrinter->SetSuccessor(gameInformationPrinter);

	// Setting what info is going to be displayed in console
	consolePrinterRequest = (ConsolePrinterHandler::Request)(
		(uint8_t)ConsolePrinterHandler::Request::Board |
		(uint8_t)ConsolePrinterHandler::Request::GameInformation);

	evaluator = new StandardPositionEvaluator(game);
}