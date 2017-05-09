#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "World.h"

class LoadSave
{
	LoadSave();

	public:
	static void SaveWorld(LPCTSTR FileName, const World *lpWorld);
	static void LoadWorld(LPCTSTR FileName, World *lpWorld);
};
