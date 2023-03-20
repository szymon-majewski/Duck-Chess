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

void SquareToBoardIndices(const Square& square, int& y, int& x)
{
	y = ((uint8_t)square - 1) / Board::WIDTH;
	x = ((uint8_t)square - 1) % Board::WIDTH;
}

Square BoardIndicesToSquare(const unsigned& y, const unsigned& x)
{
	return (Square)(y * Board::WIDTH + x + 1);
}

// Update: multiple pieces of same type in same row/file
std::string MoveStringFormat(const FullMove& move, const Board& board)
{
	std::string result;
	bool take = ((uint16_t)move.additionalInfo & (uint16_t)Move::captureChecker) != (uint16_t)Move::AdditionalInfo::None;
	int movingPieceY;
	int movingPieceX;
	int targetSquareY;
	int targetSquareX;

	SquareToBoardIndices(move.sourceSquare, movingPieceY, movingPieceX);
	SquareToBoardIndices(move.targetSquare, targetSquareY, targetSquareX);

	Piece::Type movingPieceType = board.pieces[movingPieceY][movingPieceX].PieceType();

	switch (movingPieceType)
	{
		case Piece::Type::Pawn:
		{
			Move::AdditionalInfo promotionType;

			if (take)
			{
				result += SquareStringFormat(move.sourceSquare, true)[0];
			}
			if ((promotionType = (Move::AdditionalInfo)((uint16_t)move.additionalInfo & Move::promotionChecker)) != Move::AdditionalInfo::None)
			{
				result += SquareStringFormat(move.targetSquare, true);

				switch (promotionType)
				{
					case Move::AdditionalInfo::PromotionToQueen:
					{
						result += "=Q";
						break;
					}
					case Move::AdditionalInfo::PromotionToRook:
					{
						result += "=R";
						break;
					}
					case Move::AdditionalInfo::PromotionToKnight:
					{
						result += "=N";
						break;
					}
					case Move::AdditionalInfo::PromotionToBishop:
					{
						result += "=B";
						break;
					}
				}

				result += Piece::FindPieceSymbol((uint8_t)Piece::Type::Duck | (uint8_t)Piece::Color::Both);
				result += SquareStringFormat(move.targetDuckSquare, true);

				return result;
			}

			break;
		}
		case Piece::Type::King:
		{
			if ((Move::AdditionalInfo)((uint16_t)move.additionalInfo & Move::castlingChecker) != Move::AdditionalInfo::None)
			{
				if (move.additionalInfo == Move::AdditionalInfo::BlackKingsideCastle ||
					move.additionalInfo == Move::AdditionalInfo::WhiteKingsideCastle)
				{
					result += KINGSIDE_CASTLING_SYMBOL;
				}
				else
				{
					result += QUEENSIDE_CASTLING_SYMBOL;
				}

				result += Piece::FindPieceSymbol((uint8_t)Piece::Type::Duck | (uint8_t)Piece::Color::Both);
				result += SquareStringFormat(move.targetDuckSquare, true);

				return result;
			}
			else
			{
				result += Piece::FindPieceSymbol((uint8_t)movingPieceType | (uint8_t)Piece::Color::White);
			}

			break;
		}
		default:
		{
			result += Piece::FindPieceSymbol((uint8_t)movingPieceType | (uint8_t)Piece::Color::White);
			break;
		}
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