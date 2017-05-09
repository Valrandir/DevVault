#pragma once
#include "World.h"
#include "Path.h"

struct Actor
{
	int x, y;
	int lx, ly;
	World *lpWorld;
	int MoveCooldown;
	bool Busy;
	CommandList::const_iterator CommandIterator;
	CommandItem Command;
	Path::PointList* PathList;

	Actor(World *lpWorld);
	void Move(int dx, int dy);
	void Tick();
	void Action();
	static bool CanPath(int x, int y, void* param);
};
