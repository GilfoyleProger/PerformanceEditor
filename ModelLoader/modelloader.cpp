#include "modelloader.h"
#include <iostream>

ModelLoader::ModelLoader(GLRenderEngine* renderEngine) : addedVertices(comparator), renderEngine(nullptr)
{
	this->renderEngine = renderEngine;
}

std::unique_ptr<Node> ModelLoader::loadModel(std::string const& path)
{
	if (!std::filesystem::exists(path))
		return nullptr;

	FileType fileType = getType(path);
	if (!checkFileType(fileType))
		return nullptr;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);//| aiProcess_FlipUVs

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return nullptr;
	}

	currentDirectory = path.substr(0, path.find_last_of('/'));
	currentLoadedTextures.clear();

	std::unique_ptr<Node> rootNode = std::make_unique<Node>();
	processNode(scene->mRootNode, scene, rootNode.get(), fileType);

	return std::move(rootNode);
}

void ModelLoader::processNode(aiNode* node, const aiScene* scene, Node* meshNode, FileType fileType)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshNode->attachMesh(processMesh(mesh, scene, fileType));
	}

	meshNode->applyRelativeTransform(toGLM(node->mTransformation));
	meshNode->setName(std::string(node->mName.C_Str()));
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		std::unique_ptr<Node> childNode = std::make_unique<Node>();
		processNode(node->mChildren[i], scene, childNode.get(), fileType);
		meshNode->attachNode(std::move(childNode));
	}
}

std::unique_ptr<Mesh> ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene, FileType fileType)
{
	HalfEdge::HalfEdgeTable halfEdgeTable;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> vertexNormals;
	std::vector<glm::vec2> faceTextureCoords;
	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		vertices.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));

		if (mesh->HasNormals())
		{
			vertexNormals.push_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
		}

		glm::vec2 textureCoords;
		if (mesh->mTextureCoords[0])
		{
			faceTextureCoords.emplace_back(glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
		}
		else
		{
			textureCoords = glm::vec2(0.0f, 0.0f);
		}
	}

	addedVertices.clear();

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		int a = face.mIndices[0];
		int b = face.mIndices[1];
		int c = face.mIndices[2];

		HalfEdge::VertexHandle vh0 = addVertex(halfEdgeTable, vertices[a]);
		HalfEdge::VertexHandle vh1 = addVertex(halfEdgeTable, vertices[b]);
		HalfEdge::VertexHandle vh2 = addVertex(halfEdgeTable, vertices[c]);
		HalfEdge::FaceHandle fh = halfEdgeTable.addFace(vh0, vh1, vh2);

		if (!vertexNormals.empty() && fh.index != -1)
		{
			glm::vec3 faceNormal = (vertexNormals[a] + vertexNormals[b] + vertexNormals[c]) / 3.0f;
			halfEdgeTable.deref(fh).normal = faceNormal;
		}

		if (!faceTextureCoords.empty() && fh.index != -1)
		{
			halfEdgeTable.deref(fh).textureCoords.push_back(faceTextureCoords[a]);
			halfEdgeTable.deref(fh).textureCoords.push_back(faceTextureCoords[b]);
			halfEdgeTable.deref(fh).textureCoords.push_back(faceTextureCoords[c]);
		}
	}
	halfEdgeTable.connectTwins();

	std::unique_ptr<Mesh> meshPtr = std::make_unique<Mesh>(halfEdgeTable);
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	aiString name;
	if (AI_SUCCESS != material->Get(AI_MATKEY_NAME, name))
	{
		name = "unnamed";
	}
	meshPtr->getMaterial().name = std::string(name.C_Str());
	meshPtr->setMeshName(std::string(mesh->mName.C_Str()));

	meshPtr->getMaterial().ambient = glm::vec3(0.f, 0.f, 0.f);
	meshPtr->getMaterial().ambientEnabled = false;
	aiColor3D ambient(0.f, 0.f, 0.f);
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, ambient))
	{
		meshPtr->getMaterial().ambient = glm::vec3(ambient.r, ambient.g, ambient.b);
		meshPtr->getMaterial().ambientEnabled = true;
	}

	meshPtr->getMaterial().diffuse = glm::vec3(0.f, 0.f, 0.f);
	meshPtr->getMaterial().diffuseEnabled = false;
	aiColor3D diffuse(0.f, 0.f, 0.f);
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse))
	{
		meshPtr->getMaterial().diffuse = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
		meshPtr->getMaterial().diffuseEnabled = true;
	}

	meshPtr->getMaterial().specular = glm::vec3(0.f, 0.f, 0.f);
	meshPtr->getMaterial().specularEnabled = false;
	aiColor3D specular(0.f, 0.f, 0.f);
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, specular))
	{
		meshPtr->getMaterial().specular = glm::vec3(specular.r, specular.g, specular.b);
		meshPtr->getMaterial().specularEnabled = true;
	}

	meshPtr->getMaterial().emission = glm::vec3(0.f, 0.f, 0.f);
	meshPtr->getMaterial().emissionEnabled = false;
	aiColor3D emission(0.f, 0.f, 0.f);
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_EMISSIVE, emission))
	{
		meshPtr->getMaterial().emission = glm::vec3(emission.r, emission.g, emission.b);
		meshPtr->getMaterial().emissionEnabled = true;
	}

	meshPtr->getMaterial().shininess = 1.0f;
	float shininess(1.f);
	if (AI_SUCCESS == material->Get(AI_MATKEY_SHININESS, shininess))
	{
		meshPtr->getMaterial().shininess = shininess;
	}

	std::vector<Texture> textures;
	aiString path;
	if (AI_SUCCESS != material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
	}

	std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

	std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	std::vector<int> texturesId;
	for (auto& tex : textures)
	{
		texturesId.push_back(tex.id);
	}
	meshPtr->setTextures(texturesId);
	return std::move(meshPtr);
}

HalfEdge::VertexHandle ModelLoader::addVertex(HalfEdge::HalfEdgeTable& halfEdgeTable, glm::vec3 vertex)
{
	HalfEdge::VertexHandle vh;

	auto iter = addedVertices.emplace(vertex, HalfEdge::VertexHandle());

	if (iter.second)
	{
		vh = halfEdgeTable.addVertex(vertex);
		iter.first->second = vh;
		return vh;
	}
	else
	{
		vh = iter.first->second;
		return vh;
	}
}

std::vector<Texture> ModelLoader::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < currentLoadedTextures.size(); j++)
		{
			if (std::strcmp(currentLoadedTextures[j].fullpath.data(), str.C_Str()) == 0)
			{
				textures.push_back(currentLoadedTextures[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{
			// if texture hasn't been loaded already, load it
			Texture texture = loadTexture(std::string(str.C_Str()), this->currentDirectory);
			textures.push_back(texture);
			currentLoadedTextures.push_back(std::move(texture));
		}
	}
	return textures;
}

Texture ModelLoader::loadTexture(const std::string& name, const std::string& directory)
{
	std::string filename = directory + '/' + name;
	int textureId = renderEngine->loadTexture(filename);
	Texture& texture = renderEngine->getTexture(textureId);
	texture.id = textureId;
	texture.fullpath = filename;
	texture.name = name;
	return texture;
}

glm::mat4 ModelLoader::toGLM(const aiMatrix4x4& aiMatrix)
{
	// Since aiMatrix is row-major order, and glmMatrix is column-major order,
	// we need to assign the columns of aiMatrix to the rows of glmMatrix
	glm::mat4 glmMatrix = glm::mat4(1.0f);

	glmMatrix[0][0] = aiMatrix[0][0];
	glmMatrix[1][0] = aiMatrix[0][1];
	glmMatrix[2][0] = aiMatrix[0][2];
	glmMatrix[3][0] = aiMatrix[0][3];

	glmMatrix[0][1] = aiMatrix[1][0];
	glmMatrix[1][1] = aiMatrix[1][1];
	glmMatrix[2][1] = aiMatrix[1][2];
	glmMatrix[3][1] = aiMatrix[1][3];

	glmMatrix[0][2] = aiMatrix[2][0];
	glmMatrix[1][2] = aiMatrix[2][1];
	glmMatrix[2][2] = aiMatrix[2][2];
	glmMatrix[3][2] = aiMatrix[2][3];

	glmMatrix[0][3] = aiMatrix[3][0];
	glmMatrix[1][3] = aiMatrix[3][1];
	glmMatrix[2][3] = aiMatrix[3][2];
	glmMatrix[3][3] = aiMatrix[3][3];

	return glmMatrix;
}

FileType ModelLoader::getType(std::string const& path)
{
	std::string extension = std::filesystem::path(path).extension().string();
	if (!extension.empty())
		extension.erase(0, 1);
	std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return std::tolower(c); });

	if (extension == "stl")
		return FileType::STL;
	else if (extension == "collada")
		return FileType::COLLADA;
	else if (extension == "obj")
		return FileType::OBJ;
	else if (extension == "fbx")
		return FileType::FBX;
}

bool ModelLoader::checkFileType(FileType fileType)
{
	if (fileType == FileType::STL)
		return true;
	else if (fileType == FileType::COLLADA)
		return true;
	else if (fileType == FileType::OBJ)
		return true;
	else if (fileType == FileType::FBX)
		return true;
	else
		return false;
}
