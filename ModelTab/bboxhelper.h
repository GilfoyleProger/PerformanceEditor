#pragma once

//#include "viewport.h"
#include "Model.h"
#include <vector>
struct ray
{
    glm::vec3 orig;
    glm::vec3 dir{ 0,0,1 };
};
namespace BboxHelper
{
    //void formNodeList(std::vector<Node*>& nodeList, const std::vector<std::unique_ptr<Node>>& nodePtrList);
    void formNodeList(std::vector<Node*> & nodeList, const std::vector<std::unique_ptr<Node>> & nodePtrList);
    std::vector<Node*> getNodeList(const Model & model);
    std::vector<Node*> getNodeList( Node* node);
    std::vector<std::pair<Node*, bbox>> getNodeToBboxList(const Model & model);
    bbox getBbox(const Node & node);
    bbox calcNodesBoundingBox(const std::vector<Node*> & nodeList);
    bbox calcModelBoundingBox(const Model & model);
  //  bbox calcFaceBoundingBox(const HalfEdge::FaceHandle& fh, Node * node);
    bbox combineBoxes(const bbox & a, const bbox & b);
    bool rayIntersectBox(const ray& rayToIntersect, const bbox& boundaryBox);
    bool boxInsideBox(const bbox & a, const bbox & b);
    bool pointInsideBox(const glm::vec3& point, const bbox& box);
    bool lessThanEqual(const double& a, const double& b);
    bool greaterThanEqual(const double& a, const double& b);
    bool areEqual(const double& a, const double& b);
    bool lessThan(const double& a, const double& b);
    bool greaterThan(const double& a, const double& b);
}
