// ==WindhawkMod==
// @id              win10-taskbar-search-state-refresh-on-win11-24h2
// @name            Win10 Taskbar Search State Refresh on Win11 24H2+
// @description     This mod makes the Windows 10 taskbar search box/icon running on Windows 11 24H2+ follow the Search setting immediately, retrying and verifying until it is applied
// @version         1.0.0
// @include         explorer.exe
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @architecture    x86-64
// @compilerOptions -lversion
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Win10 Taskbar Search State Refresh

This mod addresses an issue where the legacy Windows 10 taskbar, running inside a Windows 11
system, does not always redraw when the taskbar Search setting (hidden / icon / box) is changed.
It monitors the setting and triggers the taskbar to apply it immediately. This modification has
been tested on Windows 11 24H2.

Note: for a complete Windows 10 taskbar experience on Windows 11 24H2 / 25H2, install the
[Windows 10 taskbar on Windows 11 24H2](https://windhawk.net/mods/win10-taskbar-on-win11-24h2)
mod first. That mod downloads and runs the Windows 10 explorer.exe (10.0.19039) as the shell;
this mod only works on top of that shell and does nothing on the stock Windows 11 taskbar.

## How it works
- It monitors `HKCU\Software\Microsoft\Windows\CurrentVersion\Search\SearchboxTaskbarMode`
  (0 = hidden, 1 = icon, 2 = box, 3 = icon and label) via a registry notification, with a
  one-second poll as a fallback in case a notification is missed.
- When the value changes, it sends `WM_SETTINGCHANGE` with `"TraySettings"` to `Shell_TrayWnd`
  and `Shell_SecondaryTrayWnd` of this process. This is the single method that reliably makes
  the Windows 10 taskbar reload its settings, including the search mode, on shell build
  10.0.19039. The message is sent twice with a short gap so a momentarily busy explorer still
  catches it.
- The message is never broadcast system-wide: only the taskbar windows of this process are
  targeted. No fake `SPI_SETWORKAREA` and no UI Automation are used.
- The startup refresh is skipped on a normal explorer start (the taskbar reads the value itself
  when it is created) and only runs when the mod is enabled while the taskbar is already running.
- Every refresh is written to the Windhawk log.

## "Search icon and label" (approximation)
The Windows 10 taskbar only supports three states: hidden, icon, and box. "Search icon and label"
(value 3) is a Windows 11 option that the Windows 10 taskbar does not implement, so by default
selecting it changes nothing there. As the closest equivalent, this mod makes the taskbar interpret
value 3 as 2 (search box). This applies only to reads performed inside explorer.exe: the registry
value itself is never modified, and the Windows 11 Settings page continues to show the user's
choice. It is an approximation, not a true "icon and label" layout. It can be disabled with
`labelAsBox`.

## Settings
- `labelAsBox`: treat "Search icon and label" as the search box (see above).

## Limitations
- This mod never restarts explorer and never writes to the registry. The "icon and label" mapping is
  performed by adjusting the value as returned to explorer.exe (RegQueryValueExW / RegGetValueW);
  if the taskbar reads the value by other means, the mapping will not take effect.
- The mod does nothing on the stock Windows 11 shell (build 22000+); it exits at load time.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- labelAsBox: true
  $name: Treat "icon and label" as search box
  $description: This setting controls whether "Search icon and label" is treated as the search box. The Windows 10 taskbar has no "Search icon and label" mode, so when on, that choice is shown as the search box (an approximation). The registry value is not modified.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winver.h>

#include <algorithm>
#include <atomic>
#include <cstring>
#include <cwchar>
#include <string>
#include <vector>

namespace {

constexpr wchar_t kSearchKeyPath[] = L"Software\\Microsoft\\Windows\\CurrentVersion\\Search";
constexpr wchar_t kModeValueName[] = L"SearchboxTaskbarMode";
constexpr DWORD kFirstWindows11Build = 22000;

std::atomic<bool> g_stopping{false};
std::atomic<bool> g_labelAsBox{true};
std::atomic<int> g_remapLogCount{0};
HANDLE g_stopEvent = nullptr;
HANDLE g_resyncEvent = nullptr;
HANDLE g_worker = nullptr;
bool g_taskbarExistedAtInit = false;

using RegQueryValueExW_t = LONG (WINAPI*)(HKEY, LPCWSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
using RegGetValueW_t = LSTATUS (WINAPI*)(HKEY, LPCWSTR, LPCWSTR, DWORD, LPDWORD, PVOID, LPDWORD);
RegQueryValueExW_t g_origRegQueryValueExW = nullptr;
RegGetValueW_t g_origRegGetValueW = nullptr;

void RemapLabelMode(LPCWSTR valueName, DWORD type, void* data, DWORD size) {
    if (!data || size != sizeof(DWORD) || type != REG_DWORD || !valueName) return;
    if (!g_labelAsBox.load() || _wcsicmp(valueName, kModeValueName) != 0) return;
    DWORD value = 0;
    memcpy(&value, data, sizeof(value));
    if (value != 3) return;
    value = 2;
    memcpy(data, &value, sizeof(value));
    if (g_remapLogCount.fetch_add(1) < 5)
        Wh_Log(L"[SearchRefresh] explorer read SearchboxTaskbarMode=3; reported as 2 (search box)");
}

LONG WINAPI RegQueryValueExW_Hook(HKEY key, LPCWSTR name, LPDWORD reserved, LPDWORD type,
                                  LPBYTE data, LPDWORD size) {
    const LONG result = g_origRegQueryValueExW(key, name, reserved, type, data, size);
    if (result == ERROR_SUCCESS && data && size)
        RemapLabelMode(name, type ? *type : REG_DWORD, data, *size);
    return result;
}

LSTATUS WINAPI RegGetValueW_Hook(HKEY key, LPCWSTR subKey, LPCWSTR value, DWORD flags,
                                 LPDWORD type, PVOID data, LPDWORD size) {
    const LSTATUS result = g_origRegGetValueW(key, subKey, value, flags, type, data, size);
    if (result == ERROR_SUCCESS && data && size)
        RemapLabelMode(value, type ? *type : REG_DWORD, data, *size);
    return result;
}

int ReadSearchMode() {
    DWORD value = 0, size = sizeof(value);
    const RegGetValueW_t getValue = g_origRegGetValueW ? g_origRegGetValueW : RegGetValueW;
    const LONG result = getValue(HKEY_CURRENT_USER, kSearchKeyPath, kModeValueName,
                                 RRF_RT_REG_DWORD, nullptr, &value, &size);
    return result == ERROR_SUCCESS ? static_cast<int>(value) : -1;
}

BOOL CALLBACK CollectTrayProc(HWND hwnd, LPARAM lParam) {
    wchar_t className[64]{};
    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className))) return TRUE;
    if (wcscmp(className, L"Shell_TrayWnd") != 0 && wcscmp(className, L"Shell_SecondaryTrayWnd") != 0)
        return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId())
        reinterpret_cast<std::vector<HWND>*>(lParam)->push_back(hwnd);
    return TRUE;
}

std::vector<HWND> FindTrays() {
    std::vector<HWND> trays;
    EnumWindows(CollectTrayProc, reinterpret_cast<LPARAM>(&trays));
    return trays;
}

void RefreshTaskbar() {
    const std::vector<HWND> trays = FindTrays();
    if (trays.empty()) {
        Wh_Log(L"[SearchRefresh] refresh: no taskbar window found in this process");
        return;
    }
    for (HWND tray : trays) {
        DWORD_PTR result = 0;
        SendMessageTimeoutW(tray, WM_SETTINGCHANGE, 0,
                            reinterpret_cast<LPARAM>(L"TraySettings"),
                            SMTO_ABORTIFHUNG, 2000, &result);
    }
    Wh_Log(L"[SearchRefresh] refresh sent to %d taskbar window(s)",
           static_cast<int>(trays.size()));
}

void ApplyRefresh(const wchar_t* reason) {
    Wh_Log(L"[SearchRefresh] %s: refreshing taskbar", reason);
    RefreshTaskbar();
    if (WaitForSingleObject(g_stopEvent, 300) == WAIT_OBJECT_0) return;
    RefreshTaskbar();
}

DWORD WINAPI WorkerProc(void*) {
    HANDLE notifyEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    HKEY key = nullptr;
    auto arm = [&]() {
        if (!notifyEvent) return;
        if (!key && RegOpenKeyExW(HKEY_CURRENT_USER, kSearchKeyPath, 0,
                                  KEY_NOTIFY | KEY_QUERY_VALUE, &key) != ERROR_SUCCESS)
            key = nullptr;
        if (key && RegNotifyChangeKeyValue(key, FALSE, REG_NOTIFY_CHANGE_LAST_SET,
                                           notifyEvent, TRUE) != ERROR_SUCCESS) {
            RegCloseKey(key);
            key = nullptr;
        }
    };

    if (WaitForSingleObject(g_stopEvent, 2000) != WAIT_OBJECT_0) {
        int lastMode = ReadSearchMode();
        if (g_taskbarExistedAtInit) {
            ApplyRefresh(L"startup");
        } else {
            Wh_Log(L"[SearchRefresh] startup: taskbar not yet running, skipping initial refresh");
        }
        arm();
        HANDLE handles[3] = {g_stopEvent, g_resyncEvent, notifyEvent};
        const DWORD count = notifyEvent ? 3 : 2;
        while (!g_stopping.load()) {
            const DWORD wait = WaitForMultipleObjects(count, handles, FALSE, 1000);
            if (wait == WAIT_OBJECT_0) break;
            if (wait == WAIT_OBJECT_0 + 1) {
                ApplyRefresh(L"settings changed");
            } else if (wait == WAIT_OBJECT_0 + 2) {
                if (WaitForSingleObject(g_stopEvent, 150) == WAIT_OBJECT_0) break;
                arm();
            } else if (!key) {
                arm();
            }
            const int mode = ReadSearchMode();
            if (mode != lastMode) {
                Wh_Log(L"[SearchRefresh] SearchboxTaskbarMode changed: %d -> %d", lastMode, mode);
                lastMode = mode;
                ApplyRefresh(L"setting changed");
            }
        }
    }
    if (key) RegCloseKey(key);
    if (notifyEvent) CloseHandle(notifyEvent);
    return 0;
}

bool IsMainExplorerProcess() {
    HWND shell = GetShellWindow();
    if (shell) {
        DWORD shellPid = 0;
        GetWindowThreadProcessId(shell, &shellPid);
        return shellPid == GetCurrentProcessId();
    }
    std::wstring command = GetCommandLineW();
    std::transform(command.begin(), command.end(), command.begin(),
                   [](wchar_t c) { return static_cast<wchar_t>(towlower(c)); });
    return command.find(L" /factory") == std::wstring::npos &&
           command.find(L" /separate") == std::wstring::npos &&
           command.find(L" -embedding") == std::wstring::npos;
}

bool IsWindows10TaskbarShell() {
    const HMODULE module = GetModuleHandleW(nullptr);
    if (!module) return false;
    wchar_t path[MAX_PATH]{};
    if (!GetModuleFileNameW(module, path, ARRAYSIZE(path))) return false;
    DWORD handle = 0;
    const DWORD size = GetFileVersionInfoSizeW(path, &handle);
    if (!size) return false;
    std::vector<BYTE> data(size);
    if (!GetFileVersionInfoW(path, 0, size, data.data())) return false;
    VS_FIXEDFILEINFO* info = nullptr;
    UINT len = 0;
    if (!VerQueryValueW(data.data(), L"\\", reinterpret_cast<LPVOID*>(&info), &len) ||
        !info || len < sizeof(VS_FIXEDFILEINFO)) {
        return false;
    }
    const DWORD build = HIWORD(info->dwFileVersionLS);
    Wh_Log(L"[SearchRefresh] main module build = %u", build);
    return build < kFirstWindows11Build;
}

void LoadSettings() {
    g_labelAsBox = Wh_GetIntSetting(L"labelAsBox") != 0;
}

}  // namespace

BOOL Wh_ModInit() {
    if (!IsMainExplorerProcess()) {
        Wh_Log(L"[SearchRefresh] not the Explorer shell process; skipping this instance");
        return FALSE;
    }
    if (!IsWindows10TaskbarShell()) {
        Wh_Log(L"[SearchRefresh] main module is not the Windows 10 shell (build >= 22000); "
               L"install the Windows 10 taskbar mod first, this mod does nothing here");
        return FALSE;
    }
    LoadSettings();
    g_stopping = false;
    g_taskbarExistedAtInit = !FindTrays().empty();
    if (g_taskbarExistedAtInit)
        Wh_Log(L"[SearchRefresh] taskbar window already present at init; startup refresh enabled");
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_resyncEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_resyncEvent) {
        if (g_stopEvent) { CloseHandle(g_stopEvent); g_stopEvent = nullptr; }
        if (g_resyncEvent) { CloseHandle(g_resyncEvent); g_resyncEvent = nullptr; }
        return FALSE;
    }
    if (HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll")) {
        void* queryValue = reinterpret_cast<void*>(GetProcAddress(kernelBase, "RegQueryValueExW"));
        void* getValue = reinterpret_cast<void*>(GetProcAddress(kernelBase, "RegGetValueW"));
        if (queryValue)
            Wh_SetFunctionHook(queryValue, reinterpret_cast<void*>(RegQueryValueExW_Hook),
                               reinterpret_cast<void**>(&g_origRegQueryValueExW));
        if (getValue)
            Wh_SetFunctionHook(getValue, reinterpret_cast<void*>(RegGetValueW_Hook),
                               reinterpret_cast<void**>(&g_origRegGetValueW));
    }
    g_worker = CreateThread(nullptr, 0, WorkerProc, nullptr, 0, nullptr);
    if (!g_worker) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        CloseHandle(g_resyncEvent);
        g_resyncEvent = nullptr;
        return FALSE;
    }
    Wh_Log(L"[SearchRefresh] watching SearchboxTaskbarMode");
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_resyncEvent) SetEvent(g_resyncEvent);
}

void Wh_ModUninit() {
    g_stopping = true;
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_worker) {
        WaitForSingleObject(g_worker, INFINITE);
        CloseHandle(g_worker);
        g_worker = nullptr;
    }
    if (g_stopEvent) { CloseHandle(g_stopEvent); g_stopEvent = nullptr; }
    if (g_resyncEvent) { CloseHandle(g_resyncEvent); g_resyncEvent = nullptr; }
}
