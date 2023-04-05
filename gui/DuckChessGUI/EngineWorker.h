#pragma once

#include <QObject>
#include "Engine.h"
#include "Position.h"

class EngineWorker : public QObject
{
    Q_OBJECT

private:
    Engine engine;

public:
    explicit EngineWorker(QObject* parent = nullptr);

    unsigned int Depth();

public slots:
    void Search(const Position& position);

signals:
    void ResultReady(const Engine::SearchInfo& result);
};
