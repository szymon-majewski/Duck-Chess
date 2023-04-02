#include <QMimeData>
#include <QDrag>
#include <QMouseEvent>

#include "PieceLabel.h"
#include "mainWindow.h"

PieceLabel::PieceLabel(QWidget* parent, MainWindow* mainWindow, unsigned int x, unsigned int y) :
    QLabel(parent),
    mainWindow(mainWindow),
    x(x),
    y(y) {}

void PieceLabel::mousePressEvent(QMouseEvent *event)
{
    mainWindow->OnPieceClicked(x, y);
}
