// ==WindhawkMod==
// @id              classic-taskbar-context-menu
// @name            Classic Taskbar Context Menu
// @description     Restores the classic taskbar context menu
// @version         1.0.0
// @author          ItsProfessional
// @github          https://github.com/ItsProfessional
// @include         explorer.exe
// @compilerOptions -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Taskbar Context Menu
This mod restores the classic taskbar context menu.

You can also disable icons in the context menu in the settings of this mod.

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

//msvcrt.lib;msvcrtd.lib
#ifdef _DEBUG
#pragma comment (linker, "/nodefaultlib:msvcrtd.lib")
#else
#pragma comment (linker, "/nodefaultlib:msvcrt.lib")
#endif



//Icons
#define MYICON_SETTING 0
#define MYICON_TASKMGR 1
#define MYICON_SHOWDESKTOP 2
#define MyIcons_Count 3

//Icon
#define STANDARD_DPI 96
#define DPI_SCALE(in) in * GetDPI() / STANDARD_DPI


//Custom Messages
#define WM_TWEAKER 0x0409
#define TWEAKER_EXIT 0x90



// Memory functions
#pragma function(memcpy)
void* __cdecl memcpy(void* dst, const void* src, size_t count)
{
	void* ret = dst;

#if defined (_M_IA64)
	{
		extern void RtlMoveMemory(void*, const void*, size_t count);

		RtlMoveMemory(dst, src, count);
	}
#else /* defined (_M_IA64) */
	/*
	* copy from lower addresses to higher addresses
	*/
	while (count--) {
		*(char*)dst = *(char*)src;
		dst = (char*)dst + 1;
		src = (char*)src + 1;
	}
#endif /* defined (_M_IA64) */

	return(ret);
}



#pragma function(memset)
void* __cdecl memset(void* src, int c, size_t count)
{
	char* tmpsrc = (char*)src;
	while (count--)
		*tmpsrc++ = (char)c;
	return src;
}


#pragma function(strcmp)
int __cdecl strcmp(const char* src, const char* dst)
{
	int   ret = 0;

	while (!(ret = *(unsigned   char*)src - *(unsigned   char*)dst) && *dst)
		++src, ++dst;

	if (ret < 0)
		ret = -1;
	else   if (ret > 0)
		ret = 1;

	return(ret);
}

#pragma function(strlen)
size_t __cdecl strlen(char const* _Str) {
	if (_Str == NULL)
		return 0;

	size_t len = 0;
	for (; *_Str++ != '\0';)
		len++;

	return len;
}


int GetDPI() {
	HDC hdc = GetDC(NULL);
	int DPI = GetDeviceCaps(hdc, LOGPIXELSX);
	ReleaseDC(NULL, hdc);
	return DPI;
}





// Icon functions
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



typedef struct MENU98_INIT_T {
	HWND hWnd_TaskBar;
	void* TrackPopupMenuEx;
	LPSTR cmdLine;
} MENU98_INIT;


typedef DWORD(__cdecl* FPT___Menu98Init)(MENU98_INIT*);
typedef DWORD(__cdecl* FPT___Menu98Unload)(LPVOID);



static HMODULE gLibModule = 0;

static LONG_PTR OldWndProc_TaskBar = NULL;
static LONG_PTR OldWndProc_TaskBar_SecondScreen = NULL;

static HWND hWnd_TaskBar = 0;
static HWND hWnd_TaskBar_SecondScreen = NULL;
static HWND hWnd_ATL1 = 0;
static HWND hWnd_ATL2 = 0;

static HMODULE menu98Module = 0;

void ClassicMenu(HMENU hMenu) {
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

void ClassicMenuIfPossible(HWND hWnd, HMENU hMenu) {
	char clsName[256];
	GetClassNameA(hWnd, clsName, 256);

	if (strcmp(clsName, "TrayShowDesktopButtonWClass") == 0) {
		if (settings.showIcons) SetMenuItemBitmaps(hMenu, 0x1A2D, MF_BYCOMMAND, MyIcons_Get(MYICON_SHOWDESKTOP), MyIcons_Get(MYICON_SHOWDESKTOP));
		ClassicMenu(hMenu);
	} else if (
		strcmp(clsName, "NotificationsMenuOwner") == 0 ||		// Notification Button
		strcmp(clsName, "LauncherTipWnd") == 0 ||				// Win+X menu
		strcmp(clsName, "MultitaskingViewFrame") == 0 ||		// Multitask Button
		
        hWnd == hWnd_ATL1 || hWnd == hWnd_ATL2) {				// Network Icon and Volumn Icon
		ClassicMenu(hMenu);

		for (int i = 0; i < GetMenuItemCount(hMenu); i++)
			ClassicMenu(GetSubMenu(hMenu, i));
	}
}

void RestoreWndProc() {
	if (OldWndProc_TaskBar != NULL) {
		SetWindowLongPtr(hWnd_TaskBar, GWLP_WNDPROC, OldWndProc_TaskBar);
    }

	if (OldWndProc_TaskBar_SecondScreen != NULL) {
		SetWindowLongPtr(hWnd_TaskBar_SecondScreen, GWLP_WNDPROC, OldWndProc_TaskBar_SecondScreen);
    }
}

void CloseBackground() {
	MyIcons_Free();
	RestoreWndProc();

	if (menu98Module != NULL) {
		FPT___Menu98Unload fpMenu98Unload = (FPT___Menu98Unload)GetProcAddress(menu98Module, "__Menu98Unload");

		if (fpMenu98Unload != NULL) {
			fpMenu98Unload(NULL);
		} else {
			MessageBoxW(0, L"Failed to unload Menu98!", L"Fatal Error", MB_ICONERROR);
		}
	}


	CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FreeLibraryAndExitThread, gLibModule, 0, NULL));
}

LRESULT CALLBACK WndProc_TaskBar(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	WNDPROC fpWndProcOld = WNDPROC(OldWndProc_TaskBar);

	switch (uMsg) {
        case WM_TWEAKER: {
            if (wParam == TWEAKER_EXIT) CloseBackground();
            return 0;
        }

        case WM_INITMENUPOPUP: {
            LRESULT ret = fpWndProcOld(hwnd, uMsg, wParam, lParam);

            ClassicMenu((HMENU)wParam);

            if (!settings.showIcons) return ret;

            HWND hWnd_NotifyWnd = FindWindowEx(hwnd, NULL, TEXT("TrayNotifyWnd"), NULL);
            HWND hWnd_Clock = FindWindowEx(hWnd_NotifyWnd, NULL, TEXT("TrayClockWClass"), NULL);

            RECT rect;
            GetWindowRect(hWnd_Clock, &rect);

            POINT pt;
            GetCursorPos(&pt);

            if (((pt.x > rect.left)&(pt.x < rect.right)&(pt.y > rect.top)&(pt.y < rect.bottom))) {
                SetMenuItemBitmaps((HMENU)wParam, 413, MF_BYCOMMAND, MyIcons_Get(MYICON_SETTING), MyIcons_Get(MYICON_SETTING));
                SetMenuItemBitmaps((HMENU)wParam, 420, MF_BYCOMMAND, MyIcons_Get(MYICON_TASKMGR), MyIcons_Get(MYICON_TASKMGR));
                SetMenuItemBitmaps((HMENU)wParam, 407, MF_BYCOMMAND, MyIcons_Get(MYICON_SHOWDESKTOP), MyIcons_Get(MYICON_SHOWDESKTOP));
            } else if (hWnd_NotifyWnd == hwnd || hWnd_TaskBar == hwnd) {
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

            return ret;
        }
	}

	return fpWndProcOld(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK WndProc_TaskBar_SecondScreen(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	WNDPROC fpWndProcOld = WNDPROC(OldWndProc_TaskBar_SecondScreen);

	if (uMsg == WM_INITMENUPOPUP) {
		LRESULT ret = fpWndProcOld(hwnd, uMsg, wParam, lParam);

        ClassicMenu((HMENU)wParam);
		if (!settings.showIcons) return ret;

		SetMenuItemBitmaps((HMENU)wParam, 0x19d, MF_BYCOMMAND, MyIcons_Get(MYICON_SETTING), MyIcons_Get(MYICON_SETTING));
		SetMenuItemBitmaps((HMENU)wParam, 0x1a4, MF_BYCOMMAND, MyIcons_Get(MYICON_TASKMGR), MyIcons_Get(MYICON_TASKMGR));
		SetMenuItemBitmaps((HMENU)wParam, 0x197, MF_BYCOMMAND, MyIcons_Get(MYICON_SHOWDESKTOP), MyIcons_Get(MYICON_SHOWDESKTOP));

		return ret;
	}

	return fpWndProcOld(hwnd, uMsg, wParam, lParam);
}


//DWORD WINAPI ThreadProc(LPVOID lpParameter);

typedef BOOL (WINAPI *FPT_TrackPopupMenu)(HMENU, UINT, int, int, int, HWND, CONST RECT*);
typedef BOOL(WINAPI *FPT_TrackPopupMenuEx)(HMENU, UINT, int, int, HWND, LPTPMPARAMS);

// Pointer for calling original MessageBoxW.
FPT_TrackPopupMenu pOriginalTrackPopupMenu;
FPT_TrackPopupMenuEx pOriginalTrackPopupMenuEx;

// Detour function which overrides TrackPopupMenu.
BOOL WINAPI TrackPopupMenu_Hook(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd, CONST RECT* prcRect) {
	if (IsWindow(hWnd)) ClassicMenuIfPossible(hWnd, hMenu);

	return pOriginalTrackPopupMenu(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
}

BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm) {
	if (IsWindow(hWnd)) ClassicMenuIfPossible(hWnd, hMenu);
	
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










void LoadSettings() {
    settings.showIcons = Wh_GetIntSetting(L"showIcons");
}


BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();


    Wh_SetFunctionHook((void*)TrackPopupMenu, (void*)TrackPopupMenu_Hook, (void**)&pOriginalTrackPopupMenu);
    Wh_SetFunctionHook((void*)TrackPopupMenuEx, (void*)TrackPopupMenuEx_Hook, (void**)&pOriginalTrackPopupMenuEx);
    


	MyIcons_Load();

	hWnd_TaskBar = FindWindow(TEXT("Shell_TrayWnd"), NULL);
	// hWnd_TaskBar = *(HWND*)param;
	if (IsWindow(hWnd_TaskBar)) {
		OldWndProc_TaskBar = GetWindowLongPtr(hWnd_TaskBar, GWLP_WNDPROC);

		if (OldWndProc_TaskBar != NULL) SetWindowLongPtr(hWnd_TaskBar, GWLP_WNDPROC, (LONG_PTR)&WndProc_TaskBar);
	}

	// The taskbar on the second screen
	hWnd_TaskBar_SecondScreen = FindWindowA("Shell_SecondaryTrayWnd", NULL);
	if (IsWindow(hWnd_TaskBar_SecondScreen)) {
		OldWndProc_TaskBar_SecondScreen = GetWindowLongPtr(hWnd_TaskBar_SecondScreen, GWLP_WNDPROC);
		
        if (OldWndProc_TaskBar_SecondScreen != NULL) SetWindowLongPtr(hWnd_TaskBar_SecondScreen, GWLP_WNDPROC, (LONG_PTR)&WndProc_TaskBar_SecondScreen);
	}

	menu98Module = LoadLibraryA("menu98.dll");
	if (menu98Module != NULL) {
		FPT___Menu98Init fpMenu98Init = (FPT___Menu98Init)GetProcAddress(menu98Module, "__Menu98Init");

		if (fpMenu98Init != NULL) {
			MENU98_INIT menu98InitInfo;

			menu98InitInfo.hWnd_TaskBar = hWnd_TaskBar;
			menu98InitInfo.TrackPopupMenuEx = (void *) pOriginalTrackPopupMenuEx;
			menu98InitInfo.cmdLine = (char*)hWnd_TaskBar + sizeof(HWND);

			fpMenu98Init(&menu98InitInfo);
		}
	}

	// Network & Volumn
	HWND hPNIHiddenWnd = FindWindowA("PNIHiddenWnd", NULL);
	EnumWindows(EnumWindowsCallBack, GetWindowThreadProcessId(hPNIHiddenWnd, NULL));

	// Notification
	HWND hNotificationWindow = FindWindowA("NotifyIconOverflowWindow", NULL); // EnumChildWindows


    return TRUE;
}


void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    CloseBackground();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    LoadSettings();
}
