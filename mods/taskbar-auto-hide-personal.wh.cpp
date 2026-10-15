// ==WindhawkMod==
// @id              taskbar-auto-hide-personal
// @name            Taskbar Auto-Hide
// @description     Auto-hide policy and animation speed in one independent mod
// @version         0.1.0
// @author          Jip (based on two m417z taskbar mods)
// @github          https://github.com/iiRedPanda
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lversion -lruntimeobject
// ==/WindhawkMod==

// Modified fork; changes and upstream attribution are described in the embedded README.

// ==WindhawkModReadme==
/*
# Taskbar Auto-Hide

Independent controls for taskbar auto-hide policy and animation speed in one Windhawk mod.

## Features

- Windows default, hide for maximized windows, hide for intersecting windows, or never auto-hide.
- Foreground-window filtering, program exclusions, and primary-monitor selection.
- Optional animation override with separate show/hide speed controls.

## Settings

Choose **Auto-hide policy** to follow Windows, hide for maximized or intersecting windows, or keep the taskbar visible. Use **Foreground window only**, **Excluded programs**, and **Primary monitor only** to narrow which windows and displays affect the policy.

**Customize animation speed** changes the duration of supported taskbar animations. If Windows animation effects are disabled, the native taskbar has no visible animation to speed up. This setting does not turn Windows animation effects on.

Enable **Customize animation speed** to change **Show speed percent** and **Hide speed percent** independently. A value of 100 uses the original speed; larger values shorten the animation.

Policy and animation default to Windows behavior. The frame-rate setting applies to the legacy taskbar, not the native Windows 11 animation.

## Compatibility and status

Experimental source release. Compilation and the default no-override configuration have been checked on one Windows 11 PC with three displays. The maximized-window policy was checked with desktop, small-window, and maximized-window states. Speed customization did not change the visible result there because Windows animation effects were disabled. Other policy modes, animation behavior with effects enabled, broader multi-monitor behavior, and Explorer restart recovery remain unverified. The inherited ExplorerPatcher path is retained; StartAllBack compatibility is unverified. Concurrent policy/speed mods can apply overlapping hooks.

## Credits and license

Modified GPLv3 fork of m417z's [Taskbar Auto-Hide When Maximized](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-auto-hide-when-maximized.wh.cpp) and [Taskbar Auto-Hide Speed](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-auto-hide-speed.wh.cpp). Fork modifications are credited in the source metadata.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- mode: system
  $name: Auto-hide policy
  $options:
  - system: Windows default
  - intersected: When a window intersects the taskbar
  - maximized: When a window is maximized
  - never: Never auto-hide
- foregroundWindowOnly: false
  $name: Foreground window only
- excludedPrograms: [""]
  $name: Excluded programs
- primaryMonitorOnly: false
  $name: Primary monitor only
- speedEnabled: false
  $name: Customize animation speed
  $description: Requires Windows animation effects to be enabled; otherwise the native taskbar has no visible animation to speed up.
- showSpeedup: 100
  $name: Show speed percent
- hideSpeedup: 100
  $name: Hide speed percent
- frameRate: 60
  $name: Frame rate for legacy taskbar
  $description: Does not affect the native Windows 11 animation.
- oldTaskbarOnWin11: false
  $name: Use legacy taskbar path
  $description: For ExplorerPatcher or a similar replacement taskbar.
*/
// ==/WindhawkModSettings==

#include <algorithm>
#include <windhawk_utils.h>
#include <dwmapi.h>
#include <psapi.h>
#include <winrt/base.h>
#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <winrt/Windows.UI.Composition.h>

namespace policy {
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
std::atomic<bool> g_initialized;
std::atomic<bool> g_explorerPatcherInitialized;

bool g_wasAutoHideProcessed;
bool g_wasAutoHideDisabled;
std::mutex g_winEventHookThreadMutex;
std::atomic<HANDLE> g_winEventHookThread;
std::unordered_map<void*, HWND> g_taskbarsKeptShown;
std::unordered_map<HWND, void*> g_taskbarToViewCoordinator;
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

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return false;
    }

    SendMessage(hTaskbarWnd, g_getTaskbarRectRegisteredMsg, (WPARAM)monitor,
                (LPARAM)rect);
    return true;
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

    // Don't keep the taskbar shown for a fullscreen window.
    if (EqualRect(&windowRect, &monitorInfo->rcMonitor)) {
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

    // Always show taskbar when MultitaskingView (Win+Tab) is active.
    if (g_multitaskingViewHwnd) {
        return true;
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

void AdjustAllTaskbars() {
    std::unordered_set<HWND> secondaryTaskbarWindows;
    HWND hWnd = FindTaskbarWindows(&secondaryTaskbarWindows);
    if (hWnd) {
        AdjustTaskbar(hWnd);
    }

    for (HWND hSecondaryWnd : secondaryTaskbarWindows) {
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

using ViewCoordinator_ShouldTaskbarBeExpanded_t =
    bool(WINAPI*)(void* pThis, HWND hMMTaskbarWnd, bool expanded);
ViewCoordinator_ShouldTaskbarBeExpanded_t
    ViewCoordinator_ShouldTaskbarBeExpanded_Original;
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

    return ViewCoordinator_ShouldTaskbarBeExpanded_Original(
        pThis, hMMTaskbarWnd, expanded);
}

using ViewCoordinator_UpdateIsExpanded_t = void(WINAPI*)(void* pThis,
                                                         HWND hMMTaskbarWnd,
                                                         int reason);
ViewCoordinator_UpdateIsExpanded_t ViewCoordinator_UpdateIsExpanded_Original;

void UpdateViewCoordinatorIsExpanded(HWND hWnd) {
    // From ViewCoordinator::HandleIsPointerOverTaskbarFrameChanged.
    constexpr int kReasonIsPointerOverTaskbarFrameChanged = 7;

    if (ViewCoordinator_UpdateIsExpanded_Original) {
        auto it = g_taskbarToViewCoordinator.find(hWnd);
        if (it != g_taskbarToViewCoordinator.end()) {
            ViewCoordinator_UpdateIsExpanded_Original(
                it->second, hWnd, kReasonIsPointerOverTaskbarFrameChanged);
        }
    }
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
void WINAPI TrayUI__Hide_Hook(void* pThis) {
    Wh_Log(L">");

    auto it = g_taskbarsKeptShown.find(pThis);
    if (it != g_taskbarsKeptShown.end()) {
        KillTimer(it->second, kTrayUITimerHide);
        return;
    }

    TrayUI__Hide_Original(pThis);
}

using CSecondaryTray__AutoHide_t = void(WINAPI*)(void* pThis, bool param1);
CSecondaryTray__AutoHide_t CSecondaryTray__AutoHide_Original;
void WINAPI CSecondaryTray__AutoHide_Hook(void* pThis, bool param1) {
    Wh_Log(L">");

    auto it = g_taskbarsKeptShown.find(pThis);
    if (it != g_taskbarsKeptShown.end()) {
        KillTimer(it->second, kTrayUITimerHide);
        return;
    }

    CSecondaryTray__AutoHide_Original(pThis, param1);
}

using TrayUI_Unhide_t = void(WINAPI*)(void* pThis,
                                      int trayUnhideFlags,
                                      int unhideRequest);
TrayUI_Unhide_t TrayUI_Unhide_Original;

using CSecondaryTray__Unhide_t = void(WINAPI*)(void* pThis,
                                               int trayUnhideFlags,
                                               int unhideRequest);
CSecondaryTray__Unhide_t CSecondaryTray__Unhide_Original;

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
        AdjustTaskbar(hWnd);
    } else if (Msg == WM_NCDESTROY) {
        Wh_Log(L"WM_NCDESTROY: %08X", (DWORD)(ULONG_PTR)hWnd);
        g_taskbarsKeptShown.erase(
            QueryViaVtableBackwards(pThis, TrayUI_vftable_IInspectable));
        g_taskbarToViewCoordinator.erase(hWnd);
    } else if (Msg == kHandleTrayPrivateSettingMessage) {
        // Prevent auto-hide from being disabled while the mod is loaded.
        if ((DWORD)wParam == 4) {
            BOOL bSetAutoHideEnabled = (BOOL)lParam;
            if (!bSetAutoHideEnabled) {
                return 0;
            }
        }
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
        if (!g_wasAutoHideProcessed) {
            g_wasAutoHideProcessed = true;
            g_wasAutoHideDisabled =
                !SendMessage(hWnd, kHandleTrayPrivateSettingMessage,
                             kTrayPrivateSettingAutoHideGet, 0);
            if (g_wasAutoHideDisabled) {
                SendMessage(hWnd, kHandleTrayPrivateSettingMessage,
                            kTrayPrivateSettingAutoHideSet, TRUE);
            }
        }

        HMONITOR monitor = TrayUI_GetStuckMonitor_Original(pThis);
        bool keepShown = ShouldKeepTaskbarShown(hWnd, monitor);

        void* pTrayUI_IInspectable =
            QueryViaVtableBackwards(pThis, TrayUI_vftable_IInspectable);

        bool keptShown = g_taskbarsKeptShown.contains(pTrayUI_IInspectable);

        if (keepShown != keptShown) {
            Wh_Log(L"> keepShown=%d", keepShown);

            if (keepShown) {
                g_taskbarsKeptShown[pTrayUI_IInspectable] = hWnd;

                void* pTrayUI_ITrayComponentHost =
                    QueryViaVtable(pThis, TrayUI_vftable_ITrayComponentHost);
                TrayUI_Unhide_Original(pTrayUI_ITrayComponentHost, 0, 0);

                UpdateViewCoordinatorIsExpanded(hWnd);
            } else {
                g_taskbarsKeptShown.erase(pTrayUI_IInspectable);

                SetTimer(hWnd, kTrayUITimerHide, 0, nullptr);

                UpdateViewCoordinatorIsExpanded(hWnd);
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
        AdjustTaskbar(hWnd);
    } else if (Msg == WM_NCDESTROY) {
        Wh_Log(L"WM_NCDESTROY: %08X", (DWORD)(ULONG_PTR)hWnd);
        g_taskbarsKeptShown.erase(pThis);
        g_taskbarToViewCoordinator.erase(hWnd);
    } else if (Msg == g_updateTaskbarStateRegisteredMsg) {
        void* pCSecondaryTray_ISecondaryTray =
            QueryViaVtable(pThis, CSecondaryTray_vftable_ISecondaryTray);

        HMONITOR monitor =
            CSecondaryTray_GetMonitor_Original(pCSecondaryTray_ISecondaryTray);

        bool keepShown = ShouldKeepTaskbarShown(hWnd, monitor);

        bool keptShown = g_taskbarsKeptShown.contains(pThis);

        if (keepShown != keptShown) {
            Wh_Log(L"> keepShown=%d", keepShown);

            if (keepShown) {
                g_taskbarsKeptShown[pThis] = hWnd;

                CSecondaryTray__Unhide_Original(pThis, 0, 0);

                UpdateViewCoordinatorIsExpanded(hWnd);
            } else {
                g_taskbarsKeptShown.erase(pThis);

                SetTimer(hWnd, kTrayUITimerHide, 0, nullptr);

                UpdateViewCoordinatorIsExpanded(hWnd);
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

            if (!AdjustAllTaskbarsIfNotPending()) {
                Wh_Log(L"Adjustment already pending, will retry later...");
                return;
            }

            KillTimer(nullptr, g_pendingEventsTimer);
            g_pendingEventsTimer = 0;
        });
}

DWORD WINAPI WinEventHookThread(LPVOID lpThreadParameter) {
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
    // Taskbar.View.dll
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
    if (g_winVersion >= WinVersion::Win11 && !g_taskbarViewDllLoaded &&
        GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
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
    PCWSTR mode = Wh_GetStringSetting(L"mode");
    g_settings.mode = Mode::intersected;
    if (wcscmp(mode, L"maximized") == 0) {
        g_settings.mode = Mode::maximized;
    } else if (wcscmp(mode, L"never") == 0) {
        g_settings.mode = Mode::never;
    }
    Wh_FreeStringSetting(mode);

    g_settings.foregroundWindowOnly = Wh_GetIntSetting(L"foregroundWindowOnly");

    g_settings.excludedPrograms.clear();

    for (int i = 0;; i++) {
        PCWSTR program = Wh_GetStringSetting(L"excludedPrograms[%d]", i);

        bool hasProgram = *program;
        if (hasProgram) {
            std::wstring programUpper = program;
            LCMapStringEx(
                LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, &programUpper[0],
                static_cast<int>(programUpper.length()), &programUpper[0],
                static_cast<int>(programUpper.length()), nullptr, nullptr, 0);

            g_settings.excludedPrograms.insert(std::move(programUpper));
        }

        Wh_FreeStringSetting(program);

        if (!hasProgram) {
            break;
        }
    }

    g_settings.primaryMonitorOnly = Wh_GetIntSetting(L"primaryMonitorOnly");
    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

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
            g_taskbarViewDllLoaded = true;
            if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
                return FALSE;
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
    // Registered once by the outer dispatcher.

    g_initialized = true;

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_winVersion >= WinVersion::Win11 && !g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");

                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
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

    if (g_wasAutoHideDisabled) {
        HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
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

}

namespace speed {
struct {
    int showSpeedup;
    int hideSpeedup;
    int frameRate;
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
std::atomic<bool> g_initialized;
std::atomic<bool> g_explorerPatcherInitialized;

double g_recipCyclesPerSecond;

std::atomic<DWORD> g_slideWindowThreadId;
int g_slideWindowSpeedup;
double g_slideWindowStartTime;
double g_slideWindowLastFrameStartTime;

winrt::Windows::UI::Composition::KeyFrameAnimation g_collapseAnimation =
    nullptr;
winrt::Windows::Foundation::TimeSpan g_collapseOriginalDuration;
winrt::Windows::UI::Composition::KeyFrameAnimation g_expandAnimation = nullptr;
winrt::Windows::Foundation::TimeSpan g_expandOriginalDuration;

void SetAnimationDuration(
    winrt::Windows::UI::Composition::KeyFrameAnimation animation,
    winrt::Windows::Foundation::TimeSpan duration) {
    // From the docs: Minimum allowed value is 1ms and maximum allowed value is
    // 24 days.
    constexpr auto kMin = duration_cast<winrt::Windows::Foundation::TimeSpan>(
        std::chrono::milliseconds{1});
    constexpr auto kMax = duration_cast<winrt::Windows::Foundation::TimeSpan>(
        std::chrono::hours{24 * 24});
    if (duration < kMin) {
        duration = kMin;
    } else if (duration > kMax) {
        duration = kMax;
    }
    animation.Duration(duration);
}

using SharedAnimations_TaskbarCollapseAnimation_t =
    void**(WINAPI*)(void* pThis, void* param1);
SharedAnimations_TaskbarCollapseAnimation_t
    SharedAnimations_TaskbarCollapseAnimation_Original;
void** WINAPI SharedAnimations_TaskbarCollapseAnimation_Hook(void* pThis,
                                                             void* param1) {
    Wh_Log(L">");

    void** ret =
        SharedAnimations_TaskbarCollapseAnimation_Original(pThis, param1);

    winrt::Windows::UI::Composition::KeyFrameAnimation animation = nullptr;
    winrt::copy_from_abi(animation, ret[0]);

    if (!g_collapseAnimation) {
        g_collapseOriginalDuration = animation.Duration();
        g_collapseAnimation = animation;
        SetAnimationDuration(animation, g_collapseOriginalDuration * 100 /
                                            g_settings.hideSpeedup);
    }

    return ret;
}

using SharedAnimations_TaskbarExpandAnimation_t = void**(WINAPI*)(void* pThis,
                                                                  void* param1);
SharedAnimations_TaskbarExpandAnimation_t
    SharedAnimations_TaskbarExpandAnimation_Original;
void** WINAPI SharedAnimations_TaskbarExpandAnimation_Hook(void* pThis,
                                                           void* param1) {
    Wh_Log(L">");

    void** ret =
        SharedAnimations_TaskbarExpandAnimation_Original(pThis, param1);

    winrt::Windows::UI::Composition::KeyFrameAnimation animation = nullptr;
    winrt::copy_from_abi(animation, ret[0]);

    if (!g_expandAnimation) {
        g_expandOriginalDuration = animation.Duration();
        g_expandAnimation = animation;
        SetAnimationDuration(
            animation, g_expandOriginalDuration * 100 / g_settings.showSpeedup);
    }

    return ret;
}

void TimerInitialize() {
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    double cyclesPerSecond = static_cast<double>(freq.QuadPart);
    g_recipCyclesPerSecond = 1.0 / cyclesPerSecond;
}

double TimerGetCycles() {
    LARGE_INTEGER T1;
    QueryPerformanceCounter(&T1);
    return static_cast<double>(T1.QuadPart);
}

double TimerGetSeconds() {
    return TimerGetCycles() * g_recipCyclesPerSecond;
}

using TrayUI_SlideWindow_t = void(WINAPI*)(void* pThis,
                                           HWND hWnd,
                                           const RECT* rect,
                                           HMONITOR monitor,
                                           bool show,
                                           bool animate);
TrayUI_SlideWindow_t TrayUI_SlideWindow_Original;
void WINAPI TrayUI_SlideWindow_Hook(void* pThis,
                                    HWND hWnd,
                                    const RECT* rect,
                                    HMONITOR monitor,
                                    bool show,
                                    bool animate) {
    Wh_Log(L">");

    g_slideWindowSpeedup =
        show ? g_settings.showSpeedup : g_settings.hideSpeedup;
    g_slideWindowStartTime = TimerGetSeconds();
    g_slideWindowLastFrameStartTime = g_slideWindowStartTime;
    g_slideWindowThreadId = GetCurrentThreadId();

    TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show, animate);

    g_slideWindowThreadId = 0;
}

using GetTickCount_t = decltype(&GetTickCount);
GetTickCount_t GetTickCount_Original;
DWORD WINAPI GetTickCount_Hook() {
    if (g_slideWindowThreadId != GetCurrentThreadId()) {
        return GetTickCount_Original();
    }

    DWORD ms = (g_slideWindowLastFrameStartTime - g_slideWindowStartTime) *
                   1000.0 * (g_slideWindowSpeedup / 100.0) +
               0.5;

    Wh_Log(L"> %u", ms);

    return ms;
}

using Sleep_t = decltype(&Sleep);
Sleep_t Sleep_Original;
void WINAPI Sleep_Hook(DWORD dwMilliseconds) {
    if (g_slideWindowThreadId != GetCurrentThreadId()) {
        Sleep_Original(dwMilliseconds);
        return;
    }

    double frameTotalTime = 1000.0 / g_settings.frameRate;

    double frameElapsedTime =
        (TimerGetSeconds() - g_slideWindowLastFrameStartTime) * 1000.0;

    int sleepTime = frameTotalTime - frameElapsedTime + 0.5;

    Wh_Log(L"> %f - %f = %d", frameTotalTime, frameElapsedTime, sleepTime);

    if (sleepTime > 0) {
        Sleep_Original(sleepTime);
    }

    g_slideWindowLastFrameStartTime = TimerGetSeconds();
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: struct std::pair<struct winrt::Windows::UI::Composition::KeyFrameAnimation,struct winrt::WindowsUdk::UI::Composition::AnimationLastFrameTelemetry> __cdecl winrt::Taskbar::implementation::SharedAnimations::TaskbarCollapseAnimation(void)const )"},
            &SharedAnimations_TaskbarCollapseAnimation_Original,
            SharedAnimations_TaskbarCollapseAnimation_Hook,
            true,  // For the new Windows 11 animation.
        },
        {
            {LR"(public: struct std::pair<struct winrt::Windows::UI::Composition::KeyFrameAnimation,struct winrt::WindowsUdk::UI::Composition::AnimationLastFrameTelemetry> __cdecl winrt::Taskbar::implementation::SharedAnimations::TaskbarExpandAnimation(void)const )"},
            &SharedAnimations_TaskbarExpandAnimation_Original,
            SharedAnimations_TaskbarExpandAnimation_Hook,
            true,  // For the new Windows 11 animation.
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
    if (g_winVersion >= WinVersion::Win11 && !g_taskbarViewDllLoaded &&
        GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
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
            {LR"(public: virtual void __cdecl TrayUI::SlideWindow(struct HWND__ *,struct tagRECT const *,struct HMONITOR__ *,bool,bool))"},
            &TrayUI_SlideWindow_Original,
            TrayUI_SlideWindow_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
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
        {R"(?SlideWindow@TrayUI@@UEAAXPEAUHWND__@@PEBUtagRECT@@PEAUHMONITOR__@@_N3@Z)",
         &TrayUI_SlideWindow_Original, TrayUI_SlideWindow_Hook},
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

void LoadSettings() {
    g_settings.showSpeedup = std::clamp(Wh_GetIntSetting(L"showSpeedup"), 1, 500);
    g_settings.hideSpeedup = std::clamp(Wh_GetIntSetting(L"hideSpeedup"), 1, 500);
    g_settings.frameRate = std::clamp(Wh_GetIntSetting(L"frameRate"), 30, 240);
    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

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
            g_taskbarViewDllLoaded = true;
            if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
                return FALSE;
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
    // Registered once by the outer dispatcher.

    auto pKernelBaseGetTickCount = (decltype(&GetTickCount))GetProcAddress(
        kernelBaseModule, "GetTickCount");
    WindhawkUtils::SetFunctionHook(
        pKernelBaseGetTickCount, GetTickCount_Hook, &GetTickCount_Original);

    auto pKernelBaseSleep =
        (decltype(&Sleep))GetProcAddress(kernelBaseModule, "Sleep");
    WindhawkUtils::SetFunctionHook(pKernelBaseSleep, Sleep_Hook,
                                       &Sleep_Original);

    TimerInitialize();

    g_initialized = true;

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_winVersion >= WinVersion::Win11 && !g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");

                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    // Try again in case there's a race between the previous attempt and the
    // LoadLibraryExW hook.
    if (!g_explorerPatcherInitialized) {
        HandleLoadedExplorerPatcher();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_collapseAnimation) {
        g_collapseAnimation.Duration(g_collapseOriginalDuration);
    }

    if (g_expandAnimation) {
        g_expandAnimation.Duration(g_expandOriginalDuration);
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    LoadSettings();

    if (g_collapseAnimation) {
        SetAnimationDuration(
            g_collapseAnimation,
            g_collapseOriginalDuration * 100 / g_settings.hideSpeedup);
    }

    if (g_expandAnimation) {
        SetAnimationDuration(g_expandAnimation, g_expandOriginalDuration * 100 /
                                                    g_settings.showSpeedup);
    }

    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11;

    return TRUE;
}

}


namespace {
bool policyStarted = false;
bool speedStarted = false;
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t loadLibraryOriginal = nullptr;

HMODULE WINAPI LoadLibraryExW_Dispatch(LPCWSTR path, HANDLE file, DWORD flags) {
    HMODULE module = loadLibraryOriginal(path, file, flags);
    if (!module) return module;
    if (policyStarted) {
        policy::HandleLoadedModuleIfExplorerPatcher(module);
        policy::HandleLoadedModuleIfTaskbarView(module, path);
    }
    if (speedStarted) {
        speed::HandleLoadedModuleIfExplorerPatcher(module);
        speed::HandleLoadedModuleIfTaskbarView(module, path);
    }
    return module;
}
}

BOOL Wh_ModInit() {
    PCWSTR mode = Wh_GetStringSetting(L"mode");
    bool usePolicy = wcscmp(mode, L"system") != 0;
    Wh_FreeStringSetting(mode);
    bool useSpeed = Wh_GetIntSetting(L"speedEnabled") != 0;

    if (usePolicy) {
        if (!policy::Wh_ModInit()) return FALSE;
        policyStarted = true;
    }
    if (useSpeed) {
        if (!speed::Wh_ModInit()) {
            if (policyStarted) policy::Wh_ModUninit();
            policyStarted = false;
            return FALSE;
        }
        speedStarted = true;
    }
    if (policyStarted || speedStarted) {
        HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
        if (!kernelBase) {
            if (speedStarted) speed::Wh_ModUninit();
            if (policyStarted) policy::Wh_ModUninit();
            return FALSE;
        }
        auto target = reinterpret_cast<LoadLibraryExW_t>(
            GetProcAddress(kernelBase, "LoadLibraryExW"));
        if (!target) {
            if (speedStarted) speed::Wh_ModUninit();
            if (policyStarted) policy::Wh_ModUninit();
            return FALSE;
        }
        if (!WindhawkUtils::SetFunctionHook(target, LoadLibraryExW_Dispatch,
                                            &loadLibraryOriginal)) {
            if (speedStarted) speed::Wh_ModUninit();
            if (policyStarted) policy::Wh_ModUninit();
            speedStarted = policyStarted = false;
            return FALSE;
        }
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    if (policyStarted) policy::Wh_ModAfterInit();
    if (speedStarted) speed::Wh_ModAfterInit();
}

void Wh_ModUninit() {
    if (speedStarted) speed::Wh_ModUninit();
    if (policyStarted) policy::Wh_ModUninit();
    speedStarted = false;
    policyStarted = false;
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;
    return TRUE;
}
