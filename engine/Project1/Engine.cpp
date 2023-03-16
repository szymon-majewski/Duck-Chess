#include "Engine.h"
#include "StandardPositionEvaluator.h"
#include "MaterialEvaluator.h"
#include "ColoringBoardPrinter.h"
#include "StandardBoardPrinter.h"
#include "MovePrinter.h"

Engine::Engine() :
	Engine(STARTING_POSITION_FEN) {}

Engine::Engine(std::string fen) :
	engineConfigurator(EngineConfigurator(this)),
	session(Position())
{
	searchDepth = DEFAULT_SEARCH_DEPTH;
	fenParser.ParseFen(fen, session.position);
	session.position.materialDisparity = session.position.CountMaterial();

	evaluator = std::make_unique<MaterialEvaluator>();

	// Check coloring flag here
	//boardPrinter = new StandardBoardPrinter(&session.position.board);
	boardPrinter = std::make_shared<ColoringBoardPrinter>(&session.position.board);

	positionInformationPrinter = std::make_shared<PositionInformationPrinter>(&session.position);
	movePrinter = std::make_shared<MovePrinter>(&movePrinterMove, &movePrinterBoard);

	// Setting printers order
	movePrinter->SetSuccessor(boardPrinter);
	boardPrinter->SetSuccessor(positionInformationPrinter);
	firstConsolePrinter = movePrinter;

	// Setting what info is going to be displayed in console
	consolePrinterRequest = (ConsolePrinterHandler::Request)(
		(uint8_t)ConsolePrinterHandler::Request::Board |
		(uint8_t)ConsolePrinterHandler::Request::PositionInformation |
		(uint8_t)ConsolePrinterHandler::Request::Move);
}

std::unique_ptr<Engine::SearchInfo> Engine::Search()
{
	std::unique_ptr<SearchInfo> returned = MinMaxSearch(session.position, searchDepth, NEGATIVE_INFINITY_EVALUATION, POSITIVE_INFINITY_EVALUATION, FullMove());

	returned->movesPath.pop_front();

	return returned;
}

//Evaluation Engine::MinMaxSearch(Position& position, unsigned depth, Evaluation alpha, Evaluation beta, std::shared_ptr<EvaluationTree::Node>& node)
std::unique_ptr<Engine::SearchInfo> Engine::MinMaxSearch(Position& position, unsigned depth, Evaluation alpha, Evaluation beta, const FullMove& prevMove)
{
	if (depth == 0)
	{
		return std::make_unique<SearchInfo>(evaluator->Evaluate(position), prevMove);
	}

	std::unique_ptr<std::list<FullMove>> moves = movesGenerator.GenerateLegalMoves(position);

	std::unique_ptr<SearchInfo> bestSearchInfo = std::make_unique<SearchInfo>();
	std::unique_ptr<SearchInfo> currentSearchInfo;

	// White move
	if (position.playerToMove == PlayerColor::White)
	{
		bestSearchInfo->evaluation = OVER_NEGATIVE_INFINITY_EVALUATION;

		for (const FullMove& move : *moves)
		{
			session.MakeMove(move);

			// Check if white won the game
			if (session.winnerColor != PlayerColor::None)
			{
				session.UndoMove();

				bestSearchInfo = std::make_unique<SearchInfo>(POSITIVE_INFINITY_EVALUATION, move);
				break;
			}

			currentSearchInfo = MinMaxSearch(session.position, depth - 1, alpha, beta, move);
			session.UndoMove();

			if (currentSearchInfo->evaluation > bestSearchInfo->evaluation)
			{
				//bestSearchInfo = std::move(currentSearchInfo);
				bestSearchInfo->evaluation = currentSearchInfo->evaluation;
				bestSearchInfo->movesPath = currentSearchInfo->movesPath;
			}

			if (alpha < currentSearchInfo->evaluation)
			{
				alpha = currentSearchInfo->evaluation;
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
		bestSearchInfo->evaluation = OVER_POSITIVE_INFINITY_EVALUATION;

		for (const FullMove& move : *moves)
		{
			session.MakeMove(move);

			// Check if black won the game
			if (session.winnerColor != PlayerColor::None)
			{
				session.UndoMove();

				bestSearchInfo = std::make_unique<SearchInfo>(NEGATIVE_INFINITY_EVALUATION, move);
				break;
			}

			currentSearchInfo = MinMaxSearch(session.position, depth - 1, alpha, beta, move);
			session.UndoMove();

			if (currentSearchInfo->evaluation < bestSearchInfo->evaluation)
			{
				//bestSearchInfo = std::move(currentSearchInfo);
				bestSearchInfo->evaluation = currentSearchInfo->evaluation;
				bestSearchInfo->movesPath = currentSearchInfo->movesPath;
			}

			if (beta > currentSearchInfo->evaluation)
			{
				beta = currentSearchInfo->evaluation;
			}

			if (beta <= alpha)
			{
				break;
			}
		}
	}

	bestSearchInfo->movesPath.emplace_front(prevMove);

	return bestSearchInfo;
}

void Engine::OrderMoves(std::unique_ptr<std::list<FullMove>>& moves)
{

}

void Engine::Print()
{
	firstConsolePrinter->Handle(consolePrinterRequest);
}

void Engine::PrintBestMoves(const std::list<FullMove>& movesPath)
{
	for (const FullMove& move : movesPath)
	{
		movePrinterMove = move;
		movePrinterBoard = session.position.board;

		session.MakeMove(move);

		Print();
	}
}

//void Engine::PrintBestMoves(Evaluation bestEvaluation)
//{
//	std::shared_ptr<EvaluationTree::Node> currentNode = evaluationTree.root;
//
//	while (!currentNode->children.empty())
//	{
//		for (std::shared_ptr<EvaluationTree::Node>& child : currentNode->children)
//		{
//			if (child->data.first == bestEvaluation)
//			{
//				currentNode = child;
//				break;
//			}
//		}
//
//		movePrinterMove = currentNode->data.second;
//		movePrinterBoard = session.position.board;
//
//		session.MakeMove(currentNode->data.second);
//
//		Print();
//	}
//}