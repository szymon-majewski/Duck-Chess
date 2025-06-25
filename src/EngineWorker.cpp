#include <chrono>
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

    auto start = std::chrono::high_resolution_clock::now();
    auto result = *engine.Search(position);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // There came signals during this search, so results of this search are deprecated
    if (*windowSignalsSentWithoutResponse > 1)
    {
        --(*windowSignalsSentWithoutResponse);
        return;
    }

    emit ResultReady(result, duration);
}

unsigned int EngineWorker::Depth()
{
    return engine.searchDepth;
}

void EngineWorker::SetDepth(unsigned int depth)
{
    engine.searchDepth = depth;
}
