#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#include<iostream>

CONST CHAR g_sz_MY_WINDOW_CLASS[] = "My Window";

INT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));

	wc.style = 0;
	wc.cbSize = sizeof(wc);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;

	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

	wc.hInstance = hInstance;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_sz_MY_WINDOW_CLASS;
	wc.lpfnWndProc = (WNDPROC)WndProc;

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Class registration failed", NULL, MB_OK | MB_ICONERROR);
		return 0;
	}

	HWND hwnd = CreateWindowEx
	(
		NULL,
		g_sz_MY_WINDOW_CLASS,
		g_sz_MY_WINDOW_CLASS,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL
	);

	RECT window;
	RECT screen;

	GetWindowRect(hwnd, &window);
	GetWindowRect(GetDesktopWindow(), &screen);

	INT sizeX = (screen.right - screen.left) * 0.75;
	INT sizeY = (screen.bottom - screen.top) * 0.75;

	SetWindowPos(hwnd, NULL, 0, 0, sizeX, sizeY, SWP_NOMOVE);

	GetWindowRect(hwnd, &window);
	GetWindowRect(GetDesktopWindow(), &screen);

	INT X = (screen.right - screen.left - (window.right - window.left)) / 2;
	INT Y = (screen.bottom - screen.top - (window.bottom - window.top)) / 2;

	SetWindowPos(hwnd, NULL, X, Y, 0, 0, SWP_NOSIZE);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.message;
}

INT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
		break;
	case WM_MOVE:
	case WM_SIZE:
	{
		RECT wnd;
		GetWindowRect(hwnd, &wnd);
		printf("Location: %ix%i;\nSize:\t%ix%i;", wnd.left, wnd.top, wnd.right, wnd.bottom);
		printf("\n-----------------------------------------\n");

		INT window_width = wnd.right - wnd.left;
		INT window_height = wnd.bottom - wnd.top;

		CONST INT SIZE = 256;
		CHAR title[SIZE]{};
		
		sprintf(title, "Location: %ix%i: Size: %ix%i", wnd.left, wnd.top, window_width, window_height);
		SetWindowText(hwnd, title);
	}
	break;
	case WM_COMMAND:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return FALSE;
}