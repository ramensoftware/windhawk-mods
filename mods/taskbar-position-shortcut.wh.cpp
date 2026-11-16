// ==WindhawkMod==
// @id              taskbar-position-shortcut
// @name            Taskbar Position Shortcut
// @description     Add a native taskbar-position submenu with per-app rules, exclusions, maximized-window rules, and a configurable switching delay.
// @version         0.0.1
// @author          Oji
// https://github.com/          eliogos
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Position Shortcut

**Tested on:** Windows 11 Pro, build **26300.9818**.
Earlier Windows 11 builds have **not been tested**. I also tested this on a single monitor.

Adds a **Taskbar position** submenu to the native Windows 11 taskbar right-click menu. Set the position for the previously focused app, or change
what unlisted apps use via **Global fallback**.

Finally, Windows brought back a feature they thought was not being used.
Personally, when I work in Photoshop, I prefer the taskbar on the left. Or in After Effects, scrolling through layers in the timeline feels more natural with the taskbar on top.

## Features

- **Per-app rules:** Bottom, Top, Left, Right, Windows setting, or Default
  (follow global fallback). Rules saved from the menu persist between restarts.
- **Exclusions:** Ignore an app from the menu or list it in the Settings
  blacklist. Excluded apps temporarily use the remembered Windows position.
- **Maximized-only:** Use the topmost maximized app on the same monitor.
  Small floating windows (such as File Explorer over Photoshop) won't
  replace its position rule. Fullscreen alone doesn't count.
- **Switch delay:** Wait before applying a new position. 0 skips the
  configured delay; checks run approximately every 50 milliseconds.
  A brief safety cooldown prevents overlapping position changes.
- **Simple settings:** App positions are saved directly from the taskbar menu.
  The only editable app list in Windhawk Settings is the blacklist.

## Rule precedence

1. Blacklist (including menu toggles) -> Windows position
2. Menu-saved app rule
3. Global fallback -> Windows position by default

**Default** means follow global fallback; **Windows setting** means bypass
that fallback and restore the remembered Windows position. There is no
separate Default-app whitelist because it duplicated the unlisted case.

Menu-saved rules are stored in Windhawk mod-local storage. They persist across
restarts but do not appear in the Settings tab. The blacklist in Settings
is a separate, manually editable list. It takes precedence over menu rules.
The old Bottom, Top, Left, Right, and Default whitelist settings are no
longer read. Previously saved menu rules are preserved.

**Drag to reposition:** I haven't tried implementing this yet, but according
to [this article](https://www.windowscentral.com/microsoft/windows-11/its-a-deliberate-design-decision-not-a-missing-capability-microsoft-confirms-windows-11-wont-ever-let-you-move-the-taskbar-by-dragging-it-like-on-windows-10),
Microsoft doesn't plan to add native drag-to-move support. This mod does not
implement a drag gesture.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- maximizedOnly: false
  $name: Only apply app-specific positions when maximized
  $description: Use the topmost maximized window on the foreground monitor, including when a smaller window is in front. Fullscreen windows count only if Windows also reports them as maximized. Global fallback applies if no maximized window is found.
- switchDelayMs: 400
  $name: Position switch delay (milliseconds)
  $description: The target position must remain unchanged for this duration before switching. 0 skips the delay (up to ~50 ms polling latency). A brief cooldown protects Explorer from overlapping moves.
- excludedApps: [""]
  $name: Blacklist
  $description: Exclude these executables from automatic position changes. Example - explorer.exe. Blacklisted apps use the remembered Windows position. Add apps here or ignore them using the taskbar menu.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <winver.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/base.h>

#include <algorithm>
#include <atomic>
#include <cwctype>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxi = winrt::Windows::UI::Xaml::Input;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

static constexpr wchar_t kItemName[] = L"WindhawkTaskbarPositionSubmenu";
static constexpr wchar_t kSeparatorName[] = L"WindhawkTaskbarPositionSeparator";
static constexpr wchar_t kRegistryPath[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";
static constexpr wchar_t kLocationValue[] = L"TaskbarLocation";
static constexpr UINT kTaskbarLocationMessage = 0x5CA;
static constexpr int kUseWindowsDefault = -1;
static constexpr int kNoPendingChange = -2;
static constexpr int kRuleWindowsDefault = -3;
static constexpr int kRuleUnlisted = -4;
static constexpr int kNoStoredOverride = -5;
static constexpr int kRuleExcluded = -6;

struct EdgeOption {
    const wchar_t* label;
    DWORD edge;
};

static constexpr EdgeOption kEdges[] = {
    {L"Bottom", ABE_BOTTOM},
    {L"Top", ABE_TOP},
    {L"Left", ABE_LEFT},
    {L"Right", ABE_RIGHT},
};

struct AppRule {
    std::wstring executable;
    int position;
};

std::atomic<bool> g_taskbarViewModuleHooked{false};
std::atomic<bool> g_unloading{false};
std::atomic<bool> g_supported{false};
std::atomic<int> g_manualFallback{kUseWindowsDefault};
std::atomic<int> g_pendingFallback{kNoPendingChange};
std::atomic<DWORD> g_windowsDefaultEdge{ABE_BOTTOM};
std::atomic<bool> g_maximizedOnly{false};
std::atomic<int> g_switchDelayMs{400};
thread_local int g_taskbarSettingsMenuDepth = 0;
thread_local bool g_currentMenuInjected = false;
thread_local std::wstring g_menuTargetApp;

std::mutex g_rulesMutex;
std::vector<AppRule> g_rules;
std::mutex g_foregroundMutex;
std::wstring g_lastForegroundApp;
HANDLE g_stopEvent = nullptr;
HANDLE g_wakeEvent = nullptr;
HANDLE g_monitorThread = nullptr;

bool IsValidEdge(int edge) {
    return edge >= ABE_LEFT && edge <= ABE_BOTTOM;
}

bool HStringEquals(winrt::hstring const& value, const wchar_t* text) {
    return std::wstring_view(value.c_str(), value.size()) == text;
}

bool IsNamedItem(wuxc::MenuFlyoutItemBase const& baseItem,
                 const wchar_t* name) {
    try {
        if (auto frameworkElement = baseItem.try_as<wux::FrameworkElement>()) {
            return HStringEquals(frameworkElement.Name(), name);
        }
    } catch (...) {
    }
    return false;
}

bool IsSeparator(wuxc::MenuFlyoutItemBase const& baseItem) {
    try {
        return !!baseItem.try_as<wuxc::MenuFlyoutSeparator>();
    } catch (...) {
    }
    return false;
}

std::wstring NormalizeExecutable(std::wstring input) {
    auto nonSpace = [](wchar_t c) { return !iswspace(c); };
    auto begin = std::find_if(input.begin(), input.end(), nonSpace);
    auto end = std::find_if(input.rbegin(), input.rend(), nonSpace).base();
    if (begin >= end) {
        return {};
    }
    input = std::wstring(begin, end);
    if (input.size() >= 2 && input.front() == L'"' &&
        input.back() == L'"') {
        input = input.substr(1, input.size() - 2);
    }
    auto slash = input.find_last_of(L"\\/");
    if (slash != std::wstring::npos) {
        input.erase(0, slash + 1);
    }
    std::transform(input.begin(), input.end(), input.begin(), [](wchar_t c) {
        return static_cast<wchar_t>(towlower(c));
    });
    if (input.empty()) {
        return {};
    }
    if (input.size() < 4 || input.substr(input.size() - 4) != L".exe") {
        input += L".exe";
    }
    return input;
}

void LoadAppRules() {
    g_maximizedOnly.store(Wh_GetIntSetting(L"maximizedOnly") != 0);
    int delay = Wh_GetIntSetting(L"switchDelayMs");
    g_switchDelayMs.store(std::max(0, std::min(10000, delay)));
    std::vector<AppRule> loaded;
    for (int i = 0; i < 256; ++i) {
        wchar_t key[128];
        swprintf_s(key, L"excludedApps[%d]", i);
        PCWSTR setting = Wh_GetStringSetting(key);
        std::wstring name = NormalizeExecutable(setting ? setting : L"");
        if (setting) {
            Wh_FreeStringSetting(setting);
        }
        if (name.empty()) {
            continue;
        }
        auto existing = std::find_if(loaded.begin(), loaded.end(),
                                     [&](AppRule const& rule) {
            return rule.executable == name;
        });
        if (existing == loaded.end()) {
            loaded.push_back({std::move(name), kRuleExcluded});
        }
    }

    {
        std::lock_guard<std::mutex> lock(g_rulesMutex);
        g_rules.swap(loaded);
    }
    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }
}

bool TryReadTaskbarEdge(DWORD* edge) {
    DWORD value = ABE_BOTTOM;
    DWORD size = sizeof(value);
    DWORD type = 0;
    LONG error = RegGetValueW(HKEY_CURRENT_USER, kRegistryPath,
                              kLocationValue, RRF_RT_REG_DWORD,
                              &type, &value, &size);
    if (error != ERROR_SUCCESS || !IsValidEdge(static_cast<int>(value))) {
        return false;
    }
    *edge = value;
    return true;
}

bool ApplyTaskbarEdge(DWORD edge) {
    if (!IsValidEdge(static_cast<int>(edge))) {
        return false;
    }
    HKEY key = nullptr;
    LONG error = RegOpenKeyExW(HKEY_CURRENT_USER, kRegistryPath, 0,
                               KEY_SET_VALUE, &key);
    if (error != ERROR_SUCCESS) {
        Wh_Log(L"Can't open taskbar settings: %ld", error);
        return false;
    }
    error = RegSetValueExW(key, kLocationValue, 0, REG_DWORD,
                           reinterpret_cast<const BYTE*>(&edge), sizeof(edge));
    RegCloseKey(key);
    if (error != ERROR_SUCCESS) {
        Wh_Log(L"Can't write taskbar position: %ld", error);
        return false;
    }

    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (taskbar) {
        DWORD_PTR result = 0;
        if (!SendMessageTimeoutW(taskbar, kTaskbarLocationMessage, 6,
                                 static_cast<LPARAM>(edge),
                                 SMTO_ABORTIFHUNG | SMTO_BLOCK,
                                 1500, &result)) {
            Wh_Log(L"Taskbar position notification failed: %lu", GetLastError());
        }
        SendMessageTimeoutW(taskbar, WM_SETTINGCHANGE, 0,
                            reinterpret_cast<LPARAM>(L"TraySettings"),
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 1500, &result);
    }
    Wh_Log(L"Taskbar position requested: %lu", edge);
    return true;
}

bool TryGetForegroundApp(HWND window, std::wstring* executable) {
    if (!window) {
        return false;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    if (!pid) {
        return false;
    }

    if (pid == GetCurrentProcessId()) {
        wchar_t className[128] = {};
        if (!GetClassNameW(window, className, ARRAYSIZE(className))) {
            return false;
        }
        if (_wcsicmp(className, L"Progman") == 0 ||
            _wcsicmp(className, L"WorkerW") == 0) {
            executable->clear();
            return true;
        }
        if (_wcsicmp(className, L"CabinetWClass") != 0 &&
            _wcsicmp(className, L"ExploreWClass") != 0) {
            return false;
        }
    }

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return false;
    }
    wchar_t path[32768] = {};
    DWORD length = ARRAYSIZE(path);
    BOOL ok = QueryFullProcessImageNameW(process, 0, path, &length);
    CloseHandle(process);
    if (!ok) {
        return false;
    }

    *executable = NormalizeExecutable(std::wstring(path, length));
    if (*executable == L"shellexperiencehost.exe" ||
        *executable == L"startmenuexperiencehost.exe" ||
        *executable == L"searchhost.exe" ||
        *executable == L"textinputhost.exe") {
        return false;
    }
    return !executable->empty();
}

bool IsMaximizedAppWindow(HWND window) {
    return window && IsWindowVisible(window) && !IsIconic(window) &&
           IsZoomed(window);
}

struct MaximizedWindowSearch {
    HMONITOR monitor;
    std::wstring executable;
};

BOOL CALLBACK FindMaximizedWindow(HWND window, LPARAM context) {
    auto* search = reinterpret_cast<MaximizedWindowSearch*>(context);
    if (!IsMaximizedAppWindow(window) ||
        GetAncestor(window, GA_ROOT) != window ||
        (GetWindowLongPtrW(window, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) ||
        MonitorFromWindow(window, MONITOR_DEFAULTTONULL) != search->monitor) {
        return TRUE;
    }

    std::wstring app;
    if (!TryGetForegroundApp(window, &app) || app.empty()) {
        return TRUE;
    }

    search->executable = std::move(app);
    return FALSE;
}

// Find the highest maximized application in Z-order on the same monitor.
// A smaller overlay doesn't replace a maximized application's rule.
std::wstring TopMaximizedAppOnForegroundMonitor() {
    HWND foreground = GetForegroundWindow();
    if (!foreground) {
        return {};
    }
    HMONITOR monitor = MonitorFromWindow(foreground, MONITOR_DEFAULTTONULL);
    if (!monitor) {
        return {};
    }
    MaximizedWindowSearch search{monitor, {}};
    EnumWindows(FindMaximizedWindow, reinterpret_cast<LPARAM>(&search));
    return search.executable;
}

void UpdateForegroundApp(HWND window) {
    std::wstring executable;
    if (!TryGetForegroundApp(window, &executable)) {
        return;
    }
    bool changed = false;
    {
        std::lock_guard<std::mutex> lock(g_foregroundMutex);
        if (g_lastForegroundApp != executable) {
            g_lastForegroundApp = std::move(executable);
            changed = true;
        }
    }
    if (changed && g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }
}

std::wstring LastForegroundApp() {
    std::lock_guard<std::mutex> lock(g_foregroundMutex);
    return g_lastForegroundApp;
}

void CALLBACK ForegroundChanged(HWINEVENTHOOK, DWORD event, HWND window,
                                LONG, LONG, DWORD, DWORD) {
    if (event == EVENT_SYSTEM_FOREGROUND && !g_unloading.load()) {
        UpdateForegroundApp(window);
    }
}

int FindStaticAppPosition(std::wstring const& executable) {
    if (executable.empty()) {
        return kRuleUnlisted;
    }
    std::lock_guard<std::mutex> lock(g_rulesMutex);
    for (auto const& rule : g_rules) {
        if (rule.executable == executable) {
            return rule.position;
        }
    }
    return kRuleUnlisted;
}

std::wstring AppOverrideKey(std::wstring const& executable) {
    return L"AppPosition::" + executable;
}

int GetStoredAppPosition(std::wstring const& executable) {
    if (executable.empty()) {
        return kNoStoredOverride;
    }
    std::wstring key = AppOverrideKey(executable);
    int value = Wh_GetIntValue(key.c_str(), kNoStoredOverride);
    if (value == kRuleUnlisted || value == kRuleWindowsDefault ||
        value == kRuleExcluded || IsValidEdge(value)) {
        return value;
    }
    return kNoStoredOverride;
}

bool IsAppExcluded(std::wstring const& executable) {
    if (executable.empty()) {
        return false;
    }
    if (FindStaticAppPosition(executable) == kRuleExcluded) {
        return true;
    }
    return Wh_GetIntValue((L"Excluded::" + executable).c_str(), 0) != 0;
}

void SetAppExcluded(std::wstring const& executable, bool excluded) {
    if (executable.empty() || g_unloading.load()) {
        return;
    }
    std::wstring key = L"Excluded::" + executable;
    if (excluded) {
        Wh_SetIntValue(key.c_str(), 1);
    } else {
        Wh_DeleteValue(key.c_str());
    }
    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }
}

int FindAppPosition(std::wstring const& executable) {
    if (IsAppExcluded(executable)) {
        return kRuleExcluded;
    }
    int overridePosition = GetStoredAppPosition(executable);
    if (overridePosition != kNoStoredOverride) {
        return overridePosition;
    }
    return kRuleUnlisted;
}

DWORD WINAPI MonitorThreadProc(void*) {
    int candidatePosition = kNoPendingChange;
    ULONGLONG candidateSince = 0;
    int lastWritten = Wh_GetIntValue(L"LastAppliedEdge", -1);
    bool ownsLastEdge = Wh_GetIntValue(L"OwnedLastEdge", 0) != 0;
    ULONGLONG lastChangeAt = 0;
    static constexpr ULONGLONG kMinimumMoveIntervalMs = 350;
    HANDLE events[] = {g_stopEvent, g_wakeEvent};

    MSG initialMessage;
    PeekMessageW(&initialMessage, nullptr, 0, 0, PM_NOREMOVE);
    HWINEVENTHOOK foregroundHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
        ForegroundChanged, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!foregroundHook) {
        Wh_Log(L"Foreground event hook unavailable; using polling");
    }
    UpdateForegroundApp(GetForegroundWindow());

    while (!g_unloading.load()) {
        if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) {
            break;
        }
        UpdateForegroundApp(GetForegroundWindow());

        DWORD current;
        if (TryReadTaskbarEdge(&current)) {
            int pending = g_pendingFallback.exchange(kNoPendingChange);
            if (pending != kNoPendingChange) {
                g_manualFallback.store(pending);
                Wh_SetIntValue(L"ManualFallbackEdge", pending);
            }

            if (!ownsLastEdge || static_cast<int>(current) != lastWritten) {
                g_windowsDefaultEdge.store(current);
                Wh_SetIntValue(L"WindowsDefaultEdge", static_cast<int>(current));
                ownsLastEdge = false;
                Wh_SetIntValue(L"OwnedLastEdge", 0);
            }

            std::wstring activeApp = g_maximizedOnly.load()
                                         ? TopMaximizedAppOnForegroundMonitor()
                                         : LastForegroundApp();
            int position = FindAppPosition(activeApp);
            int fallback = g_manualFallback.load();
            DWORD desired = g_windowsDefaultEdge.load();
            if (IsValidEdge(position)) {
                desired = static_cast<DWORD>(position);
            } else if (position == kRuleUnlisted && IsValidEdge(fallback)) {
                desired = static_cast<DWORD>(fallback);
            }

            if (position == kRuleExcluded) {
                desired = g_windowsDefaultEdge.load();
            }
            int newCandidate = static_cast<int>(desired);
            ULONGLONG now = GetTickCount64();
            if (candidatePosition != newCandidate) {
                candidatePosition = newCandidate;
                candidateSince = now;
            }
            if (desired != current && !g_unloading.load() &&
                now - candidateSince >=
                    static_cast<ULONGLONG>(g_switchDelayMs.load()) &&
                (lastChangeAt == 0 ||
                 now - lastChangeAt >= kMinimumMoveIntervalMs)) {
                Wh_SetIntValue(L"WindowsDefaultEdge",
                               static_cast<int>(g_windowsDefaultEdge.load()));
                Wh_SetIntValue(L"LastAppliedEdge", static_cast<int>(desired));
                Wh_SetIntValue(L"OwnedLastEdge", 1);
                lastWritten = static_cast<int>(desired);
                ownsLastEdge = true;
                lastChangeAt = GetTickCount64();
                if (!ApplyTaskbarEdge(desired)) {
                    ownsLastEdge = false;
                    Wh_SetIntValue(L"OwnedLastEdge", 0);
                }
            }
        }

        DWORD wait = MsgWaitForMultipleObjects(
            ARRAYSIZE(events), events, FALSE, 50, QS_ALLINPUT);
        if (wait == WAIT_OBJECT_0) {
            break;
        }
        if (wait == WAIT_OBJECT_0 + ARRAYSIZE(events)) {
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
    }
    if (foregroundHook) {
        UnhookWinEvent(foregroundHook);
    }
    return 0;
}

wuxc::FontIcon MakeIcon(wchar_t const* glyph) {
    wuxc::FontIcon icon;
    icon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    icon.Glyph(glyph);
    icon.FontSize(16);
    return icon;
}

using MenuFlyoutItemBaseVector_Append_t =
    void(__cdecl*)(void* pThis, wuxc::MenuFlyoutItemBase const& item);
MenuFlyoutItemBaseVector_Append_t MenuFlyoutItemBaseVector_Append_Original;

void QueueFallbackChange(int edge) {
    if (g_unloading || (!IsValidEdge(edge) && edge != kUseWindowsDefault)) {
        return;
    }
    g_pendingFallback.store(edge);
    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }
}

void QueueAppPositionChange(std::wstring const& executable, int position) {
    if (g_unloading.load() || executable.empty() ||
        (!IsValidEdge(position) && position != kRuleUnlisted &&
         position != kRuleWindowsDefault &&
         position != kNoStoredOverride)) {
        return;
    }

    std::wstring key = AppOverrideKey(executable);
    BOOL saved = position == kNoStoredOverride
                     ? Wh_DeleteValue(key.c_str())
                     : Wh_SetIntValue(key.c_str(), position);
    if (!saved) {
        Wh_Log(L"Failed to save app position: %s", executable.c_str());
        return;
    }
    Wh_Log(L"Saved position %d for %s", position, executable.c_str());
    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }
}

void AppendGlobalFallbackOptions(wuxc::MenuFlyoutSubItem const& submenu) {
    int fallback = g_pendingFallback.load();
    if (fallback == kNoPendingChange) {
        fallback = g_manualFallback.load();
    }

    wuxc::MenuFlyoutItem defaultItem;
    defaultItem.Text(L"Default (Windows setting)");
    if (fallback == kUseWindowsDefault) {
        defaultItem.Icon(MakeIcon(L"\xE73E"));
    }
    defaultItem.Click([](wf::IInspectable const&,
                         wux::RoutedEventArgs const&) {
        QueueFallbackChange(kUseWindowsDefault);
    });
    submenu.Items().Append(defaultItem);
    submenu.Items().Append(wuxc::MenuFlyoutSeparator{});

    for (auto const& option : kEdges) {
        wuxc::MenuFlyoutItem item;
        item.Text(option.label);
        if (fallback == static_cast<int>(option.edge)) {
            item.Icon(MakeIcon(L"\xE73E"));
        }
        int edge = static_cast<int>(option.edge);
        item.Click([edge](wf::IInspectable const&,
                          wux::RoutedEventArgs const&) {
            QueueFallbackChange(edge);
        });
        submenu.Items().Append(item);
    }
}

void AppendAppOptions(wuxc::MenuFlyoutSubItem const& submenu,
                      std::wstring const& app) {
    int stored = GetStoredAppPosition(app);
    int configured = FindStaticAppPosition(app);
    bool excluded = IsAppExcluded(app);

    wuxc::MenuFlyoutItem heading;
    std::wstring headingText = L"For " + app;
    heading.Text(winrt::hstring{headingText.c_str()});
    heading.IsEnabled(false);
    submenu.Items().Append(heading);
    submenu.Items().Append(wuxc::MenuFlyoutSeparator{});

    wuxc::MenuFlyoutItem excludeItem;
    bool settingsExcluded = configured == kRuleExcluded;
    excludeItem.Text(settingsExcluded ? L"Blacklisted in Windhawk settings"
                     : excluded ? L"Stop ignoring this app"
                                : L"Ignore this app (blacklist)");
    excludeItem.IsEnabled(!settingsExcluded);
    if (excluded) {
        excludeItem.Icon(MakeIcon(L"\xE73E"));
    }
    excludeItem.Click([app, excluded](wf::IInspectable const&,
                                      wux::RoutedEventArgs const&) {
        SetAppExcluded(app, !excluded);
    });
    submenu.Items().Append(excludeItem);
    submenu.Items().Append(wuxc::MenuFlyoutSeparator{});

    wuxc::MenuFlyoutItem defaultItem;
    defaultItem.Text(L"Default (follow global fallback)");
    if (stored == kRuleUnlisted || stored == kNoStoredOverride) {
        defaultItem.Icon(MakeIcon(L"\xE73E"));
    }
    defaultItem.Click([app](wf::IInspectable const&,
                            wux::RoutedEventArgs const&) {
        QueueAppPositionChange(app, kRuleUnlisted);
    });
    submenu.Items().Append(defaultItem);

    wuxc::MenuFlyoutItem windowsItem;
    windowsItem.Text(L"Windows setting");
    if (stored == kRuleWindowsDefault) {
        windowsItem.Icon(MakeIcon(L"\xE73E"));
    }
    windowsItem.Click([app](wf::IInspectable const&,
                            wux::RoutedEventArgs const&) {
        QueueAppPositionChange(app, kRuleWindowsDefault);
    });
    submenu.Items().Append(windowsItem);
    submenu.Items().Append(wuxc::MenuFlyoutSeparator{});

    for (auto const& option : kEdges) {
        wuxc::MenuFlyoutItem item;
        item.Text(option.label);
        if (stored == static_cast<int>(option.edge)) {
            item.Icon(MakeIcon(L"\xE73E"));
        }
        int edge = static_cast<int>(option.edge);
        item.Click([app, edge](wf::IInspectable const&,
                               wux::RoutedEventArgs const&) {
            QueueAppPositionChange(app, edge);
        });
        submenu.Items().Append(item);
    }

    submenu.Items().Append(wuxc::MenuFlyoutSeparator{});
    wuxc::MenuFlyoutSubItem global;
    global.Text(L"Global fallback");
    global.Icon(MakeIcon(L"\xE80F"));
    AppendGlobalFallbackOptions(global);
    submenu.Items().Append(global);
}

void AppendInjectedItems(void* vectorThis) {
    g_currentMenuInjected = true;

    wuxc::MenuFlyoutSubItem submenu;
    submenu.Name(kItemName);
    submenu.Tag(winrt::box_value(winrt::hstring{kItemName}));
    submenu.Text(L"Taskbar position");
    submenu.Icon(MakeIcon(L"\xE90E"));

    std::wstring app = g_menuTargetApp;
    if (app.empty()) {
        AppendGlobalFallbackOptions(submenu);
    } else {
        AppendAppOptions(submenu, app);
    }

    wuxc::MenuFlyoutSeparator separator;
    separator.Name(kSeparatorName);
    separator.Tag(winrt::box_value(winrt::hstring{kSeparatorName}));

    MenuFlyoutItemBaseVector_Append_Original(vectorThis, submenu);
    MenuFlyoutItemBaseVector_Append_Original(vectorThis, separator);
}

void __cdecl MenuFlyoutItemBaseVector_Append_Hook(
    void* pThis,
    wuxc::MenuFlyoutItemBase const& item) {
    if (g_taskbarSettingsMenuDepth > 0 && !g_currentMenuInjected) {
        try {
            if (!IsSeparator(item) &&
                !IsNamedItem(item, kItemName) &&
                !IsNamedItem(item, kSeparatorName)) {
                AppendInjectedItems(pThis);
            }
        } catch (...) {
            Wh_Log(L"Couldn't inject Taskbar position submenu");
        }
    }
    MenuFlyoutItemBaseVector_Append_Original(pThis, item);
}

struct ScopedTaskbarSettingsMenuBuild {
    bool outer = false;

    ScopedTaskbarSettingsMenuBuild() {
        outer = g_taskbarSettingsMenuDepth++ == 0;
        if (outer) {
            g_currentMenuInjected = false;
            UpdateForegroundApp(GetForegroundWindow());
            g_menuTargetApp = g_maximizedOnly.load()
                                  ? TopMaximizedAppOnForegroundMonitor()
                                  : LastForegroundApp();
        }
    }

    ~ScopedTaskbarSettingsMenuBuild() {
        if (outer && !g_currentMenuInjected) {
            Wh_Log(L"No taskbar context menu append observed");
        }
        g_taskbarSettingsMenuDepth--;
        if (outer) {
            g_menuTargetApp.clear();
        }
    }
};

using ContextMenus_ShowTaskbarSettingsContextMenu_t =
    void(__cdecl*)(wux::FrameworkElement const& target,
                   void* taskbarSettings,
                   wuxi::ContextRequestedEventArgs const& args,
                   unsigned long long options);
ContextMenus_ShowTaskbarSettingsContextMenu_t
    ContextMenus_ShowTaskbarSettingsContextMenu_Original;

void __cdecl ContextMenus_ShowTaskbarSettingsContextMenu_Hook(
    wux::FrameworkElement const& target,
    void* taskbarSettings,
    wuxi::ContextRequestedEventArgs const& args,
    unsigned long long options) {
    ScopedTaskbarSettingsMenuBuild scopedBuild;
    ContextMenus_ShowTaskbarSettingsContextMenu_Original(
        target, taskbarSettings, args, options);
}

using RtlGetVersion_t = LONG(WINAPI*)(OSVERSIONINFOW* versionInfo);

DWORD GetWindowsBuild() {
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    auto rtlGetVersion = ntdll ? reinterpret_cast<RtlGetVersion_t>(
                                     GetProcAddress(ntdll, "RtlGetVersion"))
                               : nullptr;
    if (!rtlGetVersion) {
        return 0;
    }

    OSVERSIONINFOW versionInfo = {};
    versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);

    if (rtlGetVersion(&versionInfo) != 0) {
        return 0;
    }

    Wh_Log(L"Windows version: %lu.%lu.%lu", versionInfo.dwMajorVersion,
           versionInfo.dwMinorVersion, versionInfo.dwBuildNumber);

    return versionInfo.dwBuildNumber;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE module) {
    void* fixedFileInfo = nullptr;
    UINT len = 0;

    HRSRC resource =
        FindResourceW(module, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION);
    if (resource) {
        HGLOBAL global = LoadResource(module, resource);
        if (global) {
            void* data = LockResource(global);
            if (data) {
                if (!VerQueryValueW(data, L"\\", &fixedFileInfo, &len) ||
                    len == 0) {
                    fixedFileInfo = nullptr;
                }
            }
        }
    }

    return static_cast<VS_FIXEDFILEINFO*>(fixedFileInfo);
}

void LogModuleInfo(HMODULE module, const wchar_t* reason) {
    wchar_t path[MAX_PATH] = {};
    GetModuleFileNameW(module, path, ARRAYSIZE(path));

    const wchar_t* moduleName = wcsrchr(path, L'\\');
    moduleName = moduleName ? moduleName + 1 : path;

    VS_FIXEDFILEINFO* versionInfo = GetModuleVersionInfo(module);
    if (versionInfo) {
        Wh_Log(L"%s module=%s path=%s version=%u.%u.%u.%u", reason,
               moduleName, path, HIWORD(versionInfo->dwFileVersionMS),
               LOWORD(versionInfo->dwFileVersionMS),
               HIWORD(versionInfo->dwFileVersionLS),
               LOWORD(versionInfo->dwFileVersionLS));
    } else {
        Wh_Log(L"%s module=%s path=%s version=<unavailable>", reason,
               moduleName, path);
    }
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }

    return module;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    LogModuleInfo(module, L"Resolving taskbar symbols");

    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {
                LR"(void __cdecl winrt::Taskbar::implementation::ContextMenus::ShowTaskbarSettingsContextMenu(struct winrt::Windows::UI::Xaml::FrameworkElement const &,struct winrt::WindowsUdk::UI::Shell::TaskbarSettings const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &,unsigned __int64))",
            },
            &ContextMenus_ShowTaskbarSettingsContextMenu_Original,
            ContextMenus_ShowTaskbarSettingsContextMenu_Hook,
        },
        {
            {
                LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IVector<struct winrt::Windows::Foundation::Collections::IVector<struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase>,struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase>::Append(struct winrt::Windows::UI::Xaml::Controls::MenuFlyoutItemBase const &)const )",
            },
            &MenuFlyoutItemBaseVector_Append_Original,
            MenuFlyoutItemBaseVector_Append_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"Skipping injection: missing symbols");
        return false;
    }

    Wh_Log(L"Hook installed: ContextMenus::ShowTaskbarSettingsContextMenu");
    Wh_Log(L"Hook installed: IVector<MenuFlyoutItemBase>::Append");

    return true;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module,
                                     LPCWSTR loadedFileName,
                                     bool applyHookOperations) {
    if (!module || g_taskbarViewModuleHooked) {
        return;
    }

    if (GetTaskbarViewModuleHandle() != module) {
        return;
    }

    if (g_taskbarViewModuleHooked.exchange(true)) {
        return;
    }

    Wh_Log(L"Taskbar module loaded via LoadLibraryExW: %s",
           loadedFileName ? loadedFileName : L"<null>");

    if (HookTaskbarViewDllSymbols(module)) {
        if (applyHookOperations) {
            Wh_ApplyHookOperations();
        }
    } else {
        Wh_Log(L"Skipping injection: unsupported build or missing symbols");
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);

    if (module) {
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName, true);
    }

    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    DWORD build = GetWindowsBuild();
    DWORD current = 0;
    if ((build && build < 22000) || !TryReadTaskbarEdge(&current)) {
        Wh_Log(L"Native taskbar-position registry value unavailable");
        return TRUE;
    }
    g_supported = true;

    int savedDefault = Wh_GetIntValue(L"WindowsDefaultEdge", -1);
    int savedApplied = Wh_GetIntValue(L"LastAppliedEdge", -1);
    bool owned = Wh_GetIntValue(L"OwnedLastEdge", 0) != 0;
    DWORD initialDefault = owned && IsValidEdge(savedDefault) &&
                                   IsValidEdge(savedApplied) &&
                                   current == static_cast<DWORD>(savedApplied)
                               ? static_cast<DWORD>(savedDefault)
                               : current;
    g_windowsDefaultEdge.store(initialDefault);
    Wh_SetIntValue(L"WindowsDefaultEdge", static_cast<int>(initialDefault));
    if (current != static_cast<DWORD>(savedApplied)) {
        Wh_SetIntValue(L"OwnedLastEdge", 0);
    }
    int fallback = Wh_GetIntValue(L"ManualFallbackEdge", kUseWindowsDefault);
    g_manualFallback.store(IsValidEdge(fallback) ? fallback : kUseWindowsDefault);
    LoadAppRules();

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewModuleHooked = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            Wh_Log(L"Skipping injection: missing symbols");
            return TRUE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
    }

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto loadLibraryExW = kernelBase ? reinterpret_cast<LoadLibraryExW_t>(
                                          GetProcAddress(kernelBase,
                                                         "LoadLibraryExW"))
                                    : nullptr;
    if (!loadLibraryExW) {
        Wh_Log(L"LoadLibraryExW not found; hook not installed");
        return TRUE;
    }

    if (WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original)) {
        Wh_Log(L"Hook installed: LoadLibraryExW");
    } else {
        Wh_Log(L"LoadLibraryExW hook install failed");
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"AfterInit");
    if (!g_supported) {
        return;
    }
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_wakeEvent) {
        Wh_Log(L"Cannot start application monitor: event creation failed");
        return;
    }
    g_monitorThread = CreateThread(nullptr, 0, MonitorThreadProc,
                                   nullptr, 0, nullptr);
    if (!g_monitorThread) {
        Wh_Log(L"Cannot start application monitor: thread creation failed");
        return;
    }

    if (!g_taskbarViewModuleHooked) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewModuleHooked.exchange(true)) {
                Wh_Log(L"Taskbar view module found after init");

                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                } else {
                    Wh_Log(L"Skipping injection: missing symbols");
                }
            }
        }
    }
}

void Wh_ModSettingsChanged() {
    if (!g_unloading.load()) {
        LoadAppRules();
    }
}

void Wh_ModUninit() {
    g_unloading = true;
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_monitorThread) {
        WaitForSingleObject(g_monitorThread, INFINITE);
        CloseHandle(g_monitorThread);
        g_monitorThread = nullptr;
    }
    if (g_wakeEvent) {
        CloseHandle(g_wakeEvent);
        g_wakeEvent = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }

    if (g_supported) {
        DWORD current;
        DWORD baseline = g_windowsDefaultEdge.load();
        if (TryReadTaskbarEdge(&current) && current != baseline) {
            ApplyTaskbarEdge(baseline);
        }
        Wh_SetIntValue(L"OwnedLastEdge", 0);
    }
    Wh_Log(L"Uninit");
}
