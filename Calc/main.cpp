#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <Shlwapi.h>
#include "resource.h"

#pragma comment (lib, "Shlwapi.lib")

CONST CHAR g_sz_CLASS_NAME[] = "Calc PV_319";

CONST INT g_i_INTERVAL = 5;
CONST INT g_i_BUTTON_SIZE = 50;
CONST INT g_i_BUTTON_DOUBLE_SIZE = g_i_BUTTON_SIZE * 2 + g_i_INTERVAL;

CONST INT  g_i_FONT_HEIGH = 32;
CONST INT  g_i_FONT_WIDTH = g_i_FONT_HEIGH * 2 / 5;

CONST INT g_i_DISPLAY_WIDTH = g_i_BUTTON_SIZE * 5 + g_i_INTERVAL * 4;
CONST INT g_i_DISPLAY_HEIGHT = g_i_FONT_HEIGH + 4;

CONST INT g_i_START_X = 10;
CONST INT g_i_START_Y = 10;
CONST INT g_i_BUTTON_START_X = g_i_START_X;
CONST INT g_i_BUTTON_START_Y = g_i_START_Y + g_i_DISPLAY_HEIGHT + g_i_INTERVAL;
CONST INT g_i_OPERATION_BUTTON_START_X = g_i_START_X + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 3;
CONST INT g_i_OPERATION_BUTTON_START_Y = g_i_BUTTON_START_Y;
CONST INT g_i_CONTROL_BUTTON_START_X = g_i_START_X + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 4;
CONST INT g_i_CONTROL_BUTTON_START_Y = g_i_BUTTON_START_Y;

CONST INT g_i_WINDOW_WIDTH = g_i_DISPLAY_WIDTH + g_i_START_X * 2 + 16;
CONST INT g_i_WINDOW_HEIGHT = g_i_DISPLAY_HEIGHT + g_i_START_Y * 2 + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 4 + 38;

CONST CHAR* g_OPERATIONS[] = { "+", "-", "*", "/" };

CONST COLORREF g_DISPLAY_BACKGROUND[] = { RGB(0, 0, 150), RGB(100, 100, 100) };
CONST COLORREF g_DISPLAY_FOREGROUND[] = { RGB(255, 0, 0), RGB(0, 255, 0) };
CONST COLORREF g_WINDOW_BACKGROUND[] = { RGB(0, 0, 75), RGB(50, 50, 50) };


CHAR* GetFileName(CHAR* resource);
VOID SetSkin(HWND hwnd, CONST CHAR* skin);
VOID SetFont(HWND hwnd, CONST CHAR* font);
VOID SetSkinFromDLL(HWND hwnd, CONST CHAR* skin);
VOID SetIconFromDLL(HWND hwnd, CONST CHAR* icon_name);
VOID SetFontFromDLL(HWND hwnd, CONST CHAR* font);

VOID GetExeDirectory(CHAR* buffer, DWORD size);

INT ButtonPressedRelease(HWND hwnd, WPARAM wParam);

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	WNDCLASSEX wClass;
	ZeroMemory(&wClass, sizeof(wClass));

	wClass.style = 0;
	wClass.cbSize = sizeof(wClass);
	wClass.cbClsExtra = 0;
	wClass.cbWndExtra = 0;

	wClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wClass.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	//HBITMAP hBackground = (HBITMAP)LoadImage(hInstance, "Picture\\wolf.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	//wClass.hbrBackground = CreatePatternBrush(hBackground);

	wClass.hInstance = hInstance;
	wClass.lpszClassName = g_sz_CLASS_NAME;
	wClass.lpfnWndProc = (WNDPROC)WndProc;
	wClass.lpszMenuName = NULL;

	if (!RegisterClassEx(&wClass))
	{
		MessageBox(NULL, "Class registration failed", NULL, MB_OK | MB_ICONERROR);
		return 0;
	}

	HWND hwnd = CreateWindowEx
	(
		NULL,
		g_sz_CLASS_NAME,
		g_sz_CLASS_NAME,
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,
		g_i_WINDOW_WIDTH, g_i_WINDOW_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static DOUBLE a = DBL_MIN;
	static DOUBLE b = DBL_MIN;
	static INT operation = 0;
	static BOOL input = FALSE;
	static BOOL input_operation = FALSE;

	static INT color_index = 0;
	static HANDLE hMyFont = NULL;

	static BOOL mOptionsChecked = TRUE;
	static INT menu_item_index = 0;

	switch (uMsg)
	{
	case WM_CREATE:
	{
		//AllocConsole();
		freopen("CONOUT$", "w", stdout);

		HWND hEditDisplay = CreateWindowEx
		(
			NULL, "Edit", "0",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
			g_i_START_X, g_i_START_Y,
			g_i_DISPLAY_WIDTH, g_i_DISPLAY_HEIGHT,
			hwnd, (HMENU)IDC_EDIT_DISPLAY,
			GetModuleHandle(NULL), NULL
		);

		CHAR filepath[MAX_PATH]{};
		GetExeDirectory(filepath, MAX_PATH);
		CHAR dllpath[MAX_PATH]{};
		PathCombine(dllpath, filepath, "Fonts\\Digital-7.dll");

		HINSTANCE hInstFont = LoadLibrary(dllpath);
		HRSRC hFontRes = FindResource(hInstFont, MAKEINTRESOURCE(99), "BINARY");
		HGLOBAL hFntMem = LoadResource(hInstFont, hFontRes);
		VOID* fntDara = LockResource(hFntMem);
		DWORD nFonts = 0;
		DWORD len = SizeofResource(hInstFont, hFontRes);
		hMyFont = AddFontMemResourceEx(fntDara, len, nullptr, &nFonts);

		HFONT hFont = CreateFont
		(
			g_i_FONT_HEIGH, g_i_FONT_WIDTH, 0, 0,
			FW_BOLD,
			FALSE,
			FALSE,
			FALSE,
			DEFAULT_CHARSET,
			OUT_TT_PRECIS,
			CLIP_TT_ALWAYS,
			ANTIALIASED_QUALITY,
			FF_DONTCARE,
			"Digital-7"
		);

		FreeLibrary(hInstFont);

		SendMessage(hEditDisplay, WM_SETFONT, (WPARAM)hFont, TRUE);

		CHAR sz_digit[2] = "0";
		for (int i = 6; i >= 0; i -= 3)
		{
			for (int j = 0; j < 3; j++)
			{
				sz_digit[0] = 49 + i + j;
				CreateWindowEx
				(
					NULL, "Button", sz_digit,
					WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
					g_i_BUTTON_START_X + j * (g_i_BUTTON_SIZE + g_i_INTERVAL),
					g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * (2 - i / 3),
					g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
					hwnd, (HMENU)(IDC_BUTTON_1 + i + j),
					GetModuleHandle(NULL), NULL
				);
			}
		}

		CreateWindowEx
		(
			NULL, "Button", "0",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			g_i_BUTTON_START_X,
			g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 3,
			g_i_BUTTON_DOUBLE_SIZE, g_i_BUTTON_SIZE,
			hwnd, (HMENU)IDC_BUTTON_0,
			GetModuleHandle(NULL), NULL
		);

		CreateWindowEx
		(
			NULL, "Button", ".",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			g_i_BUTTON_START_X + g_i_BUTTON_DOUBLE_SIZE + g_i_INTERVAL,
			g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 3,
			g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
			hwnd, (HMENU)IDC_BUTTON_POINT,
			GetModuleHandle(NULL), NULL
		);

		for (int i = 0; i < 4; i++)
		{
			CreateWindowEx
			(
				NULL, "Button", g_OPERATIONS[i],
				WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
				g_i_OPERATION_BUTTON_START_X,
				g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * (3 - i),
				g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
				hwnd, (HMENU)(IDC_BUTTON_PLUS + i),
				GetModuleHandle(NULL), NULL
			);
		}

		CreateWindowEx
		(
			NULL, "Button", "<-",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			g_i_CONTROL_BUTTON_START_X,
			g_i_CONTROL_BUTTON_START_Y,
			g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
			hwnd, (HMENU)IDC_BUTTON_BSP,
			GetModuleHandle(NULL), NULL
		);

		CreateWindowEx
		(
			NULL, "Button", "C",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			g_i_CONTROL_BUTTON_START_X,
			g_i_CONTROL_BUTTON_START_Y + g_i_BUTTON_SIZE + g_i_INTERVAL,
			g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
			hwnd, (HMENU)IDC_BUTTON_CLR,
			GetModuleHandle(NULL), NULL
		);

		CreateWindowEx
		(
			NULL, "Button", "=",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			g_i_CONTROL_BUTTON_START_X,
			g_i_CONTROL_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 2,
			g_i_BUTTON_SIZE, g_i_BUTTON_DOUBLE_SIZE,
			hwnd, (HMENU)IDC_BUTTON_EQUAL,
			GetModuleHandle(NULL), NULL
		);

		SetSkinFromDLL(hwnd, "square_blue");
		//SetFontFromDLL(hwnd, "Digital-7");
		//SetIconFromDLL(hwnd, "icon");
	}
	break;
	/*case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		HBRUSH hBrush = CreateSolidBrush(backgroundColor);
		FillRect(hdc, &ps.rcPaint, hBrush);

		DeleteObject(hBrush);
		EndPaint(hwnd, &ps);
	}
	break;*/
	case WM_CTLCOLOREDIT:
	{
		HDC hdc = (HDC)wParam;
		HWND hEdit = (HWND)lParam;

		if (GetDlgCtrlID(hEdit) == IDC_EDIT_DISPLAY)
		{
			SetTextColor(hdc, g_DISPLAY_FOREGROUND[color_index]);
			SetBkColor(hdc, g_DISPLAY_BACKGROUND[color_index]);

			HBRUSH hbrBackground = CreateSolidBrush(g_WINDOW_BACKGROUND[color_index]);
			SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG)hbrBackground);
			SendMessage(hwnd, WM_ERASEBKGND, wParam, 0);
			return (LRESULT)hbrBackground;
		}
	}
	break;
	case WM_COMMAND:
	{
		CONST INT SIZE = 256;
		CHAR sz_display[SIZE] = {};
		CHAR sz_digit[2] = {};
		HWND hEditDisplay = GetDlgItem(hwnd, IDC_EDIT_DISPLAY);
		SendMessage(hEditDisplay, WM_GETTEXT, SIZE, (LPARAM)sz_display);

		if (LOWORD(wParam) >= IDC_BUTTON_0 && LOWORD(wParam) <= IDC_BUTTON_9)
		{
			if (!input && !input_operation)
			{
				SendMessage(hwnd, WM_COMMAND, LOWORD(IDC_BUTTON_CLR), 0);
				sz_display[0] = 0;
			}
			if (!input && input_operation) sz_display[0] = 0;

			sz_digit[0] = LOWORD(wParam) - IDC_BUTTON_0 + '0';

			if (strlen(sz_display) == 1 && sz_display[0] == '0')
				sz_display[0] = sz_digit[0];
			else
				strcat(sz_display, sz_digit);

			SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)sz_display);

			input = TRUE;
		}

		if (LOWORD(wParam) == IDC_BUTTON_POINT)
		{
			if (input && strchr(sz_display, '.')) break;
			if (input_operation && a == atof(sz_display))
			{
				strcpy(sz_display, "0.");
			}
			else
				strcat(sz_display, ".");

			input = TRUE;
			SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)sz_display);
		}

		if (LOWORD(wParam) == IDC_BUTTON_BSP)
		{
			if (strlen(sz_display) == 1) sz_display[0] = '0';
			else sz_display[strlen(sz_display) - 1] = 0;
			SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)sz_display);
		}

		if (LOWORD(wParam) == IDC_BUTTON_CLR)
		{
			a = b = DBL_MIN;
			operation = 0;
			input = FALSE;
			input_operation = FALSE;
			strcpy(sz_display, "0");

			SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)sz_display);
		}

		if (LOWORD(wParam) >= IDC_BUTTON_PLUS && LOWORD(wParam) <= IDC_BUTTON_SLASH)
		{
			if (a == DBL_MIN) a = atof(sz_display);
			if (input_operation) SendMessage(hwnd, WM_COMMAND, LOWORD(IDC_BUTTON_EQUAL), 0);
			operation = LOWORD(wParam);
			input = FALSE;
			input_operation = TRUE;
		}

		if (LOWORD(wParam) == IDC_BUTTON_EQUAL)
		{
			if (input || b == DBL_MIN && !input) b = atof(sz_display);
			input = FALSE;

			switch (operation)
			{
			case IDC_BUTTON_PLUS: a += b; break;
			case IDC_BUTTON_MINUS: a -= b; break;
			case IDC_BUTTON_ASTER: a *= b; break;
			case IDC_BUTTON_SLASH: a /= b; break;
			}

			input_operation = FALSE;

			if (a == DBL_MIN) strcpy(sz_display, "0");
			else sprintf(sz_display, "%g", a);
			SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)sz_display);
		}

		SetFocus(hwnd);
	}
	break;
	case WM_KEYDOWN:
	{
		SendMessage(GetDlgItem(hwnd, ButtonPressedRelease(hwnd, wParam)), BM_SETSTATE, TRUE, 0);
	}
	break;
	case WM_KEYUP:
	{
		SendMessage(GetDlgItem(hwnd, ButtonPressedRelease(hwnd, wParam)), BM_SETSTATE, FALSE, 0);
		SendMessage(hwnd, WM_COMMAND, ButtonPressedRelease(hwnd, wParam), 0);
	}
	break;
	case WM_CONTEXTMENU:
	{
		HMENU hMenu = CreatePopupMenu();
		HMENU hSubmenuSkins = CreatePopupMenu();

		InsertMenu(hSubmenuSkins, 0, MF_BYPOSITION | MF_STRING, IDR_SQUARE_BLUE, "Square Blue");
		InsertMenu(hSubmenuSkins, 1, MF_BYPOSITION | MF_STRING, IDR_METAL_MISTRAL, "Metal Mistral");

		InsertMenu(hMenu, 0, MF_BYPOSITION | MF_POPUP, (UINT_PTR)hSubmenuSkins, "Skins");
		InsertMenu(hMenu, 1, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
		InsertMenu(hMenu, 2, MF_BYPOSITION | MF_STRING, IDR_EXIT, "Exit");

		if (hMenu)
		{
			if (hSubmenuSkins)
			{
				SetForegroundWindow(hwnd);

				MENUITEMINFO mi = { 0 };
				mi.cbSize = sizeof(MENUITEMINFO);
				mi.fMask = MIIM_STATE;
				mi.fState = MFS_CHECKED;
				SetMenuItemInfo(hSubmenuSkins, menu_item_index, MFS_CHECKED, &mi);
			}
		}

		BOOL skin_index = TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_RETURNCMD, LOWORD(lParam), HIWORD(lParam), 0, hwnd, NULL);
		switch (skin_index)
		{
		case IDR_SQUARE_BLUE:
		{
			menu_item_index = 0;
			SetSkinFromDLL(hwnd, "square_blue");
		}
		break;
		case IDR_METAL_MISTRAL:
		{
			menu_item_index = 1;
			SetSkinFromDLL(hwnd, "metal_mistral");
		}
		break;
		case IDR_EXIT: DestroyWindow(hwnd);
		}

		DestroyMenu(hSubmenuSkins);
		DestroyMenu(hMenu);

		if (skin_index >= IDR_SQUARE_BLUE && skin_index <= IDR_METAL_MISTRAL)
		{
			color_index = skin_index - IDR_CONTEXT_MENU - 1;
			HWND hEditDisplay = GetDlgItem(hwnd, IDC_EDIT_DISPLAY);
			HDC hdcDisplay = GetDC(hEditDisplay);
			SendMessage(hwnd, WM_CTLCOLOREDIT, (WPARAM)hdcDisplay, (LPARAM)hEditDisplay);
			ReleaseDC(hEditDisplay, hdcDisplay);
		}
		HWND hEditDisplay = GetDlgItem(hwnd, IDC_EDIT_DISPLAY);
		CONST INT SIZE = 256;
		CHAR sz_display[SIZE]{};
		SendMessage(hEditDisplay, WM_GETTEXT, SIZE, (LPARAM)sz_display);
		SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)sz_display);
	}
	break;
	case WM_DESTROY:
	{
		RemoveFontMemResourceEx(hMyFont);
		HWND hEdit = GetDlgItem(hwnd, IDC_EDIT_DISPLAY);
		HDC hdc = GetDC(hEdit);
		ReleaseDC(hEdit, hdc);
		PostQuitMessage(0);
	}
	break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

LPCSTR GetFileName(_In_ LPCSTR resource)
{
	return resource + strspn(resource, "_") + 1;
}

CONST CHAR* g_BUTTON_FILENAME[] =
{
"button_0",
"button_1",
"button_2",
"button_3",
"button_4",
"button_5",
"button_6",
"button_7",
"button_8",
"button_9",
"button_point",
"button_plus"	,
"button_minus",
"button_aster",
"button_slash",
"button_bsp",
"button_clr",
"button_equal",
};

VOID SetSkin(HWND hwnd, CONST CHAR* skin)
{
	CHAR sz_path[MAX_PATH]{};
	CHAR sz_filename[FILENAME_MAX]{};
	CHAR sz_full_name[MAX_PATH]{};

	system("cls");

	for (int i = 0; i < 18; i++)
	{
		HWND hButton = GetDlgItem(hwnd, IDC_BUTTON_0 + i);
		sprintf(sz_filename, "ButtonsBMP\\%s\\%s.bmp", skin, g_BUTTON_FILENAME[i]);
		std::cout << sz_filename << std::endl;
		HBITMAP bmpButton = (HBITMAP)LoadImage
		(
			GetModuleHandle(NULL),
			sz_filename,
			IMAGE_BITMAP,
			i + IDC_BUTTON_0 == IDC_BUTTON_0 ? g_i_BUTTON_DOUBLE_SIZE : g_i_BUTTON_SIZE,
			i + IDC_BUTTON_0 == IDC_BUTTON_EQUAL ? g_i_BUTTON_DOUBLE_SIZE : g_i_BUTTON_SIZE,
			LR_LOADFROMFILE
		);
		SendMessage(hButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmpButton);
	}
}

VOID SetFont(HWND hwnd, CONST CHAR* font)
{
	CHAR sz_fontname[FILENAME_MAX]{};
	HWND hEditDisplay = GetDlgItem(hwnd, IDC_EDIT_DISPLAY);
	sprintf(sz_fontname, "Font\\%s.ttf", font);
	AddFontResourceEx(sz_fontname, FR_PRIVATE, 0);
	HFONT hFont = CreateFont
	(
		g_i_FONT_HEIGH, g_i_FONT_WIDTH, 0, 0,
		FW_BOLD,
		FALSE,
		FALSE,
		FALSE,
		DEFAULT_CHARSET,
		OUT_TT_PRECIS,
		CLIP_TT_ALWAYS,
		ANTIALIASED_QUALITY,
		FF_DONTCARE,
		font
	);
	SendMessage(hEditDisplay, WM_SETFONT, (WPARAM)hFont, TRUE);
}

VOID SetFontFromDLL(HWND hwnd, CONST CHAR* font)
{
	CHAR sz_fontname[FILENAME_MAX]{};
	sprintf(sz_fontname, "D:\\surce\\repos\\WinAPI\\Calc\\Font\\%s.dll", font);

	HINSTANCE hInstFont = LoadLibrary(sz_fontname);
	HRSRC hFontRes = FindResource(hInstFont, MAKEINTRESOURCE(99), "BINARY");
	HGLOBAL hFntMem = LoadResource(hInstFont, hFontRes);
	VOID* fntDara = LockResource(hFntMem);
	DWORD nFonts = 0;
	DWORD len = SizeofResource(hInstFont, hFontRes);
	HANDLE hMyFont = AddFontMemResourceEx(fntDara, len, nullptr, &nFonts);

	HWND hEditDisplay = GetDlgItem(hwnd, IDC_EDIT_DISPLAY);
	AddFontResourceEx(sz_fontname, FR_PRIVATE, 0);
	HFONT hFont = CreateFont
	(
		g_i_FONT_HEIGH, g_i_FONT_WIDTH, 0, 0,
		FW_BOLD,
		FALSE,
		FALSE,
		FALSE,
		DEFAULT_CHARSET,
		OUT_TT_PRECIS,
		CLIP_TT_ALWAYS,
		ANTIALIASED_QUALITY,
		FF_DONTCARE,
		font
	);

	FreeLibrary(hInstFont);

	SendMessage(hEditDisplay, WM_SETFONT, (WPARAM)hFont, TRUE);

}

VOID SetSkinFromDLL(HWND hwnd, CONST CHAR* skin)
{
	CHAR filepath[MAX_PATH]{};
	CHAR filename[MAX_PATH]{};
	GetExeDirectory(filepath, MAX_PATH);
	sprintf(filename, "%s\\ButtonsBMP\\%s.dll", filepath, skin);
	HMODULE hInst = LoadLibrary(filename);

	for (int i = IDC_BUTTON_0; i <= IDC_BUTTON_EQUAL; i++)
	{
		HBITMAP buttonBMP = (HBITMAP)LoadImage
		(
			hInst,
			MAKEINTRESOURCE(i),
			IMAGE_BITMAP,
			i > IDC_BUTTON_0 ? g_i_BUTTON_SIZE : g_i_BUTTON_DOUBLE_SIZE,
			i < IDC_BUTTON_EQUAL ? g_i_BUTTON_SIZE : g_i_BUTTON_DOUBLE_SIZE,
			NULL
		);
		SendMessage(GetDlgItem(hwnd, i), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)buttonBMP);
	}

	FreeLibrary(hInst);
}

VOID SetIconFromDLL(HWND hwnd, CONST CHAR* icon_name)
{
	CHAR filename[MAX_PATH]{};
	sprintf(filename, "D:\\surce\\repos\\WinAPI\\Calc\\ICO\\%s.dll", icon_name);
	HMODULE hInst = LoadLibrary(filename);

	HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));

	SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	//SetClassLongPtr(hwnd, GCLP_HICON, (LONG)hIcon);

	FreeLibrary(hInst);
}

INT ButtonPressedRelease(HWND hwnd, WPARAM wParam)
{
	if (GetKeyState(VK_SHIFT) < 0)
	{
		if (wParam == 0x38)
		{
			return IDC_BUTTON_ASTER;
		}
	}

	else if (wParam >= 0x30 && wParam <= 0x39)
	{
		return wParam - 0x30 + IDC_BUTTON_0;
	}

	else if (wParam >= 0x60 && wParam <= 0x69)
	{
		return wParam - 0x60 + IDC_BUTTON_0;
	}

	switch (wParam)
	{
	case VK_DECIMAL:
	case VK_OEM_PERIOD:
		return LOWORD(IDC_BUTTON_POINT);
		break;
	case VK_BACK:
		return LOWORD(IDC_BUTTON_BSP);
		break;
	case VK_ESCAPE:
		return LOWORD(IDC_BUTTON_CLR);
		break;
	case VK_RETURN:
		return LOWORD(IDC_BUTTON_EQUAL);
		break;

	case VK_ADD:
	case VK_OEM_PLUS:
		return LOWORD(IDC_BUTTON_PLUS);
		break;
	case VK_SUBTRACT:
	case VK_OEM_MINUS:
		return LOWORD(IDC_BUTTON_MINUS);
		break;
	case VK_MULTIPLY:
		return LOWORD(IDC_BUTTON_ASTER);
		break;
	case VK_DIVIDE:
	case VK_OEM_2:
		return LOWORD(IDC_BUTTON_SLASH);
		break;
	}
}

VOID GetExeDirectory(CHAR* buffer, DWORD size)
{
	GetModuleFileName(NULL, buffer, size);
	PathRemoveFileSpec(buffer);
}