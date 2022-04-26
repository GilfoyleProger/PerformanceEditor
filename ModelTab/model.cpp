#include "model.h"

Model::Model() {}

void Model::attachNode(std::unique_ptr<Node> node)
{
    if (node != nullptr)
        nodes.emplace_back(std::move(node));
}

const std::vector<std::unique_ptr<Node>>& Model::getNodes() const
{
    return nodes;
}

void Model::detachNode(Node* node)
{
    for (int i = 0; i < nodes.size(); i++)
    {
        if (nodes[i].get() == node)
        {
            nodes.erase(nodes.begin() + i);
            break;
        }
    }
}
