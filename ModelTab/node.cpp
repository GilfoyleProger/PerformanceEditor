#include "node.h"
#include <iostream>

Node::Node() : parentPtr(nullptr), relativeTransform(glm::mat4(1.0f)), nodeName(""),
absoluteTransform(glm::mat4(1.0f)), isNeedRecalcAbsoluteTransform(true) {}

Node::~Node() {}

void Node::setName(const std::string& inName)
{
	nodeName = inName;
}

const std::string& Node::getName() const
{
	return nodeName;
}

void Node::attachMesh(std::unique_ptr<Mesh> inMesh)
{
	if (inMesh != nullptr)
		meshes.push_back(std::move(inMesh));
}
const std::vector<std::unique_ptr<Mesh>>& Node::getMeshes() const
{
	return meshes;
}

void Node::setParent(Node* inParent)
{
	if (inParent != nullptr)
		parentPtr = inParent;
	setNeedRecalcAbsoluteTransform(this);
}

Node* Node::getParent() const
{
	return parentPtr;
}

void Node::setRelativeTransform(const glm::mat4& trf)
{
	relativeTransform = trf;
	setNeedRecalcAbsoluteTransform(this);
}

const glm::mat4& Node::getRelativeTransform() const
{
	return relativeTransform;
}

void Node::applyRelativeTransform(const glm::mat4& trf)
{
	relativeTransform = trf * relativeTransform;
	setNeedRecalcAbsoluteTransform(this);
}

const std::vector<std::unique_ptr<Node>>& Node::getChildren() const
{
	return childrens;
}

glm::mat4 Node::calcAbsoluteTransform() const
{
	if (isNeedRecalcAbsoluteTransform)
	{
		Node* thisNode = const_cast<Node*>(this);
		thisNode->isNeedRecalcAbsoluteTransform = false;

		Node* nextParentPtr = parentPtr;
		thisNode->absoluteTransform = relativeTransform;

		while (nextParentPtr != nullptr)
		{
			thisNode->absoluteTransform = nextParentPtr->getRelativeTransform() * thisNode->absoluteTransform;
			nextParentPtr = nextParentPtr->parentPtr;
		}
	}

	return absoluteTransform;
}

void Node::attachNode(std::unique_ptr<Node> newNode)
{
	if (newNode != nullptr)
	{
		try
		{
			if (newNode->getParent() != nullptr)
				throw std::logic_error("logic_error: newNode should have no parent");

			newNode->setParent(this);
			childrens.emplace_back(std::move(newNode));
		}
		catch (std::logic_error& err)
		{
			std::cout << err.what() << "\n";
		}
	}
}

void Node::deleteFromParent()
{
	if (parentPtr != 0)
	{
		for (int i = 0; i < parentPtr->childrens.size(); i++)
		{
			if (parentPtr->childrens[i].get() == this)
			{
				parentPtr->childrens[i].release();
				parentPtr->childrens.erase(parentPtr->childrens.begin() + i);
				break;
			}
		}
	}

	delete this;
}

bbox Node::getLocalBbox()
{
	bbox localBbox = bbox{ glm::vec3(0,0,0), glm::vec3(0,0,0) };
	if (!meshes.empty())
	{
		localBbox.min = meshes[0]->getBoundingBox().min;
		localBbox.max = meshes[0]->getBoundingBox().max;

		for (const auto& mesh : meshes)
		{
			std::vector<glm::vec3> points = { mesh->getBoundingBox().min, mesh->getBoundingBox().max };

			for (const auto& point : points)
			{
				// x-axis
				if (point.x < localBbox.min.x)
					localBbox.min.x = point.x;
				if (point.x > localBbox.max.x)
					localBbox.max.x = point.x;
				// y-axis
				if (point.y < localBbox.min.y)
					localBbox.min.y = point.y;
				if (point.y > localBbox.max.y)
					localBbox.max.y = point.y;
				// z-axis
				if (point.z < localBbox.min.z)
					localBbox.min.z = point.z;
				if (point.z > localBbox.max.z)
					localBbox.max.z = point.z;
			}
		}
	}
	return localBbox;
}

bbox Node::getWorldBbox()
{
	bbox worldBbox = bbox{ glm::vec3(0,0,0), glm::vec3(0,0,0) };
	if (!meshes.empty())
	{
		worldBbox = meshes[0]->getBoundingBox(calcAbsoluteTransform());

		for (const auto& mesh : meshes)
		{
			bbox meshBbox = mesh->getBoundingBox(calcAbsoluteTransform());
			std::vector<glm::vec3> points = { meshBbox.min, meshBbox.max };

			for (const auto& point : points)
			{
				// x-axis
				if (point.x < worldBbox.min.x)
					worldBbox.min.x = point.x;
				if (point.x > worldBbox.max.x)
					worldBbox.max.x = point.x;
				// y-axis
				if (point.y < worldBbox.min.y)
					worldBbox.min.y = point.y;
				if (point.y > worldBbox.max.y)
					worldBbox.max.y = point.y;
				// z-axis
				if (point.z < worldBbox.min.z)
					worldBbox.min.z = point.z;
				if (point.z > worldBbox.max.z)
					worldBbox.max.z = point.z;
			}
		}
	}
	return worldBbox;
}

void Node::setNeedRecalcAbsoluteTransform(Node* node)
{
	if (node != nullptr)
	{
		node->isNeedRecalcAbsoluteTransform = true;

		for (auto& child : node->childrens)
			setNeedRecalcAbsoluteTransform(child.get());
	}
}
