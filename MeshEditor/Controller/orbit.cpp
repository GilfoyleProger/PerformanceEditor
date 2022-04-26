#include "orbit.h"
#include "View/view.h"

void Orbit::onMouseMove(View& view, double x, double y)
{
	if (isOrbitModeEnabled)
	{
		orbitMode(view, prevCursorX, prevCursorY, x, y);

		prevCursorX = x;
		prevCursorY = y;
	}
}

void Orbit::onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y)
{
	if (action == Action::Press)
		isOrbitModeEnabled = button == ButtonCode::RightButton;
	else if (action == Action::Release)
		isOrbitModeEnabled = false;

	prevCursorX = x;
	prevCursorY = y;
}

void Orbit::orbitMode(View& view, double prevPosX, double prevPosY, double currPosX, double currPosY)
{
	if (prevPosX != currPosX || prevPosY != currPosY)
	{
		glm::vec3 firstPointOnSphere = getArcballVec(view, prevPosX, prevPosY);
		glm::vec3 secondPointOnSphere = getArcballVec(view, currPosX, currPosY);

		view.getViewport().getCamera().orbit(firstPointOnSphere, secondPointOnSphere);
	}
}

glm::vec3 Orbit::getArcballVec(View& view, double xPos, double yPos)
{
	convertToRange(xPos,
				   0.0,
			       view.getViewport().getWidth(),
				  -view.getViewport().calcAspectRatio(),
				   view.getViewport().calcAspectRatio());

	convertToRange(yPos,
				   0.0,
				   view.getViewport().getHeight(),
				   1.0,
				  -1.0);

	glm::vec3 pointOnSphere = glm::vec3(xPos, yPos, 0);

	if (glm::length(pointOnSphere) >= 1.0)
		pointOnSphere = glm::normalize(pointOnSphere);
	else
		pointOnSphere.z = sqrt(1.0 - pow(pointOnSphere.x, 2) - pow(pointOnSphere.y, 2));

	return pointOnSphere;
}

void Orbit::convertToRange(double& oldValue, double oldMin, double oldMax, double newMin, double newMax)
{
	double oldRange = oldMax - oldMin;
	double newRange = newMax - newMin;
	oldValue = (((oldValue - oldMin) * newRange) / oldRange) + newMin;
}
