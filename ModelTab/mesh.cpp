#include "mesh.h"
#include <glm/glm.hpp>
#include <QImage>
#include <QOpenGLTexture>

Mesh::Mesh(const HalfEdge::HalfEdgeTable& halfEdgeTable)
{
	this->halfEdgeTable = halfEdgeTable;
	initMeshData(positions, indices);
	calculateBBox();
}

Material& Mesh::getMaterial()
{
	return material;
}

void Mesh::render(GLRenderEngine* re, ShaderProgram* shaderProgram)
{
renderEngine = re;
	//Container container = std::move(re->createMesh(positions, indices));
	if (isGeometryInitialized)
	{
        setShaderUniform("material.diffuseMapEnabled", getMaterial().diffuseMapEnabled);
        setShaderUniform("material.specularMapEnabled", getMaterial().specularMapEnabled);
        setShaderUniform("material.normalMapEnabled", getMaterial().normalMapEnabled);
		re->render(container.vao, indices.size(), shaderProgram, DrawType::Arrays, uniformContainer, texturesId);
		//re->render(re->containers[uuid].vao.get(), indices.size(), shaderProgram, DrawType::Arrays, uniformContainer, textures);
	}
	else
	{
		//Container container;

		container = std::move(re->createMesh(positions, indices));
		//uuid = re->containers.size();
		//re->containers.emplace(uuid, std::move(container));

		//renderEngine = re;

		isGeometryInitialized = true;
	}
}

void Mesh::renderEdges()
{
	//TODO:
}

const bbox& Mesh::getBoundingBox() const
{
	return boundingBox;
}

bbox Mesh::getBoundingBox(glm::mat4 transform)
{
	bbox transformedBbox = bbox{ glm::vec3(0,0,0), glm::vec3(0,0,0) };
	auto& vertices = halfEdgeTable.getVertices();

	if (vertices.size() != 0)
	{
		transformedBbox.min = transform * glm::vec4(halfEdgeTable.getPoint(halfEdgeTable.handle(vertices[0])), 1.0f);
		transformedBbox.max = transform * glm::vec4(halfEdgeTable.getPoint(halfEdgeTable.handle(vertices[0])), 1.0f);

		for (const auto& vertex : vertices)
		{
			HalfEdge::VertexHandle vh = halfEdgeTable.handle(vertex);
			glm::vec3 point = transform * glm::vec4(halfEdgeTable.getPoint(vh), 1.0f);


			// x-axis
			if (point.x < transformedBbox.min.x)
				transformedBbox.min.x = point.x;
			if (point.x > transformedBbox.max.x)
				transformedBbox.max.x = point.x;
			// y-axis
			if (point.y < transformedBbox.min.y)
				transformedBbox.min.y = point.y;
			if (point.y > transformedBbox.max.y)
				transformedBbox.max.y = point.y;
			// z-axis
			if (point.z < transformedBbox.min.z)
				transformedBbox.min.z = point.z;
			if (point.z > transformedBbox.max.z)
				transformedBbox.max.z = point.z;
		}
	}

	return transformedBbox;
}

const HalfEdge::HalfEdgeTable& Mesh::getHalfEdgeTable() const
{
	return halfEdgeTable;
}

void Mesh::setShaderUniform(std::string name, QVector4D value)
{
	uniformContainer.uniformToVec4[name] = value;
}

void Mesh::setShaderUniform(std::string name, QVector3D value)
{
	uniformContainer.uniformToVec3[name] = value;
}

void Mesh::setShaderUniform(std::string name, QVector2D value)
{
	uniformContainer.uniformToVec2[name] = value;
}

void Mesh::setShaderUniform(std::string name, GLfloat value)
{
	uniformContainer.uniformToFloat[name] = value;
}

void Mesh::setShaderUniform(std::string name, bool value)
{
	uniformContainer.uniformToBool[name] = value;
}

void Mesh::setShaderUniform(std::string name, GLint value)
{
    uniformContainer.uniformToInt[name] = value;
}

std::vector<std::vector<HalfEdge::Vertex>> Mesh::getFaceTriangles(HalfEdge::FaceHandle fh)
{
	HalfEdge::HalfEdgeHandle heh = halfEdgeTable.deref(fh).heh;
	HalfEdge::HalfEdgeHandle nextHeh = heh;
	HalfEdge::HalfEdgeHandle startHeh;
	std::vector<HalfEdge::Vertex> tempVertices;

	while (nextHeh.index != startHeh.index)
	{
		auto& twin = halfEdgeTable.deref(nextHeh).twin;
		tempVertices.emplace_back(halfEdgeTable.deref(halfEdgeTable.deref(twin).dst));
		nextHeh = halfEdgeTable.next((nextHeh));

		if (startHeh.index == -1)
			startHeh = heh;
	}

	std::vector<std::vector<HalfEdge::Vertex>> faceTriangles;

	// Save first triangle
	std::vector<HalfEdge::Vertex> firstTriangle = { tempVertices[0], tempVertices[1], tempVertices[2] };
	faceTriangles.emplace_back(firstTriangle);

	if (tempVertices.size() == 4)
	{
		// Then it is quad face and we need to save second triangle
		std::vector<HalfEdge::Vertex> secondTriangle = { tempVertices[2], tempVertices[3], tempVertices[0] };
		faceTriangles.emplace_back(secondTriangle);
	}

	return faceTriangles;
}

void Mesh::initMeshData(std::vector<Vertex>& positions, std::vector<int>& indices)
{
	for (const auto& face : halfEdgeTable.getFaces())
	{
		HalfEdge::FaceHandle fh = halfEdgeTable.handle(face);
		std::vector<std::vector<HalfEdge::Vertex>> faceTriangles = getFaceTriangles(fh);

		for (int i = 0; i < faceTriangles.size(); i++)
		{
			Vertex vertex;
			std::vector<HalfEdge::Vertex> triangleVertices = faceTriangles[i];
			glm::vec3 a = halfEdgeTable.getStartPoint(triangleVertices[0].heh);
			vertex.position = a;
			vertex.normal = halfEdgeTable.deref(fh).normal;

			if (!halfEdgeTable.deref(fh).textureCoords.empty())
			{
				vertex.texCoords = halfEdgeTable.deref(fh).textureCoords[0];
			}
			else
			{
				vertex.texCoords = glm::vec2(0, 0);
			}
			indices.emplace_back(indices.size());
			positions.push_back(vertex);

			glm::vec3 b = halfEdgeTable.getStartPoint(triangleVertices[1].heh);
			vertex.position = b;
			vertex.normal = halfEdgeTable.deref(fh).normal;

			if (!halfEdgeTable.deref(fh).textureCoords.empty())
			{
				vertex.texCoords = halfEdgeTable.deref(fh).textureCoords[1];
			}
			else
			{
				vertex.texCoords = glm::vec2(0, 0);
			}
			indices.emplace_back(indices.size());
			positions.push_back(vertex);

			glm::vec3 c = halfEdgeTable.getStartPoint(triangleVertices[2].heh);
			vertex.position = c;
			vertex.normal = halfEdgeTable.deref(fh).normal;

			if (!halfEdgeTable.deref(fh).textureCoords.empty())
			{
				vertex.texCoords = halfEdgeTable.deref(fh).textureCoords[2];
			}
			else
			{
				vertex.texCoords = glm::vec2(0, 0);
			}
			indices.emplace_back(indices.size());
			positions.push_back(vertex);
		}
	}

	/*
	for (const auto& face : halfEdgeTable.getFaces())
	{
		HalfEdge::FaceHandle fh = halfEdgeTable.handle(face);
		std::vector<std::vector<HalfEdge::Vertex>> faceTriangles = getFaceTriangles(fh);

		for (int i = 0; i < faceTriangles.size(); i++)
		{
			std::vector<HalfEdge::Vertex> triangleVertices = faceTriangles[i];
			glm::vec3 a = halfEdgeTable.getStartPoint(triangleVertices[0].heh);
			positions.emplace_back(a.x);
			positions.emplace_back(a.y);
			positions.emplace_back(a.z);

			positions.push_back(halfEdgeTable.deref(fh).normal.x);
			positions.push_back(halfEdgeTable.deref(fh).normal.y);
			positions.push_back(halfEdgeTable.deref(fh).normal.z);
			if (!halfEdgeTable.deref(fh).textureCoords.empty()) {
				positions.push_back(halfEdgeTable.deref(fh).textureCoords[0].x);
				positions.push_back(halfEdgeTable.deref(fh).textureCoords[0].y);
			}
			else
			{
				positions.push_back(0);
				positions.push_back(0);
			}
			indices.emplace_back(indices.size());

			glm::vec3 b = halfEdgeTable.getStartPoint(triangleVertices[1].heh);
			positions.emplace_back(b.x);
			positions.emplace_back(b.y);
			positions.emplace_back(b.z);
			positions.push_back(halfEdgeTable.deref(fh).normal.x);
			positions.push_back(halfEdgeTable.deref(fh).normal.y);
			positions.push_back(halfEdgeTable.deref(fh).normal.z);
			if (!halfEdgeTable.deref(fh).textureCoords.empty()) {
				positions.push_back(halfEdgeTable.deref(fh).textureCoords[1].x);
				positions.push_back(halfEdgeTable.deref(fh).textureCoords[1].y);
			}
			else
			{
				positions.push_back(0);
				positions.push_back(0);
			}
			indices.emplace_back(indices.size());

			glm::vec3 c = halfEdgeTable.getStartPoint(triangleVertices[2].heh);
			positions.emplace_back(c.x);
			positions.emplace_back(c.y);
			positions.emplace_back(c.z);
			positions.push_back(halfEdgeTable.deref(fh).normal.x);
			positions.push_back(halfEdgeTable.deref(fh).normal.y);
			positions.push_back(halfEdgeTable.deref(fh).normal.z);
			if (!halfEdgeTable.deref(fh).textureCoords.empty()) {
				positions.push_back(halfEdgeTable.deref(fh).textureCoords[2].x);
				positions.push_back(halfEdgeTable.deref(fh).textureCoords[2].y);
			}
			else
			{
				positions.push_back(0);
				positions.push_back(0);
			}
			indices.emplace_back(indices.size());
		}
	}
	*/
}

void Mesh::calculateBBox()
{
	auto& vertices = halfEdgeTable.getVertices();

	if (vertices.size() != 0)
	{
		boundingBox.min = halfEdgeTable.getPoint(halfEdgeTable.handle(vertices[0]));
		boundingBox.max = halfEdgeTable.getPoint(halfEdgeTable.handle(vertices[0]));

		for (const auto& vertex : vertices)
		{
			HalfEdge::VertexHandle vh = halfEdgeTable.handle(vertex);
			glm::vec3 point = halfEdgeTable.getPoint(vh);

			// x-axis
			if (point.x < boundingBox.min.x)
				boundingBox.min.x = point.x;
			if (point.x > boundingBox.max.x)
				boundingBox.max.x = point.x;
			// y-axis
			if (point.y < boundingBox.min.y)
				boundingBox.min.y = point.y;
			if (point.y > boundingBox.max.y)
				boundingBox.max.y = point.y;
			// z-axis
			if (point.z < boundingBox.min.z)
				boundingBox.min.z = point.z;
			if (point.z > boundingBox.max.z)
				boundingBox.max.z = point.z;
		}
	}
}

void Mesh::setTextures(std::vector<int> t)
{
	for (auto& d : t)
        texturesId.emplace_back(d);
}
