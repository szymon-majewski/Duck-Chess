#pragma once

#include <QLabel>

class MainWindow;

class PieceLabel : public QLabel
{
public:
    MainWindow* mainWindow;
    unsigned int x;
    unsigned int y;

    PieceLabel(QWidget* parent, MainWindow* mainWindow, unsigned int x, unsigned int y);

protected:
    void mousePressEvent(QMouseEvent *event) override;
};
