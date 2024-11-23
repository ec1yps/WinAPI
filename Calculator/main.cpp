#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include "resource.h"

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lCmdLine, INT nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DLGPROC(DlgProc), 0);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
		SendMessage(hwnd, WM_SETICON, 0, (LPARAM)hIcon);
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_ZERO:
		{
			HWND hEditRes = GetDlgItem(hwnd, IDC_EDIT_RESULT);
			CONST INT SIZE = 256;
			CHAR sz_buffer[SIZE] = {};
			SendMessage(hEditRes, WM_GETTEXT, SIZE, (LPARAM)sz_buffer);
			strcpy(sz_buffer, "0");
			SendMessage(hEditRes, WM_SETTEXT, 0, (LPARAM)sz_buffer);
		}
		break;
		case IDC_BUTTON_ONE:
			SendMessage(GetDlgItem(hwnd, IDC_EDIT_RESULT), WM_SETTEXT, 0, (LPARAM)"1");
			break;
		case IDC_BUTTON_TWO:
			SendMessage(GetDlgItem(hwnd, IDC_EDIT_RESULT), WM_SETTEXT, 0, (LPARAM)"2");
			break;
		case IDC_BUTTON_THREE:
			break;
		case IDC_BUTTON_FOUR:
			break;
		case IDC_BUTTON_FIVE:
			break;
		case IDC_BUTTON_SIX:
			break;
		case IDC_BUTTON_SEVEN:
			break;
		case IDC_BUTTON_EIGHT:
			break;
		case IDC_BUTTON_NINE:
			break;
		case IDC_BUTTON_POS_NEG:
			break;
		case IDC_BUTTON_DECIMAL_POINT:
			break;
		case IDC_BUTTON_ECUALLY:
			break;
		case IDC_BUTTON_MULTIPLICATION:
			break;
		case IDC_BUTTON_DIVISION:
			break;
		case IDC_BUTTON_PLUS:
			break;
		case IDC_BUTTON_MINUS:
			break;
		case IDC_BUTTON_ONE_DIVIDED_INTO:
			break;
		case IDC_BUTTON_POWER:
			break;
		case IDC_BUTTON_ROOT:
			break;
		case IDC_BUTTON_CLR:
			break;
		case IDC_BUTTON_DELETE:
			break;
		}
		break;
	case WM_CLOSE: EndDialog(hwnd, 0); break;
	}
	return FALSE;
}