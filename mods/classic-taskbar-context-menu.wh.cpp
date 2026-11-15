// ==WindhawkMod==
// @id              classic-taskbar-context-menu
// @name            Non Immersive Taskbar Context Menu
// @description     Restores the non-immersive taskbar context menu
// @version         1.0.3
// @author          ItsProfessional
// @github          https://github.com/ItsProfessional
// @include         explorer.exe
// @compilerOptions -lgdi32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Non Immersive Taskbar Context Menu
This mod restores the non-immersive taskbar context menus.

You can also disable icons in the context menu in the settings of this mod.

**Only works on the Windows 10 taskbar.**

The code is based on [Taskbar-Context-Menu-Tweaker](https://github.com/rikka0w0/Taskbar-Context-Menu-Tweaker).

![Screenshot](https://raw.githubusercontent.com/ItsProfessional/Screenshots/main/Windhawk/classic-taskbar-context-menu.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showIcons: true
  $name: Show icons
  $description: Enable this if you want icons to be shown in the context menu of the taskbar.
*/
// ==/WindhawkModSettings==

struct {
    bool showIcons;
} settings;

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>
#include <string.h>
#include <unordered_set>

//msvcrt.lib;msvcrtd.lib
#ifdef _DEBUG
#pragma comment (linker, "/nodefaultlib:msvcrtd.lib")
#else
#pragma comment (linker, "/nodefaultlib:msvcrt.lib")
#endif



#define MYICON_SETTING 0
#define MYICON_TASKMGR 1
#define MYICON_SHOWDESKTOP 2
#define MyIcons_Count 3

#define STANDARD_DPI 96
#define DPI_SCALE(in) in * GetDPI() / STANDARD_DPI


std::unordered_set<HWND> handles;

UINT g_subclassRegisteredMsg = RegisterWindowMessage(L"Windhawk_SetWindowSubclassFromAnyThread_classic-taskbar-context-menu");
const UINT WM_TASKBARCREATED = RegisterWindowMessage(L"TaskbarCreated");

struct SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM {
    SUBCLASSPROC pfnSubclass;
    UINT_PTR uIdSubclass;
    DWORD_PTR dwRefData;
    BOOL result;
};

LRESULT CALLBACK CallWndProcForWindowSubclass(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == g_subclassRegisteredMsg && cwp->wParam) {
            SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM* param = (SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM*)cwp->lParam;
            param->result = SetWindowSubclass(cwp->hwnd, param->pfnSubclass, param->uIdSubclass, param->dwRefData);
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

BOOL SetWindowSubclassFromAnyThread(HWND hWnd, SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    
    if (dwThreadId == 0) return FALSE;
    if (dwThreadId == GetCurrentThreadId()) return SetWindowSubclass(hWnd, pfnSubclass, uIdSubclass, dwRefData);

    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProcForWindowSubclass, nullptr, dwThreadId);

    if (!hook) return FALSE;

    SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM param;
    param.pfnSubclass = pfnSubclass;
    param.uIdSubclass = uIdSubclass;
    param.dwRefData = dwRefData;
    param.result = FALSE;
    
    SendMessage(hWnd, g_subclassRegisteredMsg, TRUE, (WPARAM)&param);

    UnhookWindowsHookEx(hook);

    return param.result;
}


int GetDPI() {
	HDC hdc = GetDC(NULL);
	int DPI = GetDeviceCaps(hdc, LOGPIXELSX);
	ReleaseDC(NULL, hdc);
	return DPI;
}



#define ICON_SETTING_INDEX 21
#define ICON_SHOWDESKTOP_INDEX 34

static HBITMAP MyIcons[MyIcons_Count];



HBITMAP IconToBitmap(HICON hIcon, INT size = 0) {
	ICONINFO info = { 0 };
	if (hIcon == NULL || !GetIconInfo(hIcon, &info) || !info.fIcon) {
		//TO-DO: Free the hbmColor and hbmMask

		return NULL;
	}

	INT nWidth = 0;
	INT nHeight = 0;
	if (size > 0) {
		nWidth = size;
		nHeight = size;
	}
	else {
		if (info.hbmColor != NULL) {
			BITMAP bmp = { 0 };
			GetObject(info.hbmColor, sizeof(bmp), &bmp);

			nWidth = bmp.bmWidth;
			nHeight = bmp.bmHeight;
		}
	}

	if (info.hbmColor != NULL) {
		DeleteObject(info.hbmColor);
		info.hbmColor = NULL;
	}

	if (info.hbmMask != NULL) {
		DeleteObject(info.hbmMask);
		info.hbmMask = NULL;
	}

	if (nWidth <= 0 || nHeight <= 0) return NULL;

	INT nPixelCount = nWidth * nHeight;

	HDC dc = GetDC(NULL);
	INT* pData = NULL;
	HDC dcMem = NULL;
	HBITMAP hBmpOld = NULL;
	bool* pOpaque = NULL;
	HBITMAP dib = NULL;
	BOOL bSuccess = FALSE;

	do {
		BITMAPINFOHEADER bi = { 0 };
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = nWidth;
		bi.biHeight = -nHeight;
		bi.biPlanes = 1;
		bi.biBitCount = 32;
		bi.biCompression = BI_RGB;
		dib = CreateDIBSection(dc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (VOID**)&pData, NULL, 0);
		if (dib == NULL) break;

		memset(pData, 0, nPixelCount * 4);

		dcMem = CreateCompatibleDC(dc);
		if (dcMem == NULL) break;

		hBmpOld = (HBITMAP)SelectObject(dcMem, dib);
		::DrawIconEx(dcMem, 0, 0, hIcon, nWidth, nHeight, 0, NULL, DI_MASK);

		pOpaque = (bool*)malloc(sizeof(bool) * nPixelCount);
		if (pOpaque == NULL) break;

		for (INT i = 0; i < nPixelCount; ++i)
			pOpaque[i] = !pData[i];

		memset(pData, 0, nPixelCount * 4);
		::DrawIconEx(dcMem, 0, 0, hIcon, nWidth, nHeight, 0, NULL, DI_NORMAL);

		BOOL bPixelHasAlpha = FALSE;
		UINT* pPixel = (UINT*)pData;
		for (INT i = 0; i < nPixelCount; ++i, ++pPixel) {
			if ((*pPixel & 0xff000000) != 0) {
				bPixelHasAlpha = TRUE;
				break;
			}
		}

		if (!bPixelHasAlpha) {
			pPixel = (UINT*)pData;
			for (INT i = 0; i < nPixelCount; ++i, ++pPixel) {
				if (pOpaque[i]) *pPixel |= 0xFF000000;
				else *pPixel &= 0x00FFFFFF;
			}
		}

		bSuccess = TRUE;

	} while (FALSE);


	if (pOpaque != NULL) {
		free(pOpaque);
		pOpaque = NULL;
	}

	if (dcMem != NULL) {
		SelectObject(dcMem, hBmpOld);
		DeleteDC(dcMem);
	}

	ReleaseDC(NULL, dc);

	if (!bSuccess) {
		if (dib != NULL)
		{
			DeleteObject(dib);
			dib = NULL;
		}
	}

	return dib;
}





HBITMAP MyIcons_Get(unsigned char index) {
	return MyIcons[index];
}


void MyIcons_Load() {
	HICON hIcon = NULL;
	ExtractIconEx(L"shell32.dll", ICON_SETTING_INDEX, NULL, &hIcon, 1);
	MyIcons[MYICON_SETTING] = IconToBitmap(hIcon, DPI_SCALE(16));
	DestroyIcon(hIcon);

	ExtractIconEx(L"taskmgr.exe", 0, NULL, &hIcon, 1);
	MyIcons[MYICON_TASKMGR] = IconToBitmap(hIcon, DPI_SCALE(16));
	DestroyIcon(hIcon);

	ExtractIconEx(L"shell32.dll", ICON_SHOWDESKTOP_INDEX, NULL, &hIcon, 1);
	MyIcons[MYICON_SHOWDESKTOP] = IconToBitmap(hIcon, DPI_SCALE(16));
	DestroyIcon(hIcon);
}


void MyIcons_Free() {
	for (int i = 0; i < MyIcons_Count; i++) {
		if (MyIcons[i] != NULL) DeleteObject(MyIcons[i]);
    }
}


static HWND hWnd_TaskBar = 0;
static HWND hWnd_TaskBar_SecondScreen = NULL;
static HWND hWnd_ATL1 = 0;
static HWND hWnd_ATL2 = 0;


void ApplyClassicMenu(HMENU hMenu) {
	MENUINFO info;

	info.cbSize = sizeof(MENUINFO);
	info.fMask = MIM_BACKGROUND;

	GetMenuInfo(hMenu, &info);
    
	info.hbrBack = NULL;

	SetMenuInfo(hMenu, &info);

	for (int i = 0; i < GetMenuItemCount(hMenu); i++) {
		MENUITEMINFO menuInfo;

		menuInfo.cbSize = sizeof(MENUITEMINFO);
		menuInfo.fMask = MIIM_FTYPE | MIIM_SUBMENU;

		GetMenuItemInfo(hMenu, i, true, &menuInfo);

		menuInfo.fType &= ~MFT_OWNERDRAW;
		menuInfo.fMask = MIIM_FTYPE;

		SetMenuItemInfo(hMenu, i, true, &menuInfo);
	}
}


void ApplyClassicMenuIfPossible(HWND hWnd, HMENU hMenu) {
	char clsName[256];
	GetClassNameA(hWnd, clsName, 256);

    if (strcmp(clsName, "TrayShowDesktopButtonWClass") == 0) {
		if (settings.showIcons) SetMenuItemBitmaps(hMenu, 0x1A2D, MF_BYCOMMAND, MyIcons_Get(MYICON_SHOWDESKTOP), MyIcons_Get(MYICON_SHOWDESKTOP));
		ApplyClassicMenu(hMenu);

    } else if
    (
        strcmp(clsName, "NotificationsMenuOwner") == 0 ||       // Notification Button
        strcmp(clsName, "LauncherTipWnd") == 0 ||               // Win+X menu
        strcmp(clsName, "MultitaskingViewFrame") == 0 ||        // Multitask Button
        hWnd == hWnd_ATL1 || hWnd == hWnd_ATL2                  // Network Icon and Volumn Icon
    ) {
        ApplyClassicMenu(hMenu);

        for (int i = 0; i < GetMenuItemCount(hMenu); i++) ApplyClassicMenu(GetSubMenu(hMenu, i));
    }
}




LRESULT CALLBACK TaskbarSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	switch (uMsg) {
        case WM_INITMENUPOPUP: {
            ApplyClassicMenu((HMENU)wParam);

            if (!settings.showIcons) return DefSubclassProc(hWnd, uMsg, wParam, lParam);;

            HWND hWnd_NotifyWnd = FindWindowEx(hWnd, NULL, TEXT("TrayNotifyWnd"), NULL);
            HWND hWnd_Clock = FindWindowEx(hWnd_NotifyWnd, NULL, TEXT("TrayClockWClass"), NULL);

            RECT rect;
            GetWindowRect(hWnd_Clock, &rect);

            POINT pt;
            GetCursorPos(&pt);

            if (((pt.x > rect.left)&(pt.x < rect.right)&(pt.y > rect.top)&(pt.y < rect.bottom))) {
                SetMenuItemBitmaps((HMENU)wParam, 413, MF_BYCOMMAND, MyIcons_Get(MYICON_SETTING), MyIcons_Get(MYICON_SETTING));
                SetMenuItemBitmaps((HMENU)wParam, 420, MF_BYCOMMAND, MyIcons_Get(MYICON_TASKMGR), MyIcons_Get(MYICON_TASKMGR));
                SetMenuItemBitmaps((HMENU)wParam, 407, MF_BYCOMMAND, MyIcons_Get(MYICON_SHOWDESKTOP), MyIcons_Get(MYICON_SHOWDESKTOP));

            } else if (hWnd_NotifyWnd == hWnd || hWnd_TaskBar == hWnd) {
                SetMenuItemBitmaps((HMENU)wParam, 414, MF_BYCOMMAND, MyIcons_Get(MYICON_SETTING), MyIcons_Get(MYICON_SETTING));
                SetMenuItemBitmaps((HMENU)wParam, 421, MF_BYCOMMAND, MyIcons_Get(MYICON_TASKMGR), MyIcons_Get(MYICON_TASKMGR));
                SetMenuItemBitmaps((HMENU)wParam, 408, MF_BYCOMMAND, MyIcons_Get(MYICON_SHOWDESKTOP), MyIcons_Get(MYICON_SHOWDESKTOP));

                SetMenuItemBitmaps((HMENU)wParam, 413, MF_BYCOMMAND, MyIcons_Get(MYICON_SETTING), MyIcons_Get(MYICON_SETTING));
                SetMenuItemBitmaps((HMENU)wParam, 420, MF_BYCOMMAND, MyIcons_Get(MYICON_TASKMGR), MyIcons_Get(MYICON_TASKMGR));
                SetMenuItemBitmaps((HMENU)wParam, 407, MF_BYCOMMAND, MyIcons_Get(MYICON_SHOWDESKTOP), MyIcons_Get(MYICON_SHOWDESKTOP));

            } else {
                SetMenuItemBitmaps((HMENU)wParam, 0x019E, MF_BYCOMMAND, MyIcons_Get(MYICON_SETTING), MyIcons_Get(MYICON_SETTING));
                SetMenuItemBitmaps((HMENU)wParam, 0x01A5, MF_BYCOMMAND, MyIcons_Get(MYICON_TASKMGR), MyIcons_Get(MYICON_TASKMGR));
                SetMenuItemBitmaps((HMENU)wParam, 0x0198, MF_BYCOMMAND, MyIcons_Get(MYICON_SHOWDESKTOP), MyIcons_Get(MYICON_SHOWDESKTOP));
            }
        }

        default:
            if (uMsg == g_subclassRegisteredMsg && !wParam) RemoveWindowSubclass(hWnd, TaskbarSubclassProc, 0);
            break;
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK SecondTaskbarSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (uMsg == g_subclassRegisteredMsg && !wParam) RemoveWindowSubclass(hWnd, SecondTaskbarSubclassProc, 0);

	if (uMsg == WM_INITMENUPOPUP) {
        ApplyClassicMenu((HMENU)wParam);

		if (!settings.showIcons) return DefSubclassProc(hWnd, uMsg, wParam, lParam);;

		SetMenuItemBitmaps((HMENU)wParam, 0x19d, MF_BYCOMMAND, MyIcons_Get(MYICON_SETTING), MyIcons_Get(MYICON_SETTING));
		SetMenuItemBitmaps((HMENU)wParam, 0x1a4, MF_BYCOMMAND, MyIcons_Get(MYICON_TASKMGR), MyIcons_Get(MYICON_TASKMGR));
		SetMenuItemBitmaps((HMENU)wParam, 0x197, MF_BYCOMMAND, MyIcons_Get(MYICON_SHOWDESKTOP), MyIcons_Get(MYICON_SHOWDESKTOP));
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}



using TrackPopupMenu_t = decltype(&TrackPopupMenu);
TrackPopupMenu_t pOriginalTrackPopupMenu;
BOOL WINAPI TrackPopupMenu_Hook(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd, CONST RECT* prcRect) {
	if (IsWindow(hWnd)) ApplyClassicMenuIfPossible(hWnd, hMenu);

	return pOriginalTrackPopupMenu(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
}


using TrackPopupMenuEx_t = decltype(&TrackPopupMenuEx);
TrackPopupMenuEx_t pOriginalTrackPopupMenuEx;
BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm) {
	if (IsWindow(hWnd)) ApplyClassicMenuIfPossible(hWnd, hMenu);
	
	return pOriginalTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);
}



BOOL CALLBACK EnumWindowsCallBack(HWND hwnd, LPARAM lParam) {
	if (GetWindowThreadProcessId(hwnd, NULL) == (DWORD)lParam) {
		// Window belong to the same thread
		char className[255];
		GetClassNameA(hwnd, className, sizeof(className));

		if (className[0] == 'A' && className[1] == 'T' && className[2] == 'L') {		
            if (GetWindowTextLengthA(hwnd) == 0) hWnd_ATL1 = hwnd;
            else hWnd_ATL2 = hwnd;
		}
	}

	return TRUE;
}

void SubclassTaskbars()
{
    // The taskbar on the first screen
    hWnd_TaskBar = FindWindowW(L"Shell_TrayWnd", NULL);

    if (IsWindow(hWnd_TaskBar)) {
        handles.insert(hWnd_TaskBar);
        SetWindowSubclassFromAnyThread(hWnd_TaskBar, TaskbarSubclassProc, 0, 0);
    }

    // The taskbar on the rest of the screens
    hWnd_TaskBar_SecondScreen = FindWindowW(L"Shell_SecondaryTrayWnd", NULL);

    if (IsWindow(hWnd_TaskBar_SecondScreen)) {
        handles.insert(hWnd_TaskBar_SecondScreen);
        SetWindowSubclassFromAnyThread(hWnd_TaskBar_SecondScreen, SecondTaskbarSubclassProc, 0, 0);
    }


    // Network & Volumn
    HWND hPNIHiddenWnd = FindWindowA("PNIHiddenWnd", NULL);
    EnumWindows(EnumWindowsCallBack, GetWindowThreadProcessId(hPNIHiddenWnd, NULL));
}



LRESULT CALLBACK ExplorerWindowSubclassProc(
    _In_ HWND hWnd,
    _In_ UINT uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam,
    _In_ UINT_PTR uIdSubclass,
    _In_ DWORD_PTR dwRefData
) {
    if(!hWnd) return DefSubclassProc(hWnd, uMsg, wParam, lParam);

    if (uMsg == g_subclassRegisteredMsg && !wParam) {
        RemoveWindowSubclass(hWnd, ExplorerWindowSubclassProc, 0);
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    HWND hWnd_Shell = GetShellWindow();
    if(!hWnd_Shell) return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    if(hWnd != hWnd_Shell) return DefSubclassProc(hWnd, uMsg, wParam, lParam);


    if (uMsg == WM_TASKBARCREATED) SubclassTaskbars();

    
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}


BOOL CALLBACK EnumExplorerWindowsFunc(HWND hWnd, LPARAM lParam) {
    if(!hWnd) return TRUE;

    HWND hWnd_Shell = GetShellWindow();
    if(!hWnd_Shell) return TRUE;

    if(hWnd != hWnd_Shell) return TRUE;

    handles.insert(hWnd);
    SetWindowSubclass(hWnd, ExplorerWindowSubclassProc, 0, 0);
    
    return TRUE;
}




using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t pOriginalCreateWindowExW;
HWND WINAPI CreateWindowExWHook(
    DWORD dwExStyle,
    LPCWSTR lpClassName,
    LPCWSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam
) {
    HWND hWnd = pOriginalCreateWindowExW(
        dwExStyle, lpClassName, lpWindowName,
        dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam
    );

    if (!hWnd) return hWnd;
    

    handles.insert(hWnd);
    SetWindowSubclass(hWnd, ExplorerWindowSubclassProc, 0, 0);

    return hWnd;
}







void LoadSettings() {
    settings.showIcons = Wh_GetIntSetting(L"showIcons");
}


BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    MyIcons_Load();

    // For the case where the shell is not already running and thus taskbar has not initialized yet
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExWHook, (void**)&pOriginalCreateWindowExW);

    // For the case where the mod was started after the shell is already running, but the taskbar has not initialized yet.
    EnumWindows(EnumExplorerWindowsFunc, 0);


    Wh_SetFunctionHook((void*)TrackPopupMenu, (void*)TrackPopupMenu_Hook, (void**)&pOriginalTrackPopupMenu);
    Wh_SetFunctionHook((void*)TrackPopupMenuEx, (void*)TrackPopupMenuEx_Hook, (void**)&pOriginalTrackPopupMenuEx);


    // For the case where the mod started the shell is already and after taskbar has already initialized.
    HWND hWnd = GetShellWindow();
    if(hWnd) SubclassTaskbars();


    return TRUE;
}


void Wh_ModUninit() {
    Wh_Log(L"Uninit");

	MyIcons_Free();

    // Unsubclass the taskbars
    for (const HWND& hWnd : handles) SendMessage(hWnd, g_subclassRegisteredMsg, FALSE, 0);
}


void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    LoadSettings();
}
