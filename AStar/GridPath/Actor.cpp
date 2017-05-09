#include "Actor.h"

Actor::Actor(World *lpWorld)
{
	this->lpWorld = lpWorld;
	x = y = 0;
	lx = 1;
	ly = 0;
	MoveCooldown = 0;
	Busy = false;
	PathList = NULL;
}

void Actor::Move(int dx, int dy)
{
	int nx, ny;

	if(MoveCooldown || !(dx || dy))
		return;

	nx = x + dx;
	ny = y + dy;
	lx = dx > 0 ? 1 : dx < 0 ? -1 : 0;
	ly = dy > 0 ? 1 : dy < 0 ? -1 : 0;

	if(lpWorld->CanPath(nx, ny))
	{
		lpWorld->SetPoint(x, y, WorldDef_Ground);
		lpWorld->SetPoint(nx, ny, WorldDef_Water);
		x = nx;
		y = ny;
		MoveCooldown = 10;
	}
}

void Actor::Tick()
{
	if(MoveCooldown > 0)
	{
		MoveCooldown--;
		return;
	}

	if(!Busy)
	{
		if(lpWorld->GrabNearestCommand(x, y, CommandIterator))
		{
			Command = *CommandIterator;
			Path path;
			Path::Point p1 = {x, y};
			Path::Point p2 = {Command.x, Command.y};
			path.Setup(p1, p2, lpWorld->SquareWidth, Actor::CanPath, this);
			int ret = path.Process();
			if(ret == 1)
			{
				PathList = path.TracePath();
				PathList->pop_front();
				Busy = true;
			}
		}
	}

	if(PathList && !MoveCooldown)
	{
		Path::Point pt = PathList->front();
		PathList->pop_front();

		int dx = pt.x < x ? -1 : pt.x > x ? 1 : 0;
		int dy = pt.y < y ? -1 : pt.y > y ? 1 : 0;
		Move(dx, dy);

		if(PathList->empty())
		{
			delete PathList;
			PathList = NULL;
			Busy = false;
			lpWorld->CompleteCommand(CommandIterator);
		}

	}
}

void Actor::Action()
{
	int nx, ny;

	if(!(lx || ly))
		return;

	nx = x + lx;
	ny = y + ly;

	if(!lpWorld->CanPath(nx, ny))
		lpWorld->SetPoint(nx, ny, WorldDef_Ground);
}

bool Actor::CanPath(int x, int y, void* param)
{
	return ((Actor*)param)->lpWorld->CanPath(x, y);
}
