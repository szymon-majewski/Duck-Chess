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

const std::string bugHuntFen = "@7/8/8/8/8/6k1/5b2/7K w - - 0 1";

const std::string myGameFenRev = "r2qkb1r/1p1n1p2/p4npp/1Npp4/2@1b3/1P5P/PBPPQPP1/2KNRB1R b kq - 0 1";

const std::string enPassantWhiteFen = "1k6/8/8/4pP2/8/8/8/1K6 w - e6 0 1";
const std::string enPassantBlackFen = "4k3/8/8/8/4pP2/8/8/1K6 b - f3 0 1";
const std::string enPassant2Fen = "4k3/8/8/8/pP6/8/8/1K6 b - b3 0 1";
const std::string enPassant3Fen = "4k3/8/8/8/p7/8/1P6/1K6 w - - 0 1";

const std::string promotionFen = "8/1k4PP/7K/8/8/8/8/8 w - - 0 1";


// depth 3
const std::string bug2Fen = "rnbqkbnr/2pppppp/1p@5/pB6/3P1N2/2P1P3/PP3PPP/RNBQK2R b KQkq - 0 1";

int main(int argc, char** argv)
{
	Engine engine = Engine(myGameFen2);
	engine.searchDepth = 3;

	engine.Print();

	//engine.GameLoop();

	Engine::SearchInfo eval = *engine.Search();
	engine.PrintBestMoves(eval.movesPath);
	std::cout << "Evaluation: " << eval.evaluation;

 	return 0;
}

/*
TODO:
* Change PlayerInputManager so it only reads and parses input and all engine instructions are
* -- called in Engine class itself
* Enpassant is the source of all your problems...
* Delete these OVER_INFINITIES and instead just put first move in bestSearchInfo
* Returning list of moves in MinMaxSearch doesn't work properly, if one of the players is winning.
* -- it seems tay o plcorrect moves, but just doesn't print all of them
* !!!!!!!!!! myGameFen, depth 4, move 1 suddenly changes to move 34 and some of the pieces change to opponent pieces (even different types)
* -- it's because of castling - turn of adding castling moves in MovesGenerator
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