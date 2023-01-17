#pragma once

#include "Game.h"

class PositionEvaluator
{
protected:

	Game* game;

public:

	static std::unordered_map <Piece::Type, double> piecesMaterial;

	PositionEvaluator(Game* game);
	virtual double Evaluate() = 0;
};