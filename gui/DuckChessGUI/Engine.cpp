#include "Engine.h"
#include "MaterialEvaluator.h"
#include "StandardPositionEvaluator.h"

Engine::Engine() :
    Engine(STARTING_POSITION_FEN) {}

Engine::Engine(std::string fen)
{
    searchDepth = DEFAULT_SEARCH_DEPTH;
    session.position.materialDisparity = session.position.CountMaterial();

    //evaluator = std::make_unique<StandardPositionEvaluator>();
    evaluator = std::make_unique<MaterialEvaluator>();
}

std::unique_ptr<Engine::SearchInfo> Engine::Search(Position position)
{
    session.Clear();
    session.position = position;

    auto returned = MinMaxSearch(searchDepth, NEGATIVE_INFINITY_EVALUATION, POSITIVE_INFINITY_EVALUATION, FullMove());
    returned->movesPath.pop_front();

    return returned;
}

//Evaluation Engine::MinMaxSearch(Position& position, unsigned depth, Evaluation alpha, Evaluation beta, std::shared_ptr<EvaluationTree::Node>& node)
std::unique_ptr<Engine::SearchInfo> Engine::MinMaxSearch(unsigned depth, Evaluation alpha, Evaluation beta, const FullMove& prevMove)
{
    if (depth == 0)
    {
        //return MinMaxSearchCaptures(alpha, beta, prevMove);
        return std::make_unique<SearchInfo>(evaluator->Evaluate(session.position), prevMove);
    }

    std::unique_ptr<std::vector<FullMove>> moves = movesGenerator.GenerateLegalMoves(session.position);

    std::unique_ptr<SearchInfo> bestSearchInfo = std::make_unique<SearchInfo>();
    std::unique_ptr<SearchInfo> currentSearchInfo;

    // White move
    if (session.position.playerToMove == PlayerColor::White)
    {
        bestSearchInfo->evaluation = OVER_NEGATIVE_INFINITY_EVALUATION;

        for (const FullMove& move : *moves)
        {
            session.MakeMove(move);

            // Check if white won the game
            if (session.winnerColor != PlayerColor::None)
            {
                session.UndoMove();

                bestSearchInfo = std::make_unique<SearchInfo>(POSITIVE_INFINITY_EVALUATION, move);
                break;
            }

            currentSearchInfo = MinMaxSearch(depth - 1, alpha, beta, move);
            session.UndoMove();

            if (currentSearchInfo->evaluation > bestSearchInfo->evaluation)
            {
                //bestSearchInfo = std::move(currentSearchInfo);
                bestSearchInfo->evaluation = currentSearchInfo->evaluation;
                bestSearchInfo->movesPath = currentSearchInfo->movesPath;
            }

            if (alpha < currentSearchInfo->evaluation)
            {
                alpha = currentSearchInfo->evaluation;
            }

            if (beta <= alpha)
            {
                break;
            }
        }
    }
    // Black move
    else
    {
        bestSearchInfo->evaluation = OVER_POSITIVE_INFINITY_EVALUATION;

        for (const FullMove& move : *moves)
        {
            session.MakeMove(move);

            // Check if black won the game
            if (session.winnerColor != PlayerColor::None)
            {
                session.UndoMove();

                bestSearchInfo = std::make_unique<SearchInfo>(NEGATIVE_INFINITY_EVALUATION, move);
                break;
            }

            currentSearchInfo = MinMaxSearch(depth - 1, alpha, beta, move);
            session.UndoMove();

            if (currentSearchInfo->evaluation < bestSearchInfo->evaluation)
            {
                //bestSearchInfo = std::move(currentSearchInfo);
                bestSearchInfo->evaluation = currentSearchInfo->evaluation;
                bestSearchInfo->movesPath = currentSearchInfo->movesPath;
            }

            if (beta > currentSearchInfo->evaluation)
            {
                beta = currentSearchInfo->evaluation;
            }

            if (beta <= alpha)
            {
                break;
            }
        }
    }

    bestSearchInfo->movesPath.emplace_front(prevMove);

    return bestSearchInfo;
}

std::unique_ptr<Engine::SearchInfo> Engine::MinMaxSearchCaptures(Evaluation alpha, Evaluation beta, const FullMove& prevMove)
{
    Evaluation standardSearchEvaluation = evaluator->Evaluate(session.position);

    auto moves = movesGenerator.GenerateLegalCaptures(session.position);

    if (moves->empty())
    {
        return std::make_unique<SearchInfo>(standardSearchEvaluation, prevMove);
    }

    std::unique_ptr<SearchInfo> bestSearchInfo = std::make_unique<SearchInfo>();
    std::unique_ptr<SearchInfo> currentSearchInfo;

    // White move
    if (session.position.playerToMove == PlayerColor::White)
    {
        bestSearchInfo->evaluation = OVER_NEGATIVE_INFINITY_EVALUATION;

        for (const FullMove& move : *moves)
        {
            session.MakeMove(move);

            // Check if white won the game
            if (session.winnerColor != PlayerColor::None)
            {
                session.UndoMove();

                bestSearchInfo = std::make_unique<SearchInfo>(POSITIVE_INFINITY_EVALUATION, move);
                break;
            }

            currentSearchInfo = MinMaxSearchCaptures(alpha, beta, move);
            session.UndoMove();

            if (currentSearchInfo->evaluation > bestSearchInfo->evaluation)
            {
                //bestSearchInfo = std::move(currentSearchInfo);
                bestSearchInfo->evaluation = currentSearchInfo->evaluation;
                bestSearchInfo->movesPath = currentSearchInfo->movesPath;
            }

            if (alpha < currentSearchInfo->evaluation)
            {
                alpha = currentSearchInfo->evaluation;
            }

            if (beta <= alpha)
            {
                break;
            }
        }

        if (standardSearchEvaluation >= alpha)
        {
            return std::make_unique<SearchInfo>(standardSearchEvaluation, prevMove);
        }
    }
    // Black move
    else
    {
        bestSearchInfo->evaluation = OVER_POSITIVE_INFINITY_EVALUATION;

        for (const FullMove& move : *moves)
        {
            session.MakeMove(move);

            // Check if black won the game
            if (session.winnerColor != PlayerColor::None)
            {
                session.UndoMove();

                bestSearchInfo = std::make_unique<SearchInfo>(NEGATIVE_INFINITY_EVALUATION, move);
                break;
            }

            currentSearchInfo = MinMaxSearchCaptures(alpha, beta, move);
            session.UndoMove();

            if (currentSearchInfo->evaluation < bestSearchInfo->evaluation)
            {
                //bestSearchInfo = std::move(currentSearchInfo);
                bestSearchInfo->evaluation = currentSearchInfo->evaluation;
                bestSearchInfo->movesPath = currentSearchInfo->movesPath;
            }

            if (beta > currentSearchInfo->evaluation)
            {
                beta = currentSearchInfo->evaluation;
            }

            if (beta <= alpha)
            {
                break;
            }
        }

        if (standardSearchEvaluation <= beta)
        {
            return std::make_unique<SearchInfo>(standardSearchEvaluation, prevMove);
        }
    }

    bestSearchInfo->movesPath.emplace_front(prevMove);

    return bestSearchInfo;
}
