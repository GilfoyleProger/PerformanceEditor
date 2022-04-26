#include "zoom.h"
#include "View/view.h"

void Zoom::onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y)
{
	if (button == ButtonCode::ScrollButton)
		view.getViewport().getCamera().zoom(y * zoomStep);
}
