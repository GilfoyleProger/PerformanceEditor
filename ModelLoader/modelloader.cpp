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
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
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

void ModelLoader::saveModel(const Model& modelPtr, std::string const& path)
{
	if (path.empty() || std::filesystem::exists(path))
		return;

	FileType fileType = getType(path);

	if (!checkFileType(fileType))
		return;

	Assimp::Exporter exporter;

	aiScene aiscene;

	const int meshCount = getMeshCount(modelPtr);
	aiscene.mMeshes = new aiMesh * [meshCount];
	aiscene.mNumMeshes = 0;

	aiscene.mMaterials = new aiMaterial * [meshCount];
	aiscene.mNumMaterials = 0;

	aiscene.mRootNode = new aiNode();
	aiscene.mRootNode->mName = "Scene";

	std::string materialPath = path;
	auto pos = materialPath.find_last_of('/');
	materialPath = materialPath.substr(0, pos);

	for (auto& node : modelPtr.getNodes())
	{
		saveNode(aiscene.mRootNode, node->getChildren(), &aiscene, materialPath);
	}

	if (aiscene.mNumMeshes > 0)
	{
		exporter.Export(&aiscene, getExtensionStr(path).c_str(), path.c_str());
	}
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

std::unique_ptr<Mesh> ModelLoader::processMesh(aiMesh* aimesh, const aiScene* aiscene, FileType fileType)
{
	HalfEdge::HalfEdgeTable halfEdgeTable;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> vertexNormals;
	std::vector<glm::vec2> faceTextureCoords;
	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < aimesh->mNumVertices; i++)
	{
		vertices.push_back(glm::vec3(aimesh->mVertices[i].x, aimesh->mVertices[i].y, aimesh->mVertices[i].z));

		if (aimesh->HasNormals())
		{
			vertexNormals.push_back(glm::vec3(aimesh->mNormals[i].x, aimesh->mNormals[i].y, aimesh->mNormals[i].z));
		}

		glm::vec2 textureCoords;
		if (aimesh->mTextureCoords[0])
		{
			faceTextureCoords.emplace_back(glm::vec2(aimesh->mTextureCoords[0][i].x, aimesh->mTextureCoords[0][i].y));
		}
		else
		{
			faceTextureCoords.emplace_back(0.0f, 0.0f);
		}
	}

	addedVertices.clear();

	for (unsigned int i = 0; i < aimesh->mNumFaces; i++)
	{
		aiFace face = aimesh->mFaces[i];

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

	std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>(halfEdgeTable);
	processMaterial(aimesh, mesh.get(), aiscene, fileType);

	return std::move(mesh);
}

void ModelLoader::processMaterial(aiMesh* aimesh, Mesh* mesh, const aiScene* scene, FileType fileType)
{
	aiMaterial* material = scene->mMaterials[aimesh->mMaterialIndex];

	aiString name;
	if (AI_SUCCESS != material->Get(AI_MATKEY_NAME, name))
	{
		name = "unnamed";
	}
	mesh->getMaterial().name = std::string(name.C_Str());
	mesh->setMeshName(std::string(aimesh->mName.C_Str()));

	mesh->getMaterial().ambient = glm::vec3(0.f, 0.f, 0.f);
	mesh->getMaterial().ambientEnabled = false;
	aiColor3D ambient(0.f, 0.f, 0.f);
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, ambient))
	{
		mesh->getMaterial().ambient = glm::vec3(ambient.r, ambient.g, ambient.b);
		mesh->getMaterial().ambientEnabled = true;
	}

	mesh->getMaterial().diffuse = glm::vec3(0.f, 0.f, 0.f);
	mesh->getMaterial().diffuseEnabled = false;
	aiColor3D diffuse(0.f, 0.f, 0.f);
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse))
	{
		mesh->getMaterial().diffuse = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
		mesh->getMaterial().diffuseEnabled = true;
	}

	mesh->getMaterial().specular = glm::vec3(0.f, 0.f, 0.f);
	mesh->getMaterial().specularEnabled = false;
	aiColor3D specular(0.f, 0.f, 0.f);
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, specular))
	{
		mesh->getMaterial().specular = glm::vec3(specular.r, specular.g, specular.b);
		mesh->getMaterial().specularEnabled = true;
	}

	mesh->getMaterial().emission = glm::vec3(0.f, 0.f, 0.f);
	mesh->getMaterial().emissionEnabled = false;
	aiColor3D emission(0.f, 0.f, 0.f);
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_EMISSIVE, emission))
	{
		mesh->getMaterial().emission = glm::vec3(emission.r, emission.g, emission.b);
		mesh->getMaterial().emissionEnabled = true;
	}

	mesh->getMaterial().shininess = 1.0f;
	float shininess(1.f);
	if (AI_SUCCESS == material->Get(AI_MATKEY_SHININESS, shininess))
	{
		mesh->getMaterial().shininess = shininess;
	}

	std::vector<Texture> textures;
	aiString path;
	if (AI_SUCCESS != material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
	}

	std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	if (!diffuseMaps.empty())
		mesh->getMaterial().diffuseMapEnabled = true;
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

	std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	if (!specularMaps.empty())
		mesh->getMaterial().specularMapEnabled = true;
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	if (!normalMaps.empty())
		mesh->getMaterial().normalMapEnabled = true;
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

	std::vector<int> texturesId;
	for (auto& texture : textures)
	{
		texturesId.push_back(texture.id);
	}
	mesh->setTextures(texturesId);
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
			if (std::strcmp(currentLoadedTextures[j].name.data(), str.C_Str()) == 0)
			{
				textures.push_back(currentLoadedTextures[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{
			// if texture hasn't been loaded already, load it
			Texture* texture = loadTexture(std::string(str.C_Str()), this->currentDirectory, getTextureType(type));
			if (texture)
			{
				textures.push_back(*texture);
				currentLoadedTextures.push_back(*texture);
			}
		}
	}
	return textures;
}

Texture* ModelLoader::loadTexture(const std::string& name, const std::string& directory, const std::string& type)
{
	std::string filename = directory + '/' + name;
	int textureId = renderEngine->loadTexture(filename);
	if (textureId != -1)
	{
		Texture& texture = *renderEngine->getTexture(textureId);
		texture.id = textureId;
		texture.enabled = true;
		texture.fullpath = filename;
		texture.name = name;
		texture.type = type;
		return &texture;
	}
	return nullptr;
}

std::string ModelLoader::getTextureType(aiTextureType type)
{
	if (type == aiTextureType_DIFFUSE)
	{
		return "diffuseMap";
	}
	else if (type == aiTextureType_SPECULAR)
	{
		return "specularMap";
	}
	else if (type == aiTextureType_NORMALS)
	{
		return "normalMap";
	}
	return "";
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

aiMatrix4x4 ModelLoader::toAi(const glm::mat4& glmMatrix)
{
	// Since aiMatrix is row-major order, and glmMatrix is column-major order,
	// we need to assign the columns of aiMatrix to the rows of glmMatrix
	aiMatrix4x4 aiMatrix = aiMatrix4x4();

	aiMatrix[0][0] = glmMatrix[0][0];
	aiMatrix[1][0] = glmMatrix[0][1];
	aiMatrix[2][0] = glmMatrix[0][2];
	aiMatrix[3][0] = glmMatrix[0][3];

	aiMatrix[0][1] = glmMatrix[1][0];
	aiMatrix[1][1] = glmMatrix[1][1];
	aiMatrix[2][1] = glmMatrix[1][2];
	aiMatrix[3][1] = glmMatrix[1][3];

	aiMatrix[0][2] = glmMatrix[2][0];
	aiMatrix[1][2] = glmMatrix[2][1];
	aiMatrix[2][2] = glmMatrix[2][2];
	aiMatrix[3][2] = glmMatrix[2][3];

	aiMatrix[0][3] = glmMatrix[3][0];
	aiMatrix[1][3] = glmMatrix[3][1];
	aiMatrix[2][3] = glmMatrix[3][2];
	aiMatrix[3][3] = glmMatrix[3][3];

	return aiMatrix;
}

FileType ModelLoader::getType(std::string const& path)
{
	std::string extension = std::filesystem::path(path).extension().string();
	if (!extension.empty())
		extension.erase(0, 1);
	std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return std::tolower(c); });

	if (extension == "stl")
		return FileType::STL;
	else if (extension == "dae")
		return FileType::COLLADA;
	else if (extension == "obj")
		return FileType::OBJ;
	else if (extension == "fbx")
		return FileType::FBX;
}

std::string ModelLoader::getExtensionStr(std::string const& path)
{
	std::string extension = std::filesystem::path(path).extension().string();
	if (!extension.empty())
		extension.erase(0, 1);
	std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return std::tolower(c); });
	return extension;
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

void ModelLoader::formNodeList(std::vector<Node*>& nodeList, const std::vector<std::unique_ptr<Node>>& nodePtrList)
{
	for (const auto& nodePtr : nodePtrList)
	{
		if (!nodePtr->getMeshes().empty())
			nodeList.emplace_back(nodePtr.get());
		formNodeList(nodeList, nodePtr->getChildren());
	}
}

std::vector<Node*> ModelLoader::getNodeList(const Model& model)
{
	std::vector<Node*> nodeList;
	formNodeList(nodeList, model.getNodes());
	return nodeList;
}

void ModelLoader::saveNode(aiNode* aiParentNode, const std::vector<std::unique_ptr<Node>>& nodes, aiScene* scene, std::string const& materialPath)
{
	for (const auto& node : nodes)
	{
		aiNode* ainode = new aiNode();
		ainode->mName = node->getName().c_str();
		ainode->mMeshes = new unsigned int[node->getMeshes().size()];
		ainode->mTransformation = toAi(node->getRelativeTransform());

		for (auto& mesh : node->getMeshes())
		{
			saveMesh(mesh.get(), ainode, scene);
			saveMaterial(mesh.get(), scene, materialPath);
		}

		aiParentNode->addChildren(1, &ainode);
		if (!node->getChildren().empty())
			saveNode(ainode, node->getChildren(), scene, materialPath);
	}
}

void ModelLoader::saveMesh(Mesh* mesh, aiNode* ainode, aiScene* scene)
{
	scene->mMaterials[scene->mNumMaterials] = new aiMaterial();
	scene->mNumMaterials = scene->mNumMaterials + 1;

	scene->mMeshes[scene->mNumMeshes] = new aiMesh();
	scene->mMeshes[scene->mNumMeshes]->mMaterialIndex = scene->mNumMaterials - 1;
	scene->mNumMeshes = scene->mNumMeshes + 1;

	ainode->mMeshes[ainode->mNumMeshes] = scene->mNumMeshes - 1;
	ainode->mNumMeshes = ainode->mNumMeshes + 1;

	auto pMesh = scene->mMeshes[scene->mNumMeshes - 1];
	pMesh->mName = mesh->getMeshName().c_str();
	const auto& halfEdgeTable = mesh->getHalfEdgeTable();
	const auto& faces = halfEdgeTable.getFaces();
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> textureCoords;
	for (int i = 0; i < faces.size(); i++)
	{
		std::vector<glm::vec3> faceVertices;
		std::vector<glm::vec3> faceNormals;
		std::vector<glm::vec2> faceTextureCoords;
		getFaceVertices(halfEdgeTable, halfEdgeTable.handle(faces[i]), faceVertices, faceNormals, faceTextureCoords);
		vertices.insert(vertices.end(), faceVertices.begin(), faceVertices.end());
		normals.insert(normals.end(), faceNormals.begin(), faceNormals.end());
		textureCoords.insert(textureCoords.end(), faceTextureCoords.begin(), faceTextureCoords.end());
	}

	const int verticesCount = vertices.size();
	pMesh->mVertices = new aiVector3D[verticesCount];
	pMesh->mNormals = new aiVector3D[verticesCount];
	pMesh->mNumVertices = verticesCount;

	const int textureCoordsCount = textureCoords.size();
	pMesh->mTextureCoords[0] = new aiVector3D[textureCoordsCount];
	pMesh->mNumUVComponents[0] = textureCoordsCount;

	for (int i = 0; i < vertices.size(); i++)
	{
		pMesh->mVertices[i] = aiVector3D(vertices[i].x, vertices[i].y, vertices[i].z);
		pMesh->mNormals[i] = aiVector3D(normals[i].x, normals[i].y, normals[i].z);
	}

	if (textureCoords.size() == vertices.size())
	{
		for (int i = 0; i < textureCoords.size(); i++)
		{
			if (!textureCoords.empty())
			{
				pMesh->mTextureCoords[0][i] = aiVector3D(textureCoords[i].x, textureCoords[i].y, 0);
			}
		}
	}

	pMesh->mFaces = new aiFace[vertices.size() / 3];
	pMesh->mNumFaces = (unsigned int)(vertices.size() / 3);

	int k = 0;
	for (int i = 0; i < (vertices.size() / 3); i++)
	{
		aiFace& face = pMesh->mFaces[i];
		face.mIndices = new unsigned int[3];
		face.mNumIndices = 3;

		face.mIndices[0] = k;
		face.mIndices[1] = k + 1;
		face.mIndices[2] = k + 2;
		k = k + 3;
	}
}

void ModelLoader::saveMaterial(Mesh* mesh, aiScene* scene, std::string const& path)
{
	auto pMaterial = scene->mMaterials[scene->mNumMaterials - 1];

	aiColor3D ambientColor = aiColor3D(mesh->getMaterial().ambient.x, mesh->getMaterial().ambient.y, mesh->getMaterial().ambient.z);
	aiColor3D diffuseColor = aiColor3D(mesh->getMaterial().diffuse.x, mesh->getMaterial().diffuse.y, mesh->getMaterial().diffuse.z);
	aiColor3D specularColor = aiColor3D(mesh->getMaterial().specular.x, mesh->getMaterial().specular.y, mesh->getMaterial().specular.z);
	aiColor3D emissiveColor = aiColor3D(mesh->getMaterial().emission.x, mesh->getMaterial().emission.y, mesh->getMaterial().emission.z);
	float shininess = mesh->getMaterial().shininess;

	pMaterial->AddProperty(&ambientColor, 1, AI_MATKEY_COLOR_AMBIENT);
	pMaterial->AddProperty(&diffuseColor, 1, AI_MATKEY_COLOR_DIFFUSE);
	pMaterial->AddProperty(&specularColor, 1, AI_MATKEY_COLOR_SPECULAR);
	pMaterial->AddProperty(&emissiveColor, 1, AI_MATKEY_COLOR_EMISSIVE);
	pMaterial->AddProperty(&shininess, 1, AI_MATKEY_SHININESS);

	aiString ainame = aiString(mesh->getMaterial().name.c_str());
	pMaterial->AddProperty(&ainame, AI_MATKEY_NAME);

	if (mesh->getDiffuseMap())
	{
		aiString textureName = aiString(mesh->getDiffuseMap()->name.c_str());
		pMaterial->AddProperty(&textureName, AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0));
		std::string pathImage = path + "/" + mesh->getDiffuseMap()->name;
		mesh->getDiffuseMap()->image.save(QString::fromStdString(pathImage));
	}

	if (mesh->getSpecularMap())
	{
		aiString textureName = aiString(mesh->getSpecularMap()->name.c_str());
		pMaterial->AddProperty(&textureName, AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0));
		std::string pathImage = path + "/" + mesh->getSpecularMap()->name;
		mesh->getSpecularMap()->image.save(QString::fromStdString(pathImage));
	}

	if (mesh->getNormalMap())
	{
		aiString textureName = aiString(mesh->getNormalMap()->name.c_str());
		pMaterial->AddProperty(&textureName, AI_MATKEY_TEXTURE(aiTextureType_NORMALS, 0));
		std::string pathImage = path + "/" + mesh->getNormalMap()->name;
		mesh->getNormalMap()->image.save(QString::fromStdString(pathImage));
	}
}

void ModelLoader::getFaceVertices(const HalfEdge::HalfEdgeTable& halfEdgeTable, const HalfEdge::FaceHandle& fh,
	std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& textureCoords)
{
	std::vector<glm::vec3> tempVertices;

	HalfEdge::HalfEdgeHandle heh = halfEdgeTable.deref(fh).heh;
	HalfEdge::HalfEdgeHandle nextHeh = heh;
	HalfEdge::HalfEdgeHandle startHeh;
	while (nextHeh.index != startHeh.index)
	{
		auto& twin = halfEdgeTable.deref(nextHeh).twin;
		tempVertices.push_back(halfEdgeTable.getPoint(halfEdgeTable.deref(twin).dst));

		nextHeh = halfEdgeTable.next((nextHeh));

		if (startHeh.index == -1)
			startHeh = heh;
	}

	// Save first triangle
	vertices = { tempVertices[0], tempVertices[1], tempVertices[2] };
	normals = { halfEdgeTable.deref(fh).normal, halfEdgeTable.deref(fh).normal, halfEdgeTable.deref(fh).normal };
	if (!halfEdgeTable.deref(fh).textureCoords.empty())
	{
		textureCoords = { halfEdgeTable.deref(fh).textureCoords[0], halfEdgeTable.deref(fh).textureCoords[1], halfEdgeTable.deref(fh).textureCoords[2] };
	}

	if (tempVertices.size() == 4)
	{
		// Then it is quad face and we need to save second triangle
		vertices.push_back(tempVertices[2]);
		vertices.push_back(tempVertices[3]);
		vertices.push_back(tempVertices[0]);

		normals.push_back(halfEdgeTable.deref(fh).normal);
		normals.push_back(halfEdgeTable.deref(fh).normal);
		normals.push_back(halfEdgeTable.deref(fh).normal);

		if (!halfEdgeTable.deref(fh).textureCoords.empty())
		{
			textureCoords.push_back(halfEdgeTable.deref(fh).textureCoords[2]);
			textureCoords.push_back(halfEdgeTable.deref(fh).textureCoords[3]);
			textureCoords.push_back(halfEdgeTable.deref(fh).textureCoords[0]);
		}
	}
}

int ModelLoader::getMeshCount(const Model& model)
{
	std::vector<Node*> nodes = getNodeList(model);
	int meshCount = 0;
	for (const auto& node : nodes)
	{
		meshCount += node->getMeshes().size();
	}
	return meshCount;
}
