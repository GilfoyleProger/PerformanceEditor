#pragma once

#include "halfedgetable.h"
#include "glrenderengine.h"

struct bbox
{
	glm::vec3 min;
	glm::vec3 max;
};

class Mesh
{
public:
	Mesh(const HalfEdge::HalfEdgeTable& halfEdgeTable);

	void render(GLRenderEngine* renderEngine, ShaderProgram* shaderProgram);
	void renderEdges();

	const bbox& getBoundingBox() const;
	bbox getBoundingBox(glm::mat4 transform);
	const HalfEdge::HalfEdgeTable& getHalfEdgeTable() const;

	Material& getMaterial();

	void setShaderUniform(std::string name, QVector4D value);
	void setShaderUniform(std::string name, QVector3D value);
	void setShaderUniform(std::string name, QVector2D value);
	void setShaderUniform(std::string name, GLfloat value);
	void setShaderUniform(std::string name, bool value);
	void setShaderUniform(std::string name, GLint value);
	void setTextures(std::vector<int> textures);
	void setMeshName(std::string inName) {
		meshName = inName;
	}
	std::string getMeshName() {
		return meshName;// = inName;
	}
	const Texture* getDiffuseMap() const
	{
		for (int i = 0; i < texturesId.size(); i++)
		{
			Texture* texture = renderEngine->getTexture(texturesId[i]);
			if (texture) {
				if (texture->type == "diffuseMap")
				{
					return renderEngine->getTexture(texturesId[i]);
				}
			}
		}
		return nullptr;
	}
	const Texture* getSpecularMap() const
	{
		for (int i = 0; i < texturesId.size(); i++)
		{
			if (renderEngine->getTexture(texturesId[i])->type == "specularMap")
			{
				return renderEngine->getTexture(texturesId[i]);
			}
		}
		return nullptr;
	}
	const Texture* getNormalMap() const
	{
		for (int i = 0; i < texturesId.size(); i++)
		{
			if (renderEngine->getTexture(texturesId[i])->type == "normalMap")
			{
				return renderEngine->getTexture(texturesId[i]);
			}
		}
		return nullptr;
	}
private:
	HalfEdge::HalfEdgeTable halfEdgeTable;
	UniformContainer uniformContainer;
	GLRenderEngine* renderEngine;
	std::string meshName = "";
	Container container;
	Material material;
	bbox boundingBox;
	int uuid = 0;
	//std::vector<Texture> textures;
	std::vector<int> texturesId;
	std::vector<Vertex> positions;
	std::vector<int> indices;
	bool isGeometryInitialized = false;
	std::vector<std::vector<HalfEdge::Vertex>> getFaceTriangles(HalfEdge::FaceHandle fh);
	void initMeshData(std::vector<Vertex>& positions, std::vector<int>& indices);
	void calculateBBox();
};
