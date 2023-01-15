#include "Engine.h"
#include "FenParser.h"
#include "BoardPrinter.h"

std::string someFen = "8/5k2/3p3D/1p1Pp2p/pP2Pp1P/P4P1K/8/8 b - - 99 50";

int main(int argc, char** argv)
{
	FenParser parser;
	Engine::GetInstance()->SetGame(new Game());

	parser.ParseFen(someFen, *Engine::GetInstance()->GetGame());
	Engine::GetInstance()->Print();

	return 0;
}