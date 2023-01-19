#pragma once

#include <string>
#include <stdexcept>
#include "Board.h"
#include "Position.h"

class FenParser
{
public:

	void ParseFen(const std::string& fen, Position& game) const;
	const static int METADATA_COUNT = 5;

private:

	std::string* SplitFenMetadata(std::string& metadata) const;
	Square SquareIdToSquare(std::string squareId) const;
	CastlingRights ParseCastilngRights(std::string castlingRightsString) const;
};