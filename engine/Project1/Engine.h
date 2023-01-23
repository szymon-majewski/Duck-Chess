#pragma once

#include "Position.h"
#include "PositionEvaluator.h"
#include "BoardPrinter.h"
#include "PositionInformationPrinter.h"
#include "MovesGenerator.h"
#include "FenParser.h"
#include "Session.h"
#include "EvaulationTree.h"

extern std::string MoveStringFormat(const Move& move, Piece::Type movingPieceType, bool take);
extern void SquareToBoardIndices(const Square& square, int& y, int& x);

class Engine
{
private: /*DEBUG ->*/ public:

	static Engine* instance;
	Engine();

	Session* session;
	MovesGenerator movesGenerator;
	PositionEvaluator* evaluator;
	EvaluationTree evaluationTree;
	ConsolePrinterHandler* boardPrinter;
	ConsolePrinterHandler* positionInformationPrinter;
	ConsolePrinterHandler* firstConsolePrinter;
	ConsolePrinterHandler::Request consolePrinterRequest;
	FenParser fenParser;

	unsigned searchDepth;

public:

	static Engine* GetInstance();
	~Engine();

	Evaluation Search(Position& position, unsigned depth, Evaluation alpha, Evaluation beta);

private:

	Evaluation MinMaxSearch(Position& position, unsigned depth, Evaluation alpha, Evaluation beta, std::shared_ptr<EvaluationTree::Node> node);

public:

	void Print();
	void PrintBestMoves(Evaluation bestEvaluation);

	Session* GetSession();
	void SetSession(Session* session);
};