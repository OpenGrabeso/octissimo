// Octissimo.cpp : Defines the entry point for the application.
//
// based on https://www.codeproject.com/Articles/4768/Basic-use-of-Shell-NotifyIcon-in-Win32

#include "win.h"
#include "resource.h"
#include <functional>
#include <json.hpp>
#include "GithubStatus.h"

using json = nlohmann::json;

#define MAX_LOADSTRING 100
#define   WM_USER_SHELLICON (WM_USER + 1)
#define   IDT_TIMER (WM_USER + 2)

HINSTANCE hInst;   // current instance
HWND hWnd;
NOTIFYICONDATA nidApp;
HMENU hPopMenu;
TCHAR szTitle[MAX_LOADSTRING];               // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];         // the main window class name
TCHAR szApplicationToolTip[MAX_LOADSTRING];       // the main window class name

std::string token;
std::string login = "Not logged in";

GithubStatus *status;
Status lastStatus;

ATOM MyRegisterClass(HINSTANCE hInstance);

BOOL InitInstance(HINSTANCE, int nCmdShow);

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT StatusIcon(const char *statusText) {
	if (!strcmp(statusText, "none")) return IDI_NONE;
	if (!strcmp(statusText, "minor")) return IDI_MINOR;
	if (!strcmp(statusText, "major")) return IDI_MAJOR;
	if (!strcmp(statusText, "critical")) return IDI_CRITICAL;
	return IDI_UNKNOWN;
}

void StatusReceivedFirst(const Status &s) {
	lastStatus = s;
	strncpy(nidApp.szTip, lastStatus.message.c_str(), sizeof(nidApp.szTip) - 1);
	nidApp.uFlags |= NIF_TIP;
	nidApp.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(StatusIcon(s.icon.c_str())));
	Shell_NotifyIcon(NIM_MODIFY, &nidApp);
}

void StatusReceivedNext(const Status &s) {
	auto message = lastStatus.message;
	StatusReceivedFirst(s);
	if (lastStatus.message != message) {
		nidApp.uFlags |= NIF_INFO;
		strncpy(nidApp.szInfoTitle, "GitHub Status", sizeof(nidApp.szInfoTitle) - 1);
		strncpy(nidApp.szInfo, lastStatus.message.c_str(), sizeof(nidApp.szInfoTitle) - 1);
	}
}

void InitStatus() {
	status->update(StatusReceivedFirst);
}

void UpdateStatus() {
	status->update(StatusReceivedNext);
}


int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_OCTISSIMO, szWindowClass, MAX_LOADSTRING);

	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	status = new GithubStatus();
	InitStatus();

	SetTimer(hWnd, IDT_TIMER, 60000, (TIMERPROC) NULL);
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OCTISSIMO));

	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OCTISSIMO));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_OCTISSIMO);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_OCTISSIMO));

	return RegisterClassEx(&wcex);
}

void PerformLogin() {
	if (!token.empty()) {
		try {
			auto request = Request("api.github.com");
			auto callback = [](const std::string &response) {
				auto result = json::parse(response);
				login = result["login"];
			};
			string auth = "Authorization: Bearer " + token;
			request.update("/user", callback, auth);
		} catch(...) {
			fprintf(stderr, "Some exception during login\n");
		}
	}
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {

	UNREFERENCED_PARAMETER(nCmdShow);

	HICON hMainIcon;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
							  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd) {
		return FALSE;
	}

	hMainIcon = LoadIcon(hInstance, (LPCTSTR) MAKEINTRESOURCE(IDI_NONE));

	nidApp.cbSize = sizeof(NOTIFYICONDATA); // sizeof the struct in bytes
	nidApp.hWnd = (HWND) hWnd;              //handle of the window which will process this app. messages
	nidApp.uID = IDI_OCTISSIMO;           //ID of the icon that willl appear in the system tray
	nidApp.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; //ORing of all the flags
	nidApp.hIcon = LoadIcon(hInstance, (LPCTSTR) MAKEINTRESOURCE(IDI_NONE)); // handle of the Icon to be displayed, obtained from LoadIcon
	nidApp.uCallbackMessage = WM_USER_SHELLICON;
	LoadString(hInstance, IDS_APPTOOLTIP, nidApp.szTip, MAX_LOADSTRING);
	Shell_NotifyIcon(NIM_ADD, &nidApp);

	HKEY key;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\OpenGrabeso\\Octissimo"), 0, KEY_READ, &key) == 0) {
		TCHAR buffer[100];
		DWORD size = sizeof(buffer);
		if (RegGetValue(key, "", _T("token"), RRF_RT_REG_SZ, NULL, buffer, &size) == 0) {
			token = buffer;
		}
		RegCloseKey(key);
	}

	PerformLogin();

	return TRUE;
}


INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
		case WM_INITDIALOG:
			return (INT_PTR) TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR) TRUE;
			}
			break;
	}
	return (INT_PTR) FALSE;
}

INT_PTR CALLBACK Login(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
		case WM_INITDIALOG:
			// security / privacy: we allow the user only to set them token, not to retrieve it
			//SetDlgItemText(hDlg, IDC_TOKENEDIT, token.c_str());

			SetDlgItemText(hDlg, IDC_CURRENTUSERTEXT, login.c_str());
			return (INT_PTR) TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
				if (LOWORD(wParam) == IDOK) {
					TCHAR buffer[100];
					GetDlgItemText(hDlg, IDC_TOKENEDIT, buffer, sizeof(buffer));

					// as we leave the input empty by default, do not reset the token if user types nothing and presses OK
					// an alternative would be to disable OK when the input is empty
					if (strlen(buffer) > 0) {
						token = buffer;

						HKEY key;
						if (RegCreateKey(HKEY_CURRENT_USER, _T("Software\\OpenGrabeso\\Octissimo"), &key) == 0) {
							RegSetValueEx(key, _T("token"), 0, REG_SZ, (const BYTE *) buffer, strlen(buffer));
							RegCloseKey(key);
						}

						PerformLogin();
					}

				}
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR) TRUE;
			}
			break;
	}
	return (INT_PTR) FALSE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	int wmId, wmEvent;
	POINT lpClickPoint;

	switch (message) {

		case WM_USER_SHELLICON:
			// systray msg callback
			switch (LOWORD(lParam)) {
				case WM_RBUTTONDOWN:
					UINT uFlag = MF_BYPOSITION | MF_STRING;
					GetCursorPos(&lpClickPoint);
					hPopMenu = CreatePopupMenu();
					InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDM_ABOUT, _T("About"));
					InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDM_LOGIN, _T("Login"));
					InsertMenu(hPopMenu, 0xFFFFFFFF, MF_SEPARATOR, IDM_SEP, _T("SEP"));
					InsertMenu(hPopMenu, 0xFFFFFFFF, uFlag, IDM_CHECK, _T("Check status"));
					InsertMenu(hPopMenu, 0xFFFFFFFF, MF_SEPARATOR, IDM_SEP, _T("SEP"));
					InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDM_EXIT, _T("Exit"));

					SetForegroundWindow(hWnd);
					TrackPopupMenu(hPopMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, lpClickPoint.x,
										lpClickPoint.y, 0, hWnd, NULL);
					return TRUE;

			}
			break;
		case IDT_TIMER:
			UpdateStatus();
			break;
		case WM_COMMAND:
			wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			// Parse the menu selections:
			switch (wmId) {
				case IDM_ABOUT:
					DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
					break;
				case IDM_LOGIN:
					DialogBox(hInst, MAKEINTRESOURCE(IDD_LOGINBOX), hWnd, Login);
					break;
				case IDM_CHECK:
					UpdateStatus();
					break;
				case IDM_EXIT:
					Shell_NotifyIcon(NIM_DELETE, &nidApp);
					DestroyWindow(hWnd);
					break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
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

