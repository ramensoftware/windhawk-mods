// ==WindhawkMod==
// @id              desktop-icon-arrangement
// @name            Desktop Icon Arrangement
// @description     Preserve existing desktop icon positions and place new icons in free columns to the right.
// @version         0.1.1
// @author          Nerdworld
// @github          https://github.com/nerdworldDE
// @include         windhawk.exe
// @compilerOptions -lole32 -lshell32 -luuid
// @license         MIT
// ==/WindhawkMod==

// SPDX-License-Identifier: MIT

// ==WindhawkModReadme==
/*
# Desktop Icon Arrangement

Experimental mod for Windows 11. Turn off **View > Auto arrange icons**
on the desktop before enabling it. **Align icons to grid** can stay enabled.

The mod remembers the current layout. When a new desktop item appears, it
restores the positions of existing items and puts the new item in a free column
to their right. Further new items fill this area from top to bottom, then move
to the next column. If that area is full, it tries other monitors and free gaps.
When there is insufficient space for a whole batch, Windows keeps its layout.

This also applies to files dropped on the desktop: their drop location is
replaced by the configured placement. Moving existing icons remains possible.
Changing **First column for new icons** lets you choose a fixed starting column;
0 calculates it from your arrangement whenever a fresh baseline is established.

This is a corrective mod, not an interception of Explorer's insertion code.
Icons may briefly move before the next check (250 ms by default during desktop
activity). A short follow-up period corrects delayed rearrangements by Explorer.
Desktop mouse gestures end follow-up corrections when no new items appear.
Positions are kept in memory, and Explorer
stores changes through its normal desktop API. Disabling the mod stops future
corrections and leaves the current arrangement in place.

The mod runs in a dedicated Windhawk process. Shell notifications from the user
and public desktop folders trigger brief checks for changes. The saved layout
is also refreshed while the desktop is active and once when it loses focus.
Otherwise, a fallback check runs every five seconds. Missed notifications can
therefore delay a correction. Mouse gestures in other apps do not pause it.

Icon size, monitor layout, grid spacing, view mode, auto-arrange and starting
column changes establish a fresh baseline, as does reconnecting after an
Explorer restart. The mod does not restore layouts from earlier sessions.
Renames reported by Shell notifications retain their positions; undelivered or
late rename notifications can make a renamed item look like a new item.

Enable logging for this mod in Windhawk to see connection, correction and
error messages.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- pollIntervalMs: 250
  $name: Check interval (milliseconds)
  $name:de-DE: Prüfintervall (Millisekunden)
  $description: "100–2000. Interval during desktop activity and after changes. Background fallback checks run every five seconds."
- firstNewColumn: 0
  $name: First column for new icons
  $name:de-DE: Erste Spalte für neue Symbole
  $description: "0 = automatically to the right of each fresh baseline; 1 = leftmost column, 2 = second column, etc. Occupied positions are skipped."
- settleTimeMs: 1000
  $name: Follow-up time (milliseconds)
  $name:de-DE: Nachkorrekturzeit (Millisekunden)
  $description: "0–5000. Correct delayed rearrangements by Explorer after a new item appears."
*/
// ==/WindhawkModSettings==

#include <algorithm>
#include <array>
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

// Fast observations are limited to desktop activity and bounded change bursts.
// Pending work also ensures a final observation when the desktop loses focus.
struct RefreshSchedule {
    static constexpr uint64_t fallbackInterval = 5000;
    uint64_t nextPoll = 0;
    uint64_t nextFallback = fallbackInterval;
    uint64_t followUntil = 0;
    bool foreground = false;
    bool pending = true;

    void Wake(uint64_t now) {
        pending = true;
        nextPoll = now;
    }

    void SetForeground(bool active, uint64_t now) {
        if (foreground != active) {
            foreground = active;
            Wake(now);
        }
    }

    void FollowUntil(uint64_t until) {
        followUntil = std::max<uint64_t>(followUntil, until);
    }

    void Changed(uint64_t now, unsigned settleMs, unsigned interval) {
        Wake(now);
        // Shell notifications can precede insertion into the desktop view.
        FollowUntil(now + std::max<unsigned>({1000, settleMs, 2 * interval}));
    }

    uint64_t Deadline(uint64_t now) const {
        if (pending) return nextPoll;
        if (foreground || now < followUntil) {
            return std::min<uint64_t>(nextPoll, nextFallback);
        }
        return nextFallback;
    }

    void Completed(uint64_t now, unsigned interval) {
        pending = false;
        nextPoll = now + interval;
        nextFallback = now + fallbackInterval;
    }

    void Deferred(uint64_t now, unsigned interval) {
        pending = true;
        nextPoll = now + interval;
    }
};

}  // namespace arrangement

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
constexpr UINT kDesktopActivity = WM_APP + 2;
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
        PITEMID_CHILD children[64]{};
        ULONG fetched = 0;
        HRESULT enumHr = enumerator->Next(ARRAYSIZE(children), children, &fetched);
        if (FAILED(enumHr)) return enumHr;
        // Own the entire batch before processing: early returns and allocation
        // exceptions must also release PIDLs that have not been visited yet.
        std::array<Pidl, ARRAYSIZE(children)> batch;
        for (size_t i = 0; i < batch.size(); ++i) batch[i].reset(children[i]);
        if (fetched > batch.size() || (enumHr == S_OK && fetched == 0)) return E_FAIL;
        for (ULONG i = 0; i < fetched; ++i) {
            Pidl pidl = std::move(batch[i]);
            if (!pidl || items.size() >= 10000) return E_FAIL;
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
        // S_FALSE can contain a partial final batch, which must be processed.
        if (enumHr == S_FALSE) break;
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

bool IsDesktopWindow(HWND window, HWND desktopWindow) {
    if (!window || !desktopWindow) return false;
    if (window == desktopWindow || IsChild(desktopWindow, window) ||
        window == GetAncestor(desktopWindow, GA_ROOT)) return true;
    wchar_t className[32]{};
    if (!GetClassNameW(window, className, ARRAYSIZE(className)) ||
        (wcscmp(className, L"Progman") != 0 &&
         wcscmp(className, L"WorkerW") != 0)) return false;
    DWORD owner = 0;
    DWORD desktopOwner = 0;
    GetWindowThreadProcessId(window, &owner);
    GetWindowThreadProcessId(desktopWindow, &desktopOwner);
    return owner && owner == desktopOwner;
}

bool DesktopMouseDown(HWND desktopWindow) {
    if (!desktopWindow) return false;
    GUITHREADINFO info{};
    info.cbSize = sizeof(info);
    DWORD thread = GetWindowThreadProcessId(desktopWindow, nullptr);
    bool desktopCapture = thread && GetGUIThreadInfo(thread, &info) &&
                          IsDesktopWindow(info.hwndCapture, desktopWindow);
    if (!desktopCapture &&
        !IsDesktopWindow(GetForegroundWindow(), desktopWindow)) return false;
    return (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 ||
           (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
}

struct NotificationQueue {
    std::vector<std::pair<std::wstring, std::wstring>> renames;
    HWND desktopWindow = nullptr;
    bool changed = false;
    bool activity = false;
    bool gesture = false;
};

// OUTOFCONTEXT callbacks run on the registering worker thread. Post messages
// only: never query COM or change the tracker from a reentrant callback.
thread_local HWND g_notificationWindow = nullptr;

void CALLBACK DesktopActivityProc(HWINEVENTHOOK, DWORD event, HWND window,
                                  LONG, LONG, DWORD, DWORD) {
    if (g_notificationWindow) {
        PostMessageW(g_notificationWindow, kDesktopActivity, event,
                     reinterpret_cast<LPARAM>(window));
    }
}

LRESULT CALLBACK NotificationProc(HWND window, UINT message, WPARAM wParam,
                                  LPARAM lParam) {
    if (message == WM_NCCREATE) {
        auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
        SetWindowLongPtrW(window, GWLP_USERDATA,
                          reinterpret_cast<LONG_PTR>(create->lpCreateParams));
    }
    auto* queue = reinterpret_cast<NotificationQueue*>(
        GetWindowLongPtrW(window, GWLP_USERDATA));
    if (message == kDesktopActivity) {
        if (queue) {
            queue->activity = true;
            if ((wParam == EVENT_SYSTEM_CAPTURESTART ||
                 wParam == EVENT_SYSTEM_CAPTUREEND) &&
                IsDesktopWindow(reinterpret_cast<HWND>(lParam),
                                queue->desktopWindow)) {
                queue->gesture = true;
            }
        }
        return 0;
    }
    if (message == kShellChange) {
        PIDLIST_ABSOLUTE* pidls = nullptr;
        LONG event = 0;
        HANDLE lock = SHChangeNotification_Lock(reinterpret_cast<HANDLE>(wParam),
                                                static_cast<DWORD>(lParam),
                                                &pidls, &event);
        if (lock) {
            if (queue) queue->changed = true;
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
                Wh_Log(L"Could not queue a desktop notification");
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
    std::array<ULONG, 2> ids{};
    std::array<Pidl, 2> folders;
    ~ShellRegistration() {
        Reset();
    }
    void Reset() {
        for (size_t i = 0; i < ids.size(); ++i) {
            if (ids[i]) SHChangeNotifyDeregister(ids[i]);
            ids[i] = 0;
            folders[i].reset();
        }
    }
    void Register(HWND window) {
        Reset();
        const KNOWNFOLDERID* folderIds[] = {&FOLDERID_Desktop,
                                           &FOLDERID_PublicDesktop};
        for (size_t i = 0; i < ids.size(); ++i) {
            PIDLIST_ABSOLUTE folder = nullptr;
            HRESULT hr = SHGetKnownFolderIDList(*folderIds[i], 0, nullptr, &folder);
            folders[i].reset(folder);
            if (FAILED(hr) || !folders[i]) continue;
            if (i && folders[0] && ILIsEqual(folders[0].get(), folders[i].get())) {
                continue;
            }
            // Separate non-recursive registrations: cEntries > 1 can prevent
            // deregistration, according to SHChangeNotifyRegister's contract.
            SHChangeNotifyEntry entry{folders[i].get(), FALSE};
            ids[i] = SHChangeNotifyRegister(
                window,
                SHCNRF_ShellLevel | SHCNRF_InterruptLevel | SHCNRF_NewDelivery,
                SHCNE_CREATE | SHCNE_MKDIR | SHCNE_UPDATEDIR |
                    SHCNE_RENAMEITEM | SHCNE_RENAMEFOLDER |
                    SHCNE_DELETE | SHCNE_RMDIR,
                kShellChange, 1, &entry);
        }
        Wh_Log(L"Desktop folder notifications: user=%s, public=%s",
                ids[0] ? L"enabled" : L"unavailable",
                ids[1] ? L"enabled" : L"unavailable");
    }
};

struct ActivityHooks {
    HWINEVENTHOOK foreground = nullptr;
    HWINEVENTHOOK capture = nullptr;
    explicit ActivityHooks(HWND window) {
        g_notificationWindow = window;
        foreground = SetWinEventHook(EVENT_SYSTEM_FOREGROUND,
                                     EVENT_SYSTEM_FOREGROUND, nullptr,
                                     DesktopActivityProc, 0, 0,
                                     WINEVENT_OUTOFCONTEXT);
        capture = SetWinEventHook(EVENT_SYSTEM_CAPTURESTART,
                                  EVENT_SYSTEM_CAPTUREEND, nullptr,
                                  DesktopActivityProc, 0, 0,
                                  WINEVENT_OUTOFCONTEXT);
        if (!foreground || !capture) {
            Wh_Log(L"Some desktop activity events are unavailable; fallback checks remain enabled");
        }
    }
    ~ActivityHooks() {
        if (capture) UnhookWinEvent(capture);
        if (foreground) UnhookWinEvent(foreground);
        g_notificationWindow = nullptr;
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

    ActivityHooks activityHooks(notification.window);
    Desktop desktop;
    arrangement::Tracker tracker;
    arrangement::RefreshSchedule schedule;
    Geometry lastGeometry;
    ShellRegistration registration;
    uint64_t nextConnect = 0;
    int lastColumn = g_firstNewColumn.load();
    bool lastAutoArrange = false;
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
                notifications.desktopWindow = desktop.window;
                registration.Register(notification.window);
                schedule.Changed(now, 2000, g_pollInterval.load());
                Wh_Log(L"Connected to desktop");
            } else {
                Wh_Log(L"Desktop unavailable (0x%08X), retrying",
                        static_cast<unsigned>(hr));
            }
            nextConnect = now + 2000;
        }

        notifications.activity = false;
        schedule.SetForeground(IsDesktopWindow(GetForegroundWindow(),
                                                desktop.window), now);
        if (std::exchange(notifications.changed, false)) {
            schedule.Changed(now, g_settleTime.load(), g_pollInterval.load());
        }
        if (std::exchange(notifications.gesture, false)) {
            mouseWasDown = true;
            schedule.Wake(now);
        }

        if (desktop.view && !IsWindow(desktop.window)) {
            registration.Reset();
            desktop = Desktop{};
            tracker = arrangement::Tracker{};
            notifications.desktopWindow = nullptr;
            notifications.renames.clear();
            mouseWasDown = false;
            nextConnect = now + 2000;
        }

        if (desktop.view && now >= schedule.Deadline(now)) {
            // A gesture is relevant only on the desktop or while its list view
            // owns mouse capture. Clicks in another app never cancel follow-up.
            bool mouseDown = DesktopMouseDown(desktop.window);
            HWND editWindow = FindWindowExW(desktop.window, nullptr, L"Edit", nullptr);
            bool editing = editWindow && IsWindowVisible(editWindow);
            if (mouseDown) mouseWasDown = true;
            if (mouseDown || editing) {
                schedule.Deferred(now, g_pollInterval.load());
            } else {
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

                mouseWasDown |= std::exchange(notifications.gesture, false);
                if (DesktopMouseDown(desktop.window)) {
                    mouseWasDown = true;
                    schedule.Deferred(GetTickCount64(), g_pollInterval.load());
                    continue;
                }
                if (hr == S_OK && !staleRenameSnapshot) {
                    int column = g_firstNewColumn.load();
                    bool autoArrange = (flags & FWF_AUTOARRANGE) != 0;
                    bool reset = !tracker.initialized || !(geometry == lastGeometry) ||
                                 column != lastColumn ||
                                 autoArrange || autoArrange != lastAutoArrange ||
                                 (geometry.mode != FVM_ICON &&
                                  geometry.mode != FVM_SMALLICON);
                    if (reset) {
                        tracker.Reset(current, geometry.areas, geometry.spacing, column);
                        lastGeometry = geometry;
                        lastColumn = column;
                        lastAutoArrange = autoArrange;
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
                        notifications.desktopWindow = nullptr;
                        notifications.renames.clear();
                        mouseWasDown = false;
                        nextConnect = now + 2000;
                    }
                }
                schedule.FollowUntil(tracker.settleUntil);
                schedule.Completed(GetTickCount64(), g_pollInterval.load());
            }
        }

        // COM can dispatch callbacks while a snapshot is being read. Consume
        // their queued flags before blocking, even if no messages remain.
        if (notifications.changed || notifications.activity ||
            notifications.gesture) continue;
        now = GetTickCount64();
        uint64_t deadline = desktop.view ? schedule.Deadline(now) : nextConnect;
        DWORD wait = deadline > now ?
            static_cast<DWORD>(std::min<uint64_t>(deadline - now, INFINITE - 1)) : 0;
        DWORD result = MsgWaitForMultipleObjectsEx(2, events, wait, QS_ALLINPUT,
                                                   MWMO_INPUTAVAILABLE);
        if (result == WAIT_OBJECT_0 || result == WAIT_FAILED) break;
        if (result == WAIT_OBJECT_0 + 1) schedule.Wake(GetTickCount64());
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
        // Keep worker cleanup inside its COM apartment on allocation failures.
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

BOOL WhTool_ModInit() {
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

void WhTool_ModSettingsChanged() {
    LoadSettings();
    if (g_settingsEvent) SetEvent(g_settingsEvent);
}

void WhTool_ModUninit() {
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
