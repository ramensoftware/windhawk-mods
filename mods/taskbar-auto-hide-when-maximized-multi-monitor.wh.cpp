// ==WindhawkMod==
// @id              taskbar-auto-hide-when-maximized-multi-monitor
// @name            Taskbar auto-hide when maximized - multi-monitor fix
// @description     Auto-hide the taskbar only when a window is maximized or overlaps it. Fork of m417z's mod with multi-monitor, stuck-taskbar, hide-latency and hover-reveal fixes on Windows 11.
// @version         1.3.0
// @author          Saderius
// @github          https://github.com/Saderius
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lversion
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/Saderius/multi-monitor-taskbar-autohide

// ==WindhawkModReadme==
/*
# Taskbar auto-hide when maximized - multi-monitor fix

Instead of hiding the taskbar as soon as the mouse leaves it, this mod docks the
taskbar out of the way only when a window on that monitor is maximized or
overlaps the taskbar area. The rest of the time the taskbar stays visible.

This is a fork of
[m417z's Taskbar auto-hide when maximized](https://windhawk.net/mods/taskbar-auto-hide-when-maximized).
The core feature is preserved and expanded into a full multi-monitor architecture
with robust startup, display power-cycle recovery, and shell timer integration on Windows 11.

## What this fork changes

**Startup & Symbol Loading on Windows 11**

- Until the `ViewCoordinator` hooks are installed, the mod stays out of the way
  and lets Windows handle auto-hide passively.
- Symbol hooking for `Taskbar.View.dll` (and `ExplorerExtensions.dll` on Win11 24H2+)
  is synchronized atomically and throttled to prevent redundant attempts.
- Hook retries execute safely on a background worker thread,
  never freezing the Explorer taskbar UI thread during symbol downloads.
- A failed symbol load is retried automatically every ~30 seconds, so the mod
  starts working after symbols arrive without restarting Explorer.

**Multi-Monitor Architecture & Refined Shell Timer Path**

- Orchestrates independent taskbars across multiple monitors (`TrayUI` for the primary
  display and `CSecondaryTray` for secondary displays).
- Replaces raw internal hide calls with `WM_TIMER` (`kTrayUITimerHide`, ID 2) posted
  directly to the taskbar's message queue. This allows Explorer to execute its full
  suite of safety checks before retracting:
  * Keyboard navigation and focus (`Win+T`, `Win+B`, Tab).
  * Modern Start Menu, Search host, and Shell Experience popups.
  * Jump lists, context menus, and DWM Thumbnail Previews.
  * System tray flyouts (Volume, Network, Calendar, Notification Center).
- Direct hide is reserved strictly as an emergency fallback if the shell's
  auto-hide setting cannot be restored.
- When **Primary monitor only** is enabled, secondary taskbars remain completely
  unmanaged, allowing Windows default behavior to operate untouched.

**Windows Auto-Hide Setting Behavior (`StuckRects3`)**

- This mod requires Windows' native "Automatically hide the taskbar" setting to be enabled
  to expand the desktop work area (`SPI_SETWORKAREA`), register AppBars, and coordinate
  XAML view states.
- If auto-hide is disabled when the mod starts, it records your initial preference
  and turns auto-hide on via the primary tray.
- Setting changes are guarded against modal message loop re-entrancy, eliminating
  cascading warning dialogs.
- When unloaded or disabled cleanly in Windhawk (`Wh_ModUninit`), the mod restores
  your original auto-hide setting.
- Note: If Explorer terminates abnormally or crashes, the setting in `StuckRects3`
  may remain enabled; it can be toggled back at any time in Windows Settings
  (*Settings > Personalization > Taskbar > Taskbar behaviors*).

**Survives a Monitor Power Cycle (Displays off/on, sleep, dock/undock)**

- Re-evaluates state immediately on display re-enumeration (`WM_DISPLAYCHANGE`),
  deduplicated exclusively on the primary tray to eliminate redundant message storms,
  followed by a debounced 750ms settling timer to gracefully absorb asynchronous
  multi-monitor wake-up and window restoration.
- If the shell loses auto-hide during an AppBar registration collision, the mod
  restores it using a persistent session-wide flag.
- Validates monitor handles after display changes, falling back to the monitor the
  taskbar actually occupies if the shell holds a stale handle.
- Restores topmost z-order for taskbars recreated during display sleep/wake.

**Edge Hover Reveal & Frameless Maximized Apps**

- Arms the shell's unhide timer after retracting so touching the screen edge reveals
  the taskbar on all monitors.
- Frameless windows whose maximized client area covers the entire monitor (e.g.
  the ChatGPT desktop app or Steam) are exempted in-process so Windows doesn't
  mistake them for fullscreen games and drop `WS_EX_TOPMOST`.

Credits: original mod by m417z, published under the GNU General Public License v3.0.
Fork maintained by Saderius.

## Notes

- Windows 10 and Windows 11, 64-bit `explorer.exe`.
- In *maximized* mode, only true maximized windows (and windows snapped to a full
  height, e.g. `Win`+`Left`) keep the taskbar visible.
- In *intersected* mode (default), any window overlapping the taskbar area does.
- Set *Mode* to *never* to stop the taskbar from auto-hiding at all while the mod
  is enabled.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- mode: intersected
  $name: Mode
  $options:
  - intersected: Auto-hide when a window is maximized or intersects the taskbar
  - maximized: Auto-hide only when a window is maximized
  - never: Never auto-hide
- foregroundWindowOnly: false
  $name: Apply only to foreground window
  $description: >-
    Enable this option to apply the auto-hide taskbar feature only to the
    selected window.
- excludedPrograms: [""]
  $name: Excluded programs
  $description: >-
    The taskbar won't auto-hide due to windows of these programs being maximized
    or intersecting the taskbar.

    Entries can be process names, paths or application IDs, for example:

    mspaint.exe

    C:\Windows\System32\notepad.exe

    Microsoft.WindowsCalculator_8wekyb3d8bbwe!App
- primaryMonitorOnly: false
  $name: Primary monitor only
  $description: >-
    Apply the mod's behavior only to the primary monitor taskbar. Secondary
    monitors will use Windows' default auto-hide behavior.
- oldTaskbarOnWin11: false
  $name: Customize the old taskbar on Windows 11
  $description: >-
    Enable this option to customize the old taskbar on Windows 11 (if using
    ExplorerPatcher or a similar tool).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <psapi.h>

#include <winrt/base.h>

#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

enum class Mode {
    intersected,
    maximized,
    never,
};

struct {
    Mode mode;
    bool foregroundWindowOnly;
    std::unordered_set<std::wstring> excludedPrograms;
    bool primaryMonitorOnly;
    bool oldTaskbarOnWin11;
} g_settings;

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
    Win11_24H2,
};

WinVersion g_winVersion;

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_taskbarViewHookInProgress{false};
std::atomic<DWORD> g_taskbarViewLastAttemptTick{0};
std::atomic<bool> g_initialized;
std::atomic<bool> g_explorerPatcherInitialized;

bool g_wasAutoHideProcessed;
bool g_wasAutoHideOriginallyDisabled;
std::mutex g_winEventHookThreadMutex;
std::atomic<HANDLE> g_winEventHookThread;
std::unordered_map<void*, HWND> g_taskbarsKeptShown;
// Taskbar object -> its window, so a hide seen anywhere (the shell's own
// timers included) can arm the unhide timer on the right window.
std::unordered_map<void*, HWND> g_taskbarWndByObject;
std::unordered_map<HWND, void*> g_taskbarToViewCoordinator;

using ViewCoordinator_IsExpanded_t =
    bool(WINAPI*)(void* pThis, HWND hMMTaskbarWnd);
ViewCoordinator_IsExpanded_t ViewCoordinator_IsExpanded_Original;

using ViewCoordinator_ShouldTaskbarBeExpanded_t =
    bool(WINAPI*)(void* pThis, HWND hMMTaskbarWnd, bool expanded);
ViewCoordinator_ShouldTaskbarBeExpanded_t
    ViewCoordinator_ShouldTaskbarBeExpanded_Original;

// Tracks whether turning auto-hide on via the shell's private setting message
// failed (e.g. stale auto-hide toolbar registration collision after display cycles).
// Maintained as a session-wide flag to eliminate warning dialog spam.
bool g_autoHideEnableFailed = false;
bool g_autoHideEnableAttempted = false;
std::unordered_set<HWND> g_taskbarsPendingTopmostCheck;
UINT_PTR g_pendingEventsTimer;
std::atomic<HWND> g_multitaskingViewHwnd;
std::atomic<HWND> g_altTabViewHwnd;

// TrayUI::_HandleTrayPrivateSettingMessage
constexpr UINT kHandleTrayPrivateSettingMessage = WM_USER + 0x1CA;

enum {
    kTrayPrivateSettingAutoHideGet = 3,
    kTrayPrivateSettingAutoHideSet = 4,
};

constexpr WCHAR kUpdateTaskbarStatePendingTickCount[] =
    L"Windhawk_UpdateTaskbarStatePendingTickCount_" WH_MOD_ID;

static const UINT g_getTaskbarRectRegisteredMsg =
    RegisterWindowMessage(L"Windhawk_GetTaskbarRect_" WH_MOD_ID);

static const UINT g_updateTaskbarStateRegisteredMsg =
    RegisterWindowMessage(L"Windhawk_UpdateTaskbarState_" WH_MOD_ID);

enum {
    kTrayUITimerHide = 2,
    kTrayUITimerUnhide = 3,
};

#if __cplusplus < 202302L
// Missing in older MinGW headers.
DECLARE_HANDLE(CO_MTA_USAGE_COOKIE);
WINOLEAPI CoIncrementMTAUsage(CO_MTA_USAGE_COOKIE* pCookie);
WINOLEAPI CoDecrementMTAUsage(CO_MTA_USAGE_COOKIE Cookie);
#endif

// Missing in older MinGW headers.
#ifndef EVENT_OBJECT_CLOAKED
#define EVENT_OBJECT_CLOAKED 0x8017
#endif
#ifndef EVENT_OBJECT_UNCLOAKED
#define EVENT_OBJECT_UNCLOAKED 0x8018
#endif

#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
DECLARE_HANDLE(DPI_AWARENESS_CONTEXT);
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT)-4)
#endif

struct AutoPerMonitorDpi {
    DPI_AWARENESS_CONTEXT oldContext = nullptr;
    AutoPerMonitorDpi() {
        using SetThreadDpiAwarenessContext_t =
            DPI_AWARENESS_CONTEXT(WINAPI*)(DPI_AWARENESS_CONTEXT);
        static auto pfn = reinterpret_cast<SetThreadDpiAwarenessContext_t>(
            GetProcAddress(GetModuleHandleW(L"user32.dll"),
                           "SetThreadDpiAwarenessContext"));
        if (pfn) {
            oldContext = pfn(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        }
    }
    ~AutoPerMonitorDpi() {
        if (oldContext) {
            using SetThreadDpiAwarenessContext_t =
                DPI_AWARENESS_CONTEXT(WINAPI*)(DPI_AWARENESS_CONTEXT);
            static auto pfn = reinterpret_cast<SetThreadDpiAwarenessContext_t>(
                GetProcAddress(GetModuleHandleW(L"user32.dll"),
                               "SetThreadDpiAwarenessContext"));
            if (pfn) {
                pfn(oldContext);
            }
        }
    }
};

using IsWindowArranged_t = BOOL(WINAPI*)(HWND hwnd);
IsWindowArranged_t pIsWindowArranged;

// Private API for window band (z-order band).
// https://blog.adeltax.com/window-z-order-in-windows-10/
using GetWindowBand_t = BOOL(WINAPI*)(HWND hWnd, PDWORD pdwBand);
GetWindowBand_t pGetWindowBand;

// https://devblogs.microsoft.com/oldnewthing/20200302-00/?p=103507
bool IsWindowCloaked(HWND hwnd) {
    BOOL isCloaked = FALSE;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &isCloaked,
                                           sizeof(isCloaked))) &&
           isCloaked;
}

enum class MultitaskingViewType {
    None,
    WinTab,
    AltTab,
};

// Detects Alt+Tab or Win+Tab and returns the type.
MultitaskingViewType GetMultitaskingViewType(HWND hWnd) {
    // Must be in the current process (explorer.exe).
    DWORD dwProcessId = 0;
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
    if (!dwThreadId || dwProcessId != GetCurrentProcessId()) {
        return MultitaskingViewType::None;
    }

    WCHAR className[64];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className)) ||
        _wcsicmp(className, L"XamlExplorerHostIslandWindow") != 0) {
        return MultitaskingViewType::None;
    }

    // The Win+Tab window uses band ZBID_IMMERSIVE_APPCHROME.
    constexpr DWORD ZBID_IMMERSIVE_APPCHROME = 5;

    // The Alt+Tab window uses band ZBID_SYSTEM_TOOLS. The virtual desktop
    // switcher (which we don't want) uses band ZBID_IMMERSIVE_EDGY.
    constexpr DWORD ZBID_SYSTEM_TOOLS = 16;

    DWORD band = 0;
    if (!pGetWindowBand || !pGetWindowBand(hWnd, &band)) {
        return MultitaskingViewType::None;
    }

    MultitaskingViewType type;
    if (band == ZBID_IMMERSIVE_APPCHROME) {
        type = MultitaskingViewType::WinTab;
    } else if (band == ZBID_SYSTEM_TOOLS) {
        type = MultitaskingViewType::AltTab;
    } else {
        return MultitaskingViewType::None;
    }

    // Check thread description for "MultitaskingView".
    HANDLE hThread =
        OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, dwThreadId);
    if (!hThread) {
        return MultitaskingViewType::None;
    }

    bool isMultitaskingView = false;
    PWSTR description = nullptr;
    if (SUCCEEDED(GetThreadDescription(hThread, &description)) && description) {
        isMultitaskingView = wcscmp(description, L"MultitaskingView") == 0;
        LocalFree(description);
    }

    CloseHandle(hThread);
    return isMultitaskingView ? type : MultitaskingViewType::None;
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

bool IsTaskbarWindow(HWND hWnd) {
    WCHAR szClassName[32];
    if (!GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName))) {
        return false;
    }

    return _wcsicmp(szClassName, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0;
}

HWND FindTaskbarWindows(std::unordered_set<HWND>* secondaryTaskbarWindows) {
    secondaryTaskbarWindows->clear();

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return nullptr;
    }

    DWORD taskbarThreadId = GetWindowThreadProcessId(hTaskbarWnd, nullptr);
    if (!taskbarThreadId) {
        return nullptr;
    }

    auto enumWindowsProc = [&secondaryTaskbarWindows](HWND hWnd) -> BOOL {
        WCHAR szClassName[32];
        if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
            return TRUE;
        }

        if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
            secondaryTaskbarWindows->insert(hWnd);
        }

        return TRUE;
    };

    EnumThreadWindows(
        taskbarThreadId,
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& proc = *reinterpret_cast<decltype(enumWindowsProc)*>(lParam);
            return proc(hWnd);
        },
        reinterpret_cast<LPARAM>(&enumWindowsProc));

    return hTaskbarWnd;
}

bool GetTaskbarRectForMonitor(HMONITOR monitor, RECT* rect) {
    SetRectEmpty(rect);

    // The primary taskbar's TrayUI can report the stuck rect for any monitor,
    // which is what ShouldKeepTaskbarShown needs for the intersection test.
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return false;
    }

    SendMessage(hTaskbarWnd, g_getTaskbarRectRegisteredMsg, (WPARAM)monitor,
                (LPARAM)rect);
    return true;
}

// The taskbar can hold an HMONITOR that no longer resolves after the displays
// have been re-enumerated (monitor sleep/wake with DPMS, display off/on,
// dock/undock). GetMonitorInfo() then fails, so every "is a window on this
// monitor" check fails too and the mod decides to keep the taskbar shown
// forever. Fall back to the monitor the taskbar window is actually on.
HMONITOR GetTaskbarMonitor(HWND hTaskbarWnd, HMONITOR monitor) {
    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    if (monitor && GetMonitorInfo(monitor, &monitorInfo)) {
        return monitor;
    }

    HMONITOR fallbackMonitor =
        MonitorFromWindow(hTaskbarWnd, MONITOR_DEFAULTTONEAREST);
    Wh_Log(L"Taskbar %08X has a stale monitor %p, using %p instead",
           (DWORD)(ULONG_PTR)hTaskbarWnd, monitor, fallbackMonitor);
    return fallbackMonitor;
}

bool GetAutoHideEnabled(HWND hTaskbarWnd = nullptr) {
    if (!hTaskbarWnd) {
        hTaskbarWnd = FindCurrentProcessTaskbarWnd();
        if (!hTaskbarWnd) return false;
    }
    return SendMessage(hTaskbarWnd, kHandleTrayPrivateSettingMessage,
                       kTrayPrivateSettingAutoHideGet, 0) != 0;
}

// Turns auto-hide back on if it got turned off, targeting the primary taskbar.
// Guarded against modal message loop re-entrancy and display power cycle recreation.
bool EnsureAutoHideEnabled() {
    HWND hPrimaryWnd = FindCurrentProcessTaskbarWnd();
    if (!hPrimaryWnd) {
        return false;
    }

    if (!g_wasAutoHideProcessed) {
        g_wasAutoHideProcessed = true;
        g_wasAutoHideOriginallyDisabled = !GetAutoHideEnabled(hPrimaryWnd);
    }

    if (GetAutoHideEnabled(hPrimaryWnd)) {
        g_autoHideEnableFailed = false;
        return true;
    }

    if (g_autoHideEnableFailed || g_autoHideEnableAttempted) {
        return false;
    }

    // Mark attempt BEFORE SendMessage to prevent re-entrant modal dialog stacking
    g_autoHideEnableAttempted = true;

    SendMessage(hPrimaryWnd, kHandleTrayPrivateSettingMessage,
                kTrayPrivateSettingAutoHideSet, TRUE);

    g_autoHideEnableAttempted = false;

    if (GetAutoHideEnabled(hPrimaryWnd)) {
        Wh_Log(L"Auto-hide was off, turned it back on for primary taskbar %08X",
               (DWORD)(ULONG_PTR)hPrimaryWnd);
        g_autoHideEnableFailed = false;
        return true;
    }

    Wh_Log(L"Auto-hide can't be turned on, falling back to direct hide");
    g_autoHideEnableFailed = true;
    return false;
}

bool IsTaskbarVisibleByRatio(HWND hTaskbarWnd,
                             HMONITOR monitor,
                             int numerator,
                             int denominator) {
    RECT rect;
    if (!GetWindowRect(hTaskbarWnd, &rect) || IsRectEmpty(&rect)) {
        return false;
    }

    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    if (!GetMonitorInfo(monitor, &monitorInfo)) {
        return true;
    }

    RECT visibleRect;
    if (!IntersectRect(&visibleRect, &rect, &monitorInfo.rcMonitor)) {
        return false;
    }

    int thickness = rect.bottom - rect.top;
    if (rect.right - rect.left < thickness) {
        thickness = rect.right - rect.left;
    }

    int visibleThickness = visibleRect.bottom - visibleRect.top;
    if (visibleRect.right - visibleRect.left < visibleThickness) {
        visibleThickness = visibleRect.right - visibleRect.left;
    }

    return visibleThickness * denominator >= thickness * numerator;
}

// True if the taskbar is currently on screen (>= 50% visible).
bool IsTaskbarOnScreen(HWND hTaskbarWnd, HMONITOR monitor) {
    return IsTaskbarVisibleByRatio(hTaskbarWnd, monitor, 1, 2);
}

// Stricter than IsTaskbarOnScreen: true only while the taskbar is fully out (>= 90%),
// so that acting on it mid-animation can't restart the slide.
bool IsTaskbarFullyShown(HWND hTaskbarWnd, HMONITOR monitor) {
    if (g_winVersion >= WinVersion::Win11 && !g_settings.oldTaskbarOnWin11) {
        if (ViewCoordinator_IsExpanded_Original) {
            auto it = g_taskbarToViewCoordinator.find(hTaskbarWnd);
            if (it != g_taskbarToViewCoordinator.end() && it->second) {
                try {
                    return ViewCoordinator_IsExpanded_Original(it->second, hTaskbarWnd);
                } catch (...) {
                    // Fall back to geometric measurement if XAML coordinator throws
                }
            }
        }
    }

    return IsTaskbarVisibleByRatio(hTaskbarWnd, monitor, 9, 10);
}

bool IsShellCoreWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return false;
    WCHAR szClass[64]{};
    GetClassNameW(hWnd, szClass, ARRAYSIZE(szClass));
    if (wcscmp(szClass, L"Windows.UI.Core.CoreWindow") != 0) {
        return false;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        return true;
    }
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (hProc) {
        WCHAR szPath[MAX_PATH]{};
        DWORD size = ARRAYSIZE(szPath);
        if (QueryFullProcessImageNameW(hProc, 0, szPath, &size)) {
            CloseHandle(hProc);
            if (wcsstr(szPath, L"StartMenuExperienceHost.exe") ||
                wcsstr(szPath, L"ShellExperienceHost.exe") ||
                wcsstr(szPath, L"SearchHost.exe")) {
                return true;
            }
        } else {
            CloseHandle(hProc);
        }
    }
    return false;
}

bool IsCursorOverTaskbar(HWND hTaskbarWnd) {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return false;
    }

    RECT rect;
    if (GetWindowRect(hTaskbarWnd, &rect) && PtInRect(&rect, pt)) {
        return true;
    }

    HWND hUnderCursor = WindowFromPoint(pt);
    if (!hUnderCursor) {
        return false;
    }

    if (hUnderCursor == hTaskbarWnd || IsChild(hTaskbarWnd, hUnderCursor)) {
        return true;
    }

    HWND hRoot = GetAncestor(hUnderCursor, GA_ROOT);
    if (!hRoot) {
        hRoot = hUnderCursor;
    }

    // The desktop window (Progman, WorkerW, or Shell window) is never a taskbar flyout.
    if (hRoot == GetShellWindow()) {
        return false;
    }

    WCHAR szRootClass[64]{};
    if (GetClassNameW(hRoot, szRootClass, ARRAYSIZE(szRootClass))) {
        if (wcscmp(szRootClass, L"Progman") == 0 ||
            wcscmp(szRootClass, L"WorkerW") == 0 ||
            wcscmp(szRootClass, L"CabinetWClass") == 0) {
            return false;
        }
    }

    // Check if the cursor is on the same monitor as this taskbar.
    HMONITOR hMonTaskbar = MonitorFromWindow(hTaskbarWnd, MONITOR_DEFAULTTONEAREST);
    HMONITOR hMonCursor = MonitorFromPoint(pt, MONITOR_DEFAULTTONULL);
    if (hMonCursor && hMonTaskbar && hMonCursor != hMonTaskbar) {
        return false;
    }

    // Check for authentic shell flyouts, context menus, tooltips, or overflow islands.
    WCHAR szClass[64]{};
    GetClassNameW(hUnderCursor, szClass, ARRAYSIZE(szClass));

    if (wcscmp(szClass, L"#32768") == 0 ||
        wcscmp(szRootClass, L"#32768") == 0 ||
        wcscmp(szClass, L"TopLevelWindowForOverflowXamlIsland") == 0 ||
        wcscmp(szRootClass, L"TopLevelWindowForOverflowXamlIsland") == 0 ||
        wcscmp(szClass, L"NotifyIconOverflowWindow") == 0 ||
        wcscmp(szRootClass, L"NotifyIconOverflowWindow") == 0 ||
        wcscmp(szClass, L"TaskbandToolTip") == 0 ||
        wcscmp(szClass, L"tooltips_class32") == 0 ||
        wcscmp(szClass, L"Xaml_WindowedPopupAdapterClass") == 0 ||
        wcscmp(szRootClass, L"Xaml_WindowedPopupAdapterClass") == 0 ||
        IsShellCoreWindow(hUnderCursor) ||
        IsShellCoreWindow(hRoot)) {
        return true;
    }

    return false;
}

// https://gist.github.com/m417z/451dfc2dad88d7ba88ed1814779a26b4
std::wstring GetWindowAppId(HWND hWnd) {
    // {c8900b66-a973-584b-8cae-355b7f55341b}
    constexpr winrt::guid CLSID_StartMenuCacheAndAppResolver{
        0x660b90c8,
        0x73a9,
        0x4b58,
        {0x8c, 0xae, 0x35, 0x5b, 0x7f, 0x55, 0x34, 0x1b}};

    // {de25675a-72de-44b4-9373-05170450c140}
    constexpr winrt::guid IID_IAppResolver_8{
        0xde25675a,
        0x72de,
        0x44b4,
        {0x93, 0x73, 0x05, 0x17, 0x04, 0x50, 0xc1, 0x40}};

    struct IAppResolver_8 : public IUnknown {
       public:
        virtual HRESULT STDMETHODCALLTYPE GetAppIDForShortcut() = 0;
        virtual HRESULT STDMETHODCALLTYPE GetAppIDForShortcutObject() = 0;
        virtual HRESULT STDMETHODCALLTYPE
        GetAppIDForWindow(HWND hWnd,
                          WCHAR** pszAppId,
                          void* pUnknown1,
                          void* pUnknown2,
                          void* pUnknown3) = 0;
        virtual HRESULT STDMETHODCALLTYPE
        GetAppIDForProcess(DWORD dwProcessId,
                           WCHAR** pszAppId,
                           void* pUnknown1,
                           void* pUnknown2,
                           void* pUnknown3) = 0;
    };

    HRESULT hr;
    std::wstring result;

    CO_MTA_USAGE_COOKIE cookie;
    bool mtaUsageIncreased = SUCCEEDED(CoIncrementMTAUsage(&cookie));

    winrt::com_ptr<IAppResolver_8> appResolver;
    hr = CoCreateInstance(CLSID_StartMenuCacheAndAppResolver, nullptr,
                          CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER,
                          IID_IAppResolver_8, appResolver.put_void());
    if (SUCCEEDED(hr)) {
        WCHAR* pszAppId;
        hr = appResolver->GetAppIDForWindow(hWnd, &pszAppId, nullptr, nullptr,
                                            nullptr);
        if (SUCCEEDED(hr)) {
            result = pszAppId;
            CoTaskMemFree(pszAppId);
        }
    }

    appResolver = nullptr;

    if (mtaUsageIncreased) {
        CoDecrementMTAUsage(cookie);
    }

    return result;
}

std::wstring GetProcessFileName(DWORD dwProcessId) {
    HANDLE hProcess =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
    if (!hProcess) {
        return std::wstring{};
    }

    WCHAR processPath[MAX_PATH];

    DWORD dwSize = ARRAYSIZE(processPath);
    if (!QueryFullProcessImageName(hProcess, 0, processPath, &dwSize)) {
        CloseHandle(hProcess);
        return std::wstring{};
    }

    CloseHandle(hProcess);

    PCWSTR processFileName = wcsrchr(processPath, L'\\');
    if (!processFileName) {
        return std::wstring{};
    }

    processFileName++;
    return processFileName;
}

std::wstring GetWindowLogInfo(HWND hWnd) {
    DWORD dwProcessId = 0;
    GetWindowThreadProcessId(hWnd, &dwProcessId);
    std::wstring processName = GetProcessFileName(dwProcessId);

    WCHAR className[256];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        wcscpy_s(className, L"<unknown>");
    }

    WCHAR windowName[256];
    if (!GetWindowText(hWnd, windowName, ARRAYSIZE(windowName))) {
        windowName[0] = L'\0';
    }

    LONG style = GetWindowLong(hWnd, GWL_STYLE);
    LONG exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

    RECT rect{};
    GetWindowRect(hWnd, &rect);

    WCHAR buffer[1024];
    swprintf_s(buffer,
               L"window %08X: PID=%u, process=%s, class=%s, name=%s, "
               L"style=0x%08X, exStyle=0x%08X, rect={%d,%d,%d,%d}",
               (DWORD)(DWORD_PTR)hWnd, dwProcessId, processName.c_str(),
               className, windowName, style, exStyle, rect.left, rect.top,
               rect.right, rect.bottom);
    return buffer;
}

bool IsWindowExcluded(HWND hWnd) {
    if (g_settings.excludedPrograms.empty()) {
        return false;
    }

    DWORD resolvedWindowProcessPathLen = 0;
    WCHAR resolvedWindowProcessPath[MAX_PATH];
    WCHAR resolvedWindowProcessPathUpper[MAX_PATH];

    DWORD dwProcessId = 0;
    if (GetWindowThreadProcessId(hWnd, &dwProcessId)) {
        HANDLE hProcess =
            OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
        if (hProcess) {
            DWORD dwSize = ARRAYSIZE(resolvedWindowProcessPath);
            if (QueryFullProcessImageName(hProcess, 0,
                                          resolvedWindowProcessPath, &dwSize)) {
                resolvedWindowProcessPathLen = dwSize;
            }

            CloseHandle(hProcess);
        }
    }

    if (resolvedWindowProcessPathLen > 0) {
        LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE,
                      resolvedWindowProcessPath,
                      resolvedWindowProcessPathLen + 1,
                      resolvedWindowProcessPathUpper,
                      resolvedWindowProcessPathLen + 1, nullptr, nullptr, 0);
    } else {
        *resolvedWindowProcessPath = L'\0';
        *resolvedWindowProcessPathUpper = L'\0';
    }

    if (resolvedWindowProcessPathLen > 0 &&
        g_settings.excludedPrograms.contains(resolvedWindowProcessPathUpper)) {
        return true;
    }

    if (PCWSTR programFileNameUpper =
            wcsrchr(resolvedWindowProcessPathUpper, L'\\')) {
        programFileNameUpper++;
        if (*programFileNameUpper &&
            g_settings.excludedPrograms.contains(programFileNameUpper)) {
            return true;
        }
    }

    std::wstring appId = GetWindowAppId(hWnd);
    LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, appId.data(),
                  appId.length(), appId.data(), appId.length(), nullptr,
                  nullptr, 0);
    if (g_settings.excludedPrograms.contains(appId.c_str())) {
        return true;
    }

    return false;
}

static inline bool RectCoversMonitor(const RECT& r, const RECT& rcMonitor) {
    return r.left <= rcMonitor.left + 2 &&
           r.top <= rcMonitor.top + 2 &&
           r.right >= rcMonitor.right - 2 &&
           r.bottom >= rcMonitor.bottom - 2;
}

bool DoesWindowClientCoverMonitor(HWND hWnd, const RECT& rcMonitor) {
    AutoPerMonitorDpi dpiScope;

    WINDOWINFO wi{
        .cbSize = sizeof(WINDOWINFO),
    };
    if (GetWindowInfo(hWnd, &wi) && !IsRectEmpty(&wi.rcClient)) {
        if (RectCoversMonitor(wi.rcClient, rcMonitor)) {
            return true;
        }
    }

    RECT frameBounds{};
    if (SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS,
                                        &frameBounds, sizeof(frameBounds)))) {
        if (RectCoversMonitor(frameBounds, rcMonitor)) {
            return true;
        }
    }

    RECT clientRect{};
    if (GetClientRect(hWnd, &clientRect) && !IsRectEmpty(&clientRect)) {
        SetLastError(0);
        if (MapWindowPoints(hWnd, nullptr,
                            reinterpret_cast<POINT*>(&clientRect), 2) ||
            GetLastError() == 0) {
            if (RectCoversMonitor(clientRect, rcMonitor)) {
                return true;
            }
        }
    }

    return false;
}

bool IsMaximizedOrCoveringApp(HWND hWnd, const RECT& rcMonitor) {
    if (!hWnd || !IsWindow(hWnd)) {
        return false;
    }
    if (!DoesWindowClientCoverMonitor(hWnd, rcMonitor)) {
        return false;
    }
    if (IsZoomed(hWnd)) {
        return true;
    }
    LONG_PTR style = GetWindowLongPtr(hWnd, GWL_STYLE);
    // If the window has no caption (WS_CAPTION = WS_BORDER | WS_DLGFRAME),
    // it is a true fullscreen window (e.g. game, media player, F11 browser),
    // NOT a normal maximized window. Do NOT exempt it from rudeness!
    if ((style & WS_CAPTION) != WS_CAPTION) {
        return false;
    }
    if (style & WS_THICKFRAME) {
        return true;
    }
    return false;
}

bool IsCandidateRudeWindow(HWND hWnd) {
    if (!IsWindowVisible(hWnd) || IsWindowCloaked(hWnd) || IsIconic(hWnd)) {
        return false;
    }

    if (GetWindowLongPtr(hWnd, GWL_STYLE) & WS_CHILD) {
        return false;
    }

    if (GetWindowLongPtr(hWnd, GWL_EXSTYLE) &
        (WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT)) {
        return false;
    }

    if (hWnd == g_altTabViewHwnd || hWnd == GetShellWindow() ||
        GetProp(hWnd, L"DesktopWindow") || IsTaskbarWindow(hWnd)) {
        return false;
    }

    if (GetClassWord(hWnd, GCW_ATOM) == 32768) {
        return false;
    }

    return true;
}

HWND FindTopMonitorCoveringWindow(HMONITOR monitor,
                                  MONITORINFO* outMonitorInfo = nullptr) {
    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    if (!GetMonitorInfo(monitor, &monitorInfo)) {
        return nullptr;
    }

    if (outMonitorInfo) {
        *outMonitorInfo = monitorInfo;
    }

    HWND hForegroundWnd = GetForegroundWindow();
    if (hForegroundWnd && IsCandidateRudeWindow(hForegroundWnd) &&
        MonitorFromWindow(hForegroundWnd, MONITOR_DEFAULTTONEAREST) ==
            monitor &&
        DoesWindowClientCoverMonitor(hForegroundWnd, monitorInfo.rcMonitor)) {
        return hForegroundWnd;
    }

    HWND hTopCoveringWnd = nullptr;
    auto enumProc = [&](HWND hWnd) -> BOOL {
        if (!IsCandidateRudeWindow(hWnd)) {
            return TRUE;
        }

        if (MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST) != monitor) {
            return TRUE;
        }

        if (DoesWindowClientCoverMonitor(hWnd, monitorInfo.rcMonitor)) {
            hTopCoveringWnd = hWnd;
            return FALSE;
        }

        // Ignore small always-on-top utility/PiP windows, but stop if a normal
        // non-covering window sits above any full-monitor window on this
        // display.
        if (!(GetWindowLongPtr(hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST)) {
            return FALSE;
        }

        return TRUE;
    };

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& proc = *reinterpret_cast<decltype(enumProc)*>(lParam);
            return proc(hWnd);
        },
        reinterpret_cast<LPARAM>(&enumProc));

    return hTopCoveringWnd;
}

bool CanHideTaskbarForWindow(HWND hWnd,
                             HMONITOR monitor,
                             const MONITORINFO* monitorInfo,
                             const RECT* taskbarRect) {
    if (!IsWindowVisible(hWnd) || IsWindowCloaked(hWnd) || IsIconic(hWnd) ||
        (GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_NOACTIVATE)) {
        return false;
    }

    // Ignore the Alt+Tab overlay so it doesn't affect taskbar state.
    if (hWnd == g_altTabViewHwnd) {
        return false;
    }

    if (hWnd == GetShellWindow() || GetProp(hWnd, L"DesktopWindow")) {
        return false;
    }

    // Exclude menus (#32768).
    if (GetClassWord(hWnd, GCW_ATOM) == 32768) {
        return false;
    }

    // Check this after the other checks, as it's the most expensive one.
    if (IsWindowExcluded(hWnd)) {
        return false;
    }

    WINDOWPLACEMENT wp{
        .length = sizeof(WINDOWPLACEMENT),
    };

    if (GetWindowPlacement(hWnd, &wp) && wp.showCmd == SW_SHOWMAXIMIZED) {
        if (MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST) == monitor) {
            return true;
        }

        return false;
    }

    bool isWindowArranged = pIsWindowArranged && pIsWindowArranged(hWnd);
    if (isWindowArranged &&
        MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST) != monitor) {
        return false;
    }

    RECT windowRect{};
    DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &windowRect,
                          sizeof(windowRect));

    // Don't keep the taskbar shown for a fullscreen or monitor-covering window.
    if (EqualRect(&windowRect, &monitorInfo->rcMonitor) ||
        DoesWindowClientCoverMonitor(hWnd, monitorInfo->rcMonitor)) {
        return true;
    }

    // It makes sense to treat arranged windows (e.g. with Win+left) as
    // maximized, as they occupy the whole monitor height. Still check for
    // intersection, as a window can also just occupy the upper side of the
    // screen (e.g. Win+left, Win+up).
    if (g_settings.mode == Mode::intersected ||
        (g_settings.mode == Mode::maximized && isWindowArranged)) {
        RECT intersectRect;
        if (IntersectRect(&intersectRect, &windowRect, taskbarRect)) {
            return true;
        }
    }

    return false;
}

bool ShouldKeepTaskbarShown(HWND hTaskbarWnd, HMONITOR monitor) {
    if (g_settings.primaryMonitorOnly &&
        monitor != MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY)) {
        return false;
    }

    if (g_settings.mode == Mode::never) {
        return true;
    }

    // On the Windows 11 taskbar, the ViewCoordinator hooks are what actually let
    // the taskbar collapse. Until they're installed (symbols still loading, or
    // loading failed), stay out of the way and let Windows handle auto-hide.
    // Otherwise the taskbar gets pinned visible with no way to hide.
    if (g_winVersion >= WinVersion::Win11 && !g_settings.oldTaskbarOnWin11 &&
        !ViewCoordinator_ShouldTaskbarBeExpanded_Original) {
        return false;
    }

    // Always show taskbar when MultitaskingView (Win+Tab) is active.
    if (g_multitaskingViewHwnd) {
        HWND hMultitaskingViewWnd = g_multitaskingViewHwnd;
        if (IsWindow(hMultitaskingViewWnd) &&
            !IsWindowCloaked(hMultitaskingViewWnd)) {
            return true;
        }

        // The view is gone and we missed the event that said so. Forget it
        // instead of keeping the taskbar shown for the rest of the session.
        Wh_Log(L"MultitaskingView window %08X is gone",
               (DWORD)(ULONG_PTR)hMultitaskingViewWnd);
        g_multitaskingViewHwnd = nullptr;
    }

    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    GetMonitorInfo(monitor, &monitorInfo);

    RECT taskbarRect{};
    GetTaskbarRectForMonitor(monitor, &taskbarRect);

    if (g_settings.foregroundWindowOnly) {
        HWND hForegroundWnd = GetForegroundWindow();
        return !hForegroundWnd ||
               !CanHideTaskbarForWindow(hForegroundWnd, monitor, &monitorInfo,
                                        &taskbarRect);
    }

    bool canHideTaskbar = false;

    DWORD dwTaskbarThreadId = GetCurrentThreadId();

    auto enumWindowsProc = [&](HWND hWnd) -> BOOL {
        if (GetWindowThreadProcessId(hWnd, nullptr) == dwTaskbarThreadId) {
            return TRUE;
        }

        canHideTaskbar =
            CanHideTaskbarForWindow(hWnd, monitor, &monitorInfo, &taskbarRect);
        if (!canHideTaskbar) {
            return TRUE;
        }

        Wh_Log(L"Can hide taskbar %08X for %s", (DWORD)(DWORD_PTR)hTaskbarWnd,
               GetWindowLogInfo(hWnd).c_str());
        return FALSE;
    };

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& proc = *reinterpret_cast<decltype(enumWindowsProc)*>(lParam);
            return proc(hWnd);
        },
        reinterpret_cast<LPARAM>(&enumWindowsProc));

    return !canHideTaskbar;
}

void* QueryViaVtable(void* object, void* vtable) {
    void* ptr = object;
    while (*(void**)ptr != vtable) {
        ptr = (void**)ptr + 1;
    }
    return ptr;
}

void* QueryViaVtableBackwards(void* object, void* vtable) {
    void* ptr = object;
    while (*(void**)ptr != vtable) {
        ptr = (void**)ptr - 1;
    }
    return ptr;
}

DWORD WINAPI WinEventHookThread(LPVOID lpThreadParameter);

bool HookTaskbarViewDllSymbols(HMODULE module);
HMODULE GetTaskbarViewModuleHandle();

void AdjustTaskbar(HWND hMMTaskbarWnd, bool clearPendingWhenDone = false) {
    if (g_settings.mode != Mode::never) {
        if (!g_winEventHookThread) {
            std::lock_guard<std::mutex> guard(g_winEventHookThreadMutex);

            if (!g_winEventHookThread) {
                g_winEventHookThread = CreateThread(
                    nullptr, 0, WinEventHookThread, nullptr, 0, nullptr);
            }
        }
    }

    PostMessage(hMMTaskbarWnd, g_updateTaskbarStateRegisteredMsg,
                clearPendingWhenDone, 0);
}

// Taskbar.View.dll shows up shortly after Explorer starts, usually while its
// symbols are still being downloaded. We synchronize hooking attempts with CAS
// and a 30s throttle to avoid race conditions and never block the Explorer UI thread.
bool TryHookTaskbarViewDll(bool forceAttempt = false) {
    if (g_winVersion < WinVersion::Win11 || g_settings.oldTaskbarOnWin11 ||
        g_taskbarViewDllLoaded) {
        return false;
    }

    bool expected = false;
    if (!g_taskbarViewHookInProgress.compare_exchange_strong(expected, true)) {
        return false;
    }
    if (g_taskbarViewDllLoaded) {
        g_taskbarViewHookInProgress = false;
        return false;
    }

    DWORD now = GetTickCount();
    DWORD last = g_taskbarViewLastAttemptTick.load();
    if (!forceAttempt && last && (now - last < 30 * 1000)) {
        g_taskbarViewHookInProgress = false;
        return false;
    }

    g_taskbarViewLastAttemptTick.store(now);

    HMODULE module = GetTaskbarViewModuleHandle();
    if (!module) {
        g_taskbarViewHookInProgress = false;
        return false;
    }

    Wh_Log(L"Attempting Taskbar.View.dll hooks (module=%p)", module);

    if (HookTaskbarViewDllSymbols(module)) {
        g_taskbarViewDllLoaded = true;
        g_taskbarViewHookInProgress = false;
        Wh_ApplyHookOperations();
        Wh_Log(L"Taskbar.View.dll hooks applied successfully");
        return true;
    } else {
        Wh_Log(L"HookTaskbarViewDllSymbols failed, throttling retry");
        g_taskbarViewHookInProgress = false;
        return false;
    }
}

// A taskbar rebuilt after a display change can come back without the topmost
// flag. Its hidden edge - the two pixels the shell leaves on screen - then sits
// underneath whatever window is in front, so the mouse never reaches the
// taskbar and it can't be revealed by touching the screen edge. Bars the shell
// creates itself are topmost, so restore it here unless a true (non-maximized)
// fullscreen window currently owns that monitor.
void EnsureTaskbarTopmost(HWND hTaskbarWnd) {
    if (!g_taskbarsPendingTopmostCheck.contains(hTaskbarWnd)) {
        return;
    }
    g_taskbarsPendingTopmostCheck.erase(hTaskbarWnd);

    HMONITOR monitor = MonitorFromWindow(hTaskbarWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    if (!GetMonitorInfo(monitor, &monitorInfo)) {
        return;
    }

    HWND hCoveringWnd = FindTopMonitorCoveringWindow(monitor, &monitorInfo);
    if (hCoveringWnd && !IsMaximizedOrCoveringApp(hCoveringWnd, monitorInfo.rcMonitor)) {
        return;
    }

    if (!(GetWindowLongPtr(hTaskbarWnd, GWL_EXSTYLE) & WS_EX_TOPMOST)) {
        SetWindowPos(hTaskbarWnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        Wh_Log(L"Restored topmost for taskbar %08X",
               (DWORD)(ULONG_PTR)hTaskbarWnd);
    }
}

void AdjustAllTaskbars() {
    std::unordered_set<HWND> secondaryTaskbarWindows;
    HWND hWnd = FindTaskbarWindows(&secondaryTaskbarWindows);
    if (hWnd) {
        EnsureTaskbarTopmost(hWnd);
        AdjustTaskbar(hWnd);
    }

    for (HWND hSecondaryWnd : secondaryTaskbarWindows) {
        EnsureTaskbarTopmost(hSecondaryWnd);
        AdjustTaskbar(hSecondaryWnd);
    }
}

bool AdjustAllTaskbarsIfNotPending() {
    std::unordered_set<HWND> secondaryTaskbarWindows;
    HWND hWnd = FindTaskbarWindows(&secondaryTaskbarWindows);

    DWORD currentTickCount = GetTickCount();
    DWORD pendingTickCount = 0;

    if (hWnd) {
        DWORD tickCount = (DWORD)(DWORD_PTR)GetProp(
            hWnd, kUpdateTaskbarStatePendingTickCount);
        if (tickCount > pendingTickCount) {
            pendingTickCount = tickCount;
        }
    }

    for (HWND hSecondaryWnd : secondaryTaskbarWindows) {
        DWORD tickCount = (DWORD)(DWORD_PTR)GetProp(
            hSecondaryWnd, kUpdateTaskbarStatePendingTickCount);
        if (tickCount > pendingTickCount) {
            pendingTickCount = tickCount;
        }
    }

    // Consider times larger than 10 seconds as expired, to prevent having
    // it stuck in this state.
    if (pendingTickCount && currentTickCount - pendingTickCount < 1000 * 10) {
        return false;
    }

    if (hWnd) {
        SetProp(hWnd, kUpdateTaskbarStatePendingTickCount,
                (HANDLE)(DWORD_PTR)currentTickCount);
        AdjustTaskbar(hWnd, /*clearPendingWhenDone=*/true);
    }

    for (HWND hSecondaryWnd : secondaryTaskbarWindows) {
        SetProp(hSecondaryWnd, kUpdateTaskbarStatePendingTickCount,
                (HANDLE)(DWORD_PTR)currentTickCount);
        AdjustTaskbar(hSecondaryWnd, /*clearPendingWhenDone=*/true);
    }

    return true;
}

constexpr UINT_PTR kModRetractTimer = 0x54424D4D; // 'TBMM'
constexpr UINT_PTR kModDisplayChangeSettlingTimer = 0x54424D44; // 'TBMD'
constexpr PCWSTR kModRetractLeftTickProp = L"Windhawk_TBMM_LeftTick";

bool IsTaskbarOrChildForeground(HWND hTaskbarWnd) {
    HWND hFore = GetForegroundWindow();
    if (!hFore) {
        return false;
    }
    if (hFore == hTaskbarWnd || IsChild(hTaskbarWnd, hFore)) {
        return true;
    }
    HWND hCapture = GetCapture();
    if (hCapture && (hCapture == hTaskbarWnd || IsChild(hTaskbarWnd, hCapture))) {
        return true;
    }

    HWND hRoot = GetAncestor(hFore, GA_ROOT);
    if (!hRoot) {
        hRoot = hFore;
    }

    // The desktop window (Progman, WorkerW, or Shell window) is never a taskbar flyout.
    if (hRoot == GetShellWindow()) {
        return false;
    }

    WCHAR szRootClass[64]{};
    if (GetClassNameW(hRoot, szRootClass, ARRAYSIZE(szRootClass))) {
        if (wcscmp(szRootClass, L"Progman") == 0 ||
            wcscmp(szRootClass, L"WorkerW") == 0 ||
            wcscmp(szRootClass, L"CabinetWClass") == 0) {
            return false;
        }
    }

    // Check if the foreground window is on this taskbar's monitor.
    HMONITOR hMonTaskbar = MonitorFromWindow(hTaskbarWnd, MONITOR_DEFAULTTONEAREST);
    HMONITOR hMonFore = MonitorFromWindow(hFore, MONITOR_DEFAULTTONULL);
    if (hMonFore && hMonTaskbar && hMonFore != hMonTaskbar) {
        return false;
    }

    WCHAR szClass[64]{};
    GetClassNameW(hFore, szClass, ARRAYSIZE(szClass));

    if (wcscmp(szClass, L"#32768") == 0 ||
        wcscmp(szRootClass, L"#32768") == 0 ||
        wcscmp(szClass, L"TopLevelWindowForOverflowXamlIsland") == 0 ||
        wcscmp(szRootClass, L"TopLevelWindowForOverflowXamlIsland") == 0 ||
        wcscmp(szClass, L"NotifyIconOverflowWindow") == 0 ||
        wcscmp(szRootClass, L"NotifyIconOverflowWindow") == 0 ||
        wcscmp(szClass, L"Xaml_WindowedPopupAdapterClass") == 0 ||
        wcscmp(szRootClass, L"Xaml_WindowedPopupAdapterClass") == 0 ||
        wcscmp(szClass, L"TaskListThumbnailWnd") == 0 ||
        wcscmp(szRootClass, L"TaskListThumbnailWnd") == 0 ||
        IsShellCoreWindow(hFore) ||
        IsShellCoreWindow(hRoot)) {
        return true;
    }

    return false;
}

bool WINAPI ViewCoordinator_ShouldTaskbarBeExpanded_Hook(void* pThis,
                                                         HWND hMMTaskbarWnd,
                                                         bool expanded) {
    Wh_Log(L"> hMMTaskbarWnd=%08X, expanded=%d",
           (DWORD)(ULONG_PTR)hMMTaskbarWnd, expanded);

    g_taskbarToViewCoordinator[hMMTaskbarWnd] = pThis;

    // Return true if the taskbar should be kept shown.
    for (const auto& pair : g_taskbarsKeptShown) {
        if (pair.second == hMMTaskbarWnd) {
            Wh_Log(L"Returning true for taskbar kept shown");
            return true;
        }
    }

    bool result = ViewCoordinator_ShouldTaskbarBeExpanded_Original(
        pThis, hMMTaskbarWnd, expanded);

    if (result && hMMTaskbarWnd == FindCurrentProcessTaskbarWnd()) {
        // Arm the retract safety watcher only for the primary taskbar.
        SetTimer(hMMTaskbarWnd, kModRetractTimer, 100, nullptr);
    }

    return result;
}

using ViewCoordinator_UpdateIsExpanded_t = void(WINAPI*)(void* pThis,
                                                         HWND hMMTaskbarWnd,
                                                         int reason);
ViewCoordinator_UpdateIsExpanded_t ViewCoordinator_UpdateIsExpanded_Original;

using ViewCoordinator_CollapseTaskbar_t = void(WINAPI*)(void* pThis,
                                                         HWND hMMTaskbarWnd);
ViewCoordinator_CollapseTaskbar_t ViewCoordinator_CollapseTaskbar_Original;


using ViewCoordinator_ForceCompleteEdgeGesture_t = void(WINAPI*)(void* pThis,
                                                                 HWND hMMTaskbarWnd);
ViewCoordinator_ForceCompleteEdgeGesture_t
    ViewCoordinator_ForceCompleteEdgeGesture_Original;

void UpdateViewCoordinatorIsExpanded(HWND hWnd) {
    // From ViewCoordinator::HandleIsPointerOverTaskbarFrameChanged.
    constexpr int kReasonIsPointerOverTaskbarFrameChanged = 7;

    if (ViewCoordinator_UpdateIsExpanded_Original) {
        void* pViewCoord = nullptr;
        auto it = g_taskbarToViewCoordinator.find(hWnd);
        if (it != g_taskbarToViewCoordinator.end() && it->second) {
            pViewCoord = it->second;
        }
        if (pViewCoord) {
            try {
                ViewCoordinator_UpdateIsExpanded_Original(
                    pViewCoord, hWnd, kReasonIsPointerOverTaskbarFrameChanged);
            } catch (...) {
                Wh_Log(L"Exception in ViewCoordinator_UpdateIsExpanded for hWnd %08X",
                       (DWORD)(ULONG_PTR)hWnd);
            }
        }
    }
}

void CollapseWin11Taskbar(HWND hWnd) {
    if (g_winVersion < WinVersion::Win11 || g_settings.oldTaskbarOnWin11) {
        return;
    }
    void* pVC = nullptr;
    auto it = g_taskbarToViewCoordinator.find(hWnd);
    if (it != g_taskbarToViewCoordinator.end() && it->second) {
        pVC = it->second;
    }
    if (!pVC) {
        return;
    }

    try {
        if (ViewCoordinator_ForceCompleteEdgeGesture_Original) {
            ViewCoordinator_ForceCompleteEdgeGesture_Original(pVC, hWnd);
        }
        if (ViewCoordinator_CollapseTaskbar_Original) {
            ViewCoordinator_CollapseTaskbar_Original(pVC, hWnd);
        }
    } catch (...) {
        Wh_Log(L"Exception in CollapseWin11Taskbar for hWnd %08X",
               (DWORD)(ULONG_PTR)hWnd);
    }
    UpdateViewCoordinatorIsExpanded(hWnd);
}

void* TrayUI_vftable_IInspectable;
void* TrayUI_vftable_ITrayComponentHost;
void* CSecondaryTray_vftable_ISecondaryTray;

using TrayUI_GetStuckMonitor_t = HMONITOR(WINAPI*)(void* pThis);
TrayUI_GetStuckMonitor_t TrayUI_GetStuckMonitor_Original;

using CSecondaryTray_GetMonitor_t = HMONITOR(WINAPI*)(void* pThis);
CSecondaryTray_GetMonitor_t CSecondaryTray_GetMonitor_Original;

using TrayUI_GetStuckRectForMonitor_t = bool(WINAPI*)(void* pThis,
                                                      HMONITOR hMonitor,
                                                      RECT* rect);
TrayUI_GetStuckRectForMonitor_t TrayUI_GetStuckRectForMonitor_Original;

using TrayUI_GetStuckRectForMonitor_Win10_t = RECT*(WINAPI*)(void* pThis,
                                                             RECT* rect,
                                                             HMONITOR hMonitor);
TrayUI_GetStuckRectForMonitor_Win10_t
    TrayUI_GetStuckRectForMonitor_Win10_Original;

using TrayUI__Hide_t = void(WINAPI*)(void* pThis);
TrayUI__Hide_t TrayUI__Hide_Original;

// g_taskbarsKeptShown is a cache that only gets refreshed on window events, so
// it can outlive its cause - for example when the mod is toggled off and on
// while a taskbar is kept shown, or when the event that should have removed the
// entry never arrives. Trusting the cache blindly would then kill the hide
// timer forever and leave the taskbar stuck visible until Explorer restarts, so
// re-check the current state before blocking a hide.
bool StillKeepTaskbarShown(std::unordered_map<void*, HWND>& keptShown,
                           std::unordered_map<void*, HWND>::iterator it) {
    HWND hWnd = it->second;

    HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    if (ShouldKeepTaskbarShown(hWnd, monitor)) {
        return true;
    }

    Wh_Log(L"Kept shown state is stale for taskbar %08X", (DWORD)(ULONG_PTR)hWnd);

    keptShown.erase(it);
    UpdateViewCoordinatorIsExpanded(hWnd);
    return false;
}

void WINAPI TrayUI__Hide_Hook(void* pThis) {
    Wh_Log(L">");

    auto it = g_taskbarsKeptShown.find(pThis);
    if (it != g_taskbarsKeptShown.end()) {
        HWND hWnd = it->second;
        if (StillKeepTaskbarShown(g_taskbarsKeptShown, it)) {
            KillTimer(hWnd, kTrayUITimerHide);
            return;
        }
    }

    TrayUI__Hide_Original(pThis);

    // Whichever code path hid the taskbar - the shell's own timers included -
    // the shell's unhide timer (the one that watches the screen edge) is not
    // necessarily armed, so touching the edge would do nothing. Arm it here.
    if (auto wndIt = g_taskbarWndByObject.find(pThis);
        wndIt != g_taskbarWndByObject.end()) {
        SetTimer(wndIt->second, kTrayUITimerUnhide, 100, nullptr);
    }
}

using CSecondaryTray__AutoHide_t = void(WINAPI*)(void* pThis, bool param1);
CSecondaryTray__AutoHide_t CSecondaryTray__AutoHide_Original;
void WINAPI CSecondaryTray__AutoHide_Hook(void* pThis, bool param1) {
    Wh_Log(L">");

    auto it = g_taskbarsKeptShown.find(pThis);
    if (it != g_taskbarsKeptShown.end()) {
        HWND hWnd = it->second;
        if (StillKeepTaskbarShown(g_taskbarsKeptShown, it)) {
            KillTimer(hWnd, kTrayUITimerHide);
            return;
        }
    }

    CSecondaryTray__AutoHide_Original(pThis, param1);

    if (param1) {
        if (auto wndIt = g_taskbarWndByObject.find(pThis);
            wndIt != g_taskbarWndByObject.end()) {
            SetTimer(wndIt->second, kTrayUITimerUnhide, 100, nullptr);
        }
    }
}

using TrayUI_Unhide_t = void(WINAPI*)(void* pThis,
                                      int trayUnhideFlags,
                                      int unhideRequest);
TrayUI_Unhide_t TrayUI_Unhide_Original;

using CSecondaryTray__Unhide_t = void(WINAPI*)(void* pThis,
                                               int trayUnhideFlags,
                                               int unhideRequest);
CSecondaryTray__Unhide_t CSecondaryTray__Unhide_Original;

// In-process rudeness suppression: when a maximized frameless window triggers
// rude-window notifications, keep RudenessChanged / OnRudeWindowStateChange
// from stripping WS_EX_TOPMOST and setting TaskbarModel::IsFullscreenWindowHidingTaskbar(true).
// True fullscreen apps (where IsZoomed is false) pass through untouched.
using TrayUI_RudenessChanged_t = void(WINAPI*)(void* pThis, int isRude);
TrayUI_RudenessChanged_t TrayUI_RudenessChanged_Original;
void WINAPI TrayUI_RudenessChanged_Hook(void* pThis, int isRude) {
    if (isRude) {
        HMONITOR primaryMonitor =
            MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO monitorInfo{
            .cbSize = sizeof(MONITORINFO),
        };
        HWND hCoveringWnd =
            FindTopMonitorCoveringWindow(primaryMonitor, &monitorInfo);
        if (hCoveringWnd &&
            IsMaximizedOrCoveringApp(hCoveringWnd, monitorInfo.rcMonitor) &&
            !IsWindowExcluded(hCoveringWnd)) {
            Wh_Log(
                L"Suppressing TrayUI::RudenessChanged(1) for maximized/covering window "
                L"%08X",
                (DWORD)(ULONG_PTR)hCoveringWnd);
            isRude = 0;
        }
    }

    TrayUI_RudenessChanged_Original(pThis, isRude);
}

using CSecondaryTray_OnRudeWindowStateChange_t =
    void(WINAPI*)(void* pThis, void* pMonitor, int flags, HWND hRudeWnd);
CSecondaryTray_OnRudeWindowStateChange_t
    CSecondaryTray_OnRudeWindowStateChange_Original;
void WINAPI CSecondaryTray_OnRudeWindowStateChange_Hook(void* pThis,
                                                        void* pMonitor,
                                                        int flags,
                                                        HWND hRudeWnd) {
    if ((flags & 1) && hRudeWnd && !IsWindowExcluded(hRudeWnd)) {
        HMONITOR monitor = MonitorFromWindow(hRudeWnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo{
            .cbSize = sizeof(MONITORINFO),
        };
        if (GetMonitorInfo(monitor, &monitorInfo) &&
            IsMaximizedOrCoveringApp(hRudeWnd, monitorInfo.rcMonitor)) {
            Wh_Log(
                L"Suppressing CSecondaryTray::OnRudeWindowStateChange for "
                L"maximized/covering window %08X",
                (DWORD)(ULONG_PTR)hRudeWnd);
            flags &= ~1;
        }
    }

    CSecondaryTray_OnRudeWindowStateChange_Original(pThis, pMonitor, flags,
                                                    hRudeWnd);
}

using TrayUI__ResetZorder_t = void(WINAPI*)(void* pThis, int flags);
TrayUI__ResetZorder_t TrayUI__ResetZorder_Original;
void WINAPI TrayUI__ResetZorder_Hook(void* pThis, int flags) {
    TrayUI__ResetZorder_Original(pThis, flags);

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd && g_taskbarsPendingTopmostCheck.contains(hTaskbarWnd)) {
        HMONITOR primaryMonitor =
            MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO monitorInfo{
            .cbSize = sizeof(MONITORINFO),
        };
        HWND hCoveringWnd = FindTopMonitorCoveringWindow(primaryMonitor, &monitorInfo);
        if (!hCoveringWnd || IsMaximizedOrCoveringApp(hCoveringWnd, monitorInfo.rcMonitor)) {
            EnsureTaskbarTopmost(hTaskbarWnd);
        }
    }
}

using CSecondaryTray__ResetZorder_t = void(WINAPI*)(void* pThis, int flags);
CSecondaryTray__ResetZorder_t CSecondaryTray__ResetZorder_Original;
void WINAPI CSecondaryTray__ResetZorder_Hook(void* pThis, int flags) {
    CSecondaryTray__ResetZorder_Original(pThis, flags);

    auto wndIt = g_taskbarWndByObject.find(pThis);
    if (wndIt != g_taskbarWndByObject.end() &&
        g_taskbarsPendingTopmostCheck.contains(wndIt->second)) {
        HWND hWnd = wndIt->second;
        HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo{
            .cbSize = sizeof(MONITORINFO),
        };
        HWND hCoveringWnd = FindTopMonitorCoveringWindow(monitor, &monitorInfo);
        if (!hCoveringWnd || IsMaximizedOrCoveringApp(hCoveringWnd, monitorInfo.rcMonitor)) {
            EnsureTaskbarTopmost(hWnd);
        }
    }
}

using TrayUI_WndProc_t = LRESULT(WINAPI*)(void* pThis,
                                          HWND hWnd,
                                          UINT Msg,
                                          WPARAM wParam,
                                          LPARAM lParam,
                                          bool* flag);
TrayUI_WndProc_t TrayUI_WndProc_Original;
LRESULT WINAPI TrayUI_WndProc_Hook(void* pThis,
                                   HWND hWnd,
                                   UINT Msg,
                                   WPARAM wParam,
                                   LPARAM lParam,
                                   bool* flag) {
    if (Msg == WM_NCCREATE) {
        Wh_Log(L"WM_NCCREATE: %08X", (DWORD)(ULONG_PTR)hWnd);
        g_taskbarsPendingTopmostCheck.insert(hWnd);
        AdjustTaskbar(hWnd);
        g_taskbarWndByObject[pThis] = hWnd;
    } else if (Msg == WM_NCDESTROY) {
        Wh_Log(L"WM_NCDESTROY: %08X", (DWORD)(ULONG_PTR)hWnd);
        KillTimer(hWnd, kModRetractTimer);
        KillTimer(hWnd, kModDisplayChangeSettlingTimer);
        RemoveProp(hWnd, kModRetractLeftTickProp);
        RemoveProp(hWnd, L"Windhawk_WasFullyShown");
        g_taskbarsKeptShown.erase(
            QueryViaVtableBackwards(pThis, TrayUI_vftable_IInspectable));
        g_taskbarToViewCoordinator.erase(hWnd);
        g_taskbarsPendingTopmostCheck.erase(hWnd);
        g_taskbarWndByObject.erase(pThis);
    } else if (Msg == kHandleTrayPrivateSettingMessage) {
        if (wParam == kTrayPrivateSettingAutoHideSet && !lParam) {
            // Block external requests from disabling auto-hide while mod is active
            return 0;
        }
    } else if (Msg == WM_DISPLAYCHANGE) {
        Wh_Log(L"WM_DISPLAYCHANGE: %08X", (DWORD)(ULONG_PTR)hWnd);
        // Monitors came or went. Explorer rebuilds the taskbars and can lose
        // the auto-hide setting while doing it, so re-check everything immediately...
        AdjustAllTaskbars();
        // ...and arm a debounced settling timer so that after asynchronous
        // display re-enumeration and window restore completes, the layout is re-evaluated.
        SetTimer(hWnd, kModDisplayChangeSettlingTimer, 750, nullptr);
    } else if (Msg == WM_TIMER && wParam == kModDisplayChangeSettlingTimer) {
        KillTimer(hWnd, kModDisplayChangeSettlingTimer);
        Wh_Log(L"kModDisplayChangeSettlingTimer expired: re-evaluating all taskbars");
        std::unordered_set<HWND> secondaryTaskbars;
        FindTaskbarWindows(&secondaryTaskbars);
        g_taskbarsPendingTopmostCheck.insert(hWnd);
        for (HWND hSec : secondaryTaskbars) {
            g_taskbarsPendingTopmostCheck.insert(hSec);
        }
        EnsureAutoHideEnabled();
        AdjustAllTaskbars();
        return 0;
    } else if (Msg == WM_WINDOWPOSCHANGED) {
        HMONITOR mon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        bool fullyShown = IsTaskbarFullyShown(hWnd, mon);
        bool wasFullyShown = GetProp(hWnd, L"Windhawk_WasFullyShown") != nullptr;
        if (fullyShown && !wasFullyShown) {
            SetProp(hWnd, L"Windhawk_WasFullyShown", (HANDLE)1);
            // When the taskbar is fully revealed while auto-hide is active,
            // ensure the retract timer is running so it collapses after the cursor leaves.
            bool keepShown = ShouldKeepTaskbarShown(hWnd, mon);
            if (!keepShown) {
                SetTimer(hWnd, kModRetractTimer, 100, nullptr);
            }
        } else if (!fullyShown && wasFullyShown) {
            RemoveProp(hWnd, L"Windhawk_WasFullyShown");
        }
    } else if (Msg == WM_TIMER && wParam == kModRetractTimer) {
        HMONITOR monitor =
            GetTaskbarMonitor(hWnd, TrayUI_GetStuckMonitor_Original(pThis));
        bool keepShown = ShouldKeepTaskbarShown(hWnd, monitor);

        if (keepShown) {
            KillTimer(hWnd, kModRetractTimer);
            RemoveProp(hWnd, kModRetractLeftTickProp);
            return 0;
        }

        if (IsCursorOverTaskbar(hWnd) || IsTaskbarOrChildForeground(hWnd)) {
            RemoveProp(hWnd, kModRetractLeftTickProp);
            return 0;
        }

        bool isExpanded = false;
        if (g_winVersion >= WinVersion::Win11 && !g_settings.oldTaskbarOnWin11) {
            isExpanded = IsTaskbarFullyShown(hWnd, monitor);
        } else {
            isExpanded = IsTaskbarOnScreen(hWnd, monitor);
        }

        // If neither expanded nor on screen, stop the retract timer.
        if (!isExpanded && !IsTaskbarOnScreen(hWnd, monitor)) {
            KillTimer(hWnd, kModRetractTimer);
            RemoveProp(hWnd, kModRetractLeftTickProp);
            return 0;
        }

        DWORD currentTick = GetTickCount();
        DWORD leftTick = (DWORD)(ULONG_PTR)GetProp(hWnd, kModRetractLeftTickProp);
        if (leftTick == 0) {
            SetProp(hWnd, kModRetractLeftTickProp,
                    (HANDLE)(ULONG_PTR)currentTick);
            return 0;
        }

        if (currentTick - leftTick >= 400) {
            RemoveProp(hWnd, kModRetractLeftTickProp);
            KillTimer(hWnd, kModRetractTimer);

            Wh_Log(L"Retract timeout expired, requesting shell hide for %08X",
                   (DWORD)(ULONG_PTR)hWnd);

            if (EnsureAutoHideEnabled()) {
                PostMessage(hWnd, WM_TIMER, kTrayUITimerHide, 0);
            } else if (g_autoHideEnableFailed) {
                CollapseWin11Taskbar(hWnd);
                TrayUI__Hide_Original(pThis);
                SetTimer(hWnd, kTrayUITimerUnhide, 100, nullptr);
            }
            return 0;
        }

        return 0;
    } else if (Msg == g_getTaskbarRectRegisteredMsg) {
        HMONITOR monitor = (HMONITOR)wParam;
        RECT* rect = (RECT*)lParam;
        if (TrayUI_GetStuckRectForMonitor_Original) {
            if (!TrayUI_GetStuckRectForMonitor_Original(pThis, monitor, rect)) {
                SetRectEmpty(rect);
            }
        } else if (TrayUI_GetStuckRectForMonitor_Win10_Original) {
            TrayUI_GetStuckRectForMonitor_Win10_Original(pThis, rect, monitor);
        } else {
            SetRectEmpty(rect);
        }
    } else if (Msg == g_updateTaskbarStateRegisteredMsg) {
        // Until ViewCoordinator hooks are installed on Win11, stay passive
        // and let Windows handle auto-hide.
        if (g_winVersion >= WinVersion::Win11 && !g_settings.oldTaskbarOnWin11 &&
            !ViewCoordinator_ShouldTaskbarBeExpanded_Original) {
            if (wParam) {
                RemoveProp(hWnd, kUpdateTaskbarStatePendingTickCount);
            }
            return TrayUI_WndProc_Original(pThis, hWnd, Msg, wParam, lParam, flag);
        }

        EnsureTaskbarTopmost(hWnd);
        g_taskbarsPendingTopmostCheck.erase(hWnd);

        if (!g_wasAutoHideProcessed) {
            g_wasAutoHideProcessed = true;
            g_wasAutoHideOriginallyDisabled =
                !SendMessage(hWnd, kHandleTrayPrivateSettingMessage,
                             kTrayPrivateSettingAutoHideGet, 0);
            if (g_wasAutoHideOriginallyDisabled) {
                SendMessage(hWnd, kHandleTrayPrivateSettingMessage,
                            kTrayPrivateSettingAutoHideSet, TRUE);
            }
        }

        HMONITOR monitor =
            GetTaskbarMonitor(hWnd, TrayUI_GetStuckMonitor_Original(pThis));
        bool keepShown = ShouldKeepTaskbarShown(hWnd, monitor);
        g_taskbarWndByObject[pThis] = hWnd;

        void* pTrayUI_IInspectable =
            QueryViaVtableBackwards(pThis, TrayUI_vftable_IInspectable);

        bool keptShown = g_taskbarsKeptShown.contains(pTrayUI_IInspectable);

        if (keepShown != keptShown) {
            Wh_Log(L"> keepShown=%d", keepShown);

            if (keepShown) {
                KillTimer(hWnd, kTrayUITimerUnhide);
                KillTimer(hWnd, kModRetractTimer);
                RemoveProp(hWnd, kModRetractLeftTickProp);
                g_taskbarsKeptShown[pTrayUI_IInspectable] = hWnd;

                void* pTrayUI_ITrayComponentHost =
                    QueryViaVtable(pThis, TrayUI_vftable_ITrayComponentHost);
                TrayUI_Unhide_Original(pTrayUI_ITrayComponentHost, 0, 0);

                UpdateViewCoordinatorIsExpanded(hWnd);
            } else {
                g_taskbarsKeptShown.erase(pTrayUI_IInspectable);

                PostMessage(hWnd, WM_TIMER, kTrayUITimerHide, 0);

                UpdateViewCoordinatorIsExpanded(hWnd);
            }
        }

        // The shell only runs its hide timer while "Automatically hide the
        // taskbar" is on, and it can lose that setting when it rebuilds the
        // taskbars after a display change. Ensure auto-hide is on and post
        // WM_TIMER kTrayUITimerHide so the shell validates focus, popups, and
        // flyouts before retracting. If auto-hide permanently failed, fall back
        // to direct hide.
        if (!keepShown && IsTaskbarFullyShown(hWnd, monitor)) {
            if (EnsureAutoHideEnabled()) {
                PostMessage(hWnd, WM_TIMER, kTrayUITimerHide, 0);
            } else if (g_autoHideEnableFailed) {
                if (!IsCursorOverTaskbar(hWnd) && !IsTaskbarOrChildForeground(hWnd)) {
                    CollapseWin11Taskbar(hWnd);
                    TrayUI__Hide_Original(pThis);
                    SetTimer(hWnd, kTrayUITimerUnhide, 100, nullptr);
                } else {
                    SetTimer(hWnd, kModRetractTimer, 100, nullptr);
                }
            }
        }

        if (wParam) {
            RemoveProp(hWnd, kUpdateTaskbarStatePendingTickCount);
        }
    }

    LRESULT ret =
        TrayUI_WndProc_Original(pThis, hWnd, Msg, wParam, lParam, flag);

    return ret;
}

using CSecondaryTray_v_WndProc_t = LRESULT(
    WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
CSecondaryTray_v_WndProc_t CSecondaryTray_v_WndProc_Original;
LRESULT WINAPI CSecondaryTray_v_WndProc_Hook(void* pThis,
                                             HWND hWnd,
                                             UINT Msg,
                                             WPARAM wParam,
                                             LPARAM lParam) {
    if (Msg == WM_NCCREATE) {
        Wh_Log(L"WM_NCCREATE: %08X", (DWORD)(ULONG_PTR)hWnd);
        g_taskbarsPendingTopmostCheck.insert(hWnd);
        AdjustTaskbar(hWnd);
        g_taskbarWndByObject[pThis] = hWnd;
    } else if (Msg == WM_NCDESTROY) {
        Wh_Log(L"WM_NCDESTROY: %08X", (DWORD)(ULONG_PTR)hWnd);
        g_taskbarsKeptShown.erase(pThis);
        g_taskbarToViewCoordinator.erase(hWnd);
        g_taskbarsPendingTopmostCheck.erase(hWnd);
        g_taskbarWndByObject.erase(pThis);
    } else if (Msg == g_updateTaskbarStateRegisteredMsg) {
        if (g_settings.primaryMonitorOnly) {
            if (wParam) {
                RemoveProp(hWnd, kUpdateTaskbarStatePendingTickCount);
            }
            return CSecondaryTray_v_WndProc_Original(pThis, hWnd, Msg, wParam, lParam);
        }

        // Until ViewCoordinator hooks are installed on Win11, stay passive
        // and let Windows handle auto-hide.
        if (g_winVersion >= WinVersion::Win11 && !g_settings.oldTaskbarOnWin11 &&
            !ViewCoordinator_ShouldTaskbarBeExpanded_Original) {
            if (wParam) {
                RemoveProp(hWnd, kUpdateTaskbarStatePendingTickCount);
            }
            return CSecondaryTray_v_WndProc_Original(pThis, hWnd, Msg, wParam, lParam);
        }

        EnsureTaskbarTopmost(hWnd);
        g_taskbarsPendingTopmostCheck.erase(hWnd);

        void* pCSecondaryTray_ISecondaryTray =
            QueryViaVtable(pThis, CSecondaryTray_vftable_ISecondaryTray);

        HMONITOR monitor = GetTaskbarMonitor(
            hWnd,
            CSecondaryTray_GetMonitor_Original(pCSecondaryTray_ISecondaryTray));

        bool keepShown = ShouldKeepTaskbarShown(hWnd, monitor);
        g_taskbarWndByObject[pThis] = hWnd;

        bool keptShown = g_taskbarsKeptShown.contains(pThis);

        if (keepShown != keptShown) {
            Wh_Log(L"> keepShown=%d", keepShown);

            if (keepShown) {
                KillTimer(hWnd, kTrayUITimerUnhide);
                g_taskbarsKeptShown[pThis] = hWnd;

                CSecondaryTray__Unhide_Original(pThis, 0, 0);

                UpdateViewCoordinatorIsExpanded(hWnd);
            } else {
                g_taskbarsKeptShown.erase(pThis);

                PostMessage(hWnd, WM_TIMER, kTrayUITimerHide, 0);

                UpdateViewCoordinatorIsExpanded(hWnd);
            }
        }

        // The shell only runs its hide timer while "Automatically hide the
        // taskbar" is on, and it can lose that setting when it rebuilds the
        // taskbars after a display change. Ensure auto-hide is on and post
        // WM_TIMER kTrayUITimerHide so the shell validates focus, popups, and
        // flyouts before retracting. If auto-hide permanently failed, fall back
        // to direct hide.
        if (!keepShown && IsTaskbarFullyShown(hWnd, monitor)) {
            if (EnsureAutoHideEnabled()) {
                PostMessage(hWnd, WM_TIMER, kTrayUITimerHide, 0);
            } else if (g_autoHideEnableFailed) {
                if (!IsCursorOverTaskbar(hWnd) && !IsTaskbarOrChildForeground(hWnd)) {
                    CSecondaryTray__AutoHide_Original(pThis, true);
                    SetTimer(hWnd, kTrayUITimerUnhide, 100, nullptr);
                }
            }
        }

        if (wParam) {
            RemoveProp(hWnd, kUpdateTaskbarStatePendingTickCount);
        }
    }

    LRESULT ret =
        CSecondaryTray_v_WndProc_Original(pThis, hWnd, Msg, wParam, lParam);

    return ret;
}

void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook,
                           DWORD event,
                           HWND hWnd,
                           LONG idObject,
                           LONG idChild,
                           DWORD dwEventThread,
                           DWORD dwmsEventTime) {
    if (idObject != OBJID_WINDOW ||
        (GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD) || IsTaskbarWindow(hWnd)) {
        return;
    }

    HWND hParentWnd = GetAncestor(hWnd, GA_PARENT);
    if (hParentWnd && hParentWnd != GetDesktopWindow()) {
        return;
    }

    // Check for Multitasking View window state changes.
    {
        auto multitaskingViewType = GetMultitaskingViewType(hWnd);
        bool isTrackedWinTab = hWnd == g_multitaskingViewHwnd;
        bool isTrackedAltTab = hWnd == g_altTabViewHwnd;

        if (multitaskingViewType != MultitaskingViewType::None ||
            isTrackedWinTab || isTrackedAltTab) {
            auto& targetHwnd =
                (multitaskingViewType == MultitaskingViewType::AltTab ||
                 isTrackedAltTab)
                    ? g_altTabViewHwnd
                    : g_multitaskingViewHwnd;

            bool entering = event == EVENT_OBJECT_SHOW ||
                            event == EVENT_OBJECT_UNCLOAKED ||
                            event == EVENT_OBJECT_CREATE;
            bool leaving = event == EVENT_OBJECT_HIDE ||
                           event == EVENT_OBJECT_CLOAKED ||
                           event == EVENT_OBJECT_DESTROY;

            if (entering) {
                HWND expected = nullptr;
                if (!targetHwnd.compare_exchange_strong(expected, hWnd)) {
                    return;
                }
                Wh_Log(
                    L"MultitaskingView entering (%s)",
                    &targetHwnd == &g_altTabViewHwnd ? L"Alt+Tab" : L"Win+Tab");
            } else if (leaving) {
                HWND expected = hWnd;
                if (!targetHwnd.compare_exchange_strong(expected, nullptr)) {
                    return;
                }
                Wh_Log(
                    L"MultitaskingView leaving (%s)",
                    &targetHwnd == &g_altTabViewHwnd ? L"Alt+Tab" : L"Win+Tab");
            } else {
                return;
            }

            // Fall through to trigger timer for all taskbars.
        }
    }

    Wh_Log(
        L"Event %s for %s",
        [](DWORD event) -> PCWSTR {
            switch (event) {
                case EVENT_OBJECT_CREATE:
                    return L"OBJECT_CREATE";
                case EVENT_OBJECT_DESTROY:
                    return L"OBJECT_DESTROY";
                case EVENT_OBJECT_SHOW:
                    return L"OBJECT_SHOW";
                case EVENT_OBJECT_HIDE:
                    return L"OBJECT_HIDE";
                case EVENT_OBJECT_LOCATIONCHANGE:
                    return L"OBJECT_LOCATIONCHANGE";
                case EVENT_OBJECT_CLOAKED:
                    return L"OBJECT_CLOAKED";
                case EVENT_OBJECT_UNCLOAKED:
                    return L"OBJECT_UNCLOAKED";
                case EVENT_SYSTEM_FOREGROUND:
                    return L"SYSTEM_FOREGROUND";
                default:
                    return L"UNKNOWN";
            }
        }(event),
        GetWindowLogInfo(hWnd).c_str());




    // Maximizing is the case where the hide delay is noticeable, so don't wait
    // for the event batch below: re-evaluate right away. Checking the flag is a
    // single read and only happens for location change events.
    if (event == EVENT_OBJECT_LOCATIONCHANGE && !g_pendingEventsTimer) {
        HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo{
            .cbSize = sizeof(MONITORINFO),
        };
        if (GetMonitorInfo(monitor, &monitorInfo) &&
            IsMaximizedOrCoveringApp(hWnd, monitorInfo.rcMonitor)) {
            if (AdjustAllTaskbarsIfNotPending()) {
                return;
            }
        }
    }

    if (g_pendingEventsTimer) {
        return;
    }

    g_pendingEventsTimer = SetTimer(
        nullptr, 0, 200,
        [](HWND hwnd,         // handle of window for timer messages
           UINT uMsg,         // WM_TIMER message
           UINT_PTR idEvent,  // timer identifier
           DWORD dwTime       // current system time
        ) {
            Wh_Log(L">");

            if (g_winVersion >= WinVersion::Win11 && !g_settings.oldTaskbarOnWin11 &&
                !g_taskbarViewDllLoaded) {
                if (TryHookTaskbarViewDll(false)) {
                    AdjustAllTaskbars();
                }
            }

            if (!AdjustAllTaskbarsIfNotPending()) {
                Wh_Log(L"Adjustment already pending, will retry later...");
                return;
            }

            KillTimer(nullptr, g_pendingEventsTimer);
            g_pendingEventsTimer = 0;
        });
}

DWORD WINAPI WinEventHookThread(LPVOID lpThreadParameter) {
    AutoPerMonitorDpi dpiScope;

    UINT_PTR retryTimer = 0;
    if (g_winVersion >= WinVersion::Win11 && !g_settings.oldTaskbarOnWin11 &&
        !g_taskbarViewDllLoaded) {
        retryTimer = SetTimer(nullptr, 0, 10000,
                              [](HWND, UINT, UINT_PTR id, DWORD) {
                                  if (g_taskbarViewDllLoaded) {
                                      KillTimer(nullptr, id);
                                      return;
                                  }
                                  if (TryHookTaskbarViewDll(false)) {
                                      KillTimer(nullptr, id);
                                      AdjustAllTaskbars();
                                  }
                              });
    }

    HWINEVENTHOOK winObjectEventHook1 =
        SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_HIDE, nullptr,
                        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!winObjectEventHook1) {
        Wh_Log(L"Error: SetWinEventHook");
    }

    HWINEVENTHOOK winObjectEventHook2 = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!winObjectEventHook2) {
        Wh_Log(L"Error: SetWinEventHook");
    }

    HWINEVENTHOOK winObjectEventHook3 =
        SetWinEventHook(EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED, nullptr,
                        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!winObjectEventHook3) {
        Wh_Log(L"Error: SetWinEventHook");
    }

    HWINEVENTHOOK winSystemEventHook1 = nullptr;
    if (g_settings.foregroundWindowOnly) {
        winSystemEventHook1 =
            SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                            nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
        if (!winSystemEventHook1) {
            Wh_Log(L"Error: SetWinEventHook");
        }
    }

    BOOL bRet;
    MSG msg;
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
        if (bRet == -1) {
            msg.wParam = 0;
            break;
        }

        if (msg.hwnd == NULL && msg.message == WM_APP) {
            PostQuitMessage(0);
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (retryTimer) {
        KillTimer(nullptr, retryTimer);
    }

    if (winObjectEventHook1) {
        UnhookWinEvent(winObjectEventHook1);
    }

    if (winObjectEventHook2) {
        UnhookWinEvent(winObjectEventHook2);
    }

    if (winObjectEventHook3) {
        UnhookWinEvent(winObjectEventHook3);
    }

    if (winSystemEventHook1) {
        UnhookWinEvent(winSystemEventHook1);
    }

    g_multitaskingViewHwnd = nullptr;
    g_altTabViewHwnd = nullptr;

    return 0;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: bool __cdecl winrt::Taskbar::implementation::ViewCoordinator::ShouldTaskbarBeExpanded(unsigned __int64,bool))"},
            &ViewCoordinator_ShouldTaskbarBeExpanded_Original,
            ViewCoordinator_ShouldTaskbarBeExpanded_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::UpdateIsExpanded(unsigned __int64,enum TaskbarTipTest::TaskbarExpandCollapseReason))"},
            &ViewCoordinator_UpdateIsExpanded_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: bool __cdecl winrt::Taskbar::implementation::ViewCoordinator::IsExpanded(unsigned __int64))"},
            &ViewCoordinator_IsExpanded_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::CollapseTaskbar(unsigned __int64))"},
            &ViewCoordinator_CollapseTaskbar_Original,
            nullptr,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::ForceCompleteEdgeGesture(unsigned __int64))"},
            &ViewCoordinator_ForceCompleteEdgeGesture_Original,
            nullptr,
            true,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (g_winVersion < WinVersion::Win11 || g_taskbarViewDllLoaded ||
        GetTaskbarViewModuleHandle() != module) {
        return;
    }

    Wh_Log(L"Loaded %s", lpLibFileName);
    bool force = (g_taskbarViewLastAttemptTick.load() == 0);
    TryHookTaskbarViewDll(force);
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
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

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

WinVersion GetExplorerVersion() {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo) {
        return WinVersion::Unsupported;
    }

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD minor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD qfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Version: %u.%u.%u.%u", major, minor, build, qfe);

    switch (major) {
        case 10:
            if (build < 22000) {
                return WinVersion::Win10;
            } else if (build < 26100) {
                return WinVersion::Win11;
            } else {
                return WinVersion::Win11_24H2;
            }
            break;
    }

    return WinVersion::Unsupported;
}

struct EXPLORER_PATCHER_HOOK {
    PCSTR symbol;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;

    template <typename Prototype>
    EXPLORER_PATCHER_HOOK(
        PCSTR symbol,
        Prototype** originalFunction,
        std::type_identity_t<Prototype*> hookFunction = nullptr,
        bool optional = false)
        : symbol(symbol),
          pOriginalFunction(reinterpret_cast<void**>(originalFunction)),
          hookFunction(reinterpret_cast<void*>(hookFunction)),
          optional(optional) {}
};

bool HookExplorerPatcherSymbols(HMODULE explorerPatcherModule) {
    if (g_explorerPatcherInitialized.exchange(true)) {
        return true;
    }

    if (g_winVersion >= WinVersion::Win11) {
        g_winVersion = WinVersion::Win10;
    }

    EXPLORER_PATCHER_HOOK hooks[] = {
        {R"(??_7TrayUI@@6BITrayDeskBand@@@)", &TrayUI_vftable_IInspectable},
        {R"(??_7TrayUI@@6BITrayComponentHost@@@)",
         &TrayUI_vftable_ITrayComponentHost},
        {R"(??_7CSecondaryTray@@6BISecondaryTray@@@)",
         &CSecondaryTray_vftable_ISecondaryTray},
        {R"(?GetStuckMonitor@TrayUI@@UEAAPEAUHMONITOR__@@XZ)",
         &TrayUI_GetStuckMonitor_Original},
        {R"(?GetMonitor@CSecondaryTray@@UEAAPEAUHMONITOR__@@XZ)",
         &CSecondaryTray_GetMonitor_Original},
        {R"(?GetStuckRectForMonitor@TrayUI@@UEAA_NPEAUHMONITOR__@@PEAUtagRECT@@@Z)",
         &TrayUI_GetStuckRectForMonitor_Original},
        {R"(?_Hide@TrayUI@@QEAAXXZ)", &TrayUI__Hide_Original,
         TrayUI__Hide_Hook},
        {R"(?_AutoHide@CSecondaryTray@@AEAAX_N@Z)",
         &CSecondaryTray__AutoHide_Original, CSecondaryTray__AutoHide_Hook},
        {R"(?Unhide@TrayUI@@UEAAXW4TrayUnhideFlags@TrayCommon@@W4UnhideRequest@3@@Z)",
         &TrayUI_Unhide_Original},
        {R"(?_Unhide@CSecondaryTray@@AEAAXW4TrayUnhideFlags@TrayCommon@@W4UnhideRequest@3@@Z)",
         &CSecondaryTray__Unhide_Original},
        {R"(?RudenessChanged@TrayUI@@UEAAXH@Z)",
         &TrayUI_RudenessChanged_Original, TrayUI_RudenessChanged_Hook, true},
        {R"(?OnRudeWindowStateChange@CSecondaryTray@@UEAAXPEAUIImmersiveMonitor@@W4RudeWindowStateChangeFlags@@PEAUHWND__@@@Z)",
         &CSecondaryTray_OnRudeWindowStateChange_Original,
         CSecondaryTray_OnRudeWindowStateChange_Hook, true},
        {R"(?WndProc@TrayUI@@UEAA_JPEAUHWND__@@I_K_JPEA_N@Z)",
         &TrayUI_WndProc_Original, TrayUI_WndProc_Hook},
        {R"(?v_WndProc@CSecondaryTray@@EEAA_JPEAUHWND__@@I_K_J@Z)",
         &CSecondaryTray_v_WndProc_Original, CSecondaryTray_v_WndProc_Hook,
         // Available in versions newer than 67.1.
         true},
    };

    bool succeeded = true;

    for (const auto& hook : hooks) {
        void* ptr = (void*)GetProcAddress(explorerPatcherModule, hook.symbol);
        if (!ptr) {
            Wh_Log(L"ExplorerPatcher symbol%s doesn't exist: %S",
                   hook.optional ? L" (optional)" : L"", hook.symbol);
            if (!hook.optional) {
                succeeded = false;
            }
            continue;
        }

        if (hook.hookFunction) {
            Wh_SetFunctionHook(ptr, hook.hookFunction, hook.pOriginalFunction);
        } else {
            *hook.pOriginalFunction = ptr;
        }
    }

    if (!succeeded) {
        Wh_Log(L"HookExplorerPatcherSymbols failed");
    } else if (g_initialized) {
        Wh_ApplyHookOperations();
    }

    return succeeded;
}

bool IsExplorerPatcherModule(HMODULE module) {
    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            return false;
    }

    PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) {
        return false;
    }

    moduleFileName++;

    if (_wcsnicmp(L"ep_taskbar.", moduleFileName, sizeof("ep_taskbar.") - 1) ==
        0) {
        Wh_Log(L"ExplorerPatcher taskbar module: %s", moduleFileName);
        return true;
    }

    return false;
}

bool HandleLoadedExplorerPatcher() {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods),
                           &cbNeeded)) {
        for (size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++) {
            if (IsExplorerPatcherModule(hMods[i])) {
                return HookExplorerPatcherSymbols(hMods[i]);
            }
        }
    }

    return true;
}

void HandleLoadedModuleIfExplorerPatcher(HMODULE module) {
    if (module && !((ULONG_PTR)module & 3) && !g_explorerPatcherInitialized) {
        if (IsExplorerPatcherModule(module)) {
            HookExplorerPatcherSymbols(module);
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfExplorerPatcher(module);
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }

    return module;
}

bool HookTaskbarSymbols() {
    HMODULE module;
    if (g_winVersion <= WinVersion::Win10) {
        module = GetModuleHandle(nullptr);
    } else {
        module = LoadLibraryEx(L"taskbar.dll", nullptr,
                               LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!module) {
            Wh_Log(L"Couldn't load taskbar.dll");
            return false;
        }
    }

    // Taskbar.dll, explorer.exe
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {
                // Windows 11.
                LR"(const TrayUI::`vftable'{for `IInspectable'})",

                // Windows 10.
                LR"(const TrayUI::`vftable'{for `ITrayDeskBand'})",
            },
            &TrayUI_vftable_IInspectable,
        },
        {
            {LR"(const TrayUI::`vftable'{for `ITrayComponentHost'})"},
            &TrayUI_vftable_ITrayComponentHost,
        },
        {
            {LR"(const CSecondaryTray::`vftable'{for `ISecondaryTray'})"},
            &CSecondaryTray_vftable_ISecondaryTray,
        },
        {
            {LR"(public: virtual struct HMONITOR__ * __cdecl TrayUI::GetStuckMonitor(void))"},
            &TrayUI_GetStuckMonitor_Original,
        },
        {
            {LR"(public: virtual struct HMONITOR__ * __cdecl CSecondaryTray::GetMonitor(void))"},
            &CSecondaryTray_GetMonitor_Original,
        },
        {
            {LR"(public: virtual bool __cdecl TrayUI::GetStuckRectForMonitor(struct HMONITOR__ *,struct tagRECT *))"},
            &TrayUI_GetStuckRectForMonitor_Original,
            nullptr,
            true,  // Windows 11.
        },
        {
            {LR"(public: virtual struct tagRECT __cdecl TrayUI::GetStuckRectForMonitor(struct HMONITOR__ *))"},
            &TrayUI_GetStuckRectForMonitor_Win10_Original,
            nullptr,
            true,  // Windows 10.
        },
        {
            {LR"(public: void __cdecl TrayUI::_Hide(void))"},
            &TrayUI__Hide_Original,
            TrayUI__Hide_Hook,
        },
        {
            {LR"(private: void __cdecl CSecondaryTray::_AutoHide(bool))"},
            &CSecondaryTray__AutoHide_Original,
            CSecondaryTray__AutoHide_Hook,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))"},
            &TrayUI_Unhide_Original,
        },
        {
            {LR"(private: void __cdecl CSecondaryTray::_Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))"},
            &CSecondaryTray__Unhide_Original,
        },
        {
            {LR"(public: void __cdecl TrayUI::_ResetZorder(int))"},
            &TrayUI__ResetZorder_Original,
            TrayUI__ResetZorder_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl CSecondaryTray::_ResetZorder(int))"},
            &CSecondaryTray__ResetZorder_Original,
            CSecondaryTray__ResetZorder_Hook,
            true,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::RudenessChanged(int))"},
            &TrayUI_RudenessChanged_Original,
            TrayUI_RudenessChanged_Hook,
            true,
        },
        {
            {LR"(public: virtual void __cdecl CSecondaryTray::OnRudeWindowStateChange(struct IImmersiveMonitor *,enum RudeWindowStateChangeFlags,struct HWND__ *))"},
            &CSecondaryTray_OnRudeWindowStateChange_Original,
            CSecondaryTray_OnRudeWindowStateChange_Hook,
            true,
        },
        {
            {LR"(public: virtual __int64 __cdecl TrayUI::WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,bool *))"},
            &TrayUI_WndProc_Original,
            TrayUI_WndProc_Hook,
        },
        {
            {LR"(private: virtual __int64 __cdecl CSecondaryTray::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &CSecondaryTray_v_WndProc_Original,
            CSecondaryTray_v_WndProc_Hook,
        },
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

void LoadSettings() {
    auto mode = WindhawkUtils::StringSetting::make(L"mode");
    g_settings.mode = Mode::intersected;
    if (wcscmp(mode.get(), L"maximized") == 0) {
        g_settings.mode = Mode::maximized;
    } else if (wcscmp(mode.get(), L"never") == 0) {
        g_settings.mode = Mode::never;
    }

    g_settings.foregroundWindowOnly = Wh_GetIntSetting(L"foregroundWindowOnly");

    g_settings.excludedPrograms.clear();

    for (int i = 0;; i++) {
        auto program =
            WindhawkUtils::StringSetting::make(L"excludedPrograms[%d]", i);

        if (!*program.get()) {
            break;
        }

        std::wstring programUpper = program.get();
        LCMapStringEx(
            LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, &programUpper[0],
            static_cast<int>(programUpper.length()), &programUpper[0],
            static_cast<int>(programUpper.length()), nullptr, nullptr, 0);

        g_settings.excludedPrograms.insert(std::move(programUpper));
    }

    g_settings.primaryMonitorOnly = Wh_GetIntSetting(L"primaryMonitorOnly");
    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    // Windhawk can unload and reload the mod without unloading the DLL, so stale
    // state from a previous run (a taskbar still marked as kept shown, or a
    // multitasking view window that's long gone) would survive and pin the
    // taskbar visible. Start from a clean slate.
    g_taskbarsKeptShown.clear();
    g_taskbarToViewCoordinator.clear();
    g_autoHideEnableFailed = false;
    g_autoHideEnableAttempted = false;
    g_taskbarsPendingTopmostCheck.clear();
    g_multitaskingViewHwnd = nullptr;
    g_altTabViewHwnd = nullptr;
    g_wasAutoHideProcessed = false;
    g_wasAutoHideOriginallyDisabled = false;

    HMODULE hUser32Module =
        LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hUser32Module) {
        pIsWindowArranged = (IsWindowArranged_t)GetProcAddress(
            hUser32Module, "IsWindowArranged");
        pGetWindowBand =
            (GetWindowBand_t)GetProcAddress(hUser32Module, "GetWindowBand");
    }

    g_winVersion = GetExplorerVersion();
    if (g_winVersion == WinVersion::Unsupported) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    if (g_settings.oldTaskbarOnWin11) {
        bool hasWin10Taskbar = g_winVersion < WinVersion::Win11_24H2;

        if (g_winVersion >= WinVersion::Win11) {
            g_winVersion = WinVersion::Win10;
        }

        if (hasWin10Taskbar && !HookTaskbarSymbols()) {
            return FALSE;
        }
    } else if (g_winVersion >= WinVersion::Win11) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            g_taskbarViewLastAttemptTick.store(GetTickCount());
            if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                g_taskbarViewDllLoaded = true;
            } else {
                // Don't fail the whole mod: the taskbar/tray hooks below still
                // work, and the ViewCoordinator hooks are retried later. Until
                // then, ShouldKeepTaskbarShown() stays out of the way and lets
                // Windows handle auto-hide.
                Wh_Log(L"HookTaskbarViewDllSymbols failed, will retry later");
            }
        } else {
            Wh_Log(L"Taskbar view module not loaded yet");
        }

        if (!HookTaskbarSymbols()) {
            return FALSE;
        }
    } else {
        if (!HookTaskbarSymbols()) {
            return FALSE;
        }
    }

    if (!HandleLoadedExplorerPatcher()) {
        Wh_Log(L"HandleLoadedExplorerPatcher failed");
        return FALSE;
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                   LoadLibraryExW_Hook,
                                   &LoadLibraryExW_Original);

    g_initialized = true;

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_winVersion >= WinVersion::Win11 && !g_taskbarViewDllLoaded) {
        TryHookTaskbarViewDll(false);
    }

    // Try again in case there's a race between the previous attempt and the
    // LoadLibraryExW hook.
    if (!g_explorerPatcherInitialized) {
        HandleLoadedExplorerPatcher();
    }

    WNDCLASS wndclass;
    if (GetClassInfo(GetModuleHandle(nullptr), L"Shell_TrayWnd", &wndclass)) {
        AdjustAllTaskbars();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_winEventHookThread) {
        PostThreadMessage(GetThreadId(g_winEventHookThread), WM_APP, 0, 0);
        WaitForSingleObject(g_winEventHookThread, INFINITE);
        CloseHandle(g_winEventHookThread);
        g_winEventHookThread = nullptr;
    }

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        KillTimer(hTaskbarWnd, kModRetractTimer);
        KillTimer(hTaskbarWnd, kModDisplayChangeSettlingTimer);
        RemoveProp(hTaskbarWnd, kModRetractLeftTickProp);
        RemoveProp(hTaskbarWnd, L"Windhawk_WasFullyShown");
    }
    for (const auto& pair : g_taskbarWndByObject) {
        if (pair.second && pair.second != hTaskbarWnd) {
            KillTimer(pair.second, kModRetractTimer);
            RemoveProp(pair.second, kModRetractLeftTickProp);
            RemoveProp(pair.second, L"Windhawk_WasFullyShown");
        }
    }

    if (g_wasAutoHideOriginallyDisabled) {
        if (hTaskbarWnd) {
            SendMessage(hTaskbarWnd, kHandleTrayPrivateSettingMessage,
                        kTrayPrivateSettingAutoHideSet, FALSE);
        }
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevForegroundWindowOnly = g_settings.foregroundWindowOnly;
    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    LoadSettings();

    if (g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11) {
        *bReload = TRUE;
        return TRUE;
    }

    if (g_settings.mode == Mode::never ||
        prevForegroundWindowOnly != g_settings.foregroundWindowOnly) {
        std::lock_guard<std::mutex> guard(g_winEventHookThreadMutex);

        if (g_winEventHookThread) {
            PostThreadMessage(GetThreadId(g_winEventHookThread), WM_APP, 0, 0);
            WaitForSingleObject(g_winEventHookThread, INFINITE);
            CloseHandle(g_winEventHookThread);
            g_winEventHookThread = nullptr;
        }
    }

    AdjustAllTaskbars();

    return TRUE;
}
