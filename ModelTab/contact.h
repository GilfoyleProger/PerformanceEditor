#pragma once

#include "node.h"

struct Contact
{
	HalfEdge::FaceHandle face;
	Mesh* mesh;
	Node* node;
	glm::vec3 point;
};
