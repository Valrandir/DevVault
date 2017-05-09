#pragma once
#include "WorldDef.h"
#include "Grid2D.h"

struct World : public Grid2D<>
{
	int SquareWidth;
	int SquareHeight;

	World(int SquareWidth, int SquareHeight);
	virtual ~World();

	bool IsFree(int x, int y) const;
	bool CanPath(int x, int y) const;
};
