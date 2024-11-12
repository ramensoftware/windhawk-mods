// ==WindhawkMod==
// @id              desktop-icons-view
// @name            Desktop icons view
// @description     Change desktop icons view to list, details, small icons, or tiles
// @version         1.0.2
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomdlg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop icons view

Change desktop icons view to list, details, small icons, or tiles.

Based on [List view desktop](https://github.com/deanm/lvd).

### Acknowledgements

* **nvhhr**: Coming up with the mod idea, testing.

### Screenshots

![List](https://i.imgur.com/51q6OKl.png) \
*List*

![Details](https://i.imgur.com/a5qBigL.png) \
*Details*

![Small icons](https://i.imgur.com/PM4MfMs.png) \
*Small icons*

![Tiles](https://i.imgur.com/gqTawuD.png) \
*Tiles*
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- style: list
  $name: Desktop icons style
  $options:
  - list: List
  - details: Details
  - smallicon: Small icons
  - tile: Tiles
- colwidth: 500
  $name: Column Width
  $description: Sets the width of filenames
- monitor: 1
  $name: Monitor
  $description: The monitor number to have your desktop icons on
*/
// ==/WindhawkModSettings==

#include <commctrl.h>

struct {
    int style;
    int colwidth;
    int monitor;
} settings;

HWND FindChild(HWND parent, LPCWSTR cls, LPCWSTR win) {
    if (parent == nullptr) {
        return nullptr;
    }
    return FindWindowEx(parent, nullptr, cls, win);
}

bool IsFolderViewWnd(HWND hWnd) {
    WCHAR buffer[64];

    if (!GetClassName(hWnd, buffer, ARRAYSIZE(buffer)) ||
        _wcsicmp(buffer, L"SysListView32")) {
        return false;
    }

    if (!GetWindowText(hWnd, buffer, ARRAYSIZE(buffer)) ||
        _wcsicmp(buffer, L"FolderView")) {
        return false;
    }

    HWND hParentWnd = GetAncestor(hWnd, GA_PARENT);
    if (!hParentWnd) {
        return false;
    }

    if (!GetClassName(hParentWnd, buffer, ARRAYSIZE(buffer)) ||
        _wcsicmp(buffer, L"SHELLDLL_DefView")) {
        return false;
    }

    if (GetWindowTextLength(hParentWnd) > 0) {
        return false;
    }

    HWND hParentWnd2 = GetAncestor(hParentWnd, GA_PARENT);
    if (!hParentWnd2) {
        return false;
    }

    if ((!GetClassName(hParentWnd2, buffer, ARRAYSIZE(buffer)) ||
         _wcsicmp(buffer, L"Progman")) &&
        hParentWnd2 != GetShellWindow()) {
        return false;
    }

    return true;
}

HWND GetFolderViewWnd() {
    HWND hFolderFolderViewWnd =
        FindChild(FindChild(GetShellWindow(), L"SHELLDLL_DefView", L""),
                  L"SysListView32", L"FolderView");
    if (!hFolderFolderViewWnd) {
        return nullptr;
    }

    DWORD dwProcessId = 0;
    if (!GetWindowThreadProcessId(hFolderFolderViewWnd, &dwProcessId) ||
        dwProcessId != GetCurrentProcessId()) {
        return nullptr;
    }

    return hFolderFolderViewWnd;
}

HMONITOR GetMonitorById(int monitorId) {
    HMONITOR monitorResult = nullptr;
    int currentMonitorId = 0;

    auto monitorEnumProc = [&monitorResult, &currentMonitorId,
                            monitorId](HMONITOR hMonitor) -> BOOL {
        if (currentMonitorId == monitorId) {
            monitorResult = hMonitor;
            return FALSE;
        }
        currentMonitorId++;
        return TRUE;
    };

    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR hMonitor, HDC hdc, LPRECT lprcMonitor,
           LPARAM dwData) -> BOOL {
            auto& proc = *reinterpret_cast<decltype(monitorEnumProc)*>(dwData);
            return proc(hMonitor);
        },
        reinterpret_cast<LPARAM>(&monitorEnumProc));

    return monitorResult;
}

void SetDesktopStyle(HWND hFolderViewWnd, int view) {
    DWORD style = GetWindowLong(hFolderViewWnd, GWL_STYLE);
    if (view == LV_VIEW_ICON) {
        SendMessage(hFolderViewWnd, LVM_SETVIEW, LV_VIEW_ICON, 0);

        SetWindowLong(hFolderViewWnd, GWL_STYLE, style | LVS_NOSCROLL);
    } else {
        // Setup the style (removing noscroll), otherwise list style doesn't
        // seem to work correctly (docs say noscroll is incompatible with list).
        SetWindowLong(hFolderViewWnd, GWL_STYLE, style & ~LVS_NOSCROLL);
        // Switch view.
        SendMessage(hFolderViewWnd, LVM_SETVIEW, view, 0);
        SendMessage(hFolderViewWnd, LVM_SETEXTENDEDLISTVIEWSTYLE,
                    LVS_EX_DOUBLEBUFFER, 0);
        // Set the column width.
        ListView_SetColumnWidth(hFolderViewWnd, 0, settings.colwidth);
    }

    // Get working area of desktop and position desktop window accordingly.
    HMONITOR monitor = GetMonitorById(settings.monitor - 1);
    if (!monitor) {
        monitor = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTONEAREST);
    }

    MONITORINFO monitorInfo{
        .cbSize = sizeof(monitorInfo),
    };
    if (GetMonitorInfo(monitor, &monitorInfo)) {
        RECT rc = monitorInfo.rcWork;
        MapWindowPoints(nullptr, GetAncestor(hFolderViewWnd, GA_PARENT),
                        (POINT*)(&rc), sizeof(RECT) / sizeof(POINT));
        int x = rc.left;
        int y = rc.top;
        int cx = rc.right - rc.left;
        int cy = rc.bottom - rc.top;
        SetWindowPos(hFolderViewWnd, nullptr, x, y, cx, cy, SWP_NOZORDER);
    }
    // Refresh.
    UpdateWindow(hFolderViewWnd);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
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
                                 PVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd || !IsFolderViewWnd(hWnd)) {
        return hWnd;
    }

    Wh_Log(L"FolderView window created: %08X", (DWORD)(ULONG_PTR)hWnd);
    // SetDesktopStyle(hWnd, settings.style);

    static UINT s_timer = 0;
    static HWND s_hWnd;
    s_hWnd = hWnd;
    s_timer = SetTimer(nullptr, s_timer, 1000,
                       [](HWND hwnd,  // handle of window for timer messages
                          UINT uMsg,  // WM_TIMER message
                          UINT_PTR idEvent,  // timer identifier
                          DWORD dwTime       // current system time
                          ) WINAPI {
                           Wh_Log(L">");

                           KillTimer(nullptr, s_timer);
                           s_timer = 0;

                           SetDesktopStyle(s_hWnd, settings.style);
                           s_hWnd = nullptr;
                       });

    return hWnd;
}

void LoadSettings() {
    PCWSTR style = Wh_GetStringSetting(L"style");
    settings.style = LV_VIEW_ICON;
    if (wcscmp(style, L"details") == 0) {
        settings.style = LV_VIEW_DETAILS;
    } else if (wcscmp(style, L"smallicon") == 0) {
        settings.style = LV_VIEW_SMALLICON;
    } else if (wcscmp(style, L"list") == 0) {
        settings.style = LV_VIEW_LIST;
    } else if (wcscmp(style, L"tile") == 0) {
        settings.style = LV_VIEW_TILE;
    }
    Wh_FreeStringSetting(style);

    settings.colwidth = Wh_GetIntSetting(L"colwidth");
    settings.monitor = Wh_GetIntSetting(L"monitor");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (HWND hFolderFolderViewWnd = GetFolderViewWnd()) {
        SetDesktopStyle(hFolderFolderViewWnd, settings.style);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (HWND hFolderFolderViewWnd = GetFolderViewWnd()) {
        SetDesktopStyle(hFolderFolderViewWnd, LV_VIEW_ICON);
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L"Settings changed");
    *bReload = TRUE;
    return TRUE;
}
