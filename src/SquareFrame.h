#pragma once

#include <QFrame>

class MainWindow;

class SquareFrame : public QFrame
{
public:
    MainWindow* mainWindow;
    unsigned int x;
    unsigned int y;

    SquareFrame() = default;
    SquareFrame(MainWindow* mainWindow, unsigned int x, unsigned int y);

protected:
    void mousePressEvent(QMouseEvent* event) override;
};
