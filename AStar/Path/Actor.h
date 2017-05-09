#pragma once
#include "World.h"

struct Actor
{
	int x, y;
	int lx, ly;
	World *lpWorld;
	int cooldown;

	Actor(World *lpWorld);
	void Move(int dx, int dy);
	void Tick();
	void Action();
};
