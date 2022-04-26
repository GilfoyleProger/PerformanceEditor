#include "pointlight.h"

PointLight::PointLight()
{
    std::unique_ptr<Mesh> meshPtr = std::make_unique<Mesh>(createSphere(glm::vec3(0,0,0), 1, 18, 9));
    Node::attachMesh(std::move(meshPtr));
}
