// ==WindhawkMod==
// @id              gather-windows-to-monitor
// @name            Gather Windows To Display
// @description     Move one window or gather eligible open windows to a chosen display with global hotkeys
// @version         0.1.2
// @author          Fred
// @github          https://github.com/fjdiazt
// @include         windhawk.exe
// @compilerOptions -ldwmapi -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Gather Windows To Display

Global hotkeys move the foreground window or gather visible application windows
to a selected display work area.

By default, `Ctrl+Alt+Shift+W` moves the foreground window to the primary display,
and `Ctrl+Alt+Shift+1` through `Ctrl+Alt+Shift+5` move it to a numbered display.
Numbered gather actions are available but disabled by default.

Configure hotkeys in Windhawk settings. Use strings such as `Ctrl+Alt+Shift+1`,
`Ctrl+Win+M`, `F9`, or `None`.

Window size behavior can preserve dimensions or scale them proportionally between
the source and target display work areas.

Skipped by default: minimized windows, hidden windows, tool windows,
desktop/taskbar/shell UI, cloaked UWP/helper windows, and untitled windows.

The foreground-window action ignores the owned-window and untitled-window filters.
The fullscreen setting applies to every action. Windows already on the target display
are left unchanged. Gather actions restore maximized windows before moving them and
leave them restored for arranging; the foreground-window action re-maximizes them on
the target display. Moved windows are raised above existing target-display windows
without taking focus.

Enable logging in Windhawk to see per-window skip reasons.

Known limitations: numbered display actions use detection order, which can differ
from the display numbers in Windows Settings. Moving minimized windows without restoring is
not attempted because it is not reliable for all apps.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- HotkeyForegroundPrimary: "Ctrl+Alt+Shift+W"
  $name: Move foreground window to primary display
- HotkeyForegroundMonitor1: "Ctrl+Alt+Shift+1"
  $name: Move foreground window to display 1
  $description: Uses the first display detected by Windhawk, which may not be display 1 in Windows Settings.
- HotkeyForegroundMonitor2: "Ctrl+Alt+Shift+2"
  $name: Move foreground window to display 2
  $description: Uses the second display detected by Windhawk, which may not be display 2 in Windows Settings.
- HotkeyForegroundMonitor3: "Ctrl+Alt+Shift+3"
  $name: Move foreground window to display 3
  $description: Uses the third display detected by Windhawk, which may not be display 3 in Windows Settings.
- HotkeyForegroundMonitor4: "Ctrl+Alt+Shift+4"
  $name: Move foreground window to display 4
  $description: Uses the fourth display detected by Windhawk, which may not be display 4 in Windows Settings.
- HotkeyForegroundMonitor5: "Ctrl+Alt+Shift+5"
  $name: Move foreground window to display 5
  $description: Uses the fifth display detected by Windhawk, which may not be display 5 in Windows Settings.
- HotkeyPrimary: "Ctrl+Alt+Shift+P"
  $name: Gather to primary display
- HotkeyMouse: "Ctrl+Alt+Shift+M"
  $name: Gather to display under mouse
- HotkeyForeground: "Ctrl+Alt+Shift+A"
  $name: Gather to foreground window display
  $description: Moves all eligible windows to the display containing the active window.
- HotkeyMonitor1: "None"
  $name: Gather to display 1
  $description: Uses the first display detected by Windhawk, which may not be display 1 in Windows Settings.
- HotkeyMonitor2: "None"
  $name: Gather to display 2
  $description: Uses the second display detected by Windhawk, which may not be display 2 in Windows Settings.
- HotkeyMonitor3: "None"
  $name: Gather to display 3
  $description: Uses the third display detected by Windhawk, which may not be display 3 in Windows Settings.
- HotkeyMonitor4: "None"
  $name: Gather to display 4
  $description: Uses the fourth display detected by Windhawk, which may not be display 4 in Windows Settings.
- HotkeyMonitor5: "None"
  $name: Gather to display 5
  $description: Uses the fifth display detected by Windhawk, which may not be display 5 in Windows Settings.
- SkipMinimized: false
  $name: Skip minimized windows
- RestoreMinimized: false
  $name: Restore minimized windows before moving
  $description: Used only when Skip minimized windows is turned off.
- SkipFullscreen: false
  $name: Skip fullscreen windows
  $description: Applies to every action, including moving only the active window.
- SizeMode: fit
  $name: Window size behavior
  $description: Fit shrinks only oversized windows. Preserve never resizes. Scale resizes all windows for the destination display.
  $options:
  - fit: Fit oversized windows (default)
  - preserve: Always preserve size
  - scale: Scale proportionally
- CascadeWindows: true
  $name: Cascade windows
  $description: Offsets moved windows so they do not completely overlap.
- CascadeOffset: 24
  $name: Cascade offset in pixels
- Anchor: center
  $name: Window anchor
  $description: Chooses the starting position for moved windows.
  $options:
  - center: Center
  - top-left: Top left
  - mouse: Mouse cursor
- IncludeOwnedWindows: true
  $name: Include owned windows/popups
  $description: Also moves dialogs and secondary windows attached to another app window.
*/
// ==/WindhawkModSettings==

#include <dwmapi.h>
#include <shellapi.h>
#include <windows.h>

#include <algorithm>
#include <atomic>
#include <cwctype>
#include <string>
#include <vector>

enum class TargetMode {
    Primary,
    ForegroundPrimary,
    ForegroundMonitor1,
    ForegroundMonitor2,
    ForegroundMonitor3,
    ForegroundMonitor4,
    ForegroundMonitor5,
    Monitor1,
    Monitor2,
    Monitor3,
    Monitor4,
    Monitor5,
    Mouse,
    Foreground,
    Count,
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
    AlreadyOnTarget,
    BadRect,
};

enum class AnchorMode {
    Center,
    TopLeft,
    Mouse,
};

enum class SizeMode {
    Fit,
    Preserve,
    Scale,
};

struct Settings {
    bool skipMinimized;
    bool restoreMinimized;
    bool skipFullscreen;
    SizeMode sizeMode;
    bool cascadeWindows;
    int cascadeOffset;
    AnchorMode anchor;
    bool includeOwnedWindows;
    std::wstring hotkeys[(int)TargetMode::Count];
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

constexpr int CascadeOffsetForIndex(int step, int available, size_t index) {
    if (step <= 0 || available <= 0) return 0;
    int positionCount = available / step + 1;
    return (int)(index % (size_t)positionCount) * step;
}
static_assert(CascadeOffsetForIndex(24, 120, 5) == 120);
static_assert(CascadeOffsetForIndex(24, 120, 6) == 0);

constexpr int ScaleDimensionForWorkArea(int dimension, int sourceExtent,
                                        int targetExtent) {
    if (dimension <= 0 || sourceExtent <= 0 || targetExtent <= 0) {
        return dimension;
    }
    int scaled = (int)((long long)dimension * targetExtent / sourceExtent);
    return std::max(1, scaled);
}
static_assert(ScaleDimensionForWorkArea(800, 1920, 1280) == 533);
static_assert(ScaleDimensionForWorkArea(1, 7680, 800) == 1);

constexpr bool IsForegroundOnlyAction(TargetMode mode) {
    return mode == TargetMode::ForegroundPrimary ||
           (mode >= TargetMode::ForegroundMonitor1 &&
            mode <= TargetMode::ForegroundMonitor5);
}

constexpr int NumberedDisplayIndex(TargetMode mode) {
    if (mode >= TargetMode::ForegroundMonitor1 &&
        mode <= TargetMode::ForegroundMonitor5) {
        return (int)mode - (int)TargetMode::ForegroundMonitor1;
    }
    if (mode >= TargetMode::Monitor1 && mode <= TargetMode::Monitor5) {
        return (int)mode - (int)TargetMode::Monitor1;
    }
    return -1;
}
static_assert(IsForegroundOnlyAction(TargetMode::ForegroundMonitor5));
static_assert(!IsForegroundOnlyAction(TargetMode::Monitor5));
static_assert(NumberedDisplayIndex(TargetMode::ForegroundMonitor2) == 1);
static_assert(NumberedDisplayIndex(TargetMode::Monitor5) == 4);

constexpr UINT WM_APP_RELOAD = WM_APP + 1;
constexpr UINT WM_APP_STOP = WM_APP + 2;

Settings g_settings{};
std::vector<Hotkey> g_hotkeys;
HANDLE g_worker;
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
SizeMode ParseSizeMode(const std::wstring& text);

void LoadSettings() {
    g_settings.hotkeys[(int)TargetMode::Primary] = GetStringSetting(L"HotkeyPrimary");
    g_settings.hotkeys[(int)TargetMode::ForegroundPrimary] =
        GetStringSetting(L"HotkeyForegroundPrimary");
    g_settings.hotkeys[(int)TargetMode::ForegroundMonitor1] =
        GetStringSetting(L"HotkeyForegroundMonitor1");
    g_settings.hotkeys[(int)TargetMode::ForegroundMonitor2] =
        GetStringSetting(L"HotkeyForegroundMonitor2");
    g_settings.hotkeys[(int)TargetMode::ForegroundMonitor3] =
        GetStringSetting(L"HotkeyForegroundMonitor3");
    g_settings.hotkeys[(int)TargetMode::ForegroundMonitor4] =
        GetStringSetting(L"HotkeyForegroundMonitor4");
    g_settings.hotkeys[(int)TargetMode::ForegroundMonitor5] =
        GetStringSetting(L"HotkeyForegroundMonitor5");
    g_settings.hotkeys[(int)TargetMode::Monitor1] = GetStringSetting(L"HotkeyMonitor1");
    g_settings.hotkeys[(int)TargetMode::Monitor2] = GetStringSetting(L"HotkeyMonitor2");
    g_settings.hotkeys[(int)TargetMode::Monitor3] = GetStringSetting(L"HotkeyMonitor3");
    g_settings.hotkeys[(int)TargetMode::Monitor4] = GetStringSetting(L"HotkeyMonitor4");
    g_settings.hotkeys[(int)TargetMode::Monitor5] = GetStringSetting(L"HotkeyMonitor5");
    g_settings.hotkeys[(int)TargetMode::Mouse] = GetStringSetting(L"HotkeyMouse");
    g_settings.hotkeys[(int)TargetMode::Foreground] = GetStringSetting(L"HotkeyForeground");
    g_settings.skipMinimized = Wh_GetIntSetting(L"SkipMinimized") != 0;
    g_settings.restoreMinimized = Wh_GetIntSetting(L"RestoreMinimized") != 0;
    g_settings.skipFullscreen = Wh_GetIntSetting(L"SkipFullscreen") != 0;
    g_settings.sizeMode = ParseSizeMode(GetStringSetting(L"SizeMode"));
    g_settings.cascadeWindows = Wh_GetIntSetting(L"CascadeWindows") != 0;
    g_settings.cascadeOffset = std::max(0, Wh_GetIntSetting(L"CascadeOffset"));
    g_settings.anchor = ParseAnchorMode(GetStringSetting(L"Anchor"));
    g_settings.includeOwnedWindows = Wh_GetIntSetting(L"IncludeOwnedWindows") != 0;
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

SizeMode ParseSizeMode(const std::wstring& text) {
    std::wstring value = TrimUpper(text);
    if (value == L"PRESERVE") return SizeMode::Preserve;
    if (value == L"SCALE") return SizeMode::Scale;
    return SizeMode::Fit;
}

bool TryParseVirtualKey(const std::wstring& key, UINT* vk) {
    struct NamedVirtualKey {
        const wchar_t* name;
        UINT value;
    };
    static constexpr NamedVirtualKey namedKeys[] = {
        { L"LEFT", VK_LEFT },       { L"RIGHT", VK_RIGHT },
        { L"UP", VK_UP },           { L"DOWN", VK_DOWN },
        { L"HOME", VK_HOME },       { L"END", VK_END },
        { L"PGUP", VK_PRIOR },      { L"PAGEUP", VK_PRIOR },
        { L"PGDN", VK_NEXT },       { L"PAGEDOWN", VK_NEXT },
        { L"INSERT", VK_INSERT },   { L"INS", VK_INSERT },
        { L"DELETE", VK_DELETE },   { L"DEL", VK_DELETE },
        { L"SPACE", VK_SPACE },     { L"TAB", VK_TAB },
        { L"ESC", VK_ESCAPE },      { L"ESCAPE", VK_ESCAPE },
        { L"BACKSPACE", VK_BACK },
    };

    UINT parsed = 0;
    if (key.size() == 1 &&
        ((key[0] >= L'A' && key[0] <= L'Z') ||
         (key[0] >= L'0' && key[0] <= L'9'))) {
        parsed = key[0];
    } else if (key.size() >= 2 && key.size() <= 3 && key[0] == L'F') {
        if (key[1] == L'0') return false;
        int number = 0;
        for (size_t i = 1; i < key.size(); i++) {
            if (key[i] < L'0' || key[i] > L'9') return false;
            number = number * 10 + key[i] - L'0';
        }
        if (number >= 1 && number <= 24) parsed = VK_F1 + number - 1;
    } else {
        for (const NamedVirtualKey& namedKey : namedKeys) {
            if (key == namedKey.name) {
                parsed = namedKey.value;
                break;
            }
        }
    }

    if (!parsed) return false;
    *vk = parsed;
    return true;
}

enum class HotkeyParseResult {
    Valid,
    Disabled,
    Invalid,
};

HotkeyParseResult ParseHotkey(const std::wstring& text, UINT* modifiers,
                              UINT* vk) {
    std::wstring s = TrimUpper(text);
    if (s.empty() || s == L"NONE" || s == L"DISABLED") {
        return HotkeyParseResult::Disabled;
    }

    UINT parsedModifiers = MOD_NOREPEAT;
    UINT parsedVk = 0;
    size_t start = 0;
    for (;;) {
        size_t pos = s.find(L'+', start);
        std::wstring token = TrimUpper(s.substr(start, pos - start));
        if (token == L"CTRL" || token == L"CONTROL") {
            parsedModifiers |= MOD_CONTROL;
        } else if (token == L"ALT") {
            parsedModifiers |= MOD_ALT;
        } else if (token == L"SHIFT") {
            parsedModifiers |= MOD_SHIFT;
        } else if (token == L"WIN" || token == L"WINDOWS") {
            parsedModifiers |= MOD_WIN;
        } else {
            UINT tokenVk = 0;
            if (parsedVk || !TryParseVirtualKey(token, &tokenVk)) {
                return HotkeyParseResult::Invalid;
            }
            parsedVk = tokenVk;
        }

        if (pos == std::wstring::npos) break;
        start = pos + 1;
    }

    if (!parsedVk) return HotkeyParseResult::Invalid;
    *modifiers = parsedModifiers;
    *vk = parsedVk;
    return HotkeyParseResult::Valid;
}

void UnregisterConfiguredHotkeys() {
    for (const Hotkey& hotkey : g_hotkeys) {
        UnregisterHotKey(nullptr, hotkey.id);
    }
    g_hotkeys.clear();
}

void RegisterConfiguredHotkeys() {
    UnregisterConfiguredHotkeys();

    // Existing actions register first so an update never changes what an
    // already-configured shortcut does when a new default uses the same keys.
    const TargetMode modes[] = {
        TargetMode::Primary, TargetMode::ForegroundPrimary, TargetMode::Monitor1,
        TargetMode::Monitor2, TargetMode::Monitor3, TargetMode::Monitor4,
        TargetMode::Monitor5, TargetMode::Mouse, TargetMode::Foreground,
        TargetMode::ForegroundMonitor1, TargetMode::ForegroundMonitor2,
        TargetMode::ForegroundMonitor3, TargetMode::ForegroundMonitor4,
        TargetMode::ForegroundMonitor5,
    };
    const wchar_t* names[] = {
        L"primary display", L"foreground to primary display", L"display 1",
        L"display 2", L"display 3", L"display 4", L"display 5",
        L"display under mouse", L"foreground window display",
        L"foreground to display 1", L"foreground to display 2",
        L"foreground to display 3", L"foreground to display 4",
        L"foreground to display 5",
    };

    for (size_t i = 0; i < ARRAYSIZE(modes); i++) {
        UINT modifiers = 0;
        UINT vk = 0;
        const std::wstring& configured = g_settings.hotkeys[(int)modes[i]];
        HotkeyParseResult parseResult = ParseHotkey(configured, &modifiers, &vk);
        if (parseResult == HotkeyParseResult::Disabled) {
            Wh_Log(L"Hotkey disabled: %s", names[i]);
            continue;
        }
        if (parseResult == HotkeyParseResult::Invalid) {
            Wh_Log(L"Invalid hotkey: %s = %s", names[i], configured.c_str());
            continue;
        }

        const Hotkey* conflict = nullptr;
        for (const Hotkey& hotkey : g_hotkeys) {
            if (hotkey.modifiers == modifiers && hotkey.vk == vk) {
                conflict = &hotkey;
                break;
            }
        }
        if (conflict) {
            Wh_Log(L"Hotkey conflict: %s = %s already used by %s; skipping",
                   names[i], configured.c_str(), conflict->name);
            continue;
        }

        int id = 100 + i;
        if (RegisterHotKey(nullptr, id, modifiers, vk)) {
            g_hotkeys.push_back({ id, modifiers, vk, modes[i], names[i] });
            Wh_Log(L"Hotkey registered: %s = %s", names[i],
                   configured.c_str());
        } else {
            Wh_Log(L"Hotkey register failed: %s = %s, error=%u", names[i],
                   configured.c_str(), GetLastError());
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
    Wh_Log(L"Displays: %zu", monitors.size());
    for (size_t i = 0; i < monitors.size(); i++) {
        const RECT& m = monitors[i].monitor;
        const RECT& w = monitors[i].work;
        Wh_Log(L"Display %zu%s: bounds=(%ld,%ld,%ld,%ld) work=(%ld,%ld,%ld,%ld)",
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
    if (mode == TargetMode::Primary || mode == TargetMode::ForegroundPrimary) {
        return PrimaryMonitor(monitors);
    }
    int numberedIndex = NumberedDisplayIndex(mode);
    if (numberedIndex >= 0) {
        size_t index = (size_t)numberedIndex;
        if (index < monitors.size()) return &monitors[index];
        Wh_Log(L"Requested display %zu missing; using primary", index + 1);
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
        case SkipReason::AlreadyOnTarget: return L"already on target display";
        case SkipReason::BadRect: return L"bad rect";
        default: return L"none";
    }
}

bool IsEligibleWindow(HWND hwnd, SkipReason* reason, bool bulk) {
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
    if (bulk && !g_settings.includeOwnedWindows && GetWindow(hwnd, GW_OWNER)) {
        *reason = SkipReason::OwnedWindow;
        return false;
    }

    wchar_t title[256]{};
    GetWindowText(hwnd, title, ARRAYSIZE(title));
    if (bulk && title[0] == L'\0') {
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
    wchar_t title[128]{};
    wchar_t className[128]{};
    GetWindowText(hwnd, title, ARRAYSIZE(title));
    GetClassName(hwnd, className, ARRAYSIZE(className));
    Wh_Log(L"Skip hwnd=%p class=%s title=%s reason=%s", hwnd, className, title, SkipReasonText(reason));
}

bool MoveWindowToMonitor(HWND hwnd, const MonitorInfo& target, int cascadeIndex,
                         bool bulk) {
    if (!IsWindow(hwnd)) return false;
    HMONITOR sourceHandle = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (sourceHandle == target.handle) return false;
    const RECT& workArea = target.work;
    bool wasMaximized = IsZoomed(hwnd);
    bool wasMinimized = IsIconic(hwnd);
    bool wasTopmost = (GetWindowLongPtr(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;

    RECT rect{};
    if (wasMaximized || wasMinimized) {
        WINDOWPLACEMENT placement{ sizeof(placement) };
        if (!GetWindowPlacement(hwnd, &placement)) return false;
        rect = placement.rcNormalPosition;
    } else if (!GetWindowRect(hwnd, &rect)) {
        return false;
    }

    if (wasMaximized) {
        ShowWindowAsync(hwnd, SW_SHOWNOACTIVATE);
    } else if (wasMinimized && g_settings.restoreMinimized) {
        ShowWindowAsync(hwnd, SW_SHOWNOACTIVATE);
    }
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    int workWidth = workArea.right - workArea.left;
    int workHeight = workArea.bottom - workArea.top;

    if (g_settings.sizeMode == SizeMode::Scale) {
        MONITORINFO sourceInfo{ sizeof(sourceInfo) };
        if (!GetMonitorInfo(sourceHandle, &sourceInfo)) return false;
        int sourceWorkWidth = sourceInfo.rcWork.right - sourceInfo.rcWork.left;
        int sourceWorkHeight = sourceInfo.rcWork.bottom - sourceInfo.rcWork.top;
        width = ScaleDimensionForWorkArea(width, sourceWorkWidth, workWidth);
        height = ScaleDimensionForWorkArea(height, sourceWorkHeight, workHeight);
    }
    if (g_settings.sizeMode != SizeMode::Preserve) {
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

    int minX = (int)workArea.left;
    int minY = (int)workArea.top;
    int maxX = (int)std::max(workArea.left, workArea.right - std::min(width, workWidth));
    int maxY = (int)std::max(workArea.top, workArea.bottom - std::min(height, workHeight));
    x = std::clamp(x, minX, maxX);
    y = std::clamp(y, minY, maxY);

    if (g_settings.cascadeWindows) {
        int availableOffset = std::min(maxX - x, maxY - y);
        int offset = CascadeOffsetForIndex(g_settings.cascadeOffset,
                                           availableOffset, cascadeIndex);
        x += offset;
        y += offset;
    }

    UINT flags = SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_ASYNCWINDOWPOS;
    if (g_settings.sizeMode == SizeMode::Preserve) {
        flags |= SWP_NOSIZE;
    }
    bool moved = SetWindowPos(hwnd, HWND_TOPMOST, x, y, width, height, flags) != FALSE;
    if (moved && !wasTopmost) {
        UINT demoteFlags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                           SWP_NOOWNERZORDER | SWP_ASYNCWINDOWPOS;
        if (!SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, demoteFlags)) {
            Wh_Log(L"Failed to clear topmost for hwnd=%p, retrying", hwnd);
            if (!SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0,
                              demoteFlags)) {
                Wh_Log(L"Failed to clear topmost for hwnd=%p", hwnd);
            }
        }
    }
    if (wasMaximized && (!moved || !bulk)) {
        ShowWindowAsync(hwnd, SW_MAXIMIZE);
    }
    return moved;
}

struct GatherState {
    const MonitorInfo* target;
    bool bulk;
    std::vector<HWND> windows;
    int found;
    int moved;
    int skipped;
};

BOOL CALLBACK GatherEnumProc(HWND hwnd, LPARAM lParam) {
    auto state = reinterpret_cast<GatherState*>(lParam);
    SkipReason reason;
    if (!IsEligibleWindow(hwnd, &reason, state->bulk)) {
        state->skipped++;
        DebugLogSkipReason(hwnd, reason);
        return TRUE;
    }
    if (MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST) ==
        state->target->handle) {
        state->skipped++;
        DebugLogSkipReason(hwnd, SkipReason::AlreadyOnTarget);
        return TRUE;
    }

    state->found++;
    state->windows.push_back(hwnd);
    return TRUE;
}

void GatherWindows(TargetMode mode) {
    auto monitors = GetMonitors();
    const MonitorInfo* target = ResolveTargetMonitor(mode, monitors);
    if (!target) {
        Wh_Log(L"No displays found");
        return;
    }

    Wh_Log(L"Target work area: (%ld,%ld,%ld,%ld)", target->work.left, target->work.top,
           target->work.right, target->work.bottom);
    bool foregroundOnly = IsForegroundOnlyAction(mode);
    GatherState state{ target,
                       !foregroundOnly,
                       {},
                       0,
                       0,
                       0 };
    if (foregroundOnly) {
        GatherEnumProc(GetForegroundWindow(), (LPARAM)&state);
    } else {
        EnumWindows(GatherEnumProc, (LPARAM)&state);
    }
    for (size_t i = state.windows.size(); i-- > 0;) {
        HWND hwnd = state.windows[i];
        if (MoveWindowToMonitor(hwnd, *state.target, (int)i, state.bulk)) {
            state.moved++;
        } else {
            state.skipped++;
            DebugLogSkipReason(hwnd, SkipReason::Invalid);
        }
    }
    Wh_Log(L"Gather done: found=%d moved=%d skipped=%d", state.found, state.moved, state.skipped);
}

DWORD WINAPI WorkerMain(LPVOID) {
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
    return 0;
}

BOOL WhTool_ModInit() {
    g_workerReady = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_workerReady) {
        Wh_Log(L"CreateEvent failed: %u", GetLastError());
        return FALSE;
    }
    g_worker = CreateThread(nullptr, 0, WorkerMain, nullptr, 0, nullptr);
    if (!g_worker) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        CloseHandle(g_workerReady);
        g_workerReady = nullptr;
        return FALSE;
    }
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
    if (g_worker) {
        WaitForSingleObject(g_worker, 3000);
        CloseHandle(g_worker);
        g_worker = nullptr;
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
