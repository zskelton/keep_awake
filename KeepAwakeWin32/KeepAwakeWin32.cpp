// KeepAwakeWin32.cpp : Defines the entry point for the application.
//
#include "framework.h"
#include "KeepAwakeWin32.h"
#include <CommCtrl.h>
#include <shellapi.h>

// Set Visual Styles
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Defines
#define MAX_LOADSTRING 100
#define TRAY_ICONUID 100
#define WM_TRAYMESSAGE (WM_USER + 1)

// Global Variables:
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

// Controls and States
HWND hwndProgress;
HWND hwndButton;
HWND hwndStatus;
bool keepAwake;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// Main Entry Point
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_KEEPAWAKEWIN32, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_KEEPAWAKEWIN32));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

// Register Class
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_KEEPAWAKEWIN32));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_KEEPAWAKEWIN32);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

// Start Instance
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 400, 200, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

// Draw Tray Icon
void TrayDrawIcon(HWND hWnd) {
	HINSTANCE g_hInst = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = TRAY_ICONUID;
	nid.uVersion = NOTIFYICON_VERSION;
	nid.uCallbackMessage = WM_TRAYMESSAGE;
	nid.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_KEEPAWAKEWIN32));
	LoadString(g_hInst, IDS_APP_TITLE, nid.szTip, 128);
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	Shell_NotifyIcon(NIM_ADD, &nid);
}

// Tray Icon Delete
void TrayDeleteIcon(HWND hWnd) {
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = TRAY_ICONUID;
	Shell_NotifyIcon(NIM_DELETE, &nid);
}

// Set Keep Awake Mode
void SetKeepAwake(bool direction)
{
	// Set State to New Mode
	keepAwake = direction;
	// Set Computer Accordingly
	if (keepAwake) {
		// Turn On
		SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED);
		SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)L"Keep Awake Set.");
		SendMessage(hwndProgress, PBM_SETPOS, (WPARAM)100, 0);
	}
	else {
		// Turn Off
		SetThreadExecutionState(ES_CONTINUOUS);
		SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)L"Click Button to turn on 'Keep Awake' function.");
		SendMessage(hwndProgress, PBM_SETPOS, (WPARAM)0, 0);
	}
}

// Callback Loop
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HINSTANCE g_hInst = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
    switch (message)
    {
	case WM_CREATE:
	{
		keepAwake = false;
		hwndProgress = CreateWindowEx(0, PROGRESS_CLASS, 0, WS_CHILD | WS_VISIBLE, 10, 10, 360, 40, hWnd, 0, g_hInst, 0);
		hwndButton = CreateWindowW(L"BUTTON", L"Set Status", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 140, 70, 100, 25, hWnd, 0, g_hInst, 0);
		hwndStatus = CreateWindowEx(0, STATUSCLASSNAME, 0, SBARS_SIZEGRIP | WS_CHILD | WS_VISIBLE, 160, 100, 100, 100, hWnd, 0, g_hInst, 0);
		SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)L"Click Button to turn on 'Keep Awake' function.");
		// Turn On Automatically
		SetKeepAwake(true);
	}
	break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
				// Turn Off As Default
				SetThreadExecutionState(ES_CONTINUOUS);
                DestroyWindow(hWnd);
                break;
			case BN_CLICKED:
				// Turn On/Off Based on Previous Setting
				SetKeepAwake(!keepAwake);
				break;
			case ID_CONTEXT_RESTORE:
				ShowWindow(hWnd, SW_SHOWNORMAL);
				SetForegroundWindow(hWnd);
				TrayDeleteIcon(hWnd);
				break;
			case ID_CONTEXT_EXIT:
				SetForegroundWindow(hWnd);
				SendMessage(hWnd, WM_CLOSE, 0, 0);
				break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_TRAYMESSAGE:
		switch (lParam) {
		case WM_LBUTTONDBLCLK:
			ShowWindow(hWnd, SW_SHOWNORMAL);
			SetForegroundWindow(hWnd);
			TrayDeleteIcon(hWnd);
			break;
		case WM_RBUTTONUP:			
			POINT cursor;
			GetCursorPos(&cursor);
			SetForegroundWindow(hWnd);
			TrackPopupMenu((HMENU)GetSubMenu(LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_MENU_POPUP)), 0), TPM_BOTTOMALIGN | TPM_LEFTALIGN, cursor.x, cursor.y, 0, hWnd, NULL);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_SIZE:
		switch (wParam) {
			case SIZE_MINIMIZED:
				TrayDrawIcon(hWnd);
				ShowWindow(hWnd, SW_HIDE);
				break;
		}
		break;
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
