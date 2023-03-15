#include <memory>
#include <list>

/*DEBUG*/ #include <iostream>

#include "Engine.h"

//std::string someFen = "rnbqkb1r/ppppp1PP/3pn3/8/8/8/PPPPPP2/RNBQKBNR w KQkq - 0 1";
//std::string mateInTwoFen = "rn2kb1r/p1p1p2p/b4pp1/8/4PP1P/4nNP1/P4K1R/2q5 b kq - 3 18";
const std::string simpleDuckFen = "3k4/5@2/8/8/2B5/8/8/5K2 w - - 0 1";
const std::string duckChessPuzzleFen = "8/5Q1p/7k/5@1p/4Bn2/6q1/6P1/7K w - - 0 1";
const std::string myGameFen = "r2qkb1r/1p1n1p2/p4npp/1Npp4/2@1b3/1P5P/PBPPQPP1/2KNRB1R w kq - 0 1";
const std::string myGameFen2 = "r2qkb1r/1p1n@p2/p2N1npp/2pp4/4b3/1P5P/PBPPQPP1/2KNRB1R b kq - 1 1";
const std::string myGameFen3 = "r2qkb1r/1p1n1p2/p2N1npp/2pp4/8/1P5P/PBPPQPb1/@1KNRB1R w kq - 1 1";
const std::string testFen = "rnbqkbnr/ppp1pppp/7@/8/7P/3p1PP1/PPPPP3/RNBQKBNR w KQkq - 0 4";
const std::string castlingTestFen = "r@2k3/8/8/8/8/8/8/4K2R w Kq - 0 1";
const std::string bugFen = "8/7k/2b5/7@/8/8/8/4K2R w - - 0 1";

int main(int argc, char** argv)
{
	Engine engine = Engine(testFen);

	engine.searchDepth = 2;
	engine.Print();

	Engine::SearchInfo eval = *engine.Search();
	engine.PrintBestMoves(eval.movesPath);
	std::cout << "Evaluation: " << eval.evaluation;

 	return 0;
}

/*
TODO:
* Delete the whole evaluation tree module. Instead after evaluating the position just generate all the moves and choose the path with the exact evaluation
* -- always choose the one that is the longest from the opponents perspective, and the shortest from player perspective. Shouldn't be much longer 
* -- than it is right now, but the memory optimasation will be significant.
* !!!!!!!!!! myGameFen, depth 4, move 1 suddenly changes to move 34 and some of the pieces change to opponent pieces (even different types)
* ColoringBoardPrinter attribute dynamic setting
* Request bit flags dynamic setting
* (It actually needs a game pointer to )
* squaresToEdgeCount make generate statically
* Update engine pointers either to unique_ptr or make it local (should be fine that way)
* Implement alpha-beta pruning. X
* Store evaluation in smaller variable
* Update GenerateLegalMoves - make cross join with free spaces to include duck moves
* Moves generator doesn't generate duck moves for castling and en passant
* Use flyweight design pattern to minimize memory usage in evaluation tree (can it even be done?)
* For now I memento whole position, but it's not acceptable for memory demand reason. Change it. Every move will from now on contain additional info about
* what piece if any was captured, so AdditionalInfo will be a 32 (or 16?) bit flag
* tuples structured binding in MoveMemento
* Probably Move::AdditionalInfo doesnt need color in castling info
* Probably movingPieceColor is not necessary in position Update and UndoMove
* There is some kind of bug when engine searches after taking the king. Run myGameFen on depth 4 or castlingTestFen on depth 3
* Why is node passed by copy in Engine::MinMaxSearch
* MoveStringFormat does not consider ambigous pieces moves
* When king or rook moves player should lose castling rights
*/

// Board pieces start from row 1 (so it goes bottom up)

// Reverting moves - additional info is not enough to revert moves.
// It's not enough info. If a piece is taken, there is no way to know what piece was that.
// Possible solutions: 
// Memento whole position, not a move.
// Additional info for all kind of takes. That would be painful to code.

// FOR NOW EN PASSANT IS NOT MARKED AS CAPTURE!!!!!!!!

// Engine::Engine(std::string) - initializing searchDepth(DEFAULT_SEARCH_DEPTH) didn't work - had to initialize in constructor body