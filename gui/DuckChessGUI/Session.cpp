#include "Session.h"

void Session::MakeMove(const FullMove& move)
{
    moveMemento.SaveMove(move, position.enPassantTarget, position.plyClock, position.castlingRights);
    winnerColor = position.Update(move);
}

void Session::UndoMove()
{
    position.UndoMove(moveMemento.RevertMove());
    winnerColor = PlayerColor::None;
}

void Session::Clear()
{
    moveMemento.Clear();
}
