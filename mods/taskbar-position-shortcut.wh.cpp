// ==WindhawkMod==
// @id              taskbar-position-shortcut
// @name            Taskbar Position Shortcut
// @description     Adds quick taskbar position controls and optional saved positions for individual apps.
// @version         0.1.0
// @author          Oji
// @github          https://github.com/eliogos
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion -ladvapi32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Position Shortcut
Adds a **Taskbar position** submenu when you right-click the Windows 11 taskbar. Move it to any edge without saving anything, or set a position for a specific app.
Finally, Windows brought back a feature they thought was not being used. I prefer my taskbar on the left when working in Photoshop, and on top when scrolling through layers in After Effects.
![Taskbar position context menu](https://imgur.com/a/BU9e2Wt)

## Features

- **Quick shortcut:** Choose Follow Windows setting, Top, Left, Right, or Bottom. No app rule is saved.
- **Per-app positions:** Open the app's submenu to save a position. Choose Default to remove its rule.
- **Maximized-only mode:** Only apply app rules when a maximized window is found.
- **Switch delay:** Adjust how long the mod waits before moving the taskbar.
- **Blacklist:** Ignore apps you don't want triggering position changes.

App rules are saved locally in Windhawk and won't appear in its Settings tab.

## Compatibility

Requires Windows 11 with native taskbar positioning. I've tried it on builds **26220.9836** and **26300.9818**. Older builds haven't been tested. I've only tested it with one monitor.

The mod doesn't change the saved Windows taskbar position. It may conflict with other mods that move the taskbar.

Credits: Taskbar menu injection based on `taskbar-restart-explorer`; runtime position hooking based on `taskbar-on-top`.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- maximizedOnly: false
  $name: Only apply app-specific positions when maximized
  $description: Use the topmost maximized window on the foreground monitor, including when a smaller window is in front. Fullscreen windows count only if Windows also reports them as maximized. The Windows taskbar position applies if no maximized window is found.
- switchDelayMs: 400
  $name: Position switch delay (milliseconds)
  $description: The target position must remain unchanged for this duration before switching. 0 skips the configured delay. A brief cooldown protects Explorer from overlapping moves.
- excludedApps: [""]
  $name: Blacklist
  $description: Exclude these executables from automatic position changes. Example - explorer.exe. Blacklisted apps leave the taskbar in its current position. Add apps here or ignore them using the taskbar menu.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <winver.h>
#include <dwmapi.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
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
    {L"Top", ABE_TOP},
    {L"Left", ABE_LEFT},
    {L"Right", ABE_RIGHT},
    {L"Bottom", ABE_BOTTOM},
};



std::atomic<bool> g_taskbarViewModuleHooked{false};
std::atomic<bool> g_menuHookAttempted{false};
std::atomic<bool> g_unloading{false};
std::atomic<bool> g_supported{false};
std::atomic<DWORD> g_windowsDefaultEdge{ABE_BOTTOM};
std::atomic<int> g_effectiveEdge{-1};
std::atomic<bool> g_hostProcess{false};
std::atomic<bool> g_runtimeHookReady{false};
std::atomic<DWORD> g_taskbarThreadId{0};
std::atomic<bool> g_maximizedOnly{false};
std::atomic<int> g_quickPosition{kNoPendingChange};
std::atomic<int> g_switchDelayMs{400};
thread_local int g_taskbarSettingsMenuDepth = 0;
thread_local bool g_currentMenuInjected = false;
thread_local std::wstring const* g_menuTargetApp = nullptr;

std::mutex g_rulesMutex;
std::vector<std::wstring> g_rules;
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
    std::vector<std::wstring> loaded;
    for (int i = 0; i < 256; ++i) {
        wchar_t key[128];
        swprintf_s(key, L"excludedApps[%d]", i);
        WindhawkUtils::StringSetting setting =
            WindhawkUtils::StringSetting::make(key);
        std::wstring name = NormalizeExecutable(setting.get());
        if (name.empty()) {
            continue;
        }
        if (std::find(loaded.begin(), loaded.end(), name) == loaded.end()) {
            loaded.push_back(std::move(name));
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

using RegGetValueW_t = decltype(&RegGetValueW);
RegGetValueW_t RegGetValueW_Original = nullptr;
thread_local bool g_inTaskbarSettingsLoad = false;
void StartMonitorWhenReady();
void ActivateTaskbarHost();
using TrayUI_GetSaveStateAndInitRects_t = void(__cdecl*)(void*);
TrayUI_GetSaveStateAndInitRects_t TrayUI_GetSaveStateAndInitRects_Original = nullptr;
void __cdecl TrayUI_GetSaveStateAndInitRects_Hook(void* pThis) {
    ActivateTaskbarHost();
    bool old = g_inTaskbarSettingsLoad;
    g_inTaskbarSettingsLoad = true;
    TrayUI_GetSaveStateAndInitRects_Original(pThis);
    g_inTaskbarSettingsLoad = old;
}
using CSecondaryTray_LoadSettings_t = HRESULT(__cdecl*)(void*);
CSecondaryTray_LoadSettings_t CSecondaryTray_LoadSettings_Original = nullptr;
HRESULT __cdecl CSecondaryTray_LoadSettings_Hook(void* pThis) {
    bool old = g_inTaskbarSettingsLoad;
    g_inTaskbarSettingsLoad = true;
    HRESULT result = CSecondaryTray_LoadSettings_Original(pThis);
    g_inTaskbarSettingsLoad = old;
    return result;
}
using TrayUI_WndProc_t = LRESULT(WINAPI*)(void*, HWND, UINT, WPARAM, LPARAM, bool*);
TrayUI_WndProc_t TrayUI_WndProc_Original = nullptr;
LRESULT WINAPI TrayUI_WndProc_Hook(void* pThis, HWND window, UINT message,
                                    WPARAM wParam, LPARAM lParam, bool* flag) {
    if (message == kTaskbarLocationMessage && wParam == 6 &&
        !g_unloading.load() && g_hostProcess.load()) {
        int effective = g_effectiveEdge.load();
        if (IsValidEdge(effective)) lParam = static_cast<LPARAM>(effective);
    }
    return TrayUI_WndProc_Original(pThis, window, message, wParam, lParam, flag);
}
using TaskbarSettings_Location_t = int(__cdecl*)(void* pThis);
TaskbarSettings_Location_t TaskbarSettings_Location_Original = nullptr;

HWND FindCurrentProcessTaskbarWnd() {
    HWND window = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD pid = 0;
    if (window) GetWindowThreadProcessId(window, &pid);
    return pid == GetCurrentProcessId() ? window : nullptr;
}

bool TryReadTaskbarEdge(DWORD* edge) {
    DWORD value = ABE_BOTTOM;
    DWORD size = sizeof(value);
    auto read = RegGetValueW_Original ? RegGetValueW_Original : RegGetValueW;
    LONG error = read(HKEY_CURRENT_USER, kRegistryPath, kLocationValue,
                      RRF_RT_REG_DWORD, nullptr, &value, &size);
    if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
        // Windows can represent its default Bottom position by omitting the
        // registry value. Never create or persist the value just to enable
        // the mod. The native taskbar hooks remain the capability check.
        *edge = ABE_BOTTOM;
        return true;
    }
    if (error != ERROR_SUCCESS || !IsValidEdge(static_cast<int>(value))) {
        Wh_Log(L"TaskbarLocation read failed: error=%ld, value=%lu", error, value);
        return false;
    }
    *edge = value;
    return true;
}

int __cdecl TaskbarSettings_Location_Hook(void* pThis) {
    if (!g_unloading.load() && g_hostProcess.load()) {
        int edge = g_effectiveEdge.load();
        if (IsValidEdge(edge)) return edge;
    }
    return TaskbarSettings_Location_Original(pThis);
}

LONG WINAPI RegGetValueW_Hook(HKEY hkey, LPCWSTR subkey, LPCWSTR value,
                              DWORD flags, LPDWORD type, PVOID data, LPDWORD size) {
    DWORD bufferSize = size ? *size : 0;
    LONG result = RegGetValueW_Original(hkey, subkey, value, flags, type, data, size);
    if (g_unloading.load() || !g_hostProcess.load() ||
        (!g_inTaskbarSettingsLoad &&
         GetCurrentThreadId() != g_taskbarThreadId.load()) ||
        hkey != HKEY_CURRENT_USER || !subkey || !value || !data || !size ||
        _wcsicmp(subkey, kRegistryPath) || _wcsicmp(value, kLocationValue) ||
        !(flags & RRF_RT_REG_DWORD) || bufferSize < sizeof(DWORD)) return result;
    int edge = g_effectiveEdge.load();
    if (!IsValidEdge(edge)) return result;
    *static_cast<DWORD*>(data) = static_cast<DWORD>(edge);
    *size = sizeof(DWORD);
    if (type) *type = REG_DWORD;
    return ERROR_SUCCESS;
}

bool ApplyTaskbarEdge(DWORD edge) {
    if (!IsValidEdge(static_cast<int>(edge)) || !g_runtimeHookReady.load()) return false;
    HWND taskbar = FindCurrentProcessTaskbarWnd();
    if (!taskbar) return false;
    int previous = g_effectiveEdge.exchange(static_cast<int>(edge));
    DWORD_PTR result = 0;
    if (!SendMessageTimeoutW(taskbar, kTaskbarLocationMessage, 6,
                             static_cast<LPARAM>(edge), SMTO_ABORTIFHUNG | SMTO_BLOCK,
                             1500, &result)) {
        g_effectiveEdge.store(previous);
        return false;
    }
    SendMessageTimeoutW(taskbar, WM_SETTINGCHANGE, 0,
                        reinterpret_cast<LPARAM>(L"TraySettings"),
                        SMTO_ABORTIFHUNG | SMTO_BLOCK, 1500, &result);
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
    std::wstring path(32768, L'\0');
    DWORD length = static_cast<DWORD>(path.size());
    BOOL ok = QueryFullProcessImageNameW(process, 0, path.data(), &length);
    CloseHandle(process);
    if (!ok) {
        return false;
    }

    *executable = NormalizeExecutable(path.substr(0, length));
    if (*executable == L"shellexperiencehost.exe" ||
        *executable == L"startmenuexperiencehost.exe" ||
        *executable == L"searchhost.exe" ||
        *executable == L"textinputhost.exe") {
        return false;
    }
    return !executable->empty();
}

bool IsMaximizedAppWindow(HWND window) {
    if (!window || !IsWindowVisible(window) || IsIconic(window) || !IsZoomed(window)) return false;
    DWORD cloaked = 0;
    return FAILED(DwmGetWindowAttribute(window, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) || !cloaked;
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
    if (changed) {
        // A manual shortcut lasts only until the active app changes.
        g_quickPosition.store(kNoPendingChange);
        if (g_wakeEvent) SetEvent(g_wakeEvent);
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
        if (rule == executable) return kRuleExcluded;
    }
    return kRuleUnlisted;
}

std::wstring StorageName(wchar_t const* prefix, std::wstring executable) {
    std::replace(executable.begin(), executable.end(), L'=', L'*');
    return std::wstring(prefix) + executable;
}
std::wstring AppOverrideKey(std::wstring const& executable) {
    return StorageName(L"AppPosition::", executable);
}

int GetStoredAppPosition(std::wstring const& executable) {
    if (executable.empty()) {
        return kNoStoredOverride;
    }
    std::wstring key = AppOverrideKey(executable);
    int value = Wh_GetIntValue(key.c_str(), kNoStoredOverride);
    if (value == kRuleUnlisted || value == kRuleWindowsDefault ||
        IsValidEdge(value)) {
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
    return Wh_GetIntValue(StorageName(L"Excluded::", executable).c_str(), 0) != 0;
}

void SetAppExcluded(std::wstring const& executable, bool excluded) {
    if (executable.empty() || g_unloading.load()) {
        return;
    }
    std::wstring key = StorageName(L"Excluded::", executable);
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

void CALLBACK WindowStateChanged(HWINEVENTHOOK, DWORD event, HWND window,
                                 LONG object, LONG child, DWORD, DWORD) {
    if (g_unloading.load() || !g_maximizedOnly.load() ||
        object != OBJID_WINDOW || child != CHILDID_SELF || !window) return;
    if (event == EVENT_OBJECT_LOCATIONCHANGE &&
        GetAncestor(window, GA_ROOT) != window) return;
    if (g_wakeEvent) SetEvent(g_wakeEvent);
}

DWORD WINAPI MonitorThreadProc(void*) {
    MSG message{};
    PeekMessageW(&message, nullptr, 0, 0, PM_NOREMOVE);
    HWINEVENTHOOK foregroundHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
        ForegroundChanged, 0, 0, WINEVENT_OUTOFCONTEXT);
    HWINEVENTHOOK stateHook = nullptr;
    HWINEVENTHOOK locationHook = nullptr;
    HWINEVENTHOOK destroyHook = nullptr;
    HWINEVENTHOOK cloakHook = nullptr;
    bool stateHooksEnabled = false;
    auto updateStateHooks = [&]() {
        bool enabled = g_maximizedOnly.load();
        if (enabled == stateHooksEnabled) return;
        stateHooksEnabled = enabled;
        if (enabled) {
            stateHook = SetWinEventHook(EVENT_SYSTEM_MINIMIZESTART,
                EVENT_SYSTEM_MINIMIZEEND, nullptr, WindowStateChanged,
                0, 0, WINEVENT_OUTOFCONTEXT);
            locationHook = SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE,
                EVENT_OBJECT_LOCATIONCHANGE, nullptr, WindowStateChanged,
                0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
            destroyHook = SetWinEventHook(EVENT_OBJECT_DESTROY,
                EVENT_OBJECT_HIDE, nullptr, WindowStateChanged,
                0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
            cloakHook = SetWinEventHook(EVENT_OBJECT_CLOAKED,
                EVENT_OBJECT_UNCLOAKED, nullptr, WindowStateChanged,
                0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
        } else {
            if (stateHook) UnhookWinEvent(stateHook);
            if (locationHook) UnhookWinEvent(locationHook);
            if (destroyHook) UnhookWinEvent(destroyHook);
            if (cloakHook) UnhookWinEvent(cloakHook);
            stateHook = locationHook = destroyHook = cloakHook = nullptr;
        }
    };
    UpdateForegroundApp(GetForegroundWindow());
    HKEY registryKey = nullptr;
    HANDLE registryEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    RegOpenKeyExW(HKEY_CURRENT_USER, kRegistryPath, 0, KEY_NOTIFY, &registryKey);
    auto armNotification = [&]() {
        if (registryKey && registryEvent) {
            RegNotifyChangeKeyValue(registryKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET,
                                    registryEvent, TRUE);
        }
    };
    armNotification();
    int candidate = kNoPendingChange;
    ULONGLONG candidateSince = 0;
    ULONGLONG lastChangeAt = 0;
    ULONGLONG nextRetryAt = 0;
    constexpr ULONGLONG minimumInterval = 350;
    DWORD lastRealEdge = ABE_BOTTOM;
    TryReadTaskbarEdge(&lastRealEdge);
    bool evaluate = true;
    while (!g_unloading.load()) {
        if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) break;
        updateStateHooks();
        if (evaluate) {
            DWORD realEdge = ABE_BOTTOM;
            if (TryReadTaskbarEdge(&realEdge)) {
                if (g_windowsDefaultEdge.exchange(realEdge) != realEdge) {
                    Wh_Log(L"Windows setting changed to %lu", realEdge);
                    // Preserve the current app-specific override, if any.
                    // TrayUI_WndProc_Hook keeps the frame and content aligned.
                }
                int desired = static_cast<int>(realEdge);
                int manual = g_quickPosition.load();
                bool quickMode = manual != kNoPendingChange;
                if (quickMode) {
                    if (IsValidEdge(manual)) desired = manual;
                } else {
                    std::wstring app = g_maximizedOnly.load()
                        ? TopMaximizedAppOnForegroundMonitor() : LastForegroundApp();
                    int position = FindAppPosition(app);
                    if (position == kRuleExcluded) {
                        desired = g_effectiveEdge.load();
                        if (!IsValidEdge(desired)) desired = static_cast<int>(realEdge);
                    } else if (IsValidEdge(position)) {
                        desired = position;
                    }
                }
                ULONGLONG now = GetTickCount64();
                if (candidate != desired) {
                    candidate = desired;
                    candidateSince = now;
                }
                int applied = g_effectiveEdge.load();
                if (!IsValidEdge(applied)) applied = static_cast<int>(realEdge);
                if (desired != applied && now - candidateSince >=
                        static_cast<ULONGLONG>(quickMode ? 0 : g_switchDelayMs.load()) &&
                    (!lastChangeAt || now - lastChangeAt >= minimumInterval) &&
                    now >= nextRetryAt) {
                    if (ApplyTaskbarEdge(static_cast<DWORD>(desired))) {
                        lastChangeAt = GetTickCount64();
                        nextRetryAt = 0;
                    } else {
                        nextRetryAt = GetTickCount64() + 1000;
                    }
                }
                lastRealEdge = realEdge;
            }
        }
        ULONGLONG now = GetTickCount64();
        DWORD timeout = INFINITE;
        int effective = g_effectiveEdge.load();
        if (!IsValidEdge(effective)) effective = static_cast<int>(lastRealEdge);
        if (candidate != effective && IsValidEdge(candidate)) {
            ULONGLONG ready = candidateSince + static_cast<ULONGLONG>(
                g_quickPosition.load() == kNoPendingChange ? g_switchDelayMs.load() : 0);
            if (lastChangeAt) ready = std::max(ready, lastChangeAt + minimumInterval);
            ready = std::max(ready, nextRetryAt);
            timeout = ready > now ? static_cast<DWORD>(std::min<ULONGLONG>(ready - now, 10000)) : 0;
        }
        if (!foregroundHook) timeout = std::min<DWORD>(timeout, 400);
        HANDLE handles[3] = {g_stopEvent, g_wakeEvent, registryEvent};
        DWORD count = registryEvent && registryKey ? 3 : 2;
        DWORD result = MsgWaitForMultipleObjects(count, handles, FALSE, timeout, QS_ALLINPUT);
        if (result == WAIT_OBJECT_0) break;
        evaluate = result != WAIT_OBJECT_0 + count;
        if (count == 3 && result == WAIT_OBJECT_0 + 2) armNotification();
        if (result == WAIT_OBJECT_0 + count) {
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) break;
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
        if (!foregroundHook) UpdateForegroundApp(GetForegroundWindow());
    }
    if (foregroundHook) UnhookWinEvent(foregroundHook);
    if (stateHook) UnhookWinEvent(stateHook);
    if (locationHook) UnhookWinEvent(locationHook);
    if (destroyHook) UnhookWinEvent(destroyHook);
    if (cloakHook) UnhookWinEvent(cloakHook);
    if (registryKey) RegCloseKey(registryKey);
    if (registryEvent) CloseHandle(registryEvent);
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
struct InjectedItem {
    winrt::weak_ref<wuxc::MenuFlyoutItem> item;
    winrt::event_token token{};
};
std::vector<InjectedItem> g_injectedItems;

template<typename Handler>
void AddClick(wuxc::MenuFlyoutItem const& item, Handler handler) {
    auto token = item.Click(wux::RoutedEventHandler{handler});
    g_injectedItems.push_back({winrt::weak_ref<wuxc::MenuFlyoutItem>(item), token});
}


void QueueQuickPositionChange(int edge) {
    if (g_unloading.load() ||
        (edge != kUseWindowsDefault && !IsValidEdge(edge))) return;
    g_quickPosition.store(edge);
    if (g_wakeEvent) SetEvent(g_wakeEvent);
}

void AppendQuickPositionOptions(wuxc::MenuFlyoutSubItem const& submenu) {
    int selected = g_quickPosition.load();
    if (selected == kNoPendingChange) selected = g_effectiveEdge.load();

    wuxc::MenuFlyoutItem restore;
    restore.Text(L"Follow Windows setting");
    if (selected == kUseWindowsDefault ||
        (g_quickPosition.load() == kNoPendingChange &&
         selected == static_cast<int>(g_windowsDefaultEdge.load()))) {
        restore.Icon(MakeIcon(L"\xE73E"));
    }
    AddClick(restore, [](wf::IInspectable const&, wux::RoutedEventArgs const&) {
        QueueQuickPositionChange(kUseWindowsDefault);
    });
    submenu.Items().Append(restore);
    submenu.Items().Append(wuxc::MenuFlyoutSeparator{});

    for (auto const& option : kEdges) {
        wuxc::MenuFlyoutItem item;
        item.Text(option.label);
        int edge = static_cast<int>(option.edge);
        if (edge == selected &&
            !(g_quickPosition.load() == kNoPendingChange &&
              edge == static_cast<int>(g_windowsDefaultEdge.load()))) {
            item.Icon(MakeIcon(L"\xE73E"));
        }
        AddClick(item, [edge](wf::IInspectable const&, wux::RoutedEventArgs const&) {
            QueueQuickPositionChange(edge);
        });
        submenu.Items().Append(item);
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
    // App menu selections supersede a temporary manual shortcut.
    g_quickPosition.store(kNoPendingChange);
    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }
}

void AppendAppOptions(wuxc::MenuFlyoutSubItem const& submenu,
                      std::wstring const& app) {
    int stored = GetStoredAppPosition(app);
    bool excluded = IsAppExcluded(app);
    bool settingsExcluded = FindStaticAppPosition(app) == kRuleExcluded;

    wuxc::MenuFlyoutItem defaultItem;
    defaultItem.Text(L"Default (follow Windows setting)");
    if (stored == kRuleUnlisted || stored == kRuleWindowsDefault ||
        stored == kNoStoredOverride) {
        defaultItem.Icon(MakeIcon(L"\xE73E"));
    }
    AddClick(defaultItem, [app](wf::IInspectable const&, wux::RoutedEventArgs const&) {
        QueueAppPositionChange(app, kNoStoredOverride);
    });
    submenu.Items().Append(defaultItem);

    submenu.Items().Append(wuxc::MenuFlyoutSeparator{});

    for (auto const& option : kEdges) {
        wuxc::MenuFlyoutItem item;
        item.Text(option.label);
        int edge = static_cast<int>(option.edge);
        if (stored == edge) item.Icon(MakeIcon(L"\xE73E"));
        AddClick(item, [app, edge](wf::IInspectable const&, wux::RoutedEventArgs const&) {
            QueueAppPositionChange(app, edge);
        });
        submenu.Items().Append(item);
    }

    submenu.Items().Append(wuxc::MenuFlyoutSeparator{});
    wuxc::MenuFlyoutItem excludeItem;
    excludeItem.Text(settingsExcluded ? L"Blacklisted in Windhawk settings"
                     : excluded ? L"Stop ignoring this app"
                                : L"Ignore this app (blacklist)");
    excludeItem.IsEnabled(!settingsExcluded);
    if (excluded) excludeItem.Icon(MakeIcon(L"\xE73E"));
    AddClick(excludeItem, [app, excluded](wf::IInspectable const&,
                                          wux::RoutedEventArgs const&) {
        SetAppExcluded(app, !excluded);
    });
    submenu.Items().Append(excludeItem);
}

void AppendInjectedItems(void* vectorThis) {
    if (!g_runtimeHookReady.load()) {
        Wh_Log(L"Taskbar position submenu disabled: runtime location hooks unavailable");
        return;
    }
    std::erase_if(g_injectedItems, [](InjectedItem const& entry) { return !entry.item.get(); });
    g_currentMenuInjected = true;

    wuxc::MenuFlyoutSubItem submenu;
    submenu.Name(kItemName);
    submenu.Tag(winrt::box_value(winrt::hstring{kItemName}));
    submenu.Text(L"Taskbar position");
    submenu.Icon(MakeIcon(L"\xE90E"));

    std::wstring app = g_menuTargetApp ? *g_menuTargetApp : L"";
    AppendQuickPositionOptions(submenu);

    if (!app.empty()) {
        submenu.Items().Append(wuxc::MenuFlyoutSeparator{});
        wuxc::MenuFlyoutSubItem appSubmenu;
        appSubmenu.Text(winrt::hstring{app});
        appSubmenu.Icon(MakeIcon(L"\xE8B7"));
        AppendAppOptions(appSubmenu, app);
        submenu.Items().Append(appSubmenu);
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
    std::wstring app;

    ScopedTaskbarSettingsMenuBuild() {
        outer = g_taskbarSettingsMenuDepth++ == 0;
        if (outer) {
            g_currentMenuInjected = false;
            UpdateForegroundApp(GetForegroundWindow());
            app = g_maximizedOnly.load()
                      ? TopMaximizedAppOnForegroundMonitor() : LastForegroundApp();
            g_menuTargetApp = &app;
        }
    }

    ~ScopedTaskbarSettingsMenuBuild() {
        if (outer && !g_currentMenuInjected) {
            Wh_Log(L"No taskbar context menu append observed");
        }
        g_taskbarSettingsMenuDepth--;
        if (outer) {
            g_menuTargetApp = nullptr;
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
    return module;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    LogModuleInfo(module, L"Resolving taskbar symbols");

    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
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

    if (!HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"Skipping injection: missing symbols");
        return false;
    }

    Wh_Log(L"Hook installed: ContextMenus::ShowTaskbarSettingsContextMenu");
    Wh_Log(L"Hook installed: IVector<MenuFlyoutItemBase>::Append");

    return true;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void*);
struct UiThreadRequest { RunFromWindowThreadProc_t callback; void* data; };
UINT UiThreadMessage() {
    static UINT message = RegisterWindowMessageW(L"Windhawk_TaskbarPositionShortcut_UI");
    return message;
}
LRESULT CALLBACK UiThreadHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        auto* info = reinterpret_cast<CWPSTRUCT*>(lParam);
        if (info->message == UiThreadMessage()) {
            auto* request = reinterpret_cast<UiThreadRequest*>(info->lParam);
            request->callback(request->data);
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}
bool RunFromWindowThread(HWND window, RunFromWindowThreadProc_t callback, void* data) {
    DWORD thread = GetWindowThreadProcessId(window, nullptr);
    if (!thread) return false;
    if (thread == GetCurrentThreadId()) { callback(data); return true; }
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, UiThreadHook, nullptr, thread);
    if (!hook) return false;
    UiThreadRequest request{callback, data};
    SendMessageW(window, UiThreadMessage(), 0, reinterpret_cast<LPARAM>(&request));
    UnhookWindowsHookEx(hook);
    return true;
}
void WINAPI RevokeClickHandlers(void*) {
    for (auto const& entry : g_injectedItems) {
        try { if (auto item = entry.item.get()) item.Click(entry.token); } catch (...) {}
    }
    g_injectedItems.clear();
}

// Menu modules can load after Windhawk initialization. The original
// taskbar-restart-explorer mod handles that case with a LoadLibraryExW hook.
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original = nullptr;
std::atomic<bool> g_afterInit{false};
std::atomic<bool> g_monitorStarting{false};
std::atomic<bool> g_menuHookInstalling{false};

void StartMonitorWhenReady() {
    if (!g_afterInit.load() || !g_supported.load() || !g_hostProcess.load() ||
        !g_runtimeHookReady.load() || !g_taskbarViewModuleHooked.load() ||
        g_unloading.load()) {
        return;
    }
    if (g_monitorStarting.exchange(true)) return;
    if (g_monitorThread || g_unloading.load()) {
        g_monitorStarting = false;
        return;
    }

    if (!g_stopEvent) g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_wakeEvent) g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (g_stopEvent && g_wakeEvent && !g_unloading.load()) {
        g_monitorThread = CreateThread(nullptr, 0, MonitorThreadProc,
                                       nullptr, 0, nullptr);
        if (!g_monitorThread) Wh_Log(L"Monitor thread failed to start: %lu", GetLastError());
    } else {
        Wh_Log(L"Monitor events could not be created");
    }
    g_monitorStarting = false;
}

void TryInstallMenuHooks(HMODULE module, bool applyHooks) {
    if (!module || g_unloading.load() ||
        g_menuHookAttempted.load() || g_menuHookInstalling.exchange(true)) {
        return;
    }
    if (g_menuHookAttempted.exchange(true)) {
        g_menuHookInstalling = false;
        return;
    }
    if (HookTaskbarViewDllSymbols(module)) {
        g_taskbarViewModuleHooked = true;
        if (applyHooks) Wh_ApplyHookOperations();
        Wh_Log(L"Taskbar menu hooks ready");
    } else {
        Wh_Log(L"Taskbar menu symbols unavailable for this loaded module");
    }
    g_menuHookInstalling = false;
    StartMonitorWhenReady();
}

void ActivateTaskbarHost() {
    if (g_unloading.load()) return;
    g_taskbarThreadId = GetCurrentThreadId();
    bool wasHost = g_hostProcess.exchange(true);
    if (!wasHost) Wh_Log(L"Taskbar host discovered on thread %lu", GetCurrentThreadId());
    StartMonitorWhenReady();
}

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);
    if (module && module == GetTaskbarViewModuleHandle()) {
        TryInstallMenuHooks(module, true);
    }
    return module;
}

BOOL Wh_ModInit() {
    DWORD build = GetWindowsBuild();
    if (!build || build < 22000) {
        Wh_Log(L"Windows 11 is required; detected build %lu", build);
        return TRUE;
    }

    DWORD actual = ABE_BOTTOM;
    if (!TryReadTaskbarEdge(&actual)) {
        // A registry read failure is not sufficient to reject the native
        // feature. Keep menu injection independent of the value's existence.
        Wh_Log(L"TaskbarLocation could not be read; assuming Bottom until available");
    }
    g_supported = true;
    g_windowsDefaultEdge.store(actual);
    LoadAppRules();

    // Do not assume Shell_TrayWnd already exists at injection time.
    HWND taskbar = FindCurrentProcessTaskbarWnd();
    if (taskbar) {
        g_hostProcess = true;
        g_taskbarThreadId = GetWindowThreadProcessId(taskbar, nullptr);
    }
    // No early return: the taskbar window is normally created AFTER ModInit.
    // TrayUI initialization discovers the hosting Explorer process later.

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto regGetValue = kernelBase ? reinterpret_cast<RegGetValueW_t>(
        GetProcAddress(kernelBase, "RegGetValueW")) : nullptr;
    if (regGetValue) {
        if (!WindhawkUtils::SetFunctionHook(regGetValue, RegGetValueW_Hook,
                                            &RegGetValueW_Original)) {
            Wh_Log(L"RegGetValueW hook failed");
        }
    }

    // The runtime location symbol belongs to taskbar.dll, not Taskbar.View.dll.
    HMODULE taskbarDll = LoadLibraryExW(L"taskbar.dll", nullptr,
                                         LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (taskbarDll) {
        WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
            {
                {LR"(public: virtual __int64 __cdecl TrayUI::WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,bool *))"},
                &TrayUI_WndProc_Original, TrayUI_WndProc_Hook,
            },
            {
                {LR"(public: enum winrt::WindowsUdk::UI::Shell::TaskbarLocation __cdecl winrt::WindowsUdk::UI::Shell::implementation::TaskbarSettings::Location(void))"},
                &TaskbarSettings_Location_Original, TaskbarSettings_Location_Hook,
            },
            {
                {LR"(public: void __cdecl TrayUI::_GetSaveStateAndInitRects(void))"},
                &TrayUI_GetSaveStateAndInitRects_Original,
                TrayUI_GetSaveStateAndInitRects_Hook, true,
            },
            {
                {LR"(private: long __cdecl CSecondaryTray::_LoadSettings(void))"},
                &CSecondaryTray_LoadSettings_Original,
                CSecondaryTray_LoadSettings_Hook, true,
            },
        };
        bool symbolsResolved = HookSymbols(taskbarDll, taskbarDllHooks,
            ARRAYSIZE(taskbarDllHooks));
        g_runtimeHookReady = symbolsResolved && !!RegGetValueW_Original &&
            !!TaskbarSettings_Location_Original && !!TrayUI_WndProc_Original;
        Wh_Log(L"Runtime hook details: symbols=%d, registry=%d, location=%d, wndproc=%d",
               symbolsResolved, !!RegGetValueW_Original,
               !!TaskbarSettings_Location_Original, !!TrayUI_WndProc_Original);
    } else {
        Wh_Log(L"taskbar.dll failed to load: %lu", GetLastError());
    }
    Wh_Log(L"Runtime position hooks: %s", g_runtimeHookReady.load() ? L"ready" : L"unavailable");

    // Install immediately when possible, AND watch for later module loading.
    TryInstallMenuHooks(GetTaskbarViewModuleHandle(), false);
    if (kernelBase) {
        auto loadLibraryExW = reinterpret_cast<LoadLibraryExW_t>(
            GetProcAddress(kernelBase, "LoadLibraryExW"));
        if (loadLibraryExW && !WindhawkUtils::SetFunctionHook(
            loadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original)) {
            Wh_Log(L"LoadLibraryExW hook failed; deferred menu loading unavailable");
        }
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    g_afterInit = true;
    // Handle a module loaded between Wh_ModInit and Wh_ModAfterInit.
    TryInstallMenuHooks(GetTaskbarViewModuleHandle(), true);
    if (!g_hostProcess.load()) {
        HWND taskbar = FindCurrentProcessTaskbarWnd();
        if (taskbar) {
            g_hostProcess = true;
            g_taskbarThreadId = GetWindowThreadProcessId(taskbar, nullptr);
        }
    }
    StartMonitorWhenReady();
    if (!g_menuHookAttempted.load()) {
        Wh_Log(L"Waiting for Taskbar.View.dll to load");
    }
}

void Wh_ModSettingsChanged() {
    if (!g_unloading.load()) {
        LoadAppRules();
    }
}

void Wh_ModUninit() {
    g_unloading = true;
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_monitorThread) {
        WaitForSingleObject(g_monitorThread, INFINITE);
        CloseHandle(g_monitorThread);
        g_monitorThread = nullptr;
    }
    if (g_hostProcess.load()) {
        HWND taskbar = FindCurrentProcessTaskbarWnd();
        if (taskbar) {
            if (!RunFromWindowThread(taskbar, RevokeClickHandlers, nullptr)) {
                Wh_Log(L"FATAL: Unable to revoke XAML click delegates");
            }
            DWORD real = ABE_BOTTOM;
            if (TryReadTaskbarEdge(&real)) {
                g_effectiveEdge.store(real);
                DWORD_PTR result = 0;
                SendMessageTimeoutW(taskbar, kTaskbarLocationMessage, 6, real,
                    SMTO_ABORTIFHUNG | SMTO_BLOCK, 1500, &result);
                SendMessageTimeoutW(taskbar, WM_SETTINGCHANGE, 0,
                    reinterpret_cast<LPARAM>(L"TraySettings"),
                    SMTO_ABORTIFHUNG | SMTO_BLOCK, 1500, &result);
            }
        }
    }
    if (g_wakeEvent) CloseHandle(g_wakeEvent);
    if (g_stopEvent) CloseHandle(g_stopEvent);
    Wh_Log(L"Uninit");
}
