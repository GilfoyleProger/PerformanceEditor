#include "viewport.h"
#include <glm/gtx/transform.hpp>
#include <glm/ext/matrix_projection.hpp>

glm::mat4 Viewport::calcProjectionMatrix() const
{
	if (isParallelProj)
	{
		double height = calcTargetPlaneHeight() / 2.0;
		double width = calcTargetPlaneWidth() / 2.0;

		return glm::ortho(-width, width, -height, height, zNear, zFar);
	}

	double tangent = glm::tan(glm::radians(fov / 2.0));
	double height = zNear * tangent;
	double width = height * calcAspectRatio();

	return glm::frustum(-width, width, -height, height, zNear, zFar);
}

void Viewport::setViewportSize(uint32_t inWidth, uint32_t inHeight)
{
	viewportWidth = inWidth;
	viewportHeight = inHeight;
}

void Viewport::setFOV(double inFOV)
{
	fov = inFOV;
}

void Viewport::setZNear(double inZNear)
{
	zNear = inZNear;
}

void Viewport::setZFar(double inZFar)
{
	zFar = inZFar;
}

void Viewport::setParallelProjection(bool use)
{
	isParallelProj = use;
}

double Viewport::getZNear() const
{
	return zNear;
}

double Viewport::getZFar() const
{
	return zFar;
}

double Viewport::getFov() const
{
	return fov;
}

double Viewport::getWidth() const
{
	return viewportWidth;
}

double Viewport::getHeight() const
{
	return viewportHeight;
}

bool Viewport::isParallelProjection() const
{
	return isParallelProj;
}

ray Viewport::calcCursorRay(double x, double y) const
{
	 y = getHeight() - y;
	glm::vec4 viewport = glm::vec4(0, 0, viewportWidth, viewportHeight);

	glm::vec3 a = glm::unProject(glm::vec3{ x, y, -1 },
		camera.calcViewMatrix(),
		calcProjectionMatrix(),
		viewport);

	glm::vec3 b = glm::unProject(glm::vec3{ x, y,  1 },
		camera.calcViewMatrix(),
		calcProjectionMatrix(),
		viewport);

	return ray{ a, glm::normalize(b - a) };
}

double Viewport::calcTargetPlaneWidth() const
{
	return calcTargetPlaneHeight() * calcAspectRatio();
}

double Viewport::calcTargetPlaneHeight() const
{
	return 2.0 * camera.distanceFromEyeToTarget() * tan(glm::radians(fov / 2.0));
}

double Viewport::calcAspectRatio() const
{
	return static_cast<double>(viewportWidth) / viewportHeight;
}

Camera& Viewport::getCamera()
{
	return camera;
}

const Camera& Viewport::getCamera() const
{
	return camera;
}

void Viewport::zoomToFit(glm::vec3 min, glm::vec3 max)
{
	camera.translate(-camera.getTarget());

	glm::vec3 boxCentre = (min + max) / 2.0f;
	camera.translate(boxCentre);

	double boundSphereRadius = glm::length(min - boxCentre);
	double camDistance = boundSphereRadius / glm::tan(glm::radians(fov / 2.0));

	camera.setDistanceToTarget(camDistance);
}
