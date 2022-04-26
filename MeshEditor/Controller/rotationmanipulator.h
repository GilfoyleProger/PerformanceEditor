#include "Manipulator.h"

class RotationManipulator : public Manipulator
{
public:
    RotationManipulator(glm::vec3 dirL);
    void handleMovement(MovementType movementType, const Viewport& viewport, double x, double y) override;
    void setDirection(glm::vec3 inDirL);
private:
    glm::vec3 startPointL;
    glm::vec3 dirL;

    HalfEdge::HalfEdgeTable getTorus(double minorRadius, double majorRadius, uint32_t majorSegments);
    bool rayPlaneIntersection(ray inRay, glm::vec3 planeNormal, glm::vec3 planePoint, glm::vec3& intersectionPoint);
    glm::mat4 getTranslationComponent(glm::mat4 matToDecompose);
};
