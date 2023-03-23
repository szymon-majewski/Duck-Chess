#include <iostream>

#include "PlayerInputManager.h"
#include "Engine.h"

extern Square SquareIdToSquare(std::string squareId);
extern std::string MoveStringFormat(const FullMove& move, const Board& board);

PlayerInputManager::PlayerInputManager(Engine& engine) :
	engine(engine) {}

void PlayerInputManager::ReadAndManageInput()
{
	/* Commands:
	* move [move in PGN]
	* movec [piece source square] [piece target square] [duck target square] ?[promotion piece]?
	* undo
	* eval
	* board
	* exit
	*/
	std::string command;
	bool commandRead = false;

	while (!commandRead)
	{
		std::cin >> command;

		if (command == "move")
		{
			commandRead = ManageMove();
		}
		else if (command == "undo")
		{
			commandRead = ManageUndo();
		}
		else if (command == "eval")
		{
			commandRead = ManageEval();
		}
		else if (command == "movec")
		{
			commandRead = ManageMoveC();
		}
		else if (command == "board")
		{
			engine.Print();
			commandRead = true;
		}
		else if (command == "exit")
		{
			std::exit(0);
		}
		else
		{
			std::cout << "Unknown command: " + command << std::endl;
		}
	}
}

bool PlayerInputManager::ManageMove()
{
	// If game ended earlier
	if (engine.session.winnerColor != PlayerColor::None)
	{
		return false;
	}

	std::string moveStr;
	FullMove inputMove;
	bool validMove = false;
	auto moves = engine.movesGenerator.GenerateLegalMoves(engine.session.position);

	std::cin >> moveStr;

	for (const FullMove& move : *moves)
	{
		if (moveStr == MoveStringFormat(move, engine.session.position.board))
		{
			inputMove = move;
			validMove = true;

			break;
		}
	}

	if (!validMove)
	{
		return false;
	}

	std::cout << "\n" << MoveStringFormat(inputMove, engine.session.position.board) << "\n\n";
	engine.session.MakeMove(inputMove);
	engine.Print();

	if (engine.session.winnerColor != PlayerColor::None)
	{
		PrintWinnerEvaluation();
		return true;
	}

	auto searchInfo = engine.Search();

	std::cout << MoveStringFormat((*searchInfo).movesPath.front(), engine.session.position.board) << "\n\n";
	engine.session.MakeMove((*searchInfo).movesPath.front());
	engine.Print();

	if (engine.session.winnerColor != PlayerColor::None)
	{
		PrintWinnerEvaluation();
		return true;
	}

	return true;
}

bool PlayerInputManager::ManageUndo()
{
	// Undo computer and player move, but undo only once if computer lost
	engine.session.UndoMove();

	if (engine.session.winnerColor == engine.playerColor || engine.session.winnerColor == PlayerColor::None)
	{
		engine.session.UndoMove();
	}
	engine.Print();

	return true;
}

bool PlayerInputManager::ManageEval()
{
	// If game ended earlier
	if (engine.session.winnerColor != PlayerColor::None)
	{
		PrintWinnerEvaluation();
	}
	else
	{
		auto searchInfo = engine.Search();
		std::string bestMove = MoveStringFormat((*searchInfo).movesPath.front(), engine.session.position.board);

		engine.PrintEvaluation((*searchInfo).evaluation);
		std::cout << "Best move: " << bestMove << "\n\n";
	}

	return true;
}

bool PlayerInputManager::ManageMoveC()
{
	// If game ended earlier
	if (engine.session.winnerColor != PlayerColor::None)
	{
		return false;
	}

	std::string squareStr;
	Square squares[3];
	FullMove inputMove;
	bool validMove = false;
	auto moves = engine.movesGenerator.GenerateLegalMoves(engine.session.position);

	for (int i = 0; i < 3; ++i)
	{
		std::cin >> squareStr;
		squares[i] = SquareIdToSquare(squareStr);
	}

	for (const FullMove& move : *moves)
	{
		if (move.sourceSquare == squares[0] &&
			move.targetSquare == squares[1] &&
			move.targetDuckSquare == squares[2])
		{
			inputMove = move;
			validMove = true;

			if ((Move::AdditionalInfo)((uint16_t)move.additionalInfo & (uint16_t)Move::promotionChecker) != Move::AdditionalInfo::None)
			{
				std::string promotingPieceSymbol;
				std::cin >> promotingPieceSymbol;

				if (promotingPieceSymbol == "q")
				{
					inputMove.additionalInfo = (Move::AdditionalInfo)((uint16_t)inputMove.additionalInfo | (uint16_t)Move::AdditionalInfo::PromotionToQueen);
				}
				else if (promotingPieceSymbol == "r")
				{
					inputMove.additionalInfo = (Move::AdditionalInfo)((uint16_t)inputMove.additionalInfo | (uint16_t)Move::AdditionalInfo::PromotionToRook);
				}
				else if (promotingPieceSymbol == "n")
				{
					inputMove.additionalInfo = (Move::AdditionalInfo)((uint16_t)inputMove.additionalInfo | (uint16_t)Move::AdditionalInfo::PromotionToKnight);
				}
				else if (promotingPieceSymbol == "b")
				{
					inputMove.additionalInfo = (Move::AdditionalInfo)((uint16_t)inputMove.additionalInfo | (uint16_t)Move::AdditionalInfo::PromotionToBishop);
				}
				else
				{
					validMove = false;
				}
			}

			break;
		}
	}

	if (!validMove)
	{
		return false;
	}

	std::cout << "\n" << MoveStringFormat(inputMove, engine.session.position.board) << "\n\n";
	engine.session.MakeMove(inputMove);
	engine.Print();

	if (engine.session.winnerColor != PlayerColor::None)
	{
		PrintWinnerEvaluation();
		return true;
	}

	auto searchInfo = engine.Search();

	std::cout << MoveStringFormat((*searchInfo).movesPath.front(), engine.session.position.board) << "\n\n";
	engine.session.MakeMove((*searchInfo).movesPath.front());
	engine.Print();

	if (engine.session.winnerColor != PlayerColor::None)
	{
		PrintWinnerEvaluation();
		return true;
	}

	return true;
}

void PlayerInputManager::PrintWinnerEvaluation()
{
	std::cout << "Evaluation: ";

	if (engine.session.winnerColor == PlayerColor::White)
	{
		std::cout << "1-0";
	}
	else
	{
		std::cout << "0-1";
	}

	std::cout << '\n';
}