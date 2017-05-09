#pragma once
#include "WorldDef.h"
#include "Grid2D.h"
#include <list>

struct CommandItem
{
	int x, y;
	int Action;

	CommandItem() : x(0), y(0), Action(0) {}
	CommandItem(int x, int y, int Action) : x(x), y(y), Action(Action) {}
};

typedef std::list<CommandItem> CommandList;

struct World : public Grid2D<>
{
	int SquareWidth;
	int SquareHeight;

	CommandList cmdList;
	
	void AddDigCommand(int x, int y)
	{
		CommandItem ci(x, y, WorldDef_Action_Dig);
		cmdList.push_back(ci);
	}

	bool GrabNearestCommand(int x, int y, CommandList::const_iterator& cmd_it) const
	{
		CommandList::const_iterator ci, end, closest;
		int distance, distance0 = -1;

		closest = end = cmdList.end();
		for(ci = cmdList.begin(); ci != end; ci++)
		{
			distance = abs(x - ci->x) + abs(y - ci->y);
			if(distance0 == -1 || distance < distance0)
			{
				distance0 = distance;
				closest = ci;
			}
		}

		if(closest == end)
			return false;
		else
		{
			cmd_it = closest;
			return true;
		}
	}

		void CompleteCommand(CommandList::const_iterator& cmd_it)
		{
			cmdList.erase(cmd_it);
		}

	World(int SquareWidth, int SquareHeight);
	virtual ~World();

	bool CanPath(int x, int y) const;
	void WorldGen();
};
