#pragma once

#include "Piece.h"

class Board
{
public:

    // Board dimensions
    static const int WIDTH = 8;
    static const int HEIGHT = 8;
    static const int SQUARES_COUNT = WIDTH * HEIGHT;

    Piece pieces[HEIGHT][WIDTH];

    friend class Engine;
};
