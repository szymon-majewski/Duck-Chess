#pragma once

#include <memory>
#include <array>

#include "Position.h"
#include "PositionEvaluator.h"
#include "MovesGenerator.h"
#include "Session.h"

class Engine
{
private: /*DEBUG ->*/ public:
    Session session;
    MovesGenerator movesGenerator;
    std::unique_ptr<PositionEvaluator> evaluator;

    unsigned searchDepth;
    const unsigned DEFAULT_SEARCH_DEPTH = 3;

public:
    class SearchInfo
    {
    public:

        Evaluation evaluation;
        std::list<FullMove> movesPath;

        SearchInfo() :
            evaluation(0),
            movesPath(std::list<FullMove>()) {}

        SearchInfo(const Evaluation& evaluation, const FullMove& firstMove) :
            evaluation(evaluation),
            movesPath({ firstMove }) {}
    };

    Engine();
    Engine(std::string fen);

    std::unique_ptr<SearchInfo> Search(Position position);

private:
    std::unique_ptr<SearchInfo> MinMaxSearchWhite(unsigned depth, Evaluation alpha, Evaluation beta, const FullMove& prevMove);
    std::unique_ptr<SearchInfo> MinMaxSearchBlack(unsigned depth, Evaluation alpha, Evaluation beta, const FullMove& prevMove);
    std::unique_ptr<SearchInfo> MinMaxSearchCaptures(int depth, const FullMove& prevMove);
};
