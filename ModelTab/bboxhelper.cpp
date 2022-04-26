#include "bboxhelper.h"
#include <algorithm>

void BboxHelper::formNodeList(std::vector<Node*> & nodeList, const std::vector<std::unique_ptr<Node>> & nodePtrList)
{
    for (const auto& nodePtr : nodePtrList)
    {
        if (!nodePtr->getMeshes().empty())
            nodeList.emplace_back(nodePtr.get());
        formNodeList(nodeList, nodePtr->getChildren());
    }
}

std::vector<Node*> BboxHelper::getNodeList(const Model & model)
{
    std::vector<Node*> nodeList;
    formNodeList(nodeList, model.getNodes());
    return nodeList;
}

std::vector<Node*> BboxHelper::getNodeList( Node* node)
{
    std::vector<Node*> nodeList;
    nodeList.push_back(node);
    formNodeList(nodeList, node->getChildren());
    return nodeList;
}

std::vector<std::pair<Node*, bbox>> BboxHelper::getNodeToBboxList(const Model & model)
{
    std::vector<Node*> nodeList = getNodeList(model);
    std::vector<std::pair<Node*, bbox>> nodeToBboxList;

    for (const auto& node : nodeList)
    {
        if (!node->getMeshes().empty())
            nodeToBboxList.emplace_back(node, getBbox(*node));
    }

    return nodeToBboxList;
}




bbox BboxHelper::getBbox(const Node& node)
{
    bbox computedBBOX = bbox{ glm::vec3(0.0f), glm::vec3(0.0f) };
for(auto& mesh:node.getMeshes()){

        auto& halfEdgeTable = mesh->getHalfEdgeTable();
        auto& vertices = halfEdgeTable.getVertices();

        glm::mat4 absoluteTransform = node.calcAbsoluteTransform();
        computedBBOX.min = absoluteTransform * glm::vec4(halfEdgeTable.getPoint(halfEdgeTable.handle(vertices[0])), 1.0f);
        computedBBOX.max = absoluteTransform * glm::vec4(halfEdgeTable.getPoint(halfEdgeTable.handle(vertices[0])), 1.0f);

        for (const auto& vertex : vertices)
        {
            HalfEdge::VertexHandle vh = halfEdgeTable.handle(vertex);
            glm::vec3 point = absoluteTransform * glm::vec4(halfEdgeTable.getPoint(vh), 1.0f);

            // x-axis
            if (BboxHelper::lessThan(point.x, computedBBOX.min.x))
                computedBBOX.min.x = point.x;
            if (BboxHelper::greaterThan(point.x, computedBBOX.max.x))
                computedBBOX.max.x = point.x;
            // y-axis
            if (BboxHelper::lessThan(point.y, computedBBOX.min.y))
                computedBBOX.min.y = point.y;
            if (BboxHelper::greaterThan(point.y, computedBBOX.max.y))
                computedBBOX.max.y = point.y;
            // z-axis
            if (BboxHelper::lessThan(point.z, computedBBOX.min.z))
                computedBBOX.min.z = point.z;
            if (BboxHelper::greaterThan(point.z, computedBBOX.max.z))
                computedBBOX.max.z = point.z;
        }

}

    return computedBBOX;
}


bbox BboxHelper::calcNodesBoundingBox(const std::vector<Node*> & nodeList)
{
    bbox computedBBOX = bbox();

    if (!nodeList.empty())
    {
        computedBBOX = nodeList[0]->getWorldBbox();

        for (int i = 1; i < nodeList.size(); i++)
        {
            bbox boundingBox = nodeList[i]->getWorldBbox();

            // x-axis
            if (lessThan(boundingBox.min.x, computedBBOX.min.x))
                computedBBOX.min.x = boundingBox.min.x;
            if (greaterThan(boundingBox.max.x, computedBBOX.max.x))
                computedBBOX.max.x = boundingBox.max.x;
            // y-axis
            if (lessThan(boundingBox.min.y, computedBBOX.min.y))
                computedBBOX.min.y = boundingBox.min.y;
            if (greaterThan(boundingBox.max.y, computedBBOX.max.y))
                computedBBOX.max.y = boundingBox.max.y;
            // z-axis
            if (lessThan(boundingBox.min.z, computedBBOX.min.z))
                computedBBOX.min.z = boundingBox.min.z;
            if (greaterThan(boundingBox.max.z, computedBBOX.max.z))
                computedBBOX.max.z = boundingBox.max.z;
        }

        float offset = 0.01f;
        computedBBOX.min += (glm::normalize(computedBBOX.min - computedBBOX.max) * offset);
        computedBBOX.max += (glm::normalize(computedBBOX.max - computedBBOX.min) * offset);
    }

    return computedBBOX;
    /*
    bbox computedBBOX = bbox();

    if (!nodeList.empty())
    {
        computedBBOX = getBbox(*nodeList[0]);

        for (int i = 1; i < nodeList.size(); i++)
        {
            bbox boundingBox = getBbox(*nodeList[i]);

            // x-axis
            if (lessThan(boundingBox.min.x, computedBBOX.min.x))
                computedBBOX.min.x = boundingBox.min.x;
            if (greaterThan(boundingBox.max.x, computedBBOX.max.x))
                computedBBOX.max.x = boundingBox.max.x;
            // y-axis
            if (lessThan(boundingBox.min.y, computedBBOX.min.y))
                computedBBOX.min.y = boundingBox.min.y;
            if (greaterThan(boundingBox.max.y, computedBBOX.max.y))
                computedBBOX.max.y = boundingBox.max.y;
            // z-axis
            if (lessThan(boundingBox.min.z, computedBBOX.min.z))
                computedBBOX.min.z = boundingBox.min.z;
            if (greaterThan(boundingBox.max.z, computedBBOX.max.z))
                computedBBOX.max.z = boundingBox.max.z;
        }

        float offset = 0.01f;
        computedBBOX.min += (glm::normalize(computedBBOX.min - computedBBOX.max) * offset);
        computedBBOX.max += (glm::normalize(computedBBOX.max - computedBBOX.min) * offset);
    }

    return computedBBOX;
    */
}

bbox BboxHelper::calcModelBoundingBox(const Model & model)
{
    std::vector<Node*> nodeList = getNodeList(model);
    return calcNodesBoundingBox(nodeList);
}
/*
bbox BboxHelper::calcFaceBoundingBox(const HalfEdge::FaceHandle & fh, Node * node)
{
    
    auto& halfEdgeTable = node->getMesh()->getHalfEdgeTable();
    HalfEdge::HalfEdgeHandle startHeh = halfEdgeTable.deref(fh).heh;
    HalfEdge::HalfEdgeHandle nextHeh = startHeh;
    std::vector<glm::vec3> vertices;
    glm::mat4 absoluteTransform = node->calcAbsoluteTransform();
    do
    {
        glm::vec3 point = halfEdgeTable.getStartPoint(nextHeh);
        vertices.emplace_back(absoluteTransform * glm::vec4(point, 1));
        nextHeh = halfEdgeTable.next(nextHeh);
    } while (startHeh.index != nextHeh.index);


    bbox computedBBOX = bbox{ glm::vec3(0.0f), glm::vec3(0.0f) };
    computedBBOX.min = absoluteTransform * glm::vec4(vertices[0], 1.0f);
    computedBBOX.max = absoluteTransform * glm::vec4(vertices[0], 1.0f);

    for (const auto& vertex : vertices)
    {
        glm::vec3 point = absoluteTransform * glm::vec4(vertex, 1.0f);

        // x-axis
        if (BboxHelper::lessThan(point.x, computedBBOX.min.x))
            computedBBOX.min.x = point.x;
        if (BboxHelper::greaterThan(point.x, computedBBOX.max.x))
            computedBBOX.max.x = point.x;
        // y-axis
        if (BboxHelper::lessThan(point.y, computedBBOX.min.y))
            computedBBOX.min.y = point.y;
        if (BboxHelper::greaterThan(point.y, computedBBOX.max.y))
            computedBBOX.max.y = point.y;
        // z-axis
        if (BboxHelper::lessThan(point.z, computedBBOX.min.z))
            computedBBOX.min.z = point.z;
        if (BboxHelper::greaterThan(point.z, computedBBOX.max.z))
            computedBBOX.max.z = point.z;
    }

    bbox computedBBOX = bbox{ glm::vec3(0.0f), glm::vec3(0.0f) };
    return computedBBOX;
}
*/
bool BboxHelper::rayIntersectBox(const ray& rayToIntersect, const bbox& boundaryBox)
{
    glm::vec3 invDir = 1.0f / rayToIntersect.dir;

    double tmin = (boundaryBox.min.x - rayToIntersect.orig.x) * invDir.x;
    double tmax = (boundaryBox.max.x - rayToIntersect.orig.x) * invDir.x;

    if (greaterThan(tmin, tmax))
        std::swap(tmin, tmax);

    double tymin = (boundaryBox.min.y - rayToIntersect.orig.y) * invDir.y;
    double tymax = (boundaryBox.max.y - rayToIntersect.orig.y) * invDir.y;

    if (greaterThan(tymin, tymax))
        std::swap(tymin, tymax);

    if (greaterThan(tmin, tymax) || greaterThan(tymin, tmax))
        return false;

    if (greaterThan(tymin, tmin))
        tmin = tymin;

    if (lessThan(tymax, tmax))
        tmax = tymax;

    double tzmin = (boundaryBox.min.z - rayToIntersect.orig.z) * invDir.z;
    double tzmax = (boundaryBox.max.z - rayToIntersect.orig.z) * invDir.z;

    if (greaterThan(tzmin, tzmax))
        std::swap(tzmin, tzmax);

    if (greaterThan(tmin, tzmax) || greaterThan(tzmin, tmax))
        return false;

    if (greaterThan(tzmin, tmin))
        tmin = tzmin;

    if (lessThan(tzmax, tmax))
        tmax = tzmax;

    double t = tmin;
    if (t < -std::numeric_limits<double>::epsilon())
    {
        t = tmax;
        if (t < -std::numeric_limits<double>::epsilon())
            return false;
    }

    return true;
}

bool BboxHelper::boxInsideBox(const bbox & a, const bbox & b)
{
    return pointInsideBox(a.min, b) && pointInsideBox(a.max, b);
}

bool BboxHelper::pointInsideBox(const glm::vec3 & point, const bbox & box)
{
    return greaterThanEqual(point.x, box.min.x) && lessThanEqual(point.x, box.max.x) &&
           greaterThanEqual(point.y, box.min.y) && lessThanEqual(point.y, box.max.y) &&
           greaterThanEqual(point.z, box.min.z) && lessThanEqual(point.z, box.max.z);
}

bbox BboxHelper::combineBoxes(const bbox & a, const bbox & b)
{
    bbox computedBBOX = a;

    // x-axis
    if (lessThan(b.min.x, computedBBOX.min.x))
        computedBBOX.min.x = b.min.x;
    if (greaterThan(b.max.x, computedBBOX.max.x))
        computedBBOX.max.x = b.max.x;
    // y-axis
    if (lessThan(b.min.y, computedBBOX.min.y))
        computedBBOX.min.y = b.min.y;
    if (greaterThan(b.max.y, computedBBOX.max.y))
        computedBBOX.max.y = b.max.y;
    // z-axis
    if (lessThan(b.min.z, computedBBOX.min.z))
        computedBBOX.min.z = b.min.z;
    if (greaterThan(b.max.z, computedBBOX.max.z))
        computedBBOX.max.z = b.max.z;

    return computedBBOX;
}

bool BboxHelper::lessThanEqual(const double & a, const double & b)
{
    return lessThan(a, b) || areEqual(a, b);
}

bool BboxHelper::greaterThanEqual(const double & a, const double & b)
{
    return greaterThan(a, b) || areEqual(a, b);
}

bool BboxHelper::areEqual(const double & a, const double & b)
{
    return std::abs(a - b) <= std::numeric_limits<double>::epsilon();
}

bool BboxHelper::lessThan(const double & a, const double & b)
{
    return (a - b) < -std::numeric_limits<double>::epsilon();
}

bool BboxHelper::greaterThan(const double & a, const double & b)
{
    return (a - b) > std::numeric_limits<double>::epsilon();
}
