#pragma once

#include "Octree.h"
#include "BboxHelper.h"
#include "Model.h"
#include <set>

struct Triangle
{
	glm::vec3 A, B, C;
};

class FaceOctree : public Octree
{
public:
	FaceOctree(Node* node, Mesh* mesh);

	void addFace(const HalfEdge::FaceHandle& fh);
	void removeFace(const HalfEdge::FaceHandle& fh);
	void updateFace(const HalfEdge::FaceHandle& fh);
	std::vector<HalfEdge::FaceHandle> getFacesOfIntersectedOctants(const ray& ray);
	Node* getNode() const;
	Mesh* getMesh();
	void updateRootBox(bbox box) 
	{
		rootBoundingBox = box;
	}
private:
	struct OctreeNode
	{
		std::vector<std::unique_ptr<OctreeNode>> childrens;
		std::set<int> values;
	};
	std::unique_ptr<OctreeNode> rootOctreeNode;
	Node* node;
	Mesh* mesh;

	bool isLeaf(OctreeNode* octreeNode);
	bbox computeOctantBox(const bbox& parentOctant, OctantPlace octantPlace);

	void addFace(OctreeNode* octreeNode, int depth, const bbox& box, const HalfEdge::FaceHandle& fh);
	void removeFace(OctreeNode* octreeNode, const HalfEdge::FaceHandle& fh);

	void removeValue(OctreeNode* octreeNode, const HalfEdge::FaceHandle& fh);
	void tryMerge(OctreeNode* octreeNode);

	void split(OctreeNode* octreeNode, const bbox& box);
	OctantPlace getOctant(const bbox& box, const HalfEdge::FaceHandle& fh);

	bool faceInsideBbox(const HalfEdge::FaceHandle& fh, const bbox& box);
	bool triangleInsideBbox(const Triangle& triangle, const bbox& box);

	std::vector<Triangle> getTriangles(const HalfEdge::FaceHandle& fh);
	void intersectedQuery(const ray& ray, OctreeNode* octNode, const bbox& octNodeBox, std::vector<HalfEdge::FaceHandle>& triangles);

	void removeFaceByValue(OctreeNode* octreeNode, const HalfEdge::FaceHandle& fh);
	void collectAllValues(OctreeNode* octreeNode, std::vector<HalfEdge::FaceHandle>& values);
};
