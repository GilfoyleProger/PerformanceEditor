#pragma once

#include "Manipulator.h"

class ScaleManipulator : public Manipulator
{
public:
    ScaleManipulator(glm::vec3 dirL);
    void handleMovement(MovementType movementType, const Viewport& viewport, double x, double y) override;
    void setDirection(glm::vec3 inDirL);
private:
    glm::vec3 startPointL;
    glm::vec3 dirL;

    bool closestPointToSecondRay(ray firstRay, ray secondRay, glm::vec3& closestPoint);
    HalfEdge::HalfEdgeTable getCube(double cubeSideLength);
    HalfEdge::HalfEdgeTable getCylinder(double radius, double height, uint32_t numSubdivisions);
    HalfEdge::HalfEdgeTable getScaleArrow(double cylinderHeight, double cylinderRadius, uint32_t numSubdivisions);

    glm::mat4 scaleInArbitaryDirection(glm::vec3 dir, float d);
    glm::mat4 getTranslationComponent(glm::mat4 mat);
};
