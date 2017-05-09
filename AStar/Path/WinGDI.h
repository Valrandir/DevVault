#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class WinGDI
{
	private:
	HINSTANCE hInstance;
	HWND hWnd;
	HDC hBackDC;
	HBITMAP hBackBitmap;
	RECT ClientRect;
	void (*EventOnClick)(int, int, void*);
	void* EventOnClickParam;

	void AdjustWindowRect(DWORD Style, int& Width, int& Height);
	void SetupBackBuffer(HWND hWnd, int Width, int Height);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void OnPaint();
	void OnClick(LPARAM lParam);

	public:
	WinGDI(HINSTANCE hInstance, LPCTSTR ClassName, LPCTSTR Caption, int Width, int Height);
	~WinGDI();
	BOOL Tick();
	void Update();
	void SetEventOnClick(void (*EventOnClick)(int, int, void*), void* EventOnClickParam);
	HDC GetBackDC();
	int GetWidth();
	int GetHeight();
};
