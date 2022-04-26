
#pragma once

#include "controller.h"
#include <glm/glm.hpp>
#include "node.h"
class AddRoadTileController : public Controller
{
public:
    //void onMouseMove(View& view, double x, double y) override;
    void onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y) override;
private:
	void updateNodes(const std::vector<std::unique_ptr<Node>>& nodes);
	//{
	/*
		for (const auto& node : nodes)
		{
			if (node->getMesh() != nullptr)
			{
				std::string vertexShaderPath = "C:/Users/Stepan/Documents/Thesis/ViaFactorem/GLRenderEngine/shaders/vertexShader.shader";
				std::string fragmentShaderPath = "C:/Users/Stepan/Documents/Thesis/ViaFactorem/GLRenderEngine/shaders/fragmentShader.shader";
				node->getMesh()->setShaderPaths(vertexShaderPath, fragmentShaderPath);
				node->getMesh()->setShaderUniform("color", QVector3D(0, 0, 1));
			}
			updateNodes(node->getChildren());
		}*/
	//}
//	bool isOrbitModeEnabled = false;
//	double prevCursorX = 0.0;
//	double prevCursorY = 0.0;

    //void orbitMode(View& view, double prevPosX, double prevPosY, double currPosX, double currPosY);
    //glm::vec3 getArcballVec(View& view, double xPos, double yPos);
//	void convertToRange(double& oldValue, double oldMin, double oldMax, double newMin, double newMax);
};
