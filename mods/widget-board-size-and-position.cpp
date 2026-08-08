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

Allow you to override the default position and dimension of the new WinUI3 Widget Board (`WidgetBoard.exe`).

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
  $description: "Where Widget Board shows up horiztontally."
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
std::atomic<bool> g_attaching{false};
std::atomic<HWND> g_boardHwnd{nullptr};

HWINEVENTHOOK g_showHook = nullptr;

constexpr UINT_PTR kSubclassId = 1;
constexpr wchar_t kBoardClass[] = L"WindowsDashboard";

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------

HAnchor ParseHAnchor(PCWSTR value) {
    if (!value || _wcsicmp(value, L"system") == 0)
        return HAnchor::System;
    if (_wcsicmp(value, L"center") == 0)
        return HAnchor::Center;
    if (_wcsicmp(value, L"right") == 0)
        return HAnchor::Right;
    return HAnchor::Left;
}

VAnchor ParseVAnchor(PCWSTR value) {
    if (!value || _wcsicmp(value, L"system") == 0)
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

    PCWSTR h = Wh_GetStringSetting(L"horizontalAnchor");
    g_settings.hAnchor = static_cast<int>(ParseHAnchor(h));
    if (h)
        Wh_FreeStringSetting(h);

    PCWSTR v = Wh_GetStringSetting(L"verticalAnchor");
    g_settings.vAnchor = static_cast<int>(ParseVAnchor(v));
    if (v)
        Wh_FreeStringSetting(v);
}

// -----------------------------------------------------------------------------
// Thread helper, adapted from the Start Menu Size mod.
// SetWindowSubclass/RemoveWindowSubclass are performed on the HWND's GUI thread.
// -----------------------------------------------------------------------------

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT registeredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID param;
    };

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!threadId)
        return false;

    if (threadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == registeredMsg) {
                    auto* param = reinterpret_cast<PARAM*>(cwp->lParam);
                    param->proc(param->param);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr,
        threadId);

    if (!hook) {
        Wh_Log(L"SetWindowsHookEx failed, error=%u", GetLastError());
        return false;
    }

    PARAM param{proc, procParam};
    SendMessage(hWnd, registeredMsg, 0, reinterpret_cast<LPARAM>(&param));
    UnhookWindowsHookEx(hook);
    return true;
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

HWND FindBoardWindow() {
    struct EnumData {
        HWND result = nullptr;
    } data;

    EnumWindows(
        [](HWND hwnd, LPARAM lParam) -> BOOL {
            auto* data = reinterpret_cast<EnumData*>(lParam);
            if (IsBoardWindow(hwnd)) {
                data->result = hwnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&data));

    return data.result;
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
                                   UINT_PTR subclassId,
                                   DWORD_PTR refData) {
    switch (msg) {
        case WM_WINDOWPOSCHANGING:
            ForceGeometry(hwnd, reinterpret_cast<WINDOWPOS*>(lParam));
            break;

        case WM_NCDESTROY: {
            RemoveWindowSubclass(hwnd, BoardSubclassProc, subclassId);
            HWND expected = hwnd;
            g_boardHwnd.compare_exchange_strong(expected, nullptr);
            Wh_Log(L"WindowsDashboard destroyed: hwnd=%p", hwnd);
            break;
        }
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

void WINAPI ApplyGeometryOnWindowThread(PVOID parameter) {
    ApplyGeometry(reinterpret_cast<HWND>(parameter));
}

// -----------------------------------------------------------------------------
// Attach / detach
// -----------------------------------------------------------------------------

void WINAPI AttachOnWindowThread(PVOID parameter) {
    HWND hwnd = reinterpret_cast<HWND>(parameter);

    if (!IsBoardWindow(hwnd) || g_unloading) {
        g_attaching = false;
        return;
    }

    if (!SetWindowSubclass(hwnd, BoardSubclassProc, kSubclassId, 0)) {
        Wh_Log(L"SetWindowSubclass failed, error=%u", GetLastError());
        g_attaching = false;
        return;
    }

    g_boardHwnd = hwnd;
    g_attaching = false;

    Wh_Log(L"Attached to WindowsDashboard: hwnd=%p dpi=%u",
           hwnd,
           GetDpiForWindow(hwnd));

    ApplyGeometry(hwnd);
}

void WINAPI DetachOnWindowThread(PVOID parameter) {
    HWND hwnd = reinterpret_cast<HWND>(parameter);
    if (IsWindow(hwnd))
        RemoveWindowSubclass(hwnd, BoardSubclassProc, kSubclassId);
}

void AttachToBoardWindow(HWND hwnd) {
    if (g_unloading || !IsBoardWindow(hwnd))
        return;

    HWND existing = g_boardHwnd.load();
    if (existing == hwnd && IsWindow(existing))
        return;

    bool expected = false;
    if (!g_attaching.compare_exchange_strong(expected, true))
        return;

    if (g_unloading || !IsBoardWindow(hwnd)) {
        g_attaching = false;
        return;
    }

    if (!RunFromWindowThread(hwnd, AttachOnWindowThread, hwnd))
        g_attaching = false;
}

void ScanForBoardWindow() {
    if (g_unloading)
        return;

    HWND existing = g_boardHwnd.load();
    if (existing && IsWindow(existing))
        return;

    g_boardHwnd = nullptr;

    HWND hwnd = FindBoardWindow();
    if (hwnd)
        AttachToBoardWindow(hwnd);
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

}  // namespace

// -----------------------------------------------------------------------------
// Windhawk lifecycle
// -----------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L">");
    LoadSettings();

    HMODULE modModule = nullptr;
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&WinEventProc),
            &modModule)) {
        Wh_Log(L"GetModuleHandleExW failed, error=%u", GetLastError());
        return FALSE;
    }

    g_showHook = SetWinEventHook(EVENT_OBJECT_SHOW,
                                 EVENT_OBJECT_SHOW,
                                 modModule,
                                 WinEventProc,
                                 GetCurrentProcessId(),
                                 0,
                                 WINEVENT_INCONTEXT);
    if (!g_showHook) {
        Wh_Log(L"SetWinEventHook failed, error=%u", GetLastError());
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    ScanForBoardWindow();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");
    g_unloading = true;

    if (g_showHook) {
        UnhookWinEvent(g_showHook);
        g_showHook = nullptr;
    }

    HWND hwnd = g_boardHwnd.exchange(nullptr);
    if (hwnd && IsWindow(hwnd))
        RunFromWindowThread(hwnd, DetachOnWindowThread, hwnd);
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();

    HWND hwnd = g_boardHwnd.load();
    if (hwnd && IsWindow(hwnd)) {
        RunFromWindowThread(hwnd, ApplyGeometryOnWindowThread, hwnd);
    } else {
        ScanForBoardWindow();
    }
}
