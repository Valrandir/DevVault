#include "Actor.h"

Actor::Actor(World *lpWorld)
{
	this->lpWorld = lpWorld;
	x = y = 0;
	lx = 1;
	ly = 0;
	cooldown = 0;
}

void Actor::Move(int dx, int dy)
{
	int nx, ny;

	if(cooldown || !(dx || dy))
		return;

	nx = x + dx;
	ny = y + dy;
	lx = dx > 0 ? 1 : dx < 0 ? -1 : 0;
	ly = dy > 0 ? 1 : dy < 0 ? -1 : 0;

	if(lpWorld->IsFree(nx, ny))
	{
		lpWorld->SetPoint(x, y, WorldDef_Ground);
		lpWorld->SetPoint(nx, ny, WorldDef_Water);
		x = nx;
		y = ny;
		cooldown = 10;
	}
}

void Actor::Tick()
{
	if(cooldown > 0)
		cooldown--;
}

void Actor::Action()
{
	int nx, ny;

	if(!(lx || ly))
		return;

	nx = x + lx;
	ny = y + ly;

	if(!lpWorld->IsFree(nx, ny))
		lpWorld->SetPoint(nx, ny, WorldDef_Ground);
}
