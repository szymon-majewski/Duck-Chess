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
#include "PlayerInputManager.h"

class Engine
{
private: /*DEBUG ->*/ public:

	Session session;
	MovesGenerator movesGenerator;
	std::unique_ptr<PositionEvaluator> evaluator;
	//EvaluationTree evaluationTree;
	FenParser fenParser;
	EngineConfigurator engineConfigurator;

	//Game
	PlayerInputManager playerInputManager;
	PlayerColor playerColor;

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

	class SearchInfo
	{
	public:

		Evaluation evaluation;
		std::list<FullMove> movesPath;

		SearchInfo() :
			evaluation(0),
			movesPath(std::list<FullMove>()) {}

		SearchInfo(const Evaluation& evaluation, const FullMove& firstMove) :
			evaluation(evaluation),
			movesPath({ firstMove }) {}
	};

	Engine();
	Engine(std::string fen);

	std::unique_ptr<SearchInfo> Search();

	void Print();
	void PrintBestMoves(const std::list<FullMove>& movesPath);

	void GameLoop();

	void PrintEvaluation(Evaluation evaluation);

private:

	//Evaluation MinMaxSearch(Position& position, unsigned depth, Evaluation alpha, Evaluation beta, std::shared_ptr<EvaluationTree::Node>& node);
	std::unique_ptr<SearchInfo> MinMaxSearch(Position& position, unsigned depth, Evaluation alpha, Evaluation beta, const FullMove& prevMove);
	void OrderMoves(std::unique_ptr<std::list<FullMove>>& moves);

private:

	friend class EngineConfigurator;
};