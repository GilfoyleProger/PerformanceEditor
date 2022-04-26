#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include "node.h"

class PointLight : public Node
{
public:
    glm::vec3 getPos()
    {
        bbox lightBbox = Node::getWorldBbox();
        glm::vec3 center = (lightBbox.max + lightBbox.min) / 2.0f;
        return center;
    }


    HalfEdge::HalfEdgeTable createSphere(glm::vec3 origin, double sphereRadius, double sliceCount, double ringCount)
    {
        HalfEdge::HalfEdgeTable halfEdgeTable;
        std::vector<HalfEdge::VertexHandle> sphereVertices;

        const double PI = 3.1415926535897931;

        double x, y, z, xy; // vertex position
        // for vertices computing
        double sliceStep = 2.0 * PI / sliceCount;
        double ringStep  = PI / ringCount;
        double sliceAngle, ringAngle;

        for (unsigned int i = 0; i <= ringCount; ++i)
        {
            ringAngle = PI / 2.0 - i * ringStep;
            xy = sphereRadius * cos(ringAngle);
            z = sphereRadius * sin(ringAngle) + origin.z;

            // add vertices per slice
            for (unsigned int j = 0; j <= sliceCount; ++j)
            {
                sliceAngle = j * sliceStep;

                // vertex position computing
                x = xy * cos(sliceAngle) + origin.x;
                y = xy * sin(sliceAngle) + origin.y;
                sphereVertices.emplace_back(halfEdgeTable.addVertex(glm::vec3( x, y, z )));
             //   vertices.emplace_back(glm::vec3{ x, y, z });
            }
        }

      //  std::vector<glm::vec3> ; // form sphere triangles by vertices in CCW order

        for (int i = 0; i < ringCount; ++i)
        {
            unsigned int currVertexIndex = i * (sliceCount + 1); // current ring
            unsigned int nextVertexIndex = currVertexIndex + sliceCount + 1; // next ring

            for (int j = 0; j < sliceCount; ++j, ++currVertexIndex, ++nextVertexIndex)
            {
                if (i != 0) // because 2 triangles per ring excluding first and last ring
                {
                    halfEdgeTable.addFace(sphereVertices[currVertexIndex],
                                          sphereVertices[nextVertexIndex],
                                          sphereVertices[currVertexIndex + 1] );

                }
                if (i != (ringCount - 1))
                {

                    halfEdgeTable.addFace(sphereVertices[currVertexIndex + 1],
                            sphereVertices[nextVertexIndex],
                            sphereVertices[nextVertexIndex + 1] );
                }
            }
        }
        halfEdgeTable.connectTwins();
        // form triangle soup and compute normals of the sphere
        return halfEdgeTable;
    }







    HalfEdge::HalfEdgeTable createCube(double cubeSideLength)
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
void setAmbientEnabled(bool state)
{
    isAmbientEnabled =  state;
}
bool ambientEnabled()
{
return isAmbientEnabled;
}
void setDiffuseEnabled(bool state)
{
    isDiffuseEnabled =  state;
}
bool diffuseEnabled()
{
return isDiffuseEnabled;
}
void setSpecularEnabled(bool state)
{
    isSpecularEnabled =  state;
}
bool specularEnabled()
{
return isSpecularEnabled;
}

bool enabled()
{
return isLightEnabled;
}
void setEnabled(bool state)
{
 isLightEnabled=state;
}

glm::vec3& ambient()
{
return Ka;
}
glm::vec3& diffuse()
{
return Kd;
}
glm::vec3& specular()
{
return Ks;
}
bool visible()
{
return isVisible;
}
void setVisible(bool state)
{
 isVisible=state;
}
    void setPos(glm::vec3 newPos)
    {
        bbox lightBbox = Node::getWorldBbox();
        glm::vec3 center = (lightBbox.max + lightBbox.min) / 2.0f;
        glm::vec3 delta = newPos - center;
        glm::mat4 mat = glm::mat4(1.0f);
        mat[3] = glm::vec4(delta, 1.0f);
        setRelativeTransform(mat);
    }
bool isLightEnabled = true;
bool isVisible = true;
    glm::vec3 Ka = glm::vec3(0.0f);
    bool isAmbientEnabled = false;
    bool isDiffuseEnabled = false;
    bool isSpecularEnabled = false;
    glm::vec3 Kd = glm::vec3(0.0f);
    glm::vec3 Ks = glm::vec3(0.0f);


    PointLight();

};
