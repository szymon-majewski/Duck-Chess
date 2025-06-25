#pragma once

#include <QObject>
#include "Engine.h"
#include "Position.h"

class EngineWorker : public QObject
{
    Q_OBJECT

private:
    Engine engine;
    int* windowSignalsSentWithoutResponse;

public:
    explicit EngineWorker(int* windowSignalsSentWithoutResponse, QObject* parent = nullptr);

    unsigned int Depth();
    void SetDepth(unsigned int depth);

public slots:
    void Search(Position position);

signals:
    void ResultReady(const Engine::SearchInfo& result, long long time);
};
