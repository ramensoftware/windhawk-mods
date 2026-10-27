// ==WindhawkMod==
// @id              taskbar-per-monitor-alignment
// @name            Per-monitor taskbar icon alignment
// @description     Choose icon alignment (Left, Center, Right) and Start button placement (Left, Right) independently for each monitor (Windows 11).
// @version         1.0.0
// @author          vmTools
// @github          https://github.com/VitalyTz
// @homepage        https://github.com/VitalyTz/windhawk-taskbar-per-monitor-alignment
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luuid -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Per-monitor taskbar icon alignment

Choose how the taskbar looks on **each monitor separately**: its icons **Left**,
**Center**, **Right** or Windows' default, and the **Start button** at the left edge,
the right edge, or with the icons. Windows only offers one alignment for every
taskbar, and only Left or Center.

![A monitor with centered icons and the Start button at the left edge](https://raw.githubusercontent.com/VitalyTz/windhawk-taskbar-per-monitor-alignment/main/docs/monitor2-overview.jpg)

| Icons | Start button | Result |
| --- | --- | --- |
| Left | with the icons | ![](https://raw.githubusercontent.com/VitalyTz/windhawk-taskbar-per-monitor-alignment/main/docs/icons-left.jpg) |
| Center | with the icons | ![](https://raw.githubusercontent.com/VitalyTz/windhawk-taskbar-per-monitor-alignment/main/docs/icons-center.jpg) |
| Right | with the icons | ![](https://raw.githubusercontent.com/VitalyTz/windhawk-taskbar-per-monitor-alignment/main/docs/icons-right.jpg) |
| Center | left edge | ![](https://raw.githubusercontent.com/VitalyTz/windhawk-taskbar-per-monitor-alignment/main/docs/icons-center-start-left.jpg) |
| Left | right edge | ![](https://raw.githubusercontent.com/VitalyTz/windhawk-taskbar-per-monitor-alignment/main/docs/icons-left-start-right.jpg) |

## Settings

![Mod settings](https://raw.githubusercontent.com/VitalyTz/windhawk-taskbar-per-monitor-alignment/main/docs/settings.jpg)

* **Primary monitor / Other monitors** set the icon alignment and the Start button
  placement for every monitor of that kind. *Windows default* keeps whatever Windows
  is set to.
* **Per-monitor overrides** handle individual displays: add one row per monitor and
  pick it from the **Monitor** dropdown, *1st* to *6th from the left* or *Rightmost*.
  Positions follow the arrangement in Windows display settings, so they keep working
  when displays are renumbered or reconnected. Choose *Custom* to type a display
  device name (`DISPLAY1`), a monitor name (`DELL U2719D`) or part of a device path
  (`DISPLAY#DELA0C1`) instead. The first matching row wins.
* **Start button:** *Follow the icons* leaves it in the icon group, as Windows does.
  *Left edge* and *Right edge (next to the tray)* place it independently, and the
  icons close the gap. Choosing the edge the icons already gather at changes nothing.

Right is this mod's own option: Windows' alignment setting has no Right, so a Right
taskbar is told Left and its icons are aligned right in the taskbar's layout, stopping
at the system tray. The tray itself never moves.

## Installation

Install the mod, then restart Windows Explorer **once** (Task Manager > Windows
Explorer > Restart, or sign out and back in). The mod has to be there when each
taskbar is created to capture its settings. After that, changing settings applies
immediately without a restart. Enable **Show my taskbar on all displays** in Windows
settings to get a taskbar on every monitor.

Disabling the mod puts every taskbar back to Windows' own layout.

## Compatibility

Windows 11, 64-bit, native taskbar only. Tested on Windows 11 26H2 (build 26300)
with Windhawk 1.7.3, on three displays with different resolutions and scales.

The mod relies on Windows' private taskbar internals. A Windows update can change
them: if the core hooks can't be found the mod refuses to start, and the XAML-based
features (Right icons, Start button placement) simply stay off. Windows 10,
ExplorerPatcher, StartAllBack and other mods that change the taskbar's alignment or
layout are not supported. Start menu and search flyouts keep using Windows' own
placement. No registry values are changed.

Source, build instructions and tests:
https://github.com/VitalyTz/windhawk-taskbar-per-monitor-alignment
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- primaryAlignment: center
  $name: Primary monitor alignment
  $options:
  - default: Windows default
  - left: Left
  - center: Center
  - right: Right
- secondaryAlignment: left
  $name: Other monitors alignment
  $options:
  - default: Windows default
  - left: Left
  - center: Center
  - right: Right
- primaryStartAlignment: follow
  $name: Primary monitor Start button
  $description: >-
    Where the Start button sits. Follow the icons keeps it in the icon group, where
    Windows puts it. Left and Right place it independently of the icons.
  $options:
  - follow: Follow the icons
  - left: Left edge
  - right: Right edge (next to the tray)
- secondaryStartAlignment: follow
  $name: Other monitors Start button
  $options:
  - follow: Follow the icons
  - left: Left edge
  - right: Right edge (next to the tray)
- monitors:
  - - monitor: custom
      $name: Monitor
      $description: >-
        Pick a monitor by where it sits from left to right, or choose Custom to
        type an identifier below. Positions follow the displays' arrangement in
        Windows display settings, so they keep working if the displays are renumbered.
      $options:
      - custom: Custom (use the identifier below)
      - pos1: 1st from the left
      - pos2: 2nd from the left
      - pos3: 3rd from the left
      - pos4: 4th from the left
      - pos5: 5th from the left
      - pos6: 6th from the left
      - last: Rightmost
    - device: ""
      $name: Custom monitor identifier
      $description: >-
        Used when Monitor is Custom. A display device name (DISPLAY1), a monitor
        friendly name (DELL U2719D), or part of a monitor device path
        (DISPLAY#DELA0C1, matched as a substring of the full path). Blank entries
        are ignored. First matching entry wins.
    - alignment: default
      $name: Icon alignment
      $options:
      - default: Windows default
      - left: Left
      - center: Center
      - right: Right
    - startAlignment: follow
      $name: Start button
      $options:
      - follow: Follow the icons
      - left: Left edge
      - right: Right edge (next to the tray)
  $name: Per-monitor overrides
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <commctrl.h>
#include <oleidl.h>
#include <algorithm>
#include <cmath>
#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

// Windows' GetCurrentTime macro collides with a XAML interface method.
#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

// Windows' own TaskbarAlignment has only Left and Center; Right is this mod's
// own value, applied to the taskbar's XAML layout rather than to the setting.
constexpr int kWindowsDefault = -1;
constexpr int kLeft = 0;
constexpr int kCenter = 1;
constexpr int kRight = 2;
// The Start button's own choice. Left and Right share the icons' values; this one
// means it stays in the icon group, where Windows puts it.
constexpr int kFollowIcons = -1;

// A taskbar's wanted layout, packed into one int to travel in a window message:
// bit 0 is right-aligned icons, the next two bits are the Start button's mode + 1.
int PackLayout(bool right, int startMode) { return (right ? 1 : 0) | ((startMode + 1) << 1); }
bool LayoutWantsRight(int packed) { return (packed & 1) != 0; }
int LayoutStartMode(int packed) { return ((packed >> 1) & 3) - 1; }

// A rule can name its monitor by an identifier, or by where the monitor sits from
// left to right, which survives display renumbering and needs no typing.
constexpr int kPositionNone = -1;
constexpr int kPositionLast = -2;
constexpr int kMaxPositions = 6;

struct MonitorRule {
    std::wstring identifier;
    int alignment;
    int start = kFollowIcons;
    int position = kPositionNone;  // 0 is the leftmost monitor
};
struct Settings {
    int primary = -1;
    int secondary = -1;
    std::vector<MonitorRule> monitors;
    // True when some rule can only be matched through the display configuration,
    // so that lookup is skipped entirely for plain device-name rules.
    bool extendedIdentifiers = false;
    int primaryStart = kFollowIcons;
    int secondaryStart = kFollowIcons;
    bool usesPosition = false;
};
Settings g_settings;
std::mutex g_mutex;

// One entry per live TaskbarSettings. The site holds an owning reference so the
// window can be resolved again later; the HWND is re-queried whenever the cached
// one is gone, so moving a taskbar or changing the primary display is handled.
struct TaskbarEntry {
    IUnknown* site = nullptr;
    HWND hwnd = nullptr;
    // Windows' own alignment, as last read while no override applied to this taskbar.
    int windowsAlignment = 1;
};
std::unordered_map<void*, TaskbarEntry> g_taskbars;
std::unordered_map<void*, void*> g_interfaces;
void* g_settingsInterfaceVtable;
std::atomic<bool> g_unloading = false;

// C++/WinRT stores interface subobjects inside the implementation. Locate the
// exact symbol-resolved vtable instead of assuming a fixed interface offset,
// which was object + 0x10 in the build checked against Microsoft's symbols.
// ReadProcessMemory fails cleanly at unreadable memory; no unchecked dereference.
void* FindSettingsInterface(void* object) {
    // Without the vtable symbol, every empty slot would compare equal to null.
    if (!g_settingsInterfaceVtable) return nullptr;
    for (size_t offset = 0; offset < 32 * sizeof(void*); offset += sizeof(void*)) {
        void* candidate = nullptr;
        auto address = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(object) + offset);
        SIZE_T read = 0;
        if (!ReadProcessMemory(GetCurrentProcess(), address, &candidate, sizeof(candidate), &read) ||
            read != sizeof(candidate)) break;
        if (candidate == g_settingsInterfaceVtable) return address;
    }
    return nullptr;
}

std::wstring NormalizeIdentifier(std::wstring value) {
    const auto first = value.find_first_not_of(L" \t");
    if (first == std::wstring::npos) return {};
    value = value.substr(first, value.find_last_not_of(L" \t") - first + 1);
    // Device names are interchangeable with their short form. Device paths start
    // with \\?\ and are left intact.
    if (value.starts_with(L"\\\\.\\")) value.erase(0, 4);
    for (auto& c : value) if (c >= L'a' && c <= L'z') c -= L'a' - L'A';
    return value;
}

// Identifiers of the monitor a taskbar currently sits on. Friendly name and path
// stay empty unless a configured rule needs them.
struct MonitorIdentity {
    std::wstring device;
    std::wstring friendly;
    std::wstring path;
    int position = -1;  // Index from the left among all active monitors, if known.
    int count = 0;
};

bool IdentifierMatches(const std::wstring& rule, const MonitorIdentity& monitor) {
    const auto wanted = NormalizeIdentifier(rule);
    if (wanted.empty()) return false;
    if (wanted == monitor.device) return true;
    if (!monitor.friendly.empty() && wanted == monitor.friendly) return true;
    // Only a rule that looks like part of a device path is matched loosely, so a
    // short friendly name can never match a path by accident.
    return wanted.find(L'#') != std::wstring::npos && !monitor.path.empty() &&
           monitor.path.find(wanted) != std::wstring::npos;
}

bool RuleMatches(const MonitorRule& rule, const MonitorIdentity& monitor) {
    if (rule.position == kPositionNone) return IdentifierMatches(rule.identifier, monitor);
    if (monitor.position < 0) return false;
    return rule.position == kPositionLast ? monitor.position == monitor.count - 1
                                          : monitor.position == rule.position;
}

// DISPLAY followed by digits is resolvable from the monitor info alone.
bool LooksLikeDeviceName(const std::wstring& rule) {
    const auto value = NormalizeIdentifier(rule);
    if (!value.starts_with(L"DISPLAY") || value.size() == 7) return false;
    for (size_t i = 7; i < value.size(); ++i)
        if (value[i] < L'0' || value[i] > L'9') return false;
    return true;
}

struct Placement { int icons = kWindowsDefault; int start = kFollowIcons; };

Placement SelectPlacement(const Settings& settings, const MonitorIdentity& monitor, bool primary) {
    for (const auto& rule : settings.monitors)
        if (RuleMatches(rule, monitor)) return {rule.alignment, rule.start};
    return primary ? Placement{settings.primary, settings.primaryStart}
                   : Placement{settings.secondary, settings.secondaryStart};
}

int SelectAlignment(const Settings& settings, const MonitorIdentity& monitor, bool primary) {
    return SelectPlacement(settings, monitor, primary).icons;
}

// A Start button wanted where the icons already gather is simply left in the group.
int ResolveStartMode(int start, int effectiveIcons) {
    return start < 0 || start == effectiveIcons ? kFollowIcons : start;
}

int ReadAlignment(const wchar_t* key) {
    PCWSTR value = Wh_GetStringSetting(key);
    const int result = !value                          ? kWindowsDefault :
                       wcscmp(value, L"left") == 0     ? kLeft :
                       wcscmp(value, L"center") == 0   ? kCenter :
                       wcscmp(value, L"right") == 0    ? kRight :
                                                         kWindowsDefault;
    if (value) Wh_FreeStringSetting(value);
    return result;
}

// Device name -> friendly name and device path, from the active display paths.
struct DisplayNames { std::wstring friendly; std::wstring path; };
std::unordered_map<std::wstring, DisplayNames> g_displayNames;
ULONGLONG g_displayNamesTick = 0;
bool g_displayNamesValid = false;
constexpr ULONGLONG kDisplayNamesLifetimeMs = 5000;

std::wstring Bounded(const wchar_t* value, size_t capacity) {
    return std::wstring(value, wcsnlen(value, capacity));
}

void RebuildDisplayNames() {
    g_displayNames.clear();
    g_displayNamesTick = GetTickCount64();
    g_displayNamesValid = true;
    UINT32 pathCount = 0, modeCount = 0;
    if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount, &modeCount) != ERROR_SUCCESS)
        return;
    std::vector<DISPLAYCONFIG_PATH_INFO> paths(pathCount);
    std::vector<DISPLAYCONFIG_MODE_INFO> modes(modeCount);
    if (QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, paths.data(), &modeCount,
                           modes.data(), nullptr) != ERROR_SUCCESS)
        return;
    paths.resize(pathCount);
    for (const auto& path : paths) {
        DISPLAYCONFIG_SOURCE_DEVICE_NAME source{};
        source.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
        source.header.size = sizeof(source);
        source.header.adapterId = path.sourceInfo.adapterId;
        source.header.id = path.sourceInfo.id;
        if (DisplayConfigGetDeviceInfo(&source.header) != ERROR_SUCCESS) continue;
        const auto device = NormalizeIdentifier(
            Bounded(source.viewGdiDeviceName, ARRAYSIZE(source.viewGdiDeviceName)));
        // Cloned displays share a source; the first active path wins.
        if (device.empty() || g_displayNames.contains(device)) continue;
        DISPLAYCONFIG_TARGET_DEVICE_NAME target{};
        target.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
        target.header.size = sizeof(target);
        target.header.adapterId = path.targetInfo.adapterId;
        target.header.id = path.targetInfo.id;
        if (DisplayConfigGetDeviceInfo(&target.header) != ERROR_SUCCESS) continue;
        g_displayNames[device] = {
            NormalizeIdentifier(Bounded(target.monitorFriendlyDeviceName,
                                        ARRAYSIZE(target.monitorFriendlyDeviceName))),
            NormalizeIdentifier(Bounded(target.monitorDevicePath,
                                        ARRAYSIZE(target.monitorDevicePath)))};
    }
}

// Called with g_mutex held. Rebuilds on an unknown device so a newly attached
// monitor is picked up at once, and expires so a swapped monitor isn't stale.
void FillExtendedIdentity(MonitorIdentity& monitor) {
    const bool expired = !g_displayNamesValid ||
                         GetTickCount64() - g_displayNamesTick > kDisplayNamesLifetimeMs;
    if (expired || !g_displayNames.contains(monitor.device)) RebuildDisplayNames();
    const auto names = g_displayNames.find(monitor.device);
    if (names == g_displayNames.end()) return;
    monitor.friendly = names->second.friendly;
    monitor.path = names->second.path;
}

// The Start button can only go to an edge. Anything else leaves it in the icon group.
int ReadStartAlignment(const wchar_t* key) {
    const int alignment = ReadAlignment(key);
    return alignment == kLeft || alignment == kRight ? alignment : kFollowIcons;
}

// Left-to-right order of the active monitors, ties broken top to bottom.
struct MonitorSpot { HMONITOR monitor; LONG left; LONG top; };
void FillPosition(MonitorIdentity& identity, HMONITOR monitor) {
    std::vector<MonitorSpot> spots;
    EnumDisplayMonitors(nullptr, nullptr,
        [](HMONITOR handle, HDC, LPRECT, LPARAM data) -> BOOL {
            MONITORINFO info{};
            info.cbSize = sizeof(info);
            if (GetMonitorInfoW(handle, &info))
                reinterpret_cast<std::vector<MonitorSpot>*>(data)->push_back(
                    {handle, info.rcMonitor.left, info.rcMonitor.top});
            return TRUE;
        }, reinterpret_cast<LPARAM>(&spots));
    std::sort(spots.begin(), spots.end(), [](const MonitorSpot& a, const MonitorSpot& b) {
        return a.left != b.left ? a.left < b.left : a.top < b.top;
    });
    identity.count = static_cast<int>(spots.size());
    for (size_t i = 0; i < spots.size(); ++i)
        if (spots[i].monitor == monitor) identity.position = static_cast<int>(i);
}

int ReadPosition(const wchar_t* key) {
    PCWSTR value = Wh_GetStringSetting(key);
    int result = kPositionNone;
    if (value && wcscmp(value, L"last") == 0) result = kPositionLast;
    else if (value && wcsncmp(value, L"pos", 3) == 0 && value[3] >= L'1' &&
             value[3] <= L'0' + kMaxPositions && !value[4])
        result = value[3] - L'1';
    if (value) Wh_FreeStringSetting(value);
    return result;
}

void LoadSettings() {
    Settings next;
    next.primary = ReadAlignment(L"primaryAlignment");
    next.secondary = ReadAlignment(L"secondaryAlignment");
    next.primaryStart = ReadStartAlignment(L"primaryStartAlignment");
    next.secondaryStart = ReadStartAlignment(L"secondaryStartAlignment");
    // Windhawk arrays are contiguous. Missing entries return an empty string.
    // Read the array length via a sentinel: blank device entries are allowed,
    // so inspect alignment as well (every real row has a non-empty selection).
    for (int i = 0; i < 256; ++i) {
        wchar_t deviceKey[64], alignmentKey[64];
        swprintf_s(deviceKey, L"monitors[%d].device", i);
        swprintf_s(alignmentKey, L"monitors[%d].alignment", i);
        PCWSTR marker = Wh_GetStringSetting(alignmentKey);
        bool exists = marker && *marker;
        if (marker) Wh_FreeStringSetting(marker);
        if (!exists) break;
        PCWSTR device = Wh_GetStringSetting(deviceKey);
        wchar_t startKey[64], monitorKey[64];
        swprintf_s(startKey, L"monitors[%d].startAlignment", i);
        swprintf_s(monitorKey, L"monitors[%d].monitor", i);
        const int position = ReadPosition(monitorKey);
        // A row names its monitor by position, or else by a typed identifier.
        if (position != kPositionNone || (device && *device))
            next.monitors.push_back({position != kPositionNone || !device ? L"" : device,
                                     ReadAlignment(alignmentKey), ReadStartAlignment(startKey),
                                     position});
        if (device) Wh_FreeStringSetting(device);
    }
    for (const auto& rule : next.monitors) {
        if (rule.position != kPositionNone) next.usesPosition = true;
        else if (!LooksLikeDeviceName(rule.identifier)) next.extendedIdentifiers = true;
    }
    std::lock_guard lock(g_mutex);
    g_settings = std::move(next);
    // Display identifiers are only consulted through the new rules.
    g_displayNamesValid = false;
}

HWND WindowFromSite(IUnknown* site) {
    if (!site) return nullptr;
    IOleWindow* window = nullptr;
    if (FAILED(site->QueryInterface(IID_IOleWindow, reinterpret_cast<void**>(&window))))
        return nullptr;
    HWND hwnd = nullptr;
    HRESULT hr = window->GetWindow(&hwnd);
    window->Release();
    return SUCCEEDED(hr) && hwnd ? GetAncestor(hwnd, GA_ROOT) : nullptr;
}

bool OwnWindow(HWND hwnd) {
    if (!IsWindow(hwnd)) return false;
    DWORD process = 0;
    GetWindowThreadProcessId(hwnd, &process);
    return process == GetCurrentProcessId();
}

// Returns the taskbar window for a settings object, or null when the object is
// untracked or no window can be resolved. COM calls are made outside g_mutex,
// with the site kept alive by an extra reference taken under the lock.
HWND ResolveWindow(void* self) {
    IUnknown* site = nullptr;
    HWND cached = nullptr;
    {
        std::lock_guard lock(g_mutex);
        auto entry = g_taskbars.find(self);
        if (entry == g_taskbars.end()) return nullptr;
        cached = entry->second.hwnd;
        site = entry->second.site;
        if (site) site->AddRef();
    }
    HWND hwnd = cached;
    if (!OwnWindow(hwnd)) {
        hwnd = WindowFromSite(site);
        if (!OwnWindow(hwnd)) hwnd = nullptr;
        std::lock_guard lock(g_mutex);
        auto entry = g_taskbars.find(self);
        if (entry != g_taskbars.end()) entry->second.hwnd = hwnd;
    }
    if (site) site->Release();
    return hwnd;
}

using SettingsCtor = void* (WINAPI*)(void*, void*, IUnknown*);
SettingsCtor g_ctor;
void* WINAPI SettingsCtorHook(void* self, void* host, IUnknown* site) {
    if (site) site->AddRef();
    {
        std::lock_guard lock(g_mutex);
        // Register before construction: Alignment can be read by the ctor.
        auto& entry = g_taskbars[self];
        if (entry.site) entry.site->Release();  // Reused address, dtor not seen.
        entry = {site, nullptr};
    }
    // The site's window may not exist yet; ResolveWindow retries on each read.
    HWND hwnd = ResolveWindow(self);
    void* result = g_ctor(self, host, site);
    void* iface = FindSettingsInterface(self);
    {
        std::lock_guard lock(g_mutex);
        if (iface) g_interfaces[iface] = self;
    }
    Wh_Log(L"Taskbar settings %p -> site %p, window %p, alignment interface %p",
           self, site, hwnd, iface);
    if (!iface)
        Wh_Log(L"Interface layout not recognized; only direct alignment reads can be customized.");
    return result;
}

using SettingsDtor = void (WINAPI*)(void*);
SettingsDtor g_dtor;
void WINAPI SettingsDtorHook(void* self) {
    IUnknown* site = nullptr;
    {
        std::lock_guard lock(g_mutex);
        auto entry = g_taskbars.find(self);
        if (entry != g_taskbars.end()) {
            site = entry->second.site;
            g_taskbars.erase(entry);
        }
        for (auto i = g_interfaces.begin(); i != g_interfaces.end();) {
            if (i->second == self) i = g_interfaces.erase(i);
            else ++i;
        }
    }
    if (site) site->Release();
    g_dtor(self);
}

// Defined with the rest of the layout code below.
void RequestLayout(HWND hwnd, bool wantRight, int startMode);

// The placement configured for whichever monitor this taskbar is on now.
Placement PlacementForWindow(HWND hwnd) {
    auto monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
    MONITORINFOEXW info{};
    info.cbSize = sizeof(info);
    if (!monitor || !GetMonitorInfoW(monitor, &info)) return {};
    MonitorIdentity identity;
    identity.device = NormalizeIdentifier(Bounded(info.szDevice, ARRAYSIZE(info.szDevice)));
    bool usesPosition;
    {
        std::lock_guard lock(g_mutex);
        usesPosition = g_settings.usesPosition;
    }
    if (usesPosition) FillPosition(identity, monitor);
    std::lock_guard lock(g_mutex);
    if (g_settings.extendedIdentifiers) FillExtendedIdentity(identity);
    return SelectPlacement(g_settings, identity, (info.dwFlags & MONITORINFOF_PRIMARY) != 0);
}

// Asks for the taskbar's layout to match its placement. Right icons and a Start
// button away from the icon group are applied to the taskbar's XAML.
void ApplyPlacement(HWND hwnd, const Placement& placement) {
    int windowsAlignment = kCenter;
    {
        std::lock_guard lock(g_mutex);
        for (const auto& entry : g_taskbars)
            if (entry.second.hwnd == hwnd) { windowsAlignment = entry.second.windowsAlignment; break; }
    }
    const int effectiveIcons = placement.icons >= 0 ? placement.icons : windowsAlignment;
    RequestLayout(hwnd, placement.icons == kRight, ResolveStartMode(placement.start, effectiveIcons));
}

using AlignmentGetter = int (WINAPI*)(void*);
AlignmentGetter g_alignment;
int OverrideAlignment(void* self, int original) {
    if (g_unloading) return original;
    HWND hwnd = ResolveWindow(self);
    if (!hwnd) return original;
    const Placement placement = PlacementForWindow(hwnd);
    const int selected = placement.icons;
    if (selected == kWindowsDefault) {
        // No override: this is Windows' own answer, which the Start button's
        // placement may depend on.
        std::lock_guard lock(g_mutex);
        auto entry = g_taskbars.find(self);
        if (entry != g_taskbars.end()) entry->second.windowsAlignment = original;
    }
    ApplyPlacement(hwnd, placement);
    // Right is this mod's own: Windows is told Left, and the icons are moved to
    // the right in the taskbar's layout instead.
    if (selected == kRight) return kLeft;
    return selected == kWindowsDefault ? original : selected;
}

int WINAPI AlignmentHook(void* self) {
    return OverrideAlignment(self, g_alignment(self));
}

using AbiAlignmentGetter = HRESULT (WINAPI*)(void*, int*);
AbiAlignmentGetter g_abiAlignment;
HRESULT WINAPI AbiAlignmentHook(void* iface, int* value) {
    // The original reaches the direct getter internally, so a COM read can pass
    // through both hooks. Overriding twice is harmless: the answer comes from the
    // monitor, not from the value being overridden.
    HRESULT result = g_abiAlignment(iface, value);
    if (FAILED(result) || !value || g_unloading) return result;
    void* self = nullptr;
    {
        std::lock_guard lock(g_mutex);
        auto entry = g_interfaces.find(iface);
        if (entry != g_interfaces.end()) self = entry->second;
    }
    if (self) *value = OverrideAlignment(self, *value);
    return result;
}

// ---------------------------------------------------------------------------
// Right alignment
//
// Windows has no Right alignment to ask for, so the icons are moved by setting
// HorizontalAlignment on the taskbar's own item repeater. Reaching it needs the
// taskbar host belonging to this taskbar: ITaskListWndSite, the site the settings
// object was constructed with, exposes GetTaskbarHost as a virtual, and the host
// holds the XAML element. Every XAML touch happens on the taskbar's own thread,
// through a subclass on the taskbar window.
// ---------------------------------------------------------------------------

using namespace winrt::Windows::UI::Xaml;

constexpr PCWSTR kRepeaterName = L"TaskbarFrameRepeater";

using GetTaskbarHostProc = void*(WINAPI*)(void* self, void** result);
using DecrefProc = void(WINAPI*)(void* self);
// Primary and secondary taskbars use different band and tray classes, each with
// its own GetTaskbarHost; the one to call is whichever the site's vtable holds.
void* g_taskbarHostGetters[4];
void* g_frameHeight;
DecrefProc g_decref;
bool g_rightSupported;

bool ReadPointerAt(const void* address, void** out) {
    SIZE_T read = 0;
    return ReadProcessMemory(GetCurrentProcess(), address, out, sizeof(*out), &read) &&
           read == sizeof(*out);
}

// TaskbarHost::FrameHeight begins by advancing this to the XAML element the host
// holds, which is where that element's offset is read from rather than assumed.
size_t TaskbarElementOffset() {
    auto* code = static_cast<const BYTE*>(g_frameHeight);
    //  48 83 EC xx    sub rsp, xx
    //  48 83 C1 xx    add rcx, xx
    if (code && code[0] == 0x48 && code[1] == 0x83 && code[2] == 0xEC &&
        code[4] == 0x48 && code[5] == 0x83 && code[6] == 0xC1 && code[7] <= 0x7f)
        return code[7];
    return 0;
}

FrameworkElement TaskbarElementFromSite(IUnknown* site) {
    const size_t offset = TaskbarElementOffset();
    void* vtable = nullptr;
    if (!site || !offset || !g_decref || !ReadPointerAt(site, &vtable) || !vtable)
        return nullptr;
    GetTaskbarHostProc getTaskbarHost = nullptr;
    for (size_t slot = 0; slot < 64 && !getTaskbarHost; ++slot) {
        void* entry = nullptr;
        if (!ReadPointerAt(static_cast<void**>(vtable) + slot, &entry)) break;
        for (void* candidate : g_taskbarHostGetters)
            if (candidate && entry == candidate)
                getTaskbarHost = reinterpret_cast<GetTaskbarHostProc>(entry);
    }
    if (!getTaskbarHost) return nullptr;
    // A shared_ptr comes back by value: the object, then its control block.
    void* host[2]{};
    getTaskbarHost(site, host);
    FrameworkElement element{nullptr};
    void* unknown = nullptr;
    if (host[0] && ReadPointerAt(static_cast<BYTE*>(host[0]) + offset, &unknown) && unknown)
        static_cast<IUnknown*>(unknown)->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                                        winrt::put_abi(element));
    if (host[1]) g_decref(host[1]);
    return element;
}

FrameworkElement FindDescendantByClass(const FrameworkElement& element, PCWSTR className,
                                       int depth) {
    if (!element || depth <= 0) return nullptr;
    const int count = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; ++i) {
        auto child = Media::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (std::wstring_view(winrt::get_class_name(child)) == className) return child;
        if (auto found = FindDescendantByClass(child, className, depth - 1)) return found;
    }
    return nullptr;
}

FrameworkElement FindDescendantByName(const FrameworkElement& element, PCWSTR name, int depth) {
    if (!element || depth <= 0) return nullptr;
    const int count = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; ++i) {
        auto child = Media::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (std::wstring_view(child.Name()) == name) return child;
        if (auto found = FindDescendantByName(child, name, depth - 1)) return found;
    }
    return nullptr;
}

constexpr PCWSTR kTrayClassName = L"SystemTray.SystemTrayFrame";
constexpr PCWSTR kStartButtonClassName = L"Taskbar.ExperienceToggleButton";
constexpr PCWSTR kStartButtonAutomationId = L"StartButton";

FrameworkElement FindStartButton(const FrameworkElement& element, int depth) {
    if (!element || depth <= 0) return nullptr;
    const int count = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; ++i) {
        auto child = Media::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (std::wstring_view(winrt::get_class_name(child)) == kStartButtonClassName &&
            std::wstring_view(Automation::AutomationProperties::GetAutomationId(child)) ==
                kStartButtonAutomationId)
            return child;
        if (auto found = FindStartButton(child, depth - 1)) return found;
    }
    return nullptr;
}

// What one taskbar has been changed to, so it can be put back exactly.
struct LayoutState {
    winrt::weak_ref<FrameworkElement> top;
    winrt::weak_ref<FrameworkElement> repeater;
    winrt::weak_ref<FrameworkElement> tray;
    winrt::weak_ref<FrameworkElement> start;
    HorizontalAlignment originalAlignment = HorizontalAlignment::Stretch;
    Thickness originalMargin{};
    Thickness startOriginalMargin{};
    Media::Transform startOriginalTransform{nullptr};
    Media::TranslateTransform startTransform{nullptr};
    int64_t alignmentCallback = 0;
    winrt::event_token trayResized{};
    winrt::event_token layoutUpdated{};
    bool wantRight = false;
    int wantStart = kFollowIcons;
    bool rightApplied = false;
    bool startApplied = false;
};
// Touched only from a taskbar's own thread. Recursive because writing a property
// calls back into the handlers below on the same thread.
std::recursive_mutex g_xamlMutex;
std::unordered_map<HWND, LayoutState> g_layouts;

bool SameThickness(const Thickness& a, const Thickness& b) {
    return a.Left == b.Left && a.Top == b.Top && a.Right == b.Right && a.Bottom == b.Bottom;
}

// Brings the taskbar's XAML in line with what is wanted. Safe to call repeatedly:
// it only writes a property whose value would change, so it can run on every
// layout pass without feeding back into layout.
void ReapplyLayout(LayoutState& state) {
    auto repeater = state.repeater.get();
    auto top = state.top.get();
    if (!repeater || !top) return;
    auto tray = state.tray.get();
    const double trayWidth = tray ? tray.ActualWidth() : 0;

    // Icons. The item area spans the whole taskbar, tray included, so right-aligned
    // icons reserve the tray's width as margin.
    if (state.wantRight) {
        if (repeater.HorizontalAlignment() != HorizontalAlignment::Right)
            repeater.HorizontalAlignment(HorizontalAlignment::Right);
        Thickness margin = state.originalMargin;
        margin.Right += trayWidth;
        if (!SameThickness(repeater.Margin(), margin)) repeater.Margin(margin);
        state.rightApplied = true;
    } else if (state.rightApplied) {
        repeater.HorizontalAlignment(state.originalAlignment);
        repeater.Margin(state.originalMargin);
        state.rightApplied = false;
    }

    // Start button. Its slot is taken out of the icon group with a negative margin,
    // so the icons close the gap, and the button is moved to its edge with a render
    // transform, which doesn't affect layout.
    auto start = state.start.get();
    if (!start) return;
    if (state.wantStart != kFollowIcons) {
        if (!state.startApplied) {
            state.startOriginalMargin = start.Margin();
            state.startOriginalTransform = start.RenderTransform();
            state.startTransform = Media::TranslateTransform();
            start.RenderTransform(state.startTransform);
            state.startApplied = true;
        }
        // ActualWidth excludes margins, so it doesn't change with the one set below.
        const double width = start.ActualWidth();
        Thickness margin = state.startOriginalMargin;
        margin.Right -= width;
        if (!SameThickness(start.Margin(), margin)) start.Margin(margin);

        const double slot = start.TransformToVisual(top).TransformPoint({0, 0}).X -
                            state.startTransform.X();
        const double available = top.ActualWidth();
        const double target = state.wantStart == kLeft ? 0 : available - trayWidth - width;
        const double offset = target - slot;
        if (std::abs(offset - state.startTransform.X()) > 0.5) state.startTransform.X(offset);
    } else if (state.startApplied) {
        start.Margin(state.startOriginalMargin);
        start.RenderTransform(state.startOriginalTransform);
        state.startApplied = false;
    }
}

// Windows reassigns the alignment whenever its own setting is applied; put it back.
void OnAlignmentPropertyChanged(DependencyObject sender, DependencyProperty) {
    if (g_unloading) return;
    auto element = sender.try_as<FrameworkElement>();
    if (!element) return;
    std::lock_guard lock(g_xamlMutex);
    for (auto& entry : g_layouts) {
        if (entry.second.wantRight && entry.second.repeater.get() == element) {
            ReapplyLayout(entry.second);
            return;
        }
    }
}

IUnknown* AcquireSiteForWindow(HWND hwnd) {
    std::lock_guard lock(g_mutex);
    for (auto& entry : g_taskbars) {
        if (entry.second.hwnd == hwnd && entry.second.site) {
            entry.second.site->AddRef();
            return entry.second.site;
        }
    }
    return nullptr;
}

void ReleaseLayoutHandlers(LayoutState& state) {
    auto repeater = state.repeater.get();
    if (repeater && state.alignmentCallback)
        repeater.UnregisterPropertyChangedCallback(
            FrameworkElement::HorizontalAlignmentProperty(), state.alignmentCallback);
    if (repeater && state.layoutUpdated.value) repeater.LayoutUpdated(state.layoutUpdated);
    if (auto tray = state.tray.get())
        if (state.trayResized.value) tray.SizeChanged(state.trayResized);
}

// Runs on the taskbar's own thread. Returns false when the taskbar's XAML isn't
// built yet and the call should be repeated shortly; the tree is empty while a
// taskbar is still being created.
bool ApplyLayout(HWND hwnd, bool wantRight, int wantStart) {
    std::lock_guard lock(g_xamlMutex);
    auto entry = g_layouts.find(hwnd);
    if (entry == g_layouts.end()) {
        if (!wantRight && wantStart == kFollowIcons) return true;  // Nothing to undo.
        IUnknown* site = AcquireSiteForWindow(hwnd);
        auto root = TaskbarElementFromSite(site);
        if (site) site->Release();
        if (!root) return false;
        // The host's element may sit anywhere in the tree; search from the top.
        FrameworkElement top = root;
        if (auto content = root.XamlRoot() ? root.XamlRoot().Content().try_as<FrameworkElement>()
                                           : nullptr)
            top = content;
        auto repeater = FindDescendantByName(top, kRepeaterName, 64);
        if (!repeater) return false;
        LayoutState state;
        state.top = winrt::make_weak(top);
        state.repeater = winrt::make_weak(repeater);
        state.originalAlignment = repeater.HorizontalAlignment();
        state.originalMargin = repeater.Margin();
        if (auto tray = FindDescendantByClass(top, kTrayClassName, 64)) {
            state.tray = winrt::make_weak(tray);
            state.trayResized = tray.SizeChanged([hwnd](auto const&, auto const&) {
                std::lock_guard lock(g_xamlMutex);
                auto found = g_layouts.find(hwnd);
                if (found != g_layouts.end()) ReapplyLayout(found->second);
            });
        }
        if (auto start = FindStartButton(top, 64)) state.start = winrt::make_weak(start);
        state.layoutUpdated = repeater.LayoutUpdated([hwnd](auto const&, auto const&) {
            std::lock_guard lock(g_xamlMutex);
            auto found = g_layouts.find(hwnd);
            if (found != g_layouts.end()) ReapplyLayout(found->second);
        });
        state.alignmentCallback = repeater.RegisterPropertyChangedCallback(
            FrameworkElement::HorizontalAlignmentProperty(), OnAlignmentPropertyChanged);
        entry = g_layouts.emplace(hwnd, std::move(state)).first;
        Wh_Log(L"Taskbar layout ready for window %p (start button %s)", hwnd,
               entry->second.start.get() ? L"found" : L"missing");
    }
    LayoutState& state = entry->second;
    if (!state.repeater.get() || !state.top.get()) {  // The taskbar's XAML tree is gone.
        g_layouts.erase(entry);
        return !wantRight && wantStart == kFollowIcons;
    }
    // The Start button may not exist yet when the repeater already does.
    if (!state.start.get() && wantStart != kFollowIcons) {
        if (auto start = FindStartButton(state.top.get(), 64))
            state.start = winrt::make_weak(start);
        else
            return false;
    }
    state.wantRight = wantRight;
    state.wantStart = wantStart;
    ReapplyLayout(state);
    if (!wantRight && wantStart == kFollowIcons) {
        ReleaseLayoutHandlers(state);
        g_layouts.erase(entry);
    }
    return true;
}

UINT g_applyMessage;
// Taskbar windows this mod has subclassed, with the last request posted to each.
// The value packs the wanted layout, see PackLayout.
struct ApplyRequest { bool pending = false; int value = -1; int attempts = 0; };
constexpr UINT_PTR kRetryTimerId = 0x7B41;
constexpr UINT kRetryIntervalMs = 400;
constexpr int kMaxAttempts = 50;
std::unordered_map<HWND, ApplyRequest> g_applyRequests;

LRESULT CALLBACK TaskbarSubclassProc(HWND hwnd, UINT message, WPARAM wParam,
                                     LPARAM lParam, DWORD_PTR) {
    const bool isApply = g_applyMessage && message == g_applyMessage;
    const bool isRetry = message == WM_TIMER && wParam == kRetryTimerId;
    if (isApply || isRetry) {
        if (isRetry) KillTimer(hwnd, kRetryTimerId);
        int packed = static_cast<int>(lParam);
        int attempt = 0;
        {
            std::lock_guard lock(g_mutex);
            auto entry = g_applyRequests.find(hwnd);
            if (entry != g_applyRequests.end()) {
                if (isApply) {
                    entry->second.pending = false;
                    entry->second.attempts = 0;
                    entry->second.value = packed;
                }
                packed = entry->second.value;
                attempt = entry->second.attempts++;
            } else if (isRetry) {
                return 0;
            }
        }
        const bool wantRight = LayoutWantsRight(packed);
        const int wantStart = LayoutStartMode(packed);
        if (!ApplyLayout(hwnd, wantRight, wantStart) && packed != 0 &&
            attempt < kMaxAttempts && !g_unloading)
            SetTimer(hwnd, kRetryTimerId, kRetryIntervalMs, nullptr);
        return 0;
    }
    if (message == WM_TIMER && wParam == kRetryTimerId) return 0;
    return DefSubclassProc(hwnd, message, wParam, lParam);
}

// Callable from any thread, including from inside an alignment read: the work
// itself is posted, so no XAML is touched here.
void RequestLayout(HWND hwnd, bool wantRight, int startMode) {
    const int packed = PackLayout(wantRight, startMode);
    if (!g_applyMessage || (packed != 0 && !g_rightSupported)) return;
    std::lock_guard lock(g_mutex);
    auto entry = g_applyRequests.find(hwnd);
    if (entry == g_applyRequests.end()) {
        // Nothing to restore for a taskbar that was never changed.
        if (packed == 0) return;
        if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, TaskbarSubclassProc, 0)) {
            Wh_Log(L"Subclassing taskbar window %p failed", hwnd);
            return;
        }
        entry = g_applyRequests.emplace(hwnd, ApplyRequest{}).first;
    }
    if (entry->second.pending && entry->second.value == packed) return;
    entry->second.pending = true;
    entry->second.value = packed;
    PostMessageW(hwnd, g_applyMessage, 0, packed);
}

void RefreshTaskbars() {
    // Synchronous, bounded delivery: no dangling string pointer after unload.
    EnumWindows([](HWND hwnd, LPARAM) -> BOOL {
        DWORD process = 0;
        GetWindowThreadProcessId(hwnd, &process);
        wchar_t name[64]{};
        if (process == GetCurrentProcessId() && GetClassNameW(hwnd, name, 64) &&
            (wcscmp(name, L"Shell_TrayWnd") == 0 ||
             wcscmp(name, L"Shell_SecondaryTrayWnd") == 0)) {
            DWORD_PTR result;
            SendMessageTimeoutW(hwnd, WM_SETTINGCHANGE, 0,
                reinterpret_cast<LPARAM>(L"TraySettings"),
                SMTO_ABORTIFHUNG | SMTO_BLOCK, 2000, &result);
        }
        return TRUE;
    }, 0);
}

BOOL Wh_ModInit() {
    LoadSettings();
    HMODULE module = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) return FALSE;
    // Symbols verified in Microsoft's public Taskbar PDB, given in the undecorated form
    // Windhawk resolves and caches online.
    // Hook the implementation getter, whose this pointer matches the ctor/dtor;
    // the COM ABI getter uses a different interface pointer and cannot be keyed
    // by the implementation pointer without an adjustment. Both COM entries are
    // optional: without them the direct getter still drives the layout.
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskbarSettings,struct winrt::WindowsUdk::UI::Shell::ITaskbarSettings>::`vftable')"}, &g_settingsInterfaceVtable, nullptr, true},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskbarSettings,struct winrt::WindowsUdk::UI::Shell::ITaskbarSettings>::get_Alignment(int *))"}, &g_abiAlignment, AbiAlignmentHook, true},
        {{LR"(public: __cdecl winrt::WindowsUdk::UI::Shell::implementation::TaskbarSettings::TaskbarSettings(struct ITrayComponentHost *,struct ITaskListWndSite *))"}, &g_ctor, SettingsCtorHook},
        {{LR"(public: virtual __cdecl winrt::WindowsUdk::UI::Shell::implementation::TaskbarSettings::~TaskbarSettings(void))"}, &g_dtor, SettingsDtorHook},
        {{LR"(public: enum winrt::WindowsUdk::UI::Shell::TaskbarAlignment __cdecl winrt::WindowsUdk::UI::Shell::implementation::TaskbarSettings::Alignment(void))"}, &g_alignment, AlignmentHook},
        // Right alignment only. Each is optional: without them Left and Center
        // still work and a Right selection falls back to Windows' Left.
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"}, &g_taskbarHostGetters[0], nullptr, true},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"}, &g_taskbarHostGetters[1], nullptr, true},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTray::GetTaskbarHost(void)const )"}, &g_taskbarHostGetters[2], nullptr, true},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl TrayUI::GetTaskbarHost(void)const )"}, &g_taskbarHostGetters[3], nullptr, true},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"}, &g_frameHeight, nullptr, true},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"}, &g_decref, nullptr, true},
    };
    bool ok = WindhawkUtils::HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
    // Keep taskbar.dll loaded for the hook lifetime, even during early injection.
    if (!ok) {
        FreeLibrary(module);
        Wh_Log(L"Unsupported taskbar.dll: required symbols could not be resolved.");
        return FALSE;
    }
    if (!g_settingsInterfaceVtable || !g_abiAlignment)
        Wh_Log(L"COM alignment getter unavailable on this build; using the direct getter only.");
    g_applyMessage = RegisterWindowMessageW(L"Windhawk_TaskbarPerMonitorAlignment_Apply");
    bool anyHostGetter = false;
    for (void* getter : g_taskbarHostGetters) anyHostGetter = anyHostGetter || getter;
    g_rightSupported = g_applyMessage && anyHostGetter && g_decref && TaskbarElementOffset();
    if (!g_rightSupported)
        Wh_Log(L"Right alignment unavailable on this build; Right behaves as Left.");
    Wh_Log(L"Enabled. Restart Explorer once to capture existing taskbars.");
    return TRUE;
}

// Alignment isn't necessarily read again after a settings change, so the taskbars
// whose layout is handled in XAML are updated directly as well.
void RefreshLayouts() {
    std::vector<HWND> windows;
    {
        std::lock_guard lock(g_mutex);
        for (const auto& entry : g_taskbars)
            if (entry.second.hwnd) windows.push_back(entry.second.hwnd);
        for (const auto& entry : g_applyRequests) windows.push_back(entry.first);
    }
    std::sort(windows.begin(), windows.end());
    windows.erase(std::unique(windows.begin(), windows.end()), windows.end());
    for (HWND hwnd : windows)
        if (IsWindow(hwnd)) ApplyPlacement(hwnd, PlacementForWindow(hwnd));
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    RefreshTaskbars();
    RefreshLayouts();
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    // Put each right-aligned taskbar back, synchronously, before the subclass
    // and this module go away. The windows are collected first so no lock is
    // held while waiting on another thread.
    std::vector<HWND> subclassed;
    {
        std::lock_guard lock(g_mutex);
        for (const auto& entry : g_applyRequests) subclassed.push_back(entry.first);
        g_applyRequests.clear();
    }
    for (HWND hwnd : subclassed) {
        DWORD_PTR result = 0;
        SendMessageTimeoutW(hwnd, g_applyMessage, 0, 0,
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 2000, &result);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, TaskbarSubclassProc);
    }
    // Give back the site references; the destructor hook stops running shortly.
    std::unordered_map<void*, TaskbarEntry> taskbars;
    {
        std::lock_guard lock(g_mutex);
        taskbars.swap(g_taskbars);
        g_interfaces.clear();
    }
    for (auto& item : taskbars)
        if (item.second.site) item.second.site->Release();
    RefreshTaskbars();
}
