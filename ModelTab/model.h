#pragma once

#include "node.h"

class Model
{
public:
    Model();

    void attachNode(std::unique_ptr<Node> node);
    const std::vector<std::unique_ptr<Node>>& getNodes() const;
    void detachNode(Node* node);
private:
    std::vector<std::unique_ptr<Node>> nodes;
};
