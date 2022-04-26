#include "buildplate.h"

BuildPlate::BuildPlate()
{
    std::unique_ptr<Mesh> meshPtr = std::make_unique<Mesh>(createBuildPlate(20, 0.1, 20));
    Node::attachMesh(std::move(meshPtr));
}

HalfEdge::HalfEdgeTable BuildPlate::createBuildPlate(double width, double height, double depth)
{
    HalfEdge::HalfEdgeTable halfEdgeTable;
    double cubeSideLength = 1.0;


        double halfEdge = cubeSideLength / 2.0;
        std::vector<HalfEdge::VertexHandle> cubeVertices;

        cubeVertices.emplace_back(halfEdgeTable.addVertex({ width, -height,  depth }));
        cubeVertices.emplace_back(halfEdgeTable.addVertex({ width, -height, -depth }));
        cubeVertices.emplace_back(halfEdgeTable.addVertex({ -width, -height, -depth }));
        cubeVertices.emplace_back(halfEdgeTable.addVertex({ -width, -height,  depth }));
        cubeVertices.emplace_back(halfEdgeTable.addVertex({ width,  height,  depth }));
        cubeVertices.emplace_back(halfEdgeTable.addVertex({ width,  height, -depth }));
        cubeVertices.emplace_back(halfEdgeTable.addVertex({ -width,  height, -depth }));
        cubeVertices.emplace_back(halfEdgeTable.addVertex({ -width,  height,  depth }));

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




    return halfEdgeTable;
}
