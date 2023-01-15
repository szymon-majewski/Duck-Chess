#pragma once

#include "Game.h"
#include "PositionEvaluator.h"
#include "BoardPrinter.h"

class Engine
{
private:
	static Engine* instance;
	Engine();

	Game* game;
	PositionEvaluator* evaluator;
	BoardPrinter* boardPrinter;

public:
	static Engine* GetInstance();
	~Engine();

	void Print();

	Game* GetGame();
	void SetGame(Game* game);
};