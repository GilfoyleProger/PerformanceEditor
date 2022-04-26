#pragma once

#include "controller.h"

class Pan : public Controller
{
public:
	void onMouseMove(View& view, double x, double y) override;
	void onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y) override;
private:
	bool isPanModeEnabled = false;
	double prevCursorX = 0.0;
	double prevCursorY = 0.0;

	void panMode(View& view, double prevPosX, double prevPosY, double currPosX, double currPosY);
	void convertToTargetPlaneRange(View& view, double& xPos, double& yPos);
	void convertToRange(double& oldValue, double oldMin, double oldMax, double newMin, double newMax);
};
