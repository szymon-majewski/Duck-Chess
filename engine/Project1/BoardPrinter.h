#pragma once

#include <stdio.h>
#include "Board.h"
#include "Piece.h"

class BoardPrinter
{
public:
	void PrintBoard(Piece board[Board::HEIGHT][Board::WIDTH])
	{
		for (int y = 0; y < Board::HEIGHT; ++y)
		{
			for (int x = 0; x < Board::WIDTH; ++x)
			{
				printf("%c ", Piece::FindPieceSymbol(board[y][x].GetPiece()));
			}

			printf("\n");
		}
	}
};