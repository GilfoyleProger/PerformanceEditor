#include "NodeOctree.h"

NodeOctree::NodeOctree(const bbox& box)
{
	rootOctreeNode = std::make_unique<OctreeNode>();
	rootOctreeNode->childrens.resize(8);
	rootBoundingBox = box;
	threshold = 1;
	maxDepth = 10;
}

void NodeOctree::addNode(Node* node)
{
	bbox nodeBbox = node->getWorldBbox();

	if (BboxHelper::boxInsideBox(nodeBbox, rootBoundingBox))
	{
		addNode(rootOctreeNode.get(), 0, rootBoundingBox, node);
	}
	else
	{
		std::vector<std::pair<Node*, std::vector<MeshToFaceOctreePair>>> nodesToReinsert;
		popAllValues(rootOctreeNode.get(), nodesToReinsert);

		rootOctreeNode->childrens.clear();
		rootOctreeNode->childrens.resize(8);
		rootOctreeNode->values.clear();

		rootBoundingBox = BboxHelper::combineBoxes(nodeBbox, rootBoundingBox);

		for (auto& node : nodesToReinsert)
			reinsertNode(rootOctreeNode.get(), 0, rootBoundingBox, std::move(node));

		addNode(node);
	}
}

void NodeOctree::removeNode(Node* node)
{
	removeNode(rootOctreeNode.get(), 0, rootBoundingBox, node);
}

void NodeOctree::updateNode(Node* node)
{
	std::vector<Node*> nodeList = BboxHelper::getNodeList(node);

	std::vector<std::pair<Node*, std::vector<MeshToFaceOctreePair>>> nodesToUpdate;
	for (auto& node : nodeList)
	{
		std::pair<Node*, std::vector<MeshToFaceOctreePair>> values;
		popValue(rootOctreeNode.get(), node, values);
		if (!values.second.empty())
		{
			nodesToUpdate.emplace_back(std::move(values));
		}
	}

	bool isNeedUpdateNodes = false;
	bbox newBbox = BboxHelper::calcNodesBoundingBox(nodeList);
	if (!BboxHelper::boxInsideBox(newBbox, rootBoundingBox))
	{
		rootBoundingBox = BboxHelper::combineBoxes(newBbox, rootBoundingBox);
		isNeedUpdateNodes = true;
	}

	if (isNeedUpdateNodes)
	{
		std::vector<std::pair<Node*, std::vector<MeshToFaceOctreePair>>> nodesToReinsert;
		popAllValues(rootOctreeNode.get(), nodesToReinsert);

		for (auto& node : nodesToReinsert)
		{
			bbox nodeBbox = node.first->getWorldBbox();
			rootBoundingBox = BboxHelper::combineBoxes(nodeBbox, rootBoundingBox);
		}

		rootOctreeNode->childrens.clear();
		rootOctreeNode->childrens.resize(8);
		rootOctreeNode->values.clear();

		for (auto& node : nodesToReinsert)
		{
			reinsertNode(rootOctreeNode.get(), 0, rootBoundingBox, std::move(node));
		}
	}

	for (auto& node : nodesToUpdate)
	{
		addNode(node.first);
		//reinsertNode(rootOctreeNode.get(), 0, rootBoundingBox, std::move(node));
	}
}

std::vector<Contact> NodeOctree::getCandidatesToContact(const ray& ray)
{
	std::vector<FaceOctree*> faceOctreeList;
	intersectedQuery(ray, rootOctreeNode.get(), rootBoundingBox, faceOctreeList);

	std::vector<Contact> candidatesToContact;

	for (auto& faceOctree : faceOctreeList)
	{
		std::vector<HalfEdge::FaceHandle> faceList = faceOctree->getFacesOfIntersectedOctants(ray);

		for (auto& face : faceList)
			candidatesToContact.emplace_back(Contact{ face, faceOctree->getMesh(), faceOctree->getNode(),{0,0,0} });
	}

	return candidatesToContact;
}

void NodeOctree::intersectedQuery(const ray& ray, OctreeNode* octNode, const bbox& octNodeBox, std::vector<FaceOctree*>& faceOctrees)
{
	if (isLeaf(octNode))
	{
		for (const auto& value : octNode->values)
		{
			for (const auto& meshToFaceOctree : value.second)
			{
				FaceOctree* faceOctree = meshToFaceOctree.second.get();
				if (BboxHelper::rayIntersectBox(ray, meshToFaceOctree.first->getBoundingBox(value.first->calcAbsoluteTransform())))
				{
					faceOctrees.push_back(faceOctree);
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < octNode->childrens.size(); i++)
		{
			bbox box = computeOctantBox(octNodeBox, static_cast<OctantPlace>(i));
			if (BboxHelper::rayIntersectBox(ray, box))
				intersectedQuery(ray, octNode->childrens[i].get(), box, faceOctrees);
		}

		for (const auto& value : octNode->values)
		{
			for (const auto& meshToFaceOctree : value.second)
			{
				FaceOctree* faceOctree = meshToFaceOctree.second.get();
				if (BboxHelper::rayIntersectBox(ray, meshToFaceOctree.first->getBoundingBox(value.first->calcAbsoluteTransform())))
				{
					faceOctrees.push_back(faceOctree);
				}
			}
		}
	}
}

void NodeOctree::popAllValues(OctreeNode* octreeNode, std::vector<std::pair<Node*, std::vector<MeshToFaceOctreePair>>>& values)
{
	if (octreeNode != nullptr)
	{
		for (auto& value : octreeNode->values) {
			values.emplace_back(value.first, std::move(value.second));

		}
		octreeNode->values.clear();
		if (!isLeaf(octreeNode))
			for (auto& child : octreeNode->childrens)
				popAllValues(child.get(), values);
	}
}

void NodeOctree::popValue(OctreeNode* octreeNode, Node* node, std::pair<Node*, std::vector<MeshToFaceOctreePair>>& valuess)
{
	if (node != nullptr)
	{
		auto iter = octreeNode->values.find(node);
		if (iter != octreeNode->values.end())
		{
			valuess = std::move(*iter);
			octreeNode->values.erase(iter);
			return;
		}

		if (!isLeaf(octreeNode))
		{
			for (auto& child : octreeNode->childrens)
				popValue(child.get(), node, valuess);
		}
	}
}

void NodeOctree::reinsertNode(OctreeNode* octreeNode, int depth, const bbox& box, std::pair<Node*, std::vector<MeshToFaceOctreePair>> nodeToValue)
{
	if (isLeaf(octreeNode))
	{
		if (depth >= maxDepth || octreeNode->values.size() < threshold)
		{
			std::vector<MeshToFaceOctreePair> values;
			for (auto& mesh : nodeToValue.second)
			{
				std::unique_ptr<FaceOctree> faceOctree = std::move(mesh.second);
				faceOctree->updateRootBox(mesh.first->getBoundingBox(nodeToValue.first->calcAbsoluteTransform()));
				MeshToFaceOctreePair meshToFaceOctree = std::make_pair(mesh.first, std::move(faceOctree));
				values.push_back(std::move(meshToFaceOctree));
			}
			octreeNode->values.emplace(nodeToValue.first, std::move(values));
		}
		else
		{
			split(octreeNode, box);
			reinsertNode(octreeNode, depth, box, std::move(nodeToValue));
		}
	}
	else
	{
		OctantPlace octantPlace = getOctant(box, nodeToValue.first);

		if (octantPlace != OctantPlace::Indefinite)
		{
			reinsertNode(octreeNode->childrens[static_cast<int>(octantPlace)].get(), depth + 1, computeOctantBox(box, octantPlace), std::move(nodeToValue));
		}
		else
		{
			std::vector<MeshToFaceOctreePair> values;
			for (auto& mesh : nodeToValue.second)
			{
				std::unique_ptr<FaceOctree> faceOctree = std::move(mesh.second);
				faceOctree->updateRootBox(mesh.first->getBoundingBox(nodeToValue.first->calcAbsoluteTransform()));
				MeshToFaceOctreePair meshToFaceOctree = std::make_pair(mesh.first, std::move(faceOctree));
				values.push_back(std::move(meshToFaceOctree));
			}
			octreeNode->values.emplace(nodeToValue.first, std::move(values));
		}
	}
}

void NodeOctree::addNode(OctreeNode* octreeNode, int depth, const bbox& box, Node* node)
{
	if (isLeaf(octreeNode))
	{
		if (depth >= maxDepth || octreeNode->values.size() < threshold)
		{
			std::vector<MeshToFaceOctreePair> values;
			for (auto& mesh : node->getMeshes())
			{
				std::unique_ptr<FaceOctree> faceOctree = std::make_unique<FaceOctree>(node, mesh.get());
				MeshToFaceOctreePair meshToFaceOctree = std::make_pair(mesh.get(), std::move(faceOctree));
				values.push_back(std::move(meshToFaceOctree));
			}
			octreeNode->values.emplace(node, std::move(values));
		}
		else
		{
			split(octreeNode, box);
			addNode(octreeNode, depth, box, node);
		}
	}
	else
	{
		OctantPlace octantPlace = getOctant(box, node);

		if (octantPlace != OctantPlace::Indefinite)
		{
			addNode(octreeNode->childrens[static_cast<int>(octantPlace)].get(), depth + 1, computeOctantBox(box, octantPlace), node);
		}
		else
		{
			std::vector<MeshToFaceOctreePair> values;
			for (auto& mesh : node->getMeshes())
			{
				std::unique_ptr<FaceOctree> faceOctree = std::make_unique<FaceOctree>(node, mesh.get());
				MeshToFaceOctreePair meshToFaceOctree = std::make_pair(mesh.get(), std::move(faceOctree));
				values.push_back(std::move(meshToFaceOctree));
			}
			octreeNode->values.emplace(node, std::move(values));
		}
	}
}

void NodeOctree::removeNode(OctreeNode* octreeNode, OctreeNode* parentOctreeNode, const bbox& box, Node* node)
{
	if (octreeNode != nullptr)
	{
		if (BboxHelper::boxInsideBox(node->getWorldBbox(), box))
		{
			if (isLeaf(octreeNode))
			{
				removeValue(octreeNode, node);

				if (parentOctreeNode != nullptr)
					tryMerge(parentOctreeNode);
			}
			else
			{
				OctantPlace octantPlace = getOctant(box, node);

				if (octantPlace != OctantPlace::Indefinite)
					removeNode(octreeNode->childrens[static_cast<int>(octantPlace)].get(), octreeNode, computeOctantBox(box, octantPlace), node);
				else
					removeValue(octreeNode, node);
			}
		}
	}
}

bool NodeOctree::isLeaf(OctreeNode* octreeNode)
{
	return (octreeNode->childrens[0] == nullptr);
}

bbox NodeOctree::computeOctantBox(const bbox& parentOctant, OctantPlace octantPlace)
{
	// Each octant has its own number. Depending on the input octantNum will be calculated
	// size of corresponding octant
	glm::vec3 origin = (parentOctant.min + parentOctant.max) / 2.0f;

	if (octantPlace == OctantPlace::BottomLeftBack)
		return bbox{ parentOctant.min, origin };
	if (octantPlace == OctantPlace::BottomRightBack)
		return bbox{ {parentOctant.min.x, origin.y, parentOctant.min.z}, {origin.x, parentOctant.max.y, origin.z} };
	if (octantPlace == OctantPlace::BottomRightFront)
		return bbox{ {origin.x, origin.y, parentOctant.min.z}, {parentOctant.max.x, parentOctant.max.y, origin.z} };
	if (octantPlace == OctantPlace::BottomLeftFront)
		return bbox{ {origin.x, parentOctant.min.y, parentOctant.min.z}, {parentOctant.max.x, origin.y, origin.z} };
	if (octantPlace == OctantPlace::TopLeftBack)
		return bbox{ {parentOctant.min.x, parentOctant.min.y, origin.z}, {origin.x, origin.y, parentOctant.max.z} };
	if (octantPlace == OctantPlace::TopRightBack)
		return bbox{ {parentOctant.min.x, origin.y, origin.z}, {origin.x, parentOctant.max.y, parentOctant.max.z} };
	if (octantPlace == OctantPlace::TopRightFront)
		return bbox{ origin, parentOctant.max };
	if (octantPlace == OctantPlace::TopLeftFront)
		return bbox{ {origin.x, parentOctant.min.y, origin.z}, {parentOctant.max.x, origin.y, parentOctant.max.z} };
	if (octantPlace == OctantPlace::Indefinite)
		return bbox{ {0, 0, 0}, {0, 0, 0} };
}

void NodeOctree::removeValue(OctreeNode* octreeNode, Node* node)
{
	auto iter = octreeNode->values.find(node);
	if (iter != octreeNode->values.end())
		octreeNode->values.erase(iter);
}

void NodeOctree::tryMerge(OctreeNode* octreeNode)
{

	if (octreeNode != nullptr)
	{
		if (!isLeaf(octreeNode))
		{
			int trianglesCount = octreeNode->values.size();
			for (auto& child : octreeNode->childrens)
			{
				if (!isLeaf(child.get()))
					return;
				trianglesCount += child->values.size();
			}

			if (trianglesCount <= threshold)
			{
				for (auto& child : octreeNode->childrens)
				{
					for (auto& node : child->values)
						octreeNode->values.emplace(std::move(node));
					child->values.clear();
				}
			}
		}
	}
}

void NodeOctree::split(OctreeNode* octreeNode, const bbox& box)
{
	if (octreeNode != nullptr)
	{
		if (isLeaf(octreeNode))
		{
			for (auto& child : octreeNode->childrens)
			{
				child = std::make_unique<OctreeNode>();
				child->childrens.resize(8);
			}

			std::map<Node*, std::vector<MeshToFaceOctreePair>> newNodes;

			for (auto& node : octreeNode->values)
			{
				OctantPlace octantPlace = getOctant(box, node.first);

				if (octantPlace != OctantPlace::Indefinite)
					octreeNode->childrens[static_cast<int>(octantPlace)]->values.emplace(std::move(node));
				else
					newNodes.emplace(std::move(node));
			}
			octreeNode->values = std::move(newNodes);
		}
	}
}

OctantPlace NodeOctree::getOctant(const bbox& parentBox, Node* node)
{
	bbox nodeBbox = node->getWorldBbox();

	for (int i = 0; i < 8; i++)
	{
		OctantPlace octantPlace = static_cast<OctantPlace>(i);
		bbox octantBbox = computeOctantBox(parentBox, octantPlace);

		if (BboxHelper::boxInsideBox(nodeBbox, octantBbox))
			return octantPlace;
	}

	return OctantPlace::Indefinite;
}
