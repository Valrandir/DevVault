#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "WorldDef.h"

int VtoC(int v)
{
	int c;
	switch(v)
	{
		case WorldDef_Ground: c = RGB(192,192,192); break;
		case WorldDef_Rock: c = RGB(128,128,128); break;
		case WorldDef_Water: c = RGB(128,128,160); break;
		case WorldDef_Tree: c = RGB(128,160,128); break;
		default: c = 0;
	}
	return c;
}
