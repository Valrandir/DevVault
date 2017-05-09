#include "Viewer.h"
#include "BrushCache.h"

void DrawGrid(HDC hBackDC, int sx, int sy, int width, int height, COLORREF Color)
{
	int i;

	HPEN hPen = CreatePen(PS_SOLID, 0, Color);
	HPEN hPenPrev = (HPEN)SelectObject(hBackDC, hPen);

	for(i = 0; i <= width; i += sx)
	{
		MoveToEx(hBackDC, i, 0, NULL);
		LineTo(hBackDC, i, height);
	}

	for(i = 0; i <= height; i += sy)
	{
		MoveToEx(hBackDC, 0, i, NULL);
		LineTo(hBackDC, width, i);
	}

	SelectObject(hBackDC, hPenPrev);
	DeleteObject(hPen);
}

Viewer::Viewer(WinGDI* lpWinGDI, World* lpWorld)
{
	this->lpWinGDI = lpWinGDI;
	this->lpWorld = lpWorld;

	lpWinGDI->SetEventOnClick(EventOnClickProc, this);
	hBackDC = lpWinGDI->GetBackDC();
	Width = lpWinGDI->GetWidth();
	Height = lpWinGDI->GetHeight();

	SquareSizeX = 16;
	SquareSizeY = 16;
	VisibleSquareX = Width / SquareSizeX + (Width % SquareSizeX ? 1 : 0);
	VisibleSquareY = Height / SquareSizeY + (Height % SquareSizeY ? 1 : 0);
	ScrollX = 0;
	ScrollY = 0;
}

void Viewer::FillSquare(int x, int y, COLORREF Color)
{
	x *= SquareSizeX;
	y *= SquareSizeY;
	RECT rect = {x, y, x + SquareSizeX, y + SquareSizeY};
	FillRect(hBackDC, &rect, BrushCache::GetSolidBrush(Color));
}

void Viewer::Draw()
{
	int x, y, *p = lpWorld->GetPtr(ScrollX, ScrollY);
	for(y=0; y<VisibleSquareY; y++, p += -VisibleSquareX + lpWorld->SquareWidth)
		for(x=0; x<VisibleSquareX; x++, p++)
			FillSquare(x, y, VtoC(*p));

	DrawGrid(hBackDC, SquareSizeX, SquareSizeY, Width, Height, RGB(0x80,0x80,0x80));
}

void Viewer::Scroll(int dx, int dy)
{
	int nx, ny;

	if(!(dx || dy))
		return;

	nx  = ScrollX + dx;
	ny  = ScrollY + dy;

	if(nx >= 0 && nx <= lpWorld->SquareWidth - VisibleSquareX && ny >= 0 && ny <= lpWorld->SquareHeight - VisibleSquareY)
	{
		ScrollX = nx;
		ScrollY = ny;
	}
}

void Viewer::EventOnClickProc(int x, int y, void* EventOnClickParam)
{
	int px, py;
	Viewer* lpViewer = (Viewer*)EventOnClickParam;

	px = x / lpViewer->SquareSizeX + lpViewer->ScrollX;
	py = y / lpViewer->SquareSizeY + lpViewer->ScrollY;

	lpViewer->lpWorld->AddDigCommand(px, py);
}
