#pragma once

#include "Position.h"
#include "PositionEvaluator.h"
#include "BoardPrinter.h"
#include "PositionInformationPrinter.h"
#include "MovesGenerator.h"
#include "FenParser.h"
#include "Session.h"

class Engine
{
private: /*DEBUG ->*/ public:

	static Engine* instance;
	Engine();

	Session* session;
	MovesGenerator movesGenerator;
	PositionEvaluator* evaluator;
	ConsolePrinterHandler* boardPrinter;
	ConsolePrinterHandler* positionInformationPrinter;
	ConsolePrinterHandler::Request consolePrinterRequest;
	FenParser fenParser;

	unsigned searchDepth;

public:

	static Engine* GetInstance();
	~Engine();

	int32_t MinMaxSearch(Position& position, unsigned depth, int32_t alpha, int32_t beta);

	void Print();

	Session* GetSession();
	void SetSession(Session* session);
};