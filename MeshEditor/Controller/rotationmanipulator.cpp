#include "RotationManipulator.h"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/intersect.hpp"
#include <algorithm>

RotationManipulator::RotationManipulator(glm::vec3 dirL)
{
    std::unique_ptr<Mesh> meshPtr = std::make_unique<Mesh>(getTorus(0.08, 2, 30));
    Manipulator::attachMesh(std::move(meshPtr));
    setDirection(dirL);
}

void RotationManipulator::handleMovement(MovementType movementType, const Viewport& viewport, double x, double y)
{
    if ((x > viewport.getWidth()) || (x < 0) || (y > viewport.getHeight()) || (y < 0))
        return;

    glm::mat4 trfMat = calcAbsoluteTransform();
    glm::vec3 dirW = glm::normalize(glm::mat3(glm::transpose(glm::inverse(calcAbsoluteTransform()))) * glm::vec3(0, 1, 0));
    glm::vec3 dirWOrig = trfMat * glm::vec4(0, 0, 0, 1);
    ray cursorRayW = viewport.calcCursorRay(x, y);

    glm::vec3 tempPointW = glm::vec3(0.0f);
    float tolerance = std::numeric_limits<float>::epsilon();

    if (std::fabs(glm::dot(cursorRayW.dir, dirW)) > 0.1f)
    {
        rayPlaneIntersection(cursorRayW, dirW, dirWOrig, tempPointW);
    }
    else
    {
        glm::vec3 torusStartPoint = dirWOrig;
        glm::vec3 torusEndPoint = trfMat * glm::vec4(0, 2, 0, 1);
        float sphereRadius = glm::length(torusStartPoint - torusEndPoint);
        glm::vec3 intersectNormal;

        if (glm::intersectRaySphere(cursorRayW.orig, cursorRayW.dir, torusStartPoint, sphereRadius, tempPointW, intersectNormal))
            tempPointW = tempPointW - (glm::dot(dirW, (tempPointW - torusStartPoint))) * dirW;
        else
            return;
    }

    if (movementType == MovementType::Push)
    {
        startPointL = glm::inverse(trfMat) * glm::vec4(tempPointW, 1.0f);
    }
    else
    {
        glm::mat4 transMat = getTranslationComponent(getRelativeTransform());
        glm::vec3 closestPointL = glm::inverse(trfMat) * glm::vec4(tempPointW, 1.0f);
        float sign = (glm::dot(glm::cross(startPointL, closestPointL), glm::vec3(0, 1, 0)) > tolerance) ? 1.0f : -1.0f;
        float cosine = std::min(std::fabs(glm::dot(glm::normalize(startPointL), glm::normalize(closestPointL))), 1.0f);
        float angle = sign * std::acos(cosine);
        glm::mat4 deltaMat = transMat * glm::rotate(glm::mat4(1.0f), angle, dirW) * glm::inverse(transMat);
        sendFeedback(deltaMat);
    }
}

void RotationManipulator::setDirection(glm::vec3 inDirL)
{
    dirL = inDirL;
    glm::vec3 originTorusDir = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 newRelativeTransform = glm::mat4(1.0f);

    if (std::fabs(glm::dot(glm::normalize(inDirL), originTorusDir) + 1.0f) < std::numeric_limits<float>::epsilon())
        newRelativeTransform = glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));
    else
        newRelativeTransform = glm::orientation(glm::normalize(inDirL), originTorusDir);

    Manipulator::applyRelativeTransform(newRelativeTransform);
}

HalfEdge::HalfEdgeTable RotationManipulator::getTorus(double minorRadius, double majorRadius, uint32_t majorSegments)
{
    HalfEdge::HalfEdgeTable halfEdgeTable;

    if (minorRadius > 0 && majorRadius > 0 && majorSegments > 2)
    {
        uint32_t minorSegments = majorSegments;

        std::vector<HalfEdge::VertexHandle> torusVertices;
        const double majorSegmentAngleStep = 2.0 * glm::pi<double>() / majorSegments;
        const double minorSegmentAngleStep = 2.0 * glm::pi<double>() / majorSegments;

        for (int i = 0; i < majorSegments; ++i)
        {
            double cosMajorSegment = std::sin(i * majorSegmentAngleStep);
            double sinMajorSegment = std::cos(i * majorSegmentAngleStep);

            for (int j = 0; j < minorSegments; ++j)
            {
                double cosMinorSegment = std::sin(j * minorSegmentAngleStep);
                double sinMinorSegment = std::cos(j * minorSegmentAngleStep);

                double x = (majorRadius + minorRadius * cosMinorSegment) * cosMajorSegment;
                double y = (majorRadius + minorRadius * cosMinorSegment) * sinMajorSegment;
                double z = minorRadius * sinMinorSegment;

                torusVertices.emplace_back(halfEdgeTable.addVertex({ y, z, x }));
            }
        }

        for (int i = 0; i < majorSegments; ++i)
        {
            int firstCircleIndex = minorSegments * i;
            int secondCircleIndex = ((i + 1) < majorSegments) ? firstCircleIndex + minorSegments : 0;

            for (int j = 0; j < minorSegments; ++j)
            {
                HalfEdge::VertexHandle firstVertexOfFirstCircle = torusVertices[firstCircleIndex + j];
                HalfEdge::VertexHandle secondVertexOfFirstCircle = torusVertices[firstCircleIndex];

                HalfEdge::VertexHandle firstVertexOfSecondCircle = torusVertices[secondCircleIndex + j];
                HalfEdge::VertexHandle secondVertexOfSecondCircle = torusVertices[secondCircleIndex];

                if ((j + 1) < minorSegments)
                {
                    secondVertexOfFirstCircle = torusVertices[firstCircleIndex + j + 1];
                    secondVertexOfSecondCircle = torusVertices[secondCircleIndex + j + 1];
                }

                halfEdgeTable.addFace(firstVertexOfFirstCircle,
                    firstVertexOfSecondCircle,
                    secondVertexOfSecondCircle,
                    secondVertexOfFirstCircle);
            }
        }

        halfEdgeTable.connectTwins();
    }
    return halfEdgeTable;
}

bool RotationManipulator::rayPlaneIntersection(ray inRay, glm::vec3 planeNormal, glm::vec3 planePoint, glm::vec3& intersectionPoint)
{
    inRay.dir = glm::normalize(inRay.dir);
    planeNormal = glm::normalize(planeNormal);

    // assuming vectors are all normalized
    double denom = glm::dot(inRay.dir, planeNormal);
    double tolerance = std::numeric_limits<double>::epsilon();

    if (denom > -tolerance && denom < tolerance)
        return false;

    glm::vec3 p0l0 = planePoint - inRay.orig;
    float t = glm::dot(p0l0, planeNormal) / denom;
    intersectionPoint = inRay.orig + t * inRay.dir;

    return true;
}

glm::mat4 RotationManipulator::getTranslationComponent(glm::mat4 mat)
{
    glm::vec4 translation = mat[3];
    glm::mat4 resMat = glm::mat4(1.0f);
    resMat[3] = translation;

    return resMat;
}
