/*DEBUG*/ #include <iostream>

#include "Engine.h"
#include "StandardPositionEvaluator.h"
#include "ColoringBoardPrinter.h"

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

Evaluation Engine::Search(Position& position, unsigned depth, Evaluation alpha, Evaluation beta)
{
	return MinMaxSearch(position, depth, alpha, beta, evaluationTree.root);
}

Evaluation Engine::MinMaxSearch(Position& position, unsigned depth, Evaluation alpha, Evaluation beta, std::shared_ptr<EvaluationTree::Node> node)
{
	if (depth == 0)
	{
		return evaluator->Evaluate(position);
	}

	std::unique_ptr<std::list<Move>> moves = movesGenerator.GenerateLegalMoves(position);

	Evaluation bestEvaluation;
	Evaluation currentEvaluation;

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
				
				evaluationTree.AddNode(POSITIVE_INFINITY_EVALUATION, move, node);

				return POSITIVE_INFINITY_EVALUATION;
			}

			std::shared_ptr<EvaluationTree::Node> childNode = evaluationTree.AddNode(0, move, node);

			currentEvaluation = MinMaxSearch(session->position, depth - 1, alpha, beta, childNode);
			session->UndoMove();

			childNode->data.first = currentEvaluation;

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

				evaluationTree.AddNode(NEGATIVE_INFINITY_EVALUATION, move, node);

				return NEGATIVE_INFINITY_EVALUATION;
			}

			std::shared_ptr<EvaluationTree::Node> childNode = evaluationTree.AddNode(0, move, node);

			currentEvaluation = MinMaxSearch(session->position, depth - 1, alpha, beta, childNode);
			session->UndoMove();

			childNode->data.first = currentEvaluation;

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

	node->data.first = bestEvaluation;

	return bestEvaluation;
}

void Engine::Print()
{
	firstConsolePrinter->Handle(consolePrinterRequest);
}

void Engine::PrintBestMoves(Evaluation bestEvaluation)
{
	std::shared_ptr<EvaluationTree::Node> currentNode = evaluationTree.root;

	while (!currentNode->children.empty())
	{
		int srcX;
		int srcY;
		int targetX;
		int targetY;

		for (std::shared_ptr<EvaluationTree::Node>& child : currentNode->children)
		{
			if (child->data.first == bestEvaluation)
			{
				currentNode = child;
				break;
			}
		}

		SquareToBoardIndices(currentNode->data.second.sourceSquare, srcY, srcX);
		SquareToBoardIndices(currentNode->data.second.targetSquare, targetY, targetX);
		std::cout << MoveStringFormat(currentNode->data.second, session->position.board.pieces[srcY][srcX].PieceType(), session->position.board.pieces[targetY][targetX].PieceType() != Piece::Type::None) << std::endl << std::endl;

		session->MakeMove(currentNode->data.second);
		Print();
	}
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
	firstConsolePrinter = boardPrinter;

	// Setting what info is going to be displayed in console
	consolePrinterRequest = (ConsolePrinterHandler::Request)(
		(uint8_t)ConsolePrinterHandler::Request::Board |
		(uint8_t)ConsolePrinterHandler::Request::GameInformation);

	evaluator = new StandardPositionEvaluator();
}