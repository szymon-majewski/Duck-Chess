#pragma once

#include "Game.h"
#include "PositionEvaluator.h"
#include "BoardPrinter.h"
#include "GameInformationPrinter.h"
#include "MovesGenerator.h"

class Engine
{
private:

	static Engine* instance;
	Engine();

/*DEBUG*/ public:	Game* game; private:
/*DEBUG*/ public: MovesGenerator movesGenerator; private:
	PositionEvaluator* evaluator;
	BoardPrinter* boardPrinter;
	GameInformationPrinter* gameInformationPrinter;
	ConsolePrinterHandler::Request consolePrinterRequest;

public:

	static Engine* GetInstance();
	~Engine();

	void Print();

	Game* GetGame();
	void SetGame(Game* game);
};