#include "ScaleManipulator.h"
#include <glm/gtx/transform.hpp>
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/intersect.hpp"
#include <algorithm>
#include <iostream>
ScaleManipulator::ScaleManipulator(glm::vec3 dirL)
{
    std::unique_ptr<Mesh> meshPtr = std::make_unique<Mesh>(getScaleArrow(3.2, 0.01, 20));
    Manipulator::attachMesh(std::move(meshPtr));
    startPointL = glm::vec3(0.0f);
    setDirection(dirL);
}

void ScaleManipulator::handleMovement(MovementType movementType, const Viewport& viewport, double x, double y)
{
    if ((x > viewport.getWidth()) || (x < 0) || (y > viewport.getHeight()) || (y < 0))
        return;

    glm::mat4 trfMat = calcAbsoluteTransform();
    glm::vec3 dirW = glm::normalize(trfMat * glm::vec4(0, 1, 0, 0));
    glm::vec3 dirWOrig = trfMat * glm::vec4(0, 0, 0, 1);
    ray cursorRayW = viewport.calcCursorRay(x, y);

    glm::vec3 tempPointW = glm::vec3(0.0f);
    constexpr float tolerance = std::numeric_limits<float>::epsilon();
    glm::vec3 orien = glm::cross(cursorRayW.dir, dirW);

    if (glm::dot(orien, orien) > tolerance)
    {
        closestPointToSecondRay({ dirWOrig, dirW }, cursorRayW, tempPointW);
    }
    else
    {/*
        glm::vec3 up = glm::normalize(glm::cross(viewport.getCamera().calcRight(), viewport.getCamera().calcForward()));
        glm::mat4 rotationToUp = glm::orientation(up, dirW);
        dirWOrig = rotationToUp * glm::vec4(dirWOrig, 1.0f);
        dirW = rotationToUp * glm::vec4(dirW, 0.0f);
        closestPointToSecondRay({ dirWOrig, dirW }, cursorRayW, tempPointW);
        tempPointW = glm::inverse(rotationToUp) * glm::vec4(tempPointW, 1.0f);
        isParallelMode = true;*/
    }

    if (movementType == MovementType::Push)
    {
        startPointL = glm::inverse(trfMat) * glm::vec4(tempPointW, 1.0f);
    }
    else
    {
        glm::vec3 closestPointL = glm::inverse(trfMat) * glm::vec4(tempPointW, 1.0f);
        glm::vec3 deltaVec = glm::mat3(trfMat) * (closestPointL - startPointL);
        std::cout << deltaVec.x << " " << deltaVec.y << " " << deltaVec.z << "\n";
        float delta = (1.0f - (glm::length(deltaVec))/5.0f);

        if (glm::dot(deltaVec, dirW) > tolerance)
            delta = 1.0f / delta;
        
        glm::mat4 deltaMat = scaleInArbitaryDirection(dirW, delta);
        glm::mat4 transMat = getTranslationComponent(getRelativeTransform());
        sendFeedback(transMat * deltaMat * glm::inverse(transMat));
    }
}

void ScaleManipulator::setDirection(glm::vec3 inDirL)
{
    dirL = inDirL;
    glm::vec3 originArrowDir = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 newRelativeTransform = glm::mat4(1.0f);

    if (std::fabs(glm::dot(glm::normalize(inDirL), originArrowDir) + 1.0f) < std::numeric_limits<float>::epsilon())
        newRelativeTransform = glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));
    else
        newRelativeTransform = glm::orientation(glm::normalize(inDirL), originArrowDir);

    Manipulator::setRelativeTransform(newRelativeTransform);
}

bool ScaleManipulator::closestPointToSecondRay(ray firstRay, ray secondRay, glm::vec3& closestPoint)
{
    double raysDot = glm::dot(firstRay.dir, secondRay.dir);
    double tolerance = std::numeric_limits<double>::epsilon();

    if (raysDot > -tolerance && raysDot < tolerance)
        return false; // rays are parallel

    glm::vec3 perpToRaysPlane = glm::cross(firstRay.dir, secondRay.dir);
    // Compute the normal of a plane parallel to firstRay
    glm::vec3 planeNormal = glm::normalize(glm::cross(secondRay.dir, perpToRaysPlane));
    // Find the intersection between secondRay and a plane parallel to firstRay:
    // First, compute the distance from ray origin to the ray-plane intersection point
    double t = glm::dot((secondRay.orig - firstRay.orig), planeNormal) / glm::dot(firstRay.dir, planeNormal);
    // Second, compute ray-plane intersection point
    closestPoint = firstRay.orig + static_cast<float>(t) * firstRay.dir;

    return true;
}

HalfEdge::HalfEdgeTable ScaleManipulator::getCube(double cubeSideLength)
{
    HalfEdge::HalfEdgeTable halfEdgeTable;

    if (cubeSideLength > 0)
    {
        double halfEdge = cubeSideLength / 2.0;
        std::vector<HalfEdge::VertexHandle> cubeVertices;

        cubeVertices.emplace_back(halfEdgeTable.addVertex({ halfEdge, -halfEdge,  halfEdge }));
        cubeVertices.emplace_back(halfEdgeTable.addVertex({ halfEdge, -halfEdge, -halfEdge }));
        cubeVertices.emplace_back(halfEdgeTable.addVertex({ -halfEdge, -halfEdge, -halfEdge }));
        cubeVertices.emplace_back(halfEdgeTable.addVertex({ -halfEdge, -halfEdge,  halfEdge }));
        cubeVertices.emplace_back(halfEdgeTable.addVertex({ halfEdge,  halfEdge,  halfEdge }));
        cubeVertices.emplace_back(halfEdgeTable.addVertex({ halfEdge,  halfEdge, -halfEdge }));
        cubeVertices.emplace_back(halfEdgeTable.addVertex({ -halfEdge,  halfEdge, -halfEdge }));
        cubeVertices.emplace_back(halfEdgeTable.addVertex({ -halfEdge,  halfEdge,  halfEdge }));

        std::vector<int> indices = { 4, 5, 6, 7,   // top
                                     3, 7, 6, 2,   // left
                                     2, 6, 5, 1,   // back
                                     1, 0, 3, 2,   // bottom
                                     0, 4, 7, 3,   // front
                                     1, 5, 4, 0 }; // right

        for (int i = 0; i < indices.size(); i += 4)
        {
            halfEdgeTable.addFace(cubeVertices[indices[i]],
                cubeVertices[indices[i + 1]],
                cubeVertices[indices[i + 2]],
                cubeVertices[indices[i + 3]]);
        }

        halfEdgeTable.connectTwins();
    }

    return halfEdgeTable;
}

HalfEdge::HalfEdgeTable ScaleManipulator::getCylinder(double radius, double height, uint32_t numSubdivisions)
{
    HalfEdge::HalfEdgeTable halfEdgeTable;

    if (numSubdivisions > 2 && radius > 0 && height > 0)
    {
        std::vector<HalfEdge::VertexHandle> basesVertices;
        const double vertexStep = 2.0 * glm::pi<double>() / numSubdivisions;

        for (int i = 0; i < 2; ++i)
        {
            const double h = -height / 2.0 + i * height;

            for (int j = 0; j < numSubdivisions; ++j)
            {
                double angle = j * vertexStep;

                double x = std::cos(angle) * radius;
                double y = std::sin(angle) * radius;

                basesVertices.emplace_back(halfEdgeTable.addVertex({ y, h, x }));
            }
        }

        HalfEdge::VertexHandle bottomBaseCenterVertex = halfEdgeTable.addVertex({ 0, -height / 2.0, 0 });
        HalfEdge::VertexHandle topBaseCenterVertex = halfEdgeTable.addVertex({ 0, height / 2.0, 0 });

        for (int i = 0; i < numSubdivisions; ++i)
        {
            HalfEdge::VertexHandle firstTopVertex = basesVertices[numSubdivisions + i];
            HalfEdge::VertexHandle firstBottomVertex = basesVertices[i];

            HalfEdge::VertexHandle secondTopVertex = basesVertices[numSubdivisions];
            HalfEdge::VertexHandle secondBottomVertex = basesVertices[0];

            if ((i + 1) < numSubdivisions)
            {
                secondTopVertex = basesVertices[numSubdivisions + i + 1];
                secondBottomVertex = basesVertices[i + 1];
            }

            // Add the side faces of the cylinder
            halfEdgeTable.addFace(firstBottomVertex, secondBottomVertex, secondTopVertex, firstTopVertex);
            // Add the bottom triangles of the cylinder
            halfEdgeTable.addFace(bottomBaseCenterVertex, secondBottomVertex, firstBottomVertex);
            // Add the top triangles of the cylinder
            halfEdgeTable.addFace(topBaseCenterVertex, firstTopVertex, secondTopVertex);
        }

        halfEdgeTable.connectTwins();
    }

    return halfEdgeTable;
}

HalfEdge::HalfEdgeTable ScaleManipulator::getScaleArrow(double cylinderHeight, double cylinderRadius, uint32_t numSubdivisions)
{
    double coneHeight = cylinderRadius * 25.0;

    HalfEdge::HalfEdgeTable cylinderMesh = getCylinder(cylinderRadius, cylinderHeight, numSubdivisions);
    HalfEdge::HalfEdgeTable coneMesh = getCube(coneHeight);

    for (auto& vertex : cylinderMesh.getVertices())
    {
        HalfEdge::VertexHandle vh = cylinderMesh.handle(vertex);
        glm::vec3 newPoint = cylinderMesh.getPoint(vh);
        newPoint.y += (cylinderHeight / 2.0f);
        cylinderMesh.setPoint(vh, newPoint);
    }

    for (auto& vertex : coneMesh.getVertices())
    {
        HalfEdge::VertexHandle vh = coneMesh.handle(vertex);
        glm::vec3 newPoint = coneMesh.getPoint(vh);
        newPoint.y += cylinderHeight;
        coneMesh.setPoint(vh, newPoint);
    }

    cylinderMesh += coneMesh;

    return cylinderMesh;
}

glm::mat4 ScaleManipulator::scaleInArbitaryDirection(glm::vec3 dir, float d)
{
    glm::mat4 mat = glm::mat4(1.0f);

    mat[0] = glm::vec4(1.0 + (d - 1.0) * (dir.x * dir.x), (d - 1.0) * dir.x * dir.y, (d - 1.0) * dir.x * dir.z, 0.0f);
    mat[1] = glm::vec4((d - 1.0) * dir.x * dir.y, 1.0 + (d - 1.0) * (dir.y * dir.y), (d - 1.0) * dir.y * dir.z, 0.0f);
    mat[2] = glm::vec4((d - 1.0) * dir.x * dir.z, (d - 1.0) * dir.y * dir.z, 1.0 + (d - 1.0) * (dir.z * dir.z), 0.0f);

    return mat;
}

glm::mat4 ScaleManipulator::getTranslationComponent(glm::mat4 mat)
{
    glm::vec4 translation = mat[3];
    glm::mat4 resMat = glm::mat4(1.0f);
    resMat[3] = translation;

    return resMat;
}
