#pragma once

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <functional>
#include <filesystem>
#include <string>
#include <vector>
#include <map>

#include "glrenderengine.h"
#include "halfedgetable.h"
#include "model.h"

enum class FileType { STL, COLLADA, OBJ, FBX };

class ModelLoader
{
public:
	ModelLoader() = delete;
	ModelLoader(GLRenderEngine* renderEngine);
	std::unique_ptr<Node> loadModel(std::string const& path);
	void saveModel(const Model& modelPtr, std::string const& path);

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
	void processMaterial(aiMesh* aimesh, Mesh* mesh, const aiScene* scene, FileType fileType);

	HalfEdge::VertexHandle addVertex(HalfEdge::HalfEdgeTable& halfEdgeTable, glm::vec3 vertex);

	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	Texture* loadTexture(const std::string& path, const std::string& directory, const std::string& type);
	std::string getTextureType(aiTextureType type);

	glm::mat4 toGLM(const aiMatrix4x4& mat);
	aiMatrix4x4 toAi(const glm::mat4& mat);

	FileType getType(std::string const& path);
	std::string getExtensionStr(std::string const& path);
	bool checkFileType(FileType fileType);

	void formNodeList(std::vector<Node*>& nodeList, const std::vector<std::unique_ptr<Node>>& nodePtrList);
	std::vector<Node*> getNodeList(const Model& model);

	void saveNode(aiNode* aiParentNode, const std::vector<std::unique_ptr<Node>>& nodes, aiScene* scene, std::string const& materialPath);
	void saveMesh(Mesh* mesh, aiNode* ainode, aiScene* scene);
	void saveMaterial(Mesh* mesh, aiScene* scene, std::string const& path);
	void getFaceVertices(const HalfEdge::HalfEdgeTable& halfEdgeTable, const HalfEdge::FaceHandle& fh, std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& textureCoords);
	int getMeshCount(const Model& model);
};
