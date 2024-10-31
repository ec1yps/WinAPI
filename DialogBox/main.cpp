#include <Windows.h>
#include "resource.h"

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc, 0);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL isHintVisible = TRUE;
	CONST INT E_SIZE = 30;
	CHAR edit_buffer[E_SIZE] = {};

	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
		SendMessage(hwnd, WM_SETICON, 0, (LPARAM)hIcon);
		//SetDlgItemText(hwnd, IDC_EDIT_LOGIN, "Введите имя пользователя");
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_EDIT_LOGIN:
		{
			if (HIWORD(wParam) == EN_SETFOCUS) 
			{
				if (isHintVisible) 
				{
					SetWindowText((HWND)IDC_EDIT_LOGIN, "");
					isHintVisible = false;
				}
			}
			else if (HIWORD(wParam) == EN_KILLFOCUS) 
			{
				if (GetWindowTextLength((HWND)IDC_EDIT_LOGIN) == 0) 
				{
					SetWindowText((HWND)IDC_EDIT_LOGIN, "Введите имя пользователя");
					GetWindowText((HWND)IDC_EDIT_LOGIN, edit_buffer, E_SIZE);
					isHintVisible = true;
				}
			}
		}
		break;
		case IDC_BUTTON_COPY:
		{
			HWND hEditLogin = GetDlgItem(hwnd, IDC_EDIT_LOGIN);
			HWND hEditPassword = GetDlgItem(hwnd, IDC_EDIT_PASSWORD);
			CONST INT SIZE = 256;
			CHAR sz_buffer[SIZE] = {};
			SendMessage(hEditLogin, WM_GETTEXT, SIZE, (LPARAM)sz_buffer);
			SendMessage(hEditPassword, WM_SETTEXT, 0, (LPARAM)sz_buffer);
		}
		break;
		case IDOK: MessageBox(hwnd, "Была нажата кнопка ОК!", "Info", MB_OK | MB_ICONINFORMATION); break;
		case IDCANCEL: EndDialog(hwnd, 0); break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		break;
	}
	return FALSE;
}