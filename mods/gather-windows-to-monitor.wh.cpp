// ==WindhawkMod==
// @id              gather-windows-to-monitor
// @name            Gather Windows To Monitor
// @description     Move eligible open windows to a chosen monitor with global hotkeys
// @version         0.1.0
// @author          Fred
// @github          https://github.com/fjdiazt
// @include         windhawk.exe
// @compilerOptions -ldwmapi -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Gather Windows To Monitor

Global hotkeys gather visible application windows to a selected monitor work area.

Configure hotkeys in Windhawk settings. Use strings such as `Ctrl+Alt+Shift+1`,
`Ctrl+Win+M`, `F9`, or `None`.

Skipped by default: minimized windows, fullscreen windows/games, hidden windows,
tool windows, owned popups, desktop/taskbar/shell UI, cloaked UWP/helper windows,
and untitled windows.

Enable debug logging to see per-window skip reasons in the Windhawk log.

Known limitations: monitor number hotkeys use Win32 monitor enumeration order, not
the Windows Settings display number. Moving minimized windows without restoring is
not attempted because it is not reliable for all apps.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Enabled: true
  $name: Enable mod
- HotkeyPrimary: "Ctrl+Alt+Shift+P"
  $name: Gather to primary monitor
- HotkeyMonitor1: "Ctrl+Alt+Shift+1"
  $name: Gather to monitor 1
- HotkeyMonitor2: "Ctrl+Alt+Shift+2"
  $name: Gather to monitor 2
- HotkeyMonitor3: "Ctrl+Alt+Shift+3"
  $name: Gather to monitor 3
- HotkeyMouse: "Ctrl+Alt+Shift+M"
  $name: Gather to monitor under mouse
- HotkeyForeground: "Ctrl+Alt+Shift+A"
  $name: Gather to foreground window monitor
- SkipMinimized: true
  $name: Skip minimized windows
- RestoreMinimized: false
  $name: Restore minimized windows before moving
- SkipFullscreen: true
  $name: Skip fullscreen windows
- PreserveSize: true
  $name: Preserve window size
- ShrinkOversized: true
  $name: Shrink oversized windows to fit
  $description: Keeps windows usable when target monitor is smaller.
- CascadeWindows: true
  $name: Cascade windows
- CascadeOffset: 24
  $name: Cascade offset in pixels
- Anchor: center
  $name: Window anchor
  $options:
  - center: Center
  - top-left: Top left
  - mouse: Mouse cursor
- IncludeOwnedWindows: false
  $name: Include owned windows/popups
- DebugLogging: false
  $name: Debug logging
*/
// ==/WindhawkModSettings==

#include <dwmapi.h>
#include <shellapi.h>
#include <windows.h>

#include <algorithm>
#include <atomic>
#include <cwctype>
#include <string>
#include <thread>
#include <vector>

enum class TargetMode {
    Primary,
    Monitor1,
    Monitor2,
    Monitor3,
    Mouse,
    Foreground,
};

enum class SkipReason {
    None,
    Invalid,
    Invisible,
    Cloaked,
    DesktopShell,
    ToolWindow,
    OwnedWindow,
    Untitled,
    Minimized,
    MinimizedNoRestore,
    Fullscreen,
    BadRect,
};

enum class AnchorMode {
    Center,
    TopLeft,
    Mouse,
};

struct Settings {
    bool enabled;
    bool skipMinimized;
    bool restoreMinimized;
    bool skipFullscreen;
    bool preserveSize;
    bool shrinkOversized;
    bool cascadeWindows;
    int cascadeOffset;
    AnchorMode anchor;
    bool includeOwnedWindows;
    bool debugLogging;
    std::wstring hotkeys[6];
};

struct MonitorInfo {
    HMONITOR handle;
    RECT monitor;
    RECT work;
    bool primary;
};

struct Hotkey {
    int id;
    UINT modifiers;
    UINT vk;
    TargetMode mode;
    const wchar_t* name;
};

constexpr UINT WM_APP_RELOAD = WM_APP + 1;
constexpr UINT WM_APP_STOP = WM_APP + 2;

Settings g_settings{};
std::vector<Hotkey> g_hotkeys;
std::thread g_worker;
std::atomic<DWORD> g_workerThreadId{};
HANDLE g_workerReady;

std::wstring GetStringSetting(const wchar_t* name) {
    PCWSTR raw = Wh_GetStringSetting(name);
    std::wstring value = raw ? raw : L"";
    if (raw) {
        Wh_FreeStringSetting(raw);
    }
    return value;
}

AnchorMode ParseAnchorMode(const std::wstring& text);

void LoadSettings() {
    g_settings.enabled = Wh_GetIntSetting(L"Enabled") != 0;
    g_settings.hotkeys[(int)TargetMode::Primary] = GetStringSetting(L"HotkeyPrimary");
    g_settings.hotkeys[(int)TargetMode::Monitor1] = GetStringSetting(L"HotkeyMonitor1");
    g_settings.hotkeys[(int)TargetMode::Monitor2] = GetStringSetting(L"HotkeyMonitor2");
    g_settings.hotkeys[(int)TargetMode::Monitor3] = GetStringSetting(L"HotkeyMonitor3");
    g_settings.hotkeys[(int)TargetMode::Mouse] = GetStringSetting(L"HotkeyMouse");
    g_settings.hotkeys[(int)TargetMode::Foreground] = GetStringSetting(L"HotkeyForeground");
    g_settings.skipMinimized = Wh_GetIntSetting(L"SkipMinimized") != 0;
    g_settings.restoreMinimized = Wh_GetIntSetting(L"RestoreMinimized") != 0;
    g_settings.skipFullscreen = Wh_GetIntSetting(L"SkipFullscreen") != 0;
    g_settings.preserveSize = Wh_GetIntSetting(L"PreserveSize") != 0;
    g_settings.shrinkOversized = Wh_GetIntSetting(L"ShrinkOversized") != 0;
    g_settings.cascadeWindows = Wh_GetIntSetting(L"CascadeWindows") != 0;
    g_settings.cascadeOffset = std::max(0, Wh_GetIntSetting(L"CascadeOffset"));
    g_settings.anchor = ParseAnchorMode(GetStringSetting(L"Anchor"));
    g_settings.includeOwnedWindows = Wh_GetIntSetting(L"IncludeOwnedWindows") != 0;
    g_settings.debugLogging = Wh_GetIntSetting(L"DebugLogging") != 0;
}

std::wstring TrimUpper(std::wstring s) {
    auto notSpace = [](wchar_t c) { return !iswspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
    s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
    for (wchar_t& c : s) {
        c = (wchar_t)towupper(c);
    }
    return s;
}

AnchorMode ParseAnchorMode(const std::wstring& text) {
    std::wstring s = TrimUpper(text);
    if (s == L"TOP-LEFT" || s == L"TOPLEFT") return AnchorMode::TopLeft;
    if (s == L"MOUSE") return AnchorMode::Mouse;
    return AnchorMode::Center;
}

UINT VkFromToken(const std::wstring& key) {
    if (key.empty()) return 0;
    if (key.size() == 1 && key[0] >= L'A' && key[0] <= L'Z') return key[0];
    if (key.size() == 1 && key[0] >= L'0' && key[0] <= L'9') return key[0];
    if (key[0] == L'F' && key.size() <= 3) {
        int n = _wtoi(key.c_str() + 1);
        if (n >= 1 && n <= 24) return VK_F1 + n - 1;
    }
    if (key == L"LEFT") return VK_LEFT;
    if (key == L"RIGHT") return VK_RIGHT;
    if (key == L"UP") return VK_UP;
    if (key == L"DOWN") return VK_DOWN;
    if (key == L"HOME") return VK_HOME;
    if (key == L"END") return VK_END;
    if (key == L"PGUP" || key == L"PAGEUP") return VK_PRIOR;
    if (key == L"PGDN" || key == L"PAGEDOWN") return VK_NEXT;
    if (key == L"INSERT" || key == L"INS") return VK_INSERT;
    if (key == L"DELETE" || key == L"DEL") return VK_DELETE;
    if (key == L"SPACE") return VK_SPACE;
    if (key == L"TAB") return VK_TAB;
    if (key == L"ESC" || key == L"ESCAPE") return VK_ESCAPE;
    if (key == L"BACKSPACE") return VK_BACK;
    return 0;
}

bool ParseHotkey(const std::wstring& text, UINT* modifiers, UINT* vk) {
    std::wstring s = TrimUpper(text);
    if (s.empty() || s == L"NONE" || s == L"DISABLED") {
        return false;
    }

    *modifiers = MOD_NOREPEAT;
    *vk = 0;
    size_t start = 0;
    for (;;) {
        size_t pos = s.find(L'+', start);
        std::wstring token = TrimUpper(s.substr(start, pos - start));
        if (token == L"CTRL" || token == L"CONTROL") *modifiers |= MOD_CONTROL;
        else if (token == L"ALT") *modifiers |= MOD_ALT;
        else if (token == L"SHIFT") *modifiers |= MOD_SHIFT;
        else if (token == L"WIN" || token == L"WINDOWS") *modifiers |= MOD_WIN;
        else *vk = VkFromToken(token);

        if (pos == std::wstring::npos) break;
        start = pos + 1;
    }
    return *vk != 0;
}

void UnregisterConfiguredHotkeys() {
    for (const Hotkey& hotkey : g_hotkeys) {
        UnregisterHotKey(nullptr, hotkey.id);
    }
    g_hotkeys.clear();
}

void RegisterConfiguredHotkeys() {
    UnregisterConfiguredHotkeys();
    if (!g_settings.enabled) {
        Wh_Log(L"Disabled");
        return;
    }

    const TargetMode modes[] = {
        TargetMode::Primary, TargetMode::Monitor1, TargetMode::Monitor2,
        TargetMode::Monitor3, TargetMode::Mouse, TargetMode::Foreground,
    };
    const wchar_t* names[] = {
        L"primary", L"monitor 1", L"monitor 2", L"monitor 3", L"mouse", L"foreground",
    };

    for (size_t i = 0; i < ARRAYSIZE(modes); i++) {
        UINT modifiers = 0;
        UINT vk = 0;
        if (!ParseHotkey(g_settings.hotkeys[(int)modes[i]], &modifiers, &vk)) {
            Wh_Log(L"Hotkey disabled: %s", names[i]);
            continue;
        }

        int id = 100 + i;
        if (RegisterHotKey(nullptr, id, modifiers, vk)) {
            g_hotkeys.push_back({ id, modifiers, vk, modes[i], names[i] });
            Wh_Log(L"Hotkey registered: %s = %s", names[i],
                   g_settings.hotkeys[(int)modes[i]].c_str());
        } else {
            Wh_Log(L"Hotkey register failed: %s = %s, error=%u", names[i],
                   g_settings.hotkeys[(int)modes[i]].c_str(), GetLastError());
        }
    }
}

BOOL CALLBACK MonitorEnumProc(HMONITOR monitor, HDC, LPRECT, LPARAM lParam) {
    auto monitors = reinterpret_cast<std::vector<MonitorInfo>*>(lParam);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfo(monitor, &mi)) {
        monitors->push_back({ monitor, mi.rcMonitor, mi.rcWork,
                              (mi.dwFlags & MONITORINFOF_PRIMARY) != 0 });
    }
    return TRUE;
}

std::vector<MonitorInfo> GetMonitors() {
    std::vector<MonitorInfo> monitors;
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, (LPARAM)&monitors);
    Wh_Log(L"Monitors: %zu", monitors.size());
    for (size_t i = 0; i < monitors.size(); i++) {
        const RECT& m = monitors[i].monitor;
        const RECT& w = monitors[i].work;
        Wh_Log(L"Monitor %zu%s: monitor=(%ld,%ld,%ld,%ld) work=(%ld,%ld,%ld,%ld)",
               i + 1, monitors[i].primary ? L" primary" : L"", m.left, m.top,
               m.right, m.bottom, w.left, w.top, w.right, w.bottom);
    }
    return monitors;
}

const MonitorInfo* PrimaryMonitor(const std::vector<MonitorInfo>& monitors) {
    for (const MonitorInfo& monitor : monitors) {
        if (monitor.primary) return &monitor;
    }
    return monitors.empty() ? nullptr : &monitors[0];
}

const MonitorInfo* MonitorByHandle(const std::vector<MonitorInfo>& monitors, HMONITOR handle) {
    for (const MonitorInfo& monitor : monitors) {
        if (monitor.handle == handle) return &monitor;
    }
    return PrimaryMonitor(monitors);
}

const MonitorInfo* ResolveTargetMonitor(TargetMode mode, const std::vector<MonitorInfo>& monitors) {
    if (monitors.empty()) return nullptr;
    if (mode == TargetMode::Primary) return PrimaryMonitor(monitors);
    if (mode == TargetMode::Monitor1 || mode == TargetMode::Monitor2 || mode == TargetMode::Monitor3) {
        size_t index = (size_t)mode - (size_t)TargetMode::Monitor1;
        if (index < monitors.size()) return &monitors[index];
        Wh_Log(L"Requested monitor %zu missing; using primary", index + 1);
        return PrimaryMonitor(monitors);
    }
    if (mode == TargetMode::Mouse) {
        POINT pt{};
        GetCursorPos(&pt);
        return MonitorByHandle(monitors, MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY));
    }
    HWND foreground = GetForegroundWindow();
    return MonitorByHandle(monitors, MonitorFromWindow(foreground, MONITOR_DEFAULTTOPRIMARY));
}

bool IsClass(HWND hwnd, const wchar_t* name) {
    wchar_t className[128]{};
    return GetClassName(hwnd, className, ARRAYSIZE(className)) &&
           _wcsicmp(className, name) == 0;
}

bool IsShellClass(HWND hwnd) {
    const wchar_t* shellClasses[] = {
        L"Progman", L"WorkerW", L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd",
        L"DV2ControlHost", L"Windows.UI.Core.CoreWindow", L"XamlExplorerHostIslandWindow",
        L"NotifyIconOverflowWindow", L"MultitaskingViewFrame", L"Windows.UI.Composition.DesktopWindowContentBridge",
    };
    for (const wchar_t* name : shellClasses) {
        if (IsClass(hwnd, name)) return true;
    }
    return hwnd == GetDesktopWindow() || hwnd == GetShellWindow();
}

bool IsCloaked(HWND hwnd) {
    BOOL cloaked = FALSE;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked;
}

bool NearlyEqualRect(const RECT& a, const RECT& b, int tolerance) {
    return std::abs(a.left - b.left) <= tolerance &&
           std::abs(a.top - b.top) <= tolerance &&
           std::abs(a.right - b.right) <= tolerance &&
           std::abs(a.bottom - b.bottom) <= tolerance;
}

bool IsLikelyFullscreen(HWND hwnd, const RECT& rect) {
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfo(monitor, &mi)) return false;

    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    bool normalFrame = (style & (WS_CAPTION | WS_THICKFRAME)) != 0;
    if (NearlyEqualRect(rect, mi.rcMonitor, 2)) return true;
    if (!normalFrame && NearlyEqualRect(rect, mi.rcWork, 2)) return true;
    return false;
}

const wchar_t* SkipReasonText(SkipReason reason) {
    switch (reason) {
        case SkipReason::Invalid: return L"invalid";
        case SkipReason::Invisible: return L"invisible";
        case SkipReason::Cloaked: return L"cloaked";
        case SkipReason::DesktopShell: return L"desktop/shell";
        case SkipReason::ToolWindow: return L"tool/noactivate window";
        case SkipReason::OwnedWindow: return L"owned popup";
        case SkipReason::Untitled: return L"empty title";
        case SkipReason::Minimized: return L"minimized";
        case SkipReason::MinimizedNoRestore: return L"minimized without restore";
        case SkipReason::Fullscreen: return L"fullscreen";
        case SkipReason::BadRect: return L"bad rect";
        default: return L"none";
    }
}

bool IsEligibleWindow(HWND hwnd, SkipReason* reason) {
    *reason = SkipReason::None;
    if (!IsWindow(hwnd)) { *reason = SkipReason::Invalid; return false; }
    if (!IsWindowVisible(hwnd)) { *reason = SkipReason::Invisible; return false; }
    if (IsCloaked(hwnd)) { *reason = SkipReason::Cloaked; return false; }
    if (IsShellClass(hwnd)) { *reason = SkipReason::DesktopShell; return false; }

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if (exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) {
        *reason = SkipReason::ToolWindow;
        return false;
    }
    if (!g_settings.includeOwnedWindows && GetWindow(hwnd, GW_OWNER)) {
        *reason = SkipReason::OwnedWindow;
        return false;
    }

    wchar_t title[256]{};
    GetWindowText(hwnd, title, ARRAYSIZE(title));
    if (title[0] == L'\0') {
        *reason = SkipReason::Untitled;
        return false;
    }

    if (IsIconic(hwnd)) {
        if (g_settings.skipMinimized) { *reason = SkipReason::Minimized; return false; }
        if (!g_settings.restoreMinimized) { *reason = SkipReason::MinimizedNoRestore; return false; }
    }

    RECT rect{};
    if (!GetWindowRect(hwnd, &rect) || rect.right <= rect.left || rect.bottom <= rect.top) {
        *reason = SkipReason::BadRect;
        return false;
    }
    if (g_settings.skipFullscreen && IsLikelyFullscreen(hwnd, rect)) {
        *reason = SkipReason::Fullscreen;
        return false;
    }
    return true;
}

void DebugLogSkipReason(HWND hwnd, SkipReason reason) {
    if (!g_settings.debugLogging) return;
    wchar_t title[128]{};
    wchar_t className[128]{};
    GetWindowText(hwnd, title, ARRAYSIZE(title));
    GetClassName(hwnd, className, ARRAYSIZE(className));
    Wh_Log(L"Skip hwnd=%p class=%s title=%s reason=%s", hwnd, className, title, SkipReasonText(reason));
}

bool MoveWindowToMonitor(HWND hwnd, const RECT& workArea, int cascadeIndex) {
    if (!IsWindow(hwnd)) return false;
    if (IsIconic(hwnd) && g_settings.restoreMinimized) {
        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    }

    RECT rect{};
    if (!GetWindowRect(hwnd, &rect)) return false;
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    int workWidth = workArea.right - workArea.left;
    int workHeight = workArea.bottom - workArea.top;

    if (!g_settings.preserveSize || g_settings.shrinkOversized) {
        width = std::min(width, workWidth);
        height = std::min(height, workHeight);
    }

    int x = (int)workArea.left;
    int y = (int)workArea.top;
    if (g_settings.anchor == AnchorMode::Center) {
        x = (int)workArea.left + (workWidth - width) / 2;
        y = (int)workArea.top + (workHeight - height) / 2;
    } else if (g_settings.anchor == AnchorMode::Mouse) {
        POINT pt{};
        GetCursorPos(&pt);
        x = pt.x;
        y = pt.y;
    }

    int offset = g_settings.cascadeWindows ? g_settings.cascadeOffset * cascadeIndex : 0;
    x += offset;
    y += offset;
    if (x + width > workArea.right || y + height > workArea.bottom) {
        x = workArea.left;
        y = workArea.top;
    }

    int minX = (int)workArea.left;
    int minY = (int)workArea.top;
    int maxX = (int)std::max(workArea.left, workArea.right - std::min(width, workWidth));
    int maxY = (int)std::max(workArea.top, workArea.bottom - std::min(height, workHeight));
    x = std::clamp(x, minX, maxX);
    y = std::clamp(y, minY, maxY);

    UINT flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER;
    if (g_settings.preserveSize && !g_settings.shrinkOversized) {
        flags |= SWP_NOSIZE;
    }
    return SetWindowPos(hwnd, nullptr, x, y, width, height, flags) != FALSE;
}

struct GatherState {
    const RECT* workArea;
    int found;
    int moved;
    int skipped;
};

BOOL CALLBACK GatherEnumProc(HWND hwnd, LPARAM lParam) {
    auto state = reinterpret_cast<GatherState*>(lParam);
    SkipReason reason;
    if (!IsEligibleWindow(hwnd, &reason)) {
        state->skipped++;
        DebugLogSkipReason(hwnd, reason);
        return TRUE;
    }

    state->found++;
    if (MoveWindowToMonitor(hwnd, *state->workArea, state->moved)) {
        state->moved++;
    } else {
        state->skipped++;
        DebugLogSkipReason(hwnd, SkipReason::Invalid);
    }
    return TRUE;
}

void GatherWindows(TargetMode mode) {
    if (!g_settings.enabled) return;
    auto monitors = GetMonitors();
    const MonitorInfo* target = ResolveTargetMonitor(mode, monitors);
    if (!target) {
        Wh_Log(L"No monitors found");
        return;
    }

    Wh_Log(L"Target work area: (%ld,%ld,%ld,%ld)", target->work.left, target->work.top,
           target->work.right, target->work.bottom);
    GatherState state{ &target->work, 0, 0, 0 };
    EnumWindows(GatherEnumProc, (LPARAM)&state);
    Wh_Log(L"Gather done: found=%d moved=%d skipped=%d", state.found, state.moved, state.skipped);
}

void WorkerMain() {
    g_workerThreadId = GetCurrentThreadId();
    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(g_workerReady);

    LoadSettings();
    RegisterConfiguredHotkeys();

    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_HOTKEY) {
            for (const Hotkey& hotkey : g_hotkeys) {
                if (hotkey.id == (int)msg.wParam) {
                    Wh_Log(L"Hotkey pressed: %s", hotkey.name);
                    GatherWindows(hotkey.mode);
                    break;
                }
            }
        } else if (msg.message == WM_APP_RELOAD) {
            LoadSettings();
            RegisterConfiguredHotkeys();
        } else if (msg.message == WM_APP_STOP) {
            break;
        }
    }

    UnregisterConfiguredHotkeys();
    g_workerThreadId = 0;
}

BOOL WhTool_ModInit() {
    g_workerReady = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_workerReady) {
        Wh_Log(L"CreateEvent failed: %u", GetLastError());
        return FALSE;
    }
    g_worker = std::thread(WorkerMain);
    WaitForSingleObject(g_workerReady, INFINITE);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    DWORD threadId = g_workerThreadId.load();
    if (threadId) {
        PostThreadMessage(threadId, WM_APP_RELOAD, 0, 0);
    }
}

void WhTool_ModUninit() {
    DWORD threadId = g_workerThreadId.load();
    if (threadId) {
        PostThreadMessage(threadId, WM_APP_STOP, 0, 0);
    }
    if (g_worker.joinable()) {
        g_worker.join();
    }
    if (g_workerReady) {
        CloseHandle(g_workerReady);
        g_workerReady = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.
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
