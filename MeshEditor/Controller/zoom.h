#pragma once

#include "controller.h"

class Zoom : public Controller
{
private:
	void onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y) override;
private:
	double zoomStep = 0.01;
};
