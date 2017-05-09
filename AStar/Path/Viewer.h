#pragma once
#include "WinGDI.h"
#include "World.h"

class Viewer
{
	WinGDI* lpWinGDI;
	World* lpWorld;
	HDC hBackDC;
	int Width, Height;
	int SquareSizeX, SquareSizeY;
	int VisibleSquareX, VisibleSquareY;
	int ScrollX, ScrollY;

	static void EventOnClickProc(int x, int y, void* EventOnClickParam);

	public:
	Viewer(WinGDI* lpWinGDI, World* lpWorld);
	void FillSquare(int x, int y, COLORREF Color);
	void Draw();
	void Scroll(int dx, int dy);
};
