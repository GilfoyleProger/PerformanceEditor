#pragma once

#include "node.h"

struct TileBox
{
	glm::vec3 min;
	glm::vec3 max;
};

class BuildPlate : public Node
{
public:
BuildPlate();
TileBox getTileBox(glm::vec3 point) 
{
	int tileNumInWidth = std::toupper(point.y / tileSize);
	double rightBigger = tileNumInWidth * tileSize;
	double leftLesser = rightBigger - tileSize;

}
private:
HalfEdge::HalfEdgeTable createBuildPlate(double width, double height, double depth);
double widthNum = 5;
double heightNum = 5;
double tileSize = 2;
};
