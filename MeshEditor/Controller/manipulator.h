#pragma once
#include "View/viewport.h"
#include "Node.h"

#include <functional>

enum class MovementType { Push, Drag, Release };

class Manipulator : public Node
{
public:
    void setColor(glm::vec3 inColor) 
    {
        color = inColor;
    }
    glm::vec3 getColor()
    {
        return color;
    }
    virtual void handleMovement(MovementType movementType, const Viewport& viewport, double x, double y) {}

    void setCallback(const std::function<void(const glm::mat4&)>& inCallback);
protected:
    void sendFeedback(const glm::mat4& deltaMatrix);
private:
    std::function<void(const glm::mat4&)> callback;
    glm::vec3 color{0,0,0};
};
