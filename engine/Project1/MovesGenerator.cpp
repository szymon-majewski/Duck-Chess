#include "MovesGenerator.h"

const DirectionOffsets MovesGenerator::directions[DIRECTIONS_COUNT] =
{
	DirectionOffsets::North,
	DirectionOffsets::South,
	DirectionOffsets::West,
	DirectionOffsets::East,
	DirectionOffsets::NorthWest,
	DirectionOffsets::NorthEast,
	DirectionOffsets::SouthWest,
	DirectionOffsets::SouthEast,
};

unsigned MovesGenerator::squaresToEdgeCount[Board::SQUARES_COUNT][DIRECTIONS_COUNT];

MovesGenerator::MovesGenerator()
{
	for (int y = 0; y < Board::HEIGHT; ++y)
	{
		for (int x = 0; x < Board::WIDTH; ++x)
		{
			unsigned currentIndex = y * Board::WIDTH + x;

			squaresToEdgeCount[currentIndex][0] = Board::HEIGHT - y - 1;
			squaresToEdgeCount[currentIndex][1] = y;
			squaresToEdgeCount[currentIndex][2] = x;
			squaresToEdgeCount[currentIndex][3] = Board::WIDTH - x - 1;
			squaresToEdgeCount[currentIndex][4] = Min(squaresToEdgeCount[currentIndex][0], squaresToEdgeCount[currentIndex][2]);
			squaresToEdgeCount[currentIndex][5] = Min(squaresToEdgeCount[currentIndex][0], squaresToEdgeCount[currentIndex][3]);
			squaresToEdgeCount[currentIndex][6] = Min(squaresToEdgeCount[currentIndex][1], squaresToEdgeCount[currentIndex][2]);
			squaresToEdgeCount[currentIndex][7] = Min(squaresToEdgeCount[currentIndex][1], squaresToEdgeCount[currentIndex][3]);
		}
	}
}

std::unique_ptr<std::list<Move>> MovesGenerator::GenerateLegalMoves(const Position& position)
{
	std::unique_ptr<std::list<Move>> legalMoves(new std::list<Move>());

	for (unsigned y = (unsigned)Ranks::Rank1; y <= (unsigned)Ranks::Rank8; ++y)
	{
		for (unsigned x = (unsigned)Files::FileA; x <= (unsigned)Files::FileH; ++x)
		{
			Piece const* currentPiece = &position.board.pieces[y][x];

			AssignSquare(currentPiece, y, x, position.playerToMove);
		}
	}


	// Pawn moves
	for (const Square& pawnSquare : playerPiecesSquares.at(Piece::Type::Pawn))
	{
		//// One square forward
		Square northSquare = (Square)((int8_t)pawnSquare + (int8_t)DirectionOffsets::North);

		if (emptySquares.contains(northSquare))
		{
			//// Promoting
			if (northSquare >= Square::A8)
			{
				legalMoves->emplace_back(Move(pawnSquare, northSquare, Move::AdditionalInfo::PromotionToQueen));
				legalMoves->emplace_back(Move(pawnSquare, northSquare, Move::AdditionalInfo::PromotionToRook));
				legalMoves->emplace_back(Move(pawnSquare, northSquare, Move::AdditionalInfo::PromotionToKnight));
				legalMoves->emplace_back(Move(pawnSquare, northSquare, Move::AdditionalInfo::PromotionToBishop));
			}
			else
			{
				legalMoves->emplace_back(Move(pawnSquare, northSquare));

				//// Two squares forward
				Square doubleNorthSquare = (Square)((int8_t)northSquare + (int8_t)DirectionOffsets::North);

				//TODO: Maybe upgrade this a bit
				if (pawnSquare >= Square::A2 && pawnSquare <= Square::H2 && emptySquares.contains(doubleNorthSquare))
				{
					legalMoves->emplace_back(Move(pawnSquare, doubleNorthSquare));
				}
			}
		}

		//// Takes and en passant
		Square northWestSquare = (Square)((uint8_t)pawnSquare + (int8_t)DirectionOffsets::NorthWest);
		Square northEastSquare = (Square)((uint8_t)pawnSquare + (int8_t)DirectionOffsets::NorthEast);

		if ((uint8_t)pawnSquare % Board::WIDTH != 1)
		{
			if (opponentPiecesSquares.contains(northWestSquare))
			{
				// Takes and promotes
				if (northWestSquare >= Square::A8)
				{
					legalMoves->emplace_back(Move(pawnSquare, northWestSquare, Move::AdditionalInfo::PromotionToQueen));
					legalMoves->emplace_back(Move(pawnSquare, northWestSquare, Move::AdditionalInfo::PromotionToRook));
					legalMoves->emplace_back(Move(pawnSquare, northWestSquare, Move::AdditionalInfo::PromotionToKnight));
					legalMoves->emplace_back(Move(pawnSquare, northWestSquare, Move::AdditionalInfo::PromotionToBishop));
				}
				else
				{
					legalMoves->emplace_back(Move(pawnSquare, northWestSquare));
				}
			}
			else if (northWestSquare == position.enPassantTarget)
			{
				legalMoves->emplace_back(Move(pawnSquare, northWestSquare, Move::AdditionalInfo::EnPassant));
			}
		}
		if ((uint8_t)pawnSquare % Board::WIDTH != 0)
		{
			if (opponentPiecesSquares.contains(northWestSquare))
			{
				if (northWestSquare >= Square::A8)
				{
					legalMoves->emplace_back(Move(pawnSquare, northEastSquare, Move::AdditionalInfo::PromotionToQueen));
					legalMoves->emplace_back(Move(pawnSquare, northEastSquare, Move::AdditionalInfo::PromotionToRook));
					legalMoves->emplace_back(Move(pawnSquare, northEastSquare, Move::AdditionalInfo::PromotionToKnight));
					legalMoves->emplace_back(Move(pawnSquare, northEastSquare, Move::AdditionalInfo::PromotionToBishop));
				}
				else
				{
					legalMoves->emplace_back(Move(pawnSquare, northEastSquare));
				}
			}
			else if (northEastSquare == position.enPassantTarget)
			{
				legalMoves->emplace_back(Move(pawnSquare, northEastSquare, Move::AdditionalInfo::EnPassant));
			}
		}
	}

	// Knights moves
	for (const Square& knightSquare : playerPiecesSquares.at(Piece::Type::Knight))
	{
		std::unique_ptr<std::list<Square>> squaresKnightCanMoveTo = GenerateAllSquaresKnightMovesTo(knightSquare);

		for (const Square& squareKnightCanMoveTo : *squaresKnightCanMoveTo)
		{
			if (opponentPiecesSquares.contains(squareKnightCanMoveTo) || emptySquares.contains(squareKnightCanMoveTo))
			{
				legalMoves->emplace_back(Move(knightSquare, squareKnightCanMoveTo));
			}
		}
	}
	
	// Bishops moves
	GenerateLongRangePieceMoves(legalMoves, Piece::Type::Bishop);
	
	// Rooks moves
	GenerateLongRangePieceMoves(legalMoves, Piece::Type::Rook);

	// Queen moves
	GenerateLongRangePieceMoves(legalMoves, Piece::Type::Queen);

	// King moves
	Square squareInDirection;

	for (const Square& kingSquare : playerPiecesSquares.at(Piece::Type::King))
	{
		for (unsigned d = 0; d < DIRECTIONS_COUNT; ++d)
		{
			//// One square moves
			squareInDirection = (Square)((uint8_t)kingSquare + (int8_t)directions[d]);

			if (squaresToEdgeCount[(uint8_t)kingSquare - 1][d] > 0 && 
					(opponentPiecesSquares.contains(squareInDirection) ||
					 emptySquares.contains(squareInDirection)))
			{
				legalMoves->emplace_back(Move(kingSquare, squareInDirection));
			}
		}

		//// Castling
		if (position.playerToMove == PlayerColor::White)
		{
			if ((CastlingRights)((uint8_t)position.castlingRights & (uint8_t)CastlingRights::WhiteKingside) != CastlingRights::None &&
				emptySquares.contains(Square::F1) && emptySquares.contains(Square::G1))
			{
				legalMoves->emplace_back(Move(kingSquare, Square::G1, Move::AdditionalInfo::WhiteKingsideCastle));
			}
			if ((CastlingRights)((uint8_t)position.castlingRights & (uint8_t)CastlingRights::WhiteQueenside) != CastlingRights::None &&
				emptySquares.contains(Square::B1) && emptySquares.contains(Square::C1) && emptySquares.contains(Square::D1))
			{
				legalMoves->emplace_back(Move(kingSquare, Square::C1, Move::AdditionalInfo::WhiteQueensideCastle));
			}
		}
		else
		{
			if ((CastlingRights)((uint8_t)position.castlingRights & (uint8_t)CastlingRights::BlackKingside) != CastlingRights::None &&
				emptySquares.contains(Square::F8) && emptySquares.contains(Square::G8))
			{
				legalMoves->emplace_back(Move(kingSquare, Square::G8, Move::AdditionalInfo::BlackKingsideCastle));
			}
			if ((CastlingRights)((uint8_t)position.castlingRights & (uint8_t)CastlingRights::BlackQueenside) != CastlingRights::None &&
				emptySquares.contains(Square::B8) && emptySquares.contains(Square::C8) && emptySquares.contains(Square::D8))
			{
				legalMoves->emplace_back(Move(kingSquare, Square::C8, Move::AdditionalInfo::BlackQueensideCastle));
			}
		}
	}

	emptySquares.clear();
	playerPiecesSquares.clear();
	opponentPawnsSquares.clear();
	opponentPawnsSquares.clear();

	return legalMoves;
}

void MovesGenerator::AssignSquare(Piece const* piece, unsigned& y, unsigned& x, PlayerColor movingPlayerColor)
{
	if (piece->PieceType() == Piece::Type::None)
	{
		emptySquares.insert(SquareIndicesToSquare(y, x));

		return;
	}

	if (piece->PieceColor() == (Piece::Color)movingPlayerColor)
	{
		if (!playerPiecesSquares.contains(piece->PieceType()))
		{
			playerPiecesSquares.insert(std::pair(piece->PieceType(), std::unordered_set<Square>()));
		}
		
		playerPiecesSquares.at(piece->PieceType()).insert(SquareIndicesToSquare(y, x));
	}
	else
	{
		opponentPiecesSquares.insert(SquareIndicesToSquare(y, x));

		if (piece->PieceType() == Piece::Type::Pawn)
		{
			opponentPawnsSquares.insert(SquareIndicesToSquare(y, x));
		}
	}
}

Square MovesGenerator::SquareIndicesToSquare(unsigned& y, unsigned& x)
{
	return (Square)(y * Board::WIDTH + x + 1);
}

std::unique_ptr<std::list<Square>> MovesGenerator::GenerateAllSquaresKnightMovesTo(const Square& startingSquare)
{
	std::unique_ptr<std::list<Square>> knightSquares(new std::list<Square>());

	// 2x north
	uint8_t currentDirectionOffset = (uint8_t)startingSquare + (uint8_t)DirectionOffsets::North;

	if (startingSquare < Square::A7)
	{
		if ((uint8_t)startingSquare % Board::WIDTH != 1)
		{
			knightSquares->emplace_back((Square)(currentDirectionOffset + (uint8_t)DirectionOffsets::NorthWest));
		}
		if ((uint8_t)startingSquare % Board::WIDTH != 0)
		{
			knightSquares->emplace_back((Square)(currentDirectionOffset + (uint8_t)DirectionOffsets::NorthEast));
		}
	}

	// 2x south
	currentDirectionOffset = (uint8_t)startingSquare + (uint8_t)DirectionOffsets::South;

	if (startingSquare >= Square::A3)
	{
		if ((uint8_t)startingSquare % Board::WIDTH != 1)
		{
			knightSquares->emplace_back((Square)(currentDirectionOffset + (uint8_t)DirectionOffsets::SouthWest));
		}
		if ((uint8_t)startingSquare % Board::WIDTH != 0)
		{
			knightSquares->emplace_back((Square)(currentDirectionOffset + (uint8_t)DirectionOffsets::SouthEast));
		}
	}

	// 2x west
	currentDirectionOffset = (uint8_t)startingSquare + (uint8_t)DirectionOffsets::West;

	if ((uint8_t)startingSquare % Board::WIDTH != 1 && (uint8_t)startingSquare % Board::WIDTH != 2)
	{
		if (startingSquare >= Square::A2)
		{
			knightSquares->emplace_back((Square)(currentDirectionOffset + (uint8_t)DirectionOffsets::SouthWest));
		}
		if (startingSquare < Square::A8)
		{
			knightSquares->emplace_back((Square)(currentDirectionOffset + (uint8_t)DirectionOffsets::NorthWest));
		}
	}

	// 2x east
	currentDirectionOffset = (uint8_t)startingSquare + (uint8_t)DirectionOffsets::East;

	if ((uint8_t)startingSquare % Board::WIDTH != Board::WIDTH - 1 && (uint8_t)startingSquare % Board::WIDTH != 0)
	{
		if (startingSquare >= Square::A2)
		{
			knightSquares->emplace_back((Square)(currentDirectionOffset + (uint8_t)DirectionOffsets::SouthEast));
		}
		if (startingSquare < Square::A8)
		{
			knightSquares->emplace_back((Square)(currentDirectionOffset + (uint8_t)DirectionOffsets::NorthEast));
		}
	}

	return knightSquares;
}

void MovesGenerator::GenerateLongRangePieceMoves(const std::unique_ptr<std::list<Move>>& legalMoves, Piece::Type pieceType)
{
	Square squareInDirection;
	unsigned lowBound = 0;
	unsigned highBound = DIRECTIONS_COUNT;

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

	for (const Square& pieceSquare : playerPiecesSquares.at(pieceType))
	{
		for (unsigned d = lowBound; d < highBound; ++d)
		{
			squareInDirection = pieceSquare;

			for (unsigned s = 0; s < squaresToEdgeCount[(uint8_t)pieceSquare - 1][d]; ++s)
			{
				squareInDirection = (Square)((uint8_t)squareInDirection + (int8_t)directions[d]);

				if (emptySquares.contains(squareInDirection))
				{
					legalMoves->emplace_back(Move(pieceSquare, squareInDirection));
				}
				else if (opponentPiecesSquares.contains(squareInDirection))
				{
					legalMoves->emplace_back(Move(pieceSquare, squareInDirection));
					break;
				}
				else
				{
					break;
				}
			}
		}
	}
}