#include "World.h"

World::World(int SquareWidth, int SquareHeight) :
	Grid2D(SquareWidth, SquareHeight),
	SquareWidth(SquareWidth),
	SquareHeight(SquareHeight)
{
	Fill(WorldDef_Ground);
	SetRectangle(4, 4, 8, 8, WorldDef_Wall);
	SetPoint(12, 12, WorldDef_Wall);
}

World::~World()
{
}

bool World::IsFree(int x, int y) const
{
	return CanPath(x, y);
}

bool World::CanPath(int x, int y) const
{
	int val = GetVal(x, y);
	return val == WorldDef_Ground || val == WorldDef_Water;
}
