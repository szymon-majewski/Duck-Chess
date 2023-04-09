#include <memory>
#include <list>
#include <chrono>

//DEBUG
#include <iostream>

#include "Engine.h"

const std::string someFen = "rnbqkb1r/ppppp1PP/3pn3/8/8/8/PPPPPP2/RNBQKBNR w KQkq - 0 1";
const std::string mateInTwoFen = "rn2kb1r/p1p1p2p/b4pp1/8/4PP1P/4nNP1/P4K1R/2q5 b kq - 3 18";
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
const std::string enPassantBlackFen = "4k3/8/8/8/4pP2/8/8/1K6 b - f3 0 15";
const std::string enPassant2Fen = "4k3/8/8/8/pP6/8/8/1K6 b - b3 0 1";
const std::string enPassant3Fen = "4k3/8/8/8/p7/8/1P6/1K6 w - - 0 1";

const std::string promotionFen = "8/1k4PP/7K/8/8/8/8/8 w - - 0 1";

// depth 3
const std::string bug2Fen = "rnbqkbnr/2pppppp/1p@5/pB6/3P1N2/2P1P3/PP3PPP/RNBQK2R b KQkq - 0 1";

const std::string castleFen = "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1";
const std::string ambiguousFen = "k7/3N1N2/2N3N1/4q3/2N3N1/3N1N2/8/1K6 w - - 0 1";
const std::string ambiguousFenOnlyLetter = "k7/8/8/4q3/2N3N1/8/8/1K6 w - - 0 1";
const std::string ambiguousFen123 = "k7/8/2N3N1/4q3/6N1/8/8/1K6 w - - 0 1";
const std::string ambibiFen = "k7/8/2N3N1/4q3/2N3N1/8/8/1K6 w - - 0 1";
const std::string ambiQueFen = "k7/2Q5/8/2Q1q3/8/2Q5/8/1K6 w - - 0 1";
const std::string ambiMANYQUEENFen = "k7/2Q1Q3/8/4q3/8/6Q1/8/1K2Q3 w - - 0 1";
const std::string ambiBIBIBIBSHOPFen = "2k5/8/8/1BB5/1B6/5B2/2B5/3K4 w - - 0 1";
const std::string ambishopFen = "8/1B3B2/8/8/7k/1B3B2/8/3K4 w - - 0 1";
const std::string rokesFen = "8/r5RR/8/8/3R3k/1R3R2/3R4/3K4 w - - 0 1";
 
int main(int argc, char** argv)
{
	Engine engine = Engine(STARTING_POSITION_FEN);
	engine.searchDepth = 3;

	engine.Print();

	// GAME LOOP PART

	engine.playerColor = PlayerColor::White;
	engine.GameLoop();


	// BEST MOVES PATH PART

	//auto start = std::chrono::high_resolution_clock::now();
	//Engine::SearchInfo eval = *engine.Search();
	//auto end = std::chrono::high_resolution_clock::now();
	//auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

	//engine.PrintBestMoves(eval.movesPath);
	//engine.PrintEvaluation(eval.evaluation);
	//std::cout << "Execution time: " << duration << " ms" << std::endl;

 	return 0;
}

/*
TODO:
* myGameFen - gameLoop - take the king and 2 undos - duplicated duck
* Move ordering - maybe some priority system
* Undo on the first move
* Change PlayerInputManager so it only reads and parses input and all engine instructions are
* -- called in Engine class itself
* Delete these OVER_INFINITIES and instead just put first move in bestSearchInfo
* ColoringBoardPrinter attribute dynamic setting
* Request bit flags dynamic setting
* squaresToEdgeCount make generate statically
* Store evaluation in smaller variable
* Use flyweight design pattern to minimize memory usage in evaluation tree (can it even be done?)
* tuples structured binding in MoveMemento
* Probably Move::AdditionalInfo doesnt need color in castling info
* Probably movingPieceColor is not necessary in position Update and UndoMove
*/

// Board pieces start from row 1 (so it goes bottom up)

// FOR NOW EN PASSANT IS NOT MARKED AS CAPTURE!!!!!!!!

// Engine::Engine(std::string) - initializing searchDepth(DEFAULT_SEARCH_DEPTH) didn't work - had to initialize in constructor body