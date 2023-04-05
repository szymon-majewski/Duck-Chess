#include "EngineWorker.h"

EngineWorker::EngineWorker(QObject* parent) :
    QObject(parent) {}

void EngineWorker::Search(const Position& position)
{
    auto result = *engine.Search(position);
    emit ResultReady(result);
}

unsigned int EngineWorker::Depth()
{
    return engine.searchDepth;
}
