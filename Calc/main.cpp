#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include "resource.h"

CONST CHAR g_sz_CLASS_NAME[] = "Calc PV_319";

CONST INT g_i_INTERVAL = 5;
CONST INT g_i_BUTTON_SIZE = 50;
CONST INT g_i_BUTTON_DOUBLE_SIZE = g_i_BUTTON_SIZE * 2 + g_i_INTERVAL;

CONST INT g_i_DISPLAY_WIDTH = g_i_BUTTON_SIZE * 5 + g_i_INTERVAL * 4;
CONST INT g_i_DISPLAY_HEIGHT = 40;

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

CONST CHAR* g_BUTTONS_IMAGES[] =
{
	"ButtonsBMP\\square_blue\\button_1.bmp",
	"ButtonsBMP\\square_blue\\button_2.bmp",
	"ButtonsBMP\\square_blue\\button_3.bmp",
	"ButtonsBMP\\square_blue\\button_4.bmp",
	"ButtonsBMP\\square_blue\\button_5.bmp",
	"ButtonsBMP\\square_blue\\button_6.bmp",
	"ButtonsBMP\\square_blue\\button_7.bmp",
	"ButtonsBMP\\square_blue\\button_8.bmp",
	"ButtonsBMP\\square_blue\\button_9.bmp",
	"ButtonsBMP\\square_blue\\button_plus.bmp",
	"ButtonsBMP\\square_blue\\button_minus.bmp",
	"ButtonsBMP\\square_blue\\button_aster.bmp",
	"ButtonsBMP\\square_blue\\button_slash.bmp",
	"ButtonsBMP\\square_blue\\button_point.bmp",
	"ButtonsBMP\\square_blue\\button_bsp.bmp",
	"ButtonsBMP\\square_blue\\button_clr.bmp",
};

VOID load_button_image(HWND hButton, INT index);

CHAR* GetFileName(CHAR* resource);
VOID SetSkin(HWND hwnd, CONST CHAR* skin, CONST CHAR* font);

INT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	WNDCLASSEX wClass;
	ZeroMemory(&wClass, sizeof(wClass));

	wClass.style = 0;
	wClass.cbSize = sizeof(wClass);
	wClass.cbClsExtra = 0;
	wClass.cbWndExtra = 0;

	wClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wClass.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wClass.hbrBackground = (HBRUSH)COLOR_WINDOW;

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

INT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static DOUBLE a = DBL_MIN;
	static DOUBLE b = DBL_MIN;
	static INT operation = 0;
	static BOOL input = FALSE;
	static BOOL input_operation = FALSE;

	switch (uMsg)
	{
	case WM_CREATE:
	{
		AllocConsole();
		freopen("CONOUT$", "w", stdout);

		HWND hEdit = CreateWindowEx
		(
			NULL, "Edit", "0",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
			g_i_START_X, g_i_START_Y,
			g_i_DISPLAY_WIDTH, g_i_DISPLAY_HEIGHT,
			hwnd, (HMENU)IDC_EDIT_DISPLAY,
			GetModuleHandle(NULL), NULL
		);
		AddFontResource("Font\\GROBOLDoutline.ttf");
		HFONT hFont = CreateFont
		(
			g_i_DISPLAY_HEIGHT - 4, 8, 0, 0,
			FW_NORMAL,
			FALSE,
			FALSE,
			FALSE,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH,
			"GROBOLDoutline"
		);
		SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

		CHAR sz_digit[2] = "0";
		for (int i = 6; i >= 0; i -= 3)
		{
			for (int j = 0; j < 3; j++)
			{
				sz_digit[0] = 49 + i + j;
				HWND hButtonNumbers = CreateWindowEx
				(
					NULL, "Button", sz_digit,
					WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
					g_i_BUTTON_START_X + j * (g_i_BUTTON_SIZE + g_i_INTERVAL),
					g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * (2 - i / 3),
					g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
					hwnd, (HMENU)(IDC_BUTTON_1 + i + j),
					GetModuleHandle(NULL), NULL
				);
				INT index = i + j;
				//load_button_image(hButtonNumbers, index);
			}
		}

		HWND hButton0 = CreateWindowEx
		(
			NULL, "Button", "0",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			g_i_BUTTON_START_X,
			g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 3,
			g_i_BUTTON_DOUBLE_SIZE, g_i_BUTTON_SIZE,
			hwnd, (HMENU)IDC_BUTTON_0,
			GetModuleHandle(NULL), NULL
		);
		HBITMAP hBitmap0 = (HBITMAP)LoadImage
		(
			GetModuleHandle(NULL),
			"ButtonsBMP\\square_blue\\button_0.bmp",
			IMAGE_BITMAP,
			g_i_BUTTON_DOUBLE_SIZE, g_i_BUTTON_SIZE,
			LR_LOADFROMFILE
		);
		//SendMessage(hButton0, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap0);

		HWND hButtonPoint = CreateWindowEx
		(
			NULL, "Button", ".",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			g_i_BUTTON_START_X + g_i_BUTTON_DOUBLE_SIZE + g_i_INTERVAL,
			g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 3,
			g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
			hwnd, (HMENU)IDC_BUTTON_POINT,
			GetModuleHandle(NULL), NULL
		);
		//load_button_image(hButtonPoint, 13);

		for (int i = 0; i < 4; i++)
		{
			HWND hButtonOperation = CreateWindowEx
			(
				NULL, "Button", g_OPERATIONS[i],
				WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
				g_i_OPERATION_BUTTON_START_X,
				g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * (3 - i),
				g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
				hwnd, (HMENU)(IDC_BUTTON_PLUS + i),
				GetModuleHandle(NULL), NULL
			);
			INT index = i + 9;
			//load_button_image(hButtonOperation, index);
		}

		HWND hButtonBsp = CreateWindowEx
		(
			NULL, "Button", "<-",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			g_i_CONTROL_BUTTON_START_X,
			g_i_CONTROL_BUTTON_START_Y,
			g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
			hwnd, (HMENU)IDC_BUTTON_BSP,
			GetModuleHandle(NULL), NULL
		);
		//load_button_image(hButtonBsp, 14);

		HWND hButtonClr = CreateWindowEx
		(
			NULL, "Button", "C",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			g_i_CONTROL_BUTTON_START_X,
			g_i_CONTROL_BUTTON_START_Y + g_i_BUTTON_SIZE + g_i_INTERVAL,
			g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
			hwnd, (HMENU)IDC_BUTTON_CLR,
			GetModuleHandle(NULL), NULL
		);
		//load_button_image(hButtonClr, 15);

		HWND hButtonEqual = CreateWindowEx
		(
			NULL, "Button", "=",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
			g_i_CONTROL_BUTTON_START_X,
			g_i_CONTROL_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 2,
			g_i_BUTTON_SIZE, g_i_BUTTON_DOUBLE_SIZE,
			hwnd, (HMENU)IDC_BUTTON_EQUAL,
			GetModuleHandle(NULL), NULL
		);
		HBITMAP hBitmapEqual = (HBITMAP)LoadImage
		(
			GetModuleHandle(NULL),
			"ButtonsBMP\\square_blue\\button_equal.bmp",
			IMAGE_BITMAP,
			g_i_BUTTON_SIZE, g_i_BUTTON_DOUBLE_SIZE,
			LR_LOADFROMFILE
		);
		//SendMessage(hButtonEqual, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmapEqual);

		SetSkin(hwnd, "square_blue", "GROBOLDoutline");
	}
	break;
	case WM_CONTEXTMENU:
	{
		HMENU hMenu = CreatePopupMenu();
		POINT cursor;
		GetCursorPos(&cursor);
		AppendMenu(hMenu, MFT_STRING, ID_SQUARE_BLUE_STYLE, "square_blue style");
		AppendMenu(hMenu, MFT_STRING, ID_METAL_MISTRAL_STYLE, "metal_mistral style");
		TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, cursor.x, cursor.y, 0, hwnd, NULL);
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
			//input_operation = FALSE;
		}

		if (LOWORD(wParam) == IDC_BUTTON_POINT)
		{
			if (strchr(sz_display, '.')) break;
			strcat(sz_display, ".");
			SendMessage(hEditDisplay, WM_SETTEXT, 0, (LPARAM)sz_display);
		}

		//if (LOWORD(wParam) == IDC_EDIT_DISPLAY && HIWORD(wParam) == EN_SETFOCUS) SetFocus(hwnd);

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
			//else b = atof(sz_display);
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

		if (LOWORD(wParam) == ID_SQUARE_BLUE_STYLE)
		{
			/*AddFontResource("Font\\GROBOLDoutline.ttf");
			HFONT hFont = CreateFont
			(
				g_i_DISPLAY_HEIGHT - 4, 8, 0, 0,
				FW_NORMAL,
				FALSE,
				FALSE,
				FALSE,
				DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				DEFAULT_PITCH,
				"GROBOLDoutline"
			);
			SendMessage(hEditDisplay, WM_SETFONT, (WPARAM)hFont, TRUE);*/
			SetSkin(hwnd, "square_blue", "GROBOLDoutline");
		}
		
		if (LOWORD(wParam) == ID_METAL_MISTRAL_STYLE)
		{
			/*AddFontResource("Font\\HandWriting.otf");
			HFONT hFont = CreateFont
			(
				g_i_DISPLAY_HEIGHT - 8, 10, 0, 0,
				FW_NORMAL,
				FALSE,
				FALSE,
				FALSE,
				DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				DEFAULT_PITCH,
				"HandWriting"
			);
			SendMessage(hEditDisplay, WM_SETFONT, (WPARAM)hFont, TRUE);*/
			SetSkin(hwnd, "metal_mistral", "HandWriting");
		}

		SetFocus(hwnd);
	}
	break;
	case WM_KEYDOWN:
	{
		if (GetKeyState(VK_SHIFT) < 0)
		{
			if (wParam == 0x38) SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_ASTER, 0);
		}

		else if (wParam >= 0x30 && wParam <= 0x39)
			SendMessage(hwnd, WM_COMMAND, wParam - 0x30 + IDC_BUTTON_0, 0);

		else if (wParam >= VK_NUMPAD0 && wParam <= VK_NUMPAD9)
			SendMessage(hwnd, WM_COMMAND, wParam - VK_NUMPAD0 + IDC_BUTTON_0, 0);

		switch (wParam)
		{
		case VK_DECIMAL:
		case VK_OEM_PERIOD:
			SendMessage(hwnd, WM_COMMAND, LOWORD(IDC_BUTTON_POINT), 0);
			break;
		case VK_BACK:
			SendMessage(hwnd, WM_COMMAND, LOWORD(IDC_BUTTON_BSP), 0);
			break;
		case VK_ESCAPE:
			SendMessage(hwnd, WM_COMMAND, LOWORD(IDC_BUTTON_CLR), 0);
			break;
		case VK_RETURN:
			SendMessage(hwnd, WM_COMMAND, LOWORD(IDC_BUTTON_EQUAL), 0);
			break;

		case VK_ADD:
		case VK_OEM_PLUS:
			SendMessage(hwnd, WM_COMMAND, LOWORD(IDC_BUTTON_PLUS), 0);
			break;
		case VK_SUBTRACT:
		case VK_OEM_MINUS:
			SendMessage(hwnd, WM_COMMAND, LOWORD(IDC_BUTTON_MINUS), 0);
			break;
		case VK_MULTIPLY:
			SendMessage(hwnd, WM_COMMAND, LOWORD(IDC_BUTTON_ASTER), 0);
			break;
		case VK_DIVIDE:
		case VK_OEM_2:
			SendMessage(hwnd, WM_COMMAND, LOWORD(IDC_BUTTON_SLASH), 0);
			break;
		}
	}
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
}

VOID load_button_image(HWND hButton, INT index)
{
	HBITMAP hBitmap = (HBITMAP)LoadImage
	(
		GetModuleHandle(NULL),
		g_BUTTONS_IMAGES[index],
		IMAGE_BITMAP,
		g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
		LR_LOADFROMFILE
	);
	SendMessage(hButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
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

VOID SetSkin(HWND hwnd, CONST CHAR* skin, CONST CHAR* font)
{
	CHAR sz_path[MAX_PATH]{};
	CHAR sz_filename[FILENAME_MAX]{};
	CHAR sz_full_name[MAX_PATH]{};

	for (int i = 0; i < 18; i++)
	{
		HWND hButton = GetDlgItem(hwnd, IDC_BUTTON_0 + i);
		sprintf(sz_filename, "ButtonsBMP\\%s\\%s.bmp", skin, g_BUTTON_FILENAME[i]);
		std::cout << sz_filename[i] << std::endl;
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

	HWND hEditDisplay = GetDlgItem(hwnd, IDC_EDIT_DISPLAY);
	sprintf(sz_filename, "Font\\%s.ttf", font);
	AddFontResource(sz_filename);
	HFONT hFont = CreateFont
	(
		g_i_DISPLAY_HEIGHT - 4, 10, 0, 0,
		FW_NORMAL,
		FALSE,
		FALSE,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		font
	);
	SendMessage(hEditDisplay, WM_SETFONT, (WPARAM)hFont, TRUE);
}