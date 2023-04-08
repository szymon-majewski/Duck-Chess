#include "EngineWorker.h"

EngineWorker::EngineWorker(int* windowSignalsSentWithoutResponse, QObject* parent) :
    windowSignalsSentWithoutResponse(windowSignalsSentWithoutResponse),
    QObject(parent) {}

void EngineWorker::Search(Position position)
{
    // There came signals after this one
    if (*windowSignalsSentWithoutResponse > 1)
    {
        --(*windowSignalsSentWithoutResponse);
        return;
    }

    auto result = *engine.Search(position);

    // There came signals during this search, so results of this search are deprecated
    if (*windowSignalsSentWithoutResponse > 1)
    {
        --(*windowSignalsSentWithoutResponse);
        return;
    }

    emit ResultReady(result);
}

unsigned int EngineWorker::Depth()
{
    return engine.searchDepth;
}
