#include "Session.h"

void Session::MakeMove(const Move& move)
{
	positionMemento.SavePosition(position);
	position.Udpdate(move);
}

void Session::UndoMove()
{
	position = positionMemento.RevertMove();
}