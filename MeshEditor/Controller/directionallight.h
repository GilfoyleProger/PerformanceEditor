#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include "node.h"

class DirectionalLight : public Node
{
public:
	glm::vec3 getPos()
	{
		bbox lightBbox = Node::getWorldBbox();
		glm::vec3 center = (lightBbox.max + lightBbox.min) / 2.0f;
		return center;
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
    void setPos(glm::vec3 newPos) 
    {
        bbox lightBbox = Node::getWorldBbox();
        glm::vec3 center = (lightBbox.max + lightBbox.min) / 2.0f;
        glm::vec3 delta = newPos - center;
       // glm::vec3 delta = center - newPos;
        glm::mat4 mat = glm::mat4(1.0f);
        mat[3] = glm::vec4(delta, 1.0f);
        setRelativeTransform(mat);



    }
	glm::vec3 ambient = glm::vec3(0.0f);
	glm::vec3 diffuse = glm::vec3(0.0f);
	glm::vec3 specular = glm::vec3(0.0f);

	DirectionalLight();
};

#endif // DIRECTIONALLIGHT_H
