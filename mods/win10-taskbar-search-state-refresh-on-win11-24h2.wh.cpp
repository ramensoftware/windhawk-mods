// ==WindhawkMod==
// @id              win10-taskbar-search-state-refresh-on-win11-24h2
// @name            Win10 Taskbar Search State Refresh on Win11 24H2+
// @description     This mod makes the Windows 10 taskbar search box/icon running on Windows 11 24H2+ follow the Search setting immediately, retrying and verifying until it is applied
// @version         1.0.0
// @include         explorer.exe
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -luuid
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Win10 Taskbar Search State Refresh

This mod tries to address an issue where the legacy Windows 10 taskbar, running inside a Windows 11
system, does not always redraw when the taskbar Search setting (hidden / icon / box) is changed.
It monitors the setting and triggers the taskbar to apply it, verifying the result and retrying
with progressively stronger methods. This modification has been tested on Windows 11 24H2.

Note: This mod is best-effort because the message the Windows 10 taskbar actually responds to may differ between
builds, which is why each attempt and its measured result are logged.

## How it works
- It monitors `HKCU\Software\Microsoft\Windows\CurrentVersion\Search\SearchboxTaskbarMode`
  (0 = hidden, 1 = icon, 2 = box, 3 = icon and label) via a registry notification, with a
  one-second poll as a fallback in case a notification is missed.
- When the value changes (and once shortly after startup), up to N attempts are performed. Each
  attempt applies a stronger method, then waits and checks the taskbar:
  1. `WM_SETTINGCHANGE` ("TraySettings") sent to the taskbar window(s).
  2. The same message broadcast to all top-level windows and sent to every child window of the taskbar.
  3. A work-area setting change, a frame refresh, and a full redraw of the taskbar.
  4. Steps 1 and 2 repeated after a longer delay (explorer may have been busy).
- Verification uses UI Automation: the search element (AutomationId `4103`) must be missing or
  off-screen for "hidden", approximately as wide as the taskbar is tall for "icon", and wider for
  "box". Vertical taskbars cannot be verified, so only two attempts are made there.
- Every step and measurement is written to the Windhawk log with the prefix `[SearchRefresh]`.

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
- `verify`: check the taskbar after each attempt and stop as soon as it matches. When off, only two attempts are made.
- `maxAttempts`: maximum number of attempts per change (1-8).

## Limitations
- This mod never restarts explorer and never writes to the registry. The "icon and label" mapping is
  performed by adjusting the value as returned to explorer.exe (RegQueryValueExW / RegGetValueW);
  if the taskbar reads the value by other means, the mapping will not take effect.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- labelAsBox: true
  $name: Treat "icon and label" as search box
  $description: This setting controls whether "Search icon and label" is treated as the search box. The Windows 10 taskbar has no "Search icon and label" mode, so when on, that choice is shown as the search box (an approximation). The registry value is not modified.
- verify: true
  $name: Verify and retry
  $description: This setting controls whether the taskbar is checked after each attempt and the process stops as soon as it matches the setting. When off, only two attempts are made.
- maxAttempts: 4
  $name: Maximum attempts
  $description: This setting controls the maximum number of refresh attempts per change (1-8).
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <uiautomation.h>
#include <wrl/client.h>

#include <algorithm>
#include <atomic>
#include <cstring>
#include <cwchar>
#include <string>
#include <vector>

using Microsoft::WRL::ComPtr;

namespace {

constexpr wchar_t kSearchKeyPath[] = L"Software\\Microsoft\\Windows\\CurrentVersion\\Search";
constexpr wchar_t kModeValueName[] = L"SearchboxTaskbarMode";
constexpr wchar_t kSearchAutomationId[] = L"4103";

std::atomic<bool> g_stopping{false};
std::atomic<bool> g_verify{true};
std::atomic<int> g_maxAttempts{4};
std::atomic<bool> g_labelAsBox{true};
std::atomic<int> g_remapLogCount{0};
HANDLE g_stopEvent = nullptr;
HANDLE g_resyncEvent = nullptr;
HANDLE g_worker = nullptr;

enum class SearchVisual { Unknown, Hidden, Icon, Box };

const wchar_t* VisualName(SearchVisual visual) {
    switch (visual) {
        case SearchVisual::Hidden: return L"hidden";
        case SearchVisual::Icon: return L"icon";
        case SearchVisual::Box: return L"box";
        default: return L"unknown";
    }
}

using RegQueryValueExW_t = LONG (WINAPI*)(HKEY, LPCWSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
using RegGetValueW_t = LSTATUS (WINAPI*)(HKEY, LPCWSTR, LPCWSTR, DWORD, LPDWORD, PVOID, LPDWORD);
RegQueryValueExW_t g_origRegQueryValueExW = nullptr;
RegGetValueW_t g_origRegGetValueW = nullptr;

// Reports "icon and label" (3) as "box" (2) to explorer. The registry itself is untouched.
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

// Reads the real value (bypassing our own remapping hook).
int ReadSearchMode() {
    DWORD value = 0, size = sizeof(value);
    const RegGetValueW_t getValue = g_origRegGetValueW ? g_origRegGetValueW : RegGetValueW;
    const LONG result = getValue(HKEY_CURRENT_USER, kSearchKeyPath, kModeValueName,
                                 RRF_RT_REG_DWORD, nullptr, &value, &size);
    return result == ERROR_SUCCESS ? static_cast<int>(value) : -1;
}

// What the Windows 10 taskbar can actually show for a registry value.
int EffectiveMode(int mode) {
    return (mode == 3 && g_labelAsBox.load()) ? 2 : mode;
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

BOOL CALLBACK CollectChildProc(HWND hwnd, LPARAM lParam) {
    reinterpret_cast<std::vector<HWND>*>(lParam)->push_back(hwnd);
    return TRUE;
}

void SendSettingChange(HWND hwnd, WPARAM wParam, const wchar_t* area) {
    DWORD_PTR result = 0;
    SendMessageTimeoutW(hwnd, WM_SETTINGCHANGE, wParam, reinterpret_cast<LPARAM>(area),
                        SMTO_ABORTIFHUNG, 2000, &result);
}

void RunRefreshStep(int step) {
    const std::vector<HWND> trays = FindTrays();
    if (trays.empty()) {
        Wh_Log(L"[SearchRefresh] step %d: no taskbar window found in this process", step + 1);
        return;
    }
    switch (step) {
    case 0:
        for (HWND tray : trays) SendSettingChange(tray, 0, L"TraySettings");
        break;
    case 1:
        SendSettingChange(HWND_BROADCAST, 0, L"TraySettings");
        for (HWND tray : trays) {
            std::vector<HWND> children;
            EnumChildWindows(tray, CollectChildProc, reinterpret_cast<LPARAM>(&children));
            for (HWND child : children) SendSettingChange(child, 0, L"TraySettings");
        }
        break;
    case 2:
        for (HWND tray : trays) {
            SendSettingChange(tray, SPI_SETWORKAREA, nullptr);
            SetWindowPos(tray, nullptr, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                SWP_FRAMECHANGED | SWP_ASYNCWINDOWPOS);
            RedrawWindow(tray, nullptr, nullptr,
                RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN);
        }
        break;
    default:
        RunRefreshStep(0);
        RunRefreshStep(1);
        return;
    }
    Wh_Log(L"[SearchRefresh] step %d sent to %d taskbar window(s)", step + 1, static_cast<int>(trays.size()));
}

SearchVisual MeasureSearchVisual(IUIAutomation* uia) {
    const std::vector<HWND> trays = FindTrays();
    HWND primary = nullptr;
    for (HWND tray : trays) {
        wchar_t className[64]{};
        if (GetClassNameW(tray, className, ARRAYSIZE(className)) &&
            wcscmp(className, L"Shell_TrayWnd") == 0) { primary = tray; break; }
    }
    if (!primary) return SearchVisual::Unknown;

    ComPtr<IUIAutomationElement> root;
    if (FAILED(uia->ElementFromHandle(primary, root.GetAddressOf())) || !root)
        return SearchVisual::Unknown;
    RECT rootRect{};
    if (FAILED(root->get_CurrentBoundingRectangle(&rootRect))) return SearchVisual::Unknown;
    const int rootWidth = rootRect.right - rootRect.left;
    const int rootHeight = rootRect.bottom - rootRect.top;
    if (rootWidth <= 0 || rootHeight <= 0 || rootHeight > rootWidth)
        return SearchVisual::Unknown;  // vertical or not yet laid out

    VARIANT value;
    VariantInit(&value);
    value.vt = VT_BSTR;
    value.bstrVal = SysAllocString(kSearchAutomationId);
    ComPtr<IUIAutomationCondition> condition;
    const HRESULT conditionHr = uia->CreatePropertyCondition(
        UIA_AutomationIdPropertyId, value, condition.GetAddressOf());
    VariantClear(&value);
    if (FAILED(conditionHr) || !condition) return SearchVisual::Unknown;

    ComPtr<IUIAutomationElement> search;
    if (FAILED(root->FindFirst(TreeScope_Descendants, condition.Get(), search.GetAddressOf())))
        return SearchVisual::Unknown;
    if (!search) {
        Wh_Log(L"[SearchRefresh] measured: search element not present");
        return SearchVisual::Hidden;
    }
    BOOL offscreen = FALSE;
    search->get_CurrentIsOffscreen(&offscreen);
    RECT rect{};
    search->get_CurrentBoundingRectangle(&rect);
    const int width = rect.right - rect.left;
    Wh_Log(L"[SearchRefresh] measured: element width=%d, taskbar thickness=%d, offscreen=%d",
           width, rootHeight, static_cast<int>(offscreen));
    if (offscreen || width <= 0) return SearchVisual::Hidden;
    return width <= rootHeight * 2 ? SearchVisual::Icon : SearchVisual::Box;
}

void ApplyRefresh(int mode, const wchar_t* reason) {
    const int effective = EffectiveMode(mode);
    const SearchVisual expected =
        effective == 0 ? SearchVisual::Hidden :
        effective == 1 ? SearchVisual::Icon :
        effective == 2 ? SearchVisual::Box : SearchVisual::Unknown;

    ComPtr<IUIAutomation> uia;
    if (g_verify.load()) {
        CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                         IID_PPV_ARGS(uia.GetAddressOf()));
    }
    const bool canVerify = uia && expected != SearchVisual::Unknown;
    const int maxAttempts = std::clamp(g_maxAttempts.load(), 1, 8);
    const int limit = canVerify ? maxAttempts : (std::min)(2, maxAttempts);
    Wh_Log(L"[SearchRefresh] %s: mode=%d (effective %d, expected %s), verification %s, up to %d attempt(s)",
           reason, mode, effective, VisualName(expected), canVerify ? L"on" : L"off", limit);

    if (canVerify && MeasureSearchVisual(uia.Get()) == expected) {
        Wh_Log(L"[SearchRefresh] taskbar already matches the setting");
        return;
    }

    for (int attempt = 0; attempt < limit && !g_stopping.load(); ++attempt) {
        RunRefreshStep(attempt % 4);
        if (WaitForSingleObject(g_stopEvent, 500 + 300 * attempt) == WAIT_OBJECT_0) return;
        if (!canVerify) continue;
        const SearchVisual actual = MeasureSearchVisual(uia.Get());
        Wh_Log(L"[SearchRefresh] attempt %d: taskbar shows %s", attempt + 1, VisualName(actual));
        if (actual == expected) {
            Wh_Log(L"[SearchRefresh] setting applied after %d attempt(s)", attempt + 1);
            return;
        }
    }
    if (canVerify && !g_stopping.load())
        Wh_Log(L"[SearchRefresh] setting NOT confirmed after %d attempt(s)", limit);
}

DWORD WINAPI WorkerProc(void*) {
    const HRESULT coHr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    {
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

        // Give the taskbar time to come up (and the hooks time to be applied) before the first read.
        if (WaitForSingleObject(g_stopEvent, 2000) != WAIT_OBJECT_0) {
            int lastMode = ReadSearchMode();
            ApplyRefresh(lastMode, L"startup");
            arm();
            HANDLE handles[3] = {g_stopEvent, g_resyncEvent, notifyEvent};
            const DWORD count = notifyEvent ? 3 : 2;
            while (!g_stopping.load()) {
                const DWORD wait = WaitForMultipleObjects(count, handles, FALSE, 1000);
                if (wait == WAIT_OBJECT_0) break;
                if (wait == WAIT_OBJECT_0 + 1) {
                    ApplyRefresh(lastMode, L"settings changed");
                } else if (wait == WAIT_OBJECT_0 + 2) {
                    if (WaitForSingleObject(g_stopEvent, 150) == WAIT_OBJECT_0) break;  // debounce
                    arm();
                } else if (!key) {
                    arm();  // key missing earlier: retry on the poll tick
                }
                const int mode = ReadSearchMode();
                if (mode != lastMode) {
                    Wh_Log(L"[SearchRefresh] SearchboxTaskbarMode changed: %d -> %d", lastMode, mode);
                    lastMode = mode;
                    ApplyRefresh(mode, L"setting changed");
                }
            }
        }
        if (key) RegCloseKey(key);
        if (notifyEvent) CloseHandle(notifyEvent);
    }
    if (SUCCEEDED(coHr)) CoUninitialize();
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

void LoadSettings() {
    g_verify = Wh_GetIntSetting(L"verify") != 0;
    g_maxAttempts = Wh_GetIntSetting(L"maxAttempts");
    g_labelAsBox = Wh_GetIntSetting(L"labelAsBox") != 0;
}

}  // namespace

BOOL Wh_ModInit() {
    if (!IsMainExplorerProcess()) {
        Wh_Log(L"[SearchRefresh] not the Explorer shell process; skipping this instance");
        return FALSE;
    }
    LoadSettings();
    g_stopping = false;
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
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_resyncEvent) {
        CloseHandle(g_resyncEvent);
        g_resyncEvent = nullptr;
    }
}
