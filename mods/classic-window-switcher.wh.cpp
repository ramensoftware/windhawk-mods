// ==WindhawkMod==
// @id              classic-window-switcher
// @name            ClassicWindowSwitcher
// @description     Bring back the classic Alt+Tab dialog
// @version         1.2
// @author          Ingan121
// @github          https://github.com/Ingan121
// @twitter         https://twitter.com/ingan121
// @homepage        https://www.ingan121.com/
// @include         windhawk-mod-uiaccess.exe
// @include         windhawk.exe
// @license         gpl-2.0
// @compilerOptions -ldwmapi -lcomctl32 -lgdiplus -lshcore -luxtheme -lruntimeobject -lgdi32 -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# ClassicWindowSwitcher
* This mod brings back the classic Alt+Tab dialog, even on Windows 11 24H2+, which removed the old switcher in win32kfull.sys.
* This is a direct port of my [ClassicWindowSwitcher](https://github.com/Ingan121/ClassicWindowSwitcher), which is a fork of valinet's [SimpleWindowSwitcher](https://github.com/valinet/sws).
![Animated Screenshot](https://raw.githubusercontent.com/Ingan121/ClassicWindowSwitcher/refs/heads/master/cws.webp)
## Differences with the original classic switcher
* It works even on Windows 11 24H2 and later.
* It shows UWP icons properly.
* You can navigate through the items with a mouse, the mouse wheel, or arrow keys.
    * Press Ctrl+Alt+Tab to keep the switcher open after releasing the Alt key.
* You can close a focused window by pressing the Del key while the switcher is active.
* It can show a per-application window list when you press Alt+Tilde.
* It features many configurable options.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ShowDelay: 100
  $name: Show delay (ms)
  $name:ko-KR: 표시 지연 시간 (ms)
  $description: Set to the number of milliseconds to wait before showing the switcher. Set to 0 to show the switcher immediately.
  $description:ko-KR: 전환기를 표시하기 전에 대기할 시간을 밀리세컨드 단위로 입력하십시오. 0을 입력하면 전환기가 즉시 표시됩니다.
- IncludeWallpaper: false
  $name: Include 'show desktop' item
  $name:ko-KR: "'바탕 화면 표시' 항목 표시"
  $description: Include the 'show desktop' item at the end of the list.
  $description:ko-KR: "'바탕 화면 표시' 항목을 목록의 끝에 포함합니다."
- PrimaryMonitorOnly: false
  $name: Only show in primary monitor
  $name:ko-KR: 주 모니터에만 표시
  $description: Show the switcher only on the primary monitor.
  $description:ko-KR: 전환기를 주 모니터에만 표시합니다.
- NoPerApplicationList: false
  $name: Disable per-application list (Alt+`)
  $name:ko-KR: 응용 프로그램별 목록 (Alt+`) 비활성화
  $description: Disable the per-application window switcher, which is shown when Alt+Tilde is pressed.
  $description:ko-KR: Alt+`를 누르면 표시되는 응용 프로그램별 전환기를 비활성화합니다.
- SwitcherIsPerApplication: false
  $name: Only show one item per application
  $name:ko-KR: 응용 프로그램 당 한 항목만 표시
  $description: Make the switcher only show a single entry per application.
  $description:ko-KR: 전환기에 응용 프로그램 별로 하나의 항목만 표시되도록 합니다.
- AlwaysUseWindowTitleAndIcon: false
  $name: Always use the window title and icon
  $name:ko-KR: 항상 창 제목 및 아이콘 사용
- ScrollWheelBehavior: "0"
  $name: Scroll wheel behavior
  $name:ko-KR: 스크롤 휠 동작
  $description: Sets the behavior of the mouse scroll wheel when the switcher is open. If there are not enough items to scroll, the behavior falls back to item by item movement.
  $description:ko-KR: 전환기가 열린 동안 마우스 스크롤 휠의 동작을 설정합니다. 스크롤 하기에 창이 충분히 많지 않으면, 항목 별 이동이 대신 사용됩니다.
  $options:
  - 0: Disabled
  - 1: Move selection item by item, only when the cursor is over the switcher.
  - 2: Move selection item by item, regardless of cursor position.
  - 3: Scroll the grid list, only when the cursor is over the switcher.
  - 4: Scroll the grid list if the cursor is over the switcher, otherwise move selection item by item.
  - 5: Scroll the grid list, regardless of cursor position.
  - 6: Move selection item by item if the cursor is over the switcher, otherwise scroll the grid list.
  $options:ko-KR:
  - 0: 비활성화
  - 1: 전환기 위에 커서가 있을 시에만 항목 간 이동
  - 2: 커서 위치와 상관 없이 항목 간 이동
  - 3: 전환기 위에 커서가 있을 시에만 목록 스크롤
  - 4: 전환기 위에 커서가 있을 시 목록 스크롤, 아니면 항목 간 이동
  - 5: 커서 위치와 상관 없이 목록 스크롤
  - 6: 전환기 위에 커서가 있을 시 항목 간 이동, 아니면 목록 스크롤
- ScrollWheelInvert: false
  $name: Invert scroll wheel behavior
  $name:ko-KR: 스크롤 휠 동작 반전
- SkipIfOneWindow: true
  $name: Skip switcher if only one window
  $name:ko-KR: 한 창만 있으면 전환기 생략
  $description: Skip showing the switcher and immediately switch to the only window if there is just one window to switch to.
  $description:ko-KR: 전환 가능한 창이 하나만 있을 경우 전환기 표시를 건너뛰고 바로 해당 창으로 전환합니다.
- CoolSwitchColumns: 7
  $name: Number of grid columns
  $name:ko-KR: 그리드 열 수
- CoolSwitchRows: 3
  $name: Number of grid rows
  $name:ko-KR: 그리드 행 수
*/
// ==/WindhawkModSettings==

// Source code is published under The GNU General Public License v2.0.
// Fork: https://github.com/Ingan121/ClassicWindowSwitcher/blob/master/LICENSE
// Upstream: https://github.com/valinet/sws/blob/master/LICENSE

#include <windhawk_utils.h>
#include <initguid.h>
#include <windowsx.h>
#include <roapi.h>
#include <oleacc.h>
#include <shellscalingapi.h>
#include <gdiplus.h>
#include <psapi.h>
#include <shlwapi.h>
#include <dwmapi.h>
#include <ShlObj.h>
#include <ShlGuid.h>
#include <Propkey.h>
#include <tlhelp32.h>

DEFINE_GUID(LiveSetting_Property_GUID, 0xc12bcd8e, 0x2a8e, 0x4950, 0x8a, 0xe7, 0x36, 0x25, 0x11, 0x1d, 0x58, 0xeb);

#define DEFAULT_DPI_X 96.0
#define DEFAULT_DPI_Y 96.0

#define SWS_UWP_ICON_SCALE_FACTOR 0.9

#define SWS_WINDOWSWITCHER_CLASSNAME L"ClassicWindowSwitcher_WH"
#define SWS_WINDOWSWITCHER_CONTOUR_SIZE 2

#define SWS_WINDOWFLAG_IS_ON_WINDOW    0b001

#define SWS_WINDOWSWITCHERLAYOUT_INCLUDE_WALLPAPER FALSE
#define SWS_WINDOWSWITCHERLAYOUT_WALLPAPER_ALWAYS_LAST TRUE
#define SWS_WINDOWSWITCHERLAYOUT_WALLPAPER_TOGGLE TRUE

#define SWS_WINDOWSWITCHERLAYOUT_ITEMSIZE 43
#define SWS_WINDOWSWITCHERLAYOUT_ICONSIZE 32

#define SWS_WINDOWSWITCHERLAYOUT_COMPUTE_DIRECTION_INITIAL 0
#define SWS_WINDOWSWITCHERLAYOUT_COMPUTE_DIRECTION_FORWARD 1
#define SWS_WINDOWSWITCHERLAYOUT_COMPUTE_DIRECTION_BACKWARD -1

#define SWS_WINDOWSWITCHERLAYOUT_WINDOWFLAGS_ISUWP                   0b00000001

#define SWS_WINDOWSWITCHER_LAYOUTMODE_FULL 0
#define SWS_WINDOWSWITCHER_LAYOUTMODE_MINI 1

#define SWS_WINDOWSWITCHER_TIMER_TOGGLE_DESKTOP 11
#define SWS_WINDOWSWITCHER_TIMER_STARTUP 12
#define SWS_WINDOWSWITCHER_TIMER_STARTUP_DELAY 3000
#define SWS_WINDOWSWITCHER_TIMER_PEEKATDESKTOP 13
#define SWS_WINDOWSWITCHER_TIMER_ASYNCKEYCHECK 14
#define SWS_WINDOWSWITCHER_TIMER_ASYNCKEYCHECK_DELAY 100
#define SWS_WINDOWSWITCHER_TIMER_UPDATEACCESSIBLETEXT 15
#define SWS_WINDOWSWITCHER_TIMER_PAINT 16
#define SWS_WINDOWSWITCHER_TIMER_PAINT_GETICONASYNC_DELAY 500
#define SWS_WINDOWSWITCHER_TIMER_CLOSEHWND 17
#define SWS_WINDOWSWITCHER_TIMER_CLOSEHWND_DELAY 300

#define SWS_WINDOWSWITCHER_ANIMATOR_FLASH_DELAY 25
#define SWS_WINDOWSWITCHER_ANIMATOR_FLASH_STEP 0.05
#define SWS_WINDOWSWITCHER_ANIMATOR_FLASH_MAXSTATE 9

#define SWS_WINDOWSWITCHER_PAINT_MSG (WM_APP + 1)
#define SWS_WINDOWSWITCHER_RELOAD_CONFIG_MSG (WM_APP + 2)
#define SWS_WINDOWSWITCHER_RELOAD_HOTKEY_MSG (WM_APP + 3)

#define SWS_WINDOWSWITCHER_PAINTFLAGS_NONE 0b000
#define SWS_WINDOWSWITCHER_PAINTFLAGS_REDRAWENTIRE 0b001
#define SWS_WINDOWSWITCHER_PAINTFLAGS_ISFLASHANIMATION 0b010
#define SWS_WINDOWSWITCHER_PAINTFLAGS_ACTIVEMASKORINDEXCHANGED 0b100

#define SWS_VECTOR_CAPACITY 500

#define SWS_SORT_DESCENDING 0b1

#define SWS_WALLPAPERSUPPORT_NONE 0
#define SWS_WALLPAPERSUPPORT_EXPLORER 1

#define SWS_SCROLLWHEELBEHAVIOR_DISABLED 0
#define SWS_SCROLLWHEELBEHAVIOR_ONLYCLIENTAREA 1
#define SWS_SCROLLWHEELBEHAVIOR_EVERYWHERE 2
#define SWS_SCROLLWHEELBEHAVIOR_ONLYCLIENTAREA_GRIDSCROLL 3
#define SWS_SCROLLWHEELBEHAVIOR_EVERYWHERE_IFCLIENTAREA_GRIDSCROLL 4
#define SWS_SCROLLWHEELBEHAVIOR_EVERYWHERE_GRIDSCROLL 5
#define SWS_SCROLLWHEELBEHAVIOR_EVERYWHERE_IFNOTCLIENTAREA_GRIDSCROLL 6

#define SWS_WINDOWSWITCHERLAYOUT_DEFAULT_GRID_COLUMNS 7
#define SWS_WINDOWSWITCHERLAYOUT_DEFAULT_GRID_ROWS 3

inline double sws_linear(double percent, double start, double end)
{
    return start + (end - start) * percent;
}

inline double sws_easing_easeOutQuad(double p)
{
    double m = p - 1; return 1 - m * m;
}

// sws_error.h
// References:
// https://github.com/rioki/rex/blob/master/rex/dbg.h#L100

typedef long long sws_error_t;

#define SWS_ERROR_SUCCESS									S_OK
#define SWS_ERROR_SUCCESS_TEXT								L"The operation completed successfully"
#define SWS_ERROR_NO_MEMORY									0xA0010002
#define SWS_ERROR_NO_MEMORY_TEXT							L"Insufficient memory. Please close some applications and try again"
#define SWS_ERROR_NOT_INITIALIZED							0xA0010003
#define SWS_ERROR_NOT_INITIALIZED_TEXT						L"Functionality is not initialized"
#define SWS_ERROR_LOADLIBRARY_FAILED						0xA0010004
#define SWS_ERROR_LOADLIBRARY_FAILED_TEXT					L"The requested library is not available"
#define SWS_ERROR_FUNCTION_NOT_FOUND						0xA0010005
#define SWS_ERROR_FUNCTION_NOT_FOUND_TEXT					L"The requested procedure was not found"
#define SWS_ERROR_INVALID_PARAMETER                         0xA0010007
#define SWS_ERROR_INVALID_PARAMETER_TEXT                    L"One or more of the parameters supplied is invalid"
#define SWS_ERROR_APPRESOLVER_NOT_AVAILABLE					0xA001000A
#define SWS_ERROR_APPRESOLVER_NOT_AVAILABLE_TEXT            L"Unable to initialize an instance of IAppResolver8"

#ifdef __cplusplus
extern "C"
{
#endif
	wchar_t* sws_error_NumToDescription(sws_error_t errnum, BOOL* bType);

	sws_error_t sws_error_Report(sws_error_t errnum, void* data);

	inline sws_error_t sws_error_GetFromInternalError(HRESULT hResult)
	{
		return hResult;
	}

	inline sws_error_t sws_error_GetFromHRESULT(HRESULT hResult)
	{
		return hResult;
	}

	inline sws_error_t sws_error_GetFromWin32Error(DWORD win32err)
	{
		return HRESULT_FROM_WIN32(win32err);
	}

	inline sws_error_t sws_error_GetFromGdiplusStatus(int err)
	{
		return err;
	}
#ifdef __cplusplus
}
#endif

// sws_IconPainter.h
typedef struct _sws_IconPainter_CallbackParams
{
	long long timestamp;
	HWND hWnd;
	int index;
	BOOL bIsDesktop;
	BOOL bUseApplicationIcon;
} sws_IconPainter_CallbackParams;

// sws_tshwnd.h
typedef struct _sws_tshwnd
{
	HWND hWnd;
	FILETIME ft;
	BOOL bFlash;
	double cbFlashAnimationState;
	DWORD dwFlashAnimationState;
} sws_tshwnd;

// sws_utility.h
inline long long sws_milliseconds_now() {
    LARGE_INTEGER s_frequency;
    BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
    if (s_use_qpc) {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (1000LL * now.QuadPart) / s_frequency.QuadPart;
    }
    else {
        return GetTickCount();
    }
}

// https://stackoverflow.com/questions/13397571/precise-thread-sleep-needed-max-1ms-error
inline BOOLEAN sws_nanosleep(LONGLONG ns) {
    /* Declarations */
    HANDLE timer;   /* Timer handle */
    LARGE_INTEGER li;   /* Time defintion */
    /* Create timer */
    if (!(timer = CreateWaitableTimer(NULL, TRUE, NULL)))
        return FALSE;
    /* Set timer properties */
    li.QuadPart = -ns;
    if (!SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE)) {
        CloseHandle(timer);
        return FALSE;
    }
    /* Start & wait for timer */
    WaitForSingleObject(timer, INFINITE);
    /* Clean resources */
    CloseHandle(timer);
    /* Slept without problems */
    return TRUE;
}

// sws_vector.h
typedef struct _sws_vector
{
	void* pList;
	int cbSize;
	int cbCapacity;
	int cbElementSize;
} sws_vector;

// sws_window.h
typedef struct _sws_window
{
	HWND hWnd;
	DWORD dwProcessId;
	wchar_t wszPath[MAX_PATH];
	BOOL bIsApplicationFrameHost;
	sws_tshwnd* tshWnd;
	wchar_t* wszAUMID;
	struct _sws_window* pNextWindow;
} sws_window;

// sws_WindowHelpers.h
extern ULONG_PTR _sws_gdiplus_token;

// References:
// RealEnumWindows: https://stackoverflow.com/questions/38205375/enumwindows-function-in-win10-enumerates-only-desktop-apps
// IsAltTabWindow: https://devblogs.microsoft.com/oldnewthing/20071008-00/?p=24863
// GetIconFromHWND: https://github.com/cairoshell/ManagedShell/blob/master/src/ManagedShell.WindowsTasks/ApplicationWindow.cs

// bcc18b79-ba16-442f-80c4-8a59c30c463b
DEFINE_GUID(__uuidof_IShellItemImageFactory,
	0xbcc18b79,
	0xba16, 0x442f, 0x80, 0xc4,
	0x8a, 0x59, 0xc3, 0x0c, 0x46, 0x3b
);

DEFINE_GUID(__uuidof_IShellItem2,
	0x7e9fb0d3,
	0x919f, 0x4307, 0xab, 0x2e,
	0x9b, 0x18, 0x60, 0x31, 0x0c, 0x93
);

DEFINE_GUID(__uuidof_IPropertyStore,
	0x886D8EEB,
	0x8CF2, 0x4446, 0x8D, 0x02,
	0xCD, 0xBA, 0x1D, 0xBD, 0xCF, 0x99
);

DEFINE_GUID(__uuidof_AppUserModelIdProperty,
	0x9F4C2855,
	0x9F79, 0x4B39, 0xA8, 0xD0,
	0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3
);

// https://gist.github.com/m417z/451dfc2dad88d7ba88ed1814779a26b4

// {c8900b66-a973-584b-8cae-355b7f55341b}
DEFINE_GUID(CLSID_StartMenuCacheAndAppResolver, 0x660b90c8, 0x73a9, 0x4b58, 0x8c, 0xae, 0x35, 0x5b, 0x7f, 0x55, 0x34, 0x1b);

// {46a6eeff-908e-4dc6-92a6-64be9177b41c}
DEFINE_GUID(IID_IAppResolver_7, 0x46a6eeff, 0x908e, 0x4dc6, 0x92, 0xa6, 0x64, 0xbe, 0x91, 0x77, 0xb4, 0x1c);

// {de25675a-72de-44b4-9373-05170450c140}
DEFINE_GUID(IID_IAppResolver_8, 0xde25675a, 0x72de, 0x44b4, 0x93, 0x73, 0x05, 0x17, 0x04, 0x50, 0xc1, 0x40);

typedef interface IAppResolver_8 IAppResolver_8;

typedef struct IAppResolver_8Vtbl
{
	BEGIN_INTERFACE

	HRESULT(STDMETHODCALLTYPE* QueryInterface)(
		IAppResolver_8* This,
		/* [in] */ REFIID riid,
		/* [annotation][iid_is][out] */
		_COM_Outptr_  void** ppvObject);

	ULONG(STDMETHODCALLTYPE* AddRef)(
		IAppResolver_8* This);

	ULONG(STDMETHODCALLTYPE* Release)(
		IAppResolver_8* This);

	HRESULT (STDMETHODCALLTYPE* GetAppIDForShortcut)(IAppResolver_8* This);
	HRESULT (STDMETHODCALLTYPE* GetAppIDForShortcutObject)(IAppResolver_8* This);
	HRESULT (STDMETHODCALLTYPE* GetAppIDForWindow)(IAppResolver_8* This, HWND hWnd, WCHAR** pszAppId, int* pUnknown1, int* pUnknown2, int* pUnknown3);
	HRESULT (STDMETHODCALLTYPE* GetAppIDForProcess)(IAppResolver_8* This, DWORD dwProcessId, WCHAR** pszAppId, int* pUnknown1, int* pUnknown2, int* pUnknown3);

	END_INTERFACE
} IAppResolver_8Vtbl;

interface IAppResolver_8
{
	CONST_VTBL struct IAppResolver_8Vtbl* lpVtbl;
};

#define STATUS_BUFFER_TOO_SMALL 0xC0000023
typedef NTSTATUS(WINAPI* NtUserBuildHwndList)
(
	HDESK in_hDesk,
	HWND  in_hWndNext,
	BOOL  in_EnumChildren,
	BOOL  in_RemoveImmersive,
	DWORD in_ThreadID,
	UINT  in_Max,
	HWND* out_List,
	UINT* out_Cnt
	);
extern HMODULE _sws_hWin32u;
extern NtUserBuildHwndList _sws_pNtUserBuildHwndList;

typedef struct
{
	int nAccentState;
	int nFlags;
	int nColor;
	int nAnimationId;
} ACCENTPOLICY;
typedef struct
{
	int nAttribute;
	PVOID pData;
	ULONG ulDataSize;
} WINCOMPATTRDATA;
extern HINSTANCE _sws_hUser32;
typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);
extern pSetWindowCompositionAttribute _sws_SetWindowCompositionAttribute;
typedef BOOL(WINAPI* pIsShellManagedWindow)(HWND);
extern pIsShellManagedWindow _sws_IsShellManagedWindow;
extern pIsShellManagedWindow sws_IsShellFrameWindow;
typedef HWND(WINAPI* pHungWindowFromGhostWindow)(HWND);
extern pHungWindowFromGhostWindow _sws_HungWindowFromGhostWindow;
typedef HWND(WINAPI* pGhostWindowFromHungWindow)(HWND);
extern pGhostWindowFromHungWindow _sws_GhostWindowFromHungWindow;
typedef HICON(WINAPI* pInternalGetWindowIcon)(HWND,UINT);
extern pInternalGetWindowIcon _sws_InternalGetWindowIcon;
typedef HWND(WINAPI* pIsCoreWindow)(HWND);
extern pIsCoreWindow _sws_IsCoreWindow;
typedef BOOL(WINAPI* pSHWindowsPolicy)(REFGUID riid);
extern pSHWindowsPolicy sws_SHWindowsPolicy;

extern "C" WINUSERAPI BOOL WINAPI EndTask(HWND, BOOL, BOOL);

typedef HWND(WINAPI* pCreateWindowInBand)(
	_In_ DWORD dwExStyle,
	_In_opt_ LPCWSTR lpClassName,
	_In_opt_ LPCWSTR lpWindowName,
	_In_ DWORD dwStyle,
	_In_ int X,
	_In_ int Y,
	_In_ int nWidth,
	_In_ int nHeight,
	_In_opt_ HWND hWndParent,
	_In_opt_ HMENU hMenu,
	_In_opt_ HINSTANCE hInstance,
	_In_opt_ LPVOID lpParam,
	DWORD band
	);
extern pCreateWindowInBand _sws_CreateWindowInBand;

typedef BOOL(WINAPI* pSetWindowBand)(HWND hWnd, HWND hwndInsertAfter, DWORD dwBand);
extern pSetWindowBand _sws_SetWindowBand;

typedef BOOL(WINAPI* pGetWindowBand)(HWND hWnd, PDWORD pdwBand);
extern pGetWindowBand _sws_GetWindowBand;

typedef BOOL(WINAPI* pIsTopLevelWindow)(HWND);
extern pIsTopLevelWindow _sws_IsTopLevelWindow;

FILETIME sws_start_ft;
FILETIME sws_ancient_ft;

HICON sws_DefAppIcon;
HICON sws_LegacyDefAppIcon;

IAppResolver_8* sws_AppResolver;

inline FILETIME sws_WindowHelpers_GetStartTime()
{
	return sws_start_ft;
}

inline FILETIME sws_WindowHelpers_GetAncientTime()
{
	ULARGE_INTEGER uli;
	uli.LowPart = sws_ancient_ft.dwLowDateTime;
	uli.HighPart = sws_ancient_ft.dwHighDateTime;
	uli.QuadPart--;
	sws_ancient_ft.dwHighDateTime = uli.HighPart;
	sws_ancient_ft.dwLowDateTime = uli.LowPart;
	return sws_ancient_ft;
}

enum ZBID
{
	ZBID_DEFAULT = 0,
	ZBID_DESKTOP = 1,
	ZBID_UIACCESS = 2,
	ZBID_IMMERSIVE_IHM = 3,
	ZBID_IMMERSIVE_NOTIFICATION = 4,
	ZBID_IMMERSIVE_APPCHROME = 5,
	ZBID_IMMERSIVE_MOGO = 6,
	ZBID_IMMERSIVE_EDGY = 7,
	ZBID_IMMERSIVE_INACTIVEMOBODY = 8,
	ZBID_IMMERSIVE_INACTIVEDOCK = 9,
	ZBID_IMMERSIVE_ACTIVEMOBODY = 10,
	ZBID_IMMERSIVE_ACTIVEDOCK = 11,
	ZBID_IMMERSIVE_BACKGROUND = 12,
	ZBID_IMMERSIVE_SEARCH = 13,
	ZBID_GENUINE_WINDOWS = 14,
	ZBID_IMMERSIVE_RESTRICTED = 15,
	ZBID_SYSTEM_TOOLS = 16,
	ZBID_LOCK = 17,
	ZBID_ABOVELOCK_UX = 18,
};

/********************************************************/
/* enumerate all top level windows including metro apps */
/********************************************************/
sws_error_t sws_WindowHelpers_RealEnumWindows(
	WNDENUMPROC in_Proc,
	LPARAM in_Param
);

wchar_t* sws_WindowHelpers_GetAUMIDForHWND(HWND hWnd);

extern BOOL g_bIsDesktopRaised;
inline BOOL _sws_WindowHelpers_IsDesktopRaised()
{
	return g_bIsDesktopRaised;
}

inline void _sws_WindowHelpers_ToggleDesktop()
{
	keybd_event(VK_LMENU, 0, KEYEVENTF_KEYUP, 0); // ensure alt is up
	keybd_event(VK_LWIN, 0, 0, 0);
	keybd_event('D', 0, 0, 0);
	keybd_event('D', 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
}

inline BOOL sws_WindowHelpers_IsWindowUWP(HWND hWnd)
{
	return sws_IsShellFrameWindow(hWnd);
}

// sws_WindowSwitcherLayout.h
typedef struct _sws_WindowSwitcherLayout
{
	HMONITOR hMonitor;
	HWND hWnd;

	sws_vector pWindowList;
	int iX;
	int iY;
	unsigned int iWidth;
	unsigned int iHeight;
	unsigned int cbDpiX;
	unsigned int cbDpiY;
	int iIndex;
	int iFirstItemIndex;
	MONITORINFO mi;
	unsigned int numTopMost;
	BOOL bIncludeWallpaper;
	BOOL bWallpaperAlwaysLast;
	BOOL bWallpaperToggleBehavior;
	HWND hWndWallpaper;
	HFONT hFontRegular;
	unsigned int cbFontHeight;
	unsigned int cbBorderSize;
	long long timestamp;
} sws_WindowSwitcherLayout;

// sws_WindowSwitcher.h
typedef struct _sws_WindowSwitcher
{
    BOOL bIsDynamic;
    HRESULT hrCo;
    HRESULT hrRo;
    HMODULE hinstDLL;
    HWND hWnd;
    UINT msgShellHook;
    sws_WindowSwitcherLayout layout;
    int direction;
	int scrollDirection;
    int lastKey;
    HBRUSH hBackgroundBrush;
    BOOL bPartialRedraw;
    ITaskbarList* pTaskList;
    HWND hWndLast;
    BOOL bWasControl;
    HMONITOR hMonitor;
    INT cwIndex;
    DWORD cwMask;
    HANDLE hEvExit;
    BOOL bIsMouseClicking;
    long long last_change;
    sws_vector pHWNDList;
    HDPA htshwnds;
    BOOL bWallpaperAlwaysLast;
    HWND hWndWallpaper;
    UINT mode;
    HWND lastMiniModehWnd;
    HWINEVENTHOOK global_hook;
    DWORD dwShowDelay;
    BOOL bPrimaryOnly;
    UINT vkTilde;
    UINT dwOverrideMode;
    UINT opacity;
    HANDLE hShowThread;
    HANDLE hShowSignal;
    BOOL bIsInitialized;
    HWND hWndAccessible;
    IAccPropServices* pAccPropServices;
    HBRUSH hFlashBrush;
    DWORD dwPaintFlags;
    HANDLE hFlashAnimationThread;
    HANDLE hFlashAnimationSignal;
    HDC hdcWindow;
    HDC hdcPaint;
    HPAINTBUFFER hBufferedPaint;
    INT cwOldIndex;
    DWORD cwOldMask;
    HDPA hLastClosedWnds;
    long long lastUpdateTime;
    BOOL bShouldStartFlashTimerWhenShowing;
    DWORD dwOriginalMouseRouting;
    DWORD dwOriginalScrollWheelBehavior;
	BOOL bIsCursorOnSwitcher;
	BOOL bSkipIfOneWindow;

    DWORD bIncludeWallpaper;
    DWORD bNoPerApplicationList;
	DWORD dwWallpaperSupport;
	DWORD bSwitcherIsPerApplication;
    DWORD bAlwaysUseWindowTitleAndIcon;
    DWORD dwScrollWheelBehavior;
    DWORD bScrollWheelInvert;
	DWORD dwGridColumns;
    DWORD dwGridRows;
} sws_WindowSwitcher;

typedef struct _sws_WindowSwitcher_EndTaskThreadParams
{
    HWND hWnd;
    sws_WindowSwitcher* sws;
    HDESK hDesktop;
} sws_WindowSwitcher_EndTaskThreadParams;

void sws_WindowSwitcher_LoadSettings(sws_WindowSwitcher* _this);

// sws_WindowSwitcherLayoutWindow.h
typedef struct _sws_WindowSwitcherLayoutWindow
{
    HWND hWnd;
    SIZE sizWindow;
    UINT gridX;
	UINT gridY;
    RECT rcWindow;
    int iRowMax;
    HICON hIcon;
    UINT dwIconSource;
    UINT szIcon;
    RECT rcIcon;
    WCHAR wszPath[MAX_PATH];
    sws_tshwnd* tshWnd;
    sws_tshwnd* last_flashing_tshwnd;
    DWORD dwCount;
    DWORD dwWindowFlags;
    WCHAR* wszAUMID;
    HDPA dpaGroupedWnds;
} sws_WindowSwitcherLayoutWindow;

// sws_error.c
wchar_t* sws_error_NumToDescription(sws_error_t errnum, BOOL* bType)
{
    if (bType) *bType = FALSE;
    wchar_t* ret = NULL;
    switch (errnum)
    {
    case SWS_ERROR_SUCCESS:
        ret = (wchar_t*)SWS_ERROR_SUCCESS_TEXT;
        break;
    case SWS_ERROR_NO_MEMORY:
        ret = (wchar_t*)SWS_ERROR_NO_MEMORY_TEXT;
        break;
    case SWS_ERROR_NOT_INITIALIZED:
        ret = (wchar_t*)SWS_ERROR_NOT_INITIALIZED_TEXT;
        break;
    case SWS_ERROR_LOADLIBRARY_FAILED:
        ret = (wchar_t*)SWS_ERROR_LOADLIBRARY_FAILED_TEXT;
        break;
    case SWS_ERROR_FUNCTION_NOT_FOUND:
        ret = (wchar_t*)SWS_ERROR_FUNCTION_NOT_FOUND_TEXT;
        break;
    case SWS_ERROR_INVALID_PARAMETER:
        ret = (wchar_t*)SWS_ERROR_INVALID_PARAMETER_TEXT;
        break;
    case SWS_ERROR_APPRESOLVER_NOT_AVAILABLE:
        ret = (wchar_t*)SWS_ERROR_APPRESOLVER_NOT_AVAILABLE_TEXT;
        break;
    default:
        if (bType) *bType = TRUE;
        FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS |
            FORMAT_MESSAGE_MAX_WIDTH_MASK,
            NULL,
            errnum,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR)(&ret),
            0,
            NULL
        );
        break;
    }
    return ret;
}

sws_error_t sws_error_Report(sws_error_t errnum, void* data)
{
    if (errnum == SWS_ERROR_SUCCESS)
    {
        return errnum;
    }
    wchar_t* errdesc = NULL;
    _lock_file(stdout);
    Wh_Log(L"====================================\nAn error occured in the application.\nError number: 0x%x\n", errnum);
    BOOL bShouldFree = FALSE;
    if ((errdesc = sws_error_NumToDescription(errnum, &bShouldFree)))
    {
        Wh_Log(L"Description: %s\n", errdesc);
        if (bShouldFree) LocalFree(errdesc);
    }
    else
    {
        Wh_Log(L".");
    }
    if (data)
    {
        Wh_Log(L"Additional data: 0x%p\n", data);
    }
    Wh_Log(L"====================================");
    _unlock_file(stdout);
    return errnum;
}

// sws_IconPainter.c
void sws_IconPainter_DrawIcon(HICON hIcon, HDC hDC, HBRUSH hBrush, Gdiplus::GpGraphics* pGdipGraphics, INT x, INT y, INT w, INT h, RGBQUAD bkcol, BOOL bShouldFillBackground)
{
    if (hIcon == NULL || hDC == NULL || w == 0 || h == 0)
    {
        return;
    }

    BITMAPINFO bi;
    ZeroMemory(&bi, sizeof(BITMAPINFO));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = 1;
    bi.bmiHeader.biHeight = 1;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    if (bShouldFillBackground)
    {
        StretchDIBits(hDC, x, y, w, h, 0, 0, 1, 1, &bkcol, &bi, DIB_RGB_COLORS, SRCCOPY);
    }

    // Not using GdipCreateBitmapFromHICON directly because some bug in GDI+
    // renders weird black lines in some transparent areas; this is the only
    // way I could properly get this to work
    // from: https://stackoverflow.com/questions/11338009/how-do-i-copy-an-hicon-from-gdi-to-gdi-with-transparency
    if (pGdipGraphics)
    {
        ICONINFO ii;
        if (GetIconInfo(hIcon, &ii))
        {
            Gdiplus::GpBitmap* pGdipBitmap = NULL;
            Gdiplus::DllExports::GdipCreateBitmapFromHBITMAP(
                (HBITMAP)ii.hbmColor,
                (HPALETTE)NULL,
                &pGdipBitmap
            );
            if (pGdipBitmap)
            {
                INT rct[4] = { 0, 0, 0, 0 };
                Gdiplus::DllExports::GdipGetImageWidth(pGdipBitmap, (UINT*)(rct + 2));
                Gdiplus::DllExports::GdipGetImageHeight(pGdipBitmap, (UINT*)(rct + 3));
                INT PixelFormat = 0;
                Gdiplus::DllExports::GdipGetImagePixelFormat(pGdipBitmap, &PixelFormat);
                Gdiplus::BitmapData LockedBitmapData = {};
                Gdiplus::DllExports::GdipBitmapLockBits(
                    pGdipBitmap,
                    (const Gdiplus::GpRect *)rct,
                    Gdiplus::ImageLockModeRead,
                    (INT)PixelFormat,
                    &LockedBitmapData
                );
                if (LockedBitmapData.Scan0)
                {
                    Gdiplus::GpBitmap* pGdipBitmap2 = NULL;
                    Gdiplus::DllExports::GdipCreateBitmapFromScan0(
                        LockedBitmapData.Width,
                        LockedBitmapData.Height,
                        LockedBitmapData.Stride,
                        PixelFormat32bppARGB,
                        (BYTE*)LockedBitmapData.Scan0,
                        &pGdipBitmap2
                    );
                    if (pGdipBitmap2)
                    {
                        Gdiplus::DllExports::GdipDrawImageRectI(
                            pGdipGraphics,
                            pGdipBitmap2,
                            (INT)x,
                            (INT)y,
                            (INT)w,
                            (INT)h
                        );
                        Gdiplus::DllExports::GdipDisposeImage(pGdipBitmap2);
                        // We proceed to check if this worked by verifying if the
                        // written bitmap is all transparent; if it is, then it
                        // means nothing was drawn, so we fallback to creating the
                        // bitmap using GdipCreateBitmapFromHICON and drawing that 
                        HDC hDC_Test = NULL;
                        HBITMAP hBM_Test = NULL;
                        HBITMAP hBM_Test_Old = NULL;
                        if ((hDC_Test = CreateCompatibleDC(hDC)))
                        {
                            if ((hBM_Test = CreateCompatibleBitmap(hDC, w, h)))
                            {
                                if ((hBM_Test_Old = (HBITMAP)SelectObject(hDC_Test, hBM_Test)))
                                {
                                    if (BitBlt(hDC_Test, 0, 0, w, h, hDC, x, y, SRCCOPY))
                                    {
                                        BITMAPINFO info;
                                        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                                        info.bmiHeader.biWidth = w;
                                        info.bmiHeader.biHeight = -h;
                                        info.bmiHeader.biPlanes = 1;
                                        info.bmiHeader.biBitCount = 32;
                                        info.bmiHeader.biCompression = BI_RGB;
                                        info.bmiHeader.biSizeImage = w * h * 4;
                                        info.bmiHeader.biXPelsPerMeter = 0;
                                        info.bmiHeader.biYPelsPerMeter = 0;
                                        info.bmiHeader.biClrUsed = 0;
                                        info.bmiHeader.biClrImportant = 0;
                                        void* bits = malloc(info.bmiHeader.biSizeImage);
                                        if (bits)
                                        {
                                            if (GetDIBits(hDC_Test, hBM_Test, 0, h, bits, &info, DIB_RGB_COLORS))
                                            {
                                                BYTE* ptr;
                                                int ii;
                                                BOOL bUsed = FALSE;
                                                for (ii = 0, ptr = (BYTE*)bits; ii < w * h; ii++, ptr += 4)
                                                {
                                                    if (ptr[3] != bkcol.rgbReserved)
                                                    {
                                                        bUsed = TRUE;
                                                        break;
                                                    }
                                                }
                                                if (!bUsed)
                                                {
                                                    if (bShouldFillBackground)
                                                    {
                                                        StretchDIBits(hDC, x, y, w, h, 0, 0, 1, 1, &bkcol, &bi, DIB_RGB_COLORS, SRCCOPY);
                                                    }
                                                    Gdiplus::GpBitmap* pGdipBitmap3 = NULL;
                                                    Gdiplus::DllExports::GdipCreateBitmapFromHICON(
                                                        (HICON)hIcon,
                                                        &pGdipBitmap3
                                                    );
                                                    if (pGdipBitmap3)
                                                    {
                                                        Gdiplus::DllExports::GdipDrawImageRectI(
                                                            pGdipGraphics,
                                                            pGdipBitmap3,
                                                            (INT)x,
                                                            (INT)y,
                                                            (INT)w,
                                                            (INT)h
                                                        );
                                                        Gdiplus::DllExports::GdipDisposeImage(pGdipBitmap3);
                                                    }
                                                }
                                            }
                                            free(bits);
                                        }
                                    }
                                    SelectObject(hDC_Test, hBM_Test_Old);
                                }
                                DeleteObject(hBM_Test);
                            }
                            DeleteDC(hDC_Test);
                        }
                    }
                    Gdiplus::DllExports::GdipBitmapUnlockBits(
                        pGdipBitmap,
                        &LockedBitmapData
                    );
                }
                Gdiplus::DllExports::GdipDisposeImage(pGdipBitmap);
            }
            DeleteObject(ii.hbmColor);
            DeleteObject(ii.hbmMask);
        }
    }
    else
    {
        // Fallback to crappier drawing using GDI if GDI+ is unavailable
        if (bShouldFillBackground)
        {
            DrawIconEx(hDC, x, y, hIcon, w, h, 0, hBrush, DI_NORMAL);
        }
        else
        {
            DrawIcon(hDC, x, y, hIcon);
        }
    }
}

static void __stdcall _sws_IconPainter_Callback(
	HWND hWnd,
	UINT uMsg,
	sws_IconPainter_CallbackParams* params,
	HICON hIcon
)
{
    LONG_PTR ptr = GetWindowLongPtr(params->hWnd, GWLP_USERDATA);
    sws_WindowSwitcher* _this = (sws_WindowSwitcher*)(ptr);

	if (_this->layout.timestamp == params->timestamp)
	{
        DWORD dwProcessId;
        GetWindowThreadProcessId(hWnd, &dwProcessId);
        sws_WindowSwitcherLayoutWindow* pWindowList = (sws_WindowSwitcherLayoutWindow*)_this->layout.pWindowList.pList;

        if (!hIcon || (params->bUseApplicationIcon && !pWindowList[params->index].dwIconSource))
        {
            if (params->bUseApplicationIcon && !pWindowList[params->index].dwIconSource)
            {
                if (hIcon && dwProcessId != GetCurrentProcessId())
                {
                    DestroyIcon(hIcon);
                }
                pWindowList[params->index].dwIconSource = 1;
            }
            switch (pWindowList[params->index].dwIconSource)
            {
            case 0:
            {
                pWindowList[params->index].dwIconSource++;
                if (SendMessageCallbackW(hWnd, WM_GETICON, ICON_SMALL2, 0, (SENDASYNCPROC)_sws_IconPainter_Callback, (ULONG_PTR)params))
                {
                    return;
                }
                else
                {
                    pWindowList[params->index].hIcon = sws_LegacyDefAppIcon;
                }
                break;
            }
            case 1:
            {
                pWindowList[params->index].dwIconSource++;
                wchar_t wszPath[MAX_PATH];
                ZeroMemory(wszPath, MAX_PATH * sizeof(wchar_t));
                if (!params->bIsDesktop)
                {
                    if (!sws_WindowHelpers_IsWindowUWP(hWnd))
                    {
                        HANDLE hProcess;
                        hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
                        if (hProcess)
                        {
                            GetModuleFileNameExW((HMODULE)hProcess, NULL, wszPath, MAX_PATH);
                            CharLowerW(wszPath);
                            SHFILEINFOW shinfo;
                            ZeroMemory(&shinfo, sizeof(SHFILEINFOW));
                            SHGetFileInfoW(
                                wszPath,
                                FILE_ATTRIBUTE_NORMAL,
                                &shinfo,
                                sizeof(SHFILEINFOW),
                                SHGFI_ICON
                            );
                            if (shinfo.hIcon)
                            {
                                _sws_IconPainter_Callback(hWnd, uMsg, params, shinfo.hIcon);
                                return;
                            }
                        }
                    }
                    else
                    {
                        HRESULT hr = S_OK;
                        IShellItemImageFactory* imageFactory = NULL;
                        SIIGBF flags = SIIGBF_RESIZETOFIT | SIIGBF_ICONBACKGROUND;

                        IPropertyStore* propStore = NULL;
                        hr = SHGetPropertyStoreForWindow(
                            hWnd,
                            __uuidof_IPropertyStore,
                            (void**)&propStore
                        );
                        if (SUCCEEDED(hr))
                        {
                            PROPERTYKEY pKey;
                            pKey.fmtid = __uuidof_AppUserModelIdProperty;
                            pKey.pid = 5;
                            PROPVARIANT prop;
                            ZeroMemory(&prop, sizeof(PROPVARIANT));
                            if (SUCCEEDED(propStore->GetValue(pKey, &prop))) {
                                if (prop.bstrVal)
                                {
                                    SHCreateItemInKnownFolder(
                                        FOLDERID_AppsFolder,
                                        KF_FLAG_DONT_VERIFY,
                                        prop.bstrVal,
                                        __uuidof_IShellItemImageFactory,
                                        (void**)&imageFactory
                                    );
                                    if (imageFactory)
                                    {
                                        double factor = SWS_UWP_ICON_SCALE_FACTOR;
                                        int szIcon = pWindowList[params->index].szIcon / factor;

                                        SIZE size;
                                        size.cx = szIcon;
                                        size.cy = szIcon;
                                        HBITMAP hBitmap;
                                        hr = imageFactory->GetImage(
                                            size,
                                            flags,
                                            &hBitmap
                                        );
                                        if (SUCCEEDED(hr))
                                        {
                                            // Easiest way to get an HICON from an HBITMAP
                                            // I have turned the Internet upside down and was unable to find this
                                            // Only a convoluted example using GDI+
                                            // This is from the disassembly of StartIsBack/StartAllBack
                                            HIMAGELIST hImageList = ImageList_Create(size.cx, size.cy, ILC_COLOR32, 1, 0);
                                            if (ImageList_Add(hImageList, hBitmap, NULL) != -1)
                                            {
                                                HICON hExIcon = ImageList_GetIcon(hImageList, 0, 0);
                                                ImageList_Destroy(hImageList);

                                                DeleteObject(hBitmap);
                                                imageFactory->Release();

                                                pWindowList[params->index].dwWindowFlags |= SWS_WINDOWSWITCHERLAYOUT_WINDOWFLAGS_ISUWP;
                                                szIcon = pWindowList[params->index].rcIcon.right;
                                                szIcon = szIcon * factor;
                                                szIcon = pWindowList[params->index].rcIcon.right - szIcon;
                                                pWindowList[params->index].rcIcon.left = szIcon / 2;
                                                pWindowList[params->index].rcIcon.top = szIcon / 2;
                                                pWindowList[params->index].rcIcon.right = pWindowList[params->index].rcIcon.right + szIcon;
                                                pWindowList[params->index].rcIcon.bottom = pWindowList[params->index].rcIcon.bottom + szIcon;

                                                _sws_IconPainter_Callback(hWnd, uMsg, params, hExIcon);
                                                return;
                                            }
                                            DeleteObject(hBitmap);
                                        }
                                        imageFactory->Release();
                                    }
                                }
                                PropVariantClear(&prop);
                            }
                            propStore->Release();
                        }
                    }
                }
                else
                {
                    if (GetSystemDirectoryW(wszPath, MAX_PATH))
                    {
                        wcscat_s(wszPath, MAX_PATH, L"\\imageres.dll");
                        HICON hExIcon = ExtractIconW(
                            GetModuleHandleW(NULL),
                            wszPath,
                            -110
                        );
                        if (hExIcon)
                        {
                            _sws_IconPainter_Callback(hWnd, uMsg, params, hExIcon);
                            return;
                        }
                    }
                }
                pWindowList[params->index].hIcon = sws_LegacyDefAppIcon;
            }
            }
        }
        else
        {
            if (pWindowList[params->index].hIcon && sws_DefAppIcon && pWindowList[params->index].hIcon != sws_DefAppIcon && sws_LegacyDefAppIcon && pWindowList[params->index].hIcon != sws_LegacyDefAppIcon)
            {
                DestroyIcon(pWindowList[params->index].hIcon);
                pWindowList[params->index].hIcon = NULL;
            }
            if (dwProcessId == GetCurrentProcessId() && pWindowList[params->index].dwIconSource <= 1)
            {
                pWindowList[params->index].hIcon = CopyIcon(hIcon);
            }
            else
            {
                pWindowList[params->index].hIcon = hIcon;
            }
        }

        BOOL bShouldPaint = TRUE;
        for (int i = 0; i < _this->layout.pWindowList.cbSize; ++i)
        {
            if (pWindowList[i].hIcon == sws_DefAppIcon && !IsHungAppWindow(pWindowList[i].hWnd))
            {
                bShouldPaint = FALSE;
                break;
            }
        }
        if (bShouldPaint)
        {
            //Wh_Log(L"[sws] Asynchronously obtained application icons in %lld ms.\n", sws_milliseconds_now() - _this->layout.timestamp);
            KillTimer(_this->hWnd, SWS_WINDOWSWITCHER_TIMER_PAINT);
            SendMessageW(_this->hWnd, SWS_WINDOWSWITCHER_PAINT_MSG, SWS_WINDOWSWITCHER_PAINTFLAGS_REDRAWENTIRE, 0);
        }
	}

    free(params);
}

BOOL sws_IconPainter_ExtractAndDrawIconAsync(HWND hWnd, sws_IconPainter_CallbackParams* params)
{
    if (IsHungAppWindow(hWnd))
    {
        return FALSE;
    }
    SetTimer(params->hWnd, SWS_WINDOWSWITCHER_TIMER_PAINT, SWS_WINDOWSWITCHER_TIMER_PAINT_GETICONASYNC_DELAY, NULL);
	return SendMessageCallbackW(hWnd, WM_GETICON, ICON_BIG, 0, (SENDASYNCPROC)_sws_IconPainter_Callback, (ULONG_PTR)params);
}

// sws_tshwnd.c
void sws_tshwnd_ModifyTimestamp(sws_tshwnd* _this, FILETIME ft)
{
	_this->ft = ft;
}

void sws_tshwnd_UpdateTimestamp(sws_tshwnd* _this)
{
	GetSystemTimeAsFileTime(&(_this->ft));
}

int CALLBACK sws_tshwnd_CompareTimestamp(sws_tshwnd* p1, sws_tshwnd* p2, LPARAM flags)
{
	if (flags & SWS_SORT_DESCENDING)
	{
		return CompareFileTime(&(p2->ft), &(p1->ft));
	}
	return CompareFileTime(&(p1->ft), &(p2->ft));
}

int CALLBACK sws_tshwnd_CompareHWND(sws_tshwnd* p1, sws_tshwnd* p2, LPARAM)
{
	return !(p1 && p2 && p1->hWnd == p2->hWnd);
}

BOOL sws_tshwnd_GetFlashState(sws_tshwnd* _this)
{
	return _this->bFlash;
}

void sws_tshwnd_SetFlashState(sws_tshwnd* _this, BOOL bFlash)
{
	_this->bFlash = bFlash;
}

sws_error_t sws_tshwnd_Initialize(sws_tshwnd* _this, HWND hWnd)
{
	sws_error_t rv = SWS_ERROR_SUCCESS;

	if (!rv && _this)
	{
		_this->hWnd = hWnd;
		GetSystemTimeAsFileTime(&(_this->ft));
		_this->bFlash = FALSE;
		_this->cbFlashAnimationState = 0;
		_this->dwFlashAnimationState = 0;
	}

	return rv;
}

// sws_vector.c
sws_error_t sws_vector_PushBack(sws_vector* _this, void* pElement)
{
	sws_error_t rv = SWS_ERROR_SUCCESS;

	if (!rv)
	{
		if (_this->cbCapacity == 0 || _this->cbElementSize == 0)
		{
			rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_NO_MEMORY), NULL);
		}
	}
	if (!rv)
	{
		if (_this->cbSize >= _this->cbCapacity)
		{
			void* prev = _this->pList;
			_this->pList = realloc(_this->pList, (uintptr_t)_this->cbElementSize * (uintptr_t)((uintptr_t)_this->cbCapacity + SWS_VECTOR_CAPACITY));
			if (!_this->pList)
			{
				free(prev);
				_this->cbElementSize = 0;
				_this->cbCapacity = 0;
				_this->cbSize = 0;
				rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_NO_MEMORY), NULL);
			}
			else
			{
				_this->cbCapacity = _this->cbCapacity + SWS_VECTOR_CAPACITY;
			}
		}
	}
	if (!rv)
	{
		memcpy((void*)((uintptr_t)_this->pList + (uintptr_t)_this->cbSize * (uintptr_t)_this->cbElementSize), pElement, _this->cbElementSize);
		_this->cbSize++;
	}

	return rv;
}

void sws_vector_Clear(sws_vector* _this)
{
	if (_this)
	{
		free(_this->pList);
		memset(_this, 0, sizeof(sws_vector));
	}
}

sws_error_t sws_vector_Initialize(sws_vector* _this, unsigned int cbElementSize)
{
	sws_error_t rv = SWS_ERROR_SUCCESS;

	if (!rv)
	{
		if (!_this)
		{
			rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_NO_MEMORY), NULL);
		}
	}
	if (!rv)
	{
		_this->pList = calloc(SWS_VECTOR_CAPACITY, cbElementSize);
		if (!_this->pList)
		{
			rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_NO_MEMORY), NULL);
		}
		_this->cbElementSize = cbElementSize;
		_this->cbCapacity = SWS_VECTOR_CAPACITY;
		_this->cbSize = 0;
	}

	return rv;
}

// sws_window.c
extern NTSTATUS NTAPI NtQueryInformationProcess(
	_In_ HANDLE ProcessHandle,
	_In_ DWORD ProcessInformationClass,
	_Out_writes_bytes_(ProcessInformationLength) PVOID ProcessInformation,
	_In_ ULONG ProcessInformationLength,
	_Out_opt_ PULONG ReturnLength);
#define ProcessConsoleHostProcess 49

sws_error_t sws_window_Initialize(sws_window* _this, HWND hWnd)
{
	sws_error_t rv = SWS_ERROR_SUCCESS;

	if (!rv)
	{
		ZeroMemory(_this->wszPath, MAX_PATH);
		_this->hWnd = hWnd;
	}
	if (!rv)
	{
		HWND hWndOfInterest = _sws_HungWindowFromGhostWindow(hWnd);
		if (!hWndOfInterest) hWndOfInterest = hWnd;
		if (!GetWindowThreadProcessId(hWndOfInterest, &(_this->dwProcessId)))
		{
			rv = sws_error_GetFromWin32Error(GetLastError());
		}
	}
	if (!rv)
	{
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, _this->dwProcessId);
		if (hProcess)
		{
			GetModuleFileNameExW(hProcess, NULL, _this->wszPath, MAX_PATH);
			//NtQueryInformationProcess(hProcess, ProcessConsoleHostProcess, &(_this->dwConhostPID), sizeof(UINT_PTR), NULL);
			//_this->dwConhostPID--;
			//Wh_Log(L"%d - %s\n", _this->dwConhostPID, _this->wszPath);
			CloseHandle(hProcess);
		}
	}
	if (!rv)
	{
		/*
		wchar_t path[MAX_PATH];
		ZeroMemory(path, MAX_PATH);
		memcpy(path, _this->wszPath, MAX_PATH);
		wchar_t syspath[MAX_PATH];
		ZeroMemory(syspath, MAX_PATH);
		GetSystemDirectoryW(syspath, MAX_PATH);
		wcscat_s(syspath, MAX_PATH, L"\\ApplicationFrameHost.exe");
		//Wh_Log(L"%s %s\n", path, syspath);
		_this->bIsApplicationFrameHost = !_wcsicmp(path, syspath);
		*/
		_this->bIsApplicationFrameHost = sws_IsShellFrameWindow(hWnd);
		//_this->bIsApplicationFrameHost = sws_WindowHelpers_IsWindowUWP(hWnd);
		_this->tshWnd = NULL;
		_this->pNextWindow = NULL;
	}
	if (!rv)
	{
		_this->wszAUMID = sws_WindowHelpers_GetAUMIDForHWND(_this->hWnd);
	}
	return rv;
}

// sws_WindowHelpers.c
ULONG_PTR _sws_gdiplus_token;
RTL_OSVERSIONINFOW sws_global_rovi;
DWORD32 sws_global_ubr;
NtUserBuildHwndList _sws_pNtUserBuildHwndList;
pCreateWindowInBand _sws_CreateWindowInBand;
pSetWindowCompositionAttribute _sws_SetWindowCompositionAttribute;
pIsShellManagedWindow _sws_IsShellManagedWindow;
pIsShellManagedWindow sws_IsShellFrameWindow;
pGetWindowBand _sws_GetWindowBand;
pSetWindowBand _sws_SetWindowBand;
pIsTopLevelWindow _sws_IsTopLevelWindow;
HMODULE _sws_hComctl32 = 0;
HMODULE _sws_hShlwapi = 0;
HMODULE _sws_hWin32u = 0;
HINSTANCE _sws_hUser32 = 0;
HINSTANCE _sws_hUxtheme = 0;
HINSTANCE _sws_hShcore = 0;
pHungWindowFromGhostWindow _sws_HungWindowFromGhostWindow;
pGhostWindowFromHungWindow _sws_GhostWindowFromHungWindow;
pInternalGetWindowIcon _sws_InternalGetWindowIcon;
pIsCoreWindow _sws_IsCoreWindow;
pSHWindowsPolicy sws_SHWindowsPolicy;

DEFINE_GUID(POLID_TurnOffSPIAnimations, 0xD7AF00A, 0xB468, 0x4A39, 0xB0, 0x16, 0x33, 0x3E, 0x22, 0x77, 0xAB, 0xED);

BOOL CALLBACK sws_WindowHelpers_IsValidMonitor(HMONITOR hMonitor, HDC unnamedParam2, LPRECT unnamedParam3, LPARAM lParam)
{
    HMONITOR* pMonitor = (HMONITOR*)lParam;
	if (!pMonitor || !*(HMONITOR*)pMonitor) return FALSE;
	if (hMonitor == *pMonitor)
	{
		*pMonitor = NULL;
		return FALSE;
	}
	return TRUE;
}

HWND* _sws_WindowHelpers_Gui_BuildWindowList
(
	NtUserBuildHwndList pNtUserBuildHwndList,
	HDESK in_hDesk,
	HWND  in_hWnd,
	BOOL  in_EnumChildren,
	BOOL  in_RemoveImmersive,
	UINT  in_ThreadID,
	INT* out_Cnt
)
{
	/* locals */
	UINT  lv_Max;
	UINT  lv_Cnt;
	UINT  lv_NtStatus;
	HWND* lv_List;

	// initial size of list
	lv_Max = 512;

	// retry to get list
	for (;;)
	{
		// allocate list
		if ((lv_List = (HWND*)malloc(lv_Max * sizeof(HWND))) == NULL)
			break;

		// call the api
		lv_NtStatus = pNtUserBuildHwndList(
			in_hDesk, in_hWnd,
			in_EnumChildren, in_RemoveImmersive, in_ThreadID,
			lv_Max, lv_List, &lv_Cnt);

		// success?
		if (lv_NtStatus == NOERROR)
			break;

		// free allocated list
		free(lv_List);

		// clear
		lv_List = NULL;

		// other error then buffersize? or no increase in size?
		if (lv_NtStatus != STATUS_BUFFER_TOO_SMALL || lv_Cnt <= lv_Max)
			break;

		// update max plus some extra to take changes in number of windows into account
		lv_Max = lv_Cnt + 16;
	}

	// return the count
	*out_Cnt = lv_Cnt;

	// return the list, or NULL when failed
	return lv_List;
}

/********************************************************/
/* enumerate all top level windows including metro apps */
/********************************************************/
sws_error_t sws_WindowHelpers_RealEnumWindows(
	WNDENUMPROC in_Proc,
	LPARAM in_Param
)
{
	if (!_sws_pNtUserBuildHwndList)
	{
		return sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_NOT_INITIALIZED), NULL);
	}

	/* locals */
	INT   lv_Cnt;
	HWND  lv_hWnd;
	BOOL  lv_Result;
	HWND  lv_hFirstWnd;
	HWND  lv_hDeskWnd;
	HWND* lv_List;

	// no error yet
	lv_Result = TRUE;

	// first try api to get full window list including immersive/metro apps
	lv_List = _sws_WindowHelpers_Gui_BuildWindowList(_sws_pNtUserBuildHwndList, 0, 0, 0, 0, 0, &lv_Cnt);

	// success?
	if (lv_List)
	{
		// loop through list
		while (lv_Cnt-- > 0 && lv_Result)
		{
			// get handle
			lv_hWnd = lv_List[lv_Cnt];

			// filter out the invalid entry (0x00000001) then call the callback
			if (IsWindow(lv_hWnd))
				lv_Result = in_Proc(lv_hWnd, in_Param);
		}

		// free the list
		free(lv_List);
	}
	else
	{
		// get desktop window, this is equivalent to specifying NULL as hwndParent
		lv_hDeskWnd = GetDesktopWindow();

		// fallback to using FindWindowEx, get first top-level window
		lv_hFirstWnd = FindWindowEx(lv_hDeskWnd, 0, 0, 0);

		// init the enumeration
		lv_Cnt = 0;
		lv_hWnd = lv_hFirstWnd;

		// loop through windows found
		// - since 2012 the EnumWindows API in windows has a problem (on purpose by MS)
		//   that it does not return all windows (no metro apps, no start menu etc)
		// - luckally the FindWindowEx() still is clean and working
		while (lv_hWnd && lv_Result)
		{
			// call the callback
			lv_Result = in_Proc(lv_hWnd, in_Param);

			// get next window
			lv_hWnd = FindWindowEx(lv_hDeskWnd, lv_hWnd, 0, 0);

			// protect against changes in window hierachy during enumeration
			if (lv_hWnd == lv_hFirstWnd || lv_Cnt++ > 10000)
				break;
		}
	}

	return SWS_ERROR_SUCCESS;
}

BOOL _sws_WindowHelpers_IsWindowNotDesktopOrTrayHelper(HWND hWnd)
{
	ATOM SecondaryTaskbarAtom = RegisterWindowMessageW(L"Shell_SecondaryTrayWnd");
	if (GetClassWord(hWnd, GCW_ATOM) == SecondaryTaskbarAtom)
	{
		return (SecondaryTaskbarAtom == 0);
	}
	return TRUE;
}

BOOL _sws_WindowHelpers_IsWindowNotDesktopOrTray(HWND hWnd, HWND hWndDesktop)
{
	if (IsWindow(hWnd) &&
		hWnd != FindWindowW(L"Shell_TrayWnd", NULL) &&
		hWnd != hWndDesktop)
	{
		return _sws_WindowHelpers_IsWindowNotDesktopOrTrayHelper(hWnd);
	}
	return FALSE;
}

BOOL _sws_TestExStyle(HWND hWnd, DWORD dwExStyle)
{
	return dwExStyle == (dwExStyle & (DWORD)GetWindowLongPtrW(hWnd, GWL_EXSTYLE));
}

BOOLEAN _sws_IsOwnerToolWindow(HWND hwnd)
{
	BOOLEAN bRet = FALSE;

	HWND hwndCurrent = hwnd;
	HWND hwndOwner = GetWindow(hwnd, GW_OWNER);
	while (!_sws_TestExStyle(hwndCurrent, WS_EX_APPWINDOW) && hwndOwner)
	{
		HWND hwndPrev = hwndCurrent;
		hwndCurrent = hwndOwner;
		hwndOwner = GetWindow(hwndOwner, GW_OWNER);
		if (_sws_TestExStyle(hwndCurrent, WS_EX_TOOLWINDOW))
		{
			bRet = !_sws_TestExStyle(hwndPrev, WS_EX_CONTROLPARENT) || hwndOwner != NULL;
			break;
		}
	}

	return bRet;
}

BOOL _sws_IsReallyVisible(HWND hWnd)
{
	RECT rc;
	GetWindowRect(hWnd, &rc);
	return IsWindowVisible(hWnd) && !IsRectEmpty(&rc);
}

BOOL _sws_IsGhosted(HWND hwnd)
{
	return _sws_GhostWindowFromHungWindow(hwnd) != NULL;
}

BOOL _sws_ShouldListWindowInAltTab(HWND hwnd)
{
	BOOL bRet = FALSE;

	if (IsWindow(hwnd) /*&& hwnd != _hwnd*/)
	{
		DWORD dwExStyle = (DWORD)GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
		HWND hwndOwner = GetWindow(hwnd, GW_OWNER);
		BOOLEAN bOwnerVisible = IsWindow(hwndOwner) && IsWindowEnabled(hwndOwner) && _sws_IsReallyVisible(hwndOwner);
		BOOLEAN bNoActivate = (dwExStyle & WS_EX_NOACTIVATE) != 0 || (dwExStyle & WS_EX_TOOLWINDOW) != 0;
		BOOLEAN bAppWindow = (dwExStyle & WS_EX_APPWINDOW) != 0;
		if (bAppWindow)
		{
			bNoActivate = FALSE;
		}
		bRet = _sws_IsReallyVisible(hwnd)
			// && IsWindowEnabled(hwnd)
			&& !bNoActivate
			&& (bAppWindow || (!bOwnerVisible && !_sws_IsOwnerToolWindow(hwnd)))
			&& !_sws_IsGhosted(hwnd);
	}

	return bRet;
}

BOOL _sws__IsTaskWindow(HWND hwnd)
{
	DWORD dwExStyle = (DWORD)GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
	return ((dwExStyle & WS_EX_APPWINDOW) != 0 || ((dwExStyle & WS_EX_TOOLWINDOW) == 0 && (dwExStyle & WS_EX_NOACTIVATE) == 0))
		&& IsWindowVisible(hwnd)
		&& !_sws_IsGhosted(hwnd);
}

BOOL _sws_IsTaskWindow(HWND hwnd, HWND* phwndTaskWindow)
{
	BOOL bRet = FALSE;

	if (_sws_ShouldListWindowInAltTab(hwnd))
	{
		HWND hwndTaskWindow = hwnd;
		HWND hwndCurrent = hwnd;
		while ((hwndCurrent = GetWindow(hwndCurrent, GW_OWNER)))
		{
			if (!_sws__IsTaskWindow(hwndCurrent))
				break;
			hwndTaskWindow = hwndCurrent;
		}
		*phwndTaskWindow = hwndTaskWindow;
		bRet = TRUE;
	}

	return bRet;
}

wchar_t* sws_WindowHelpers_GetAUMIDForHWND(HWND hWnd)
{
	WCHAR* pszAppId;
	if (SUCCEEDED(sws_AppResolver->lpVtbl->GetAppIDForWindow(sws_AppResolver, hWnd, &pszAppId, NULL, NULL, NULL)) && pszAppId) return pszAppId;
	return NULL;
}

BOOL sws_WindowHelpers_IsWindowShellManagedByExplorerPatcher(HWND hWnd)
{
	return GetPropW(hWnd, L"valinet.ExplorerPatcher.ShellManagedWindow") != 0;
}

BOOL sws_WindowHelpers_ShouldTreatShellManagedWindowAsNotShellManaged(HWND hWnd)
{
	return GetPropW(hWnd, L"Microsoft.Windows.ShellManagedWindowAsNormalWindow") != 0;
}

BOOL sws_WindowHelpers_IsAltTabWindow(HWND hWnd)
{
	// This identifies whether a window is a shell frame and includes those
	// A shell frame corresponds to, as far as I can tell, the frame of a UWP app
	// and we want those in the Alt-Tab list
	// Bugfix: Exclude hung shell frame (immersive) UWP windows, as we already include
	// ghost app windows in their place already
	if (sws_IsShellFrameWindow(hWnd) && !_sws_GhostWindowFromHungWindow(hWnd))
	{
		return TRUE;
	}
	// Next, we need to check whether the window is shell managed and exclude it if so
	// Shell managed windows, as far as I can tell, represent all immersive UI the
	// Windows shell might present the user with, like: Start menu, Search (Win+Q),
	// notifications, taskbars etc
	if (_sws_IsShellManagedWindow(hWnd) && !sws_WindowHelpers_ShouldTreatShellManagedWindowAsNotShellManaged(hWnd))
	{
		return FALSE;
	}
	// Also, exclude some windows created by ExplorerPatcher
	if (sws_WindowHelpers_IsWindowShellManagedByExplorerPatcher(hWnd))
	{
		return FALSE;
	}
	// Lastly, this check works with the remaining classic window and determines if it is a
	// "task window" and only includes it in Alt-Tab if so; this check is taken from
	// "AltTab.dll" in Windows 7 and this is how that OS decided to include a window in its
	// window switcher
	HWND hwndTaskWindow = NULL;
	return _sws_IsTaskWindow(hWnd, &hwndTaskWindow);
}

void sws_WindowHelpers_GetDesktopText(wchar_t* wszTitle)
{
	HANDLE hExplorerFrame = GetModuleHandleW(L"ExplorerFrame.dll");
	if (hExplorerFrame)
	{
		LoadStringW((HINSTANCE)hExplorerFrame, 13140, wszTitle, MAX_PATH);
	}
	else
	{
		wcscat_s(wszTitle, MAX_PATH, L"Desktop");
	}
}

static BOOL CALLBACK _sws_WindowHelpers_GetWallpaperHWNDCallback(HWND hwnd, LPARAM lParam)
{
	HWND* ret = (HWND*)lParam;

	HWND p = FindWindowExW(hwnd, NULL, L"SHELLDLL_DefView", NULL);
	if (p)
	{
		HWND t = FindWindowExW(NULL, hwnd, L"WorkerW", NULL);
		if (t)
		{
			*ret = t;
		}
	}
	return TRUE;
}

BOOL sws_WindowHelpers_EnsureWallpaperHWND()
{
	BOOL bRet = FALSE;

	// See: https://github.com/valinet/ExplorerPatcher/issues/525
	HWND hProgman = GetShellWindow();
	if (hProgman)
	{
		DWORD_PTR res0 = -1, res1 = -1, res2 = -1, res3 = -1;
		// CDesktopBrowser::_IsDesktopWallpaperInitialized and CWallpaperRenderer::ExpireImages
		SendMessageTimeoutW(hProgman, 0x052C, 10, 0, SMTO_NORMAL, 1000, &res0);
		if (SUCCEEDED(res0))
		{
			// Generate wallpaper window
			SendMessageTimeoutW(hProgman, 0x052C, 13, 0, SMTO_NORMAL, 1000, &res1);
			SendMessageTimeoutW(hProgman, 0x052C, 13, 1, SMTO_NORMAL, 1000, &res2);
			SendMessageTimeoutW(hProgman, 0x052C, 0, 0, SMTO_NORMAL, 1000, &res3);
			bRet = !res1 && !res2 && !res3;
		}
	}

	return bRet;
}

HWND sws_WindowHelpers_GetWallpaperHWND()
{
	HWND hWnd = NULL;

	// 24H2 has the Progman window as the whole desktop (icons + wallpaper)
	// Check if SHELLDLL_DefView is a direct child of Progman
	HWND hWndProgman = GetShellWindow();
	if (hWndProgman)
	{
		HWND hWndDefView = FindWindowExW(hWndProgman, NULL, L"SHELLDLL_DefView", NULL);
		if (hWndDefView)
		{
			hWnd = hWndProgman; // We're running on 24H2 or newer
		}
	}

	// Otherwise find a top-level WorkerW window with a SHELLDLL_DefView child, that's our desktop icons
	if (!hWnd)
	{
		EnumWindows((WNDENUMPROC)_sws_WindowHelpers_GetWallpaperHWNDCallback, (LPARAM)&hWnd);
	}

	return hWnd;
}

BOOL CALLBACK sws_WindowHelpers_AddAltTabWindowsToTimeStampedHWNDList(HWND hWnd, LPARAM hdpa)
{
	if (!hdpa)
	{
		return FALSE;
	}
	if (sws_WindowHelpers_IsAltTabWindow(hWnd))
	{
		sws_tshwnd* tshWnd = (sws_tshwnd*)malloc(sizeof(sws_tshwnd));
		if (tshWnd)
		{
			sws_tshwnd_Initialize(tshWnd, hWnd);
			sws_tshwnd_ModifyTimestamp(tshWnd, sws_WindowHelpers_GetStartTime());
			DPA_AppendPtr((HDPA)hdpa, tshWnd);
		}
	}
	return TRUE;
}

BOOL sws_WindowHelpers_AreAnimationsAllowed()
{
	if (sws_SHWindowsPolicy(POLID_TurnOffSPIAnimations))
	{
		return FALSE;
	}

	BOOL bAnimationsEnabled = FALSE;
	SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &bAnimationsEnabled, 0);
	return bAnimationsEnabled;
}

void sws_WindowHelpers_GetWindowText(HWND hWnd, LPWSTR lpWStr, DWORD dwLength)
{
	HWND hWndGhost = _sws_GhostWindowFromHungWindow(hWnd);
	if (hWndGhost)
	{
		InternalGetWindowText(hWndGhost, lpWStr, dwLength);
	}
	else
	{
		InternalGetWindowText(hWnd, lpWStr, dwLength);
	}
}

HWND sws_WindowHelpers_GetLastActivePopup(HWND hWnd)
{
	HWND hOwner = GetWindow(hWnd, GW_OWNER);
	return GetLastActivePopup(hOwner ? hOwner : hWnd);
}

void sws_WindowHelpers_Clear()
{
	Gdiplus::GdiplusShutdown(_sws_gdiplus_token);
	_sws_gdiplus_token = 0;
	if (sws_DefAppIcon)
	{
		DestroyIcon(sws_DefAppIcon);
		sws_DefAppIcon = NULL;
	}
	if (_sws_hWin32u)
	{
		FreeLibrary(_sws_hWin32u);
		_sws_hWin32u = NULL;
	}
	if (_sws_hUser32)
	{
		FreeLibrary(_sws_hUser32);
		_sws_hUser32 = NULL;
	}
	if (_sws_hShlwapi)
	{
		FreeLibrary(_sws_hShlwapi);
		_sws_hShlwapi = NULL;
	}
	if (_sws_hComctl32)
	{
		FreeLibrary(_sws_hComctl32);
		_sws_hComctl32 = NULL;
	}
	if (_sws_hShcore)
	{
		FreeLibrary(_sws_hShcore);
		_sws_hShcore = NULL;
	}
	if (sws_AppResolver)
	{
		sws_AppResolver->lpVtbl->Release(sws_AppResolver);
		sws_AppResolver = NULL;
	}
}

sws_error_t sws_WindowHelpers_Initialize()
{
	sws_error_t rv = SWS_ERROR_SUCCESS;

	if (_sws_gdiplus_token)
	{
		return rv;
	}
	GetSystemTimeAsFileTime(&sws_ancient_ft);
	GetSystemTimeAsFileTime(&sws_start_ft);
	if (!rv)
	{
		_sws_hComctl32 = LoadLibraryW(L"Comctl32.dll");
		if (!_sws_hComctl32)
		{
			rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_LOADLIBRARY_FAILED), NULL);
		}
	}
	if (!rv)
	{
		LoadIconWithScaleDown(
			(HINSTANCE)NULL,
			(PCWSTR)32512,
			(int)32,
			(int)32,
			(HICON*)(&(sws_DefAppIcon))
		);
		LoadIconWithScaleDown(
			(HINSTANCE)NULL,
			(PCWSTR)32512,
			(int)32,
			(int)32,
			(HICON*)(&(sws_LegacyDefAppIcon))
		);
		//sws_LegacyDefAppIcon = LoadIconW(NULL, MAKEINTRESOURCEW(IDI_APPLICATION));
	}
	if (!rv)
	{
        Gdiplus::GdiplusStartupInput gdiplusStartupInput = { 0 };
		rv = sws_error_Report(sws_error_GetFromGdiplusStatus(Gdiplus::GdiplusStartup(&_sws_gdiplus_token, &gdiplusStartupInput, NULL)), NULL);
	}
	if (!rv)
	{
		if (!_sws_hWin32u)
		{
			_sws_hWin32u = LoadLibraryW(L"win32u.dll");
			if (!_sws_hWin32u)
			{
				rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_LOADLIBRARY_FAILED), NULL);
			}
		}
	}
	if (!rv)
	{
		if (!_sws_pNtUserBuildHwndList)
		{
			_sws_pNtUserBuildHwndList = (NtUserBuildHwndList)GetProcAddress(_sws_hWin32u, "NtUserBuildHwndList");
			if (!_sws_pNtUserBuildHwndList)
			{
				rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_FUNCTION_NOT_FOUND), NULL);
			}
		}
	}
	if (!rv)
	{
		if (!_sws_hUser32)
		{
			_sws_hUser32 = LoadLibraryW(L"user32.dll");
			if (!_sws_hUser32)
			{
				rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_LOADLIBRARY_FAILED), NULL);
			}
		}
	}
	if (!rv)
	{
		if (!_sws_HungWindowFromGhostWindow)
		{
			_sws_HungWindowFromGhostWindow = (pHungWindowFromGhostWindow)GetProcAddress(_sws_hUser32, "HungWindowFromGhostWindow");
			if (!_sws_HungWindowFromGhostWindow)
			{
				rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_FUNCTION_NOT_FOUND), NULL);
			}
		}
	}
	if (!rv)
	{
		if (!_sws_GhostWindowFromHungWindow)
		{
			_sws_GhostWindowFromHungWindow = (pGhostWindowFromHungWindow)GetProcAddress(_sws_hUser32, "GhostWindowFromHungWindow");
			if (!_sws_GhostWindowFromHungWindow)
			{
				rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_FUNCTION_NOT_FOUND), NULL);
			}
		}
	}
	if (!rv)
	{
		if (!_sws_SetWindowCompositionAttribute)
		{
			_sws_SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(_sws_hUser32, "SetWindowCompositionAttribute");
			if (!_sws_SetWindowCompositionAttribute)
			{
				rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_FUNCTION_NOT_FOUND), NULL);
			}
		}
	}
	if (!rv)
	{
		if (!_sws_IsShellManagedWindow)
		{
			_sws_IsShellManagedWindow = (pIsShellManagedWindow)GetProcAddress(_sws_hUser32, (LPCSTR)2574);
			if (!_sws_IsShellManagedWindow)
			{
				rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_FUNCTION_NOT_FOUND), NULL);
			}
		}
	}
	if (!rv)
	{
		if (!sws_IsShellFrameWindow)
		{
			sws_IsShellFrameWindow = (pIsShellManagedWindow)GetProcAddress(_sws_hUser32, (LPCSTR)2573);
			if (!sws_IsShellFrameWindow)
			{
				rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_FUNCTION_NOT_FOUND), NULL);
			}
		}
	}
	if (!rv)
	{
		if (!_sws_CreateWindowInBand)
		{
			_sws_CreateWindowInBand = (pCreateWindowInBand)GetProcAddress(_sws_hUser32, "CreateWindowInBand");
			if (!_sws_CreateWindowInBand)
			{
				rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_FUNCTION_NOT_FOUND), NULL);
			}
		}
	}
	if (!rv)
	{
		if (!_sws_GetWindowBand)
		{
			_sws_GetWindowBand = (pGetWindowBand)GetProcAddress(_sws_hUser32, "GetWindowBand");
			if (!_sws_GetWindowBand)
			{
				rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_FUNCTION_NOT_FOUND), NULL);
			}
		}
	}
	if (!rv)
	{
		if (!_sws_SetWindowBand)
		{
			_sws_SetWindowBand = (pSetWindowBand)GetProcAddress(_sws_hUser32, "SetWindowBand");
			if (!_sws_SetWindowBand)
			{
				rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_FUNCTION_NOT_FOUND), NULL);
			}
		}
	}
	if (!rv)
	{
		if (!_sws_IsTopLevelWindow)
		{
			_sws_IsTopLevelWindow = (pIsTopLevelWindow)GetProcAddress(_sws_hUser32, "IsTopLevelWindow");
			if (!_sws_IsTopLevelWindow)
			{
				rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_FUNCTION_NOT_FOUND), NULL);
			}
		}
	}
	if (!rv)
	{
		if (!_sws_InternalGetWindowIcon)
		{
			_sws_InternalGetWindowIcon = (pInternalGetWindowIcon)GetProcAddress(_sws_hUser32, "InternalGetWindowIcon");
			if (!_sws_InternalGetWindowIcon)
			{
				rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_FUNCTION_NOT_FOUND), NULL);
			}
		}
	}
	if (!rv)
	{
		if (!_sws_hShcore)
		{
			_sws_hShcore = LoadLibraryW(L"shcore.dll");
			if (!_sws_hShcore)
			{
				rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_LOADLIBRARY_FAILED), NULL);
			}
		}
	}
	if (!rv)
	{
		if (!sws_SHWindowsPolicy)
		{
			sws_SHWindowsPolicy = (pSHWindowsPolicy)GetProcAddress(_sws_hShcore, (LPCSTR)190);
			if (!sws_SHWindowsPolicy)
			{
				rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_FUNCTION_NOT_FOUND), NULL);
			}
		}
	}
	if (!rv)
	{
		if (!sws_AppResolver)
		{
			CoCreateInstance(CLSID_StartMenuCacheAndAppResolver, NULL, CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER, IID_IAppResolver_8, (void**)&sws_AppResolver);
			if (!sws_AppResolver)
			{
				rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_APPRESOLVER_NOT_AVAILABLE), NULL);
			}
		}
	}
	return rv;
}

// sws_WindowSwitcherLayoutWindow.h
int sws_WindowSwitcherLayoutWindow_AddGroupedWnd(sws_WindowSwitcherLayoutWindow* _this, HWND hWnd)
{
    int rv = DPA_AppendPtr(_this->dpaGroupedWnds, hWnd);
    if (rv != -1) _this->dwCount++;
    return rv;
}

void sws_WindowSwitcherLayoutWindow_Erase(sws_WindowSwitcherLayoutWindow* _this)
{
    SIZE siz;
    siz.cx = 0;
    siz.cy = 0;
    _this->sizWindow = siz;
    RECT rc;
    rc.left = 0;
    rc.top = 0;
    rc.bottom = 0;
    rc.right = 0;
    _this->rcWindow = rc;
    _this->iRowMax = 0;
}

void sws_WindowSwitcherLayoutWindow_Clear(sws_WindowSwitcherLayoutWindow* _this)
{
    //if (_this->hIcon && !_this->bOwnProcess)
    if (_this->hIcon && sws_DefAppIcon && _this->hIcon != sws_DefAppIcon && sws_LegacyDefAppIcon && _this->hIcon != sws_LegacyDefAppIcon)
    {
        DestroyIcon(_this->hIcon);
    }
    if (_this->dpaGroupedWnds)
    {
        DPA_Destroy(_this->dpaGroupedWnds);
    }
    if (_this->wszAUMID)
    {
        CoTaskMemFree(_this->wszAUMID);
    }
    memset(_this, 0, sizeof(sws_WindowSwitcherLayoutWindow));
}

sws_error_t sws_WindowSwitcherLayoutWindow_Initialize(sws_WindowSwitcherLayoutWindow* _this, HWND hWnd, WCHAR* wszPath)
{
    sws_error_t rv = SWS_ERROR_SUCCESS;

    if (!rv)
    {
        if (!_this)
        {
            rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_NO_MEMORY), NULL);
        }
    }
    if (!rv)
    {
        memset(_this, 0, sizeof(sws_WindowSwitcherLayoutWindow));
    }
    if (!rv)
    {
        _this->hWnd = hWnd;
        _this->dwCount = 0;
    }
    if (!rv && wszPath)
    {
        wcscpy_s(_this->wszPath, MAX_PATH, wszPath);
    }
    if (!rv)
    {
        _this->wszAUMID = sws_WindowHelpers_GetAUMIDForHWND(_this->hWnd);
    }
    if (!rv)
    {
        _this->dpaGroupedWnds = DPA_Create(SWS_VECTOR_CAPACITY);
        if (!_this->dpaGroupedWnds)
        {
            rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_NO_MEMORY), NULL);
        }
    }

    return rv;
}

// sws_WindowSwitcherLayout.c
sws_error_t sws_WindowSwitcherLayout_InvalidateLayout(sws_WindowSwitcherLayout* _this)
{
	sws_error_t rv = SWS_ERROR_SUCCESS;

	sws_WindowSwitcherLayoutWindow* pWindowList = (sws_WindowSwitcherLayoutWindow*)_this->pWindowList.pList;
	for (int iCurrentWindow = _this->pWindowList.cbSize - 1; iCurrentWindow >= 0; iCurrentWindow--)
	{
		sws_WindowSwitcherLayoutWindow_Erase(&(pWindowList[iCurrentWindow]));
	}

	return rv;
}

sws_error_t sws_WindowSwitcherLayout_ComputeLayout(sws_WindowSwitcherLayout* _this, int direction, HWND hTarget, UINT col, UINT maxRow)
{
	sws_error_t rv = SWS_ERROR_SUCCESS;

	if (!rv)
	{
		int iObtainedIndex = 0;

		sws_WindowSwitcherLayoutWindow* pWindowList = (sws_WindowSwitcherLayoutWindow*)_this->pWindowList.pList;

		BOOL bHasTarget = FALSE;

		if (direction != SWS_WINDOWSWITCHERLAYOUT_COMPUTE_DIRECTION_INITIAL)
		{
			if (direction == SWS_WINDOWSWITCHERLAYOUT_COMPUTE_DIRECTION_BACKWARD)
			{
				bHasTarget = TRUE;
				iObtainedIndex = _this->pWindowList.cbSize - 1;
			}
			else if (direction == SWS_WINDOWSWITCHERLAYOUT_COMPUTE_DIRECTION_FORWARD)
			{
				if (_this->iIndex == _this->pWindowList.cbSize - 1)
				{
					iObtainedIndex = _this->iIndex;
				}
			}
			sws_WindowSwitcherLayout_InvalidateLayout(_this);
		}

		BOOL bFinishedLayout = FALSE;

		while (1)
		{
			int iCurrentCount = 0;

			for (int iCurrentWindow = iObtainedIndex ? iObtainedIndex : _this->iIndex; iCurrentWindow >= 0; iCurrentWindow--)
			{
				//TCHAR name[200];
				//GetWindowTextW(pWindowList[iCurrentWindow].hWnd, name, 200);
				//Wh_Log(L"%d %s ", pWindowList[iCurrentWindow].hWnd, name);

				if (pWindowList[iCurrentWindow].hWnd == _this->hWnd)
				{
					continue;
				}

				if (!bFinishedLayout)
				{
					pWindowList[iCurrentWindow].iRowMax = -1;
				}

				iCurrentCount++;
				if (iCurrentCount == _this->pWindowList.cbSize)
				{
					break;
				}
			}

			if (hTarget && direction == SWS_WINDOWSWITCHERLAYOUT_COMPUTE_DIRECTION_INITIAL)
			{
				int iObtained = 0;
				int iTmpTop = pWindowList[iObtained].rcWindow.top;
				for (int j = iObtained; j >= 0; j--)
				{
					if (pWindowList[j].rcWindow.top != iTmpTop)
					{
						iObtained = j;
						break;
					}
				}
				sws_WindowSwitcherLayout_InvalidateLayout(_this);
				iObtainedIndex = iObtained;
				bFinishedLayout = FALSE;
				continue;
			}
			if (!bHasTarget)
			{
				break;
			}
		}

		UINT row = _this->pWindowList.cbSize / col;
		if (_this->pWindowList.cbSize % col)
		{
			row++;
		}
		if (row > maxRow)
		{
			row = maxRow;
		}

		if (!_this->iWidth)
		{
			_this->iWidth = col * SWS_WINDOWSWITCHERLAYOUT_ITEMSIZE * (_this->cbDpiX / DEFAULT_DPI_X) + 23 * (_this->cbDpiX / DEFAULT_DPI_X) + _this->cbBorderSize * 6;
			_this->iHeight = row * SWS_WINDOWSWITCHERLAYOUT_ITEMSIZE * (_this->cbDpiX / DEFAULT_DPI_X) + 32 * (_this->cbDpiX / DEFAULT_DPI_X) + _this->cbFontHeight * 5 / 2.0;
			_this->iX = ((_this->mi.rcWork.right - _this->mi.rcWork.left) - _this->iWidth) / 2 + _this->mi.rcWork.left;
			_this->iY = ((_this->mi.rcWork.bottom - _this->mi.rcWork.top) - _this->iHeight) / 2 + _this->mi.rcWork.top;
			//Wh_Log(L"height: %d, cbCurrentTop: %d, %f %f %f\n", _this->iHeight, cbCurrentTop, _this->cbThumbnailAvailableHeight, _this->cbBottomPadding, _this->cbPadding);
		}
	}

	if (!rv)
	{
		/*Wh_Log(L"\n");
		sws_WindowSwitcherLayoutWindow* pWindowList = _this->pWindowList.pList;
		for (UINT i = 0; i < _this->pWindowList.cbSize; ++i)
		{
			TCHAR name[200];
			GetWindowText(pWindowList[i].hWnd, name, 200);
			Wh_Log(L"%d %s\n", pWindowList[i].hWnd, name);
		}*/
	}

	return rv;
}

void sws_WindowSwitcherLayout_Clear(sws_WindowSwitcherLayout* _this)
{
	if (_this)
	{
		DeleteObject(_this->hFontRegular);
		sws_WindowSwitcherLayoutWindow* pWindowList = (sws_WindowSwitcherLayoutWindow*)_this->pWindowList.pList;
		if (pWindowList)
		{
			for (int iCurrentWindow = 0; iCurrentWindow < _this->pWindowList.cbSize; ++iCurrentWindow)
			{
				sws_WindowSwitcherLayoutWindow_Clear(&(pWindowList[iCurrentWindow]));
			}
			sws_vector_Clear(&(_this->pWindowList));
		}
		memset(_this, 0, sizeof(sws_WindowSwitcherLayout));
	}
}

sws_error_t sws_WindowSwitcherLayout_Initialize(
	sws_WindowSwitcherLayout* _this, 
	HMONITOR hMonitor, 
	HWND hWnd, 
	DWORD* settings, 
	sws_vector* pHWNDList, 
	HWND hWndTarget,
	HWND hWndWallpaper
)
{
	sws_error_t rv = SWS_ERROR_SUCCESS;

	if (!rv)
	{
		if (!_this)
		{
			rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_NO_MEMORY), NULL);
		}
		memset(_this, 0, sizeof(sws_WindowSwitcherLayout));
	}
	if (!rv)
	{
		rv = sws_WindowHelpers_Initialize();
	}
	if (!rv)
	{
		rv = sws_vector_Initialize(&(_this->pWindowList), sizeof(sws_WindowSwitcherLayoutWindow));
	}
	_this->mi.cbSize = sizeof(MONITORINFO);
	if (!rv)
	{
		if (!GetMonitorInfoW(
			hMonitor,
			&(_this->mi)
		))
		{
			rv = sws_error_Report(sws_error_GetFromWin32Error(GetLastError()), NULL);
		}
	}
	if (!rv)
	{
		_this->bWallpaperAlwaysLast = SWS_WINDOWSWITCHERLAYOUT_WALLPAPER_ALWAYS_LAST;
		_this->bIncludeWallpaper = SWS_WINDOWSWITCHERLAYOUT_INCLUDE_WALLPAPER;
		if (settings) _this->bIncludeWallpaper = settings[0] && settings[3];
		_this->bWallpaperToggleBehavior = SWS_WINDOWSWITCHERLAYOUT_WALLPAPER_TOGGLE;
		_this->hWndWallpaper = hWndWallpaper;
		if (_this->bIncludeWallpaper)
		{
			if (_this->bWallpaperAlwaysLast && !hWndTarget)
			{
				sws_WindowSwitcherLayoutWindow swsLayoutWindow;
				sws_WindowSwitcherLayoutWindow_Initialize(&swsLayoutWindow, _this->hWndWallpaper, NULL);
				sws_vector_PushBack(&_this->pWindowList, &swsLayoutWindow);
			}
		}
	}
	if (!rv)
	{
		if (pHWNDList)
		{
			wchar_t* targetAUMID = sws_WindowHelpers_GetAUMIDForHWND(hWndTarget);
			sws_window* windowList = (sws_window*)pHWNDList->pList;
			sws_window* window = NULL;
			if (hWndTarget)
			{
				for (int i = 0; i < pHWNDList->cbSize; ++i)
				{
					if (windowList[i].hWnd == hWndTarget)
					{
						window = &(windowList[i]);
						break;
					}
				}
			}
			{
				WCHAR wszRundll32Path[MAX_PATH];
				GetSystemDirectoryW(wszRundll32Path, MAX_PATH);
				wcscat_s(wszRundll32Path, MAX_PATH, L"\\rundll32.exe");
				//HWND hWndForeground = GetForegroundWindow();
				for (int i = pHWNDList->cbSize - 1; i >= 0; i--)
				{
					BOOL isCloaked;
					DwmGetWindowAttribute(windowList[i].hWnd, DWMWA_CLOAKED, &isCloaked, sizeof(BOOL));
					if (isCloaked)
					{
						continue;
					}
					if (hWndTarget && hWndTarget != windowList[i].hWnd)
					{
						if (targetAUMID)
						{
							if (!(windowList[i].wszAUMID && !wcscmp(targetAUMID, windowList[i].wszAUMID))) continue;
						}
						else
						{
							if (!window)
							{
								continue;
							}
							else if (window->dwProcessId != windowList[i].dwProcessId && _wcsicmp(window->wszPath, windowList[i].wszPath))
							{
								continue;
							}
						}
					}
					if (!hWndTarget && settings[4] && _wcsicmp(windowList[i].wszPath, wszRundll32Path))
					{
						BOOL bShouldContinue = FALSE;
						for (int j = i - 1; j >= 0; j--)
						{
							if (sws_WindowHelpers_IsAltTabWindow(windowList[j].hWnd) && windowList[i].wszAUMID && windowList[j].wszAUMID)
							{
								if (!wcscmp(windowList[i].wszAUMID, windowList[j].wszAUMID) && (settings[1] ? MonitorFromWindow(windowList[i].hWnd, MONITOR_DEFAULTTONULL) == MonitorFromWindow(windowList[j].hWnd, MONITOR_DEFAULTTONULL) : TRUE))
								{
									windowList[j].pNextWindow = windowList + i;
									bShouldContinue = TRUE;
									break;
								}
							}
							else if (sws_WindowHelpers_IsAltTabWindow(windowList[j].hWnd) &&
								(windowList[i].dwProcessId == windowList[j].dwProcessId || !_wcsicmp(windowList[i].wszPath, windowList[j].wszPath)) &&
								(settings[1] ? MonitorFromWindow(windowList[i].hWnd, MONITOR_DEFAULTTONULL) == MonitorFromWindow(windowList[j].hWnd, MONITOR_DEFAULTTONULL) : TRUE))
							{
								bShouldContinue = TRUE;
								break;
							}
						}
						if (bShouldContinue)
						{
							continue;
						}
					}
					if (settings[1] && hMonitor != MonitorFromWindow(windowList[i].hWnd, MONITOR_DEFAULTTONULL))
					{
						continue;
					}
					sws_WindowSwitcherLayoutWindow swsLayoutWindow;
					sws_WindowSwitcherLayoutWindow_Initialize(&swsLayoutWindow, windowList[i].hWnd, windowList[i].wszPath);
					for (sws_window* pcw = windowList + i; pcw != NULL; pcw = pcw->pNextWindow) sws_WindowSwitcherLayoutWindow_AddGroupedWnd(&swsLayoutWindow, pcw->hWnd);
					sws_vector_PushBack(&_this->pWindowList, &swsLayoutWindow);
				}
			}
			if (targetAUMID) CoTaskMemFree(targetAUMID);
		}
	}
	_this->hWnd = hWnd;
	_this->hMonitor = hMonitor;
	_this->iIndex = _this->pWindowList.cbSize - 1;

	if (!rv)
	{
		HRESULT hr = GetDpiForMonitor(
			hMonitor,
			MDT_DEFAULT,
			&(_this->cbDpiX),
			&(_this->cbDpiY)
		);
		rv = sws_error_Report(sws_error_GetFromHRESULT(hr), NULL);
	}
	if (!rv)
	{
		sws_WindowSwitcherLayoutWindow* pWindowList = (sws_WindowSwitcherLayoutWindow*)_this->pWindowList.pList;
		for (int iCurrentWindow = _this->pWindowList.cbSize - 1; iCurrentWindow >= 0; iCurrentWindow--)
		{
			if (!pWindowList[iCurrentWindow].hIcon)
			{
				pWindowList[iCurrentWindow].rcIcon.left = 0;
				pWindowList[iCurrentWindow].rcIcon.top = 0;
				pWindowList[iCurrentWindow].rcIcon.right = SWS_WINDOWSWITCHERLAYOUT_ICONSIZE * (_this->cbDpiX / DEFAULT_DPI_X);
				pWindowList[iCurrentWindow].rcIcon.bottom = pWindowList[iCurrentWindow].rcIcon.right;
				pWindowList[iCurrentWindow].szIcon = pWindowList[iCurrentWindow].rcIcon.right;
				pWindowList[iCurrentWindow].hIcon = sws_DefAppIcon;
			}
		}
	}
	if (!rv)
	{
		NONCLIENTMETRICS ncm;
		ncm.cbSize = sizeof(NONCLIENTMETRICS);
		SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0, _this->cbDpiX);

		_this->cbBorderSize = ncm.iBorderWidth;

		_this->hFontRegular = CreateFontIndirectW(&ncm.lfCaptionFont);
		if (!_this->hFontRegular)
		{
			rv = sws_error_Report(sws_error_GetFromWin32Error(GetLastError()), NULL);
		}
		else
		{
			HDC hdc = GetDC(_this->hWnd);
			HGDIOBJ hOldFont = SelectObject(hdc, _this->hFontRegular);

			TEXTMETRICW tm;
			GetTextMetricsW(hdc, &tm);

			SelectObject(hdc, hOldFont);
			ReleaseDC(_this->hWnd, hdc);

			_this->cbFontHeight = tm.tmHeight;
			Wh_Log(L"font width: %d, height: %d\n", tm.tmAveCharWidth, tm.tmHeight);
		}
	}

	return rv;
}

// sws_WindowSwitcher.c
static void _sws_WindowSwitcher_UpdateAccessibleText(sws_WindowSwitcher* _this)
{
    sws_WindowSwitcherLayoutWindow* pWindowList = (sws_WindowSwitcherLayoutWindow*)_this->layout.pWindowList.pList;
    if (pWindowList)
    {
        if (!_this->layout.pWindowList.cbSize)
        {
            SetWindowTextW(_this->hWndAccessible, L"");
        }
        else
        {
            WCHAR wszAccText[MAX_PATH * 2], wszTitle[MAX_PATH];
            ZeroMemory(wszAccText, MAX_PATH * 2 * sizeof(WCHAR));
            ZeroMemory(wszTitle, MAX_PATH * sizeof(WCHAR));
            if (_this->layout.bIncludeWallpaper && pWindowList[_this->layout.iIndex].hWnd == _this->layout.hWndWallpaper)
            {
                sws_WindowHelpers_GetDesktopText(wszTitle);
            }
            else
            {
                WCHAR wszRundll32Path[MAX_PATH];
                GetSystemDirectoryW(wszRundll32Path, MAX_PATH);
                wcscat_s(wszRundll32Path, MAX_PATH, L"\\rundll32.exe");
                if (_this->bAlwaysUseWindowTitleAndIcon || _this->mode != SWS_WINDOWSWITCHER_LAYOUTMODE_FULL || !_this->bSwitcherIsPerApplication || !_wcsicmp(pWindowList[_this->layout.iIndex].wszPath, wszRundll32Path))
                {
                    sws_WindowHelpers_GetWindowText(pWindowList[_this->layout.iIndex].hWnd, wszTitle, MAX_PATH);
                }
                else
                {
                    if (pWindowList[_this->layout.iIndex].dwCount > 1)
                    {
                        DWORD dwPrefixLen = 0;
                        //swprintf_s(wszTitle, MAX_PATH, L"%d: ", dwCount);
                        dwPrefixLen = 0;// wcslen(wszTitle);
                        BOOL bAUMIDOk = FALSE;
                        if (pWindowList[_this->layout.iIndex].wszAUMID)
                        {
                            IShellItem2* pItem = NULL;
                            if (SUCCEEDED(SHCreateItemInKnownFolder(FOLDERID_AppsFolder, KF_FLAG_DONT_VERIFY, pWindowList[_this->layout.iIndex].wszAUMID, IID_IShellItem2, (void**)&pItem)) && pItem)
                            {
                                LPWSTR pDisplayName = NULL;
                                if (SUCCEEDED(pItem->GetDisplayName(SIGDN_NORMALDISPLAY, &pDisplayName)) && pDisplayName)
                                {
                                    bAUMIDOk = TRUE;
                                    wcscpy_s(wszTitle + dwPrefixLen, MAX_PATH - dwPrefixLen, pDisplayName);
                                    CoTaskMemFree(pDisplayName);
                                }
                                pItem->Release();
                            }
                        }
                        if (!bAUMIDOk)
                        {
                            IShellItem2* pIShellItem2 = NULL;
                            if (SUCCEEDED(SHCreateItemFromParsingName(pWindowList[_this->layout.iIndex].wszPath, NULL, IID_IShellItem2, (void**)&pIShellItem2)))
                            {
                                LPWSTR wszOutText = NULL;
                                if (SUCCEEDED(pIShellItem2->GetString(PKEY_FileDescription, &wszOutText)))
                                {
                                    int len = wcslen(wszOutText);
                                    if (len >= 4 && wszOutText[len - 1] == L'e' && wszOutText[len - 2] == L'x' && wszOutText[len - 3] == L'e' && wszOutText[len - 4] == L'.')
                                    {
                                        CoTaskMemFree(wszOutText);
                                        if (SUCCEEDED(pIShellItem2->GetString(PKEY_Software_ProductName, &wszOutText)))
                                        {
                                            wcscpy_s(wszTitle + dwPrefixLen, MAX_PATH - dwPrefixLen, wszOutText);
                                            CoTaskMemFree(wszOutText);
                                        }
                                        else
                                        {
                                            sws_WindowHelpers_GetWindowText(pWindowList[_this->layout.iIndex].hWnd, wszTitle + dwPrefixLen, MAX_PATH - dwPrefixLen);
                                        }
                                    }
                                    else
                                    {
                                        wcscpy_s(wszTitle + dwPrefixLen, MAX_PATH - dwPrefixLen, wszOutText);
                                        CoTaskMemFree(wszOutText);
                                    }
                                }
                                else
                                {
                                    sws_WindowHelpers_GetWindowText(pWindowList[_this->layout.iIndex].hWnd, wszTitle + dwPrefixLen, MAX_PATH - dwPrefixLen);
                                }
                                pIShellItem2->Release();
                            }
                        }
                        WCHAR wszTitle2[MAX_PATH];
                        wcscpy_s(wszTitle2, MAX_PATH, wszTitle);

                        WCHAR wszFormat[MAX_PATH];
                        HANDLE hExplorer = GetModuleHandleW(NULL);
                        if (hExplorer)
                        {
                            if (pWindowList[_this->layout.iIndex].dwCount)
                            {
                                LoadStringW((HINSTANCE)hExplorer, 11115, wszFormat, MAX_PATH);
                            }
                            else
                            {
                                LoadStringW((HINSTANCE)hExplorer, 11114, wszFormat, MAX_PATH);
                            }
                        }
                        if (!hExplorer)
                        {
                            if (pWindowList[_this->layout.iIndex].dwCount)
                            {
                                wcscat_s(wszFormat, MAX_PATH, L"%s - %d running windows");
                            }
                            else
                            {
                                wcscat_s(wszFormat, MAX_PATH, L"%s - 1 running window");
                            }

                        }
                        if (pWindowList[_this->layout.iIndex].dwCount)
                        {
                            swprintf_s(wszTitle, MAX_PATH, wszFormat, wszTitle2, pWindowList[_this->layout.iIndex].dwCount);
                        }
                        else
                        {
                            swprintf_s(wszTitle, MAX_PATH, wszFormat, wszTitle2);
                        }
                    }
                    else
                    {
                        sws_WindowHelpers_GetWindowText(pWindowList[_this->layout.iIndex].hWnd, wszTitle, MAX_PATH);
                    }
                }
            }
            swprintf_s(
                wszAccText,
                MAX_PATH * 2,
                L"%s: %d of %d",
                wszTitle,
                _this->layout.pWindowList.cbSize - _this->layout.iIndex,
                _this->layout.pWindowList.cbSize
            );
            //Wh_Log(L"[sws] Accesible text: %s.\n", wszAccText);
            SetWindowTextW(_this->hWndAccessible, wszAccText);
            NotifyWinEvent(
                EVENT_OBJECT_LIVEREGIONCHANGED,
                _this->hWndAccessible,
                OBJID_CLIENT,
                CHILDID_SELF
            );
        }
    }
}

static int CALLBACK _sws_WindowSwitcher_free_stub(void* p, void* pData)
{
    // This enables correct reporting of DPAs being freed in Debug builds
    free(p);
    return 1;
}

void CALLBACK _sws_WindowSwitcher_Wineventproc(
    HWINEVENTHOOK hWinEventHook,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG idChild,
    DWORD idEventThread,
    DWORD dwmsEventTime
)
{
    if ((event == EVENT_OBJECT_CREATE) && hwnd && idObject == OBJID_WINDOW)
    {
        PostMessageW(FindWindowW(SWS_WINDOWSWITCHER_CLASSNAME, NULL), RegisterWindowMessageW(L"SHELLHOOK"), HSHELL_WINDOWCREATED, (LPARAM)hwnd);
    }
    else if ((event == EVENT_OBJECT_DESTROY) && hwnd && idObject == OBJID_WINDOW)
    {
        PostMessageW(FindWindowW(SWS_WINDOWSWITCHER_CLASSNAME, NULL), RegisterWindowMessageW(L"SHELLHOOK"), HSHELL_WINDOWDESTROYED, (LPARAM)hwnd);
    }
    else if ((event == EVENT_SYSTEM_FOREGROUND) && hwnd && (idObject == OBJID_WINDOW) && _sws_IsTopLevelWindow(hwnd))
    {
        PostMessageW(FindWindowW(SWS_WINDOWSWITCHER_CLASSNAME, NULL), RegisterWindowMessageW(L"SHELLHOOK"), HSHELL_RUDEAPPACTIVATED, (LPARAM)hwnd);
    }
}

static void WINAPI _sws_WindowSwitcher_Calculate(sws_WindowSwitcher* _this, HWND* pOldHWNDs, DWORD cntOldHWNDs, DWORD dwOldIndex)
{
    HWND hWndInitial = (_this->mode == SWS_WINDOWSWITCHER_LAYOUTMODE_FULL && _this->layout.bIncludeWallpaper && _this->layout.bWallpaperAlwaysLast && _sws_WindowHelpers_IsDesktopRaised() && !IsWindowVisible(_this->hWnd)) ? _this->layout.hWndWallpaper : NULL;

    while (TRUE)
    {
        long long start = sws_milliseconds_now();
        if (!_this->lastMiniModehWnd)
        {
            HWND hFw = GetForegroundWindow();
            HWND hOwner = GetWindow(hFw, GW_OWNER);
            _this->lastMiniModehWnd = (hOwner && IsWindowVisible(hOwner)) ? hOwner : hFw;
        }
        sws_WindowSwitcherLayout_Initialize(
            &(_this->layout),
            _this->hMonitor,
            _this->hWnd,
            &(_this->bIncludeWallpaper),
            &(_this->pHWNDList),
            (_this->mode ? _this->lastMiniModehWnd : NULL),
            _this->hWndWallpaper
        );
        long long init = sws_milliseconds_now();
        sws_WindowSwitcherLayout_ComputeLayout(&(_this->layout), SWS_WINDOWSWITCHERLAYOUT_COMPUTE_DIRECTION_INITIAL, hWndInitial, _this->dwGridColumns, _this->dwGridRows);
        long long fin = sws_milliseconds_now();
        Wh_Log(L"[sws] CalculateHelper %d [[ %lld + %lld = %lld ]].\n", _this->mode, init - start, fin - init, fin - start);

        sws_WindowSwitcherLayoutWindow* pWindowList = (sws_WindowSwitcherLayoutWindow*)_this->layout.pWindowList.pList;
        int selectionIndex = -1;
        if (IsWindowVisible(_this->hWnd))
        {
            if (dwOldIndex >= 0 && dwOldIndex <= cntOldHWNDs - 1)
            {
                for (int i = 0; i < _this->layout.pWindowList.cbSize; ++i)
                {
                    if (pWindowList[i].hWnd == pOldHWNDs[dwOldIndex])
                    {
                        selectionIndex = i;
                        break;
                    }
                }
                if (selectionIndex < 0)
                {
                    BOOL bSuperBreak = FALSE;
                    for (int j = dwOldIndex - 1; j >= 0; j--)
                    {
                        for (int i = 0; i < _this->layout.pWindowList.cbSize; ++i)
                        {
                            if (pWindowList[i].hWnd == pOldHWNDs[j])
                            {
                                selectionIndex = i;
                                bSuperBreak = TRUE;
                                break;
                            }
                        }
                        if (bSuperBreak) break;
                    }
                }
                if (selectionIndex < 0)
                {
                    BOOL bSuperBreak = FALSE;
                    for (int i = 0; i < _this->layout.pWindowList.cbSize; ++i)
                    {
                        sws_window* pHWNDList = (sws_window*)_this->pHWNDList.pList;
                        int k = -1;
                        if (pHWNDList)
                        {
                            for (int j = 0; j < _this->pHWNDList.cbSize; ++j)
                            {
                                if (pHWNDList[j].hWnd == pWindowList[i].hWnd)
                                {
                                    k = j;
                                    break;
                                }
                            }
                        }
                        if (pHWNDList && k >= 0)
                        {
                            for (sws_window* pcw = pHWNDList + k; pcw != NULL; pcw = pcw->pNextWindow)
                            {
                                if (pOldHWNDs[dwOldIndex] == pcw->hWnd)
                                {
                                    k = 0;
                                    for (int j = 0; j < _this->layout.pWindowList.cbSize; ++j)
                                    {
                                        if (pcw->hWnd == pWindowList[j].hWnd)
                                        {
                                            k = j;
                                            break;
                                        }
                                    }
                                    selectionIndex = i;
                                    bSuperBreak = TRUE;
                                    break;
                                }
                            }
                        }
                        if (bSuperBreak) break;
                    }
                }
            }
            if (selectionIndex != -1)
            {
                _this->layout.iIndex = selectionIndex;
                if (_this->layout.iIndex > _this->layout.pWindowList.cbSize - 1)
                {
                    _this->layout.iIndex = _this->layout.pWindowList.cbSize - 1;
                }
            }
        }
        if (selectionIndex == -1)
        {
            _this->layout.iIndex = _this->layout.pWindowList.cbSize == 1 ? 0 : _this->layout.iIndex - 1 - _this->layout.numTopMost;
        }
        if (_this->layout.iIndex < 0)
        {
            _this->layout.iIndex = 0;
        }
        if (_this->bIncludeWallpaper && _this->bWallpaperAlwaysLast &&
            _this->layout.pWindowList.cbSize == 2 && IsIconic(pWindowList[1].hWnd)
            )
        {
            _this->layout.iIndex = 1;
        }

        _this->cwIndex = -1;
        _this->cwMask = 0;
        _this->bPartialRedraw = FALSE;

        break;
    }
}

void _sws_WindowSwitcher_SwitchToSelectedItemAndDismiss(sws_WindowSwitcher* _this)
{
    sws_WindowSwitcherLayoutWindow* pWindowList = (sws_WindowSwitcherLayoutWindow*)_this->layout.pWindowList.pList;
    if (_this->layout.bIncludeWallpaper && pWindowList[_this->layout.iIndex].hWnd == _this->layout.hWndWallpaper)
    {
        _sws_WindowHelpers_ToggleDesktop();
    }
    else
    {
        sws_WindowSwitcherLayoutWindow* pWindowList = (sws_WindowSwitcherLayoutWindow*)_this->layout.pWindowList.pList;
        if (pWindowList)
        {
            wchar_t tt[MAX_PATH];
            sws_WindowHelpers_GetWindowText(pWindowList[_this->layout.iIndex].hWnd, tt, MAX_PATH);
            Wh_Log(L"[sws] Chosen window: %s\n", tt);
            sws_WindowHelpers_GetWindowText(sws_WindowHelpers_GetLastActivePopup(pWindowList[_this->layout.iIndex].hWnd), tt, MAX_PATH);
            Wh_Log(L"[sws] Last active popup: %s\n", tt);
            GetClassNameW(GetWindow(pWindowList[_this->layout.iIndex].hWnd, GW_OWNER), tt, MAX_PATH);
            Wh_Log(L"[sws] Owner of window: %s\n", tt);
            HWND hLastActivePopup = sws_WindowHelpers_GetLastActivePopup(pWindowList[_this->layout.iIndex].hWnd);
            SwitchToThisWindow(IsWindowVisible(hLastActivePopup) ? hLastActivePopup : pWindowList[_this->layout.iIndex].hWnd, TRUE);
        }
    }
    ShowWindow(_this->hWnd, SW_HIDE);
}

static void _sws_WindowSwitcher_DrawContour(sws_WindowSwitcher* _this, HDC hdcPaint, RECT rc, int contour_size, RGBQUAD transparent)
{
    BYTE r = 0, g = 0, b = 0;
    COLORREF highlightColor = GetSysColor(COLOR_HIGHLIGHT);
    r = GetRValue(highlightColor);
    g = GetGValue(highlightColor);
    b = GetBValue(highlightColor);

    BITMAPINFO bi;
    ZeroMemory(&bi, sizeof(BITMAPINFO));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = 1;
    bi.bmiHeader.biHeight = 1;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    RGBQUAD desiredColor = { b, g, r, 0xFF };

    int thickness = contour_size * (_this->layout.cbDpiX / DEFAULT_DPI_X);

    StretchDIBits(hdcPaint, rc.left, rc.top, thickness, rc.bottom - rc.top,
        0, 0, 1, 1, &desiredColor, &bi,
        DIB_RGB_COLORS, SRCCOPY);
    StretchDIBits(hdcPaint, rc.right - thickness, rc.top, thickness, rc.bottom - rc.top,
        0, 0, 1, 1, &desiredColor, &bi,
        DIB_RGB_COLORS, SRCCOPY);
    StretchDIBits(hdcPaint, rc.left, rc.top, rc.right - rc.left, thickness,
        0, 0, 1, 1, &desiredColor, &bi,
        DIB_RGB_COLORS, SRCCOPY);
    StretchDIBits(hdcPaint, rc.left, rc.bottom - thickness, rc.right - rc.left, thickness,
        0, 0, 1, 1, &desiredColor, &bi,
        DIB_RGB_COLORS, SRCCOPY);
}

sws_error_t sws_WindowSwitcher_RegisterHotkeys(sws_WindowSwitcher* _this, HKL hkl)
{
    sws_error_t rv = SWS_ERROR_SUCCESS;

    if (hkl)
    {
        _this->vkTilde = MapVirtualKeyExW(0x29, MAPVK_VSC_TO_VK_EX, hkl);
    }
    else
    {
        _this->vkTilde = MapVirtualKeyW(0x29, MAPVK_VSC_TO_VK_EX);
    }

    if (!rv)
    {
        if (!RegisterHotKey(_this->hWnd, 1, MOD_ALT, VK_TAB))
        {
            rv = sws_error_GetFromWin32Error(GetLastError());
        }
    }
    if (!rv)
    {
        if (!RegisterHotKey(_this->hWnd, -1, MOD_ALT, _this->vkTilde))
        {
            rv = sws_error_GetFromWin32Error(GetLastError());
        }
    }
    if (!rv)
    {
        if (!RegisterHotKey(_this->hWnd, 2, MOD_ALT | MOD_SHIFT, VK_TAB))
        {
            rv = sws_error_GetFromWin32Error(GetLastError());
        }
    }
    if (!rv)
    {
        if (!RegisterHotKey(_this->hWnd, -2, MOD_ALT | MOD_SHIFT, _this->vkTilde))
        {
            rv = sws_error_GetFromWin32Error(GetLastError());
        }
    }
    if (!rv)
    {
        if (!RegisterHotKey(_this->hWnd, 3, MOD_ALT | MOD_CONTROL, VK_TAB))
        {
            rv = sws_error_GetFromWin32Error(GetLastError());
        }
    }
    if (!rv)
    {
        if (!RegisterHotKey(_this->hWnd, -3, MOD_ALT | MOD_CONTROL, _this->vkTilde))
        {
            rv = sws_error_GetFromWin32Error(GetLastError());
        }
    }
    if (!rv)
    {
        if (!RegisterHotKey(_this->hWnd, 4, MOD_ALT | MOD_SHIFT | MOD_CONTROL, VK_TAB))
        {
            rv = sws_error_GetFromWin32Error(GetLastError());
        }
    }
    if (!rv)
    {
        if (!RegisterHotKey(_this->hWnd, -4, MOD_ALT | MOD_SHIFT | MOD_CONTROL, _this->vkTilde))
        {
            rv = sws_error_GetFromWin32Error(GetLastError());
        }
    }
    //Wh_Log(L"[sws] Hotkey registration result: %d\n", rv);

    return rv;
}

void sws_WindowSwitcher_UnregisterHotkeys(sws_WindowSwitcher* _this)
{
    UnregisterHotKey(_this->hWnd, 1);
    UnregisterHotKey(_this->hWnd, 2);
    UnregisterHotKey(_this->hWnd, 3);
    UnregisterHotKey(_this->hWnd, 4);
    UnregisterHotKey(_this->hWnd, -1);
    UnregisterHotKey(_this->hWnd, -2);
    UnregisterHotKey(_this->hWnd, -3);
    UnregisterHotKey(_this->hWnd, -4);
}

void sws_WindowSwitcher_Paint(sws_WindowSwitcher* _this, DWORD dwFlags)
{
    HWND hWnd = _this->hWnd;
    BOOL bIsWindowVisible = IsWindowVisible(_this->hWnd);

    PAINTSTRUCT ps;
    HDC hDC = BeginPaint(hWnd, &ps);

    sws_WindowSwitcherLayoutWindow* pWindowList = (sws_WindowSwitcherLayoutWindow*)_this->layout.pWindowList.pList;

    RECT rc;
    GetClientRect(hWnd, &rc);
    SIZE siz = { rc.right - rc.left, rc.bottom - rc.top };

    HDC hdcPaint = _this->hdcPaint;
    if (hdcPaint)
    {
        SelectObject(hdcPaint, _this->layout.hFontRegular);

        BYTE r = 0, g = 0, b = 0, a = 255;
        COLORREF btnFace = GetSysColor(COLOR_BTNFACE);
        r = GetRValue(btnFace) * a / 255;
        g = GetGValue(btnFace) * a / 255;
        b = GetBValue(btnFace) * a / 255;
        RGBQUAD bkcol = { b, g, r, a };

        // Draw background
        if ((dwFlags & SWS_WINDOWSWITCHER_PAINTFLAGS_REDRAWENTIRE))
        {
            BITMAPINFO bi;
            ZeroMemory(&bi, sizeof(BITMAPINFO));
            bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bi.bmiHeader.biWidth = 1;
            bi.bmiHeader.biHeight = 1;
            bi.bmiHeader.biPlanes = 1;
            bi.bmiHeader.biBitCount = 32;
            bi.bmiHeader.biCompression = BI_RGB;
            StretchDIBits(hdcPaint, 0, 0, siz.cx, siz.cy, 0, 0, 1, 1, &bkcol, &bi, DIB_RGB_COLORS, SRCCOPY);
        }

        int col = _this->dwGridColumns;
        int row = _this->dwGridRows;

        int left = 11 * (_this->layout.cbDpiX / DEFAULT_DPI_X);
        int bottom = siz.cy - _this->layout.cbFontHeight;
        RECT rcTitleArea = { left, (LONG)(bottom - _this->layout.cbFontHeight), siz.cx - left, bottom };
        InflateRect(&rcTitleArea, 0, _this->layout.cbFontHeight / 2);
        DrawEdge(
            hdcPaint,
            &rcTitleArea,
            EDGE_SUNKEN,
            BF_RECT
        );

        SetTextColor(hdcPaint, GetSysColor(COLOR_BTNTEXT));
        SetBkMode(hdcPaint, TRANSPARENT);
        InflateRect(&rcTitleArea, -4, 0);

        Gdiplus::GpGraphics* pGdipGraphics = NULL;
        Gdiplus::DllExports::GdipCreateFromHDC(
            (HDC)hdcPaint,
            &pGdipGraphics
        );

        int gridX = -1;
        int gridY = 0;

        int selGridX = -1;
        int selGridY = -1;

        int i = _this->layout.iFirstItemIndex;

        BOOL recalcNeeded = FALSE;

        for (int j = 0; j < _this->layout.pWindowList.cbSize; ++j)
        {
            pWindowList[j].gridX = -1;
            pWindowList[j].gridY = -1;
        }
        while (TRUE)
        {
            if (i-- == 0)
            {
                if (_this->layout.pWindowList.cbSize <= col * row)
                {
                    break;
                }
                else
                {
                    i = _this->layout.pWindowList.cbSize - 1;
                }
            }
            gridX++;
            if (gridX > col - 1)
            {
                gridX = 0;
                gridY++;
            }
            pWindowList[i].gridX = gridX;
            pWindowList[i].gridY = gridY;
            if (i == _this->layout.iIndex)
            {
                if (_this->layout.pWindowList.cbSize > col)
                {
                    int lastColItemCnt = _this->layout.pWindowList.cbSize % col;
                    int lastRow = (_this->layout.pWindowList.cbSize / col) + (lastColItemCnt ? 1 : 0);
                    if (_this->lastKey == VK_UP)
                    {
                        selGridX = gridX;
                        selGridY = gridY - 1;
                        if (_this->layout.pWindowList.cbSize > col * row)
                        {
                            if (selGridY < 0)
                            {
                                selGridY = 0;
                                _this->layout.iFirstItemIndex += col;
                                if (_this->layout.iFirstItemIndex > _this->layout.pWindowList.cbSize - 1)
                                {
                                    _this->layout.iFirstItemIndex -= _this->layout.pWindowList.cbSize;
                                }
                                recalcNeeded = TRUE;
                            }
                        }
                        else if (selGridY == -1)
                        {
                            selGridY = lastRow - 1;
                            if (lastColItemCnt > 0 && selGridX >= lastColItemCnt)
                            {
                                selGridX = lastColItemCnt - 1;
                            }
                        }
                        if (selGridY < 0)
                        {
                            selGridY = 0;
                        }
                    }
                    else if (_this->lastKey == VK_DOWN)
                    {
                        selGridX = gridX;
                        selGridY = gridY + 1;
                        if (_this->layout.pWindowList.cbSize > col * row)
                        {
                            if (selGridY > row - 1)
                            {
                                selGridY = row - 1;
                                _this->layout.iFirstItemIndex -= col;
                                if (_this->layout.iFirstItemIndex < 0)
                                {
                                    _this->layout.iFirstItemIndex += _this->layout.pWindowList.cbSize;
                                }
                                recalcNeeded = TRUE;
                            }
                        }
                        else if (selGridY >= lastRow - 1)
                        {
                            if (selGridY > lastRow - 1)
                            {
                                selGridY = 0;
                            }
                            if (lastColItemCnt > 0 && selGridX >= lastColItemCnt)
                            {
                                selGridX = lastColItemCnt - 1;
                            }
                        }
                        //Wh_Log(L"asdf - selGridX >= lastColItemCnt: %d >= %d (%d) && selGridY == row - 1: %d == %d (%d)\n", selGridX, lastColItemCnt, selGridX >= lastColItemCnt, selGridY, row - 1, selGridY == row - 1);
                        if (selGridY < 0)
                        {
                            selGridY = 0;
                        }
                    }
                    else
                    {
                        selGridX = gridX;
                        selGridY = gridY;
                    }
                }
                else
                {
                    selGridX = gridX;
                    selGridY = gridY;
                }
                _this->lastKey = NULL;
            }
            if (gridY >= row)
            {
                break;
            }
        }

        if (recalcNeeded)
        {
            _this->cwMask = 0;
            _this->cwIndex = -1;

            gridX = -1;
            gridY = 0;
            for (int j = 0; j < _this->layout.pWindowList.cbSize; ++j)
            {
                pWindowList[j].gridX = -1;
                pWindowList[j].gridY = -1;
            }
            i = _this->layout.iFirstItemIndex;
            while (TRUE)
            {
                if (i-- == 0)
                {
                    if (_this->layout.pWindowList.cbSize <= col * row)
                    {
                        break;
                    }
                    else
                    {
                        i = _this->layout.pWindowList.cbSize - 1;
                    }
                }
                gridX++;
                if (gridX > col - 1)
                {
                    gridX = 0;
                    gridY++;
                }
                pWindowList[i].gridX = gridX;
                pWindowList[i].gridY = gridY;
                if (gridY >= row)
                {
                    break;
                }
            }
        }

        UINT duplicateCount = 0;

        for (int i = 0; i < _this->layout.pWindowList.cbSize; ++i)
        {
            int gridX = pWindowList[i].gridX;
            int gridY = pWindowList[i].gridY;
            if (gridY >= row || gridY == -1)
            {
                continue;
            }
            if (selGridX == gridX && selGridY == gridY)
            {
                _this->layout.iIndex = i;
            }
            if (i == _this->layout.iIndex && _this->layout.pWindowList.cbSize > col * row)
            {
                //Wh_Log(L"i nsgX nsgY gridX gridY: %d %d %d %d %d\n", i, selGridX, selGridY, gridX, gridY);
                if (_this->direction == SWS_WINDOWSWITCHERLAYOUT_COMPUTE_DIRECTION_FORWARD)
                {
                    if (gridX == col - 1 && gridY == row - 1)
                    {
                        _this->scrollDirection = SWS_WINDOWSWITCHERLAYOUT_COMPUTE_DIRECTION_FORWARD;
                    }
                    else
                    {
                        _this->scrollDirection = SWS_WINDOWSWITCHERLAYOUT_COMPUTE_DIRECTION_INITIAL;
                    }
                }
                else if (_this->direction == SWS_WINDOWSWITCHERLAYOUT_COMPUTE_DIRECTION_BACKWARD)
                {
                    if (gridX == 0 && gridY == 0)
                    {
                        _this->scrollDirection = SWS_WINDOWSWITCHERLAYOUT_COMPUTE_DIRECTION_BACKWARD;
                    }
                    else
                    {
                        _this->scrollDirection = SWS_WINDOWSWITCHERLAYOUT_COMPUTE_DIRECTION_INITIAL;
                    }
                }
            }

            RGBQUAD rgbStart = bkcol;

            COLORREF highlightColor = GetSysColor(COLOR_HIGHLIGHT);
            r = GetRValue(highlightColor);
            g = GetGValue(highlightColor);
            b = GetBValue(highlightColor);

            RGBQUAD rgbEnd = { b, g, r, 0xFF };
            RGBQUAD rgbFinal;


            sws_tshwnd* tshWnd = NULL;
            if (pWindowList)
            {
                if (_this->bSwitcherIsPerApplication && _this->mode == SWS_WINDOWSWITCHER_LAYOUTMODE_FULL)
                {
                    tshWnd = pWindowList[i].last_flashing_tshwnd;
                    sws_window* pHWNDList = (sws_window*)_this->pHWNDList.pList;
                    int k = -1;
                    if (pHWNDList)
                    {
                        for (int j = 0; j < _this->pHWNDList.cbSize; ++j)
                        {
                            if (pHWNDList[j].hWnd == pWindowList[i].hWnd)
                            {
                                k = j;
                                break;
                            }
                        }
                    }
                    if (pHWNDList && k >= 0)
                    {
                        for (sws_window* pcw = pHWNDList + k; pcw != NULL; pcw = pcw->pNextWindow)
                        {
                            if (pcw->tshWnd && sws_tshwnd_GetFlashState(pcw->tshWnd))
                            {
                                tshWnd = pcw->tshWnd;
                                pWindowList[i].last_flashing_tshwnd = tshWnd;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    tshWnd = pWindowList[i].tshWnd;
                }
            }

            if (tshWnd)
            {
                if (sws_WindowHelpers_AreAnimationsAllowed())
                {
                    rgbFinal.rgbRed = sws_linear(sws_easing_easeOutQuad(tshWnd->cbFlashAnimationState), rgbStart.rgbRed, rgbEnd.rgbRed);
                    rgbFinal.rgbGreen = sws_linear(sws_easing_easeOutQuad(tshWnd->cbFlashAnimationState), rgbStart.rgbGreen, rgbEnd.rgbGreen);
                    rgbFinal.rgbBlue = sws_linear(sws_easing_easeOutQuad(tshWnd->cbFlashAnimationState), rgbStart.rgbBlue, rgbEnd.rgbBlue);
                    rgbFinal.rgbReserved = sws_linear(sws_easing_easeOutQuad(tshWnd->cbFlashAnimationState), rgbStart.rgbReserved, rgbEnd.rgbReserved);
                }
                else
                {
                    if (!sws_tshwnd_GetFlashState(tshWnd))
                    {
                        rgbFinal = rgbStart;
                    }
                    else
                    {
                        rgbFinal = rgbEnd;
                        /*if (!(tshWnd->dwFlashAnimationState % 2))
                        {
                            rgbFinal = rgbEnd;
                        }
                        else
                        {
                            rgbFinal = rgbStart;
                        }*/
                    }
                }
            }
            else
            {
                rgbFinal = rgbStart;
            }

            // Grid layout
            UINT leftStart = 11 * (_this->layout.cbDpiX / DEFAULT_DPI_X) + _this->layout.cbBorderSize;
            if (_this->layout.pWindowList.cbSize < col)
            {
                leftStart = (_this->layout.iWidth - _this->layout.pWindowList.cbSize * SWS_WINDOWSWITCHERLAYOUT_ITEMSIZE * (_this->layout.cbDpiX / DEFAULT_DPI_X)) / 2 - 2 * _this->layout.cbBorderSize;
            }
            pWindowList[i].rcWindow.left = leftStart + gridX * SWS_WINDOWSWITCHERLAYOUT_ITEMSIZE * (_this->layout.cbDpiX / DEFAULT_DPI_X);
            pWindowList[i].rcWindow.right = pWindowList[i].rcWindow.left + SWS_WINDOWSWITCHERLAYOUT_ITEMSIZE * (_this->layout.cbDpiX / DEFAULT_DPI_X);
            pWindowList[i].rcWindow.top = 16 * (_this->layout.cbDpiX / DEFAULT_DPI_X) + gridY * SWS_WINDOWSWITCHERLAYOUT_ITEMSIZE * (_this->layout.cbDpiX / DEFAULT_DPI_X);
            pWindowList[i].rcWindow.bottom = pWindowList[i].rcWindow.top + SWS_WINDOWSWITCHERLAYOUT_ITEMSIZE * (_this->layout.cbDpiX / DEFAULT_DPI_X);
            //Wh_Log(L"i gridX gridY x y: %d %d %d %d %d %d %d\n", i, gridX, gridY, pWindowList[i].rcWindow.left, pWindowList[i].rcWindow.top);

            // Draw flash rectangle
            BOOL bShouldDrawFlashRectangle = FALSE;
            if ((dwFlags & SWS_WINDOWSWITCHER_PAINTFLAGS_ISFLASHANIMATION) && tshWnd)
            {
                if (sws_tshwnd_GetFlashState(tshWnd))
                {
                    if (tshWnd->dwFlashAnimationState != SWS_WINDOWSWITCHER_ANIMATOR_FLASH_MAXSTATE)
                    {
                        if (!(tshWnd->dwFlashAnimationState % 2))
                        {
                            tshWnd->cbFlashAnimationState += SWS_WINDOWSWITCHER_ANIMATOR_FLASH_STEP;
                        }
                        else
                        {
                            tshWnd->cbFlashAnimationState -= SWS_WINDOWSWITCHER_ANIMATOR_FLASH_STEP;
                        }
                        if (tshWnd->cbFlashAnimationState <= 0.0)
                        {
                            tshWnd->cbFlashAnimationState = 0.0;
                        }
                        if (tshWnd->cbFlashAnimationState >= 1.0)
                        {
                            tshWnd->cbFlashAnimationState = 1.0;
                        }
                        bShouldDrawFlashRectangle = TRUE;
                    }

                    if (tshWnd->cbFlashAnimationState == 1.0 && tshWnd->dwFlashAnimationState >= SWS_WINDOWSWITCHER_ANIMATOR_FLASH_MAXSTATE)
                    {
                    }
                    else if (tshWnd->cbFlashAnimationState == 1.0 && tshWnd->dwFlashAnimationState == SWS_WINDOWSWITCHER_ANIMATOR_FLASH_MAXSTATE - 1)
                    {
                        tshWnd->dwFlashAnimationState = SWS_WINDOWSWITCHER_ANIMATOR_FLASH_MAXSTATE;
                    }
                    else if (tshWnd->cbFlashAnimationState == 1.0 && !(tshWnd->dwFlashAnimationState % 2))
                    {
                        tshWnd->dwFlashAnimationState = tshWnd->dwFlashAnimationState + 1;
                        tshWnd->cbFlashAnimationState -= SWS_WINDOWSWITCHER_ANIMATOR_FLASH_STEP;
                    }
                    else if (tshWnd->cbFlashAnimationState == 0.0 && (tshWnd->dwFlashAnimationState % 2))
                    {
                        tshWnd->dwFlashAnimationState = tshWnd->dwFlashAnimationState + 1;
                        tshWnd->cbFlashAnimationState += SWS_WINDOWSWITCHER_ANIMATOR_FLASH_STEP;
                    }
                }
                else
                {
                    tshWnd->dwFlashAnimationState = 0;
                    tshWnd->cbFlashAnimationState -= SWS_WINDOWSWITCHER_ANIMATOR_FLASH_STEP;
                    if (tshWnd->cbFlashAnimationState <= 0.0)
                    {
                        tshWnd->cbFlashAnimationState = 0.0;
                    }
                    else
                    {
                        bShouldDrawFlashRectangle = TRUE;
                    }
                }
            }
            if (
                pWindowList &&
                bIsWindowVisible &&
                ((dwFlags & SWS_WINDOWSWITCHER_PAINTFLAGS_REDRAWENTIRE) ||
                    ((dwFlags & SWS_WINDOWSWITCHER_PAINTFLAGS_ISFLASHANIMATION) && bShouldDrawFlashRectangle) ||
                    ((dwFlags & SWS_WINDOWSWITCHER_PAINTFLAGS_ACTIVEMASKORINDEXCHANGED) && (i == _this->cwIndex || i == _this->cwOldIndex))
                    ))
            {
                //Wh_Log(L"%d %d\n", dwFlags, i);

                RECT rc = pWindowList[i].rcWindow;
                rc.left += SWS_WINDOWSWITCHER_CONTOUR_SIZE;
                rc.top += SWS_WINDOWSWITCHER_CONTOUR_SIZE;
                rc.bottom -= SWS_WINDOWSWITCHER_CONTOUR_SIZE;
                rc.right -= SWS_WINDOWSWITCHER_CONTOUR_SIZE;

                BITMAPINFO bi;
                ZeroMemory(&bi, sizeof(BITMAPINFO));
                bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                bi.bmiHeader.biWidth = 1;
                bi.bmiHeader.biHeight = 1;
                bi.bmiHeader.biPlanes = 1;
                bi.bmiHeader.biBitCount = 32;
                bi.bmiHeader.biCompression = BI_RGB;
                StretchDIBits(hdcPaint, rc.left + 1, rc.top + 1, rc.right - rc.left - 2, rc.bottom - rc.top - 2,
                    0, 0, 1, 1, &rgbFinal, &bi,
                    DIB_RGB_COLORS, SRCCOPY);
            }

            // Draw focus highlight rectangle
            if (pWindowList &&
                bIsWindowVisible &&
                gridX == selGridX &&
                gridY == selGridY &&
                ((dwFlags & SWS_WINDOWSWITCHER_PAINTFLAGS_REDRAWENTIRE) ||
                    ((dwFlags & SWS_WINDOWSWITCHER_PAINTFLAGS_ACTIVEMASKORINDEXCHANGED) && (i == _this->cwIndex || i == _this->cwOldIndex))
                    )
                )
            {
                duplicateCount++;
                _sws_WindowSwitcher_DrawContour(
                    _this,
                    hdcPaint,
                    pWindowList[_this->layout.iIndex].rcWindow,
                    SWS_WINDOWSWITCHER_CONTOUR_SIZE,
                    rgbFinal
                );
            }

            // Draw hover rectangle
            if (pWindowList &&
                bIsWindowVisible &&
                _this->cwIndex == i &&
                ((dwFlags & SWS_WINDOWSWITCHER_PAINTFLAGS_REDRAWENTIRE) ||
                    ((dwFlags & SWS_WINDOWSWITCHER_PAINTFLAGS_ISFLASHANIMATION) && bShouldDrawFlashRectangle) ||
                    ((dwFlags & SWS_WINDOWSWITCHER_PAINTFLAGS_ACTIVEMASKORINDEXCHANGED))
                    ) &&
                _this->cwIndex >= 0 &&
                _this->cwIndex < _this->layout.pWindowList.cbSize
                )
            {
                _sws_WindowSwitcher_DrawContour(
                    _this,
                    hdcPaint,
                    pWindowList[_this->cwIndex].rcWindow,
                    SWS_WINDOWSWITCHER_CONTOUR_SIZE,
                    rgbFinal
                );
            }

            // Draw title
            if ((pWindowList && _this->cwIndex == -1 &&
                gridX == selGridX && gridY == selGridY) ||
                ((pWindowList && i == _this->cwIndex) &&
                ((dwFlags & SWS_WINDOWSWITCHER_PAINTFLAGS_REDRAWENTIRE) ||
                    ((dwFlags & SWS_WINDOWSWITCHER_PAINTFLAGS_ACTIVEMASKORINDEXCHANGED))))
                )
            {
                WCHAR wszTitle[MAX_PATH];
                memset(wszTitle, 0, MAX_PATH * sizeof(wchar_t));
                if (_this->layout.bIncludeWallpaper && pWindowList[i].hWnd == _this->layout.hWndWallpaper)
                {
                    sws_WindowHelpers_GetDesktopText(wszTitle);
                }
                else
                {
                    sws_WindowHelpers_GetWindowText(pWindowList[i].hWnd, wszTitle, MAX_PATH);
                }
                SetBkMode(hdcPaint, OPAQUE);
                SetBkColor(hdcPaint, GetSysColor(COLOR_BTNFACE));
                DrawTextW(
                    hdcPaint,
                    wszTitle,
                    -1,
                    &rcTitleArea,
                    DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_HIDEPREFIX
                );
            }

            // Draw icon
            if (pWindowList &&
                bIsWindowVisible &&
                ((dwFlags & SWS_WINDOWSWITCHER_PAINTFLAGS_REDRAWENTIRE) ||
                    ((dwFlags & SWS_WINDOWSWITCHER_PAINTFLAGS_ISFLASHANIMATION) && bShouldDrawFlashRectangle) ||
                    ((dwFlags & SWS_WINDOWSWITCHER_PAINTFLAGS_ACTIVEMASKORINDEXCHANGED) && (i == _this->cwIndex || i == _this->cwOldIndex))
                    ) &&
                pWindowList[i].hIcon &&
                pWindowList[i].iRowMax
                )
            {
                rc = pWindowList[i].rcWindow;
                INT x = rc.left + 5 * (_this->layout.cbDpiX / DEFAULT_DPI_X);
                INT y = rc.top + 5 * (_this->layout.cbDpiY / DEFAULT_DPI_Y);
                INT w = pWindowList[i].rcIcon.right;
                INT h = pWindowList[i].rcIcon.bottom;
                if (pWindowList[i].dwWindowFlags & SWS_WINDOWSWITCHERLAYOUT_WINDOWFLAGS_ISUWP)
                {
                    x = rc.left + 4 * (_this->layout.cbDpiX / DEFAULT_DPI_X);
                    y = rc.top + 3 * (_this->layout.cbDpiY / DEFAULT_DPI_Y);
                }
                //Wh_Log(L"i gridX gridY x y w h: %d %d %d %d %d %d %d\n", i, gridX, gridY, x, y, w, h);
                RGBQUAD bkcol2 = rgbFinal;
                // I don't understand why this is necessary, but otherwise icons
                // obtained from the file system have a black plate as background
                if (bkcol2.rgbReserved == 255) bkcol2.rgbReserved = 254;
                sws_IconPainter_DrawIcon(
                    pWindowList[i].hIcon,
                    hdcPaint,
                    (pWindowList[i].tshWnd && pWindowList[i].tshWnd->bFlash) ? _this->hFlashBrush : _this->hBackgroundBrush,
                    pGdipGraphics,
                    x, y, w, h,
                    bkcol2,
                    TRUE
                );
            }
        }
        if (duplicateCount > 1)
        {
            Wh_Log(L"[sws] DETECTED %d HIGHLIGHTED ITEMS\n", duplicateCount);
        }
        if (pGdipGraphics)
        {
            Gdiplus::DllExports::GdipDeleteGraphics(pGdipGraphics);
        }

        BOOL bShouldDisableFlashAnimationTimer = TRUE;
        for (int i = 0; i < _this->layout.pWindowList.cbSize; ++i)
        {
            sws_tshwnd* tshWnd = ((_this->bSwitcherIsPerApplication && _this->mode == SWS_WINDOWSWITCHER_LAYOUTMODE_FULL) ? pWindowList[i].last_flashing_tshwnd : pWindowList[i].tshWnd);

            if (pWindowList && tshWnd)
            {
                if (sws_tshwnd_GetFlashState(tshWnd))
                {
                    if (tshWnd->dwFlashAnimationState != SWS_WINDOWSWITCHER_ANIMATOR_FLASH_MAXSTATE)
                    {
                        bShouldDisableFlashAnimationTimer = FALSE;
                    }
                }
                else
                {
                    if (tshWnd->cbFlashAnimationState != 0.0)
                    {
                        bShouldDisableFlashAnimationTimer = FALSE;
                    }
                }
            }
        }
        if (bShouldDisableFlashAnimationTimer)
        {
            ResetEvent(_this->hFlashAnimationSignal);
        }

        if (bIsWindowVisible)
        {
            BitBlt(hDC, 0, 0, siz.cx, siz.cy, hdcPaint, 0, 0, SRCCOPY);
        }

        long long a1 = sws_milliseconds_now();
        //Wh_Log(L"[sws] WindowSwitcher::Paint [[ %lld ]]\n", a1 - _this->lastUpdateTime);
        _this->lastUpdateTime = a1;
    }

    EndPaint(hWnd, &ps);
}

static void WINAPI _sws_WindowSwitcher_Show(sws_WindowSwitcher* _this)
{
    long long a1 = sws_milliseconds_now();
    if (_this->dwWallpaperSupport == SWS_WALLPAPERSUPPORT_EXPLORER)
    {
        LONG_PTR atom = 0;
        RECT rc;
        SetRect(&rc, 0, 0, 0, 0);
        BOOL bIsWindowWallpaperWindow = IsWindow(_this->hWndWallpaper);
        if (bIsWindowWallpaperWindow)
        {
            GetWindowRect(_this->hWndWallpaper, &rc);
            atom = GetClassWord(_this->hWndWallpaper, GCW_ATOM);
        }
        if (!bIsWindowWallpaperWindow || rc.right - rc.left == 0 || rc.bottom - rc.top == 0 || atom != RegisterWindowMessageW(L"WorkerW"))
        {
            //Wh_Log(L"[sws] Invalid wallpaper window detected, reobtaining correct window.\n");
            _this->hWndWallpaper = NULL;
            if (sws_WindowHelpers_EnsureWallpaperHWND())
            {
                _this->hWndWallpaper = sws_WindowHelpers_GetWallpaperHWND();
            }
            else
            {
                _this->dwWallpaperSupport = SWS_WALLPAPERSUPPORT_NONE;
            }
        }
    }
    if (_this->hLastClosedWnds)
    {
        DPA_Destroy(_this->hLastClosedWnds);
        _this->hLastClosedWnds = NULL;
    }
    POINT pt;
    if (_this->bPrimaryOnly)
    {
        pt.x = 0;
        pt.y = 0;
    }
    else
    {
        GetCursorPos(&pt);
    }
    BOOL bIsMonitorValid = FALSE;
    if (_this->hMonitor && IsWindowVisible(_this->hWnd))
    {
        HMONITOR hSeekedMonitor = _this->hMonitor;
        EnumDisplayMonitors(NULL, NULL, sws_WindowHelpers_IsValidMonitor, (LPARAM)&hSeekedMonitor);
        if (!hSeekedMonitor) bIsMonitorValid = TRUE;
    }
    if (!_this->hMonitor || !bIsMonitorValid) _this->hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    sws_WindowSwitcherLayoutWindow* pWindowList = (sws_WindowSwitcherLayoutWindow*)_this->layout.pWindowList.pList;
    DWORD cntOldHWNDSs = _this->layout.pWindowList.cbSize;
    HWND* pOldHWNDs = NULL;
    DWORD dwOldIndex = _this->layout.iIndex;
    if (cntOldHWNDSs)
    {
        pOldHWNDs = (HWND*)calloc(cntOldHWNDSs, sizeof(HWND));
        if (pOldHWNDs)
        {
            for (int i = 0; i < _this->layout.pWindowList.cbSize; ++i)
            {
                pOldHWNDs[i] = pWindowList[i].hWnd;
            }
        }
    }
    sws_WindowSwitcherLayout_Clear(&(_this->layout));
    sws_vector_Clear(&(_this->pHWNDList));
    sws_vector_Initialize(&(_this->pHWNDList), sizeof(sws_window));
    HDPA hdpa = DPA_Create(SWS_VECTOR_CAPACITY);
    EnumWindows(sws_WindowHelpers_AddAltTabWindowsToTimeStampedHWNDList, (LPARAM)hdpa);
    long long a2 = sws_milliseconds_now();
    for (int i = 0; i < DPA_GetPtrCount(hdpa); ++i)
    {
        sws_tshwnd* tshWnd = (sws_tshwnd*)DPA_FastGetPtr(hdpa, i);
        int rv = DPA_Search(_this->htshwnds, tshWnd, 0, (PFNDACOMPARE)sws_tshwnd_CompareHWND, 0, 0);
        if (rv != -1)
        {
            sws_tshwnd* found_tshwnd = (sws_tshwnd*)DPA_FastGetPtr(_this->htshwnds, rv);
            sws_tshwnd_ModifyTimestamp(tshWnd, found_tshwnd->ft);
        }
        else tshWnd->hWnd = (HWND)INVALID_HANDLE_VALUE;
    }
    long long a3 = sws_milliseconds_now();
    DPA_Sort(hdpa, (PFNDACOMPARE)sws_tshwnd_CompareTimestamp, SWS_SORT_DESCENDING);
    long long a4 = sws_milliseconds_now();
    for (int i = 0; i < DPA_GetPtrCount(hdpa); ++i)
    {
        sws_tshwnd* tshWnd = (sws_tshwnd*)DPA_FastGetPtr(hdpa, i);
        if (tshWnd->hWnd != INVALID_HANDLE_VALUE)
        {
            sws_window window;
            sws_window_Initialize(&window, tshWnd->hWnd);
            sws_vector_PushBack(&(_this->pHWNDList), &window);
        }
    }
    DPA_DestroyCallback(hdpa, _sws_WindowSwitcher_free_stub, 0);
    long long a5 = sws_milliseconds_now();
    Wh_Log(L"[sws] WindowSwitcher::Show %x [[ %lld + %lld + %lld + %lld = %lld ]]\n", _this->hWndWallpaper, a2 - a1, a3 - a2, a4 - a3, a5 - a4, a5 - a1);
    _sws_WindowSwitcher_Calculate(_this, pOldHWNDs, cntOldHWNDSs, dwOldIndex);
    if (pOldHWNDs) free(pOldHWNDs);
    if (_this->layout.pWindowList.cbSize == 0)
    {
        ShowWindow(_this->hWnd, SW_HIDE);
        return;
    }
    if (_this->layout.pWindowList.cbSize == 1 && _this->bSkipIfOneWindow && _this->mode == SWS_WINDOWSWITCHER_LAYOUTMODE_FULL)
    {
        ShowWindow(_this->hWnd, SW_HIDE);
        if (pWindowList[0].hWnd != _this->hWndWallpaper)
        {
            HWND hLastActivePopup = sws_WindowHelpers_GetLastActivePopup(pWindowList[_this->layout.iIndex].hWnd);
            SwitchToThisWindow(IsWindowVisible(hLastActivePopup) ? hLastActivePopup : pWindowList[_this->layout.iIndex].hWnd, TRUE);
        }
        return;
    }
    _this->layout.iFirstItemIndex = _this->layout.pWindowList.cbSize;
    Wh_Log(L"[sws] cbSize=%d\n", _this->layout.pWindowList.cbSize);
    if (_this->hdcWindow)
    {
        EndBufferedPaint(_this->hBufferedPaint, FALSE);
        ReleaseDC(_this->hWnd, _this->hdcWindow);
        _this->hdcPaint = NULL;
    }
    _this->hdcWindow = GetDC(_this->hWnd);
    if (_this->hdcWindow)
    {
        BP_PAINTPARAMS params;
        ZeroMemory(&params, sizeof(BP_PAINTPARAMS));
        params.cbSize = sizeof(BP_PAINTPARAMS);
        params.dwFlags = BPPF_NOCLIP | BPPF_ERASE;
        RECT rc;
        SetRect(&rc, 0, 0, _this->layout.iWidth, _this->layout.iHeight);
        _this->hBufferedPaint = BeginBufferedPaint(_this->hdcWindow, &rc, BPBF_TOPDOWNDIB, &params, &(_this->hdcPaint));
    }
    pWindowList = (sws_WindowSwitcherLayoutWindow*)_this->layout.pWindowList.pList;
    sws_tshwnd* tshWnd = (sws_tshwnd*)malloc(sizeof(sws_tshwnd));
    if (pWindowList && tshWnd)
    {
        for (int iCurrentWindow = _this->layout.pWindowList.cbSize - 1; iCurrentWindow >= 0; iCurrentWindow--)
        {
            sws_tshwnd_Initialize(tshWnd, pWindowList[iCurrentWindow].hWnd);
            int rv = DPA_Search(_this->htshwnds, tshWnd, 0, (PFNDACOMPARE)sws_tshwnd_CompareHWND, 0, 0);
            if (rv != -1)
            {
                pWindowList[iCurrentWindow].tshWnd = (sws_tshwnd*)DPA_FastGetPtr(_this->htshwnds, rv);
            }
        }
    }
    if (tshWnd)
    {
        free(tshWnd);
    }
    sws_window* pHWNDList = (sws_window*)_this->pHWNDList.pList;
    sws_tshwnd* tshwnd2 = (sws_tshwnd*)malloc(sizeof(sws_tshwnd));
    if (pHWNDList && tshwnd2)
    {
        for (int i = 0; i < _this->pHWNDList.cbSize; ++i)
        {
            sws_tshwnd_Initialize(tshwnd2, pHWNDList[i].hWnd);
            int rv = DPA_Search(_this->htshwnds, tshwnd2, 0, (PFNDACOMPARE)sws_tshwnd_CompareHWND, 0, 0);
            if (rv != -1)
            {
                pHWNDList[i].tshWnd = (sws_tshwnd*)DPA_FastGetPtr(_this->htshwnds, rv);
            }
        }
    }
    if (tshwnd2)
    {
        free(tshwnd2);
    }
    if (!IsWindowVisible(_this->hWnd) && _this->dwShowDelay)
    {
        BOOL bCloak = TRUE;
        DwmSetWindowAttribute(_this->hWnd, DWMWA_CLOAK, &bCloak, sizeof(BOOL));
        SetEvent(_this->hShowSignal);
    }
    else
    {
        BOOL bCloak = FALSE;
        DwmSetWindowAttribute(_this->hWnd, DWMWA_CLOAK, &bCloak, sizeof(BOOL));
    }
    if (_this->bShouldStartFlashTimerWhenShowing)
    {
        _this->bShouldStartFlashTimerWhenShowing = FALSE;
        SetEvent(_this->hFlashAnimationSignal);
    }
    SetWindowPos(_this->hWnd, 0, _this->layout.iX, _this->layout.iY, _this->layout.iWidth, _this->layout.iHeight, SWP_NOZORDER);
    ShowWindow(_this->hWnd, SW_SHOW);
    SetForegroundWindow(_this->hWnd);
    _this->dwPaintFlags |= SWS_WINDOWSWITCHER_PAINTFLAGS_REDRAWENTIRE;
    InvalidateRect(_this->hWnd, NULL, TRUE);
    for (int iCurrentWindow = _this->layout.pWindowList.cbSize - 1; iCurrentWindow >= 0; iCurrentWindow--)
    {
        if (pWindowList[iCurrentWindow].hIcon == sws_DefAppIcon)
        {
            sws_IconPainter_CallbackParams* params = (sws_IconPainter_CallbackParams*)malloc(sizeof(sws_IconPainter_CallbackParams));
            if (params)
            {
                WCHAR wszRundll32Path[MAX_PATH];
                GetSystemDirectoryW(wszRundll32Path, MAX_PATH);
                wcscat_s(wszRundll32Path, MAX_PATH, L"\\rundll32.exe");
                params->bUseApplicationIcon = FALSE;
                if (!_this->bAlwaysUseWindowTitleAndIcon &&
                    !!_wcsicmp(pWindowList[iCurrentWindow].wszPath, wszRundll32Path) &&
                    !(pWindowList[iCurrentWindow].dwWindowFlags & SWS_WINDOWSWITCHERLAYOUT_WINDOWFLAGS_ISUWP) &&
                    _this->bSwitcherIsPerApplication &&
                    _this->mode == SWS_WINDOWSWITCHER_LAYOUTMODE_FULL &&
                    pWindowList[iCurrentWindow].dwCount > 1)
                {
                    params->bUseApplicationIcon = TRUE;
                }
                params->hWnd = _this->hWnd;
                params->index = iCurrentWindow;
                if (!_this->layout.timestamp)
                {
                    _this->layout.timestamp = sws_milliseconds_now();
                }
                params->timestamp = _this->layout.timestamp;
                params->bIsDesktop = (_this->layout.bIncludeWallpaper && pWindowList[iCurrentWindow].hWnd == _this->hWndWallpaper);
                if (!sws_IconPainter_ExtractAndDrawIconAsync(pWindowList[iCurrentWindow].hWnd, params))
                {
                    pWindowList[iCurrentWindow].hIcon = sws_LegacyDefAppIcon;
                    free(params);
                }
            }
        }
    }
    if (!_this->bWasControl)
    {
        SetTimer(_this->hWnd, SWS_WINDOWSWITCHER_TIMER_ASYNCKEYCHECK, SWS_WINDOWSWITCHER_TIMER_ASYNCKEYCHECK_DELAY, NULL);
    }
    _sws_WindowSwitcher_UpdateAccessibleText(_this);
}

static DWORD CALLBACK _sws_WindowSwitcher_EndTaskThreadProc(sws_WindowSwitcher_EndTaskThreadParams* params)
{
    SetThreadDesktop(params->hDesktop);
    if (IsHungAppWindow(params->hWnd))
    {
        sws_tshwnd* tshWnd = (sws_tshwnd*)malloc(sizeof(sws_tshwnd));
        if (tshWnd)
        {
            sws_tshwnd_Initialize(tshWnd, params->hWnd);
            int rv = DPA_Search(params->sws->htshwnds, tshWnd, 0, (PFNDACOMPARE)sws_tshwnd_CompareHWND, 0, 0);
            if (rv != -1)
            {
                EndTask(params->hWnd, FALSE, FALSE);
            }
            free(tshWnd);
        }
    }
    else
    {
        PostMessageW(params->hWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
    }
    free(params);
    return 0;
}

static LRESULT CALLBACK _sws_WindowsSwitcher_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    sws_WindowSwitcher* _this = NULL;
    if (uMsg == WM_CREATE)
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)(lParam);
        _this = (sws_WindowSwitcher*)(pCreate->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)_this);
        SetTimer(hWnd, SWS_WINDOWSWITCHER_TIMER_STARTUP, SWS_WINDOWSWITCHER_TIMER_STARTUP_DELAY, 0);
    }
    else
    {
        LONG_PTR ptr = GetWindowLongPtr(hWnd, GWLP_USERDATA);
        _this = (sws_WindowSwitcher*)(ptr);
    }

    //Wh_Log(L"%d %d %d\n", uMsg, wParam, lParam);
    if (uMsg == WM_TIMER && wParam == SWS_WINDOWSWITCHER_TIMER_ASYNCKEYCHECK)
    {
        if (!_this->bWasControl && !(GetAsyncKeyState(VK_MENU) & 0x8000))
        {
            _sws_WindowSwitcher_SwitchToSelectedItemAndDismiss(_this);
            KillTimer(hWnd, SWS_WINDOWSWITCHER_TIMER_ASYNCKEYCHECK);
            return 0;
        }
    }
    else if (uMsg == WM_TIMER && wParam == SWS_WINDOWSWITCHER_TIMER_UPDATEACCESSIBLETEXT)
    {
        _sws_WindowSwitcher_UpdateAccessibleText(_this);
        KillTimer(_this->hWnd, SWS_WINDOWSWITCHER_TIMER_UPDATEACCESSIBLETEXT);
    }
    else if (uMsg == WM_TIMER && wParam == SWS_WINDOWSWITCHER_TIMER_PAINT)
    {
        SendMessageW(_this->hWnd, SWS_WINDOWSWITCHER_PAINT_MSG, SWS_WINDOWSWITCHER_PAINTFLAGS_REDRAWENTIRE, 0);
        KillTimer(_this->hWnd, SWS_WINDOWSWITCHER_TIMER_PAINT);
    }
    else if (uMsg == WM_TIMER && wParam == SWS_WINDOWSWITCHER_TIMER_CLOSEHWND)
    {
        sws_tshwnd* tshWnd = (sws_tshwnd*)malloc(sizeof(sws_tshwnd));
        if (tshWnd)
        {
            if (_this->hLastClosedWnds)
            {
                for (int j = 0; j < DPA_GetPtrCount(_this->hLastClosedWnds); ++j)
                {
                    HWND hLastClosedWnd = (HWND)DPA_FastGetPtr(_this->hLastClosedWnds, j);
                    if (hLastClosedWnd)
                    {
                        sws_tshwnd_Initialize(tshWnd, hLastClosedWnd);
                        int rv = DPA_Search(_this->htshwnds, tshWnd, 0, (PFNDACOMPARE)sws_tshwnd_CompareHWND, 0, 0);
                        if (rv != -1)
                        {
                            sws_tshwnd* found = (sws_tshwnd*)DPA_FastGetPtr(_this->htshwnds, rv);
                            if (!found->bFlash)
                            {
                                if (sws_WindowHelpers_GetLastActivePopup(hLastClosedWnd) != hLastClosedWnd)
                                {
                                    sws_WindowSwitcherLayoutWindow* pWindowList = (sws_WindowSwitcherLayoutWindow*)_this->layout.pWindowList.pList;
                                    if (pWindowList)
                                    {
                                        for (int i = 0; i < _this->layout.pWindowList.cbSize; ++i)
                                        {
                                            if (hLastClosedWnd == pWindowList[i].hWnd)
                                            {
                                                _this->layout.iIndex = i;
                                                _sws_WindowSwitcher_SwitchToSelectedItemAndDismiss(_this);
                                                DPA_SetPtr(_this->hLastClosedWnds, j, NULL);
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            free(tshWnd);
        }
        KillTimer(_this->hWnd, SWS_WINDOWSWITCHER_TIMER_CLOSEHWND);
    }
    else if (uMsg == SWS_WINDOWSWITCHER_PAINT_MSG)
    {
        _this->dwPaintFlags |= wParam;
        RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);
    }
    else if (uMsg == SWS_WINDOWSWITCHER_RELOAD_CONFIG_MSG)
    {
        sws_WindowSwitcher_LoadSettings(_this);
    }
    else if (uMsg == WM_ERASEBKGND)
    {
        return 0;
    }
    else if (_this && uMsg == _this->msgShellHook && lParam)
    {
        if (wParam == HSHELL_WINDOWCREATED || wParam == HSHELL_WINDOWACTIVATED || wParam == HSHELL_RUDEAPPACTIVATED || wParam == HSHELL_FLASH || wParam == HSHELL_REDRAW)
        {
            sws_tshwnd* tshWnd;
            for (int i = 0; i < 2; ++i)
            {
                HWND hWnd = (HWND)lParam;
                if (!i)
                {
                    HWND hOwner = GetWindow(hWnd, GW_OWNER);
                    if (hOwner)
                    {
                        hWnd = hOwner;
                    }
                    else
                    {
                        continue;
                    }
                }
                tshWnd = (sws_tshwnd*)malloc(sizeof(sws_tshwnd));
                if (tshWnd)
                {
                    sws_tshwnd_Initialize(tshWnd, hWnd);
                    int rv = DPA_Search(_this->htshwnds, tshWnd, 0, (PFNDACOMPARE)sws_tshwnd_CompareHWND, 0, 0);
                    if (rv == -1)
                    {
                        // If this window is not in the window list and is not the foreground window, 
                        // make sure it will be last in the window list when the switcher will be presented
                        // https://github.com/valinet/ExplorerPatcher/issues/1084
                        if (hWnd != GetForegroundWindow())
                        {
                            sws_tshwnd_ModifyTimestamp(tshWnd, sws_WindowHelpers_GetAncientTime());
                        }
                        DPA_InsertPtr(_this->htshwnds, 0, tshWnd);
                        sws_WindowSwitcherLayoutWindow* pWindowList = (sws_WindowSwitcherLayoutWindow*)_this->layout.pWindowList.pList;
                        if (pWindowList)
                        {
                            for (int i = 0; i < _this->layout.pWindowList.cbSize; ++i)
                            {
                                if (hWnd == pWindowList[i].hWnd)
                                {
                                    pWindowList[i].tshWnd = tshWnd;
                                }
                            }
                        }
                        sws_window* pHWNDList = (sws_window*)_this->pHWNDList.pList;
                        if (pHWNDList)
                        {
                            for (int i = 0; i < _this->pHWNDList.cbSize; ++i)
                            {
                                if (hWnd == pHWNDList[i].hWnd)
                                {
                                    pHWNDList[i].tshWnd = tshWnd;
                                }
                            }
                        }
                    }
                    else
                    {
                        free(tshWnd);
                        // Update flash status and have the window pop at the front of the list only
                        // when the window is the foreground window; otherwise, the OS (probably) denied
                        // the foreground request from the app and the window might still be flashing
                        // and not actually in the foreground
                        // https://github.com/valinet/ExplorerPatcher/issues/1084
                        if ((wParam == HSHELL_WINDOWCREATED || wParam == HSHELL_WINDOWACTIVATED || wParam == HSHELL_RUDEAPPACTIVATED) && (hWnd == GetForegroundWindow() || sws_WindowHelpers_GetLastActivePopup(hWnd) == GetForegroundWindow()))
                        {
                            sws_tshwnd* found = (sws_tshwnd*)DPA_FastGetPtr(_this->htshwnds, rv);
                            sws_tshwnd_UpdateTimestamp(found);
                            sws_tshwnd_SetFlashState(found, FALSE);
                            found->dwFlashAnimationState = 0;
                            found->cbFlashAnimationState = 0.0;
                        }
                    }
                }
            }
        }
        if (wParam == HSHELL_WINDOWDESTROYED)
        {
            sws_tshwnd* tshWnd = (sws_tshwnd*)malloc(sizeof(sws_tshwnd));
            if (tshWnd)
            {
                sws_tshwnd_Initialize(tshWnd, (HWND)lParam);
                int rv = DPA_Search(_this->htshwnds, tshWnd, 0, (PFNDACOMPARE)sws_tshwnd_CompareHWND, 0, 0);
                if (rv != -1)
                {
                    sws_WindowSwitcherLayoutWindow* pWindowList = (sws_WindowSwitcherLayoutWindow*)_this->layout.pWindowList.pList;
                    if (pWindowList)
                    {
                        for (int i = 0; i < _this->layout.pWindowList.cbSize; ++i)
                        {
                            if (pWindowList[i].tshWnd == DPA_FastGetPtr(_this->htshwnds, rv))
                            {
                                pWindowList[i].tshWnd = NULL;
                            }
                            if (pWindowList[i].last_flashing_tshwnd == DPA_FastGetPtr(_this->htshwnds, rv))
                            {
                                pWindowList[i].last_flashing_tshwnd = NULL;
                            }
                        }
                    }
                    sws_window* pHWNDList = (sws_window*)_this->pHWNDList.pList;
                    if (pHWNDList)
                    {
                        for (int i = 0; i < _this->pHWNDList.cbSize; ++i)
                        {
                            if (pHWNDList[i].tshWnd == DPA_FastGetPtr(_this->htshwnds, rv))
                            {
                                pHWNDList[i].tshWnd = NULL;
                            }
                        }
                    }

                    free(DPA_FastGetPtr(_this->htshwnds, rv));
                    DPA_DeletePtr(_this->htshwnds, rv);
                }
                free(tshWnd);
            }
            if (IsWindowVisible(_this->hWnd))
            {
                sws_window* pHWNDList = (sws_window*)_this->pHWNDList.pList;
                int bContains = -1;
                for (int i = 0; i < _this->pHWNDList.cbSize; ++i)
                {
                    if (pHWNDList[i].hWnd == (HWND)lParam)
                    {
                        bContains = i;
                        break;
                    }
                }
                if (bContains != -1)
                {
                    _sws_WindowSwitcher_Show(_this);
                }
            }
        }
        if (wParam == HSHELL_FLASH)
        {
            sws_tshwnd* tshWnd = (sws_tshwnd*)malloc(sizeof(sws_tshwnd));
            if (tshWnd)
            {
                sws_tshwnd_Initialize(tshWnd, (HWND)lParam);
                int rv = DPA_Search(_this->htshwnds, tshWnd, 0, (PFNDACOMPARE)sws_tshwnd_CompareHWND, 0, 0);
                if (rv != -1)
                {
                    sws_tshwnd* found = (sws_tshwnd*)DPA_FastGetPtr(_this->htshwnds, rv);
                    if (!sws_tshwnd_GetFlashState(found))
                    {
                        sws_tshwnd_SetFlashState(found, TRUE);
                        if (IsWindowVisible(_this->hWnd))
                        {
                            SetEvent(_this->hFlashAnimationSignal);
                        }
                        else
                        {
                            _this->bShouldStartFlashTimerWhenShowing = TRUE;
                        }
                    }
                }
                free(tshWnd);
            }
            for (int i = 0; i < 2; ++i)
            {
                HWND hWnd = (HWND)lParam;
                if (!i)
                {
                    HWND hOwner = GetWindow(hWnd, GW_OWNER);
                    if (hOwner)
                    {
                        hWnd = hOwner;
                    }
                    else
                    {
                        continue;
                    }
                }

                if (_this->hLastClosedWnds)
                {
                    for (int j = 0; j < DPA_GetPtrCount(_this->hLastClosedWnds); ++j)
                    {
                        HWND hLastClosedWnd = (HWND)DPA_FastGetPtr(_this->hLastClosedWnds, j);
                        if (hLastClosedWnd && hLastClosedWnd == hWnd)
                        {
                            ShowWindow(_this->hWnd, SW_HIDE);
                            SwitchToThisWindow(sws_WindowHelpers_GetLastActivePopup(hLastClosedWnd), TRUE);
                        }
                    }
                }
            }
        }
        if (wParam == HSHELL_REDRAW)
        {
            for (int i = 0; i < 2; ++i)
            {
                HWND hWnd = (HWND)lParam;
                if (!i)
                {
                    HWND hOwner = GetWindow(hWnd, GW_OWNER);
                    if (hOwner)
                    {
                        hWnd = hOwner;
                    }
                    else
                    {
                        continue;
                    }
                }

                BOOL bWasForFlashingOff = FALSE;
                sws_tshwnd* tshWnd = (sws_tshwnd*)malloc(sizeof(sws_tshwnd));
                if (tshWnd)
                {
                    sws_tshwnd_Initialize(tshWnd, hWnd);
                    int rv = DPA_Search(_this->htshwnds, tshWnd, 0, (PFNDACOMPARE)sws_tshwnd_CompareHWND, 0, 0);
                    if (rv != -1)
                    {
                        sws_tshwnd* found = (sws_tshwnd*)DPA_FastGetPtr(_this->htshwnds, rv);
                        if (sws_tshwnd_GetFlashState(found))
                        {
                            sws_tshwnd_SetFlashState(found, FALSE);
                            if (IsWindowVisible(_this->hWnd))
                            {
                                SetEvent(_this->hFlashAnimationSignal);
                            }
                            else
                            {
                                _this->bShouldStartFlashTimerWhenShowing = TRUE;
                            }
                            found->dwFlashAnimationState = 0;
                            found->cbFlashAnimationState = 1.0;
                        }
                    }
                    free(tshWnd);
                }

                if (!bWasForFlashingOff)
                {
                    if (IsWindowVisible(_this->hWnd))
                    {
                        sws_WindowSwitcherLayoutWindow* pWindowList = (sws_WindowSwitcherLayoutWindow*)_this->layout.pWindowList.pList;
                        if (pWindowList)
                        {
                            for (int i = 0; i < _this->layout.pWindowList.cbSize; ++i)
                            {
                                if (hWnd == pWindowList[i].hWnd)
                                {
                                    sws_IconPainter_CallbackParams* params = (sws_IconPainter_CallbackParams*)malloc(sizeof(sws_IconPainter_CallbackParams));
                                    if (params)
                                    {
                                        WCHAR wszRundll32Path[MAX_PATH];
                                        GetSystemDirectoryW(wszRundll32Path, MAX_PATH);
                                        wcscat_s(wszRundll32Path, MAX_PATH, L"\\rundll32.exe");
                                        params->bUseApplicationIcon = FALSE;
                                        if (!_this->bAlwaysUseWindowTitleAndIcon &&
                                            _wcsicmp(pWindowList[i].wszPath, wszRundll32Path) &&
                                            !(pWindowList[i].dwWindowFlags & SWS_WINDOWSWITCHERLAYOUT_WINDOWFLAGS_ISUWP) &&
                                            _this->bSwitcherIsPerApplication &&
                                            _this->mode == SWS_WINDOWSWITCHER_LAYOUTMODE_FULL &&
                                            pWindowList[i].dwCount > 1)
                                        {
                                            params->bUseApplicationIcon = TRUE;
                                        }
                                        if (!params->bUseApplicationIcon)
                                        {
                                            params->hWnd = _this->hWnd;
                                            params->index = i;
                                            if (!_this->layout.timestamp)
                                            {
                                                _this->layout.timestamp = sws_milliseconds_now();
                                            }
                                            params->timestamp = _this->layout.timestamp;
                                            params->bIsDesktop = (_this->layout.bIncludeWallpaper && pWindowList[i].hWnd == _this->hWndWallpaper);
                                            if (!sws_IconPainter_ExtractAndDrawIconAsync(pWindowList[i].hWnd, params))
                                            {
                                                pWindowList[i].hIcon = sws_LegacyDefAppIcon;
                                                free(params);
                                                SendMessageW(_this->hWnd, SWS_WINDOWSWITCHER_PAINT_MSG, SWS_WINDOWSWITCHER_PAINTFLAGS_REDRAWENTIRE, 0);
                                            }
                                        }
                                        else
                                        {
                                            free(params);
                                            SendMessageW(_this->hWnd, SWS_WINDOWSWITCHER_PAINT_MSG, SWS_WINDOWSWITCHER_PAINTFLAGS_REDRAWENTIRE, 0);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if (wParam == HSHELL_WINDOWCREATED || wParam == HSHELL_WINDOWACTIVATED || wParam == HSHELL_RUDEAPPACTIVATED)
        {
            if (IsWindowVisible(_this->hWnd) && (HWND)lParam != _this->hWnd && sws_WindowHelpers_IsAltTabWindow((HWND)lParam))
            {
                HDPA hdpa = DPA_Create(SWS_VECTOR_CAPACITY);
                EnumWindows(sws_WindowHelpers_AddAltTabWindowsToTimeStampedHWNDList, (LPARAM)hdpa);
                sws_WindowSwitcherLayoutWindow* pWindowList = (sws_WindowSwitcherLayoutWindow*)_this->layout.pWindowList.pList;
                if (pWindowList)
                {
                    for (int i = 0; i < _this->layout.pWindowList.cbSize; ++i)
                    {
                        for (int j = 0; j < DPA_GetPtrCount(hdpa); ++j)
                        {
                            sws_tshwnd* tshWnd = (sws_tshwnd*)DPA_FastGetPtr(hdpa, j);
                            if (tshWnd->hWnd == pWindowList[i].hWnd)
                            {
                                tshWnd->hWnd = NULL;
                            }
                        }
                    }
                    BOOL bShouldShow = FALSE;
                    for (int j = 0; j < DPA_GetPtrCount(hdpa); ++j)
                    {
                        sws_tshwnd* tshWnd = (sws_tshwnd*)DPA_FastGetPtr(hdpa, j);
                        if (tshWnd->hWnd)
                        {
                            bShouldShow = TRUE;
                        }
                        free(tshWnd);
                    }
                    if (bShouldShow)
                    {
                        _sws_WindowSwitcher_Show(_this);
                    }
                    DPA_Destroy(hdpa);
                }
            }
        }
    }
    else if (uMsg == WM_CLOSE)
    {
        DestroyWindow(hWnd);
        return 0;
    }
    else if (uMsg == WM_DESTROY)
    {
        PostQuitMessage(0);
        SetEvent(_this->hEvExit);
        return 0;
    }
    else if (0 && uMsg == WM_NCHITTEST)
    {
        return HTCAPTION;
    }
    else if (uMsg == WM_SHOWWINDOW)
    {
        if (wParam == FALSE)
        {
            KillTimer(_this->hWnd, SWS_WINDOWSWITCHER_TIMER_CLOSEHWND);
            KillTimer(hWnd, SWS_WINDOWSWITCHER_TIMER_ASYNCKEYCHECK);
            _this->lastMiniModehWnd = NULL;
            if (_this->dwOriginalScrollWheelBehavior == SWS_SCROLLWHEELBEHAVIOR_EVERYWHERE ||
                _this->dwOriginalScrollWheelBehavior == SWS_SCROLLWHEELBEHAVIOR_EVERYWHERE_IFCLIENTAREA_GRIDSCROLL ||
                _this->dwOriginalScrollWheelBehavior == SWS_SCROLLWHEELBEHAVIOR_EVERYWHERE_GRIDSCROLL ||
				_this->dwOriginalScrollWheelBehavior == SWS_SCROLLWHEELBEHAVIOR_EVERYWHERE_IFNOTCLIENTAREA_GRIDSCROLL
                )
            {
                SystemParametersInfoW(SPI_SETMOUSEWHEELROUTING, 0, reinterpret_cast<PVOID>(static_cast<ULONG_PTR>(_this->dwOriginalMouseRouting)), 0);
            }
            _this->dwOriginalMouseRouting = -1;
            _this->dwOriginalScrollWheelBehavior = SWS_SCROLLWHEELBEHAVIOR_DISABLED;
        }
        else
        {
            _this->dwOriginalScrollWheelBehavior = _this->dwScrollWheelBehavior;
            if (_this->dwOriginalScrollWheelBehavior == SWS_SCROLLWHEELBEHAVIOR_EVERYWHERE ||
                _this->dwOriginalScrollWheelBehavior == SWS_SCROLLWHEELBEHAVIOR_EVERYWHERE_IFCLIENTAREA_GRIDSCROLL ||
                _this->dwOriginalScrollWheelBehavior == SWS_SCROLLWHEELBEHAVIOR_EVERYWHERE_GRIDSCROLL ||
                _this->dwOriginalScrollWheelBehavior == SWS_SCROLLWHEELBEHAVIOR_EVERYWHERE_IFNOTCLIENTAREA_GRIDSCROLL
                )
            {
                DWORD dwOriginalMouseRouting = -1;
                if (SystemParametersInfoW(SPI_GETMOUSEWHEELROUTING, 0, reinterpret_cast<PVOID>(static_cast<ULONG_PTR>(dwOriginalMouseRouting)), 0)) _this->dwOriginalMouseRouting = dwOriginalMouseRouting;
                if (dwOriginalMouseRouting != DWORD(-1) && dwOriginalMouseRouting != MOUSEWHEEL_ROUTING_FOCUS)
                {
                    if (!SystemParametersInfoW(SPI_SETMOUSEWHEELROUTING, 0, MOUSEWHEEL_ROUTING_FOCUS, 0)) _this->dwOriginalMouseRouting = -1;
                }
                else _this->dwOriginalMouseRouting = -1;
            }
            else _this->dwOriginalMouseRouting = -1;
        }
        return 0;
    }
    else if (uMsg == WM_PAINT)
    {
        sws_WindowSwitcher_Paint(_this, _this->dwPaintFlags);
        _this->dwPaintFlags = SWS_WINDOWSWITCHER_PAINTFLAGS_NONE;
        return 0;
    }
    else if (uMsg == WM_MOUSEMOVE)
    {
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(TRACKMOUSEEVENT);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = hWnd;
        TrackMouseEvent(&tme);

        _this->bIsCursorOnSwitcher = TRUE;

        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        sws_WindowSwitcherLayoutWindow* pWindowList = (sws_WindowSwitcherLayoutWindow*)_this->layout.pWindowList.pList;
        if (pWindowList)
        {
            INT cwIndex = -1;
            DWORD cwMask = 0;
            for (int i = 0; i < _this->layout.pWindowList.cbSize; ++i)
            {
                sws_WindowSwitcherLayoutWindow window = pWindowList[i];
                RECT rc = window.rcWindow;

                if (window.gridX >= _this->dwGridColumns || window.gridY >= _this->dwGridRows)
                {
                    continue;
                }

                if (x > rc.left && x < rc.right && y > rc.top && y < rc.bottom)
                {
                    cwMask |= SWS_WINDOWFLAG_IS_ON_WINDOW;
                    cwIndex = i;
                }
            }
            if (_this->cwMask != cwMask || _this->cwIndex != cwIndex)
            {
                _this->cwOldIndex = _this->cwIndex;
                _this->cwOldMask = _this->cwIndex;
                _this->cwMask = cwMask;
                _this->cwIndex = cwIndex;
                _this->dwPaintFlags |= SWS_WINDOWSWITCHER_PAINTFLAGS_REDRAWENTIRE;
                RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);
            }
        }
        return 0;
    }
    else if (uMsg == WM_MOUSELEAVE)
    {
        _this->bIsCursorOnSwitcher = FALSE;
        if (_this->cwMask != 0)
        {
            _this->cwOldIndex = _this->cwIndex;
            _this->cwOldMask = _this->cwMask;
            _this->cwMask = 0;
            _this->cwIndex = -1;
            _this->dwPaintFlags |= SWS_WINDOWSWITCHER_PAINTFLAGS_REDRAWENTIRE;
            RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);
        }
    }
    else if (uMsg == WM_LBUTTONDOWN || uMsg == WM_MBUTTONDOWN)
    {
        _this->bIsMouseClicking = TRUE;
        return 0;
    }
    else if (uMsg == WM_LBUTTONUP || uMsg == WM_MBUTTONUP || ((uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) && wParam == VK_DELETE))
    {
        if (uMsg == WM_LBUTTONUP || uMsg == WM_MBUTTONUP)
        {
            if (!_this->bIsMouseClicking) return 0;
            else _this->bIsMouseClicking = FALSE;
        }
        sws_WindowSwitcherLayoutWindow* pWindowList = (sws_WindowSwitcherLayoutWindow*)_this->layout.pWindowList.pList;
        if (!pWindowList) return 0;
        int i = 0;
        BOOL bShouldClose = FALSE;
        if ((uMsg == WM_LBUTTONUP || uMsg == WM_MBUTTONUP) ?
            (
                i = _this->cwIndex,
                bShouldClose = uMsg == WM_MBUTTONUP,
                i >= 0 && i < _this->layout.pWindowList.cbSize &&
                GET_X_LPARAM(lParam) > pWindowList[i].rcWindow.left &&
                GET_X_LPARAM(lParam) < pWindowList[i].rcWindow.right &&
                GET_Y_LPARAM(lParam) > pWindowList[i].rcWindow.top &&
                GET_Y_LPARAM(lParam) < pWindowList[i].rcWindow.bottom
                ) : (
                    i = _this->layout.iIndex,
                    bShouldClose = TRUE,
                    i >= 0 && i < _this->layout.pWindowList.cbSize
                    ) &&
            (bShouldClose ? !(_this->layout.bIncludeWallpaper && pWindowList[i].hWnd == _this->layout.hWndWallpaper) : TRUE)
            )
        {
            if (!bShouldClose)
            {
                _this->layout.iIndex = i;
                _sws_WindowSwitcher_SwitchToSelectedItemAndDismiss(_this);
                return 0;
            }
            for (int j = 0; j < DPA_GetPtrCount(pWindowList[i].dpaGroupedWnds/*_this->hLastClosedWnds*/); ++j)
            {
                HWND hWnd = (HWND)DPA_FastGetPtr(pWindowList[i].dpaGroupedWnds/*_this->hLastClosedWnds*/, j);
                if (IsHungAppWindow(hWnd))
                {
                    ShowWindow(_this->hWnd, SW_HIDE);
                    sws_WindowSwitcher_EndTaskThreadParams* pEndTaskParams = (sws_WindowSwitcher_EndTaskThreadParams*)malloc(sizeof(sws_WindowSwitcher_EndTaskThreadParams));
                    if (pEndTaskParams)
                    {
                        pEndTaskParams->hWnd = hWnd;
                        pEndTaskParams->sws = _this;
                        pEndTaskParams->hDesktop = GetThreadDesktop(GetCurrentThreadId());
                        if (!SHCreateThread((LPTHREAD_START_ROUTINE)_sws_WindowSwitcher_EndTaskThreadProc, pEndTaskParams, CTF_NOADDREFLIB, NULL))
                        {
                            free(pEndTaskParams);
                            EndTask(hWnd, FALSE, FALSE);
                        }
                    }
                }
                else
                {
                    PostMessageW(hWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
                }
            }
            //SetTimer(_this->hWnd, SWS_WINDOWSWITCHER_TIMER_CLOSEHWND, SWS_WINDOWSWITCHER_TIMER_CLOSEHWND_DELAY, NULL);
        }
        return 0;
    }
    else if ((uMsg == WM_KEYUP && wParam == VK_ESCAPE) || uMsg == WM_KILLFOCUS || (uMsg == WM_ACTIVATE && wParam == WA_INACTIVE))
    {
        _this->bWasControl = FALSE;
        ShowWindow(_this->hWnd, SW_HIDE);
        return 0;
    }
    else if ((uMsg == WM_KEYUP && wParam == VK_MENU && !_this->bWasControl) ||
        ((uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP) && wParam == VK_SPACE) ||
        ((uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP) && wParam == VK_RETURN) ||
        (uMsg == WM_KEYUP && wParam == (_this->mode == SWS_WINDOWSWITCHER_LAYOUTMODE_MINI ? _this->vkTilde : VK_TAB) && !(GetKeyState(VK_MENU) & 0x8000) && !_this->bWasControl))
    {
        _sws_WindowSwitcher_SwitchToSelectedItemAndDismiss(_this);
        return 0;
    }
    else if (uMsg == WM_HOTKEY || uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN || (uMsg == WM_MOUSEWHEEL && _this && _this->dwOriginalScrollWheelBehavior != SWS_SCROLLWHEELBEHAVIOR_DISABLED))
    {
        if (uMsg == WM_HOTKEY && (LOWORD(lParam) & MOD_CONTROL))
        {
            _this->bWasControl = TRUE;
        }
        if (((uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) && wParam == _this->vkTilde) ||
            ((uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) && wParam == VK_TAB) ||
            (uMsg == WM_HOTKEY && (LOWORD(lParam) & MOD_ALT)) ||
            ((uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) && wParam == VK_LEFT) ||
            ((uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) && wParam == VK_RIGHT) ||
            ((uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) && wParam == VK_UP) ||
            ((uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) && wParam == VK_DOWN) ||
            (uMsg == WM_MOUSEWHEEL)
            )
        {
            if (!IsWindowVisible(_this->hWnd))
            {
                if (uMsg == WM_HOTKEY && (int)wParam < 0)
                {
                    if (_this->bNoPerApplicationList)
                    {
                        return 0;
                    }
                    _this->mode = SWS_WINDOWSWITCHER_LAYOUTMODE_MINI;
                }
                else
                {
                    _this->mode = SWS_WINDOWSWITCHER_LAYOUTMODE_FULL;
                }
                _sws_WindowSwitcher_Show(_this);
                return 0;
            }
            else
            {
                _this->direction = SWS_WINDOWSWITCHERLAYOUT_COMPUTE_DIRECTION_INITIAL;
                _this->lastKey = (int)wParam;
                sws_WindowSwitcherLayoutWindow* pWindowList = (sws_WindowSwitcherLayoutWindow*)_this->layout.pWindowList.pList;

                if ((((uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) && wParam == _this->vkTilde) ||
                    (uMsg == WM_HOTKEY && ((int)wParam < 0))) &&
                    _this->mode == SWS_WINDOWSWITCHER_LAYOUTMODE_FULL && pWindowList[_this->layout.iIndex].hWnd != _this->hWndWallpaper)
                {
                    HWND hFw = pWindowList[_this->layout.iIndex].hWnd;
                    HWND hOwner = GetWindow(hFw, GW_OWNER);
                    _this->lastMiniModehWnd = (hOwner && IsWindowVisible(hOwner)) ? hOwner : hFw;
                    _this->mode = SWS_WINDOWSWITCHER_LAYOUTMODE_MINI;
                    _sws_WindowSwitcher_Show(_this);
                    return 0;
                }
                else if ((((uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) && wParam == VK_TAB) ||
                    (uMsg == WM_HOTKEY && ((int)wParam > 0))) &&
                    _this->mode == SWS_WINDOWSWITCHER_LAYOUTMODE_MINI)
                {
                    _this->mode = SWS_WINDOWSWITCHER_LAYOUTMODE_FULL;
                    _sws_WindowSwitcher_Show(_this);
                    return 0;
                }

                int col = _this->dwGridColumns;
                int row = _this->dwGridRows;

                BOOL bIsGridScrolling = FALSE;
                //Wh_Log(L"%d %d %d", _this->layout.pWindowList.cbSize, col * row, _this->layout.pWindowList.cbSize > col * row);
                if (uMsg == WM_MOUSEWHEEL && _this->layout.pWindowList.cbSize > col * row &&
                    (_this->dwOriginalScrollWheelBehavior == SWS_SCROLLWHEELBEHAVIOR_ONLYCLIENTAREA_GRIDSCROLL ||
                        (_this->dwOriginalScrollWheelBehavior == SWS_SCROLLWHEELBEHAVIOR_EVERYWHERE_IFCLIENTAREA_GRIDSCROLL && _this->bIsCursorOnSwitcher) ||
                        _this->dwOriginalScrollWheelBehavior == SWS_SCROLLWHEELBEHAVIOR_EVERYWHERE_GRIDSCROLL ||
						(_this->dwOriginalScrollWheelBehavior == SWS_SCROLLWHEELBEHAVIOR_EVERYWHERE_IFNOTCLIENTAREA_GRIDSCROLL && !_this->bIsCursorOnSwitcher))
                    )
                {
                    bIsGridScrolling = TRUE;
                }

                if (!bIsGridScrolling)
                {
                    if (
                        ((uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) && (GetKeyState(VK_SHIFT) & 0x8000)) ||
                        (uMsg == WM_HOTKEY && (LOWORD(lParam) & MOD_SHIFT)) ||
                        ((uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) && wParam == VK_LEFT) ||
                        ((uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) && wParam == VK_UP) ||
                        (uMsg == WM_MOUSEWHEEL && (_this->bScrollWheelInvert ? GET_WHEEL_DELTA_WPARAM(wParam) < 0 : GET_WHEEL_DELTA_WPARAM(wParam) > 0))
                        )
                    {
                        _this->direction = SWS_WINDOWSWITCHERLAYOUT_COMPUTE_DIRECTION_BACKWARD;

                        if (wParam != VK_UP || _this->layout.pWindowList.cbSize <= col)
                        {
                            if (_this->layout.iIndex == _this->layout.pWindowList.cbSize - 1)
                            {
                                _this->layout.iIndex = 0;
                            }
                            else
                            {
                                _this->layout.iIndex++;
                            }
                        }
                    }
                    else
                    {
                        _this->direction = SWS_WINDOWSWITCHERLAYOUT_COMPUTE_DIRECTION_FORWARD;

                        if (wParam != VK_DOWN || _this->layout.pWindowList.cbSize <= col)
                        {
                            if (_this->layout.iIndex == 0)
                            {
                                _this->layout.iIndex = _this->layout.pWindowList.cbSize - 1;
                            }
                            else
                            {
                                _this->layout.iIndex--;
                            }
                        }
                    }

                    if (_this->direction == SWS_WINDOWSWITCHERLAYOUT_COMPUTE_DIRECTION_FORWARD && _this->scrollDirection == _this->direction)
                    {
                        _this->layout.iFirstItemIndex -= col;
                        if (_this->layout.iFirstItemIndex < 0)
                        {
                            _this->layout.iFirstItemIndex += _this->layout.pWindowList.cbSize;
                        }
                        _this->cwMask = 0;
                        _this->cwIndex = -1;
                        Wh_Log(L"[sws] new first item index after scroll down: %d\n", _this->layout.iFirstItemIndex);
                    }
                    else if (_this->direction == SWS_WINDOWSWITCHERLAYOUT_COMPUTE_DIRECTION_BACKWARD && _this->scrollDirection == _this->direction)
                    {
                        _this->layout.iFirstItemIndex += col;
                        if (_this->layout.iFirstItemIndex >= (int)_this->layout.pWindowList.cbSize)
                        {
                            _this->layout.iFirstItemIndex -= _this->layout.pWindowList.cbSize;
                        }
                        _this->cwMask = 0;
                        _this->cwIndex = -1;
                        Wh_Log(L"[sws] new first item index after scroll up: %d\n", _this->layout.iFirstItemIndex);
                    }
                }
                else
                {
                    if (_this->bScrollWheelInvert ? GET_WHEEL_DELTA_WPARAM(wParam) < 0 : GET_WHEEL_DELTA_WPARAM(wParam) > 0)
                    {
                        _this->lastKey = VK_UP;
                    }
                    else
                    {
                        _this->lastKey = VK_DOWN;
                    }
                }

                _this->dwPaintFlags |= SWS_WINDOWSWITCHER_PAINTFLAGS_REDRAWENTIRE;
                RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);
                SetForegroundWindow(_this->hWnd);
                _sws_WindowSwitcher_UpdateAccessibleText(_this);
            }
        }
        return 0;
    }
    else if (uMsg == WM_INPUTLANGCHANGE || uMsg == SWS_WINDOWSWITCHER_RELOAD_HOTKEY_MSG)
    {
        sws_WindowSwitcher_UnregisterHotkeys(_this);
        sws_WindowSwitcher_RegisterHotkeys(_this, (HKL)lParam);
        return 0;
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

static DWORD CALLBACK _sws_WindowSwitcher_FlashAnimationProcedure(sws_WindowSwitcher* _this)
{
    if (_this && _this->hFlashAnimationSignal)
    {
        while (WaitForSingleObject(_this->hFlashAnimationSignal, INFINITE) == WAIT_OBJECT_0)
        {
            if (!_this->hWnd)
            {
                break;
            }
            PostMessageW(_this->hWnd, SWS_WINDOWSWITCHER_PAINT_MSG, SWS_WINDOWSWITCHER_PAINTFLAGS_ISFLASHANIMATION, 0);
            sws_nanosleep((LONGLONG)SWS_WINDOWSWITCHER_ANIMATOR_FLASH_DELAY * (LONGLONG)10000);
        }
    }
    return 0;
}

static DWORD CALLBACK _sws_WindowSwitcher_ShowAsyncProcedure(sws_WindowSwitcher* _this)
{
    if (_this && _this->hShowSignal)
    {
        while (WaitForSingleObject(_this->hShowSignal, INFINITE) == WAIT_OBJECT_0)
        {
            if (!_this->hWnd)
            {
                break;
            }
            long long mulres = (LONGLONG)_this->dwShowDelay * (LONGLONG)10000;
            long long start = sws_milliseconds_now();
            sws_nanosleep(mulres);
            Wh_Log(L"[sws] Delayed showing by %lld ms due to: user configuration.\n", sws_milliseconds_now() - start);
            if (IsWindowVisible(_this->hWnd))
            {
                BOOL bCloak = FALSE;
                DwmSetWindowAttribute(_this->hWnd, DWMWA_CLOAK, &bCloak, sizeof(BOOL));
            }
        }
    }
    return 0;
}

static sws_error_t _sws_WindowSwitcher_RegisterWindowClass(sws_WindowSwitcher* _this)
{
    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = _sws_WindowsSwitcher_WndProc;
    wc.hbrBackground = _this->hBackgroundBrush;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = SWS_WINDOWSWITCHER_CLASSNAME;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    ATOM a = RegisterClassExW(&wc);
    if (!a)
    {
        return sws_error_Report(sws_error_GetFromWin32Error(GetLastError()), NULL);
    }
    return SWS_ERROR_SUCCESS;
}

sws_error_t sws_WindowSwitcher_RunMessageQueue(sws_WindowSwitcher* _this)
{
    if (!_this) return sws_error_GetFromInternalError(SWS_ERROR_INVALID_PARAMETER);

    MSG msg;
    BOOL bRet;

    while ((bRet = GetMessageW(&msg, NULL, 0, 0)) != 0)
    {
        if (bRet == -1)
        {
            return sws_error_GetFromWin32Error(GetLastError());
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return SWS_ERROR_SUCCESS;
}

void sws_WindowSwitcher_LoadSettings(sws_WindowSwitcher* _this)
{
    _this->dwShowDelay = Wh_GetIntSetting(L"ShowDelay");
    _this->bIncludeWallpaper = Wh_GetIntSetting(L"IncludeWallpaper");
    _this->dwWallpaperSupport = _this->bIncludeWallpaper;
    _this->bPrimaryOnly = Wh_GetIntSetting(L"PrimaryMonitorOnly");
    _this->bNoPerApplicationList = Wh_GetIntSetting(L"NoPerApplicationList");
    _this->bSwitcherIsPerApplication = Wh_GetIntSetting(L"SwitcherIsPerApplication");
    _this->bAlwaysUseWindowTitleAndIcon = Wh_GetIntSetting(L"AlwaysUseWindowTitleAndIcon");

    _this->dwScrollWheelBehavior = _wtoi(WindhawkUtils::StringSetting::make(L"ScrollWheelBehavior"));
    if (_this->dwScrollWheelBehavior > SWS_SCROLLWHEELBEHAVIOR_EVERYWHERE_GRIDSCROLL)
    {
        _this->dwScrollWheelBehavior = SWS_SCROLLWHEELBEHAVIOR_DISABLED;
    }

    _this->bScrollWheelInvert = Wh_GetIntSetting(L"ScrollWheelInvert");
    _this->bSkipIfOneWindow = Wh_GetIntSetting(L"SkipIfOneWindow");

    _this->dwGridColumns = Wh_GetIntSetting(L"CoolSwitchColumns");
    if (_this->dwGridColumns < 1)
    {
        _this->dwGridColumns = SWS_WINDOWSWITCHERLAYOUT_DEFAULT_GRID_COLUMNS;
    }
    _this->dwGridRows = Wh_GetIntSetting(L"CoolSwitchRows");
    if (_this->dwGridRows < 1)
    {
        _this->dwGridRows = SWS_WINDOWSWITCHERLAYOUT_DEFAULT_GRID_ROWS;
    }
}

void sws_WindowSwitcher_Clear(sws_WindowSwitcher* _this)
{
    if (_this)
    {
        if (_this->pAccPropServices != NULL)
        {
            MSAAPROPID props[] = { LiveSetting_Property_GUID };
            _this->pAccPropServices->ClearHwndProps(
                _this->hWndAccessible,
                OBJID_CLIENT,
                CHILDID_SELF,
                props,
                ARRAYSIZE(props));
            _this->pAccPropServices->Release();
            _this->pAccPropServices = NULL;
        }
        DestroyWindow(_this->hWndAccessible);
        CloseHandle(_this->hEvExit);
        sws_WindowSwitcherLayout_Clear(&(_this->layout));
        sws_vector_Clear(&(_this->pHWNDList));
        DPA_DestroyCallback(_this->htshwnds, _sws_WindowSwitcher_free_stub, 0);
        sws_WindowSwitcher_UnregisterHotkeys(_this);
        UnhookWinEvent(_this->global_hook);
        DestroyWindow(_this->hWnd);
        _this->hWnd = NULL;
        SetEvent(_this->hShowSignal);
        WaitForSingleObject(_this->hShowThread, INFINITE);
        CloseHandle(_this->hShowSignal);
        CloseHandle(_this->hShowThread);
        SetEvent(_this->hFlashAnimationSignal);
        WaitForSingleObject(_this->hFlashAnimationThread, INFINITE);
        CloseHandle(_this->hFlashAnimationSignal);
        CloseHandle(_this->hFlashAnimationThread);
        BufferedPaintUnInit();
        UnregisterClassW(SWS_WINDOWSWITCHER_CLASSNAME, GetModuleHandle(NULL));
        DeleteObject(_this->hBackgroundBrush);
        DeleteObject(_this->hFlashBrush);
        sws_WindowHelpers_Clear();
        if (_this->hrRo != S_FALSE)
        {
            RoUninitialize();
        }
        if (_this->hrCo != S_FALSE)
        {
            CoUninitialize();
        }
        if (_this->bIsDynamic)
        {
            free(_this);
        }
        else
        {
            memset(_this, 0, sizeof(sws_WindowSwitcher));
        }
    }
}

// https://github.com/ramensoftware/windhawk-mods/blob/f79c82018ed58eb880450f0b94f74ecfc5083c15/mods/block-windows-startmenu-and-hosts.wh.cpp#L107
void TerminateProcessByName(const wchar_t* filename) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return;
    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(pe);
    if (Process32FirstW(hSnap, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, filename) == 0) {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                if (hProcess) {
                    TerminateProcess(hProcess, 0);
                    CloseHandle(hProcess);
                }
            }
        } while (Process32NextW(hSnap, &pe));
    }
    CloseHandle(hSnap);
}

sws_error_t sws_WindowSwitcher_Initialize(sws_WindowSwitcher** __this)
{
    sws_error_t rv = SWS_ERROR_SUCCESS;
    sws_WindowSwitcher* _this = NULL;

    if (!rv)
    {
        if (!*__this)
        {
            *__this = (sws_WindowSwitcher*)calloc(1, sizeof(sws_WindowSwitcher));
            if (!*__this)
            {
                rv = sws_error_Report(sws_error_GetFromInternalError(SWS_ERROR_NO_MEMORY), NULL);
            }
            (*__this)->bIsDynamic = TRUE;
        }
        else
        {
            (*__this)->bIsDynamic = FALSE;
        }
        _this = *__this;
    }
    if (!rv)
    {
        _this->hrCo = CoInitializeEx(NULL, COINIT_MULTITHREADED);
        if (_this->hrCo != S_OK && _this->hrCo != S_FALSE)
        {
            rv = sws_error_Report(sws_error_GetFromHRESULT(_this->hrCo), NULL);
        }
    }
    if (!rv)
    {
        _this->hrRo = RoInitialize(RO_INIT_MULTITHREADED);
        if (_this->hrRo != S_OK && _this->hrRo != S_FALSE)
        {
            rv = sws_error_Report(sws_error_GetFromHRESULT(_this->hrRo), NULL);
        }
    }
    if (!rv)
    {
        rv = sws_error_Report(sws_error_GetFromInternalError(sws_WindowHelpers_Initialize()), NULL);
    }
    if (!rv)
    {
        rv = sws_vector_Initialize(&(_this->pHWNDList), sizeof(sws_window));
    }
    if (!rv)
    {
        _this->hShowSignal = CreateEventW(NULL, FALSE, FALSE, NULL);
        if (!_this->hShowSignal)
        {
            rv = sws_error_Report(sws_error_GetFromWin32Error(GetLastError()), NULL);
        }
    }
    if (!rv)
    {
        _this->hShowThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_sws_WindowSwitcher_ShowAsyncProcedure, _this, 0, NULL);
        if (!_this->hShowThread)
        {
            rv = sws_error_Report(sws_error_GetFromWin32Error(GetLastError()), NULL);
        }
    }
    if (!rv)
    {
        _this->hFlashAnimationSignal = CreateEventW(NULL, TRUE, FALSE, NULL);
        if (!_this->hFlashAnimationSignal)
        {
            rv = sws_error_Report(sws_error_GetFromWin32Error(GetLastError()), NULL);
        }
    }
    if (!rv)
    {
        _this->hFlashAnimationThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_sws_WindowSwitcher_FlashAnimationProcedure, _this, 0, NULL);
        if (!_this->hFlashAnimationThread)
        {
            rv = sws_error_Report(sws_error_GetFromWin32Error(GetLastError()), NULL);
        }
    }
    if (!rv)
    {
        _this->htshwnds = DPA_Create(SWS_VECTOR_CAPACITY);
        if (!_this->htshwnds)
        {
            rv = sws_error_Report(sws_error_GetFromWin32Error(GetLastError()), NULL);
        }
        EnumWindows(sws_WindowHelpers_AddAltTabWindowsToTimeStampedHWNDList, (LPARAM)_this->htshwnds);
    }
    if (!rv)
    {
        _this->hBackgroundBrush = (HBRUSH)CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
        _this->hFlashBrush = (HBRUSH)CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
        _this->last_change = 0;
        _this->bWallpaperAlwaysLast = SWS_WINDOWSWITCHERLAYOUT_WALLPAPER_ALWAYS_LAST;
        _this->mode = SWS_WINDOWSWITCHER_LAYOUTMODE_FULL;
        _this->lastMiniModehWnd = NULL;
        _this->dwOriginalMouseRouting = -1;
        _this->scrollDirection = SWS_WINDOWSWITCHERLAYOUT_COMPUTE_DIRECTION_INITIAL;
    }
    if (!rv)
    {
        _this->msgShellHook = RegisterWindowMessageW(L"SHELLHOOK");
        if (!_this->msgShellHook)
        {
            rv = sws_error_Report(sws_error_GetFromWin32Error(GetLastError()), NULL);
        }
    }
    if (!SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)) {
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
    }
    if (!rv)
    {
        rv = sws_error_Report(sws_error_GetFromInternalError(_sws_WindowSwitcher_RegisterWindowClass(_this)), NULL);
    }
    if (!rv)
    {
        BufferedPaintInit();
        _this->hWnd = _sws_CreateWindowInBand(
            WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
            SWS_WINDOWSWITCHER_CLASSNAME,
            L"",
            WS_POPUP | WS_DLGFRAME,
            0, 0, 0, 0,
            NULL, NULL, GetModuleHandle(NULL), _this,
            ZBID_UIACCESS
        );
        if (!_this->hWnd)
        {
            // Try again without CWIB
            _this->hWnd = CreateWindowExW(
                WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
                SWS_WINDOWSWITCHER_CLASSNAME,
                L"",
                WS_POPUP | WS_DLGFRAME,
                0, 0, 0, 0,
                NULL, NULL, GetModuleHandle(NULL), _this
            );
        }
        if (!_this->hWnd)
        {
            rv = sws_error_Report(sws_error_GetFromWin32Error(GetLastError()), NULL);
        }
    }
    if (!rv)
    {
        if (!SetWinEventHook(
            EVENT_MIN,
            EVENT_MAX,
            NULL,
            _sws_WindowSwitcher_Wineventproc,
            0,
            0,
            WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
        ))
        {
            rv = sws_error_Report(sws_error_GetFromWin32Error(GetLastError()), NULL);
        }
    }
    if (!rv)
    {
        if (_this->bIsDynamic)
        {
            sws_WindowSwitcher_LoadSettings(_this);
        }
        BOOL bExcludedFromPeek = TRUE;
        DwmSetWindowAttribute(_this->hWnd, DWMWA_EXCLUDED_FROM_PEEK, &bExcludedFromPeek, sizeof(BOOL));
    }
    if (!rv)
    {
        if (sws_WindowSwitcher_RegisterHotkeys(_this, NULL) != ERROR_SUCCESS) {
            if (MessageBoxW(NULL, L"Explorer restart required. Restart now?", L"ClassicWindowSwitcher - Windhawk", MB_YESNO | MB_ICONWARNING) == IDYES) {
                TerminateProcessByName(L"explorer.exe");
                // Winlogon auto restarts explorer after termination
                // Try registering again now, before explorer registers it
                // Note: hooking RegisterHotKeys in explorer is ineffective on 11 (24H2). It seems to use a different API, maybe WinRT
                // Winning the hotkey registry race works there too
                rv = sws_error_Report(sws_WindowSwitcher_RegisterHotkeys(_this, NULL), NULL);
            }
        }
    }
    if (!rv)
    {
        if (_this->hWnd && !RegisterShellHookWindow(_this->hWnd))
        {
            rv = sws_error_Report(sws_error_GetFromWin32Error(GetLastError()), NULL);
        }
    }
    if (!rv)
    {
        _this->hEvExit = CreateEventW(NULL, FALSE, FALSE, NULL);
        if (!_this->hEvExit)
        {
            rv = sws_error_Report(sws_error_GetFromWin32Error(GetLastError()), NULL);
        }
    }
    if (!rv)
    {
        _this->hWndAccessible = CreateWindowExW(
            0,
            L"Static",
            L"",
            WS_CHILD,
            0, 0, 0, 0,
            _this->hWnd,
            NULL,
            (HINSTANCE)GetWindowLongPtrW(_this->hWnd, GWLP_HINSTANCE),
            NULL
        );
        if (!_this->hWndAccessible)
        {
            rv = sws_error_Report(sws_error_GetFromWin32Error(GetLastError()), NULL);
        }
    }
    if (!rv)
    {
        rv = sws_error_GetFromHRESULT(CoCreateInstance(CLSID_AccPropServices, NULL, CLSCTX_INPROC, IID_IAccPropServices, (void**)&(_this->pAccPropServices)));
    }
    if (!rv)
    {
        VARIANT var;
        var.vt = VT_I4;
        var.lVal = 2; //Assertive;
        rv = sws_error_GetFromHRESULT(_this->pAccPropServices->SetHwndProp(_this->hWndAccessible, OBJID_CLIENT, CHILDID_SELF, LiveSetting_Property_GUID, var));
    }

    if (!rv)
    {
        if (_this->dwWallpaperSupport == SWS_WALLPAPERSUPPORT_EXPLORER)
        {
            int k = 0;
            //Sleep(500);
            while (!sws_WindowHelpers_EnsureWallpaperHWND())
            {
                Wh_Log(L"[sws] Ensuring wallpaper\n");
                k++;
                if (k == 100)
                {
                    break;
                }
                Sleep(100);
            }
            Sleep(100);
            _this->hWndWallpaper = sws_WindowHelpers_GetWallpaperHWND();
            if (_this->hWndWallpaper)
            {
                RECT rc;
                GetWindowRect(_this->hWndWallpaper, &rc);
                Wh_Log(L"[sws] Wallpaper RECT %d %d %d %d\n", rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
            }
            else
            {
                _this->dwWallpaperSupport = SWS_WALLPAPERSUPPORT_NONE;
            }
        }
    }
    if (!rv)
    {
        _this->bIsInitialized = TRUE;
    }

    if (rv && (*__this) && (*__this)->bIsDynamic)
    {
        free((*__this));
        (*__this) = NULL;
    }

    return rv;
}

// main.c
BOOL g_bIsDesktopRaised = FALSE;
sws_WindowSwitcher* g_sws_WindowSwitcher = NULL;

DWORD WINAPI sws_main(LPVOID)
{
	sws_error_t rv = SWS_ERROR_SUCCESS;
	if (!rv)
	{
		rv = sws_WindowSwitcher_Initialize(&g_sws_WindowSwitcher);
	}
	if (!rv)
	{
		rv = sws_WindowSwitcher_RunMessageQueue(g_sws_WindowSwitcher);
	}
	if (!rv)
	{
		sws_WindowSwitcher_Clear(g_sws_WindowSwitcher);
	}
	return rv;
}

sws_error_t sws_unload()
{
    sws_error_t rv = SWS_ERROR_SUCCESS;
    if (g_sws_WindowSwitcher)
    {
		if (!PostMessageW(g_sws_WindowSwitcher->hWnd, WM_CLOSE, 0, 0)) {
            rv = GetLastError();
        }
    }
	return rv;
}

void WhTool_ModSettingsChanged() {
    if (g_sws_WindowSwitcher)
    {
		PostMessageW(g_sws_WindowSwitcher->hWnd, SWS_WINDOWSWITCHER_RELOAD_CONFIG_MSG, 0, 0);
    }
}

HANDLE g_hThread = NULL;

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL WhTool_ModInit() {
    Wh_Log(L"Init");

    g_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)sws_main, (LPVOID)0, 0, NULL);

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void WhTool_ModUninit() {
    Wh_Log(L"Uninit");
    sws_unload();
    if (g_hThread) {
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
    }
}

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
