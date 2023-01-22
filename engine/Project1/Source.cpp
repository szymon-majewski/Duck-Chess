#include <memory>
#include <list>

/*DEBUG*/ #include <iostream>

#include "Engine.h"

/*
TODO: 
* ColoringBoardPrinter attribute dynamic setting
* Request bit flags dynamic setting
* I think Evaluate function should recieve a specific position, not have a game pointer
* (It actually needs a game pointer to )
* Update GenerateLegalMoves - make cross join with free spaces to include duck moves
* GenerateLegalMoves pawn moves should be diversified for colours
* squaresToEdgeCount make generate statically
* Update engine pointers either to unique_ptr or make it local (should be fine that way)
* Implement alpha-beta pruning.
*/

// Board pieces start from row 1 (so it goes bottom up)

// Reverting moves - additional info is not enough to revert moves.
// It's not enough info. If a piece is taken, there is no way to know what piece was that.
// Possible solutions: 
// Memento whole position, not a move.
// Additional info for all kind of takes. That would be painful to code.

std::string someFen = "rnbqkb1r/ppppp1PP/3pn3/8/8/8/PPPPPP2/RNBQKBNR w KQkq - 0 1";
std::string mateInTwoFen = "rn2kb1r/p1p1p2p/b4pp1/8/4PP1P/4nNP1/P6R/2q1K3 w kq - 2 18";

int main(int argc, char** argv)
{
	Engine::GetInstance()->SetSession(new Session(*(new Position())));
	Engine::GetInstance()->fenParser.ParseFen(mateInTwoFen, Engine::GetInstance()->GetSession()->position);
	Engine::GetInstance()->searchDepth = 4;
	Engine::GetInstance()->Print();

	std::cout << std::endl;
	std::cout << Engine::GetInstance()->MinMaxSearch(Engine::GetInstance()->GetSession()->position, Engine::GetInstance()->searchDepth, NEGATIVE_INFINITY_EVALUATION, POSITIVE_INFINITY_EVALUATION);

 	return 0;
}   