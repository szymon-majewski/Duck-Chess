#include "SquareFrame.h"
#include "mainwindow.h"

SquareFrame::SquareFrame(unsigned int x, unsigned int y) :
    x(x),
    y(y) {}

void SquareFrame::mousePressEvent(QMouseEvent* event)
{
    static_cast<MainWindow*>(parent())->OnEmptySquareClicked(x, y);
}
