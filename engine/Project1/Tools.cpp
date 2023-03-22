#include <string>
#include "Piece.h"
#include "FullMove.h"
#include "Board.h"
#include "MovesGenerator.h"

std::unique_ptr<std::vector<std::pair<int, int>>> GenerateSquaresWithBitPiece(const Square& startingSquare, Piece::Type pieceType,
	BitPiece bitpiece, const Board& board, const Square moveSourceSquare);
void AddAmbiguousPartToMoveStringFormat(std::string& result, const std::vector<std::pair<int, int>>& otherPiecesCoords,
	int movingPieceY, int movingPieceX, const Square moveSourceSquare);

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
	BitPiece movingBitPiece = board.pieces[movingPieceY][movingPieceX].GetBitPiece();

	switch (movingPieceType)
	{
		case Piece::Type::Pawn:
		{
			Move::AdditionalInfo promotionType;

			if (take || ((uint16_t)move.additionalInfo & (uint16_t)Move::AdditionalInfo::EnPassant) != (uint16_t)Move::AdditionalInfo::None)
			{
				result += SquareStringFormat(move.sourceSquare, true)[0];
				result += 'x';
			}

			result += SquareStringFormat(move.targetSquare, true);

			if ((promotionType = (Move::AdditionalInfo)((uint16_t)move.additionalInfo & Move::promotionChecker)) != Move::AdditionalInfo::None)
			{
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
			}

			result += Piece::FindPieceSymbol((uint8_t)Piece::Type::Duck | (uint8_t)Piece::Color::Both);
			result += SquareStringFormat(move.targetDuckSquare, true);

			return result;
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
		case Piece::Type::Knight:
		{
			result += Piece::FindPieceSymbol((uint8_t)movingPieceType | (uint8_t)Piece::Color::White);

			std::unique_ptr<std::list<Square>> knightSquares = MovesGenerator::GenerateAllSquaresKnightMovesTo(move.targetSquare);
			std::vector<std::pair<int, int>> knightSquaresCoords((*knightSquares).size() - 1);
			int i = 0;

			for (const Square& knightSquare : *knightSquares)
			{
				if (knightSquare != move.sourceSquare)
				{
					SquareToBoardIndices(knightSquare, knightSquaresCoords[i].first, knightSquaresCoords[i].second);
					++i;
				}
			}

			std::erase_if(knightSquaresCoords,
				[&board, movingBitPiece](const std::pair<int, int> coords)
				{
					return board.pieces[coords.first][coords.second].GetBitPiece() != movingBitPiece;
				});

			AddAmbiguousPartToMoveStringFormat(result, knightSquaresCoords, movingPieceY, movingPieceX, move.sourceSquare);

			break;
		}
		default:
		{
			result += Piece::FindPieceSymbol((uint8_t)movingPieceType | (uint8_t)Piece::Color::White);

			auto otherPiecesCoords = GenerateSquaresWithBitPiece(move.targetSquare, movingPieceType, movingBitPiece, board, move.sourceSquare);
			AddAmbiguousPartToMoveStringFormat(result, *otherPiecesCoords, movingPieceY, movingPieceX, move.sourceSquare);

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

// PRIVATE SECTION

std::unique_ptr<std::vector<std::pair<int, int>>> GenerateSquaresWithBitPiece(const Square& startingSquare, Piece::Type pieceType,
	BitPiece bitpiece, const Board& board, const Square originalCallingSquare)
{
	Square squareInDirection;
	unsigned lowBound = 0;
	unsigned highBound = MovesGenerator::DIRECTIONS_COUNT;
	auto resultSquares = std::make_unique<std::vector<std::pair<int, int>>>();
	int x;
	int y;

	if (pieceType == Piece::Type::Bishop)
	{
		// Starting with d = 4 (only diagonal directions)
		lowBound = 4;
	}
	else if (pieceType == Piece::Type::Rook)
	{
		// Ending with d = 4 (only horizontal and vertical directions)
		highBound = 4;
	}

	for (unsigned d = lowBound; d < highBound; ++d)
	{
		squareInDirection = startingSquare;

		for (unsigned s = 0; s < MovesGenerator::squaresToEdgeCount[(uint8_t)startingSquare - 1][d]; ++s)
		{
			squareInDirection = (Square)((uint8_t)squareInDirection + (int8_t)MovesGenerator::DIRECTIONS[d]);
			SquareToBoardIndices(squareInDirection, y, x);

			if (board.pieces[y][x].GetBitPiece() == NO_PIECE)
			{
				continue;
			}
			else if (board.pieces[y][x].GetBitPiece() == bitpiece && squareInDirection != originalCallingSquare)
			{
				(*resultSquares).emplace_back(std::make_pair(y, x));
				break;
			}
			else
			{
				break;
			}
		}
	}

	return resultSquares;
}

void AddAmbiguousPartToMoveStringFormat(std::string& result, const std::vector<std::pair<int, int>>& otherPiecesCoords,
	int movingPieceY, int movingPieceX, const Square moveSourceSquare)
{
	if (otherPiecesCoords.size())
	{
		bool fileRelatedExists = false;
		bool rowRelatedExists = false;

		for (const std::pair<int, int>& coords : otherPiecesCoords)
		{
			if (coords.second == movingPieceX)
			{
				fileRelatedExists = true;
			}

			if (coords.first == movingPieceY)
			{
				rowRelatedExists = true;
			}
		}

		if (!(fileRelatedExists || rowRelatedExists))
		{
			result += SquareStringFormat(moveSourceSquare, true)[0];
		}
		else
		{
			if (rowRelatedExists)
			{
				result += SquareStringFormat(moveSourceSquare, true)[0];
			}
			if (fileRelatedExists)
			{
				result += SquareStringFormat(moveSourceSquare, true)[1];
			}
		}
	}
}

//FullMove StringToMove(const std::string& moveString, const Board& board)
//{
//	Piece::PIECES_SYMBOLS_MAP
//}