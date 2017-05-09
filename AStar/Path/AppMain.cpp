#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "WinGDI.h"
#include "World.h"
#include "Viewer.h"
#include "Actor.h"
#include "LoadSave.h"
#include "Path.h"

#define Delete(o)if(o){delete (o);(o)=0;}

void Init(HINSTANCE hInstance);
void Quit();
void Keyboard();
bool CanPath(int x, int y, void* param);
void WorldSave();
void WorldLoad();

WinGDI *lpWinGDI;
World *lpWorld;
Viewer *lpViewer;
Actor *lpActor;
Path *lpPath;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	Init(hInstance);

	lpActor->Move(0, 0);

	while(lpWinGDI->Tick())
	{
		Keyboard();

		lpActor->Tick();

		lpViewer->Draw();
		lpWinGDI->Update();

		Sleep(1);
	}

	Quit();
	return 0;
}

void Init(HINSTANCE hInstance)
{
	lpWinGDI = new WinGDI(hInstance, TEXT("WinGDI"), TEXT("DocPointDF"), 320, 240);
	lpWorld = new World(512, 512);
	lpViewer = new Viewer(lpWinGDI, lpWorld);
	lpActor = new Actor(lpWorld);
	lpPath = new Path();
}

void Quit()
{
	Delete(lpPath);
	Delete(lpActor);
	Delete(lpViewer);
	Delete(lpWorld);
	Delete(lpWinGDI);
}

void Keyboard()
{
	int dx = 0;
	int dy = 0;

	//Scroll
	if(GetAsyncKeyState(VK_SHIFT))
	{
		if(GetAsyncKeyState(VK_LEFT)) dx=-1;
		if(GetAsyncKeyState(VK_RIGHT)) dx=1;
		if(GetAsyncKeyState(VK_UP)) dy=-1;
		if(GetAsyncKeyState(VK_DOWN)) dy=1;
		if(GetAsyncKeyState(VK_HOME)) { dx=-1; dy=-1; }
		if(GetAsyncKeyState(VK_PRIOR)) { dx=1; dy=-1; }
		if(GetAsyncKeyState(VK_END)) { dx=-1; dy=1; }
		if(GetAsyncKeyState(VK_NEXT)) { dx=1; dy=1; }

		if(dx || dy)
		{
			lpViewer->Scroll(dx, dy);
			Sleep(1);
			return;
		}
	}

	//Actor Move
	if(GetAsyncKeyState(VK_LEFT)) dx=-1;
	if(GetAsyncKeyState(VK_RIGHT)) dx=1;
	if(GetAsyncKeyState(VK_UP)) dy=-1;
	if(GetAsyncKeyState(VK_DOWN)) dy=1;
	if(GetAsyncKeyState(VK_HOME)) { dx=-1; dy=-1; }
	if(GetAsyncKeyState(VK_PRIOR)) { dx=1; dy=-1; }
	if(GetAsyncKeyState(VK_END)) { dx=-1; dy=1; }
	if(GetAsyncKeyState(VK_NEXT)) { dx=1; dy=1; }
	if(dx || dy)
	{
		lpActor->Move(dx, dy);
		return;
	}

	//Actor Action
	if(GetAsyncKeyState(VK_SPACE))
	{
		lpActor->Action();
		return;
	}

	//Load / Save
	if(GetAsyncKeyState(VK_F6))
		WorldSave();
	else if(GetAsyncKeyState(VK_F7))
		WorldLoad();

	//Path Debug
	if(GetAsyncKeyState(TCHAR('P')))
	{	
		lpWorld->SwapType(3, 1);
		lpWorld->SwapType(4, 1);

		Path::Point Source = {0,0};
		Path::Point Target = {25,25};
		lpPath->Setup(Source, Target, lpWorld->SquareWidth, CanPath, 0);
		int ret = lpPath->Process();
		if(ret == 1)
		{
			Path::PointList* PointList;
			PointList = lpPath->TracePath();
			delete PointList;
		}
	}

}

bool CanPath(int x, int y, void* param)
{
	return lpWorld->CanPath(x, y);
}

void WorldSave()
{
	LoadSave::SaveWorld(TEXT("World.df"), lpWorld);
}

void WorldLoad()
{
	LoadSave::LoadWorld(TEXT("World.df"), lpWorld);
}
