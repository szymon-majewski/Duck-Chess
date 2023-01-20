#include <memory>
#include <list>

#include "Engine.h"

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

// Reverting moves - additional info is not enough to revert moves.
// It's not enough info. If a piece is taken, there is no way to know what piece was that.
// Possible solutions: 
// Memento whole position, not a move.
// Additional info for all kind of takes. That would be painful to code.

std::string someFen = "rnbqkb1r/ppppp1PP/3pn3/8/8/8/PPPPPP2/RNBQKBNR w KQkq - 0 1";

int main(int argc, char** argv)
{
	Engine::GetInstance()->SetSession(new Session(*(new Position())));
	Engine::GetInstance()->fenParser.ParseFen(someFen, Engine::GetInstance()->GetSession()->position);
	Engine::GetInstance()->Print();
	Engine::GetInstance()->searchDepth = 5;
	Engine::GetInstance()->MinMaxSearch(Engine::GetInstance()->GetSession()->position, Engine::GetInstance()->searchDepth, Engine::GetInstance()->GetSession()->position.playerToMove, 0, 0);

 	return 0;
}   