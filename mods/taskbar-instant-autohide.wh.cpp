// ==WindhawkMod==
// @id              taskbar-instant-autohide
// @name            Taskbar instant auto-hide
// @description     Hides the auto-hidden taskbar right after the pointer leaves it, also while an app flashes, but keeps it up for thumbnails and menus
// @version         1.0
// @author          Faci Labs
// @github          https://github.com/FreedomPortal
// @include         explorer.exe
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar instant auto-hide

![Demo](https://raw.githubusercontent.com/FreedomPortal/asset-hosting/refs/heads/main/taskbar-instant-autohide.gif)

For Windows' *Automatically hide the taskbar* (turn it on first).

1. **No wait after the pointer leaves.** Explorer's auto-hide timer is
   shortened to the *Hide delay*.
2. **Flashing apps don't hold the taskbar up.** Windows keeps the taskbar
   shown while a taskbar button flashes for attention. The mod lets it hide
   anyway (the button still flashes when you bring the taskbar back).
3. **Thumbnails, jump lists and menus still keep it up.** While the pointer
   is on a thumbnail preview, a jump list or a menu (any Explorer popup), or
   a menu / the taskbar itself is in use, the taskbar stays.
4. **Keep-open range.** An optional margin around the taskbar and its popups
   (for mods that animate or resize them, like *Dock Animation*): the
   taskbar stays while the pointer is inside it. Bigger = steadier, but the
   taskbar hides a little later after you move away.

Part 2 uses Explorer's symbols (`CTaskListWnd::PermitAutoHide`, in
taskbar.dll on Windows 11 and explorer.exe on Windows 10); Windhawk
downloads them the first time. If they can't be found, parts 1 and 3 still
work.

Don't run it together with *Better Taskbar Autohide*: both change the same
Explorer check.

If the taskbar still lingers right after enabling the mod, turn auto-hide off
and on once in Windows' taskbar settings (Explorer set its timer before the
mod loaded).

## Credits
Part 2 builds on **Better Taskbar Autohide** (`taskbar-autohide-better`) by
[Cirn09](https://github.com/Cirn09) ([blog](https://blog.cirn09.xyz/)): the
idea of hooking `CTaskListWnd::PermitAutoHide`, and the symbol names used
here. Thank you!

## If it stops working after a Windows update
Turn on **Log taskbar timers** (Advanced tab, *Show log output*), hover the
taskbar and move away, and look for the timer set when you leave and killed
when the taskbar hides. The log also says whether the symbol hook loaded.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hideDelay: 0
  $name: Hide delay (ms)
  $description: Wait after the pointer leaves the taskbar. 0 = as fast as Windows allows (about 10 ms).
- keepRange: 0
  $name: Keep-open range (px)
  $description: Extra margin around the taskbar and its popups (thumbnails, jump lists) where the pointer still keeps the taskbar up. Helps with mods that move or resize them, like Dock Animation. 0 = no margin. Scales with display scaling.
- hideWhenAppFlashes: true
  $name: Hide even while an app flashes for attention
- keepDelayForThumbnails: true
  $name: Keep the taskbar up while the pointer is on a thumbnail, jump list or menu
- logTimers: false
  $name: Log taskbar timers
  $description: Writes every timer Explorer sets or kills on the taskbar to the mod log.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_utils.h>

// Explorer's auto-hide timer on every taskbar (Windows 11, tested).
constexpr UINT_PTR kHideTimerId = 2;

struct {
    int hideDelay;
    int keepRange;
    bool hideWhenAppFlashes;
    bool keepDelayForThumbnails;
    bool logTimers;
} g_settings;

static bool ClassIs(HWND hwnd, const wchar_t* name) {
    WCHAR cls[64];
    return hwnd && GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
           wcscmp(cls, name) == 0;
}

// 0 = not a taskbar, 1 = main taskbar, 2 = taskbar of another screen
static int TaskbarKind(HWND hwnd) {
    if (ClassIs(hwnd, L"Shell_TrayWnd")) {
        return 1;
    }
    if (ClassIs(hwnd, L"Shell_SecondaryTrayWnd")) {
        return 2;
    }
    return 0;
}

static bool ThumbnailOpen() {
    HWND thumb = FindWindowW(L"TaskListThumbnailWnd", nullptr);
    return thumb && IsWindowVisible(thumb);
}

// Is a taskbar popup in use (thumbnail, menu, jump list) or the taskbar
// itself active? Then Windows' own "keep it shown" answer stands.
static BOOL CALLBACK FindMenuProc(HWND hwnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId() && IsWindowVisible(hwnd) &&
        ClassIs(hwnd, L"#32768")) {
        *(bool*)lParam = true;
        return FALSE;
    }
    return TRUE;
}

static bool IsFolderOrDesktop(HWND hwnd) {
    return ClassIs(hwnd, L"CabinetWClass") || ClassIs(hwnd, L"Progman") ||
           ClassIs(hwnd, L"WorkerW");
}

// Pointer over an Explorer popup (thumbnail, jump list, tray overflow...),
// whatever its window class on this Windows build.
static bool PointerOnExplorerPopup() {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return false;
    }
    HWND hwnd = WindowFromPoint(pt);
    HWND root = hwnd ? GetAncestor(hwnd, GA_ROOT) : nullptr;
    if (!root || TaskbarKind(root)) {
        return false;  // over the taskbar itself: Explorer's own check
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(root, &pid);
    return pid == GetCurrentProcessId() && !IsFolderOrDesktop(root);
}

// Pointer within the keep-open range of the taskbar or an Explorer popup.
struct NearScan {
    POINT pt;
    bool found;
};

static BOOL CALLBACK NearProc(HWND hwnd, LPARAM lParam) {
    NearScan* scan = (NearScan*)lParam;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId() || !IsWindowVisible(hwnd) ||
        IsFolderOrDesktop(hwnd)) {
        return TRUE;
    }
    RECT rc;
    if (!GetWindowRect(hwnd, &rc) || IsRectEmpty(&rc)) {
        return TRUE;
    }
    // Skip big invisible host windows that cover much of a screen.
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST), &mi)) {
        LONGLONG area = (LONGLONG)(rc.right - rc.left) * (rc.bottom - rc.top);
        LONGLONG mon = (LONGLONG)(mi.rcMonitor.right - mi.rcMonitor.left) *
                       (mi.rcMonitor.bottom - mi.rcMonitor.top);
        if (area * 2 > mon) {
            return TRUE;
        }
    }
    UINT dpi = GetDpiForWindow(hwnd);
    int range = MulDiv(g_settings.keepRange, dpi ? dpi : 96, 96);
    InflateRect(&rc, range, range);
    if (PtInRect(&rc, scan->pt)) {
        scan->found = true;
        return FALSE;
    }
    return TRUE;
}

static bool PointerInKeepRange() {
    if (g_settings.keepRange <= 0) {
        return false;
    }
    NearScan scan = {};
    if (!GetCursorPos(&scan.pt)) {
        return false;
    }
    EnumWindows(NearProc, (LPARAM)&scan);
    return scan.found;
}

static bool TaskbarUiInUse() {
    if (ThumbnailOpen() || PointerOnExplorerPopup() || PointerInKeepRange()) {
        return true;
    }
    bool menu = false;
    EnumWindows(FindMenuProc, (LPARAM)&menu);
    if (menu) {
        return true;
    }
    // Foreground window inside Explorer that isn't a folder window or the
    // desktop: the taskbar, a jump list, the tray overflow...
    HWND fg = GetForegroundWindow();
    DWORD pid = 0;
    if (fg) {
        GetWindowThreadProcessId(fg, &pid);
    }
    if (pid == GetCurrentProcessId() && !IsFolderOrDesktop(fg)) {
        return true;
    }
    return false;
}

// --- Part 1: shorter auto-hide timer -------------------------------------

using SetTimer_t = decltype(&SetTimer);
SetTimer_t SetTimer_Original;

UINT_PTR WINAPI SetTimer_Hook(HWND hWnd,
                              UINT_PTR nIDEvent,
                              UINT uElapse,
                              TIMERPROC lpTimerFunc) {
    int kind = TaskbarKind(hWnd);
    if (kind) {
        UINT elapse = uElapse;
        if (nIDEvent == kHideTimerId) {
            UINT wanted = g_settings.hideDelay < USER_TIMER_MINIMUM
                              ? USER_TIMER_MINIMUM
                              : (UINT)g_settings.hideDelay;
            if (wanted < elapse) {
                elapse = wanted;
            }
        }
        if (g_settings.logTimers) {
            Wh_Log(L"SetTimer %s hwnd=%p id=%Iu interval=%u -> %u",
                   kind == 1 ? L"main" : L"other", hWnd, nIDEvent, uElapse,
                   elapse);
        }
        uElapse = elapse;
    }
    return SetTimer_Original(hWnd, nIDEvent, uElapse, lpTimerFunc);
}

using KillTimer_t = decltype(&KillTimer);
KillTimer_t KillTimer_Original;

BOOL WINAPI KillTimer_Hook(HWND hWnd, UINT_PTR uIDEvent) {
    int kind = g_settings.logTimers ? TaskbarKind(hWnd) : 0;
    if (kind) {
        Wh_Log(L"KillTimer %s hwnd=%p id=%Iu", kind == 1 ? L"main" : L"other",
               hWnd, uIDEvent);
    }
    return KillTimer_Original(hWnd, uIDEvent);
}

// Each tick of the hide timer: skip it while a popup is in use, so the
// short interval never hides the taskbar under a thumbnail or jump list.
using DispatchMessageW_t = decltype(&DispatchMessageW);
DispatchMessageW_t DispatchMessageW_Original;

LRESULT WINAPI DispatchMessageW_Hook(const MSG* msg) {
    if (msg && msg->message == WM_TIMER && msg->wParam == kHideTimerId &&
        g_settings.keepDelayForThumbnails && TaskbarKind(msg->hwnd) &&
        TaskbarUiInUse()) {
        return 0;
    }
    return DispatchMessageW_Original(msg);
}

// --- Part 2: flashing apps don't hold the taskbar up ---------------------
// Approach and symbol names from "Better Taskbar Autohide"
// (taskbar-autohide-better) by Cirn09, https://github.com/Cirn09

using PermitAutoHide_t = BOOL(__thiscall*)(void* pThis);
PermitAutoHide_t PermitAutoHide_Original;

BOOL __thiscall PermitAutoHide_Hook(void* pThis) {
    BOOL permit = PermitAutoHide_Original(pThis);
    if (permit || !g_settings.hideWhenAppFlashes || TaskbarUiInUse()) {
        return permit;
    }
    return TRUE;
}

static void HookPermitAutoHide() {
    // Windows 11: taskbar.dll.
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{
             L"public: virtual int __cdecl CTaskListWnd::PermitAutoHide(void)",
             L"public: virtual int __stdcall CTaskListWnd::PermitAutoHide(void)",
         },
         (void**)&PermitAutoHide_Original,
         (void*)PermitAutoHide_Hook},
    };
    // Windows 10: explorer.exe itself.
    WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
        {{
             L"public: virtual int __cdecl CTaskListWnd::PermitAutoHide(void)",
             L"public: virtual int __stdcall CTaskListWnd::PermitAutoHide(void)",
         },
         (void**)&PermitAutoHide_Original,
         (void*)PermitAutoHide_Hook},
    };

    bool hooked;
    HMODULE taskbarDll =
        LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (taskbarDll) {
        hooked = WindhawkUtils::HookSymbols(taskbarDll, taskbarDllHooks,
                                            ARRAYSIZE(taskbarDllHooks));
    } else {
        hooked = WindhawkUtils::HookSymbols(GetModuleHandleW(nullptr),
                                            explorerExeHooks,
                                            ARRAYSIZE(explorerExeHooks));
    }
    if (hooked) {
        Wh_Log(L"PermitAutoHide hooked");
    } else {
        Wh_Log(L"PermitAutoHide not found; flashing apps will still hold "
               L"the taskbar up");
    }
}

// --- Mod entry points ----------------------------------------------------

static void LoadSettings() {
    g_settings.hideDelay = Wh_GetIntSetting(L"hideDelay");
    if (g_settings.hideDelay < 0) {
        g_settings.hideDelay = 0;
    }
    g_settings.keepRange = Wh_GetIntSetting(L"keepRange");
    if (g_settings.keepRange < 0) {
        g_settings.keepRange = 0;
    } else if (g_settings.keepRange > 300) {
        g_settings.keepRange = 300;
    }
    g_settings.hideWhenAppFlashes = Wh_GetIntSetting(L"hideWhenAppFlashes");
    g_settings.keepDelayForThumbnails =
        Wh_GetIntSetting(L"keepDelayForThumbnails");
    g_settings.logTimers = Wh_GetIntSetting(L"logTimers");
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();
    Wh_SetFunctionHook((void*)SetTimer, (void*)SetTimer_Hook,
                       (void**)&SetTimer_Original);
    Wh_SetFunctionHook((void*)KillTimer, (void*)KillTimer_Hook,
                       (void**)&KillTimer_Original);
    Wh_SetFunctionHook((void*)DispatchMessageW, (void*)DispatchMessageW_Hook,
                       (void**)&DispatchMessageW_Original);
    HookPermitAutoHide();
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
