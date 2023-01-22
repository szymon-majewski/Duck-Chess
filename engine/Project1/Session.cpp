#include "Session.h"

Session::Session(const Position& position) :
	position(position),
	winnerColor(PlayerColor::None) {}

void Session::MakeMove(const Move& move)
{
	positionMemento.SavePosition(position);
	winnerColor = position.Update(move);
}

void Session::UndoMove()
{
	position = positionMemento.RevertMove();
	winnerColor = PlayerColor::None;
}