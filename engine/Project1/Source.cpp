#include "Engine.h"
#include "FenParser.h"
#include "BoardPrinter.h"

int main(int argc, char** argv)
{
	FenParser parser;
	Engine::GetInstance()->SetGame(new Game());

	parser.ParseFen(STARTING_POSITION_FEN, *Engine::GetInstance()->GetGame());
	Engine::GetInstance()->Print();

	return 0;
}