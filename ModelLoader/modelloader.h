#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <functional>
#include <string>
#include <vector>
#include <map>

#include "glrenderengine.h"
#include "halfedgetable.h"
#include "node.h"
#include <filesystem>

enum class FileType { STL, COLLADA, OBJ, FBX };

class ModelLoader
{
public:
	ModelLoader() = delete;
	ModelLoader(GLRenderEngine* renderEngine);
	std::unique_ptr<Node> loadModel(std::string const& path);
private:
	GLRenderEngine* renderEngine;
	using Comparator = std::function<bool(const glm::vec3& a, const glm::vec3& b)>;
	Comparator comparator = [](const glm::vec3& a, const glm::vec3& b) -> bool
	{
		constexpr float tolerance = std::numeric_limits<float>::epsilon();
		if (std::fabs(a.x - b.x) > tolerance)
			return (b.x - a.x) < -tolerance;
		if (std::fabs(a.y - b.y) > tolerance)
			return (b.y - a.y) < -tolerance;
		return (b.z - a.z) < -tolerance;
	};
	std::map<glm::vec3, HalfEdge::VertexHandle, Comparator> addedVertices;
	std::vector<Texture> currentLoadedTextures;
	std::string currentDirectory;
private:
	void processNode(aiNode* node, const aiScene* scene, Node* meshNode, FileType fileType);
	std::unique_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene, FileType fileType);

	HalfEdge::VertexHandle addVertex(HalfEdge::HalfEdgeTable& halfEdgeTable, glm::vec3 vertex);

	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	Texture loadTexture(const std::string& path, const std::string& directory);

	glm::mat4 toGLM(const aiMatrix4x4& mat);
	FileType getType(std::string const& path);
	bool checkFileType(FileType fileType);
};
