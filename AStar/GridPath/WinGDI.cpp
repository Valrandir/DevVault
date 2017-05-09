#include "WinGDI.h"

void WinGDI::AdjustWindowRect(DWORD Style, int& Width, int& Height)
{
	RECT rect = {0};
	rect.right = Width;
	rect.bottom = Height;
	::AdjustWindowRect(&rect, Style, FALSE);
	Width = rect.right - rect.left;
	Height = rect.bottom - rect.top;
}

void WinGDI::SetupBackBuffer(HWND hWnd, int Width, int Height)
{
	RECT rect = {0,0,Width,Height};
	HDC hDC = GetDC(hWnd);
	hBackDC = CreateCompatibleDC(hDC);
	hBackBitmap = CreateCompatibleBitmap(hDC, Width, Height);
	SelectObject(hBackDC, hBackBitmap);
	ReleaseDC(hWnd, hDC);
	FillRect(hBackDC, &rect, GetSysColorBrush(COLOR_BTNFACE));
}

LRESULT CALLBACK WinGDI::WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
		case WM_PAINT:
			((WinGDI*)GetWindowLong(hWnd, 0))->OnPaint();
			break;
		case WM_LBUTTONDOWN:
			((WinGDI*)GetWindowLong(hWnd, 0))->OnClick(lParam);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
	return 0;
}

void WinGDI::OnPaint()
{
	int x, y, cx, cy;
	PAINTSTRUCT ps;
	BeginPaint(hWnd, &ps);
	x = ps.rcPaint.left;
	y = ps.rcPaint.top;
	cx = ps.rcPaint.right - x;
	cy = ps.rcPaint.bottom - y;
	BitBlt(ps.hdc, x, y, cx, cy, hBackDC, x, y, SRCCOPY);
	EndPaint(hWnd, &ps);
}

void WinGDI::OnClick(LPARAM lParam)
{
	if(EventOnClick)
		EventOnClick(LOWORD(lParam), HIWORD(lParam), EventOnClickParam);
}

WinGDI::WinGDI(HINSTANCE hInstance, LPCTSTR ClassName, LPCTSTR Caption, int Width, int Height)
{
	DWORD Style;
	WNDCLASSEX wc;

	EventOnClick = NULL;
	EventOnClickParam = NULL;
	this->hInstance = hInstance;
	ClientRect.left = 0;
	ClientRect.top = 0;
	ClientRect.right = Width;
	ClientRect.bottom = Height;

	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.cbWndExtra = sizeof(this);
	wc.lpszClassName = ClassName;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&wc);

	Style = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
	AdjustWindowRect(Style, Width, Height);

	hWnd = CreateWindowEx(0, ClassName, Caption, Style, CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, HWND_DESKTOP, NULL, hInstance, NULL);
	SetWindowLong(hWnd, 0, (LONG)this);

	SetupBackBuffer(hWnd, Width, Height);
	ShowWindow(hWnd, SW_SHOW);
}

WinGDI::~WinGDI()
{
	if(hBackBitmap)
	{
		DeleteObject(hBackBitmap);
		hBackBitmap = NULL;
	}

	if(hBackDC)
	{
		ReleaseDC(hWnd, hBackDC);
		hBackDC = NULL;
	}
}

BOOL WinGDI::Tick()
{
	MSG Msg;

	while(PeekMessage(&Msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);

		if(Msg.message == WM_QUIT)
			return false;
	}

	return true;
}

void WinGDI::Update()
{
	InvalidateRect(hWnd, NULL, FALSE);
}

void WinGDI::	SetEventOnClick(void (*EventOnClick)(int, int, void*), void* EventOnClickParam)
{
	this->EventOnClick = EventOnClick;
	this->EventOnClickParam = EventOnClickParam;
}

HDC WinGDI::GetBackDC()
{
	return hBackDC;
}

int WinGDI::GetWidth()
{
	return ClientRect.right - ClientRect.left;
}

int WinGDI::GetHeight()
{
	return ClientRect.bottom - ClientRect.top;
}
