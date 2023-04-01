#include <QMimeData>
#include <QDrag>
#include <QMouseEvent>

#include "PieceLabel.h"
#include "mainwindow.h"

PieceLabel::PieceLabel(QWidget* parent, unsigned int x, unsigned int y) :
    QLabel(parent),
    x(x),
    y(y) {}

void PieceLabel::mousePressEvent(QMouseEvent *event)
{
    static_cast<MainWindow*>(parent())->OnPieceClicked(x, y);
}
