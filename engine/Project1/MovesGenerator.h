#pragma once

#include <list>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include "Move.h"
#include "FullMove.h"
#include "Position.h"

class MovesGenerator
{
private:

	// Map for all player pieces
	std::unordered_map<Piece::Type, std::unordered_set<Square>> playerPiecesSquares;
	std::unordered_set<Square> opponentPiecesSquares;
	std::unordered_set<Square> opponentPawnsSquares;
	std::unordered_set<Square> emptySquares;
	Square duckSquare;

public:

	MovesGenerator();

	std::unique_ptr<std::list<FullMove>> GenerateLegalMoves(const Position& position);
	static std::unique_ptr<std::list<Square>> GenerateAllSquaresKnightMovesTo(const Square& startingSquare);

	const static unsigned DIRECTIONS_COUNT = 8;
	const static DirectionOffsets DIRECTIONS[DIRECTIONS_COUNT];
	static unsigned squaresToEdgeCount[Board::SQUARES_COUNT][DIRECTIONS_COUNT];

	Move::AdditionalInfo SquarePieceTypeToMoveInfo(const Square& square, const Board& board);

private:

	void AssignSquare(Piece const* piece, unsigned& y, unsigned& x, PlayerColor movingPlayerColor);
	void GenerateLongRangePieceMoves(std::list<Move>& legalMoves, Piece::Type pieceType, const Board& board);
};