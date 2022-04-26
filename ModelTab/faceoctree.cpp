#include "FaceOctree.h"

FaceOctree::FaceOctree(Node* inNode, Mesh*inMesh)
{
    node = inNode;
    mesh = inMesh;
    rootOctreeNode = std::make_unique<OctreeNode>();
    rootOctreeNode->childrens.resize(8);
    rootBoundingBox = inMesh->getBoundingBox(inNode->calcAbsoluteTransform());//node->getWorldBbox();
    threshold = 100;
    maxDepth = 10;

    auto& halfEdgeTable = mesh->getHalfEdgeTable();
    for (const auto& face : halfEdgeTable.getFaces())
        addFace(halfEdgeTable.handle(face));
}

bool FaceOctree::isLeaf(OctreeNode* octreeNode)
{
    return (octreeNode->childrens[0] == nullptr);
}

void FaceOctree::addFace(const HalfEdge::FaceHandle& fh)
{
    addFace(rootOctreeNode.get(), 0, rootBoundingBox, fh);
}

void FaceOctree::removeFace(const HalfEdge::FaceHandle& fh)
{
    removeFace(rootOctreeNode.get(), fh);
}

void FaceOctree::addFace(OctreeNode* octreeNode, int depth, const bbox& box, const HalfEdge::FaceHandle& fh)
{
    if (isLeaf(octreeNode))
    {
        if (depth >= maxDepth || octreeNode->values.size() < threshold)
        {
            octreeNode->values.emplace(fh.index);
        }
        else
        {
            split(octreeNode, box);
            addFace(octreeNode, depth, box, fh);
        }
    }
    else
    {
        OctantPlace octantPlace = getOctant(box, fh);

        if (octantPlace != OctantPlace::Indefinite)
            addFace(octreeNode->childrens[static_cast<int>(octantPlace)].get(), depth + 1, computeOctantBox(box, octantPlace), fh);
        else
            octreeNode->values.emplace(fh.index);
    }
}

void FaceOctree::removeFace(OctreeNode* octreeNode, const HalfEdge::FaceHandle& fh)
{
    if (octreeNode != nullptr)
    {
        removeValue(octreeNode, fh);

        if (!isLeaf(octreeNode))
        {
            for (auto& child : octreeNode->childrens)
                removeFace(child.get(), fh);
        }
    }
}

void FaceOctree::updateFace(const HalfEdge::FaceHandle& fh)
{
    auto& halfEdgeTable = mesh->getHalfEdgeTable();

    std::vector<HalfEdge::VertexHandle> faceVertices;
    HalfEdge::HalfEdgeHandle startHeh = halfEdgeTable.deref(fh).heh;
    HalfEdge::HalfEdgeHandle nextHeh = startHeh;
    do
    {
        faceVertices.emplace_back(halfEdgeTable.sourceVertex(nextHeh));
        nextHeh = halfEdgeTable.next(nextHeh);
    } while (nextHeh.index != startHeh.index);

    std::set<int> adjacentFaces;
    adjacentFaces.emplace(fh.index);

    for (auto& vertex : faceVertices)
    {
        HalfEdge::HalfEdgeHandle startHeh = halfEdgeTable.deref(vertex).heh;
        HalfEdge::HalfEdgeHandle nextHeh = startHeh;
        do
        {
            adjacentFaces.emplace(halfEdgeTable.deref(nextHeh).fh.index);
            nextHeh = halfEdgeTable.next(halfEdgeTable.twin(nextHeh));
        } while (nextHeh.index != startHeh.index);
    }

    for (auto& faceIndex : adjacentFaces)
    {
        HalfEdge::FaceHandle faceHandle = halfEdgeTable.handle(halfEdgeTable.getFaces()[faceIndex]);
        removeFaceByValue(rootOctreeNode.get(), faceHandle);
        addFace(faceHandle);
    }
}

void FaceOctree::removeValue(OctreeNode* octreeNode, const HalfEdge::FaceHandle& fh)
{
    const int meshFacesCount = mesh->getHalfEdgeTable().getFaces().size();
    std::set<int> newFaces;
    for (auto& face : octreeNode->values)
    {
        if (face != fh.index)
        {
            if (face > fh.index)
                newFaces.insert(newFaces.end(), face - 1);
            else
                newFaces.insert(newFaces.end(), face);
        }
    }
    octreeNode->values = newFaces;
}

void FaceOctree::tryMerge(OctreeNode* octreeNode)
{
    if (octreeNode != nullptr)
    {
        if (!isLeaf(octreeNode))
        {
            int trianglesCount = octreeNode->values.size();
            for (const auto& child : octreeNode->childrens)
            {
                if (!isLeaf(child.get()))
                    return;
                trianglesCount += child->values.size();
            }
            if (trianglesCount <= threshold)
            {
                for (const auto& child : octreeNode->childrens)
                {
                    for (const auto& node : child->values)
                        octreeNode->values.emplace(node);
                    child->values.clear();
                }
            }
        }
    }
}

void FaceOctree::split(OctreeNode* octreeNode, const bbox& box)
{
    if (octreeNode != nullptr)
    {
        if (isLeaf(octreeNode))
        {
            for (auto& child : octreeNode->childrens)
            {
                child = std::make_unique<OctreeNode>();
                child->childrens.resize(8);

            }
            std::set<int> newFaces;

            for (auto& faceIndex : octreeNode->values)
            {
                auto& halfEdgeTable = mesh->getHalfEdgeTable();
                auto& face = halfEdgeTable.getFaces()[faceIndex];

                OctantPlace octantPlace = getOctant(box, halfEdgeTable.handle(face));

                if (octantPlace != OctantPlace::Indefinite)
                    octreeNode->childrens[static_cast<int>(octantPlace)]->values.emplace(faceIndex);
                else
                    newFaces.emplace(faceIndex);
            }
            octreeNode->values = newFaces;
        }
    }
}

OctantPlace FaceOctree::getOctant(const bbox& box, const HalfEdge::FaceHandle& fh)
{
    for (int i = 0; i < 8; i++)
    {
        OctantPlace octantPlace = static_cast<OctantPlace>(i);
        bbox computedBox = computeOctantBox(box, octantPlace);

        if (faceInsideBbox(fh, computedBox))
            return octantPlace;
    }
    return OctantPlace::Indefinite;
}

bbox FaceOctree::computeOctantBox(const bbox& parentOctant, OctantPlace octantPlace)
{
    // Each octant has its own number. Depending on the input octantNum will be calculated
    // size of corresponding octant
    glm::vec3 origin = (parentOctant.min + parentOctant.max) / 2.0f;

    if (octantPlace == OctantPlace::BottomLeftBack)
        return bbox{ parentOctant.min, origin };
    if (octantPlace == OctantPlace::BottomRightBack)
        return bbox{ {parentOctant.min.x, origin.y, parentOctant.min.z}, {origin.x, parentOctant.max.y, origin.z} };
    if (octantPlace == OctantPlace::BottomRightFront)
        return bbox{ {origin.x, origin.y, parentOctant.min.z}, {parentOctant.max.x, parentOctant.max.y, origin.z} };
    if (octantPlace == OctantPlace::BottomLeftFront)
        return bbox{ {origin.x, parentOctant.min.y, parentOctant.min.z}, {parentOctant.max.x, origin.y, origin.z} };
    if (octantPlace == OctantPlace::TopLeftBack)
        return bbox{ {parentOctant.min.x, parentOctant.min.y, origin.z}, {origin.x, origin.y, parentOctant.max.z} };
    if (octantPlace == OctantPlace::TopRightBack)
        return bbox{ {parentOctant.min.x, origin.y, origin.z}, {origin.x, parentOctant.max.y, parentOctant.max.z} };
    if (octantPlace == OctantPlace::TopRightFront)
        return bbox{ origin, parentOctant.max };
    if (octantPlace == OctantPlace::TopLeftFront)
        return bbox{ {origin.x, parentOctant.min.y, origin.z}, {parentOctant.max.x, origin.y, parentOctant.max.z} };
    if (octantPlace == OctantPlace::Indefinite)
        return bbox{ {0, 0, 0}, {0, 0, 0} };
}

bool FaceOctree::faceInsideBbox(const HalfEdge::FaceHandle& fh, const bbox& box)
{
    std::vector<Triangle> triangles = getTriangles(fh);

    for (auto& trg : triangles)
    {
        if (!triangleInsideBbox(trg, box))
            return false;
    }

    return true;
}

bool FaceOctree::triangleInsideBbox(const Triangle& triangle, const bbox& box)
{
    return BboxHelper::pointInsideBox(triangle.A, box) &&
        BboxHelper::pointInsideBox(triangle.B, box) &&
        BboxHelper::pointInsideBox(triangle.C, box);
}

std::vector<Triangle> FaceOctree::getTriangles(const HalfEdge::FaceHandle& fh)
{
    std::vector<glm::vec3> faceVertices;

    auto& halfEdgeTable = mesh->getHalfEdgeTable();
    HalfEdge::HalfEdgeHandle startHeh = halfEdgeTable.deref(fh).heh;
    HalfEdge::HalfEdgeHandle nextHeh = startHeh;
    do
    {
        faceVertices.emplace_back(node->calcAbsoluteTransform() * glm::vec4(halfEdgeTable.getStartPoint(nextHeh), 1.0f));
        nextHeh = halfEdgeTable.next(nextHeh);
    } while (nextHeh.index != startHeh.index);

    std::vector<Triangle> triangles;

    if (faceVertices.size() < 5) // Only quad or triangle
    {
        if (faceVertices.size() > 2)
            triangles.emplace_back(Triangle{ faceVertices[0], faceVertices[1], faceVertices[2] });

        if (faceVertices.size() == 4)
            triangles.emplace_back(Triangle{ faceVertices[2], faceVertices[3], faceVertices[0] });
    }

    return triangles;
}

void FaceOctree::intersectedQuery(const ray& ray, OctreeNode* octNode, const bbox& octNodeBox, std::vector<HalfEdge::FaceHandle>& triangles)
{
    if (isLeaf(octNode))
    {

        for (const auto& t : octNode->values)
        {
            auto& halfEdgeTable = mesh->getHalfEdgeTable();
            triangles.emplace_back(halfEdgeTable.handle(halfEdgeTable.getFaces()[t]));
        }
    }
    else
    {
        for (int i = 0; i < octNode->childrens.size(); i++)
        {
            bbox box = computeOctantBox(octNodeBox, static_cast<OctantPlace>(i));
            if (BboxHelper::rayIntersectBox(ray, box))
                intersectedQuery(ray, octNode->childrens[i].get(), box, triangles);
        }
        for (const auto& t : octNode->values)
        {
            auto& halfEdgeTable = mesh->getHalfEdgeTable();
            triangles.emplace_back(halfEdgeTable.handle(halfEdgeTable.getFaces()[t]));
        }
    }
}

void FaceOctree::removeFaceByValue(OctreeNode* octreeNode, const HalfEdge::FaceHandle& fh)
{
    if (node != nullptr)
    {
        auto iter = octreeNode->values.find(fh.index);
        if (iter != octreeNode->values.end())
        {
            octreeNode->values.erase(iter);
            return;
        }

        if (!isLeaf(octreeNode))
        {
            for (auto& child : octreeNode->childrens)
                removeFaceByValue(child.get(), fh);
        }
    }
}

void FaceOctree::collectAllValues(OctreeNode* octreeNode, std::vector<HalfEdge::FaceHandle>& values)
{
    auto& halfEdgeTable = mesh->getHalfEdgeTable();

    if (octreeNode != nullptr)
    {
        for (auto& value : octreeNode->values)
            values.emplace_back(halfEdgeTable.handle(halfEdgeTable.getFaces()[value]));

        if (!isLeaf(octreeNode))
            for (auto& child : octreeNode->childrens)
                collectAllValues(child.get(), values);
    }
}

std::vector<HalfEdge::FaceHandle> FaceOctree::getFacesOfIntersectedOctants(const ray& ray)
{
    std::vector<HalfEdge::FaceHandle> triangles;
    if (BboxHelper::rayIntersectBox(ray, rootBoundingBox))
        intersectedQuery(ray, rootOctreeNode.get(), rootBoundingBox, triangles);
    return triangles;
}

Node* FaceOctree::getNode() const
{
    return node;
}

Mesh* FaceOctree::getMesh()
{
    return mesh;
}
