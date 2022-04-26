#include "camera.h"
#include <glm/gtx/transform.hpp>
#include <algorithm>

glm::mat4 Camera::calcViewMatrix() const
{
	return glm::lookAt(eye, target, up);
}

glm::vec3 Camera::calcForward() const
{
	return glm::normalize(glm::vec3(target - eye));
}

glm::vec3 Camera::calcRight() const
{
	return glm::normalize(glm::cross(calcForward(), up));
}

double Camera::distanceFromEyeToTarget() const
{
	return glm::length(target - eye);
}

const glm::vec3& Camera::getEye() const
{
	return eye;
}

const glm::vec3& Camera::getTarget() const
{
	return target;
}

const glm::vec3& Camera::getUp() const
{
	return up;
}

void Camera::setFrontView()
{
	double d = distanceFromEyeToTarget();
	setEyeTargetUp(target + glm::vec3{ 0,0,1 }, target, { 0,1,0 });
	setDistanceToTarget(d);
}

void Camera::setTopView()
{
	glm::vec3 oldTarget = target;
	setFrontView();
	rotate(oldTarget, { 1,0,0 }, glm::pi<double>() * -0.5);
}

void Camera::setRearView()
{
	glm::vec3 oldTarget = target;
	setFrontView();
	rotate(oldTarget, { 0,1,0 }, glm::pi<double>());
}

void Camera::setRightView()
{
	glm::vec3 oldTarget = target;
	setFrontView();
	rotate(oldTarget, { 0,1,0 }, glm::pi<double>() * 0.5);
}

void Camera::setLeftView()
{
	glm::vec3 oldTarget = target;
	setFrontView();
	rotate(oldTarget, { 0,1,0 }, glm::pi<double>() * -0.5);
}

void Camera::setBottomView()
{
	glm::vec3 oldTarget = target;
	setFrontView();
	rotate(oldTarget, { 1,0,0 }, glm::pi<double>() * 0.5);
}

void Camera::setIsoView()
{
	glm::vec3 oldTarget = target;
	setFrontView();
	rotate(oldTarget, { 1,0,0 }, glm::pi<double>() * -0.25);
	rotate(oldTarget, { 0,1,0 }, glm::pi<double>() * 0.25);
}

void Camera::orbit(glm::vec3 a, glm::vec3 b)
{
	if (!glm::all(glm::equal(a, b)))
	{
		float angle = std::acos(std::min(1.0f, glm::dot(a, b)));

		glm::vec3 axis = glm::cross(b, a);
		if (!glm::all(glm::equal(axis, { 0,0,0 })))
		{
			glm::mat3 transformToWorldSpace = glm::transpose(glm::mat3(calcViewMatrix()));
			glm::vec3 axisInWorldSpace = transformToWorldSpace * axis;
			glm::mat3 orbitMatrix = glm::rotate(glm::mat4(1.0f), angle, axisInWorldSpace);

			eye = target + orbitMatrix * glm::vec3(eye - target);
			up = orbitMatrix * glm::vec3(up);

			setEyeTargetUp(eye, target, up);
		}
	}
}

void Camera::pan(double u, double v)
{
	glm::vec3 panRight = calcRight() * static_cast<float>(u);
	glm::vec3 panUp = up * static_cast<float>(v);
	glm::vec3 panTransform = panRight + panUp;

	eye += panTransform;
	target += panTransform;
}

void Camera::zoom(double factor)
{
	double oldDist = distanceFromEyeToTarget();
	double newDist = oldDist - factor;

	if (newDist > 0)
		setDistanceToTarget(newDist);
}

void Camera::translate(glm::vec3 delta)
{
	eye += delta;
	target += delta;
}

void Camera::setDistanceToTarget(double d)
{
	glm::vec3 f = calcForward();
	glm::vec3 newEye = target - calcForward() * static_cast<float>(d);

	if (glm::length(target - newEye) > 0)
		eye = newEye;
}

void Camera::transform(const glm::mat4& trf)
{
	eye = trf * glm::vec4(eye, 1.0f);
	up = trf * glm::vec4(up, 0.0f);
	target = trf * glm::vec4(target, 1.0f);
	setEyeTargetUp(eye, target, up);
}

void Camera::rotate(glm::vec3 point, glm::vec3 axis, double angle)
{
	translate(-point);

	glm::mat4 trans = glm::rotate(glm::mat4(1.0f), static_cast<float>(angle), axis);
	transform(trans);

	translate(point);

	setEyeTargetUp(eye, target, up);
}

void Camera::setEyeTargetUp(glm::vec3 newEye, glm::vec3 newTarget, glm::vec3 newUp)
{
	eye = newEye;
	target = newTarget;

	glm::vec3 forward = calcForward();
	glm::vec3 axis = glm::cross(forward, newUp);
	glm::mat4 rotateMat = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), axis);

	up = rotateMat * glm::vec4(forward, 0.0f);
}

void Camera::setTarget(glm::vec3 newTarget)
{
	target = newTarget;
}
