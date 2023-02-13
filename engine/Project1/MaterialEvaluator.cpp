#include "MaterialEvaluator.h"

Evaluation MaterialEvaluator::Evaluate(const Position& position)
{
	return position.materialDisparity;
}
