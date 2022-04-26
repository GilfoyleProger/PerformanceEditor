#include "TranslationManipulator.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

TranslationManipulator::TranslationManipulator(glm::vec3 dirL)
{
	std::unique_ptr<Mesh> meshPtr = std::make_unique<Mesh>(getArrow(2.8, 0.08, 20));
	Manipulator::attachMesh(std::move(meshPtr));
	startPointL = glm::vec3(0.0f);
	setDirection(dirL);
}

void TranslationManipulator::handleMovement(MovementType movementType, const Viewport& viewport, double x, double y)
{
	glm::mat4 trfMat = calcAbsoluteTransform();
	glm::vec3 dirW = glm::normalize(trfMat * glm::vec4(0, 1, 0, 0));
	glm::vec3 dirWOrig = trfMat * glm::vec4(0, 0, 0, 1);
	ray cursorRayW = viewport.calcCursorRay(x, y);

	glm::vec3 tempPointW = glm::vec3(0.0f);
	constexpr float tolerance = std::numeric_limits<float>::epsilon();
	glm::vec3 orien = glm::cross(cursorRayW.dir, dirW);

	if (glm::dot(orien, orien) > 0.1f && !isParallelMode)
	{
		closestPointToSecondRay({ dirWOrig, dirW }, cursorRayW, tempPointW);
	}
	else
	{
		glm::vec3 up = glm::normalize(glm::cross(viewport.getCamera().calcRight(), viewport.getCamera().calcForward()));
		glm::mat4 rotationToUp = glm::orientation(up, dirW);
		dirWOrig = rotationToUp * glm::vec4(dirWOrig, 1.0f);
		dirW = rotationToUp * glm::vec4(dirW, 0.0f);
		closestPointToSecondRay({ dirWOrig, dirW }, cursorRayW, tempPointW);
		tempPointW = glm::inverse(rotationToUp) * glm::vec4(tempPointW, 1.0f);
		isParallelMode = true;
	}

	if (movementType == MovementType::Push)
	{
		startPointL = glm::inverse(trfMat) * glm::vec4(tempPointW, 1.0f);
	}
	else
	{
		glm::vec3 closestPointL = glm::inverse(trfMat) * glm::vec4(tempPointW, 1.0f);
		glm::vec3 deltaVec = glm::mat3(trfMat) * (closestPointL - startPointL);
		glm::mat4 deltaMat = glm::translate(glm::mat4(1.0f), deltaVec);
		sendFeedback(deltaMat);
	}

	if (movementType == MovementType::Release)
		isParallelMode = false;
}

void TranslationManipulator::setDirection(glm::vec3 inDirL)
{
	dirL = inDirL;
	glm::vec3 originArrowDir = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 newRelativeTransform = glm::mat4(1.0f);

	if (std::fabs(glm::dot(glm::normalize(inDirL), originArrowDir) + 1.0f) < std::numeric_limits<float>::epsilon())
		newRelativeTransform = glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));
	else
		newRelativeTransform = glm::orientation(glm::normalize(inDirL), originArrowDir);

	Manipulator::setRelativeTransform(newRelativeTransform);
}

bool TranslationManipulator::closestPointToSecondRay(ray firstRay, ray secondRay, glm::vec3& closestPoint)
{
	double tolerance = std::numeric_limits<double>::epsilon();

	glm::vec3 orien = glm::cross(firstRay.dir, secondRay.dir);
	double orienLength = glm::length(orien);
	if (orienLength > -tolerance && orienLength < tolerance)
		return false; // rays are parallel

	glm::vec3 perpToRaysPlane = glm::cross(firstRay.dir, secondRay.dir);
	// Compute the normal of a plane parallel to firstRay
	glm::vec3 planeNormal = glm::normalize(glm::cross(secondRay.dir, perpToRaysPlane));
	// Find the intersection between secondRay and a plane parallel to firstRay:
	// First, compute the distance from ray origin to the ray-plane intersection point
	double t = glm::dot((secondRay.orig - firstRay.orig), planeNormal) / glm::dot(firstRay.dir, planeNormal);
	// Second, compute ray-plane intersection point
	closestPoint = firstRay.orig + static_cast<float>(t) * firstRay.dir;

	return true;
}

HalfEdge::HalfEdgeTable TranslationManipulator::getCone(double radius, double height, uint32_t numSubdivisions)
{
	HalfEdge::HalfEdgeTable halfEdgeTable;

	if (numSubdivisions > 2 && radius > 0 && height > 0)
	{
		std::vector<HalfEdge::VertexHandle> baseVertices;
		const double vertexStep = 2.0 * glm::pi<double>() / numSubdivisions;

		const double baseZ = -height / 2.0;

		for (int i = 0; i < numSubdivisions; ++i)
		{
			double angle = i * vertexStep;

			double x = std::cos(angle) * radius;
			double y = std::sin(angle) * radius;

			baseVertices.emplace_back(halfEdgeTable.addVertex({ y, baseZ, x }));
		}

		HalfEdge::VertexHandle baseCenterVertex = halfEdgeTable.addVertex({ 0, baseZ, 0 });
		HalfEdge::VertexHandle apexVertex = halfEdgeTable.addVertex({ 0, height / 2.0, 0 });

		for (int i = 0; i < numSubdivisions; ++i)
		{
			HalfEdge::VertexHandle firstBaseVertex = baseVertices[i];
			HalfEdge::VertexHandle secondBaseVertex = ((i + 1) < numSubdivisions) ? baseVertices[i + 1] : baseVertices[0];

			// Add the side faces of the cylinder
			halfEdgeTable.addFace(firstBaseVertex, secondBaseVertex, apexVertex);
			// Add the base triangles of the cylinder
			halfEdgeTable.addFace(baseCenterVertex, secondBaseVertex, firstBaseVertex);
		}

		halfEdgeTable.connectTwins();
	}

	return halfEdgeTable;
}

HalfEdge::HalfEdgeTable TranslationManipulator::getCylinder(double radius, double height, uint32_t numSubdivisions)
{
	HalfEdge::HalfEdgeTable halfEdgeTable;

	if (numSubdivisions > 2 && radius > 0 && height > 0)
	{
		std::vector<HalfEdge::VertexHandle> basesVertices;
		const double vertexStep = 2.0 * glm::pi<double>() / numSubdivisions;

		for (int i = 0; i < 2; ++i)
		{
			const double h = -height / 2.0 + i * height;

			for (int j = 0; j < numSubdivisions; ++j)
			{
				double angle = j * vertexStep;

				double x = std::cos(angle) * radius;
				double y = std::sin(angle) * radius;

				basesVertices.emplace_back(halfEdgeTable.addVertex({ y, h, x }));
			}
		}

		HalfEdge::VertexHandle bottomBaseCenterVertex = halfEdgeTable.addVertex({ 0, -height / 2.0, 0 });
		HalfEdge::VertexHandle topBaseCenterVertex = halfEdgeTable.addVertex({ 0, height / 2.0, 0 });

		for (int i = 0; i < numSubdivisions; ++i)
		{
			HalfEdge::VertexHandle firstTopVertex = basesVertices[numSubdivisions + i];
			HalfEdge::VertexHandle firstBottomVertex = basesVertices[i];

			HalfEdge::VertexHandle secondTopVertex = basesVertices[numSubdivisions];
			HalfEdge::VertexHandle secondBottomVertex = basesVertices[0];

			if ((i + 1) < numSubdivisions)
			{
				secondTopVertex = basesVertices[numSubdivisions + i + 1];
				secondBottomVertex = basesVertices[i + 1];
			}

			// Add the side faces of the cylinder
			halfEdgeTable.addFace(firstBottomVertex, secondBottomVertex, secondTopVertex, firstTopVertex);
			// Add the bottom triangles of the cylinder
			halfEdgeTable.addFace(bottomBaseCenterVertex, secondBottomVertex, firstBottomVertex);
			// Add the top triangles of the cylinder
			halfEdgeTable.addFace(topBaseCenterVertex, firstTopVertex, secondTopVertex);
		}

		halfEdgeTable.connectTwins();
	}

	return halfEdgeTable;
}

HalfEdge::HalfEdgeTable TranslationManipulator::getArrow(double cylinderHeight, double cylinderRadius, uint32_t numSubdivisions)
{
	double coneRadius = cylinderRadius * 3.0f;
	double coneHeight = cylinderHeight / 5.0f;

	HalfEdge::HalfEdgeTable cylinderMesh = getCylinder(cylinderRadius, cylinderHeight, numSubdivisions);
	HalfEdge::HalfEdgeTable coneMesh = getCone(coneRadius, coneHeight, numSubdivisions);

	for (auto& vertex : cylinderMesh.getVertices())
	{
		HalfEdge::VertexHandle vh = cylinderMesh.handle(vertex);
		glm::vec3 newPoint = cylinderMesh.getPoint(vh);
		newPoint.y += (cylinderHeight / 2.0f);
		cylinderMesh.setPoint(vh, newPoint);
	}

	for (auto& vertex : coneMesh.getVertices())
	{
		HalfEdge::VertexHandle vh = coneMesh.handle(vertex);
		glm::vec3 newPoint = coneMesh.getPoint(vh);
		newPoint.y += cylinderHeight;
		coneMesh.setPoint(vh, newPoint);
	}

	cylinderMesh += coneMesh;

	return cylinderMesh;
}
