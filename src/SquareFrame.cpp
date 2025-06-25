#include "SquareFrame.h"
#include "mainwindow.h"

SquareFrame::SquareFrame(MainWindow* mainWindow, unsigned int x, unsigned int y) :
    mainWindow(mainWindow),
    x(x),
    y(y) {}

void SquareFrame::mousePressEvent(QMouseEvent* event)
{
    mainWindow->OnEmptySquareClicked(x, y);
}
