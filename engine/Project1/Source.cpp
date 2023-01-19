#include <memory>
#include <list>

#include "Engine.h"
#include "FenParser.h"
#include "BoardPrinter.h"

/*
TODO: 
* Request bit flags dynamic setting
* I think Evaluate function should recieve a specific position, not have a game pointer
* (It actually needs a game pointer to )
* Update GenerateLegalMoves - make cross join with free spaces to include duck moves
* GenerateLegalMoves pawn moves should be diversified for colours
* squaresToEdgeCount make generate statically
* Update engine pointers either to unique_ptr or make it local (should be fine that way)
*/

// Board pieces start from row 1 (so it goes bottom up)

std::string someFen = "rnbqkb1r/ppppp1PP/3pn3/8/8/8/PPPPPP2/RNBQKBNR w KQkq - 0 1";

int main(int argc, char** argv)
{
	FenParser parser;
	Engine::GetInstance()->SetGame(new Game());

	parser.ParseFen(someFen, *Engine::GetInstance()->GetGame());
	Engine::GetInstance()->Print();

	std::unique_ptr<std::list<Move>> moves = Engine::GetInstance()->movesGenerator.GenerateLegalMoves(*Engine::GetInstance()->game);

 	return 0;
}