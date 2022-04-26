#pragma once

#include "model.h"

enum class OctantPlace
{
    BottomLeftBack = 0,
    BottomRightBack = 1,
    BottomRightFront = 2,
    BottomLeftFront = 3,
    TopLeftBack = 4,
    TopRightBack = 5,
    TopRightFront = 6,
    TopLeftFront = 7,
    Indefinite = -1,
};

class Octree
{
public:
    virtual ~Octree() {}
protected:
    bbox rootBoundingBox;
    int threshold;
    int maxDepth;
};
