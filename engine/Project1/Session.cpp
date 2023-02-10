#include "Session.h"

Session::Session(const Position& position) :
	position(position),
	winnerColor(PlayerColor::None) {}

void Session::MakeMove(const FullMove& move)
{
	moveMemento.SaveMove(move, position.enPassantTarget, position.plyClock);
	winnerColor = position.Update(move);
}

void Session::UndoMove()
{
	position.UndoMove(moveMemento.RevertMove());
	winnerColor = PlayerColor::None;
}