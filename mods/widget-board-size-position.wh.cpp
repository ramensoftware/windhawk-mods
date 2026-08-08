// ==WindhawkMod==
// @id              widget-board-size-position
// @name            Widget Board Size and Position
// @description     Set a custom size and position for the new Widget Board on Windows 11.
// @version         0.1
// @author          meteoni
// @include         WidgetBoard.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Widget Board Size and Position

Set a custom size and position for the new Widget Board on Windows 11.

Allows you to override the default position and dimensions of the new WinUI 3 Widget Board (`WidgetBoard.exe`).

![Screenshot](https://raw.githubusercontent.com/Meteony/meteoni-assets/main/widget-board-size-position/wboard-size.png)
_Example for making the Widget Board smaller_

Width, height, and offsets are specified in DIPs (96-DPI logical pixels), so a
700x600 setting becomes 1400x1200 native pixels on a 200% display.

`horizontalAnchor` accepts: `system`, `left`, `center`, `right`.
`verticalAnchor` accepts: `system`, `top`, `center`, `bottom`.

When an anchor is `system`, Windows' requested position is preserved on that
axis. Positive offsets move inward from left/top/right/bottom; for `center`,
they move right/down from center.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- width: 700
  $name: Width
  $description: Width in DIPs. Set to 0 to preserve Windows' width.
- height: 600
  $name: Height
  $description: Height in DIPs. Set to 0 to preserve Windows' height.
- horizontalAnchor: left
  $name: Horizontal anchor
  $description: "Where Widget Board shows up horizontally."
  $options:
    - system: Default
    - left: Left
    - center: Center
    - right: Right
- verticalAnchor: top
  $name: Vertical anchor
  $description: "Where Widget Board shows up vertically."
  $options:
    - system: Default
    - top: Top
    - center: Center
    - bottom: Bottom
- offsetX: 0
  $name: Horizontal offset
  $description: Offset in DIPs. Positive values move inward from an edge.
- offsetY: 0
  $name: Vertical offset
  $description: Offset in DIPs. Positive values move inward from an edge.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <windhawk_utils.h>

#include <atomic>
#include <cwchar>

namespace {

enum class HAnchor : int {
    System,
    Left,
    Center,
    Right,
};

enum class VAnchor : int {
    System,
    Top,
    Center,
    Bottom,
};

struct Settings {
    std::atomic<int> widthDip{700};
    std::atomic<int> heightDip{600};
    std::atomic<int> offsetXDip{0};
    std::atomic<int> offsetYDip{0};
    std::atomic<int> hAnchor{static_cast<int>(HAnchor::Left)};
    std::atomic<int> vAnchor{static_cast<int>(VAnchor::Top)};
};

Settings g_settings;
std::atomic<bool> g_unloading{false};

HANDLE g_winEventThread = nullptr;
DWORD g_winEventThreadId = 0;
HANDLE g_winEventThreadReady = nullptr;
std::atomic<bool> g_winEventHookInstalled{false};

constexpr wchar_t kBoardClass[] = L"WindowsDashboard";

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------

HAnchor ParseHAnchor(PCWSTR value) {
    if (_wcsicmp(value, L"system") == 0)
        return HAnchor::System;
    if (_wcsicmp(value, L"center") == 0)
        return HAnchor::Center;
    if (_wcsicmp(value, L"right") == 0)
        return HAnchor::Right;
    return HAnchor::Left;
}

VAnchor ParseVAnchor(PCWSTR value) {
    if (_wcsicmp(value, L"system") == 0)
        return VAnchor::System;
    if (_wcsicmp(value, L"center") == 0)
        return VAnchor::Center;
    if (_wcsicmp(value, L"bottom") == 0)
        return VAnchor::Bottom;
    return VAnchor::Top;
}

void LoadSettings() {
    g_settings.widthDip = Wh_GetIntSetting(L"width");
    g_settings.heightDip = Wh_GetIntSetting(L"height");
    g_settings.offsetXDip = Wh_GetIntSetting(L"offsetX");
    g_settings.offsetYDip = Wh_GetIntSetting(L"offsetY");

    g_settings.hAnchor = static_cast<int>(ParseHAnchor(
        WindhawkUtils::StringSetting::make(L"horizontalAnchor").get()));
    g_settings.vAnchor = static_cast<int>(ParseVAnchor(
        WindhawkUtils::StringSetting::make(L"verticalAnchor").get()));
}

// -----------------------------------------------------------------------------
// Target discovery
// -----------------------------------------------------------------------------

bool IsBoardWindow(HWND hwnd) {
    if (!IsWindow(hwnd) || GetAncestor(hwnd, GA_ROOT) != hwnd)
        return false;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId())
        return false;

    WCHAR className[128]{};
    if (!GetClassName(hwnd, className, ARRAYSIZE(className)))
        return false;

    return _wcsicmp(className, kBoardClass) == 0;
}

using WindowCallback_t = void (*)(HWND hwnd);

void ForEachTopLevelWindow(WindowCallback_t callback) {
    struct EnumData {
        WindowCallback_t callback;
    } data{callback};

    EnumWindows(
        [](HWND hwnd, LPARAM lParam) -> BOOL {
            auto* data = reinterpret_cast<EnumData*>(lParam);
            data->callback(hwnd);
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&data));
}

// -----------------------------------------------------------------------------
// Geometry
// -----------------------------------------------------------------------------

int DipToPx(HWND hwnd, int dip) {
    UINT dpi = GetDpiForWindow(hwnd);
    if (!dpi)
        dpi = 96;
    return MulDiv(dip, static_cast<int>(dpi), 96);
}

HMONITOR GetRequestedMonitor(HWND hwnd, const WINDOWPOS* wp) {
    if (wp && !(wp->flags & SWP_NOMOVE)) {
        POINT pt{wp->x, wp->y};
        return MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    }

    return MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
}

void ForceGeometry(HWND hwnd, WINDOWPOS* wp) {
    if (!wp || g_unloading)
        return;

    RECT current{};
    if (!GetWindowRect(hwnd, &current))
        return;

    const int currentWidth = current.right - current.left;
    const int currentHeight = current.bottom - current.top;

    const int widthDip = g_settings.widthDip.load();
    const int heightDip = g_settings.heightDip.load();
    const int offsetXDip = g_settings.offsetXDip.load();
    const int offsetYDip = g_settings.offsetYDip.load();
    const HAnchor hAnchor =
        static_cast<HAnchor>(g_settings.hAnchor.load());
    const VAnchor vAnchor =
        static_cast<VAnchor>(g_settings.vAnchor.load());

    // Start with the geometry Windows requested. If that axis wasn't included in
    // this WINDOWPOS operation, use the current window geometry instead.
    int width = (wp->flags & SWP_NOSIZE) ? currentWidth : wp->cx;
    int height = (wp->flags & SWP_NOSIZE) ? currentHeight : wp->cy;
    int x = (wp->flags & SWP_NOMOVE) ? current.left : wp->x;
    int y = (wp->flags & SWP_NOMOVE) ? current.top : wp->y;

    if (widthDip > 0)
        width = DipToPx(hwnd, widthDip);
    if (heightDip > 0)
        height = DipToPx(hwnd, heightDip);

    HMONITOR monitor = GetRequestedMonitor(hwnd, wp);
    MONITORINFO mi{sizeof(mi)};

    if (GetMonitorInfo(monitor, &mi)) {
        const int offsetX = DipToPx(hwnd, offsetXDip);
        const int offsetY = DipToPx(hwnd, offsetYDip);

        switch (hAnchor) {
            case HAnchor::Left:
                x = mi.rcWork.left + offsetX;
                break;

            case HAnchor::Center:
                x = mi.rcWork.left +
                    ((mi.rcWork.right - mi.rcWork.left) - width) / 2 + offsetX;
                break;

            case HAnchor::Right:
                x = mi.rcWork.right - width - offsetX;
                break;

            case HAnchor::System:
                break;
        }

        switch (vAnchor) {
            case VAnchor::Top:
                y = mi.rcWork.top + offsetY;
                break;

            case VAnchor::Center:
                y = mi.rcWork.top +
                    ((mi.rcWork.bottom - mi.rcWork.top) - height) / 2 + offsetY;
                break;

            case VAnchor::Bottom:
                y = mi.rcWork.bottom - height - offsetY;
                break;

            case VAnchor::System:
                break;
        }
    }

    // If we override either dimension, make sure this operation is allowed to
    // resize. The same idea applies to anchored position axes. This matters on
    // reopen because WidgetBoard may issue a move-only or size-only operation.
    if (widthDip > 0 || heightDip > 0)
        wp->flags &= ~SWP_NOSIZE;

    if (hAnchor != HAnchor::System || vAnchor != VAnchor::System)
        wp->flags &= ~SWP_NOMOVE;

    wp->x = x;
    wp->y = y;
    wp->cx = width;
    wp->cy = height;
}

LRESULT CALLBACK BoardSubclassProc(HWND hwnd,
                                   UINT msg,
                                   WPARAM wParam,
                                   LPARAM lParam,
                                   DWORD_PTR) {
    switch (msg) {
        case WM_WINDOWPOSCHANGING:
            ForceGeometry(hwnd, reinterpret_cast<WINDOWPOS*>(lParam));
            break;

        case WM_NCDESTROY:
            Wh_Log(L"WindowsDashboard destroyed: hwnd=%p", hwnd);
            break;
    }

    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

void ApplyGeometry(HWND hwnd) {
    if (!IsWindow(hwnd) || g_unloading)
        return;

    RECT rc{};
    if (!GetWindowRect(hwnd, &rc))
        return;

    // The subclass will rewrite this request to the configured geometry.
    SetWindowPos(hwnd,
                 nullptr,
                 rc.left,
                 rc.top,
                 rc.right - rc.left,
                 rc.bottom - rc.top,
                 SWP_NOZORDER | SWP_NOACTIVATE);
}

// -----------------------------------------------------------------------------
// Attach / detach
// -----------------------------------------------------------------------------

void AttachToBoardWindow(HWND hwnd) {
    if (!IsBoardWindow(hwnd) || g_unloading)
        return;

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
            hwnd, BoardSubclassProc, 0)) {
        Wh_Log(L"Failed to subclass WindowsDashboard: hwnd=%p", hwnd);
        return;
    }

    Wh_Log(L"Attached to WindowsDashboard: hwnd=%p dpi=%u",
           hwnd,
           GetDpiForWindow(hwnd));

    ApplyGeometry(hwnd);
}

void DetachFromBoardWindow(HWND hwnd) {
    if (!IsBoardWindow(hwnd))
        return;

    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, BoardSubclassProc);
}

void ScanForBoardWindows() {
    ForEachTopLevelWindow(AttachToBoardWindow);
}

void CALLBACK WinEventProc(HWINEVENTHOOK,
                           DWORD event,
                           HWND hwnd,
                           LONG idObject,
                           LONG idChild,
                           DWORD,
                           DWORD) {
    if (event != EVENT_OBJECT_SHOW || !hwnd ||
        idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }

    AttachToBoardWindow(hwnd);
}

DWORD WINAPI WinEventThreadProc(LPVOID) {
    g_winEventThreadId = GetCurrentThreadId();

    // Force creation of this thread's message queue before initialization is
    // reported complete, so teardown can always use PostThreadMessage safely.
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    HWINEVENTHOOK showHook = SetWinEventHook(
        EVENT_OBJECT_SHOW,
        EVENT_OBJECT_SHOW,
        nullptr,
        WinEventProc,
        GetCurrentProcessId(),
        0,
        WINEVENT_OUTOFCONTEXT);

    if (!showHook) {
        Wh_Log(L"SetWinEventHook failed, error=%u", GetLastError());
        SetEvent(g_winEventThreadReady);
        return 0;
    }

    g_winEventHookInstalled = true;
    ScanForBoardWindows();
    SetEvent(g_winEventThreadReady);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (!msg.hwnd && msg.message == WM_APP)
            break;

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UnhookWinEvent(showHook);
    g_winEventHookInstalled = false;
    return 0;
}

bool StartWinEventThread() {
    g_winEventThreadReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_winEventThreadReady) {
        Wh_Log(L"CreateEventW failed, error=%u", GetLastError());
        return false;
    }

    g_winEventThread =
        CreateThread(nullptr, 0, WinEventThreadProc, nullptr, 0, nullptr);
    if (!g_winEventThread) {
        Wh_Log(L"CreateThread failed, error=%u", GetLastError());
        CloseHandle(g_winEventThreadReady);
        g_winEventThreadReady = nullptr;
        return false;
    }

    WaitForSingleObject(g_winEventThreadReady, INFINITE);
    CloseHandle(g_winEventThreadReady);
    g_winEventThreadReady = nullptr;

    if (!g_winEventHookInstalled) {
        WaitForSingleObject(g_winEventThread, INFINITE);
        CloseHandle(g_winEventThread);
        g_winEventThread = nullptr;
        g_winEventThreadId = 0;
        return false;
    }

    return true;
}

void StopWinEventThread() {
    if (!g_winEventThread)
        return;

    if (!PostThreadMessageW(g_winEventThreadId, WM_APP, 0, 0))
        Wh_Log(L"PostThreadMessageW failed, error=%u", GetLastError());

    WaitForSingleObject(g_winEventThread, INFINITE);
    CloseHandle(g_winEventThread);
    g_winEventThread = nullptr;
    g_winEventThreadId = 0;
}

void DetachFromAllBoardWindows() {
    ForEachTopLevelWindow(DetachFromBoardWindow);
}

}  // namespace

// -----------------------------------------------------------------------------
// Windhawk lifecycle
// -----------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L">");
    LoadSettings();

    return StartWinEventThread();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");
    g_unloading = true;

    StopWinEventThread();
    DetachFromAllBoardWindows();
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();
    ScanForBoardWindows();
}
