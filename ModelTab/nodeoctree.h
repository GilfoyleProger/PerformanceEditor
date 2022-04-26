#pragma once

#include "FaceOctree.h"
#include "Octree.h"
#include "Contact.h"
#include "BboxHelper.h"
#include "Model.h"
#include <map>

class NodeOctree : public Octree
{
public:
	NodeOctree(const bbox& box);

	void addNode(Node* node);
	void removeNode(Node* node);
	void updateNode(Node* node);
	std::vector<Contact> getCandidatesToContact(const ray& ray);
private:
	using MeshToFaceOctreePair = std::pair<Mesh*, std::unique_ptr<FaceOctree>>;

	struct OctreeNode
	{
		std::vector<std::unique_ptr<OctreeNode>> childrens;
		std::map<Node*, std::vector<MeshToFaceOctreePair>> values;
	};
	std::unique_ptr<OctreeNode> rootOctreeNode;

	bool isLeaf(OctreeNode* octreeNode);
	bbox computeOctantBox(const bbox& parentOctant, OctantPlace octantPlace);

	void addNode(OctreeNode* octreeNode, int depth, const bbox& box, Node* node);
	void removeNode(OctreeNode* octreeNode, OctreeNode* parentOctreeNode, const bbox& box, Node* node);
	void reinsertNode(OctreeNode* octreeNode, int depth, const bbox& box, std::pair<Node*, std::vector<MeshToFaceOctreePair>> nodeToValue);

	void removeValue(OctreeNode* octreeNode, Node* node);
	void tryMerge(OctreeNode* octreeNode);

	void split(OctreeNode* octreeNode, const bbox& box);
	OctantPlace getOctant(const bbox& box, Node* node);

	void intersectedQuery(const ray& ray, OctreeNode* octNode, const bbox& octNodeBox, std::vector<FaceOctree*>& faceOctrees);

	void popAllValues(OctreeNode* octreeNode, std::vector<std::pair<Node*, std::vector<MeshToFaceOctreePair>>>& values);
	void popValue(OctreeNode* octreeNode, Node* node, std::pair<Node*, std::vector<MeshToFaceOctreePair>>& values);
};
