#include "Session.h"

Session::Session(const Position& position) : position(position) {}

void Session::MakeMove(const Move& move)
{
	positionMemento.SavePosition(position);
	position.Udpdate(move);
}

void Session::UndoMove()
{
	position = positionMemento.RevertMove();
}