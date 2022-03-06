// ==WindhawkMod==
// @id              notepad-dark-mode
// @name            Dark Mode for Notepad
// @description     The missing dark mode theme for Notepad
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         notepad.exe
// @compilerOptions -lcomctl32 -lgdi32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Dark Mode for Notepad
This mod changes Notepad's theme to be dark, as in the screenshot below.

The code is based on the [win32-darkmode](https://github.com/adzm/win32-darkmode)
repository and the dark mode implementation in
[Notepad++](https://github.com/notepad-plus-plus/notepad-plus-plus) and
[Process Hacker](https://github.com/processhacker/processhacker).

![Screenshot](https://i.imgur.com/tCQIKVh.png)
*/
// ==/WindhawkModReadme==

#include <commctrl.h>

// wParam - TRUE to subclass, FALSE to unsubclass
// lParam - subclass data
UINT g_subclassRegisteredMsg = RegisterWindowMessage(
    L"Windhawk_SetWindowSubclassFromAnyThread_notepad-dark-mode");

struct SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM {
    SUBCLASSPROC pfnSubclass;
    UINT_PTR uIdSubclass;
    DWORD_PTR dwRefData;
    BOOL result;
};

LRESULT CALLBACK CallWndProcForWindowSubclass(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == g_subclassRegisteredMsg && cwp->wParam) {
            SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM* param =
                (SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM*)cwp->lParam;
            param->result = SetWindowSubclass(
                cwp->hwnd, param->pfnSubclass, param->uIdSubclass, param->dwRefData);
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

BOOL SetWindowSubclassFromAnyThread(HWND hWnd, SUBCLASSPROC pfnSubclass, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return FALSE;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        return SetWindowSubclass(hWnd, pfnSubclass, uIdSubclass, dwRefData);
    }

    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProcForWindowSubclass, nullptr, dwThreadId);
    if (!hook) {
        return FALSE;
    }

    SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM param;
    param.pfnSubclass = pfnSubclass;
    param.uIdSubclass = uIdSubclass;
    param.dwRefData = dwRefData;
    param.result = FALSE;
    SendMessage(hWnd, g_subclassRegisteredMsg, TRUE, (WPARAM)&param);

    UnhookWindowsHookEx(hook);

    return param.result;
}

#pragma region ProcessHackerDarkStatusBar

/*
* Code taken from Process Hacker:
* https://github.com/processhacker/processhacker/blob/b6644725ff213f2b692378112f05f4ecd9af3d42/phlib/theme.c
* Copyright (C) 2018-2019 dmex
* GNU General Public License
* Refer to the license in the repository above
*/

#include <stdlib.h>
#include <uxtheme.h>
#include <vssym32.h>
#include <windowsx.h>

typedef struct _PHP_THEME_WINDOW_STATUSBAR_CONTEXT
{
    //WNDPROC DefaultWindowProc;
    BOOLEAN MouseActive;
    HTHEME StatusThemeData;
} PHP_THEME_WINDOW_STATUSBAR_CONTEXT, *PPHP_THEME_WINDOW_STATUSBAR_CONTEXT;

ULONG PhpThemeColorMode = 0;
BOOLEAN PhpThemeEnable = FALSE;
BOOLEAN PhpThemeBorderEnable = TRUE;
HBRUSH PhMenuBackgroundBrush = NULL;
COLORREF PhpThemeWindowForegroundColor = RGB(28, 28, 28);
COLORREF PhpThemeWindowBackgroundColor = RGB(43, 43, 43);
COLORREF PhpThemeWindowTextColor = RGB(0xff, 0xff, 0xff);
HFONT PhpTabControlFontHandle = NULL;
HFONT PhpToolBarFontHandle = NULL;
HFONT PhpHeaderFontHandle = NULL;
HFONT PhpListViewFontHandle = NULL;
HFONT PhpMenuFontHandle = NULL;
HFONT PhpGroupboxFontHandle = NULL;
HFONT PhpStatusBarFontHandle = NULL;

HWND StatusBarHandle;
PHP_THEME_WINDOW_STATUSBAR_CONTEXT StatusBarContext;

LRESULT CALLBACK PhpThemeWindowStatusbarWndSubclassProc(
    _In_ HWND WindowHandle,
    _In_ UINT uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam,
    _In_ UINT_PTR uIdSubclass,
    _In_ DWORD_PTR dwRefData
    )
{
    PPHP_THEME_WINDOW_STATUSBAR_CONTEXT context;
    WNDPROC oldWndProc;

    context = &StatusBarContext;
    //oldWndProc = context->DefaultWindowProc;

    switch (uMsg)
    {
    case WM_ERASEBKGND:
        return FALSE;
    case WM_MOUSEMOVE:
        {
            if (!context->MouseActive)
            {
                TRACKMOUSEEVENT trackEvent =
                {
                    sizeof(TRACKMOUSEEVENT),
                    TME_LEAVE,
                    WindowHandle,
                    0
                };

                TrackMouseEvent(&trackEvent);
                context->MouseActive = TRUE;
            }

            InvalidateRect(WindowHandle, NULL, FALSE);
        }
        break;
    case WM_MOUSELEAVE:
        {
            InvalidateRect(WindowHandle, NULL, FALSE);
            context->MouseActive = FALSE;
        }
        break;
    case WM_PAINT:
        {
            RECT clientRect;
            PAINTSTRUCT ps;
            INT blockCoord[128];
            INT blockCount = (INT)SendMessage(WindowHandle, (UINT)SB_GETPARTS, (WPARAM)ARRAYSIZE(blockCoord), (WPARAM)blockCoord);

            // InvalidateRect(WindowHandle, NULL, FALSE);
            GetClientRect(WindowHandle, &clientRect);

            if (!BeginPaint(WindowHandle, &ps))
                break;

            SetBkMode(ps.hdc, TRANSPARENT);
            HDC hdc = CreateCompatibleDC(ps.hdc);
            SetBkMode(hdc, TRANSPARENT);

            HBITMAP hbm = CreateCompatibleBitmap(ps.hdc, clientRect.right, clientRect.bottom);
            SelectBitmap(hdc, hbm);

            if (!PhpStatusBarFontHandle)
            {
                NONCLIENTMETRICS metrics = { sizeof(NONCLIENTMETRICS) };

                if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &metrics, 0))
                {
                    PhpStatusBarFontHandle = CreateFontIndirect(&metrics.lfMessageFont);
                }
            }

            SelectFont(hdc, PhpStatusBarFontHandle);
            SetTextColor(hdc, PhpThemeWindowTextColor);
            SetDCBrushColor(hdc, PhpThemeWindowBackgroundColor);
            FillRect(hdc, &clientRect, GetStockBrush(DC_BRUSH));

            //switch (PhpThemeColorMode)
            //{
            //case 0: // New colors
            //    SetTextColor(hdc, RGB(0x0, 0x0, 0x0));
            //    SetDCBrushColor(hdc, GetSysColor(COLOR_3DFACE));  // RGB(0xff, 0xff, 0xff));
            //    FillRect(hdc, &clientRect, GetStockBrush(DC_BRUSH));
            //    break;
            //case 1: // Old colors
            //    SetTextColor(hdc, RGB(0xff, 0xff, 0xff));
            //    SetDCBrushColor(hdc, RGB(65, 65, 65)); //RGB(28, 28, 28)); // RGB(65, 65, 65));
            //    FillRect(hdc, &clientRect, GetStockBrush(DC_BRUSH));
            //    break;
            //}

            for (INT i = 0; i < blockCount; i++)
            {
                RECT blockRect = { 0, 0, 0, 0 };
                WCHAR buffer[MAX_PATH] = L"";

                if (!SendMessage(WindowHandle, SB_GETRECT, (WPARAM)i, (WPARAM)&blockRect))
                    continue;
                if (!SendMessage(WindowHandle, SB_GETTEXT, (WPARAM)i, (LPARAM)buffer))
                    continue;

                POINT pt;
                GetCursorPos(&pt);
                MapWindowPoints(NULL, WindowHandle, &pt, 1);

                if (PtInRect(&blockRect, pt))
                {
                    switch (PhpThemeColorMode)
                    {
                    case 0: // New colors
                        SetTextColor(hdc, RGB(0xff, 0xff, 0xff));
                        SetDCBrushColor(hdc, RGB(64, 64, 64));
                        FillRect(hdc, &blockRect, GetStockBrush(DC_BRUSH));
                        break;
                    case 1: // Old colors
                        SetTextColor(hdc, RGB(0xff, 0xff, 0xff));
                        SetDCBrushColor(hdc, RGB(128, 128, 128));
                        FillRect(hdc, &blockRect, GetStockBrush(DC_BRUSH));
                        break;
                    }

                    //FrameRect(hdc, &blockRect, GetSysColorBrush(COLOR_HIGHLIGHT));
                }
                else
                {
                    SetTextColor(hdc, PhpThemeWindowTextColor);
                    SetDCBrushColor(hdc, PhpThemeWindowBackgroundColor);
                    FillRect(hdc, &blockRect, GetStockBrush(DC_BRUSH));

                    //switch (PhpThemeColorMode)
                    //{
                    //case 0: // New colors
                    //    SetTextColor(hdc, PhpThemeWindowTextColor);
                    //    SetDCBrushColor(hdc, PhpThemeWindowBackgroundColor);
                    //    //SetTextColor(hdc, RGB(0x0, 0x0, 0x0));
                    //    //SetDCBrushColor(hdc, GetSysColor(COLOR_3DFACE)); // RGB(0xff, 0xff, 0xff));
                    //    FillRect(hdc, &blockRect, GetStockBrush(DC_BRUSH));
                    //    break;
                    //case 1: // Old colors
                    //    SetTextColor(hdc, RGB(0xff, 0xff, 0xff));
                    //    SetDCBrushColor(hdc, RGB(64, 64, 64));
                    //    FillRect(hdc, &blockRect, GetStockBrush(DC_BRUSH));
                    //    break;
                    //}
                    //
                    //FrameRect(hdc, &blockRect, GetSysColorBrush(COLOR_HIGHLIGHT));
                }

                DrawText(
                    hdc,
                    buffer,
                    -1,
                    &blockRect,
                    DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_HIDEPREFIX
                    );
            }

            {
                RECT sizeGripRect;

                sizeGripRect.left = clientRect.right - GetSystemMetrics(SM_CXHSCROLL);
                sizeGripRect.top = clientRect.bottom - GetSystemMetrics(SM_CYVSCROLL);
                sizeGripRect.right = clientRect.right;
                sizeGripRect.bottom = clientRect.bottom;

                if (context->StatusThemeData)
                    DrawThemeBackground(context->StatusThemeData, hdc, SP_GRIPPER, 0, &sizeGripRect, &sizeGripRect);
                else
                    DrawFrameControl(hdc, &sizeGripRect, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
            }

            BitBlt(ps.hdc, 0, 0, clientRect.right, clientRect.bottom, hdc, 0, 0, SRCCOPY);

            DeleteDC(hdc);
            DeleteBitmap(hbm);
            EndPaint(WindowHandle, &ps);
        }
        goto DefaultWndProc;
    default:
        if (uMsg == g_subclassRegisteredMsg && !wParam)
            RemoveWindowSubclass(WindowHandle, PhpThemeWindowStatusbarWndSubclassProc, 0);
        break;
    }

    //return CallWindowProc(oldWndProc, WindowHandle, uMsg, wParam, lParam);
    return DefSubclassProc(WindowHandle, uMsg, wParam, lParam);

DefaultWndProc:
    return DefWindowProc(WindowHandle, uMsg, wParam, lParam);
}

BOOL SubclassStatusBar(HWND hWnd)
{
    if (StatusBarHandle)
        return FALSE;

    StatusBarHandle = hWnd;
    PPHP_THEME_WINDOW_STATUSBAR_CONTEXT context = &StatusBarContext;
    memset(context, 0, sizeof(*context));
    context->StatusThemeData = OpenThemeData(StatusBarHandle, VSCLASS_STATUS);
    //context->DefaultWindowProc = (WNDPROC)SetWindowLongPtr(StatusBarHandle, GWLP_WNDPROC, (LONG_PTR)PhpThemeWindowStatusbarWndSubclassProc);
    SetWindowSubclassFromAnyThread(StatusBarHandle, PhpThemeWindowStatusbarWndSubclassProc, 0, 0);

    InvalidateRect(StatusBarHandle, NULL, FALSE);

    return TRUE;
}

BOOL UnsubclassStatusBar()
{
    if (!StatusBarHandle)
        return FALSE;

    PPHP_THEME_WINDOW_STATUSBAR_CONTEXT context = &StatusBarContext;
    //SetWindowLongPtr(StatusBarHandle, GWLP_WNDPROC, (LONG_PTR)context->DefaultWindowProc);
    //context->DefaultWindowProc = NULL;
    SendMessage(StatusBarHandle, g_subclassRegisteredMsg, FALSE, 0);

    InvalidateRect(StatusBarHandle, NULL, FALSE);
    StatusBarHandle = NULL;

    CloseThemeData(context->StatusThemeData);
    context->StatusThemeData = NULL;

    return TRUE;
}

#pragma endregion ProcessHackerDarkStatusBar

#pragma region DarkMode.cpp

/*
* Code taken from Notepad++:
* https://github.com/notepad-plus-plus/notepad-plus-plus/blob/bab3573be708bb908b8080e3e2007ea78a7f1932/PowerEditor/src/DarkMode/DarkMode.cpp
* Refer to the license in the repository above
*/

#include <uxtheme.h>
#include <vssym32.h>

enum IMMERSIVE_HC_CACHE_MODE
{
    IHCM_USE_CACHED_VALUE,
    IHCM_REFRESH
};

// 1903 18362
enum class PreferredAppMode
{
    Default,
    AllowDark,
    ForceDark,
    ForceLight,
    Max
};

enum WINDOWCOMPOSITIONATTRIB
{
    WCA_UNDEFINED = 0,
    WCA_NCRENDERING_ENABLED = 1,
    WCA_NCRENDERING_POLICY = 2,
    WCA_TRANSITIONS_FORCEDISABLED = 3,
    WCA_ALLOW_NCPAINT = 4,
    WCA_CAPTION_BUTTON_BOUNDS = 5,
    WCA_NONCLIENT_RTL_LAYOUT = 6,
    WCA_FORCE_ICONIC_REPRESENTATION = 7,
    WCA_EXTENDED_FRAME_BOUNDS = 8,
    WCA_HAS_ICONIC_BITMAP = 9,
    WCA_THEME_ATTRIBUTES = 10,
    WCA_NCRENDERING_EXILED = 11,
    WCA_NCADORNMENTINFO = 12,
    WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
    WCA_VIDEO_OVERLAY_ACTIVE = 14,
    WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
    WCA_DISALLOW_PEEK = 16,
    WCA_CLOAK = 17,
    WCA_CLOAKED = 18,
    WCA_ACCENT_POLICY = 19,
    WCA_FREEZE_REPRESENTATION = 20,
    WCA_EVER_UNCLOAKED = 21,
    WCA_VISUAL_OWNER = 22,
    WCA_HOLOGRAPHIC = 23,
    WCA_EXCLUDED_FROM_DDA = 24,
    WCA_PASSIVEUPDATEMODE = 25,
    WCA_USEDARKMODECOLORS = 26,
    WCA_LAST = 27
};

struct WINDOWCOMPOSITIONATTRIBDATA
{
    WINDOWCOMPOSITIONATTRIB Attrib;
    PVOID pvData;
    SIZE_T cbData;
};

using fnRtlGetNtVersionNumbers = void (WINAPI *)(LPDWORD major, LPDWORD minor, LPDWORD build);
using fnSetWindowCompositionAttribute = BOOL (WINAPI *)(HWND hWnd, WINDOWCOMPOSITIONATTRIBDATA*);
// 1809 17763
using fnShouldAppsUseDarkMode = bool (WINAPI *)(); // ordinal 132
using fnAllowDarkModeForWindow = bool (WINAPI *)(HWND hWnd, bool allow); // ordinal 133
using fnAllowDarkModeForApp = bool (WINAPI *)(bool allow); // ordinal 135, in 1809
using fnFlushMenuThemes = void (WINAPI *)(); // ordinal 136
using fnRefreshImmersiveColorPolicyState = void (WINAPI *)(); // ordinal 104
using fnIsDarkModeAllowedForWindow = bool (WINAPI *)(HWND hWnd); // ordinal 137
using fnGetIsImmersiveColorUsingHighContrast = bool (WINAPI *)(IMMERSIVE_HC_CACHE_MODE mode); // ordinal 106
using fnOpenNcThemeData = HTHEME(WINAPI *)(HWND hWnd, LPCWSTR pszClassList); // ordinal 49
// 1903 18362
using fnShouldSystemUseDarkMode = bool (WINAPI *)(); // ordinal 138
using fnSetPreferredAppMode = PreferredAppMode (WINAPI *)(PreferredAppMode appMode); // ordinal 135, in 1903
using fnIsDarkModeAllowedForApp = bool (WINAPI *)(); // ordinal 139

fnSetWindowCompositionAttribute _SetWindowCompositionAttribute = nullptr;
fnShouldAppsUseDarkMode _ShouldAppsUseDarkMode = nullptr;
fnAllowDarkModeForWindow _AllowDarkModeForWindow = nullptr;
fnAllowDarkModeForApp _AllowDarkModeForApp = nullptr;
fnFlushMenuThemes _FlushMenuThemes = nullptr;
fnRefreshImmersiveColorPolicyState _RefreshImmersiveColorPolicyState = nullptr;
fnIsDarkModeAllowedForWindow _IsDarkModeAllowedForWindow = nullptr;
fnGetIsImmersiveColorUsingHighContrast _GetIsImmersiveColorUsingHighContrast = nullptr;
fnOpenNcThemeData _OpenNcThemeData = nullptr;
// 1903 18362
//fnShouldSystemUseDarkMode _ShouldSystemUseDarkMode = nullptr;
fnSetPreferredAppMode _SetPreferredAppMode = nullptr;

bool g_darkModeSupported = false;
bool g_darkModeEnabled = false;
DWORD g_buildNumber = 0;

bool ShouldAppsUseDarkMode()
{
    if (!_ShouldAppsUseDarkMode)
    {
        return false;
    }

    return _ShouldAppsUseDarkMode();
}

bool AllowDarkModeForWindow(HWND hWnd, bool allow)
{
    if (g_darkModeSupported && _AllowDarkModeForWindow)
        return _AllowDarkModeForWindow(hWnd, allow);
    return false;
}

bool IsHighContrast()
{
    HIGHCONTRASTW highContrast = { sizeof(highContrast) };
    if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(highContrast), &highContrast, FALSE))
        return highContrast.dwFlags & HCF_HIGHCONTRASTON;
    return false;
}

void SetTitleBarThemeColor(HWND hWnd, BOOL dark)
{
    if (g_buildNumber < 18362)
        SetPropW(hWnd, L"UseImmersiveDarkModeColors", reinterpret_cast<HANDLE>(static_cast<INT_PTR>(dark)));
    else if (_SetWindowCompositionAttribute)
    {
        WINDOWCOMPOSITIONATTRIBDATA data = { WCA_USEDARKMODECOLORS, &dark, sizeof(dark) };
        _SetWindowCompositionAttribute(hWnd, &data);
    }
}

void RefreshTitleBarThemeColor(HWND hWnd)
{
    BOOL dark = FALSE;
    if (_IsDarkModeAllowedForWindow && _ShouldAppsUseDarkMode)
    {
        if (_IsDarkModeAllowedForWindow(hWnd) && _ShouldAppsUseDarkMode() && !IsHighContrast())
        {
            dark = TRUE;
        }
    }

    SetTitleBarThemeColor(hWnd, dark);
}

bool IsColorSchemeChangeMessage(LPARAM lParam)
{
    bool is = false;
    if (lParam && (0 == lstrcmpi(reinterpret_cast<LPCWCH>(lParam), L"ImmersiveColorSet")) && _RefreshImmersiveColorPolicyState)
    {
        _RefreshImmersiveColorPolicyState();
        is = true;
    }
    if (_GetIsImmersiveColorUsingHighContrast)
        _GetIsImmersiveColorUsingHighContrast(IHCM_REFRESH);
    return is;
}

bool IsColorSchemeChangeMessage(UINT message, LPARAM lParam)
{
    if (message == WM_SETTINGCHANGE)
        return IsColorSchemeChangeMessage(lParam);
    return false;
}

void AllowDarkModeForApp(bool allow)
{
    if (_AllowDarkModeForApp)
        _AllowDarkModeForApp(allow);
    else if (_SetPreferredAppMode)
        _SetPreferredAppMode(allow ? PreferredAppMode::ForceDark : PreferredAppMode::Default);
}

void FlushMenuThemes()
{
    if (_FlushMenuThemes)
    {
        _FlushMenuThemes();
    }
}

constexpr bool CheckBuildNumber(DWORD buildNumber)
{
    return (buildNumber == 17763 || // 1809
        buildNumber == 18362 || // 1903
        buildNumber == 18363 || // 1909
        buildNumber == 19041 || // 2004
        buildNumber == 19042 || // 20H2
        buildNumber == 19043 || // 21H1
        buildNumber == 19044 || // 21H2
        (buildNumber > 19044 && buildNumber < 22000) || // Windows 10 any version > 21H2
        buildNumber >= 22000);  // Windows 11 insider builds
}

void InitDarkMode()
{
    fnRtlGetNtVersionNumbers RtlGetNtVersionNumbers = nullptr;
    HMODULE hNtdllModule = GetModuleHandle(L"ntdll.dll");
    if (hNtdllModule)
    {
        RtlGetNtVersionNumbers = reinterpret_cast<fnRtlGetNtVersionNumbers>(GetProcAddress(hNtdllModule, "RtlGetNtVersionNumbers"));
    }

    if (RtlGetNtVersionNumbers)
    {
        DWORD major, minor;
        RtlGetNtVersionNumbers(&major, &minor, &g_buildNumber);
        g_buildNumber &= ~0xF0000000;
        if (major == 10 && minor == 0 && CheckBuildNumber(g_buildNumber))
        {
            HMODULE hUxtheme = LoadLibraryEx(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
            if (hUxtheme)
            {
                _OpenNcThemeData = reinterpret_cast<fnOpenNcThemeData>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(49)));
                _RefreshImmersiveColorPolicyState = reinterpret_cast<fnRefreshImmersiveColorPolicyState>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(104)));
                _GetIsImmersiveColorUsingHighContrast = reinterpret_cast<fnGetIsImmersiveColorUsingHighContrast>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(106)));
                _ShouldAppsUseDarkMode = reinterpret_cast<fnShouldAppsUseDarkMode>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(132)));
                _AllowDarkModeForWindow = reinterpret_cast<fnAllowDarkModeForWindow>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(133)));

                auto ord135 = GetProcAddress(hUxtheme, MAKEINTRESOURCEA(135));
                if (g_buildNumber < 18362)
                    _AllowDarkModeForApp = reinterpret_cast<fnAllowDarkModeForApp>(ord135);
                else
                    _SetPreferredAppMode = reinterpret_cast<fnSetPreferredAppMode>(ord135);

                _FlushMenuThemes = reinterpret_cast<fnFlushMenuThemes>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(136)));
                _IsDarkModeAllowedForWindow = reinterpret_cast<fnIsDarkModeAllowedForWindow>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(137)));

                HMODULE hUser32Module = GetModuleHandleW(L"user32.dll");
                if (hUser32Module)
                {
                    _SetWindowCompositionAttribute = reinterpret_cast<fnSetWindowCompositionAttribute>(GetProcAddress(hUser32Module, "SetWindowCompositionAttribute"));
                }

                if (_OpenNcThemeData &&
                    _RefreshImmersiveColorPolicyState &&
                    _ShouldAppsUseDarkMode &&
                    _AllowDarkModeForWindow &&
                    (_AllowDarkModeForApp || _SetPreferredAppMode) &&
                    _FlushMenuThemes &&
                    _IsDarkModeAllowedForWindow)
                {
                    g_darkModeSupported = true;
                }
            }
        }
    }
}

void SetDarkMode(bool useDark, bool fixDarkScrollbar)
{
    if (g_darkModeSupported)
    {
        AllowDarkModeForApp(useDark);
        //_RefreshImmersiveColorPolicyState();
        FlushMenuThemes();
        if (fixDarkScrollbar)
        {
            //FixDarkScrollBar();
        }
        g_darkModeEnabled = ShouldAppsUseDarkMode() && !IsHighContrast();
    }
}

#pragma endregion DarkMode.cpp

#pragma region UAHMenuBar

/*
* Code taken from:
* https://github.com/adzm/win32-darkmode/blob/56e617ccba1a5c7950a1e80fb5215ebb3b7f918b/win32-darkmode/UAHMenuBar.h
*/

// MIT license, see LICENSE
// Copyright(c) 2021 adzm / Adam D. Walling

// processes messages related to UAH / custom menubar drawing.
// return true if handled, false to continue with normal processing in your wndproc
bool UAHDarkModeWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* lr);

// window messages related to menu bar drawing
#define WM_UAHDESTROYWINDOW    0x0090	// handled by DefWindowProc
#define WM_UAHDRAWMENU         0x0091	// lParam is UAHMENU
#define WM_UAHDRAWMENUITEM     0x0092	// lParam is UAHDRAWMENUITEM
#define WM_UAHINITMENU         0x0093	// handled by DefWindowProc
#define WM_UAHMEASUREMENUITEM  0x0094	// lParam is UAHMEASUREMENUITEM
#define WM_UAHNCPAINTMENUPOPUP 0x0095	// handled by DefWindowProc

// describes the sizes of the menu bar or menu item
typedef union tagUAHMENUITEMMETRICS
{
    // cx appears to be 14 / 0xE less than rcItem's width!
    // cy 0x14 seems stable, i wonder if it is 4 less than rcItem's height which is always 24 atm
    struct {
        DWORD cx;
        DWORD cy;
    } rgsizeBar[2];
    struct {
        DWORD cx;
        DWORD cy;
    } rgsizePopup[4];
} UAHMENUITEMMETRICS;

// not really used in our case but part of the other structures
typedef struct tagUAHMENUPOPUPMETRICS
{
    DWORD rgcx[4];
    DWORD fUpdateMaxWidths : 2; // from kernel symbols, padded to full dword
} UAHMENUPOPUPMETRICS;

// hmenu is the main window menu; hdc is the context to draw in
typedef struct tagUAHMENU
{
    HMENU hmenu;
    HDC hdc;
    DWORD dwFlags; // no idea what these mean, in my testing it's either 0x00000a00 or sometimes 0x00000a10
} UAHMENU;

// menu items are always referred to by iPosition here
typedef struct tagUAHMENUITEM
{
    int iPosition; // 0-based position of menu item in menubar
    UAHMENUITEMMETRICS umim;
    UAHMENUPOPUPMETRICS umpm;
} UAHMENUITEM;

// the DRAWITEMSTRUCT contains the states of the menu items, as well as
// the position index of the item in the menu, which is duplicated in
// the UAHMENUITEM's iPosition as well
typedef struct UAHDRAWMENUITEM
{
    DRAWITEMSTRUCT dis; // itemID looks uninitialized
    UAHMENU um;
    UAHMENUITEM umi;
} UAHDRAWMENUITEM;

// the MEASUREITEMSTRUCT is intended to be filled with the size of the item
// height appears to be ignored, but width can be modified
typedef struct tagUAHMEASUREMENUITEM
{
    MEASUREITEMSTRUCT mis;
    UAHMENU um;
    UAHMENUITEM umi;
} UAHMEASUREMENUITEM;

#pragma endregion UAHMenuBar

#pragma region UAHDarkModeWndProc

/*
* Code taken from:
* https://github.com/adzm/win32-darkmode/blob/56e617ccba1a5c7950a1e80fb5215ebb3b7f918b/win32-darkmode/win32-darkmode.cpp
* Refer to the license in the repository above
*/

static HTHEME g_menuTheme = nullptr;

// processes messages related to UAH / custom menubar drawing.
// return true if handled, false to continue with normal processing in your wndproc
bool UAHDarkModeWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* lr)
{
    switch (message)
    {
    case WM_UAHDRAWMENU:
    {
        UAHMENU* pUDM = (UAHMENU*)lParam;
        RECT rc = { 0 };

        // get the menubar rect
        {
            MENUBARINFO mbi = { sizeof(mbi) };
            GetMenuBarInfo(hWnd, OBJID_MENU, 0, &mbi);

            RECT rcWindow;
            GetWindowRect(hWnd, &rcWindow);

            // the rcBar is offset by the window rect
            rc = mbi.rcBar;
            OffsetRect(&rc, -rcWindow.left, -rcWindow.top);

            rc.top -= 1;
        }

        if (!g_menuTheme) {
            g_menuTheme = OpenThemeData(hWnd, L"Menu");
        }

        DrawThemeBackground(g_menuTheme, pUDM->hdc, MENU_POPUPITEM, MPI_NORMAL, &rc, nullptr);

        return true;
    }
    case WM_UAHDRAWMENUITEM:
    {
        UAHDRAWMENUITEM* pUDMI = (UAHDRAWMENUITEM*)lParam;

        // get the menu item string
        wchar_t menuString[256] = { 0 };
        MENUITEMINFO mii = { sizeof(mii), MIIM_STRING };
        {
            mii.dwTypeData = menuString;
            mii.cch = (sizeof(menuString) / 2) - 1;

            GetMenuItemInfo(pUDMI->um.hmenu, pUDMI->umi.iPosition, TRUE, &mii);
        }

        // get the item state for drawing

        DWORD dwFlags = DT_CENTER | DT_SINGLELINE | DT_VCENTER;

        int iTextStateID = 0;
        int iBackgroundStateID = 0;
        {
            if ((pUDMI->dis.itemState & ODS_INACTIVE) | (pUDMI->dis.itemState & ODS_DEFAULT)) {
                // normal display
                iTextStateID = MPI_NORMAL;
                iBackgroundStateID = MPI_NORMAL;
            }
            if (pUDMI->dis.itemState & ODS_HOTLIGHT) {
                // hot tracking
                iTextStateID = MPI_HOT;
                iBackgroundStateID = MPI_HOT;
            }
            if (pUDMI->dis.itemState & ODS_SELECTED) {
                // clicked -- MENU_POPUPITEM has no state for this, though MENU_BARITEM does
                iTextStateID = MPI_HOT;
                iBackgroundStateID = MPI_HOT;
            }
            if ((pUDMI->dis.itemState & ODS_GRAYED) || (pUDMI->dis.itemState & ODS_DISABLED)) {
                // disabled / grey text
                iTextStateID = MPI_DISABLED;
                iBackgroundStateID = MPI_DISABLED;
            }
            if (pUDMI->dis.itemState & ODS_NOACCEL) {
                dwFlags |= DT_HIDEPREFIX;
            }
        }

        if (!g_menuTheme) {
            g_menuTheme = OpenThemeData(hWnd, L"Menu");
        }

        DrawThemeBackground(g_menuTheme, pUDMI->um.hdc, MENU_POPUPITEM, iBackgroundStateID, &pUDMI->dis.rcItem, nullptr);
        DrawThemeText(g_menuTheme, pUDMI->um.hdc, MENU_POPUPITEM, iTextStateID, menuString, mii.cch, dwFlags, 0, &pUDMI->dis.rcItem);

        return true;
    }
    case WM_THEMECHANGED:
    {
        if (g_menuTheme) {
            CloseThemeData(g_menuTheme);
            g_menuTheme = nullptr;
        }
        // continue processing in main wndproc
        return false;
    }
    default:
        return false;
    }
}

#pragma endregion UAHDarkModeWndProc

#pragma region drawUAHMenuNCBottomLine

/*
* Code taken from:
* https://github.com/notepad-plus-plus/notepad-plus-plus/blob/bab3573be708bb908b8080e3e2007ea78a7f1932/PowerEditor/src/NppDarkMode.cpp
* Refer to the license in the repository above
*/

void drawUAHMenuNCBottomLine(HWND hWnd)
{
    MENUBARINFO mbi = { sizeof(mbi) };
    if (!GetMenuBarInfo(hWnd, OBJID_MENU, 0, &mbi))
    {
        return;
    }

    RECT rcClient = { 0 };
    GetClientRect(hWnd, &rcClient);
    MapWindowPoints(hWnd, nullptr, (POINT*)&rcClient, 2);

    RECT rcWindow = { 0 };
    GetWindowRect(hWnd, &rcWindow);

    OffsetRect(&rcClient, -rcWindow.left, -rcWindow.top);

    // the rcBar is offset by the window rect
    RECT rcAnnoyingLine = rcClient;
    rcAnnoyingLine.bottom = rcAnnoyingLine.top;
    rcAnnoyingLine.top--;

    HDC hdc = GetWindowDC(hWnd);
    SetDCBrushColor(hdc, RGB(60, 60, 60));
    FillRect(hdc, &rcAnnoyingLine, (HBRUSH)GetStockObject(DC_BRUSH));
    ReleaseDC(hWnd, hdc);
}

#pragma endregion drawUAHMenuNCBottomLine

LRESULT CALLBACK NotepadWindowSubclassProc(
    _In_ HWND hWnd,
    _In_ UINT uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam,
    _In_ UINT_PTR uIdSubclass,
    _In_ DWORD_PTR dwRefData
    )
{
    LRESULT lr = 0;
    if (g_darkModeSupported && UAHDarkModeWndProc(hWnd, uMsg, wParam, lParam, &lr)) {
        return lr;
    }

    switch (uMsg)
    {
    case WM_NCACTIVATE:
    case WM_NCPAINT:
        lr = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        drawUAHMenuNCBottomLine(hWnd);
        return lr;

    case WM_CTLCOLOREDIT:
        SetBkColor((HDC)wParam, RGB(60, 60, 60));
        SetTextColor((HDC)wParam, RGB(0xff, 0xff, 0xff));
        SetDCBrushColor((HDC)wParam, RGB(60, 60, 60));
        return (INT_PTR)(HBRUSH)GetStockObject(DC_BRUSH);

    default:
        if (uMsg == g_subclassRegisteredMsg && !wParam)
            RemoveWindowSubclass(hWnd, NotepadWindowSubclassProc, 0);
        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void SubclassNotepadWindow(HWND hWnd)
{
    SetWindowSubclassFromAnyThread(hWnd, NotepadWindowSubclassProc, 0, 0);
}

void UnsubclassNotepadWindow(HWND hWnd)
{
    SendMessage(hWnd, g_subclassRegisteredMsg, FALSE, 0);
}

HWND NotepadWindowHandle;

BOOL SetDarkModeNotepadWindow(HWND hWnd)
{
    if (NotepadWindowHandle)
        return FALSE;

    NotepadWindowHandle = hWnd;

    SubclassNotepadWindow(hWnd);

    AllowDarkModeForWindow(NotepadWindowHandle, true);
    RefreshTitleBarThemeColor(NotepadWindowHandle);

    RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
    return TRUE;
}

BOOL UnsetDarkModeNotepadWindow()
{
    if (!NotepadWindowHandle)
        return FALSE;

    HWND hWnd = NotepadWindowHandle;
    NotepadWindowHandle = NULL;

    UnsubclassNotepadWindow(hWnd);

    AllowDarkModeForWindow(hWnd, false);
    RefreshTitleBarThemeColor(hWnd);

    RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
    return TRUE;
}

HWND EditWindowHandle;

BOOL SetDarkModeEditWindow(HWND hWnd)
{
    EditWindowHandle = hWnd;

    SetWindowTheme(hWnd, L"DarkMode_Explorer", NULL);
    return TRUE;
}

BOOL UnsetDarkModeEditWindow()
{
    if (!EditWindowHandle)
        return FALSE;

    HWND hWnd = EditWindowHandle;
    EditWindowHandle = NULL;

    SetWindowTheme(hWnd, NULL, NULL);
    return TRUE;
}

BOOL CALLBACK FindCurrentProcessNotepadWindowEnumFunc(HWND hWnd, LPARAM lParam)
{
    DWORD dwProcessId = 0;
    if (!GetWindowThreadProcessId(hWnd, &dwProcessId) || dwProcessId != GetCurrentProcessId())
        return TRUE;

    WCHAR szClassName[16];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0 || wcsicmp(szClassName, L"Notepad") != 0)
        return TRUE;

    *(HWND*)lParam = hWnd;
    return FALSE;
}

HWND FindCurrentProcessNotepadWindow()
{
    HWND hNotepadWnd = NULL;
    EnumWindows(FindCurrentProcessNotepadWindowEnumFunc, (LPARAM)&hNotepadWnd);
    return hNotepadWnd;
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
    LPVOID lpParam)
{
    HWND hWnd = pOriginalCreateWindowExW(
        dwExStyle,
        lpClassName,
        lpWindowName,
        dwStyle,
        X,
        Y,
        nWidth,
        nHeight,
        hWndParent,
        hMenu,
        hInstance,
        lpParam
    );

    if (!hWnd)
        return hWnd;

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName)
        Wh_Log(L"Created window of class %s", lpClassName);
    else
        Wh_Log(L"Created window of class atom %X", (DWORD)(ULONG_PTR)lpClassName);

    if (bTextualClassName && wcsicmp(lpClassName, L"Notepad") == 0)
    {
        Wh_Log(L"Notepad window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        SetDarkModeNotepadWindow(hWnd);
    }
    else if (NotepadWindowHandle && hWndParent == NotepadWindowHandle)
    {
        if (bTextualClassName && wcsicmp(lpClassName, L"Edit") == 0)
        {
            Wh_Log(L"Edit window created: %08X", (DWORD)(ULONG_PTR)hWnd);
            SetDarkModeEditWindow(hWnd);
        }
        else if (bTextualClassName && wcsicmp(lpClassName, L"msctls_statusbar32") == 0)
        {
            Wh_Log(L"msctls_statusbar32 window created: %08X", (DWORD)(ULONG_PTR)hWnd);
            SubclassStatusBar(hWnd);
        }
    }

    return hWnd;
}

BOOL Wh_ModInit(void)
{
    Wh_Log(L"Init");

    InitDarkMode();
    SetDarkMode(true, false);

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExWHook, (void**)&pOriginalCreateWindowExW);

    WNDCLASS wndclass;
    if (GetClassInfo(GetModuleHandle(NULL), L"Notepad", &wndclass))
    {
        HWND hNotepadWnd = FindCurrentProcessNotepadWindow();
        if (hNotepadWnd)
        {
            SetDarkModeNotepadWindow(hNotepadWnd);

            HWND hEditWnd = FindWindowEx(hNotepadWnd, NULL, L"Edit", NULL);
            if (hEditWnd)
                SetDarkModeEditWindow(hEditWnd);

            HWND hStatusBarWnd = FindWindowEx(hNotepadWnd, NULL, L"msctls_statusbar32", NULL);
            if (hStatusBarWnd)
                SubclassStatusBar(hStatusBarWnd);
        }
    }

    return TRUE;
}

void Wh_ModUninit(void)
{
    Wh_Log(L"Uninit");

    SetDarkMode(false, false);
    UnsubclassStatusBar();
    UnsetDarkModeEditWindow();
    UnsetDarkModeNotepadWindow();
}

void Wh_ModSettingsChanged(void)
{
    Wh_Log(L"SettingsChanged");
}
