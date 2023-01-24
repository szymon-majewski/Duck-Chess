#include "EvaulationTree.h"

EvaluationTree::Node::Node(Evaluation evaluation, const FullMove& move) :
    data(evaluation, move) {}

EvaluationTree::EvaluationTree()
{
    root = std::make_shared<Node>(0, FullMove());
}

EvaluationTree::~EvaluationTree()
{
    DeleteNode(root);
}

std::shared_ptr<EvaluationTree::Node> EvaluationTree::AddNode(Evaluation evaluation, const FullMove& move, std::shared_ptr<Node>& parent)
{
    parent->children.emplace_back(std::make_shared<Node>(evaluation, move));

    return parent->children[parent->children.size() - 1];
}

void EvaluationTree::DeleteNode(std::shared_ptr<Node>& node)
{
    for (std::shared_ptr<Node>& child : node->children)
    {
        DeleteNode(child);
    }

    node.reset();
}
