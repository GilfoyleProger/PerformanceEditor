#include "pan.h"
#include "View/view.h"

void Pan::onMouseMove(View& view, double x, double y)
{
	if (isPanModeEnabled)
	{
		panMode(view, prevCursorX, prevCursorY, x, y);

		prevCursorX = x;
		prevCursorY = y;
	}
}

void Pan::onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y)
{
	if (action == Action::Press)
		isPanModeEnabled = button == ButtonCode::LeftButton;
	else if (action == Action::Release)
		isPanModeEnabled = false;

	prevCursorX = x;
	prevCursorY = y;
}

void Pan::panMode(View& view, double prevPosX, double prevPosY, double currPosX, double currPosY)
{
	double u0 = prevPosX;
	double v0 = view.getViewport().getHeight() - prevPosY;
	convertToTargetPlaneRange(view, u0, v0);

	double u1 = currPosX;
	double v1 = view.getViewport().getHeight() - currPosY;
	convertToTargetPlaneRange(view, u1, v1);

	double deltaU = u0 - u1;
	double deltaV = v0 - v1;

	view.getViewport().getCamera().pan(deltaU, deltaV);
}

void Pan::convertToTargetPlaneRange(View& view, double& xPos, double& yPos)
{
	convertToRange(xPos,
				   0.0,
		           view.getViewport().getWidth(),
		          -view.getViewport().calcTargetPlaneWidth() / 2.0,
				   view.getViewport().calcTargetPlaneWidth() / 2.0);

	convertToRange(yPos,
				   0.0,
				   view.getViewport().getHeight(),
				  -view.getViewport().calcTargetPlaneHeight() / 2.0,
				   view.getViewport().calcTargetPlaneHeight() / 2.0);
}

void Pan::convertToRange(double& oldValue, double oldMin, double oldMax, double newMin, double newMax)
{
	double oldRange = oldMax - oldMin;
	double newRange = newMax - newMin;
	oldValue = (((oldValue - oldMin) * newRange) / oldRange) + newMin;
}
