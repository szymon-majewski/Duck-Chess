#pragma once

#include <utility>
#include <memory>
#include "FullMove.h"

class EvaluationTree
{
public:

	class Node
	{
	public:

		std::pair<int32_t, FullMove> data;
		std::vector<std::shared_ptr<Node>> children;

		Node(Evaluation evaluation, const FullMove& move);
	};

	std::shared_ptr<Node> root;

public:

	EvaluationTree();
	~EvaluationTree();

	std::shared_ptr<Node> AddNode(Evaluation evaluation, const FullMove& move, std::shared_ptr<Node>& parent);
	void DeleteNode(std::shared_ptr<Node>& node);
};