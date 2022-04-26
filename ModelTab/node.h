#pragma once

#include <string>
#include <memory>

#include <glm/glm.hpp>

#include "mesh.h"
#include <vector>

class Node
{
public:
	Node();
	virtual ~Node();

	void setName(const std::string& inName);
	const std::string& getName() const;

	void attachMesh(std::unique_ptr<Mesh> inMesh);
	//Mesh* getMesh() const;
	const std::vector<std::unique_ptr<Mesh>>& getMeshes() const;

	void setParent(Node* inParent);
	Node* getParent() const;

	void setRelativeTransform(const glm::mat4& trf);
	const glm::mat4& getRelativeTransform() const;

	void applyRelativeTransform(const glm::mat4& trf);

	const std::vector<std::unique_ptr<Node>>& getChildren() const;

	glm::mat4 calcAbsoluteTransform() const;

	void attachNode(std::unique_ptr<Node> newNode);
	void deleteFromParent();

	bbox getLocalBbox();
	bbox getWorldBbox();
private:
	Node* parentPtr;
	std::vector<std::unique_ptr<Node>> childrens;
	std::vector<std::unique_ptr<Mesh>> meshes;
	glm::mat4 relativeTransform;
	glm::mat4 absoluteTransform;
	std::string nodeName;
	bool isNeedRecalcAbsoluteTransform;

	void setNeedRecalcAbsoluteTransform(Node* node);
};
