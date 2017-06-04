// 2048(WinAPI).cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "2048(WinAPI).h"
#include <wingdi.h>

#define MAX_LOADSTRING 100

// Global Variables:
bool started;
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
GameAI current;
void save();
void load();

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MY2048WINAPI, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY2048WINAPI));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY2048WINAPI));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MY2048WINAPI);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		350,20, 800, 800, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HFONT numF = CreateFont(76, 38, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "Clear Sans");
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case ID_MENU_NEWGAME:
			current.generateNew();
			started = true;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case ID_MENU_SAVE:
			save();
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case ID_MENU_LOADLASTSAVE:
			load();
			started = true;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_KEYDOWN:
	{
		if (started)
		{
			switch (wParam)
			{
			case VK_LEFT:
				current.left();
				break;
			case VK_UP:
				current.up();
				break;
			case VK_RIGHT:
				current.right();
				break;
			case VK_DOWN:
				current.down();
				break;
			}
			InvalidateRect(hWnd, NULL, TRUE);
			if (current.is_full())
			{
				MessageBox(hWnd, "You lose!", "Game over", MB_OK);
				started = false;
			}
			else if (current.win())
			{
				started = false;
				MessageBox(hWnd, "You win!", "Congrats!", MB_OK);

			}

			else if(!current.is_pseudo_full())
				current.random_spawn();
		}
		break;
	}

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		RECT rc, tile;
		GetClientRect(hWnd, &rc);
		int _x = rc.right / 4, _y = rc.bottom / 4;
		char buff[5];
		HDC hdc, hdcMem;
		HBITMAP hbmMem;
		HGDIOBJ hbmOld;
		hdc = BeginPaint(hWnd, &ps);
		hdcMem = CreateCompatibleDC(hdc);
		hbmMem = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
		hbmOld = SelectObject(hdcMem, hbmMem);
		FillRect(hdcMem, &rc, (HBRUSH)(COLOR_WINDOW + 1));
		SetBkMode(hdcMem, TRANSPARENT);
		SelectObject(hdcMem, numF);
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
			{
				int val = current.get_tile_value(i, j);
				SetRect(&tile, i*_x, j*_y, (i + 1)*_x, (j + 1)*_y);
				SetTextColor(hdcMem, RGB(249, 246, 242));
				switch (val)
				{
				case 0:
					SelectObject(hdcMem, CreateSolidBrush(RGB(205,193,180)));
					break;
				case 2:
					SelectObject(hdcMem, CreateSolidBrush(RGB(238, 228, 218)));
					SetTextColor(hdcMem, RGB(119, 110, 101));
					break;
				case 4:
					SelectObject(hdcMem, CreateSolidBrush(RGB(237, 224, 200)));
					SetTextColor(hdcMem, RGB(119, 110, 101));
					break;
				case 8:
					SelectObject(hdcMem, CreateSolidBrush(RGB(242, 177, 121)));
					break;
				case 16:
					SelectObject(hdcMem, CreateSolidBrush(RGB(245, 149, 99)));
					break;
				case 32:
					SelectObject(hdcMem, CreateSolidBrush(RGB(246, 124, 95)));
					break;
				case 64:
					SelectObject(hdcMem, CreateSolidBrush(RGB(246, 94, 59)));
					break;
				case 128:
					SelectObject(hdcMem, CreateSolidBrush(RGB(237, 207, 114)));
					break;
				case 256:
					SelectObject(hdcMem, CreateSolidBrush(RGB(237, 204, 97)));
					break;
				case 512:
					SelectObject(hdcMem, CreateSolidBrush(RGB(237, 200, 80)));
					break;
				case 1024:
					SelectObject(hdcMem, CreateSolidBrush(RGB(237, 197, 63)));
					break;
				case 2048:
					SelectObject(hdcMem, CreateSolidBrush(RGB(237, 194, 46)));
					break;
				default:
					SelectObject(hdcMem, CreateSolidBrush(RGB(60, 58, 50)));
					break;
				}
				SelectObject(hdcMem, CreatePen(1,22,RGB(187,173,160)));
				Rectangle(hdcMem, i*_x, j*_y, (i + 1)*_x, (j + 1)*_y);
				_itoa(val, buff, 10);
				if(val)
					DrawText(hdcMem, buff, -1, &tile, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			}
		BitBlt(hdc,
			rc.left, rc.top,
			rc.right, rc.bottom,
			hdcMem,
			rc.left, rc.top,
			SRCCOPY);
		SelectObject(hdcMem, hbmOld);
		DeleteObject(hbmMem);
		DeleteDC(hdcMem);
		EndPaint(hWnd, &ps);
	}
	break;

	case WM_ERASEBKGND:
		return (LRESULT)1;

	case WM_GETMINMAXINFO:
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x = 800;
		lpMMI->ptMinTrackSize.y = 800;
		lpMMI->ptMaxTrackSize.x = 800;
		lpMMI->ptMaxTrackSize.y = 800;
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void save()
{
	ofstream s;
	s.open("save.txt");
	current.save(s);
	s.close();
}

void load()
{
	ifstream s;
	s.open("save.txt");
	current.load(s);
}