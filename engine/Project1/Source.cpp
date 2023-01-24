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
* Implement alpha-beta pruning. X
*/

// Board pieces start from row 1 (so it goes bottom up)

// Reverting moves - additional info is not enough to revert moves.
// It's not enough info. If a piece is taken, there is no way to know what piece was that.
// Possible solutions: 
// Memento whole position, not a move.
// Additional info for all kind of takes. That would be painful to code.

std::string someFen = "rnbqkb1r/ppppp1PP/3pn3/8/8/8/PPPPPP2/RNBQKBNR w KQkq - 0 1";
std::string mateInTwoFen = "rn2kb1r/p1p1p2p/b4pp1/8/4PP1P/4nNP1/P4K1R/2q5 b kq - 3 18";
std::string simpleDuckFen = "3k4/5@2/8/8/2B5/8/8/5K2 w - - 0 1";
std::string duckChessPuzzleFen = "8/5Q1p/7k/5@1p/4Bn2/6q1/6P1/7K w - - 0 1";
std::string myGameFen = "r2qkb1r/1p1n1p2/p4npp/1Npp4/2@1b3/1P5P/PBPPQPP1/2KNRB1R w kq - 0 1";

int main(int argc, char** argv)
{
	Engine::GetInstance()->SetSession(new Session(*(new Position())));
	Engine::GetInstance()->fenParser.ParseFen(myGameFen, Engine::GetInstance()->GetSession()->position);
	Engine::GetInstance()->searchDepth = 3;
	Engine::GetInstance()->Print();

	Evaluation eval = Engine::GetInstance()->Search(Engine::GetInstance()->GetSession()->position, Engine::GetInstance()->searchDepth, NEGATIVE_INFINITY_EVALUATION, POSITIVE_INFINITY_EVALUATION);
	Engine::GetInstance()->PrintBestMoves(eval);
	std::cout << "Evaluation: " << eval;

 	return 0;
}