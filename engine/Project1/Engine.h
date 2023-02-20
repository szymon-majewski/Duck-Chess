#pragma once

#include <memory>

#include "Position.h"
#include "PositionEvaluator.h"
#include "BoardPrinter.h"
#include "PositionInformationPrinter.h"
#include "MovesGenerator.h"
#include "FenParser.h"
#include "Session.h"
#include "EvaulationTree.h"
#include "EngineConfigurator.h"

class Engine
{
private: /*DEBUG ->*/ public:

	Session session;
	MovesGenerator movesGenerator;
	std::unique_ptr<PositionEvaluator> evaluator;
	EvaluationTree evaluationTree;
	FenParser fenParser;
	EngineConfigurator engineConfigurator;

	unsigned searchDepth;
	const unsigned DEFAULT_SEARCH_DEPTH = 3;

	std::shared_ptr<ConsolePrinterHandler> movePrinter;
	FullMove movePrinterMove;
	Board movePrinterBoard;
	std::shared_ptr<ConsolePrinterHandler> boardPrinter;
	std::shared_ptr<ConsolePrinterHandler> positionInformationPrinter;
	std::shared_ptr<ConsolePrinterHandler> firstConsolePrinter;
	ConsolePrinterHandler::Request consolePrinterRequest;

public:

	Engine();
	Engine(std::string fen);

	Evaluation Search();

private:

	Evaluation MinMaxSearch(Position& position, unsigned depth, Evaluation alpha, Evaluation beta, std::shared_ptr<EvaluationTree::Node> node);
	void OrderMoves(std::unique_ptr<std::list<FullMove>>& moves);

public:

	void Print();
	void PrintBestMoves(Evaluation bestEvaluation);

private:

	friend class EngineConfigurator;
};