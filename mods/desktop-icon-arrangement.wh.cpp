// ==WindhawkMod==
// @id              desktop-icon-arrangement
// @name            Desktop Icon Arrangement
// @description     Preserve existing desktop icon positions and place new icons in free columns to the right.
// @version         0.1.0
// @author          Nerdworld
// @github          https://github.com/nerdworldDE
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lshell32 -luuid
// @license         MIT
// ==/WindhawkMod==

// SPDX-License-Identifier: MIT

// ==WindhawkModReadme==
/*
# Desktop Icon Arrangement

Experimental mod for Windows 11, x64. Turn off **View > Auto arrange icons**
on the desktop before enabling it. **Align icons to grid** can stay enabled.

The mod remembers the current layout. When a new desktop item appears, it
restores the positions of existing items and puts the new item in a free column
to their right. Further new items fill this area from top to bottom, then move
to the next column. If that area is full, it tries other monitors and free gaps.
When there is insufficient space for a whole batch, Windows keeps its layout.

This also applies to files dropped on the desktop: their drop location is
replaced by the configured placement. Moving existing icons remains possible.
Changing **First column for new icons** lets you choose a fixed starting column;
0 calculates it from your arrangement when the mod is enabled.

This is a corrective mod, not an interception of Explorer's insertion code.
Icons may briefly move before the next check (250 ms by default). A short
follow-up period corrects delayed rearrangements by Explorer. Avoid manually
moving icons during that period. Positions are kept in memory, and Explorer
stores changes through its normal desktop API. Disabling the mod stops future
corrections and leaves the current arrangement in place.

Icon size, monitor layout, grid spacing and auto-arrange changes establish a
fresh baseline. The mod does not restore layouts from earlier Explorer sessions.
Renames reported by Shell notifications retain their positions; undelivered or
late rename notifications can make a renamed item look like a new item.

The submitter confirmed the desired desktop behavior on Windows 11 x64. Other
Windows builds, Windows on ARM and the wider set of scenarios above still need
testing.
Enable logging for this mod in Windhawk to see connection, correction and
error messages.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- pollIntervalMs: 250
  $name: Check interval (milliseconds)
  $name:de-DE: Prüfintervall (Millisekunden)
  $description: "100–2000. Lower values reduce the delay but query Explorer more often."
- firstNewColumn: 0
  $name: First column for new icons
  $name:de-DE: Erste Spalte für neue Symbole
  $description: "0 = automatically to the right of the initial layout; 1 = leftmost column, 2 = second column, etc. Occupied positions are skipped."
- settleTimeMs: 1000
  $name: Follow-up time (milliseconds)
  $name:de-DE: Nachkorrekturzeit (Millisekunden)
  $description: "0–5000. Correct delayed rearrangements by Explorer after a new item appears."
*/
// ==/WindhawkModSettings==

// The same code is included by the portable regression tests. Windhawk only
// needs this single .wh.cpp file; no local headers or test files are required.
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace arrangement {

struct Point {
    int x = 0;
    int y = 0;
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

struct Rect {
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;
    bool operator==(const Rect& other) const {
        return left == other.left && top == other.top &&
               right == other.right && bottom == other.bottom;
    }
    bool Contains(Point p) const {
        return p.x >= left && p.x < right && p.y >= top && p.y < bottom;
    }
};

using Positions = std::map<std::wstring, Point>;

struct Grid {
    Rect bounds;
    Point origin;
    Point spacing;
    int firstColumn = 0;
};

int PositiveMod(int value, int divisor) {
    int result = value % divisor;
    return result < 0 ? result + divisor : result;
}

Grid MakeGrid(Rect bounds, Point spacing, const Positions& baseline,
              int configuredColumn) {
    Point reference{bounds.left, bounds.top};
    bool found = false;
    for (const auto& [key, position] : baseline) {
        if (bounds.Contains(position) &&
            (!found || position.x < reference.x ||
             (position.x == reference.x && position.y < reference.y))) {
            reference = position;
            found = true;
        }
    }
    Grid grid{bounds,
              {bounds.left + PositiveMod(reference.x - bounds.left, spacing.x),
               bounds.top + PositiveMod(reference.y - bounds.top, spacing.y)},
              spacing};
    if (configuredColumn > 0) {
        grid.firstColumn = configuredColumn - 1;
    } else {
        for (const auto& [key, position] : baseline) {
            if (bounds.Contains(position)) {
                int column = (position.x - grid.origin.x) / spacing.x;
                // With grid alignment disabled, also reserve a partially
                // occupied column on the right.
                if (position.x > grid.origin.x &&
                    (position.x - grid.origin.x) % spacing.x != 0) {
                    ++column;
                }
                grid.firstColumn = std::max(grid.firstColumn, column + 1);
            }
        }
    }
    return grid;
}

std::optional<Point> FindFreePosition(const std::vector<Grid>& grids,
                                      const Positions& occupied) {
    // First use each monitor's preferred area, then look for gaps elsewhere.
    // A rectangle, rather than a single virtual-screen bounding box, prevents
    // placement in the invisible gaps between differently sized monitors.
    for (bool fallback : {false, true}) {
        for (const Grid& grid : grids) {
            if (grid.spacing.x <= 0 || grid.spacing.y <= 0) {
                continue;
            }
            int columns = (grid.bounds.right - grid.origin.x) / grid.spacing.x;
            int rows = (grid.bounds.bottom - grid.origin.y) / grid.spacing.y;
            int split = std::clamp(grid.firstColumn, 0, std::max(0, columns));
            int begin = fallback ? 0 : split;
            int end = fallback ? split : columns;
            for (int column = begin; column < end; ++column) {
                for (int row = 0; row < rows; ++row) {
                    Point candidate{grid.origin.x + column * grid.spacing.x,
                                    grid.origin.y + row * grid.spacing.y};
                    bool free = true;
                    for (const auto& [key, position] : occupied) {
                        // Be conservative for manually positioned, off-grid
                        // icons, including icons across a monitor boundary.
                        if (std::abs(int64_t(candidate.x) - position.x) <
                                grid.spacing.x &&
                            std::abs(int64_t(candidate.y) - position.y) <
                                grid.spacing.y) {
                            free = false;
                            break;
                        }
                    }
                    if (free) {
                        return candidate;
                    }
                }
            }
        }
    }
    return std::nullopt;
}

bool HasNewItems(const Positions& baseline, const Positions& current) {
    for (const auto& [key, position] : current) {
        if (baseline.find(key) == baseline.end()) {
            return true;
        }
    }
    return false;
}

std::optional<Positions> PlanAddition(const Positions& baseline,
                                      const Positions& current,
                                      const std::vector<Grid>& grids) {
    Positions desired;
    for (const auto& [key, position] : current) {
        auto old = baseline.find(key);
        if (old != baseline.end()) {
            desired.emplace(key, old->second);
        }
    }
    for (const auto& [key, position] : current) {
        if (baseline.find(key) != baseline.end()) {
            continue;
        }
        auto target = FindFreePosition(grids, desired);
        if (!target) {
            // Do not put an existing icon back on top of an unplaced new one.
            return std::nullopt;
        }
        desired.emplace(key, *target);
    }
    return desired;
}

void Rename(Positions& positions, const std::wstring& oldKey,
            const std::wstring& newKey) {
    auto item = positions.find(oldKey);
    if (item != positions.end() && oldKey != newKey) {
        Point position = item->second;
        positions.erase(item);
        positions.insert_or_assign(newKey, position);
    }
}

// Bookkeeping is independent of COM so insertion, rename and delayed layout
// behavior can be exercised by the same tests as the placement calculation.
struct Tracker {
    Positions baseline;
    std::vector<Grid> grids;
    bool initialized = false;
    uint64_t settleUntil = 0;

    void Reset(const Positions& current, const std::vector<Rect>& areas,
               Point spacing, int configuredColumn) {
        baseline = current;
        grids.clear();
        for (Rect area : areas) {
            grids.push_back(MakeGrid(area, spacing, current, configuredColumn));
        }
        initialized = true;
        settleUntil = 0;
    }

    // nullopt means the entire batch cannot fit; an empty map means no moves.
    std::optional<Positions> Observe(const Positions& current, uint64_t now,
                                     unsigned settleMs, bool manualGesture) {
        if (HasNewItems(baseline, current)) {
            auto desired = PlanAddition(baseline, current, grids);
            if (desired) {
                baseline = *desired;
                settleUntil = now + settleMs;
            } else {
                baseline = current;
                settleUntil = 0;
            }
            return desired;
        }
        if (manualGesture || now >= settleUntil) {
            // Ordinary manual movement and sorting become the new baseline.
            baseline = current;
            settleUntil = 0;
            return Positions{};
        }
        Positions desired;
        for (const auto& [key, position] : current) {
            auto old = baseline.find(key);
            if (old != baseline.end()) {
                desired.emplace(key, old->second);
            }
        }
        return desired;
    }
};

}  // namespace arrangement

#ifndef DESKTOP_ICON_ARRANGEMENT_TEST

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <exdisp.h>
#include <servprov.h>
#include <shlobj.h>
#include <shlguid.h>
#include <wrl/client.h>
#include <windhawk_api.h>

#include <atomic>
#include <memory>

namespace {

using Microsoft::WRL::ComPtr;
using arrangement::Point;
using arrangement::Positions;
using arrangement::Rect;

constexpr UINT kShellChange = WM_APP + 1;
std::atomic<int> g_pollInterval{250};
std::atomic<int> g_firstNewColumn{0};
std::atomic<int> g_settleTime{1000};
HANDLE g_stopEvent = nullptr;
HANDLE g_settingsEvent = nullptr;
HANDLE g_worker = nullptr;

struct PidlDeleter {
    void operator()(ITEMIDLIST* pidl) const { CoTaskMemFree(pidl); }
};
using Pidl = std::unique_ptr<ITEMIDLIST, PidlDeleter>;

struct StringDeleter {
    void operator()(wchar_t* value) const { CoTaskMemFree(value); }
};

struct LiveItem {
    std::wstring key;
    Pidl pidl;
    Point position;
};

std::wstring KeyForPidl(PCIDLIST_ABSOLUTE pidl) {
    PWSTR name = nullptr;
    if (FAILED(SHGetNameFromIDList(pidl, SIGDN_DESKTOPABSOLUTEPARSING, &name))) {
        return {};
    }
    std::unique_ptr<wchar_t, StringDeleter> ownedName(name);
    std::wstring key(ownedName.get());
    // Windows Shell paths are normally case-insensitive. Do not use display
    // labels, which can be duplicated and can hide filename extensions.
    int length = LCMapStringEx(LOCALE_NAME_INVARIANT, LCMAP_UPPERCASE,
                               key.data(), static_cast<int>(key.size()),
                               nullptr, 0, nullptr, nullptr, 0);
    if (length > 0) {
        std::wstring normalized(length, L'\0');
        if (LCMapStringEx(LOCALE_NAME_INVARIANT, LCMAP_UPPERCASE,
                          key.data(), static_cast<int>(key.size()),
                          normalized.data(), length, nullptr, nullptr, 0)) {
            key = std::move(normalized);
        }
    }
    return key;
}

struct Desktop {
    ComPtr<IFolderView2> view;
    HWND window = nullptr;
    Pidl root;
};

HRESULT ConnectDesktop(Desktop& desktop) {
    ComPtr<IShellWindows> windows;
    HRESULT hr = CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                   IID_PPV_ARGS(&windows));
    if (FAILED(hr)) return hr;

    VARIANT location{};
    location.vt = VT_I4;
    location.lVal = CSIDL_DESKTOP;
    VARIANT empty{};
    long unusedWindow = 0;
    ComPtr<IDispatch> dispatch;
    hr = windows->FindWindowSW(&location, &empty, SWC_DESKTOP, &unusedWindow,
                               SWFO_NEEDDISPATCH, &dispatch);
    if (hr != S_OK || !dispatch) return FAILED(hr) ? hr : E_PENDING;

    ComPtr<IServiceProvider> provider;
    ComPtr<IShellBrowser> browser;
    ComPtr<IShellView> shellView;
    hr = dispatch.As(&provider);
    if (FAILED(hr)) return hr;
    hr = provider->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&browser));
    if (FAILED(hr)) return hr;
    hr = browser->QueryActiveShellView(&shellView);
    if (FAILED(hr)) return hr;
    hr = shellView.As(&desktop.view);
    if (FAILED(hr)) return hr;
    HWND viewWindow = nullptr;
    hr = shellView->GetWindow(&viewWindow);
    if (FAILED(hr)) return hr;

    DWORD owner = 0;
    GetWindowThreadProcessId(viewWindow, &owner);
    // Separate explorer.exe folder processes must not all run this mod
    // against the desktop hosted by another Explorer process.
    if (owner != GetCurrentProcessId()) return E_ACCESSDENIED;

    desktop.window = FindWindowExW(viewWindow, nullptr, L"SysListView32", nullptr);
    if (!desktop.window) return E_NOINTERFACE;
    PIDLIST_ABSOLUTE root = nullptr;
    hr = SHGetSpecialFolderLocation(nullptr, CSIDL_DESKTOP, &root);
    if (FAILED(hr)) return hr;
    desktop.root.reset(root);
    return S_OK;
}

HRESULT ReadSnapshot(const Desktop& desktop, std::vector<LiveItem>& items,
                     Positions& positions) {
    int count = 0;
    HRESULT hr = desktop.view->ItemCount(SVGIO_ALLVIEW, &count);
    if (FAILED(hr)) return hr;
    if (count < 0 || count > 10000) return E_FAIL;
    ComPtr<IEnumIDList> enumerator;
    hr = desktop.view->Items(SVGIO_ALLVIEW, IID_PPV_ARGS(&enumerator));
    if (FAILED(hr)) return hr;
    for (;;) {
        PITEMID_CHILD child = nullptr;
        hr = enumerator->Next(1, &child, nullptr);
        Pidl pidl(child);
        if (hr == S_FALSE) break;
        if (FAILED(hr) || !pidl) return FAILED(hr) ? hr : E_FAIL;
        if (items.size() >= 10000) return E_FAIL;
        Pidl absolute(ILCombine(desktop.root.get(), pidl.get()));
        if (!absolute) return E_OUTOFMEMORY;
        std::wstring key = KeyForPidl(absolute.get());
        POINT position{};
        hr = desktop.view->GetItemPosition(pidl.get(), &position);
        if (FAILED(hr)) return hr;
        // Some views expose a sentinel position while creating an item.
        if (position.x < -100000 || position.y < -100000 ||
            position.x > 100000 || position.y > 100000) return E_PENDING;
        if (key.empty()) return E_FAIL;
        Point point{position.x, position.y};
        if (!positions.emplace(key, point).second) return E_PENDING;
        items.push_back({std::move(key), std::move(pidl), point});
    }
    int afterCount = 0;
    hr = desktop.view->ItemCount(SVGIO_ALLVIEW, &afterCount);
    if (FAILED(hr)) return hr;
    // COM calls can pump messages. Never adopt a visibly incomplete snapshot
    // during insertion or refresh as the new saved arrangement.
    if (count != afterCount || static_cast<int>(items.size()) != count) {
        return E_PENDING;
    }
    return S_OK;
}

struct Geometry {
    Point spacing;
    int iconSize = 0;
    FOLDERVIEWMODE mode = FVM_AUTO;
    std::vector<Rect> areas;
    bool operator==(const Geometry& other) const {
        return spacing == other.spacing && iconSize == other.iconSize &&
               mode == other.mode && areas == other.areas;
    }
};

struct MonitorArea {
    Rect bounds;
    bool primary;
};

BOOL CALLBACK CollectMonitor(HMONITOR monitor, HDC, LPRECT, LPARAM param) {
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (GetMonitorInfoW(monitor, &info)) {
        auto& areas = *reinterpret_cast<std::vector<MonitorArea>*>(param);
        areas.push_back({{info.rcWork.left, info.rcWork.top, info.rcWork.right,
                          info.rcWork.bottom},
                         (info.dwFlags & MONITORINFOF_PRIMARY) != 0});
    }
    return TRUE;
}

HRESULT ReadGeometry(const Desktop& desktop, Geometry& geometry) {
    POINT spacing{};
    HRESULT hr = desktop.view->GetSpacing(&spacing);
    if (FAILED(hr)) return hr;
    if (spacing.x <= 0 || spacing.y <= 0) return E_PENDING;
    geometry.spacing = {spacing.x, spacing.y};
    hr = desktop.view->GetViewModeAndIconSize(&geometry.mode, &geometry.iconSize);
    if (FAILED(hr)) return hr;

    POINT origin{};
    RECT client{};
    if (!ClientToScreen(desktop.window, &origin) ||
        !GetClientRect(desktop.window, &client)) return E_FAIL;
    std::vector<MonitorArea> monitors;
    if (!EnumDisplayMonitors(nullptr, nullptr, CollectMonitor,
                             reinterpret_cast<LPARAM>(&monitors))) return E_FAIL;
    std::sort(monitors.begin(), monitors.end(), [](const auto& a, const auto& b) {
        if (a.primary != b.primary) return a.primary;
        if (a.bounds.left != b.bounds.left) return a.bounds.left < b.bounds.left;
        return a.bounds.top < b.bounds.top;
    });
    for (const MonitorArea& monitor : monitors) {
        Rect rect{std::max<int>(client.left, monitor.bounds.left - origin.x),
                  std::max<int>(client.top, monitor.bounds.top - origin.y),
                  std::min<int>(client.right, monitor.bounds.right - origin.x),
                  std::min<int>(client.bottom, monitor.bounds.bottom - origin.y)};
        if (rect.right > rect.left && rect.bottom > rect.top) {
            geometry.areas.push_back(rect);
        }
    }
    return geometry.areas.empty() ? E_PENDING : S_OK;
}

struct NotificationQueue {
    std::vector<std::pair<std::wstring, std::wstring>> renames;
};

LRESULT CALLBACK NotificationProc(HWND window, UINT message, WPARAM wParam,
                                  LPARAM lParam) {
    if (message == WM_NCCREATE) {
        auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
        SetWindowLongPtrW(window, GWLP_USERDATA,
                          reinterpret_cast<LONG_PTR>(create->lpCreateParams));
    }
    if (message == kShellChange) {
        auto* queue = reinterpret_cast<NotificationQueue*>(
            GetWindowLongPtrW(window, GWLP_USERDATA));
        PIDLIST_ABSOLUTE* pidls = nullptr;
        LONG event = 0;
        HANDLE lock = SHChangeNotification_Lock(reinterpret_cast<HANDLE>(wParam),
                                                static_cast<DWORD>(lParam),
                                                &pidls, &event);
        if (lock) {
            try {
                if (queue && (event & (SHCNE_RENAMEITEM | SHCNE_RENAMEFOLDER)) &&
                    pidls && pidls[0] && pidls[1]) {
                    std::wstring oldKey = KeyForPidl(pidls[0]);
                    std::wstring newKey = KeyForPidl(pidls[1]);
                    if (!oldKey.empty() && !newKey.empty()) {
                        queue->renames.emplace_back(std::move(oldKey),
                                                    std::move(newKey));
                    }
                }
            } catch (...) {
                Wh_Log(L"Could not queue a desktop rename notification");
            }
            SHChangeNotification_Unlock(lock);
        }
        return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

HRESULT ApplyPositions(const Desktop& desktop, const std::vector<LiveItem>& items,
                       const Positions& desired) {
    std::vector<PCUITEMID_CHILD> pidls;
    std::vector<POINT> points;
    for (const LiveItem& item : items) {
        auto target = desired.find(item.key);
        if (target != desired.end() && !(target->second == item.position)) {
            pidls.push_back(item.pidl.get());
            points.push_back({target->second.x, target->second.y});
        }
    }
    if (pidls.empty()) return S_OK;
    // SVSI_POSITIONITEM alone leaves selection and focus with Explorer. Avoid
    // LVM_SETITEMPOSITION: it bypasses the Shell's persistent layout manager.
    HRESULT hr = desktop.view->SelectAndPositionItems(
        static_cast<UINT>(pidls.size()), pidls.data(), points.data(),
        SVSI_POSITIONITEM);
    Wh_Log(L"Positioned %u desktop icons, result 0x%08X",
            static_cast<unsigned>(pidls.size()), static_cast<unsigned>(hr));
    return hr;
}

struct NotificationWindow {
    HINSTANCE instance;
    const wchar_t* className;
    HWND window = nullptr;
    ~NotificationWindow() {
        if (window) DestroyWindow(window);
        UnregisterClassW(className, instance);
    }
};

struct ShellRegistration {
    ULONG id = 0;
    ~ShellRegistration() {
        if (id) SHChangeNotifyDeregister(id);
    }
    void Reset() {
        if (id) SHChangeNotifyDeregister(id);
        id = 0;
    }
};

void RunWorker() {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    NotificationQueue notifications;
    HINSTANCE instance = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      reinterpret_cast<LPCWSTR>(&NotificationProc), &instance);
    std::wstring className = L"Windhawk.DesktopIconArrangement." +
                             std::to_wstring(reinterpret_cast<uintptr_t>(instance));
    WNDCLASSW wc{};
    wc.lpfnWndProc = NotificationProc;
    wc.hInstance = instance;
    wc.lpszClassName = className.c_str();
    if (!RegisterClassW(&wc)) {
        Wh_Log(L"Could not register notification window: %u", GetLastError());
        return;
    }
    NotificationWindow notification{instance, className.c_str()};
    notification.window = CreateWindowExW(0, className.c_str(), L"", 0,
                                          0, 0, 0, 0, HWND_MESSAGE, nullptr,
                                          instance, &notifications);
    if (!notification.window) {
        Wh_Log(L"Could not create notification window: %u", GetLastError());
        return;
    }

    Desktop desktop;
    arrangement::Tracker tracker;
    Geometry lastGeometry;
    ShellRegistration registration;
    uint64_t nextPoll = 0;
    uint64_t nextConnect = 0;
    int lastColumn = g_firstNewColumn.load();
    bool mouseWasDown = false;
    HANDLE events[] = {g_stopEvent, g_settingsEvent};
    while (WaitForSingleObject(g_stopEvent, 0) != WAIT_OBJECT_0) {
        MSG message;
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
        if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) break;
        uint64_t now = GetTickCount64();
        if (!desktop.view && now >= nextConnect) {
            Desktop candidate;
            HRESULT hr = ConnectDesktop(candidate);
            if (SUCCEEDED(hr)) {
                desktop = std::move(candidate);
                SHChangeNotifyEntry entry{desktop.root.get(), TRUE};
                registration.id = SHChangeNotifyRegister(
                    notification.window,
                    SHCNRF_ShellLevel | SHCNRF_InterruptLevel | SHCNRF_NewDelivery,
                    SHCNE_RENAMEITEM | SHCNE_RENAMEFOLDER, kShellChange, 1, &entry);
                Wh_Log(L"Connected to desktop; rename notifications %s",
                        registration.id ? L"enabled" : L"unavailable");
            } else if (hr == E_ACCESSDENIED) {
                Wh_Log(L"This Explorer process does not host the desktop; exiting");
                break;
            } else {
                Wh_Log(L"Desktop unavailable (0x%08X), retrying",
                        static_cast<unsigned>(hr));
            }
            nextConnect = now + 2000;
        }

        // Defer observations throughout a drag so a partially applied Shell
        // operation does not replace the pre-insertion baseline.
        bool mouseDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 ||
                         (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
        HWND editWindow = desktop.window ?
            FindWindowExW(desktop.window, nullptr, L"Edit", nullptr) : nullptr;
        bool editing = editWindow && IsWindowVisible(editWindow);
        if (mouseDown) mouseWasDown = true;
        if (desktop.view && !mouseDown && !editing && now >= nextPoll) {
            std::vector<LiveItem> items;
            Positions current;
            Geometry geometry;
            HRESULT hr = ReadSnapshot(desktop, items, current);
            if (hr == S_OK) hr = ReadGeometry(desktop, geometry);
            DWORD flags = 0;
            if (hr == S_OK) hr = desktop.view->GetCurrentFolderFlags(&flags);

            bool staleRenameSnapshot = false;
            for (const auto& [oldKey, newKey] : notifications.renames) {
                arrangement::Rename(tracker.baseline, oldKey, newKey);
                if (oldKey != newKey && current.count(oldKey) &&
                    !current.count(newKey)) staleRenameSnapshot = true;
            }
            notifications.renames.clear();

            if (hr == S_OK && !staleRenameSnapshot) {
                int column = g_firstNewColumn.load();
                bool reset = !tracker.initialized || !(geometry == lastGeometry) ||
                             column != lastColumn ||
                             (flags & FWF_AUTOARRANGE) ||
                             (geometry.mode != FVM_ICON &&
                              geometry.mode != FVM_SMALLICON);
                if (reset) {
                    tracker.Reset(current, geometry.areas, geometry.spacing, column);
                    lastGeometry = geometry;
                    lastColumn = column;
                } else if (!current.empty()) {
                    // An empty view can be a transient stage of F5 refresh.
                    // Keep identities until the next non-empty observation.
                    auto desired = tracker.Observe(current, now,
                                                   g_settleTime.load(),
                                                   mouseWasDown);
                    if (!desired) {
                        Wh_Log(L"Not enough free grid space for new items; keeping Windows layout");
                    } else if (!desired->empty()) {
                        hr = ApplyPositions(desktop, items, *desired);
                        if (FAILED(hr)) {
                            // Preserve goals to retry transient failures instead
                            // of saving the shifted arrangement as correct.
                            tracker.settleUntil = now + 2000;
                        }
                    }
                }
                mouseWasDown = false;
            } else if (FAILED(hr) && hr != E_PENDING) {
                Wh_Log(L"Desktop observation failed (0x%08X); keeping saved positions",
                        static_cast<unsigned>(hr));
                if (!IsWindow(desktop.window) || hr == RPC_E_DISCONNECTED ||
                    hr == RPC_E_SERVER_DIED || hr == RPC_E_SERVER_DIED_DNE) {
                    registration.Reset();
                    desktop = Desktop{};
                    tracker = arrangement::Tracker{};
                    notifications.renames.clear();
                    nextConnect = now + 2000;
                }
            }
            nextPoll = now + g_pollInterval.load();
        }

        DWORD wait = 50;  // Also notice mouse gestures between layout polls.
        DWORD result = MsgWaitForMultipleObjectsEx(2, events, wait, QS_ALLINPUT,
                                                   MWMO_INPUTAVAILABLE);
        if (result == WAIT_OBJECT_0 || result == WAIT_FAILED) break;
        if (result == WAIT_OBJECT_0 + 1) nextPoll = 0;
    }
    // Destruction deregisters notifications, releases apartment-bound COM
    // interfaces and removes the window callback even if an exception occurs.
}

DWORD WINAPI WorkerProc(void*) {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        Wh_Log(L"Could not initialize COM: 0x%08X", static_cast<unsigned>(hr));
        return 1;
    }
    try {
        RunWorker();
    } catch (...) {
        // Never let a C++ allocation exception escape into Explorer.
        Wh_Log(L"Desktop icon worker stopped after an unexpected exception");
    }
    CoUninitialize();
    return 0;
}

void LoadSettings() {
    g_pollInterval = std::clamp(Wh_GetIntSetting(L"pollIntervalMs"), 100, 2000);
    g_firstNewColumn = std::clamp(Wh_GetIntSetting(L"firstNewColumn"), 0, 1000);
    g_settleTime = std::clamp(Wh_GetIntSetting(L"settleTimeMs"), 0, 5000);
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_settingsEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_settingsEvent) {
        if (g_stopEvent) CloseHandle(g_stopEvent);
        if (g_settingsEvent) CloseHandle(g_settingsEvent);
        g_stopEvent = g_settingsEvent = nullptr;
        return FALSE;
    }
    g_worker = CreateThread(nullptr, 0, WorkerProc, nullptr, 0, nullptr);
    if (!g_worker) {
        CloseHandle(g_stopEvent);
        CloseHandle(g_settingsEvent);
        g_stopEvent = g_settingsEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_settingsEvent) SetEvent(g_settingsEvent);
}

void Wh_ModUninit() {
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_worker) {
        WaitForSingleObject(g_worker, INFINITE);
        CloseHandle(g_worker);
        g_worker = nullptr;
    }
    if (g_stopEvent) CloseHandle(g_stopEvent);
    if (g_settingsEvent) CloseHandle(g_settingsEvent);
    g_stopEvent = g_settingsEvent = nullptr;
}

#endif  // DESKTOP_ICON_ARRANGEMENT_TEST
