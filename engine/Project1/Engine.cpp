#include "Engine.h"
#include "StandardPositionEvaluator.h"
#include "ColoringBoardPrinter.h"

#include <iostream>

extern std::string MoveStringFormat(const Move& move, Piece::Type movingPieceType, bool take);
extern void SquareToBoardIndices(const Square& square, int& y, int& x);

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
	delete instance->positionInformationPrinter;
}

int32_t Engine::MinMaxSearch(Position& position, unsigned depth, int32_t alpha, int32_t beta)
{
	if (depth == 0)
	{
		return evaluator->Evaluate(position);
	}

	std::unique_ptr<std::list<Move>> moves = movesGenerator.GenerateLegalMoves(position);

	int32_t bestEvaluation;
	int32_t currentEvaluation;

	// White move
	if (position.playerToMove == PlayerColor::White)
	{
		bestEvaluation = NEGATIVE_INFINITY_EVALUATION;

		for (const Move& move : *moves)
		{
			session->MakeMove(move);

			// Check if white won the game
			if (session->winnerColor != PlayerColor::None)
			{
				session->UndoMove();

				return POSITIVE_INFINITY_EVALUATION;
			}

			currentEvaluation = MinMaxSearch(session->position, depth - 1, alpha, beta);
			session->UndoMove();

			if (currentEvaluation > bestEvaluation)
			{
				bestEvaluation = currentEvaluation;
			}

			if (alpha < currentEvaluation)
			{
				alpha = currentEvaluation;
			}

			if (beta <= alpha)
			{
				break;
			}
		}
	}
	// Black move
	else
	{
		bestEvaluation = POSITIVE_INFINITY_EVALUATION;

		for (const Move& move : *moves)
		{
			session->MakeMove(move);

			// Check if black won the game
			if (session->winnerColor != PlayerColor::None)
			{
				session->UndoMove();

				return NEGATIVE_INFINITY_EVALUATION;
			}

			currentEvaluation = MinMaxSearch(session->position, depth - 1, alpha, beta);
			session->UndoMove();

			if (currentEvaluation < bestEvaluation)
			{
				bestEvaluation = currentEvaluation;
			}

			if (beta > currentEvaluation)
			{
				beta = currentEvaluation;
			}

			if (beta <= alpha)
			{
				break;
			}
		}
	}

	return bestEvaluation;
}

void Engine::Print()
{
	boardPrinter->Handle(consolePrinterRequest);
}

Session* Engine::GetSession()
{
	return session;
}

void Engine::SetSession(Session* session)
{
	delete this->session;
	delete boardPrinter;
	delete positionInformationPrinter;
	delete evaluator;

	this->session = session;

	boardPrinter = new BoardPrinter(&session->position.board);
	// Check coloring flag here
	boardPrinter = new ColoringBoardPrinter((BoardPrinter*)boardPrinter);

	positionInformationPrinter = new PositionInformationPrinter(&session->position);
	boardPrinter->SetSuccessor(positionInformationPrinter);

	// Setting what info is going to be displayed in console
	consolePrinterRequest = (ConsolePrinterHandler::Request)(
		(uint8_t)ConsolePrinterHandler::Request::Board |
		(uint8_t)ConsolePrinterHandler::Request::GameInformation);

	evaluator = new StandardPositionEvaluator();
}