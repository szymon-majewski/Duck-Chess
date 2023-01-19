#pragma once

#include "Position.h"
#include "PositionEvaluator.h"
#include "BoardPrinter.h"
#include "PositionInformationPrinter.h"
#include "MovesGenerator.h"

class Engine
{
private: /*DEBUG ->*/ public:

	static Engine* instance;
	Engine();

	Position* position;
	MovesGenerator movesGenerator;
	PositionEvaluator* evaluator;
	BoardPrinter* boardPrinter;
	PositionInformationPrinter* positionInformationPrinter;
	ConsolePrinterHandler::Request consolePrinterRequest;

public:

	static Engine* GetInstance();
	~Engine();

	void Print();

	Position* GetPosition();
	void SetPosition(Position* game);
};