#include "directionallight.h"

DirectionalLight::DirectionalLight()
{
	std::unique_ptr<Mesh> meshPtr = std::make_unique<Mesh>(createCube(1.0));
	Node::attachMesh(std::move(meshPtr));
}
