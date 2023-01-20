#include "Engine.h"
#include "StandardPositionEvaluator.h"

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

double Engine::MinMaxSearch(Position& position, unsigned depth, PlayerColor maximazingPlayer, int32_t alpha, int32_t beta)
{
	if (depth == 0)
	{
		return evaluator->Evaluate(position);
	}

	std::unique_ptr<std::list<Move>> moves = movesGenerator.GenerateLegalMoves(position);


	double evaluation = Engine::GetInstance()->evaluator->Evaluate(Engine::GetInstance()->session->position);
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
	positionInformationPrinter = new PositionInformationPrinter(&session->position);
	boardPrinter->SetSuccessor(positionInformationPrinter);

	// Setting what info is going to be displayed in console
	consolePrinterRequest = (ConsolePrinterHandler::Request)(
		(uint8_t)ConsolePrinterHandler::Request::Board |
		(uint8_t)ConsolePrinterHandler::Request::GameInformation);

	evaluator = new StandardPositionEvaluator();
}