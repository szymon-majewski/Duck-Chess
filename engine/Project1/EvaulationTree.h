#pragma once

#include <utility>
#include <memory>
#include "Move.h"

class EvaluationTree
{
public:

	class Node
	{
	public:

		std::pair<int32_t, Move> data;
		std::vector<std::shared_ptr<Node>> children;

		Node(Evaluation evaluation, const Move& move);
	};

	std::shared_ptr<Node> root;

public:

	EvaluationTree();
	~EvaluationTree();

	std::shared_ptr<Node> AddNode(Evaluation evaluation, const Move& move, std::shared_ptr<Node>& parent);
	void DeleteNode(std::shared_ptr<Node>& node);
};