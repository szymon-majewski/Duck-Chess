#include "MoveMemento.h"

MoveMemento::PositionData::PositionData(FullMove move, Square enPassantTarget, uint8_t plyClock, CastlingRights castlingRights) :
    move(move),
    enPassantTarget(enPassantTarget),
    plyClock(plyClock),
    castlingRights(castlingRights) {}

void MoveMemento::SaveMove(FullMove move, Square enPassantTarget, uint8_t plyClock, CastlingRights castlingRights)
{
    moveHistory.Push(PositionData(move, enPassantTarget, plyClock, castlingRights));
}

MoveMemento::PositionData MoveMemento::RevertMove()
{
    return moveHistory.Pop();
}
