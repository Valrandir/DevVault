#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <map>

class BrushCache
{
	typedef std::map<COLORREF,HBRUSH> BrushMap;
	BrushCache();
	static BrushMap* Cache;

	public:
	static HBRUSH GetSolidBrush(COLORREF Color);
	static void Purge();
};
