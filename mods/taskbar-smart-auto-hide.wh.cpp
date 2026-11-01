// ==WindhawkMod==
// @id              taskbar-smart-auto-hide
// @name            Taskbar Smart Auto-Hide
// @description     One mod for taskbar auto-hide: hide when maximized/intersected/always, keep shown on the desktop, fast or instant reveal, notification handling, and a reliable edge reveal over apps like ChatGPT
// @version         1.1
// @author          Josh
// @github          https://github.com/jallison1193-crypto
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lversion
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// This mod combines and modifies code from:
//
// - "Taskbar auto-hide when maximized" by m417z, GPL-3.0. This mod is a fork
//   of it, and most of the window detection and taskbar hooking code is his.
//   https://github.com/m417z/my-windhawk-mods
//
// - "Taskbar Auto-Hide Instant Show" by Bo0ii, Apache License 2.0. The
//   animation, easing and timer code was adapted (renamed, restructured and
//   merged with the settings of this mod).
//   https://github.com/Bo0ii/windhawk-mods
//   https://www.apache.org/licenses/LICENSE-2.0
//
// - "Better Taskbar Autohide" by Cirn09, MIT (the default license of the
//   Windhawk mods repository). The notification flash hooks were adapted: the
//   shell hook message ID is now looked up instead of hardcoded, and other
//   reasons for blocking auto-hide are respected.
//   https://github.com/ramensoftware/windhawk-mods
//
//   Copyright (c) Cirn09
//
//   Permission is hereby granted, free of charge, to any person obtaining a
//   copy of this software and associated documentation files (the
//   "Software"), to deal in the Software without restriction, including
//   without limitation the rights to use, copy, modify, merge, publish,
//   distribute, sublicense, and/or sell copies of the Software, and to permit
//   persons to whom the Software is furnished to do so, subject to the
//   following conditions:
//
//   The above copyright notice and this permission notice shall be included
//   in all copies or substantial portions of the Software.
//
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
//   NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
//   DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
//   OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
//   USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// New in this mod: the "always" mode, keeping the taskbar shown on the
// desktop, keeping it on top of maximized apps that look fullscreen (such as
// ChatGPT), and the edge reveal watcher.

// ==WindhawkModReadme==
/*
# Taskbar Smart Auto-Hide

Combines three auto-hide mods into one and adds a few things:

* **When to hide:** only when a window is maximized, when a window is maximized
  or touches the taskbar, always (like the Windows setting), or never.
* **Keep shown on the desktop:** keep the taskbar visible while the desktop is
  focused (Win+D or clicking the desktop) and on monitors with no open windows.
* **Reveal speed:** slide at a custom speed, or use instant, fade, elastic,
  bounce, slide + fade or overshoot animations. Separate show and hide delays.
* **Notifications:** a flashing taskbar button no longer pins the taskbar open
  forever; it re-hides after a delay you choose.
* **Maximized apps that look fullscreen:** some apps (for example ChatGPT)
  size their maximized window to exactly fill the screen. Windows mistakes
  them for fullscreen, drops the taskbar out of the always-on-top layer, and
  reveals it behind the app, so you only see a resize cursor. This mod keeps
  the taskbar on top while the active window is maximized.
* **Edge reveal fix:** as a backup, the mod watches the cursor and, when it
  touches the taskbar's edge while another window covers it, reveals the
  taskbar directly (once per visit to the edge).

Disable "Taskbar auto-hide when maximized", "Taskbar Auto-Hide Instant Show"
and "Better Taskbar Autohide" while using this mod, since they hook the same
taskbar functions.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- mode: intersected
  $name: When to auto-hide
  $options:
  - intersected: When a window is maximized or touches the taskbar
  - maximized: Only when a window is maximized
  - always: Always (like the Windows auto-hide setting)
  - never: Never (taskbar always visible)
- foregroundWindowOnly: false
  $name: Only consider the foreground window
  $description: >-
    Decide based on the active window only, instead of any window on the
    monitor. Applies to the "maximized" and "touches the taskbar" modes.
- keepShownOnDesktop: true
  $name: Keep shown while the desktop is focused
  $description: >-
    Keep the taskbar visible after Win+D or clicking the desktop.
- keepShownOnEmptyDesktop: true
  $name: Keep shown on an empty desktop
  $description: >-
    Keep the taskbar visible on a monitor with no open windows. The
    "maximized" and "touches the taskbar" modes already behave this way; this
    setting matters for "Always" mode.
- excludedPrograms: [""]
  $name: Excluded programs
  $description: >-
    The taskbar won't auto-hide because of windows of these programs.

    Entries can be process names, paths or application IDs, for example:

    mspaint.exe

    C:\Windows\System32\notepad.exe

    Microsoft.WindowsCalculator_8wekyb3d8bbwe!App
- primaryMonitorOnly: false
  $name: Primary monitor only
  $description: >-
    Apply the "when to auto-hide" rules only to the primary monitor's taskbar.
    Other monitors use the default Windows auto-hide behavior.
- animationType: slide
  $name: Animation type
  $description: How the taskbar appears and disappears
  $options:
  - none: Instant (no animation)
  - slide: Slide (Windows animation, with speed control)
  - elastic: Elastic (spring overshoot)
  - bounce: Bounce
  - fade: Fade
  - slideFade: Slide + fade
  - overshoot: Overshoot (subtle spring)
- showSpeedup: 400
  $name: Show speed % (Slide only)
  $description: >-
    100 = normal Windows speed, 200 = 2x faster, 400 = 4x faster.
- hideSpeedup: 400
  $name: Hide speed % (Slide only)
  $description: >-
    100 = normal Windows speed, 200 = 2x faster, 400 = 4x faster.
- showDuration: 200
  $name: Show duration in ms (other animations)
  $description: >-
    Length of the show animation for Elastic, Bounce, Fade, Slide + fade and
    Overshoot. Lower is faster.
- hideDuration: 200
  $name: Hide duration in ms (other animations)
  $description: >-
    Length of the hide animation for Elastic, Bounce, Fade, Slide + fade and
    Overshoot. Lower is faster.
- frameRate: 60
  $name: Animation frame rate
  $description: >-
    Target frame rate for animations. Use your monitor's refresh rate.
- unhideDelay: 1
  $name: Show delay in ms
  $description: >-
    How long the cursor must rest at the screen edge before the taskbar
    appears. Set to 1 for instant, or -1 to keep the Windows default.
- hideDelay: 1
  $name: Hide delay in ms
  $description: >-
    How long the taskbar waits after the cursor leaves before hiding. Set to
    1 for instant, or -1 to keep the Windows default.
- keepOnTopOfMaximized: true
  $name: Keep the taskbar on top of maximized apps
  $description: >-
    Some apps (ChatGPT and other frameless apps) size their maximized window
    to exactly fill the screen, so Windows mistakes them for fullscreen and
    reveals the taskbar behind them. This keeps the taskbar on top whenever
    the active window is maximized. True fullscreen apps aren't affected.
- edgeReveal: true
  $name: Edge reveal fix
  $description: >-
    Reveal the taskbar when the cursor touches its screen edge, even if a
    maximized app such as ChatGPT covers the edge and shows a resize cursor
    instead. Also works on monitors with no windows. Only steps in when
    another window is covering the edge.
- edgeRevealOverFullscreen: false
  $name: Edge reveal over fullscreen apps
  $description: >-
    Also force the taskbar over true fullscreen apps (videos, games and other
    borderless fullscreen windows). Maximized apps are always covered by the
    edge reveal fix.
- flashingHideDelay: 1500
  $name: Re-hide after a notification flash (ms)
  $description: >-
    When an app flashes its taskbar button, Windows keeps the taskbar shown
    until you look at it. This lets the taskbar hide again after this many
    milliseconds. Set to -1 to keep the Windows behavior.
- oldTaskbarOnWin11: false
  $name: Customize the old taskbar on Windows 11
  $description: >-
    Enable this option to customize the old taskbar on Windows 11 (if using
    ExplorerPatcher or a similar tool).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <math.h>
#include <psapi.h>

#include <winrt/base.h>

#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

enum class Mode {
    intersected,
    maximized,
    always,
    never,
};

enum class AnimationType {
    none,
    slide,
    elastic,
    bounce,
    fade,
    slideFade,
    overshoot,
};

struct {
    Mode mode;
    bool foregroundWindowOnly;
    bool keepShownOnDesktop;
    bool keepShownOnEmptyDesktop;
    std::unordered_set<std::wstring> excludedPrograms;
    bool primaryMonitorOnly;
    AnimationType animationType;
    int showSpeedup;
    int hideSpeedup;
    int showDuration;
    int hideDuration;
    int frameRate;
    int unhideDelay;
    int hideDelay;
    bool keepOnTopOfMaximized;
    bool edgeReveal;
    bool edgeRevealOverFullscreen;
    int flashingHideDelay;
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

// Posted by the edge reveal thread to a taskbar window. Handled on the taskbar
// thread, where it's safe to call the taskbar's own unhide function.
static const UINT g_edgeRevealRegisteredMsg =
    RegisterWindowMessage(L"Windhawk_EdgeReveal_" WH_MOD_ID);

// The message ID is assigned per session, so look it up instead of hardcoding.
static const UINT g_shellHookMsg = RegisterWindowMessage(L"SHELLHOOK");

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

bool IsDesktopWindowHwnd(HWND hWnd) {
    if (!hWnd) {
        return false;
    }

    if (hWnd == GetShellWindow()) {
        return true;
    }

    DWORD dwProcessId = 0;
    if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
        dwProcessId != GetCurrentProcessId()) {
        return false;
    }

    WCHAR className[32];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }

    return _wcsicmp(className, L"Progman") == 0 ||
           _wcsicmp(className, L"WorkerW") == 0;
}

// Whether the monitor has at least one regular, visible app window.
bool HasAppWindowOnMonitor(HMONITOR monitor) {
    bool found = false;

    DWORD dwTaskbarThreadId = GetCurrentThreadId();

    auto enumWindowsProc = [&](HWND hWnd) -> BOOL {
        if (GetWindowThreadProcessId(hWnd, nullptr) == dwTaskbarThreadId) {
            return TRUE;
        }

        if (!IsWindowVisible(hWnd) || IsWindowCloaked(hWnd) || IsIconic(hWnd)) {
            return TRUE;
        }

        if (GetWindowLong(hWnd, GWL_EXSTYLE) &
            (WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW)) {
            return TRUE;
        }

        if (hWnd == g_altTabViewHwnd || IsDesktopWindowHwnd(hWnd) ||
            GetProp(hWnd, L"DesktopWindow") ||
            GetClassWord(hWnd, GCW_ATOM) == 32768) {
            return TRUE;
        }

        RECT rect{};
        if (!GetWindowRect(hWnd, &rect) || IsRectEmpty(&rect)) {
            return TRUE;
        }

        if (MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST) != monitor) {
            return TRUE;
        }

        if (IsWindowExcluded(hWnd)) {
            return TRUE;
        }

        found = true;
        return FALSE;
    };

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& proc = *reinterpret_cast<decltype(enumWindowsProc)*>(lParam);
            return proc(hWnd);
        },
        reinterpret_cast<LPARAM>(&enumWindowsProc));

    return found;
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

    if (g_settings.keepShownOnDesktop &&
        IsDesktopWindowHwnd(GetForegroundWindow())) {
        return true;
    }

    if (g_settings.mode == Mode::always) {
        return g_settings.keepShownOnEmptyDesktop &&
               !HasAppWindowOnMonitor(monitor);
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
    } else if (Msg == g_edgeRevealRegisteredMsg) {
        Wh_Log(L"Edge reveal: %08X", (DWORD)(ULONG_PTR)hWnd);

        void* pTrayUI_ITrayComponentHost =
            QueryViaVtable(pThis, TrayUI_vftable_ITrayComponentHost);
        TrayUI_Unhide_Original(pTrayUI_ITrayComponentHost, 0, 0);

        // A maximized window that covers the whole monitor can make Explorer
        // drop the taskbar out of the topmost band. Put it back on top so it
        // isn't revealed behind that window.
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

        UpdateViewCoordinatorIsExpanded(hWnd);
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
    } else if (Msg == g_edgeRevealRegisteredMsg) {
        Wh_Log(L"Edge reveal: %08X", (DWORD)(ULONG_PTR)hWnd);

        CSecondaryTray__Unhide_Original(pThis, 0, 0);

        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

        UpdateViewCoordinatorIsExpanded(hWnd);
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
    if (g_settings.foregroundWindowOnly || g_settings.keepShownOnDesktop) {
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

////////////////////////////////////////////////////////////////////////////////
// Show/hide animations and delays.
// Adapted from "Taskbar Auto-Hide Instant Show" by Bo0ii.

double g_recipCyclesPerSecond;

std::atomic<DWORD> g_slideWindowThreadId;
int g_slideWindowSpeedup;
double g_slideWindowStartTime;
double g_slideWindowLastFrameStartTime;
std::atomic<int> g_animationGeneration{0};

void TimerInitialize() {
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    g_recipCyclesPerSecond = 1.0 / static_cast<double>(freq.QuadPart);
}

double TimerGetSeconds() {
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return static_cast<double>(counter.QuadPart) * g_recipCyclesPerSecond;
}

double EaseOutElastic(double t) {
    if (t <= 0.0) {
        return 0.0;
    }
    if (t >= 1.0) {
        return 1.0;
    }
    double p = 0.3;
    double s = p / 4.0;
    return pow(2.0, -10.0 * t) * sin((t - s) * (2.0 * M_PI) / p) + 1.0;
}

double EaseOutBounce(double t) {
    if (t < 1.0 / 2.75) {
        return 7.5625 * t * t;
    } else if (t < 2.0 / 2.75) {
        t -= 1.5 / 2.75;
        return 7.5625 * t * t + 0.75;
    } else if (t < 2.5 / 2.75) {
        t -= 2.25 / 2.75;
        return 7.5625 * t * t + 0.9375;
    } else {
        t -= 2.625 / 2.75;
        return 7.5625 * t * t + 0.984375;
    }
}

double EaseOutCubic(double t) {
    double t1 = t - 1.0;
    return t1 * t1 * t1 + 1.0;
}

double EaseInCubic(double t) {
    return t * t * t;
}

double EaseOutQuint(double t) {
    double t1 = 1.0 - t;
    return 1.0 - t1 * t1 * t1 * t1 * t1;
}

double EaseInQuint(double t) {
    return t * t * t * t * t;
}

double EaseOutBack(double t) {
    const double c1 = 1.70158;
    const double c3 = c1 + 1.0;
    double t1 = t - 1.0;
    return 1.0 + c3 * t1 * t1 * t1 + c1 * t1 * t1;
}

double EaseInBack(double t) {
    const double c1 = 1.70158;
    const double c3 = c1 + 1.0;
    return c3 * t * t * t - c1 * t * t;
}

// The Windows slide animation times itself with GetTickCount and paces frames
// with Sleep. While it runs, report scaled time to speed it up.
using GetTickCount_t = decltype(&GetTickCount);
GetTickCount_t GetTickCount_Original;
DWORD WINAPI GetTickCount_Hook() {
    if (g_slideWindowThreadId != GetCurrentThreadId()) {
        return GetTickCount_Original();
    }

    return (DWORD)((g_slideWindowLastFrameStartTime - g_slideWindowStartTime) *
                       1000.0 * (g_slideWindowSpeedup / 100.0) +
                   0.5);
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

    int sleepTime = (int)(frameTotalTime - frameElapsedTime + 0.5);
    if (sleepTime > 0) {
        Sleep_Original(sleepTime);
    }

    g_slideWindowLastFrameStartTime = TimerGetSeconds();
}

int Lerp(int a, int b, double t) {
    return a + (int)((b - a) * t);
}

void DoCustomAnimation(HWND hWnd,
                       const RECT* startRect,
                       const RECT* endRect,
                       bool show,
                       int myAnimGen) {
    AnimationType animType = g_settings.animationType;
    int duration = show ? g_settings.showDuration : g_settings.hideDuration;

    if (duration <= 0) {
        SetWindowPos(hWnd, nullptr, endRect->left, endRect->top,
                     endRect->right - endRect->left,
                     endRect->bottom - endRect->top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
        return;
    }

    double frameDuration = 1000.0 / g_settings.frameRate;
    int totalFrames = (int)(duration / frameDuration);
    if (totalFrames < 1) {
        totalFrames = 1;
    }

    bool usePosition = animType == AnimationType::elastic ||
                       animType == AnimationType::bounce ||
                       animType == AnimationType::slideFade ||
                       animType == AnimationType::overshoot;
    bool useFade = animType == AnimationType::fade ||
                   animType == AnimationType::slideFade;

    LONG_PTR originalExStyle = 0;
    if (useFade) {
        originalExStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
        if (!(originalExStyle & WS_EX_LAYERED)) {
            SetWindowLongPtr(hWnd, GWL_EXSTYLE,
                             originalExStyle | WS_EX_LAYERED);
        }
        SetLayeredWindowAttributes(hWnd, 0, show ? 0 : 255, LWA_ALPHA);
        DwmFlush();
    }

    if (useFade && !usePosition && show) {
        SetWindowPos(hWnd, nullptr, endRect->left, endRect->top,
                     endRect->right - endRect->left,
                     endRect->bottom - endRect->top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }

    double startTime = TimerGetSeconds();

    for (int i = 1; i <= totalFrames; i++) {
        if (g_animationGeneration.load() != myAnimGen) {
            break;
        }

        double t = (double)i / totalFrames;
        double posT = t;
        double alphaT = t;

        switch (animType) {
            case AnimationType::elastic:
                posT = show ? EaseOutElastic(t) : EaseInCubic(t);
                break;
            case AnimationType::bounce:
                posT = show ? EaseOutBounce(t) : EaseInCubic(t);
                break;
            case AnimationType::fade:
                alphaT = show ? EaseOutCubic(t) : EaseInCubic(t);
                break;
            case AnimationType::slideFade:
                posT = show ? EaseOutQuint(t) : EaseInQuint(t);
                alphaT = posT;
                break;
            case AnimationType::overshoot:
                posT = show ? EaseOutBack(t) : EaseInBack(t);
                break;
            default:
                break;
        }

        if (usePosition) {
            int x = Lerp(startRect->left, endRect->left, posT);
            int y = Lerp(startRect->top, endRect->top, posT);
            int w = Lerp(startRect->right - startRect->left,
                         endRect->right - endRect->left, posT);
            int h = Lerp(startRect->bottom - startRect->top,
                         endRect->bottom - endRect->top, posT);
            SetWindowPos(hWnd, nullptr, x, y, w, h,
                         SWP_NOZORDER | SWP_NOACTIVATE);
        }

        if (useFade) {
            BYTE alpha = (BYTE)(255.0 * (show ? alphaT : 1.0 - alphaT));
            if (alpha < 1) {
                alpha = 1;
            }
            SetLayeredWindowAttributes(hWnd, 0, alpha, LWA_ALPHA);
            DwmFlush();
        } else {
            double targetTime = startTime + (double)i * frameDuration / 1000.0;
            double sleepMs = (targetTime - TimerGetSeconds()) * 1000.0;
            if (sleepMs > 1.0) {
                Sleep_Original((DWORD)(sleepMs + 0.5));
            }
        }
    }

    if (usePosition) {
        SetWindowPos(hWnd, nullptr, endRect->left, endRect->top,
                     endRect->right - endRect->left,
                     endRect->bottom - endRect->top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }

    if (useFade) {
        if (!show) {
            SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
            DwmFlush();
            SetWindowPos(hWnd, nullptr, endRect->left, endRect->top,
                         endRect->right - endRect->left,
                         endRect->bottom - endRect->top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            DwmFlush();
        }

        // Remove WS_EX_LAYERED while the window is either off-screen (hide)
        // or at full alpha (show), so there's no flash.
        if (!(originalExStyle & WS_EX_LAYERED)) {
            SetWindowLongPtr(hWnd, GWL_EXSTYLE, originalExStyle);
            DwmFlush();
        } else if (show) {
            SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
        }
    }
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
    Wh_Log(L"> show=%d, animate=%d", show, animate);

    switch (g_settings.animationType) {
        case AnimationType::none:
            TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show,
                                        false);
            return;

        case AnimationType::slide:
            g_slideWindowSpeedup =
                show ? g_settings.showSpeedup : g_settings.hideSpeedup;
            if (g_slideWindowSpeedup <= 0) {
                g_slideWindowSpeedup = 100;
            }
            g_slideWindowStartTime = TimerGetSeconds();
            g_slideWindowLastFrameStartTime = g_slideWindowStartTime;
            g_slideWindowThreadId = GetCurrentThreadId();

            TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show,
                                        animate);

            g_slideWindowThreadId = 0;
            return;

        default:
            break;
    }

    int animGen = ++g_animationGeneration;

    RECT startRect;
    GetWindowRect(hWnd, &startRect);

    if (show) {
        // Let the original update its internal state invisibly, then move the
        // window back to the hidden position for the custom animation.
        LONG_PTR exStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
        bool addedLayered = !(exStyle & WS_EX_LAYERED);
        if (addedLayered) {
            SetWindowLongPtr(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
        }
        SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
        DwmFlush();

        TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show, false);

        SetWindowPos(hWnd, nullptr, startRect.left, startRect.top,
                     startRect.right - startRect.left,
                     startRect.bottom - startRect.top,
                     SWP_NOZORDER | SWP_NOACTIVATE);

        if (addedLayered) {
            SetWindowLongPtr(hWnd, GWL_EXSTYLE, exStyle);
        } else {
            SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
        }
        DwmFlush();

        DoCustomAnimation(hWnd, &startRect, rect, show, animGen);
    } else {
        DoCustomAnimation(hWnd, &startRect, rect, show, animGen);

        TrayUI_SlideWindow_Original(pThis, hWnd, rect, monitor, show, false);
    }
}

using SetTimer_t = decltype(&SetTimer);
SetTimer_t SetTimer_Original;
UINT_PTR WINAPI SetTimer_Hook(HWND hWnd,
                              UINT_PTR nIDEvent,
                              UINT uElapse,
                              TIMERPROC lpTimerFunc) {
    // A zero elapse time is an immediate request (for example, from the mode
    // logic above), so leave it alone.
    if (hWnd && uElapse > 0 &&
        (nIDEvent == kTrayUITimerHide || nIDEvent == kTrayUITimerUnhide) &&
        IsTaskbarWindow(hWnd)) {
        int delay = nIDEvent == kTrayUITimerUnhide ? g_settings.unhideDelay
                                                   : g_settings.hideDelay;
        if (delay >= 0) {
            uElapse = delay;
        }
    }

    return SetTimer_Original(hWnd, nIDEvent, uElapse, lpTimerFunc);
}

////////////////////////////////////////////////////////////////////////////////
// Re-hide after a notification flash.
// Adapted from "Better Taskbar Autohide" by Cirn09.

std::atomic<ULONGLONG> g_lastFlashTickCount;

using CTaskBand_v_WndProc_t = LRESULT(
    WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
CTaskBand_v_WndProc_t CTaskBand_v_WndProc_Original;
LRESULT WINAPI CTaskBand_v_WndProc_Hook(void* pThis,
                                        HWND hWnd,
                                        UINT Msg,
                                        WPARAM wParam,
                                        LPARAM lParam) {
    if (Msg == g_shellHookMsg && wParam == HSHELL_FLASH) {
        g_lastFlashTickCount = GetTickCount64();
    }

    return CTaskBand_v_WndProc_Original(pThis, hWnd, Msg, wParam, lParam);
}

using CTaskListWnd_PermitAutoHide_t = BOOL(WINAPI*)(void* pThis);
CTaskListWnd_PermitAutoHide_t CTaskListWnd_PermitAutoHide_Original;
BOOL WINAPI CTaskListWnd_PermitAutoHide_Hook(void* pThis) {
    BOOL permit = CTaskListWnd_PermitAutoHide_Original(pThis);
    if (permit || g_settings.flashingHideDelay < 0) {
        return permit;
    }

    // Only override when a flash is the likely reason auto-hide is blocked,
    // so other reasons (such as an open jump list) are still respected.
    ULONGLONG lastFlashTickCount = g_lastFlashTickCount;
    if (lastFlashTickCount &&
        GetTickCount64() - lastFlashTickCount >=
            (ULONGLONG)g_settings.flashingHideDelay) {
        return TRUE;
    }

    return permit;
}

////////////////////////////////////////////////////////////////////////////////
// Keep the taskbar on top of maximized apps that look fullscreen.
//
// Explorer moves the taskbar out of the topmost band while a fullscreen window
// is in the foreground, so videos and games aren't covered. Some apps (ChatGPT
// and other frameless Electron apps) size their maximized window to exactly the
// monitor, so Explorer mistakes them for fullscreen and the taskbar ends up
// revealed behind them. A maximized window is never really fullscreen, so when
// the foreground window is maximized, keep the taskbar topmost.

bool IsMaximizedForegroundOnMonitor(HMONITOR monitor) {
    HWND hForegroundWnd = GetForegroundWindow();
    return hForegroundWnd && IsZoomed(hForegroundWnd) &&
           MonitorFromWindow(hForegroundWnd, MONITOR_DEFAULTTONEAREST) ==
               monitor;
}

bool ShouldKeepTaskbarTopmost(HWND hWnd, HWND hWndInsertAfter, UINT uFlags) {
    if (!g_settings.keepOnTopOfMaximized || (uFlags & SWP_NOZORDER) ||
        hWndInsertAfter == HWND_TOPMOST || hWndInsertAfter == HWND_TOP) {
        return false;
    }

    // Placing the taskbar after another topmost window keeps it topmost.
    if (hWndInsertAfter != HWND_NOTOPMOST && hWndInsertAfter != HWND_BOTTOM &&
        (GetWindowLong(hWndInsertAfter, GWL_EXSTYLE) & WS_EX_TOPMOST)) {
        return false;
    }

    if (!hWnd || !IsTaskbarWindow(hWnd)) {
        return false;
    }

    return IsMaximizedForegroundOnMonitor(
        MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST));
}

using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Original;
BOOL WINAPI SetWindowPos_Hook(HWND hWnd,
                              HWND hWndInsertAfter,
                              int X,
                              int Y,
                              int cx,
                              int cy,
                              UINT uFlags) {
    if (ShouldKeepTaskbarTopmost(hWnd, hWndInsertAfter, uFlags)) {
        Wh_Log(L"Keeping taskbar %08X topmost (SetWindowPos)",
               (DWORD)(ULONG_PTR)hWnd);
        hWndInsertAfter = HWND_TOPMOST;
    }

    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

using DeferWindowPos_t = decltype(&DeferWindowPos);
DeferWindowPos_t DeferWindowPos_Original;
HDWP WINAPI DeferWindowPos_Hook(HDWP hWinPosInfo,
                                HWND hWnd,
                                HWND hWndInsertAfter,
                                int x,
                                int y,
                                int cx,
                                int cy,
                                UINT uFlags) {
    if (ShouldKeepTaskbarTopmost(hWnd, hWndInsertAfter, uFlags)) {
        Wh_Log(L"Keeping taskbar %08X topmost (DeferWindowPos)",
               (DWORD)(ULONG_PTR)hWnd);
        hWndInsertAfter = HWND_TOPMOST;
    }

    return DeferWindowPos_Original(hWinPosInfo, hWnd, hWndInsertAfter, x, y, cx,
                                   cy, uFlags);
}

////////////////////////////////////////////////////////////////////////////////
// Edge reveal.
//
// Windows reveals an auto-hidden taskbar only when its thin strip at the
// screen edge gets the cursor. Some maximized apps (ChatGPT, for example)
// cover that strip, so the cursor lands on the app's resize border instead and
// Explorer's own check fails. This thread watches the cursor and, when it rests
// on the taskbar's edge while another window covers it, asks the taskbar to
// reveal itself directly and raises it above that window.

HANDLE g_edgeRevealThread;
HANDLE g_edgeRevealStopEvent;

enum class ScreenEdge {
    none,
    left,
    top,
    right,
    bottom,
};

// Returns the monitor edge a hidden taskbar is tucked behind, or none if the
// taskbar is shown or belongs to another monitor.
ScreenEdge GetHiddenTaskbarEdge(const RECT& taskbarRect,
                                const RECT& monitorRect) {
    int width = taskbarRect.right - taskbarRect.left;
    int height = taskbarRect.bottom - taskbarRect.top;

    if (width >= height) {
        if (taskbarRect.top >= monitorRect.bottom - height / 2 &&
            taskbarRect.top <= monitorRect.bottom + height / 2) {
            return ScreenEdge::bottom;
        }
        if (taskbarRect.bottom <= monitorRect.top + height / 2 &&
            taskbarRect.bottom >= monitorRect.top - height / 2) {
            return ScreenEdge::top;
        }
    } else {
        if (taskbarRect.left >= monitorRect.right - width / 2 &&
            taskbarRect.left <= monitorRect.right + width / 2) {
            return ScreenEdge::right;
        }
        if (taskbarRect.right <= monitorRect.left + width / 2 &&
            taskbarRect.right >= monitorRect.left - width / 2) {
            return ScreenEdge::left;
        }
    }

    return ScreenEdge::none;
}

bool IsCursorOnEdge(POINT pt,
                    ScreenEdge edge,
                    const RECT& taskbarRect,
                    const RECT& monitorRect) {
    switch (edge) {
        case ScreenEdge::bottom:
            return pt.y >= monitorRect.bottom - 1 && pt.x >= taskbarRect.left &&
                   pt.x < taskbarRect.right;
        case ScreenEdge::top:
            return pt.y <= monitorRect.top && pt.x >= taskbarRect.left &&
                   pt.x < taskbarRect.right;
        case ScreenEdge::right:
            return pt.x >= monitorRect.right - 1 && pt.y >= taskbarRect.top &&
                   pt.y < taskbarRect.bottom;
        case ScreenEdge::left:
            return pt.x <= monitorRect.left && pt.y >= taskbarRect.top &&
                   pt.y < taskbarRect.bottom;
        default:
            return false;
    }
}

// Videos, games and other borderless fullscreen windows. Maximized windows and
// resizable windows (ChatGPT is both) don't count.
bool IsTrueFullscreenWindow(HWND hWnd, const RECT& monitorRect) {
    if (!hWnd || IsDesktopWindowHwnd(hWnd) || IsTaskbarWindow(hWnd)) {
        return false;
    }

    if (IsZoomed(hWnd) || (GetWindowLong(hWnd, GWL_STYLE) & WS_THICKFRAME)) {
        return false;
    }

    RECT rect;
    if (!GetWindowRect(hWnd, &rect)) {
        return false;
    }

    return rect.left <= monitorRect.left && rect.top <= monitorRect.top &&
           rect.right >= monitorRect.right && rect.bottom >= monitorRect.bottom;
}

bool IsAnyMouseButtonDown() {
    return (GetAsyncKeyState(VK_LBUTTON) | GetAsyncKeyState(VK_RBUTTON) |
            GetAsyncKeyState(VK_MBUTTON)) &
           0x8000;
}

// Returns the hidden taskbar whose edge the cursor is on, if another window is
// covering it. Returns nullptr if Windows can handle the reveal by itself.
HWND FindCoveredTaskbarAtCursor(POINT pt,
                                const RECT& monitorRect,
                                const std::vector<HWND>& taskbars) {
    for (HWND hTaskbarWnd : taskbars) {
        RECT taskbarRect;
        if (!IsWindow(hTaskbarWnd) ||
            !GetWindowRect(hTaskbarWnd, &taskbarRect)) {
            continue;
        }

        ScreenEdge edge = GetHiddenTaskbarEdge(taskbarRect, monitorRect);
        if (edge == ScreenEdge::none ||
            !IsCursorOnEdge(pt, edge, taskbarRect, monitorRect)) {
            continue;
        }

        HWND hPointWnd = WindowFromPoint(pt);
        HWND hRootWnd = hPointWnd ? GetAncestor(hPointWnd, GA_ROOT) : nullptr;
        if (hRootWnd == hTaskbarWnd) {
            // The taskbar strip has the cursor, Windows will reveal it.
            return nullptr;
        }

        if (!g_settings.edgeRevealOverFullscreen &&
            IsTrueFullscreenWindow(hRootWnd, monitorRect)) {
            return nullptr;
        }

        return hTaskbarWnd;
    }

    return nullptr;
}

DWORD WINAPI EdgeRevealThread(LPVOID lpThreadParameter) {
    constexpr DWORD kPollInterval = 10;
    constexpr DWORD kRetryInterval = 250;
    constexpr int kMaxRevealAttempts = 3;
    constexpr DWORD kTaskbarListRefreshInterval = 2000;
    // Used when the show delay is set to the Windows default.
    constexpr DWORD kDefaultRevealDelay = 50;

    std::vector<HWND> taskbars;
    DWORD taskbarsRefreshTime = 0;
    bool taskbarsLoaded = false;

    // State for the current visit of the cursor to a screen edge. Reveal at
    // most once per visit, so a taskbar that Explorer hides again doesn't keep
    // bouncing while the cursor rests on the edge.
    HWND hEdgeTaskbarWnd = nullptr;
    DWORD edgeEnterTime = 0;
    DWORD lastRevealTime = 0;
    int revealAttempts = 0;
    bool visitDone = false;

    auto resetVisit = [&]() {
        hEdgeTaskbarWnd = nullptr;
        revealAttempts = 0;
        visitDone = false;
    };

    while (WaitForSingleObject(g_edgeRevealStopEvent, kPollInterval) ==
           WAIT_TIMEOUT) {
        POINT pt;
        if (!GetCursorPos(&pt) || IsAnyMouseButtonDown()) {
            resetVisit();
            continue;
        }

        HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONULL);
        MONITORINFO monitorInfo{
            .cbSize = sizeof(MONITORINFO),
        };
        if (!monitor || !GetMonitorInfo(monitor, &monitorInfo)) {
            resetVisit();
            continue;
        }

        // Cheap check first: the cursor must be on one of the monitor's edges.
        const RECT& monitorRect = monitorInfo.rcMonitor;
        if (pt.x > monitorRect.left && pt.x < monitorRect.right - 1 &&
            pt.y > monitorRect.top && pt.y < monitorRect.bottom - 1) {
            resetVisit();
            continue;
        }

        if (visitDone) {
            continue;
        }

        DWORD now = GetTickCount();

        if (!taskbarsLoaded ||
            now - taskbarsRefreshTime >= kTaskbarListRefreshInterval) {
            std::unordered_set<HWND> secondaryTaskbarWindows;
            taskbars.clear();
            if (HWND hWnd = FindTaskbarWindows(&secondaryTaskbarWindows)) {
                taskbars.push_back(hWnd);
                taskbars.insert(taskbars.end(),
                                secondaryTaskbarWindows.begin(),
                                secondaryTaskbarWindows.end());
            }
            taskbarsRefreshTime = now;
            taskbarsLoaded = true;
        }

        HWND hTaskbarWnd = FindCoveredTaskbarAtCursor(pt, monitorRect, taskbars);
        if (!hTaskbarWnd) {
            // Either Windows can handle it, or our reveal took effect.
            if (revealAttempts > 0) {
                visitDone = true;
            }
            continue;
        }

        if (hTaskbarWnd != hEdgeTaskbarWnd) {
            hEdgeTaskbarWnd = hTaskbarWnd;
            edgeEnterTime = now;
            lastRevealTime = 0;
            revealAttempts = 0;
        }

        DWORD delay = g_settings.unhideDelay >= 0 ? g_settings.unhideDelay
                                                  : kDefaultRevealDelay;
        if (now - edgeEnterTime < delay) {
            continue;
        }

        if (revealAttempts >= kMaxRevealAttempts) {
            visitDone = true;
            continue;
        }

        if (lastRevealTime && now - lastRevealTime < kRetryInterval) {
            continue;
        }

        PostMessage(hTaskbarWnd, g_edgeRevealRegisteredMsg, 0, 0);
        lastRevealTime = now;
        revealAttempts++;
    }

    return 0;
}

void StartEdgeRevealThread() {
    if (g_edgeRevealThread) {
        return;
    }

    g_edgeRevealStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_edgeRevealStopEvent) {
        return;
    }

    g_edgeRevealThread =
        CreateThread(nullptr, 0, EdgeRevealThread, nullptr, 0, nullptr);
    if (!g_edgeRevealThread) {
        CloseHandle(g_edgeRevealStopEvent);
        g_edgeRevealStopEvent = nullptr;
    }
}

void StopEdgeRevealThread() {
    if (!g_edgeRevealThread) {
        return;
    }

    SetEvent(g_edgeRevealStopEvent);
    WaitForSingleObject(g_edgeRevealThread, INFINITE);
    CloseHandle(g_edgeRevealThread);
    g_edgeRevealThread = nullptr;
    CloseHandle(g_edgeRevealStopEvent);
    g_edgeRevealStopEvent = nullptr;
}

void UpdateEdgeRevealThread() {
    if (g_settings.edgeReveal) {
        StartEdgeRevealThread();
    } else {
        StopEdgeRevealThread();
    }
}

////////////////////////////////////////////////////////////////////////////////

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
        {R"(?SlideWindow@TrayUI@@UEAAXPEAUHWND__@@PEBUtagRECT@@PEAUHMONITOR__@@_N3@Z)",
         &TrayUI_SlideWindow_Original, TrayUI_SlideWindow_Hook, true},
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
        {
            {LR"(public: virtual void __cdecl TrayUI::SlideWindow(struct HWND__ *,struct tagRECT const *,struct HMONITOR__ *,bool,bool))"},
            &TrayUI_SlideWindow_Original,
            TrayUI_SlideWindow_Hook,
            true,
        },
        {
            {LR"(protected: virtual __int64 __cdecl CTaskBand::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &CTaskBand_v_WndProc_Original,
            CTaskBand_v_WndProc_Hook,
            true,
        },
        {
            {LR"(public: virtual int __cdecl CTaskListWnd::PermitAutoHide(void))"},
            &CTaskListWnd_PermitAutoHide_Original,
            CTaskListWnd_PermitAutoHide_Hook,
            true,
        },
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

void LoadSettings() {
    PCWSTR mode = Wh_GetStringSetting(L"mode");
    g_settings.mode = Mode::intersected;
    if (wcscmp(mode, L"maximized") == 0) {
        g_settings.mode = Mode::maximized;
    } else if (wcscmp(mode, L"always") == 0) {
        g_settings.mode = Mode::always;
    } else if (wcscmp(mode, L"never") == 0) {
        g_settings.mode = Mode::never;
    }
    Wh_FreeStringSetting(mode);

    g_settings.foregroundWindowOnly = Wh_GetIntSetting(L"foregroundWindowOnly");
    g_settings.keepShownOnDesktop = Wh_GetIntSetting(L"keepShownOnDesktop");
    g_settings.keepShownOnEmptyDesktop =
        Wh_GetIntSetting(L"keepShownOnEmptyDesktop");

    PCWSTR animationType = Wh_GetStringSetting(L"animationType");
    g_settings.animationType = AnimationType::slide;
    if (wcscmp(animationType, L"none") == 0) {
        g_settings.animationType = AnimationType::none;
    } else if (wcscmp(animationType, L"elastic") == 0) {
        g_settings.animationType = AnimationType::elastic;
    } else if (wcscmp(animationType, L"bounce") == 0) {
        g_settings.animationType = AnimationType::bounce;
    } else if (wcscmp(animationType, L"fade") == 0) {
        g_settings.animationType = AnimationType::fade;
    } else if (wcscmp(animationType, L"slideFade") == 0) {
        g_settings.animationType = AnimationType::slideFade;
    } else if (wcscmp(animationType, L"overshoot") == 0) {
        g_settings.animationType = AnimationType::overshoot;
    }
    Wh_FreeStringSetting(animationType);

    g_settings.showSpeedup = Wh_GetIntSetting(L"showSpeedup");
    g_settings.hideSpeedup = Wh_GetIntSetting(L"hideSpeedup");
    g_settings.showDuration = Wh_GetIntSetting(L"showDuration");
    g_settings.hideDuration = Wh_GetIntSetting(L"hideDuration");
    g_settings.frameRate = Wh_GetIntSetting(L"frameRate");
    if (g_settings.frameRate <= 0) {
        g_settings.frameRate = 60;
    }
    g_settings.unhideDelay = Wh_GetIntSetting(L"unhideDelay");
    g_settings.hideDelay = Wh_GetIntSetting(L"hideDelay");
    g_settings.keepOnTopOfMaximized = Wh_GetIntSetting(L"keepOnTopOfMaximized");
    g_settings.edgeReveal = Wh_GetIntSetting(L"edgeReveal");
    g_settings.edgeRevealOverFullscreen =
        Wh_GetIntSetting(L"edgeRevealOverFullscreen");
    g_settings.flashingHideDelay = Wh_GetIntSetting(L"flashingHideDelay");

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
    WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);

    auto pKernelBaseGetTickCount = (decltype(&GetTickCount))GetProcAddress(
        kernelBaseModule, "GetTickCount");
    WindhawkUtils::SetFunctionHook(
        pKernelBaseGetTickCount, GetTickCount_Hook, &GetTickCount_Original);

    auto pKernelBaseSleep =
        (decltype(&Sleep))GetProcAddress(kernelBaseModule, "Sleep");
    WindhawkUtils::SetFunctionHook(pKernelBaseSleep, Sleep_Hook,
                                       &Sleep_Original);

    if (hUser32Module) {
        auto pSetTimer =
            (decltype(&SetTimer))GetProcAddress(hUser32Module, "SetTimer");
        WindhawkUtils::SetFunctionHook(pSetTimer, SetTimer_Hook,
                                       &SetTimer_Original);

        auto pSetWindowPos = (decltype(&SetWindowPos))GetProcAddress(
            hUser32Module, "SetWindowPos");
        WindhawkUtils::SetFunctionHook(pSetWindowPos, SetWindowPos_Hook,
                                       &SetWindowPos_Original);

        auto pDeferWindowPos = (decltype(&DeferWindowPos))GetProcAddress(
            hUser32Module, "DeferWindowPos");
        WindhawkUtils::SetFunctionHook(pDeferWindowPos, DeferWindowPos_Hook,
                                       &DeferWindowPos_Original);
    }

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

    WNDCLASS wndclass;
    if (GetClassInfo(GetModuleHandle(nullptr), L"Shell_TrayWnd", &wndclass)) {
        AdjustAllTaskbars();
    }

    UpdateEdgeRevealThread();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    StopEdgeRevealThread();

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
    bool prevKeepShownOnDesktop = g_settings.keepShownOnDesktop;
    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    LoadSettings();

    if (g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11) {
        *bReload = TRUE;
        return TRUE;
    }

    UpdateEdgeRevealThread();

    // The event hook thread only listens for foreground changes when one of
    // these settings needs them, so restart it when they change.
    if (g_settings.mode == Mode::never ||
        prevForegroundWindowOnly != g_settings.foregroundWindowOnly ||
        prevKeepShownOnDesktop != g_settings.keepShownOnDesktop) {
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
