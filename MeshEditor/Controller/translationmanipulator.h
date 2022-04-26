#pragma once

#include "Manipulator.h"

class TranslationManipulator : public Manipulator
{
public:
    TranslationManipulator(glm::vec3 dirL);
    void handleMovement(MovementType movementType, const Viewport& viewport, double x, double y) override;
    void setDirection(glm::vec3 inDirL);
private:
    glm::vec3 startPointL;
    glm::vec3 dirL;
    bool isParallelMode = false;

    bool closestPointToSecondRay(ray firstRay, ray secondRay, glm::vec3& closestPoint);
    HalfEdge::HalfEdgeTable getCone(double radius, double height, uint32_t numSubdivisions);
    HalfEdge::HalfEdgeTable getCylinder(double radius, double height, uint32_t numSubdivisions);
    HalfEdge::HalfEdgeTable getArrow(double shaftHeight, double shaftRadius, uint32_t numSubdivisions);
};
