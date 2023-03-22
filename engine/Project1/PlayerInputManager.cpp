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
	* move [piece source square] [piece target square] [duck target square] ?[promotion piece]?
	* undo
	* eval
	*/
	std::string command;
	bool commandRead = false;

	while (!commandRead)
	{
		std::cin >> command;

		if (command == "move")
		{
			ManageMove();
			commandRead = true;
		}
		else if (command == "undo")
		{
			ManageUndo();
			commandRead = true;
		}
		else if (command == "eval")
		{
			ManageEval();
			commandRead = true;
		}
		else if (command == "movec")
		{
			ManageMoveC();
			commandRead = true;
		}
		else if (command == "board")
		{
			engine.Print();
			commandRead = true;
		}
	}
}

void PlayerInputManager::ManageMove()
{
	std::string moveStr;
	FullMove inputMove;
	bool validMove = false;

	while (!validMove)
	{
		std::cin >> moveStr;

		auto moves = engine.movesGenerator.GenerateLegalMoves(engine.session.position);

		for (const FullMove& move : *moves)
		{
			if (moveStr == MoveStringFormat(move, engine.session.position.board))
			{
				inputMove = move;
				validMove = true;

				break;
			}
		}
	}

	std::cout << "\n" << MoveStringFormat(inputMove, engine.session.position.board) << "\n\n";
	engine.session.MakeMove(inputMove);
	engine.Print();

	if ((Move::AdditionalInfo)((uint16_t)inputMove.additionalInfo & (uint16_t)Move::AdditionalInfo::CapturedKing) != Move::AdditionalInfo::None)
	{
		// TEMPORARY
		exit(0);
	}

	auto searchInfo = engine.Search();

	std::cout << MoveStringFormat((*searchInfo).movesPath.front(), engine.session.position.board) << "\n\n";
	engine.session.MakeMove((*searchInfo).movesPath.front());
	engine.Print();

	if ((Move::AdditionalInfo)((uint16_t)(*searchInfo).movesPath.front().additionalInfo & (uint16_t)Move::AdditionalInfo::CapturedKing) != Move::AdditionalInfo::None)
	{
		// TEMPORARY
		exit(0);
	}
}

void PlayerInputManager::ManageUndo()
{
	// Undo computer and player move
	engine.session.UndoMove();
	engine.session.UndoMove();
	engine.Print();
}

void PlayerInputManager::ManageEval()
{
	auto searchInfo = engine.Search();
	std::string bestMove = MoveStringFormat((*searchInfo).movesPath.front(), engine.session.position.board);

	std::cout << "Evaluation: " << (*searchInfo).evaluation << "\n";
	std::cout << "Best move: " << bestMove << "\n\n";
}

void PlayerInputManager::ManageMoveC()
{
	std::string squareStr;
	Square squares[3];
	FullMove inputMove;
	bool validMove = false;

	while (!validMove)
	{
		for (int i = 0; i < 3; ++i)
		{
			std::cin >> squareStr;
			squares[i] = SquareIdToSquare(squareStr);
		}

		auto moves = engine.movesGenerator.GenerateLegalMoves(engine.session.position);

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
	}

	std::cout << "\n" << MoveStringFormat(inputMove, engine.session.position.board) << "\n\n";
	engine.session.MakeMove(inputMove);
	engine.Print();

	if ((Move::AdditionalInfo)((uint16_t)inputMove.additionalInfo & (uint16_t)Move::AdditionalInfo::CapturedKing) != Move::AdditionalInfo::None)
	{
		// TEMPORARY
		exit(0);
	}

	auto searchInfo = engine.Search();

	std::cout << MoveStringFormat((*searchInfo).movesPath.front(), engine.session.position.board) << "\n\n";
	engine.session.MakeMove((*searchInfo).movesPath.front());
	engine.Print();

	if ((Move::AdditionalInfo)((uint16_t)(*searchInfo).movesPath.front().additionalInfo & (uint16_t)Move::AdditionalInfo::CapturedKing) != Move::AdditionalInfo::None)
	{
		// TEMPORARY
		exit(0);
	}
}