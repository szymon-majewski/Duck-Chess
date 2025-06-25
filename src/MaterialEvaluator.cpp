#include "MaterialEvaluator.h"

inline Evaluation MaterialEvaluator::Evaluate(const Position& position)
{
    return position.materialDisparity;
}
