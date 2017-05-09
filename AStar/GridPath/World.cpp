#include "World.h"
#include "WorldDef.h"

World::World(int SquareWidth, int SquareHeight) :
	Grid2D(SquareWidth, SquareHeight),
	SquareWidth(SquareWidth),
	SquareHeight(SquareHeight)
{
	WorldGen();
}

World::~World()
{
}

bool World::CanPath(int x, int y) const
{
	int val = GetVal(x, y);
	return val == WorldDef_Ground || val == WorldDef_Water;
}

//Left side Ground, Right side Rock
void World::WorldGen()
{
	int x = (GridSX >> 1) - 1;
	FillRectangle(0, 0, x, GridSY - 1, WorldDef_Ground);
	FillRectangle(x + 1, 0, GridSX - 1, GridSY - 1, WorldDef_Rock);
}
