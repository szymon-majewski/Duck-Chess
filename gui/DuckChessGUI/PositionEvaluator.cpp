#include "PositionEvaluator.h"

std::unordered_map<Board::Type, Evaluation> PositionEvaluator::piecesMaterial =
{
    { Board::Type::Pawns, 100 },
    { Board::Type::Knights, 550 },
    { Board::Type::Bishops, 250 },
    { Board::Type::Rooks, 400 },
    { Board::Type::Queens, 700 },
    { Board::Type::Kings, 0 }
};

// From blacks perspective
const std::array<std::array<int8_t, Board::SQUARES_COUNT>, 6> PositionEvaluator::POSITIONAL_SCORES
{{
    // Pawns
    {{
         90,  90,  90,  90,  90,  90,  90,  90,
         30,  30,  30,  40,  40,  30,  30,  30,
         20,  20,  20,  30,  30,  30,  20,  20,
         10,  10,  10,  20,  20,  10,  10,  10,
         5,   5,  10,  30,  30,   5,   5,   5,
         0,   0,   0,   5,   5,   0,   0,   0,
         0,   0,   0, -10, -10,   0,   0,   0,
         0,   0,   0,   0,   0,   0,   0,   0
    }},
    //Bishops
    {{
         0,   0,   0,   0,   0,   0,   0,   0,
         0,   0,   0,   0,   0,   0,   0,   0,
         0,   0,   0,  10,  10,   0,   0,   0,
         0,   0,  10,  20,  20,  10,   0,   0,
         0,   0,  10,  20,  20,  10,   0,   0,
         0,  10,   0,   0,   0,   0,  10,   0,
         0,  30,   0,   5,   5,   0,  30,   0,
         0,   0, -10,   0,   0, -10,   0,   0
    }},

    // Knights
    {{
         -20,   0,   0,   0,   0,   0,   0,  -20,
         -5,   0,   0,  10,  10,   0,   0,  -5,
         -5,   5,  20,  20,  20,  20,   5,  -5,
         -5,  10,  20,  30,  30,  20,  10,  -5,
         -5,  10,  20,  30,  30,  20,  10,  -5,
         -5,   5,  20,  10,  10,  20,   5,  -5,
         -5,   0,   0,   0,   0,   0,   0,  -5,
         -20, -10,   -10,   -10,   -10,   -10, -10,  -20
    }},
    // Rooks
    {{
        50,  50,  50,  50,  50,  50,  50,  50,
        50,  50,  50,  50,  50,  50,  50,  50,
        0,   0,  10,  20,  20,  10,   0,   0,
        0,   0,  10,  20,  20,  10,   0,   0,
        0,   0,  10,  20,  20,  10,   0,   0,
        0,   0,  10,  20,  20,  10,   0,   0,
        0,   0,  10,  20,  20,  10,   0,   0,
        0,   0,   10,  20,  20,   10,   0,   0
    }},
    // Queens
    {{
        10,  10,  10,  10,  10,  10,  10,  10,
        10,  10,  20,  20,  20,  20,  20,  10,
        0,   0,  10,  20,  20,  10,   0,   0,
        0,   0,  10,  20,  20,  10,   0,   0,
        0,   0,  10,  20,  20,  10,   0,   0,
        0,   0,  10,  20,  20,  10,   0,   0,
        0,   0,  10,  10,  10,  10,   0,   0,
        0,   0,   0,  0,  0,   0,   0,   0
    }},
    // Kings
    {{
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   5,   5,   5,   5,   0,   0,
        0,   5,   5,  10,  10,   5,   5,   0,
        0,   5,  10,  20,  20,  10,   5,   0,
        0,   5,  10,  20,  20,  10,   5,   0,
        0,   0,   5,  10,  10,   5,   0,   0,
        0,   5,   -5,  -5,  -5,   -5,   5,   0,
        5,   10,   15,   -15, -15,   0,  15,   10
    }}
}};
