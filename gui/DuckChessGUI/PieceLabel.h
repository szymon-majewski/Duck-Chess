#pragma once

#include <QLabel>

class PieceLabel : public QLabel
{
public:
    unsigned int x;
    unsigned int y;

    PieceLabel(QWidget* parent, unsigned int x, unsigned int y);

protected:
    void mousePressEvent(QMouseEvent *event) override;
};
