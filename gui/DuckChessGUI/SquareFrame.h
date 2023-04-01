#pragma once

#include <QFrame>

class SquareFrame : public QFrame
{
public:
    unsigned int x;
    unsigned int y;

    SquareFrame() = default;
    SquareFrame(unsigned int x, unsigned int y);

protected:
    void mousePressEvent(QMouseEvent* event) override;
};
