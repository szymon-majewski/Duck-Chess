#include <string>
#include "Piece.h"
#include "FullMove.h"
#include "Board.h"

std::string SquareStringFormat(const Square& square, bool lowerCase = false)
{
	std::string result;

	char fileLetter = lowerCase ? 'a' : 'A';

	result += fileLetter + ((uint8_t)square - 1) % Board::WIDTH;
	result += '1' + ((uint8_t)square - 1) / Board::WIDTH;

	return result;
}

// Update: multiple pieces of same type in same row/file
std::string MoveStringFormat(const FullMove& move, Piece::Type movingPieceType, bool take)
{
	std::string result;

	if (movingPieceType != Piece::Type::Pawn)
	{
		result += Piece::FindPieceSymbol((uint8_t)movingPieceType | (uint8_t)Piece::Color::White);
	}
	else if (take)
	{
		result += SquareStringFormat(move.sourceSquare, true)[0];
	}

	if (take)
	{
		result += 'x';
	}

	result += SquareStringFormat(move.targetSquare, true);
	result += Piece::FindPieceSymbol((uint8_t)Piece::Type::Duck | (uint8_t)Piece::Color::Both);
	result += SquareStringFormat(move.targetDuckSquare, true);

	return result;
}

Square SquareIdToSquare(std::string squareId)
{
	if (squareId[0] == '-')
	{
		return Square::None;
	}
	else
	{
		squareId[0] = toupper(squareId[0]);
		return (Square)((squareId[1] - '0' - 1) * Board::WIDTH + squareId[0] - 'A' + 1);
	}
}

//FullMove StringToMove(const std::string& moveString, const Board& board)
//{
//	Piece::PIECES_SYMBOLS_MAP
//}

void SquareToBoardIndices(const Square& square, int& y, int& x)
{
	y = ((uint8_t)square - 1) / Board::WIDTH;
	x = ((uint8_t)square - 1) % Board::WIDTH;
}

Square BoardIndicesToSquare(int y, int x)
{
	return (Square)(y * Board::WIDTH + x + 1);
}