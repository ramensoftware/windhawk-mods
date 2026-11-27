// ==WindhawkMod==
// @id              taskbar-disable-per-monitor
// @name            Disable taskbar per monitor
// @description     Turns the taskbar off for good on the chosen monitors: it never shows (no hover, Win key, notifications or flashing apps bring it back); taskbars on the other monitors stay as usual
// @version         1.0
// @author          eprilx
// @github          https://github.com/eprilx
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -ldwmapi
// @license         GPL-3.0-only
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// The work area hooks (CTray::RecomputeWorkArea + TrayUI::GetAutoHideFlags)
// follow the approach of "Taskbar auto-hide per monitor" by m417z (GPL-3.0):
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-auto-hide-per-monitor.wh.cpp

// ==WindhawkModReadme==
/*
# Disable taskbar per monitor

Pick one or more monitors. Their taskbar is disabled: it's invisible, can't be
clicked, and never comes back, whether from the mouse at the screen edge, the
Win key, Win+T / Win+number, flashing app buttons or notifications. Windows on
those monitors can use the space the taskbar took. Taskbars on every other
monitor are left alone. Works with the taskbar at the bottom, top, left or
right, with auto-hide on or off.

## Why not auto-hide

Auto-hide brings the taskbar back on hover, on the Win key and when an app
wants attention, and it applies to every monitor. **Taskbar auto-hide fine
tuning** can keep it from ever showing, but on all monitors at once, and
**Taskbar auto-hide per monitor** chooses auto-hide on or off per monitor but
doesn't stop the taskbar from showing. This mod turns the taskbar off on the
chosen monitors only and doesn't need or change the auto-hide setting.

## Choosing monitors

The **Monitors to disable the taskbar on** setting is a comma-separated list.
Each entry can be:

* `primary`: the primary monitor, wherever it is.
* A number (`1`, `2`, `3`...): the monitor's position in Windows' monitor
  enumeration. It may differ from the numbers shown in Display Settings, and it
  can change after plugging monitors in or out.
* Any other text: matched (case-insensitive) against part of the monitor's
  device name, model name or interface name, e.g. `GSM5BD2` or `DISPLAY2`.
  This stays correct when the numbering changes.

Examples: `primary` · `1,3` · `primary, GSM5BD2`. An empty list disables
nothing.

To see the number, device name, model and interface name of each monitor:
Advanced tab, set **Debug logging** to **Mod logs**, open **Show log output**,
then save the settings again. Every monitor is logged in a `Monitor` line.

## How it works

The taskbar window of a chosen monitor and its popups (tooltips, previews) are
cloaked with DWM, which makes them invisible and click-through without moving
them, and the work area of that monitor is computed as if its taskbar were
auto-hidden. Nothing is written to the system settings, so disabling or
removing the mod leaves no trace.

## Notes

* Windows 11. Tested on build 26300 with bottom and left taskbars, auto-hide
  on and off. "Show my taskbar on all displays" must be on if any of the chosen
  monitors is not the primary one.
* Keyboard focus can still move to a disabled taskbar (e.g. Win+T); press Esc
  to leave it.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- disableOn: "primary"
  $name: Monitors to disable the taskbar on
  $description: >-
    Comma-separated. Each entry: "primary", a monitor number (1, 2, 3...), or
    part of the monitor's device / model / interface name (see the mod's
    description for how to find them). Example: primary, 3
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <commctrl.h>
#include <dwmapi.h>
#include <shellapi.h>

#include <atomic>
#include <cwctype>
#include <mutex>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// State.

// true while unloading: every check reports "not disabled".
std::atomic<bool> g_unloading;

// Set around CTray::RecomputeWorkArea for a disabled monitor, so that
// TrayUI::GetAutoHideFlags (called from it, on the same thread) reports
// auto-hide and no space is reserved for the taskbar.
thread_local bool g_reportAutoHide;

// Serializes ApplyState calls (after init and on settings changes).
std::mutex g_applyMutex;

// Marks taskbar windows this mod has subclassed.
constexpr WCHAR kSubclassedProp[] = L"Windhawk_taskbar-disable-per-monitor";

// On managed windows: the cloak state explorer itself asked for (1 = not
// cloaked, 2 = cloaked), so it can be restored when the mod lets go.
constexpr WCHAR kExplorerCloakProp[] =
    L"Windhawk_taskbar-disable-per-monitor_explorer_cloak";

// On taskbar windows: the monitor the taskbar was last seen fully inside.
constexpr WCHAR kMonitorProp[] =
    L"Windhawk_taskbar-disable-per-monitor_monitor";

// On managed windows: present while the mod keeps the window cloaked.
constexpr WCHAR kModCloakProp[] =
    L"Windhawk_taskbar-disable-per-monitor_cloak";

////////////////////////////////////////////////////////////////////////////////
// Settings and monitor matching.

struct MonitorSelector {
    enum class Kind { Primary, Number, Name } kind;
    int number;
    std::wstring name;  // lower case
};

std::mutex g_settingsMutex;
std::vector<MonitorSelector> g_selectors;

std::wstring ToLower(std::wstring s) {
    for (auto& c : s) {
        c = (WCHAR)towlower(c);
    }
    return s;
}

std::wstring Trim(const std::wstring& s) {
    size_t begin = s.find_first_not_of(L" \t");
    if (begin == std::wstring::npos) {
        return L"";
    }
    size_t end = s.find_last_not_of(L" \t");
    return s.substr(begin, end - begin + 1);
}

void LoadSettings() {
    std::vector<MonitorSelector> selectors;

    std::wstring list = WindhawkUtils::StringSetting::make(L"disableOn").get();

    size_t pos = 0;
    while (pos <= list.size()) {
        size_t comma = list.find(L',', pos);
        if (comma == std::wstring::npos) {
            comma = list.size();
        }

        std::wstring token = ToLower(Trim(list.substr(pos, comma - pos)));
        pos = comma + 1;

        if (token.empty()) {
            continue;
        }

        if (token == L"primary") {
            selectors.push_back({MonitorSelector::Kind::Primary, 0, L""});
        } else if (token.find_first_not_of(L"0123456789") ==
                   std::wstring::npos) {
            selectors.push_back(
                {MonitorSelector::Kind::Number, _wtoi(token.c_str()), L""});
        } else {
            selectors.push_back({MonitorSelector::Kind::Name, 0, token});
        }
    }

    Wh_Log(L"Settings: disableOn=\"%s\", %d entries", list.c_str(),
           (int)selectors.size());

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_selectors = std::move(selectors);
}

struct MonitorEntry {
    HMONITOR monitor;
    int number;  // 1-based EnumDisplayMonitors position
    bool primary;
    RECT rect;
    WCHAR device[CCHDEVICENAME];
    WCHAR model[128];
    WCHAR interfaceName[128];
    bool disabled;
};

std::vector<MonitorEntry> EnumMonitors() {
    std::vector<MonitorEntry> monitors;

    auto proc = [&](HMONITOR monitor) {
        MonitorEntry entry = {};
        entry.monitor = monitor;
        entry.number = (int)monitors.size() + 1;

        MONITORINFOEX mi = {};
        mi.cbSize = sizeof(mi);
        if (GetMonitorInfo(monitor, &mi)) {
            entry.primary = !!(mi.dwFlags & MONITORINFOF_PRIMARY);
            entry.rect = mi.rcMonitor;
            wcscpy_s(entry.device, mi.szDevice);

            DISPLAY_DEVICE dd = {.cb = sizeof(dd)};
            if (EnumDisplayDevices(mi.szDevice, 0, &dd,
                                   EDD_GET_DEVICE_INTERFACE_NAME)) {
                wcscpy_s(entry.model, dd.DeviceString);
                wcscpy_s(entry.interfaceName, dd.DeviceID);
            }
        }

        monitors.push_back(entry);
    };

    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR monitor, HDC, LPRECT, LPARAM lParam) -> BOOL {
            (*reinterpret_cast<decltype(proc)*>(lParam))(monitor);
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&proc));

    return monitors;
}

bool SelectorMatches(const MonitorSelector& selector,
                     const MonitorEntry& entry) {
    switch (selector.kind) {
        case MonitorSelector::Kind::Primary:
            return entry.primary;
        case MonitorSelector::Kind::Number:
            return selector.number == entry.number;
        case MonitorSelector::Kind::Name:
            for (PCWSTR field :
                 {entry.device, entry.model, entry.interfaceName}) {
                if (ToLower(field).find(selector.name) != std::wstring::npos) {
                    return true;
                }
            }
            return false;
    }
    return false;
}

// Disabled monitors are resolved once per kCacheMs (the checks run on every
// taskbar move) and logged whenever the result changes.
constexpr ULONGLONG kCacheMs = 1000;

std::mutex g_monitorCacheMutex;
std::vector<HMONITOR> g_disabledMonitors;
ULONGLONG g_monitorCacheTime;
bool g_monitorCacheValid;
std::wstring g_monitorCacheSignature;

void InvalidateMonitorCache() {
    std::lock_guard<std::mutex> lock(g_monitorCacheMutex);
    g_monitorCacheValid = false;
    g_monitorCacheSignature.clear();
}

void RefreshMonitorCacheLocked() {
    std::vector<MonitorSelector> selectors;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        selectors = g_selectors;
    }

    auto monitors = EnumMonitors();

    std::vector<HMONITOR> disabled;
    std::wstring signature;
    for (auto& entry : monitors) {
        for (const auto& selector : selectors) {
            if (SelectorMatches(selector, entry)) {
                entry.disabled = true;
                break;
            }
        }

        if (entry.disabled) {
            disabled.push_back(entry.monitor);
        }

        signature += std::to_wstring((ULONG_PTR)entry.monitor) + L":" +
                     std::to_wstring(entry.disabled) + L";";
    }

    if (signature != g_monitorCacheSignature) {
        for (const auto& entry : monitors) {
            Wh_Log(L"Monitor %d: %s%s (%d,%d)-(%d,%d) model=\"%s\" "
                   L"interface=%s -> %s",
                   entry.number, entry.device,
                   entry.primary ? L" primary" : L"", entry.rect.left,
                   entry.rect.top, entry.rect.right, entry.rect.bottom,
                   entry.model, entry.interfaceName,
                   entry.disabled ? L"DISABLED" : L"enabled");
        }
        g_monitorCacheSignature = signature;
    }

    g_disabledMonitors = std::move(disabled);
    g_monitorCacheTime = GetTickCount64();
    g_monitorCacheValid = true;
}

bool IsDisabledMonitor(HMONITOR monitor) {
    if (!monitor || g_unloading) {
        return false;
    }

    std::lock_guard<std::mutex> lock(g_monitorCacheMutex);

    if (!g_monitorCacheValid ||
        GetTickCount64() - g_monitorCacheTime >= kCacheMs) {
        RefreshMonitorCacheLocked();
    }

    for (HMONITOR disabledMonitor : g_disabledMonitors) {
        if (disabledMonitor == monitor) {
            return true;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// Taskbar windows and their popups.

bool IsCurrentProcessWnd(HWND hWnd) {
    DWORD dwProcessId;
    return GetWindowThreadProcessId(hWnd, &dwProcessId) &&
           dwProcessId == GetCurrentProcessId();
}

bool ClassNameIs(HWND hWnd, std::initializer_list<PCWSTR> classNames) {
    WCHAR className[64];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }

    for (PCWSTR name : classNames) {
        if (_wcsicmp(className, name) == 0) {
            return true;
        }
    }
    return false;
}

bool IsTaskbarWnd(HWND hWnd) {
    return hWnd && IsCurrentProcessWnd(hWnd) &&
           ClassNameIs(hWnd, {L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd"});
}

// Top-level popups a taskbar shows over itself. Other windows it might own
// (e.g. dialogs) are left alone.
bool IsTaskbarPopupWnd(HWND hWnd) {
    HWND hOwner = GetWindow(hWnd, GW_OWNER);
    return hOwner && IsTaskbarWnd(hOwner) &&
           ClassNameIs(hWnd, {L"Xaml_WindowedPopupClass", L"tooltips_class32",
                              L"TaskListThumbnailWnd"});
}

HWND GetManagedTaskbarWnd(HWND hWnd) {
    if (IsTaskbarWnd(hWnd)) {
        return hWnd;
    }

    if (IsTaskbarPopupWnd(hWnd)) {
        return GetWindow(hWnd, GW_OWNER);
    }

    return nullptr;
}

// The monitor a taskbar belongs to. A shown taskbar lies inside its monitor.
// An auto-hidden one is moved out of it and leaves a thin strip on it, so the
// rest of the window can lie on a neighboring monitor (e.g. a left taskbar on
// a monitor whose left neighbor is another monitor): then its monitor is the
// one it was last seen inside, or else the one with the smallest part of it.
HMONITOR GetTaskbarMonitor(HWND hTaskbarWnd) {
    RECT rc;
    if (!GetWindowRect(hTaskbarWnd, &rc)) {
        return nullptr;
    }

    struct Search {
        RECT rc;
        HMONITOR inside;
        HMONITOR smallest;
        LONGLONG smallestArea;
    } search = {rc, nullptr, nullptr, 0};

    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR monitor, HDC, LPRECT monitorRect, LPARAM lParam) -> BOOL {
            auto& search = *reinterpret_cast<Search*>(lParam);
            RECT part;
            if (!IntersectRect(&part, &search.rc, monitorRect)) {
                return TRUE;
            }
            if (EqualRect(&part, &search.rc)) {
                search.inside = monitor;
                return FALSE;
            }
            LONGLONG area = (LONGLONG)(part.right - part.left) *
                            (part.bottom - part.top);
            if (!search.smallest || area < search.smallestArea) {
                search.smallest = monitor;
                search.smallestArea = area;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&search));

    if (search.inside) {
        SetProp(hTaskbarWnd, kMonitorProp, search.inside);
        return search.inside;
    }

    MONITORINFO mi = {sizeof(mi)};
    auto cached = static_cast<HMONITOR>(GetProp(hTaskbarWnd, kMonitorProp));
    if (cached && GetMonitorInfo(cached, &mi)) {
        return cached;
    }

    if (search.smallest) {
        return search.smallest;
    }

    return MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
}

bool IsTaskbarOnDisabledMonitor(HWND hTaskbarWnd) {
    return IsDisabledMonitor(GetTaskbarMonitor(hTaskbarWnd));
}

template <typename F>
void ForEachCurrentProcessTopLevelWnd(F f) {
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            if (IsCurrentProcessWnd(hWnd)) {
                (*reinterpret_cast<F*>(lParam))(hWnd);
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&f));
}

using DwmSetWindowAttribute_t = decltype(&DwmSetWindowAttribute);
DwmSetWindowAttribute_t DwmSetWindowAttribute_Original;

// Cloaks or uncloaks a taskbar window or popup to match the settings. When the
// mod doesn't need it cloaked, the cloak state explorer asked for is restored.
void UpdateCloak(HWND hWnd) {
    HWND hTaskbarWnd = GetManagedTaskbarWnd(hWnd);
    if (!hTaskbarWnd) {
        return;
    }

    bool modCloak = IsTaskbarOnDisabledMonitor(hTaskbarWnd);
    bool modCloaked = GetProp(hWnd, kModCloakProp) != nullptr;
    if (modCloak == modCloaked) {
        return;
    }

    // Explorer's own cloak from before the mod loaded is read once, so it can
    // be restored later.
    if (modCloak && !GetProp(hWnd, kExplorerCloakProp)) {
        DWORD cloaked = 0;
        DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
        SetProp(hWnd, kExplorerCloakProp,
                (HANDLE)(ULONG_PTR)((cloaked & DWM_CLOAKED_APP) ? 2 : 1));
    }

    BOOL cloak = modCloak || (ULONG_PTR)GetProp(hWnd, kExplorerCloakProp) == 2;
    HRESULT hr = DwmSetWindowAttribute_Original(hWnd, DWMWA_CLOAK, &cloak,
                                                sizeof(cloak));
    Wh_Log(L"%s %p, hr=0x%08X", modCloak ? L"Cloaked" : L"Uncloaked", hWnd,
           hr);

    if (modCloak) {
        SetProp(hWnd, kModCloakProp, (HANDLE)1);
    } else {
        RemoveProp(hWnd, kModCloakProp);
    }
}

void UpdateAllCloaks() {
    ForEachCurrentProcessTopLevelWnd([](HWND hWnd) { UpdateCloak(hWnd); });
}

// Explorer cloaks and uncloaks its own taskbar windows in some situations. Its
// request is remembered, and a disabled taskbar stays cloaked.
HRESULT WINAPI DwmSetWindowAttribute_Hook(HWND hwnd,
                                          DWORD dwAttribute,
                                          LPCVOID pvAttribute,
                                          DWORD cbAttribute) {
    if (dwAttribute == DWMWA_CLOAK && pvAttribute &&
        cbAttribute >= sizeof(BOOL)) {
        if (HWND hTaskbarWnd = GetManagedTaskbarWnd(hwnd)) {
            BOOL requested = *static_cast<const BOOL*>(pvAttribute);
            SetProp(hwnd, kExplorerCloakProp, (HANDLE)(ULONG_PTR)(requested ? 2 : 1));

            if (!requested && IsTaskbarOnDisabledMonitor(hTaskbarWnd)) {
                Wh_Log(L"Kept %p cloaked", hwnd);
                SetProp(hwnd, kModCloakProp, (HANDLE)1);
                BOOL cloak = TRUE;
                return DwmSetWindowAttribute_Original(hwnd, dwAttribute, &cloak,
                                                      sizeof(cloak));
            }
        }
    }

    return DwmSetWindowAttribute_Original(hwnd, dwAttribute, pvAttribute,
                                          cbAttribute);
}

UINT GetApplyStateMsg();
void UpdateAllCloaks();
void RecomputeWorkAreasOnTaskbarThread();

// Taskbar windows can move to another monitor (display changes), so the cloak
// is re-evaluated after every move.
LRESULT CALLBACK TaskbarSubclassProc(HWND hWnd,
                                     UINT uMsg,
                                     WPARAM wParam,
                                     LPARAM lParam,
                                     DWORD_PTR dwRefData) {
    if (uMsg == WM_NCDESTROY) {
        RemoveProp(hWnd, kSubclassedProp);
        RemoveProp(hWnd, kModCloakProp);
        RemoveProp(hWnd, kExplorerCloakProp);
        RemoveProp(hWnd, kMonitorProp);
    }

    if (uMsg == WM_DISPLAYCHANGE) {
        InvalidateMonitorCache();
        RemoveProp(hWnd, kMonitorProp);
    }

    if (uMsg == GetApplyStateMsg()) {
        UpdateAllCloaks();
        RecomputeWorkAreasOnTaskbarThread();
        return 0;
    }

    LRESULT ret = DefSubclassProc(hWnd, uMsg, wParam, lParam);

    if (uMsg == WM_WINDOWPOSCHANGED) {
        UpdateCloak(hWnd);
    }

    return ret;
}

void SubclassTaskbarWnd(HWND hWnd) {
    if (GetProp(hWnd, kSubclassedProp)) {
        return;
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd,
                                                        TaskbarSubclassProc, 0)) {
        Wh_Log(L"Subclassing taskbar window %p failed", hWnd);
        return;
    }

    SetProp(hWnd, kSubclassedProp, (HANDLE)1);
    Wh_Log(L"Subclassed taskbar window %p", hWnd);
}

void HandleCreatedWindow(HWND hWnd) {
    if (!hWnd || g_unloading) {
        return;
    }

    if (IsTaskbarWnd(hWnd)) {
        SubclassTaskbarWnd(hWnd);
    }

    UpdateCloak(hWnd);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    HandleCreatedWindow(hWnd);
    return hWnd;
}

// Undocumented, used by explorer for some shell windows.
using CreateWindowInBand_t = HWND(WINAPI*)(DWORD dwExStyle,
                                           LPCWSTR lpClassName,
                                           LPCWSTR lpWindowName,
                                           DWORD dwStyle,
                                           int X,
                                           int Y,
                                           int nWidth,
                                           int nHeight,
                                           HWND hWndParent,
                                           HMENU hMenu,
                                           HINSTANCE hInstance,
                                           LPVOID lpParam,
                                           DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle,
                                    LPCWSTR lpClassName,
                                    LPCWSTR lpWindowName,
                                    DWORD dwStyle,
                                    int X,
                                    int Y,
                                    int nWidth,
                                    int nHeight,
                                    HWND hWndParent,
                                    HMENU hMenu,
                                    HINSTANCE hInstance,
                                    LPVOID lpParam,
                                    DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
    HandleCreatedWindow(hWnd);
    return hWnd;
}

////////////////////////////////////////////////////////////////////////////////
// Work area.

using TrayUI_GetAutoHideFlags_t = DWORD(WINAPI*)(void* pThis);
TrayUI_GetAutoHideFlags_t TrayUI_GetAutoHideFlags_Original;
DWORD WINAPI TrayUI_GetAutoHideFlags_Hook(void* pThis) {
    DWORD ret = TrayUI_GetAutoHideFlags_Original(pThis);

    if (g_reportAutoHide) {
        ret |= ABS_AUTOHIDE;
    }

    return ret;
}

using CTray_RecomputeWorkArea_t = int(WINAPI*)(void* pThis,
                                               HMONITOR monitor,
                                               RECT* rect);
CTray_RecomputeWorkArea_t CTray_RecomputeWorkArea_Original;
int WINAPI CTray_RecomputeWorkArea_Hook(void* pThis,
                                        HMONITOR monitor,
                                        RECT* rect) {
    bool disabled = IsDisabledMonitor(monitor);
    if (disabled) {
        g_reportAutoHide = true;
    }

    int ret = CTray_RecomputeWorkArea_Original(pThis, monitor, rect);

    g_reportAutoHide = false;

    if (disabled && rect) {
        Wh_Log(L"Work area of disabled monitor: (%d,%d)-(%d,%d)", rect->left,
               rect->top, rect->right, rect->bottom);
    }

    return ret;
}

// CTray::RecomputeAllWorkareas makes explorer recompute and apply the work
// area of every monitor. It's virtual, so it must be called with the pointer to
// the CTray subobject whose vtable holds it. The CTray object is the global
// c_trayStatic; its vtable pointers are scanned for the one whose vtable
// contains RecomputeAllWorkareas, so no layout offset is assumed.
void* g_trayStatic;

using CTray_RecomputeAllWorkareas_t = void(WINAPI*)(void* pThis);
CTray_RecomputeAllWorkareas_t CTray_RecomputeAllWorkareas;

void* g_recomputeAllWorkareasThis;

void* FindRecomputeAllWorkareasThis() {
    if (!g_trayStatic || !CTray_RecomputeAllWorkareas) {
        return nullptr;
    }

    auto imageBase = reinterpret_cast<BYTE*>(GetModuleHandle(nullptr));
    auto ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(
        imageBase + reinterpret_cast<IMAGE_DOS_HEADER*>(imageBase)->e_lfanew);
    BYTE* imageEnd = imageBase + ntHeaders->OptionalHeader.SizeOfImage;
    auto inImage = [&](const void* p) {
        return p >= imageBase && p < imageEnd;
    };

    // The first slots of the object hold its vtable pointers (one per base
    // class with virtual functions).
    constexpr int kSlots = 32;
    auto slots = static_cast<void**>(g_trayStatic);

    for (int i = 0; i < kSlots; i++) {
        auto vtable = static_cast<void**>(slots[i]);
        if (!inImage(vtable)) {
            continue;
        }

        // A vtable ends where the next one starts.
        size_t entries = 512;
        for (int j = 0; j < kSlots; j++) {
            auto other = static_cast<void**>(slots[j]);
            if (inImage(other) && other > vtable &&
                (size_t)(other - vtable) < entries) {
                entries = other - vtable;
            }
        }

        for (size_t k = 0; k < entries && inImage(&vtable[k]); k++) {
            if (vtable[k] == (void*)CTray_RecomputeAllWorkareas) {
                Wh_Log(L"RecomputeAllWorkareas: subobject at +0x%X, slot %d",
                       i * (int)sizeof(void*), (int)k);
                return &slots[i];
            }
        }
    }

    Wh_Log(L"RecomputeAllWorkareas: subobject not found");
    return nullptr;
}

// Sent to the primary taskbar window, so that the cloak pass and the work area
// recomputation run on its thread, where every taskbar window and popup lives
// (the subclass proc and the hooks update them there too).
UINT GetApplyStateMsg() {
    static UINT msg =
        RegisterWindowMessage(L"Windhawk_taskbar-disable-per-monitor_apply");
    return msg;
}

void ApplyStateOnTaskbarThreads() {
    ForEachCurrentProcessTopLevelWnd([](HWND hWnd) {
        if (ClassNameIs(hWnd, {L"Shell_TrayWnd"})) {
            SendMessage(hWnd, GetApplyStateMsg(), 0, 0);
        }
    });
}

// Fallback when RecomputeAllWorkareas can't be called: toggling auto-hide and
// restoring it right away makes explorer recompute the work areas. The setting
// ends up as it was.
void ToggleAutoHideToRecomputeWorkAreas() {
    APPBARDATA abd = {sizeof(abd)};
    UINT state = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);

    abd.lParam = state ^ ABS_AUTOHIDE;
    SHAppBarMessage(ABM_SETSTATE, &abd);
    abd.lParam = state;
    SHAppBarMessage(ABM_SETSTATE, &abd);

    Wh_Log(L"Recomputed the work areas by toggling auto-hide");
}

void RecomputeWorkAreasOnTaskbarThread() {
    if (!g_recomputeAllWorkareasThis) {
        g_recomputeAllWorkareasThis = FindRecomputeAllWorkareasThis();
    }

    if (g_recomputeAllWorkareasThis) {
        CTray_RecomputeAllWorkareas(g_recomputeAllWorkareasThis);
    } else {
        ToggleAutoHideToRecomputeWorkAreas();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Applying.

void ApplyState() {
    std::lock_guard<std::mutex> lock(g_applyMutex);

    InvalidateMonitorCache();

    ForEachCurrentProcessTopLevelWnd([](HWND hWnd) {
        if (!g_unloading && IsTaskbarWnd(hWnd)) {
            SubclassTaskbarWnd(hWnd);
        }
    });

    ApplyStateOnTaskbarThreads();
}

////////////////////////////////////////////////////////////////////////////////
// Hook setup.

bool HookTaskbarSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Couldn't load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: virtual unsigned int __cdecl TrayUI::GetAutoHideFlags(void))"},
            &TrayUI_GetAutoHideFlags_Original,
            TrayUI_GetAutoHideFlags_Hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                    ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed (taskbar.dll)");
        return false;
    }

    return true;
}

bool HookExplorerExeSymbols() {
    HMODULE module = GetModuleHandle(nullptr);

    WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
        {
            {LR"(private: int __cdecl CTray::RecomputeWorkArea(struct HMONITOR__ *,struct tagRECT *))"},
            &CTray_RecomputeWorkArea_Original,
            CTray_RecomputeWorkArea_Hook,
        },
        {
            {LR"(public: virtual void __cdecl CTray::RecomputeAllWorkareas(void))"},
            &CTray_RecomputeAllWorkareas,
            nullptr,
            true,
        },
        {
            {LR"(struct CTrayStatic c_trayStatic)"},
            &g_trayStatic,
            nullptr,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, explorerExeHooks,
                                    ARRAYSIZE(explorerExeHooks))) {
        Wh_Log(L"HookSymbols failed (explorer.exe)");
        return false;
    }

    return true;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookTaskbarSymbols()) {
        return FALSE;
    }

    if (!HookExplorerExeSymbols()) {
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(DwmSetWindowAttribute,
                                   DwmSetWindowAttribute_Hook,
                                   &DwmSetWindowAttribute_Original);

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);

    HMODULE user32Module = GetModuleHandle(L"user32.dll");
    if (auto pCreateWindowInBand = (CreateWindowInBand_t)GetProcAddress(
            user32Module, "CreateWindowInBand")) {
        WindhawkUtils::SetFunctionHook(pCreateWindowInBand,
                                       CreateWindowInBand_Hook,
                                       &CreateWindowInBand_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // Taskbar windows that already exist (mod enabled while explorer runs).
    // Ones created later are handled by the CreateWindow hooks.
    ApplyState();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
    ApplyState();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    g_unloading = true;

    {
        std::lock_guard<std::mutex> lock(g_applyMutex);

        ApplyStateOnTaskbarThreads();

        ForEachCurrentProcessTopLevelWnd([](HWND hWnd) {
            if (GetProp(hWnd, kSubclassedProp)) {
                WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                    hWnd, TaskbarSubclassProc);
                RemoveProp(hWnd, kSubclassedProp);
            }
            RemoveProp(hWnd, kExplorerCloakProp);
            RemoveProp(hWnd, kMonitorProp);
        });
    }
}
