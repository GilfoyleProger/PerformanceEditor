#include "Manipulator.h"

void Manipulator::setCallback(const std::function<void(const glm::mat4&)>& inCallback)
{
    callback = inCallback;
}

void Manipulator::sendFeedback(const glm::mat4& deltaMatrix)
{
    auto root = dynamic_cast<Manipulator*>(getParent());

    if (root)
    {
        for (auto& sn : root->Node::getChildren())
            sn->applyRelativeTransform(deltaMatrix);

        if (root->callback)
            root->callback(deltaMatrix);
    }
    else
    {
        Node::applyRelativeTransform(deltaMatrix);
        if (callback)
            callback(deltaMatrix);
    }
}
