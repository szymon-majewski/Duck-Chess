#include "Engine.h"
#include "StandardPositionEvaluator.h"

Engine* Engine::instance = nullptr;

Engine::Engine()
{
	boardPrinter = new BoardPrinter();
	evaluator = new StandardPositionEvaluator();
}

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
	delete instance->game;
}

void Engine::Print()
{
	boardPrinter->PrintBoard(game->board.pieces);
}

Game* Engine::GetGame()
{
	return game;
}

void Engine::SetGame(Game* game)
{
	this->game = game;
}