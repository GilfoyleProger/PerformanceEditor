#pragma once

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <functional>
#include <string>
#include <vector>
#include <map>

#include "glrenderengine.h"
#include "halfedgetable.h"
//#include "node.h"
#include "model.h"
#include <filesystem>

enum class FileType { STL, COLLADA, OBJ, FBX };

class ModelLoader
{
public:
	ModelLoader() = delete;
	ModelLoader(GLRenderEngine* renderEngine);
	std::unique_ptr<Node> loadModel(std::string const& path);
	std::unique_ptr<Node> saveModel(Model* modelPtr, std::string path);
private:
	void formNodeList(std::vector<Node*>& nodeList, const std::vector<std::unique_ptr<Node>>& nodePtrList);
	std::vector<Node*> getNodeList(const Model& model);
	std::vector<Node*> getNodeList(Node* node);
	void saveNode(aiNode* aiParentNode, const std::vector<std::unique_ptr<Node>>& nodes, aiScene* scene, std::string path);
	void saveNode(aiNode* aiParentNode, Node* node, aiScene* scene);
	void getFaceVertices(const HalfEdge::HalfEdgeTable& halfEdgeTable, const HalfEdge::FaceHandle& fh,
		std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& textureCoords);
	int getMeshCount(const Model& model);
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
	Texture* loadTexture(const std::string& path, const std::string& directory, std::string type);
	std::string getTextureType(aiTextureType type)
	{
		if (type == aiTextureType_DIFFUSE)
		{
			return "diffuseMap";
		}
		else if (type == aiTextureType_SPECULAR)
		{
			return "specularMap";
		}
		return "";
	}
	glm::mat4 toGLM(const aiMatrix4x4& mat);
	aiMatrix4x4 toAi(const glm::mat4& mat);
	FileType getType(std::string const& path);
	std::string getExtensionStr(std::string const& path)
	{
		std::string extension = std::filesystem::path(path).extension().string();
		if (!extension.empty())
			extension.erase(0, 1);
		std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return std::tolower(c); });
		return extension;
	}
	bool checkFileType(FileType fileType);
};
