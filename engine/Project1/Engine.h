#pragma once

#include "Position.h"
#include "PositionEvaluator.h"
#include "BoardPrinter.h"
#include "PositionInformationPrinter.h"
#include "MovesGenerator.h"
#include "FenParser.h"

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
	FenParser fenParser;

	unsigned searchDepth;

public:

	static Engine* GetInstance();
	~Engine();

	double MinMaxSearch(Position& position, unsigned depth, PlayerColor maximazingPlayer, int32_t alpha, int32_t beta);

	void Print();

	Position* GetPosition();
	void SetPosition(Position* game);
};