// ==WindhawkMod==
// @id              toggle-desktop-icons-dblclick
// @name            Toggle Desktop Icons on Taskbar Double-Click
// @description     Double-click on an empty area of the taskbar to hide or show desktop icons
// @version         1.0
// @author          MattShadd
// @github          https://github.com/MattShaddd
// @include         explorer.exe
// @compilerOptions -DWINVER=0x0A00 -lcomctl32 -lversion -lgdi32 -ldwmapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Toggle Desktop Icons on Taskbar Double-Click

Quickly hide and show desktop icons by double-clicking on an empty area of the
taskbar. A sleek notification popup confirms the current state.

## How it works

- **Double-click** on any empty area of the taskbar → desktop icons are hidden.
- **Double-click** again → desktop icons are restored.
- A small dark popup appears briefly above the taskbar to confirm the action.
- Clicks on taskbar buttons, the system tray, clock, and other interactive
  elements are not affected — they work as usual.

## Features

- Works on **Windows 10** and **Windows 11** (both old and new taskbar).
- Supports **multi-monitor** setups (secondary taskbars included).
- Lightweight — no background threads, no global hooks, no overlay windows.
- Notification popup with smooth fade-in / fade-out animation.
- Desktop icons are automatically restored when the mod is disabled or
  uninstalled.

## Technical details

The mod uses `WindhawkUtils::SetWindowSubclassFromAnyThread` to subclass
taskbar windows, and hooks `CreateWindowExW` / `CreateWindowInBand` to detect
newly created taskbar windows. On Windows 11, the mod also hooks the
`InputSite.WindowClass` window procedure to intercept `WM_POINTERDOWN` events
used by the new XAML-based taskbar.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <commctrl.h>
#include <dwmapi.h>
#include <windowsx.h>

#include <unordered_set>

// ── Globals ─────────────────────────────────────────────────────────────────

static bool g_desktopIconsHidden = false;
static HWND g_hTaskbarWnd = nullptr;
static DWORD g_dwTaskbarThreadId = 0;
static bool g_inputSiteProcHooked = false;
static std::unordered_set<HWND> g_secondaryTaskbarWindows;
static bool g_initialized = false;

// Double-click tracking
static DWORD g_lastClickTime = 0;
static POINT g_lastClickPt = {0, 0};

// Windows version detection
static int g_nWinVersion = 0;

enum {
    WIN_VERSION_UNSUPPORTED = 0,
    WIN_VERSION_10 = 1,
    WIN_VERSION_11_21H2 = 2,
    WIN_VERSION_11_22H2 = 3,
};

#if defined(__GNUC__) && __GNUC__ > 8
#define WINAPI_LAMBDA_RETURN(return_t) ->return_t WINAPI
#elif defined(__GNUC__)
#define WINAPI_LAMBDA_RETURN(return_t) WINAPI->return_t
#else
#define WINAPI_LAMBDA_RETURN(return_t) ->return_t
#endif

#ifndef WM_POINTERDOWN
#define WM_POINTERDOWN 0x0246
#endif

// ── Version detection ───────────────────────────────────────────────────────

static VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule,
                                               UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) *puPtrLen = uPtrLen;
    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

static BOOL WindowsVersionInit() {
    VS_FIXEDFILEINFO* pFixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!pFixedFileInfo) return FALSE;

    WORD nMajor = HIWORD(pFixedFileInfo->dwFileVersionMS);
    WORD nBuild = HIWORD(pFixedFileInfo->dwFileVersionLS);

    if (nMajor == 10) {
        if (nBuild >= 22621)
            g_nWinVersion = WIN_VERSION_11_22H2;
        else if (nBuild >= 22000)
            g_nWinVersion = WIN_VERSION_11_21H2;
        else
            g_nWinVersion = WIN_VERSION_10;
    } else {
        g_nWinVersion = WIN_VERSION_10;
    }

    return TRUE;
}

// ── Taskbar detection ───────────────────────────────────────────────────────

static bool IsTaskbarWindow(HWND hWnd) {
    WCHAR szClassName[32];
    if (!GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName))) return false;
    return _wcsicmp(szClassName, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0;
}

// ── Notification popup ──────────────────────────────────────────────────────

static const wchar_t* NOTIFY_CLASS = L"WHDesktopToggleNotify";
static HWND g_hNotifyWnd = nullptr;
static UINT_PTR g_notifyFadeTimer = 0;
static int g_notifyAlpha = 0;
static bool g_notifyFadingIn = false;
static DWORD g_notifyShowTime = 0;
static const int NOTIFY_DISPLAY_MS = 1200;
static const int NOTIFY_FADE_STEP = 15;
static const int NOTIFY_TIMER_MS = 16;
static const int NOTIFY_WIDTH = 240;
static const int NOTIFY_HEIGHT = 48;
static const int NOTIFY_CORNER_RADIUS = 12;
static const int NOTIFY_MARGIN_BOTTOM = 12;

static void CALLBACK NotifyTimerProc(HWND, UINT, UINT_PTR, DWORD) {
    if (!g_hNotifyWnd) {
        if (g_notifyFadeTimer) {
            KillTimer(nullptr, g_notifyFadeTimer);
            g_notifyFadeTimer = 0;
        }
        return;
    }

    if (g_notifyFadingIn) {
        g_notifyAlpha += NOTIFY_FADE_STEP;
        if (g_notifyAlpha >= 230) {
            g_notifyAlpha = 230;
            g_notifyFadingIn = false;
            g_notifyShowTime = GetTickCount();
        }
        SetLayeredWindowAttributes(g_hNotifyWnd, 0, (BYTE)g_notifyAlpha,
                                   LWA_ALPHA);
    } else {
        if (GetTickCount() - g_notifyShowTime >= (DWORD)NOTIFY_DISPLAY_MS) {
            g_notifyAlpha -= NOTIFY_FADE_STEP;
            if (g_notifyAlpha <= 0) {
                g_notifyAlpha = 0;
                KillTimer(nullptr, g_notifyFadeTimer);
                g_notifyFadeTimer = 0;
                DestroyWindow(g_hNotifyWnd);
                g_hNotifyWnd = nullptr;
                return;
            }
            SetLayeredWindowAttributes(g_hNotifyWnd, 0, (BYTE)g_notifyAlpha,
                                       LWA_ALPHA);
        }
    }
}

static LRESULT CALLBACK NotifyWndProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                       LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            RECT rc;
            GetClientRect(hWnd, &rc);

            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hBmp =
                CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
            HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hBmp);

            // Dark rounded background
            HBRUSH hBrush = CreateSolidBrush(RGB(32, 32, 36));
            HPEN hPen = CreatePen(PS_SOLID, 1, RGB(60, 60, 66));
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdcMem, hBrush);
            HPEN hOldPen = (HPEN)SelectObject(hdcMem, hPen);
            RoundRect(hdcMem, 0, 0, rc.right, rc.bottom,
                      NOTIFY_CORNER_RADIUS, NOTIFY_CORNER_RADIUS);
            SelectObject(hdcMem, hOldBrush);
            SelectObject(hdcMem, hOldPen);
            DeleteObject(hBrush);
            DeleteObject(hPen);

            // Icon: filled circle = visible, empty circle = hidden
            const wchar_t* icon =
                g_desktopIconsHidden ? L"\x25CB" : L"\x25CF";
            HFONT hIconFont =
                CreateFontW(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                            CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                            DEFAULT_PITCH, L"Segoe UI");
            HFONT hOldFont = (HFONT)SelectObject(hdcMem, hIconFont);
            SetTextColor(hdcMem, RGB(220, 220, 225));
            SetBkMode(hdcMem, TRANSPARENT);
            RECT rcIcon = {10, 0, 36, rc.bottom};
            DrawTextW(hdcMem, icon, -1, &rcIcon,
                      DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            SelectObject(hdcMem, hOldFont);
            DeleteObject(hIconFont);

            // Text
            const wchar_t* text = g_desktopIconsHidden
                                      ? L"Desktop Icons Hidden"
                                      : L"Desktop Icons Visible";
            HFONT hTextFont =
                CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                            CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                            DEFAULT_PITCH, L"Segoe UI");
            hOldFont = (HFONT)SelectObject(hdcMem, hTextFont);
            SetTextColor(hdcMem, RGB(220, 220, 225));
            RECT rcText = {38, 0, rc.right - 10, rc.bottom};
            DrawTextW(hdcMem, text, -1, &rcText,
                      DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            SelectObject(hdcMem, hOldFont);
            DeleteObject(hTextFont);

            BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hOldBmp);
            DeleteObject(hBmp);
            DeleteDC(hdcMem);

            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_NCHITTEST:
            return HTTRANSPARENT;

        case WM_DESTROY:
            if (g_hNotifyWnd == hWnd) g_hNotifyWnd = nullptr;
            return 0;
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

static void ShowNotification() {
    static bool classRegistered = false;
    if (!classRegistered) {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = NotifyWndProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.lpszClassName = NOTIFY_CLASS;
        wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
        if (RegisterClassExW(&wc) ||
            GetLastError() == ERROR_CLASS_ALREADY_EXISTS) {
            classRegistered = true;
        } else {
            return;
        }
    }

    HWND hTaskbar = g_hTaskbarWnd;
    if (!hTaskbar) hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);

    RECT rcTaskbar;
    if (!hTaskbar || !GetWindowRect(hTaskbar, &rcTaskbar)) return;

    HMONITOR hMon = MonitorFromWindow(hTaskbar, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(hMon, &mi);

    int x = mi.rcWork.left +
            (mi.rcWork.right - mi.rcWork.left - NOTIFY_WIDTH) / 2;
    int y = rcTaskbar.top - NOTIFY_HEIGHT - NOTIFY_MARGIN_BOTTOM;

    // If taskbar is at top, place below it
    if (rcTaskbar.top <= mi.rcWork.top) {
        y = rcTaskbar.bottom + NOTIFY_MARGIN_BOTTOM;
    }

    if (g_hNotifyWnd) {
        if (g_notifyFadeTimer) {
            KillTimer(nullptr, g_notifyFadeTimer);
            g_notifyFadeTimer = 0;
        }
        DestroyWindow(g_hNotifyWnd);
        g_hNotifyWnd = nullptr;
    }

    g_hNotifyWnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        NOTIFY_CLASS, L"", WS_POPUP,
        x, y, NOTIFY_WIDTH, NOTIFY_HEIGHT,
        nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);

    if (!g_hNotifyWnd) return;

    g_notifyAlpha = 0;
    g_notifyFadingIn = true;
    SetLayeredWindowAttributes(g_hNotifyWnd, 0, 0, LWA_ALPHA);
    ShowWindow(g_hNotifyWnd, SW_SHOWNOACTIVATE);

    if (g_nWinVersion >= WIN_VERSION_11_21H2) {
        DWM_WINDOW_CORNER_PREFERENCE pref = DWMWCP_ROUND;
        DwmSetWindowAttribute(g_hNotifyWnd, DWMWA_WINDOW_CORNER_PREFERENCE,
                              &pref, sizeof(pref));
    }

    g_notifyFadeTimer =
        SetTimer(nullptr, g_notifyFadeTimer, NOTIFY_TIMER_MS, NotifyTimerProc);
}

// ── Desktop icon toggle ─────────────────────────────────────────────────────

static HWND FindDesktopShellView() {
    HWND hProgman = FindWindowW(L"Progman", nullptr);
    if (hProgman) {
        HWND hShellView =
            FindWindowExW(hProgman, nullptr, L"SHELLDLL_DefView", nullptr);
        if (hShellView) return hShellView;
    }

    HWND hWorkerW = nullptr;
    while ((hWorkerW = FindWindowExW(nullptr, hWorkerW, L"WorkerW",
                                      nullptr)) != nullptr) {
        HWND hShellView =
            FindWindowExW(hWorkerW, nullptr, L"SHELLDLL_DefView", nullptr);
        if (hShellView) return hShellView;
    }
    return nullptr;
}

static void ToggleDesktopIcons() {
    HWND hShellView = FindDesktopShellView();
    if (!hShellView) {
        Wh_Log(L"SHELLDLL_DefView not found");
        return;
    }

    HWND hListView =
        FindWindowExW(hShellView, nullptr, L"SysListView32", nullptr);
    if (!hListView) {
        Wh_Log(L"SysListView32 not found");
        return;
    }

    g_desktopIconsHidden = !g_desktopIconsHidden;
    ShowWindow(hListView, g_desktopIconsHidden ? SW_HIDE : SW_SHOW);
    Wh_Log(L"Desktop icons %s", g_desktopIconsHidden ? L"hidden" : L"shown");

    ShowNotification();
}

// ── Double-click detection ──────────────────────────────────────────────────

static bool CheckAndHandleDoubleClick(POINT ptScreen) {
    DWORD now = GetTickCount();
    DWORD dblClickTime = GetDoubleClickTime();
    int cxDblClk = GetSystemMetrics(SM_CXDOUBLECLK);
    int cyDblClk = GetSystemMetrics(SM_CYDOUBLECLK);

    int dx = abs(ptScreen.x - g_lastClickPt.x);
    int dy = abs(ptScreen.y - g_lastClickPt.y);
    DWORD elapsed = now - g_lastClickTime;

    if (elapsed <= dblClickTime &&
        dx <= cxDblClk / 2 &&
        dy <= cyDblClk / 2) {
        g_lastClickTime = 0;
        g_lastClickPt = {0, 0};
        Wh_Log(L"Double-click on taskbar at (%d, %d)", ptScreen.x,
               ptScreen.y);
        ToggleDesktopIcons();
        return true;
    }

    g_lastClickTime = now;
    g_lastClickPt = ptScreen;
    return false;
}

static bool IsClickOnEmptyArea(HWND hTaskbarWnd, POINT ptScreen) {
    HWND hChild = WindowFromPoint(ptScreen);
    if (!hChild) return false;

    HWND hCheck = hChild;
    bool insideTaskbar = false;
    for (int i = 0; i < 20 && hCheck; i++) {
        if (hCheck == hTaskbarWnd) {
            insideTaskbar = true;
            break;
        }
        hCheck = GetParent(hCheck);
    }
    if (!insideTaskbar) return false;

    if (hChild == hTaskbarWnd) return true;

    WCHAR szClassName[256] = {};
    GetClassNameW(hChild, szClassName, 256);

    if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0 ||
        _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0 ||
        _wcsicmp(szClassName, L"ReBarWindow32") == 0 ||
        _wcsicmp(szClassName, L"MSTaskSwWClass") == 0 ||
        _wcsicmp(szClassName, L"MSTaskListWClass") == 0 ||
        _wcsicmp(szClassName, L"WorkerW") == 0 ||
        _wcsicmp(szClassName,
                 L"Windows.UI.Composition.DesktopWindowContentBridge") == 0 ||
        _wcsicmp(szClassName,
                 L"Windows.UI.Input.InputSite.WindowClass") == 0) {
        return true;
    }

    return false;
}

// ── Handle left-click on taskbar ────────────────────────────────────────────

static bool OnTaskbarLButtonDown(HWND hWnd, POINT ptScreen) {
    if (!IsClickOnEmptyArea(hWnd, ptScreen)) {
        g_lastClickTime = 0;
        g_lastClickPt = {0, 0};
        return false;
    }

    return CheckAndHandleDoubleClick(ptScreen);
}

// ── Taskbar subclass proc ───────────────────────────────────────────────────

static LRESULT CALLBACK TaskbarWindowSubclassProc(_In_ HWND hWnd,
                                                   _In_ UINT uMsg,
                                                   _In_ WPARAM wParam,
                                                   _In_ LPARAM lParam,
                                                   _In_ DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_LBUTTONDOWN: {
            POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            ClientToScreen(hWnd, &pt);
            if (OnTaskbarLButtonDown(hWnd, pt)) {
                return 0;
            }
            break;
        }

        case WM_NCDESTROY:
            if (hWnd != g_hTaskbarWnd) {
                g_secondaryTaskbarWindows.erase(hWnd);
            }
            break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ── InputSite hook for Win11 new taskbar ────────────────────────────────────

static WNDPROC InputSiteWindowProc_Original;
static LRESULT CALLBACK InputSiteWindowProc_Hook(HWND hWnd,
                                                  UINT uMsg,
                                                  WPARAM wParam,
                                                  LPARAM lParam) {
    switch (uMsg) {
        case WM_POINTERDOWN: {
            POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
            if (hRootWnd && IsTaskbarWindow(hRootWnd)) {
                if (OnTaskbarLButtonDown(hRootWnd, pt)) {
                    return 0;
                }
            }
            break;
        }
    }

    return InputSiteWindowProc_Original(hWnd, uMsg, wParam, lParam);
}

// ── Subclass management ─────────────────────────────────────────────────────

static void SubclassTaskbarWindow(HWND hWnd) {
    WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd,
                                                  TaskbarWindowSubclassProc, 0);
}

static void UnsubclassTaskbarWindow(HWND hWnd) {
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(
        hWnd, TaskbarWindowSubclassProc);
}

static void HandleIdentifiedInputSiteWindow(HWND hWnd) {
    if (!g_dwTaskbarThreadId ||
        GetWindowThreadProcessId(hWnd, nullptr) != g_dwTaskbarThreadId) {
        return;
    }

    HWND hParentWnd = GetParent(hWnd);
    WCHAR szClassName[64];
    if (!hParentWnd ||
        !GetClassName(hParentWnd, szClassName, ARRAYSIZE(szClassName)) ||
        _wcsicmp(szClassName,
                 L"Windows.UI.Composition.DesktopWindowContentBridge") != 0) {
        return;
    }

    hParentWnd = GetParent(hParentWnd);
    if (!hParentWnd || !IsTaskbarWindow(hParentWnd)) {
        return;
    }

    auto wndProc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
    WindhawkUtils::Wh_SetFunctionHookT(wndProc, InputSiteWindowProc_Hook,
                                       &InputSiteWindowProc_Original);

    if (g_initialized) {
        Wh_ApplyHookOperations();
    }

    Wh_Log(L"Hooked InputSite wndproc %p", wndProc);
    g_inputSiteProcHooked = true;
}

static void HandleIdentifiedTaskbarWindow(HWND hWnd) {
    g_hTaskbarWnd = hWnd;
    g_dwTaskbarThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    SubclassTaskbarWindow(hWnd);

    for (HWND hSecondaryWnd : g_secondaryTaskbarWindows) {
        SubclassTaskbarWindow(hSecondaryWnd);
    }

    if (g_nWinVersion >= WIN_VERSION_11_21H2 && !g_inputSiteProcHooked) {
        HWND hXamlIslandWnd = FindWindowEx(
            hWnd, nullptr,
            L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
        if (hXamlIslandWnd) {
            HWND hInputSiteWnd = FindWindowEx(
                hXamlIslandWnd, nullptr,
                L"Windows.UI.Input.InputSite.WindowClass", nullptr);
            if (hInputSiteWnd) {
                HandleIdentifiedInputSiteWindow(hInputSiteWnd);
            }
        }
    }
}

static void HandleIdentifiedSecondaryTaskbarWindow(HWND hWnd) {
    if (!g_dwTaskbarThreadId ||
        GetWindowThreadProcessId(hWnd, nullptr) != g_dwTaskbarThreadId) {
        return;
    }

    g_secondaryTaskbarWindows.insert(hWnd);
    SubclassTaskbarWindow(hWnd);

    if (g_nWinVersion >= WIN_VERSION_11_21H2 && !g_inputSiteProcHooked) {
        HWND hXamlIslandWnd = FindWindowEx(
            hWnd, nullptr,
            L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
        if (hXamlIslandWnd) {
            HWND hInputSiteWnd = FindWindowEx(
                hXamlIslandWnd, nullptr,
                L"Windows.UI.Input.InputSite.WindowClass", nullptr);
            if (hInputSiteWnd) {
                HandleIdentifiedInputSiteWindow(hInputSiteWnd);
            }
        }
    }
}

// ── Window creation hooks ───────────────────────────────────────────────────

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X, int Y,
                                 int nWidth, int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd) return hWnd;

    BOOL bTextualClassName =
        ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName && _wcsicmp(lpClassName, L"Shell_TrayWnd") == 0) {
        Wh_Log(L"Taskbar window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedTaskbarWindow(hWnd);
    } else if (bTextualClassName &&
               _wcsicmp(lpClassName, L"Shell_SecondaryTrayWnd") == 0) {
        Wh_Log(L"Secondary taskbar window created: %08X",
               (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedSecondaryTaskbarWindow(hWnd);
    }

    return hWnd;
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD dwExStyle,
                                           LPCWSTR lpClassName,
                                           LPCWSTR lpWindowName,
                                           DWORD dwStyle,
                                           int X, int Y,
                                           int nWidth, int nHeight,
                                           HWND hWndParent,
                                           HMENU hMenu,
                                           HINSTANCE hInstance,
                                           LPVOID lpParam,
                                           DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle,
                                    LPCWSTR lpClassName,
                                    LPCWSTR lpWindowName,
                                    DWORD dwStyle,
                                    int X, int Y,
                                    int nWidth, int nHeight,
                                    HWND hWndParent,
                                    HMENU hMenu,
                                    HINSTANCE hInstance,
                                    LPVOID lpParam,
                                    DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd) return hWnd;

    BOOL bTextualClassName =
        ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName &&
        _wcsicmp(lpClassName, L"Windows.UI.Input.InputSite.WindowClass") == 0) {
        Wh_Log(L"InputSite window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        if (g_nWinVersion >= WIN_VERSION_11_21H2 && !g_inputSiteProcHooked) {
            HandleIdentifiedInputSiteWindow(hWnd);
        }
    }

    return hWnd;
}

// ── Find existing taskbar windows ───────────────────────────────────────────

static HWND FindCurrentProcessTaskbarWindows(
    std::unordered_set<HWND>* secondaryTaskbarWindows) {
    struct ENUM_WINDOWS_PARAM {
        HWND* hWnd;
        std::unordered_set<HWND>* secondaryTaskbarWindows;
    };

    HWND hWnd = nullptr;
    ENUM_WINDOWS_PARAM param = {&hWnd, secondaryTaskbarWindows};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI_LAMBDA_RETURN(BOOL) {
            ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId())
                return TRUE;

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0)
                return TRUE;

            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
                *param.hWnd = hWnd;
            } else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
                param.secondaryTaskbarWindows->insert(hWnd);
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnd;
}

// ── Windhawk entry points ───────────────────────────────────────────────────

BOOL Wh_ModInit() {
    Wh_Log(L"Init: Toggle Desktop Icons on Taskbar Double-Click v1.0");

    if (!WindowsVersionInit()) {
        Wh_Log(L"Version detection failed, assuming Win10");
        g_nWinVersion = WIN_VERSION_10;
    }

    // Detect current icon state
    HWND hShellView = FindDesktopShellView();
    if (hShellView) {
        HWND hListView =
            FindWindowExW(hShellView, nullptr, L"SysListView32", nullptr);
        if (hListView) {
            g_desktopIconsHidden = !IsWindowVisible(hListView);
        }
    }

    // Hook CreateWindowExW to catch taskbar window creation
    WindhawkUtils::Wh_SetFunctionHookT(CreateWindowExW, CreateWindowExW_Hook,
                                       &CreateWindowExW_Original);

    // Hook CreateWindowInBand for Win11 InputSite windows
    HMODULE user32Module =
        LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32Module) {
        auto pCreateWindowInBand = (CreateWindowInBand_t)GetProcAddress(
            user32Module, "CreateWindowInBand");
        if (pCreateWindowInBand) {
            WindhawkUtils::Wh_SetFunctionHookT(pCreateWindowInBand,
                                               CreateWindowInBand_Hook,
                                               &CreateWindowInBand_Original);
        }
    }

    g_initialized = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"AfterInit");

    WNDCLASS wndclass;
    if (GetClassInfo(GetModuleHandle(nullptr), L"Shell_TrayWnd", &wndclass)) {
        HWND hWnd =
            FindCurrentProcessTaskbarWindows(&g_secondaryTaskbarWindows);
        if (hWnd) {
            HandleIdentifiedTaskbarWindow(hWnd);
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    // Destroy notification popup
    if (g_notifyFadeTimer) {
        KillTimer(nullptr, g_notifyFadeTimer);
        g_notifyFadeTimer = 0;
    }
    if (g_hNotifyWnd) {
        DestroyWindow(g_hNotifyWnd);
        g_hNotifyWnd = nullptr;
    }

    if (g_hTaskbarWnd) {
        UnsubclassTaskbarWindow(g_hTaskbarWnd);
        for (HWND hSecondaryWnd : g_secondaryTaskbarWindows) {
            UnsubclassTaskbarWindow(hSecondaryWnd);
        }
    }

    // Restore desktop icons if hidden
    if (g_desktopIconsHidden) {
        HWND hShellView = FindDesktopShellView();
        if (hShellView) {
            HWND hListView =
                FindWindowExW(hShellView, nullptr, L"SysListView32", nullptr);
            if (hListView) ShowWindow(hListView, SW_SHOW);
        }
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
}
