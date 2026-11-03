// ==WindhawkMod==
// @id              notification-center-follows-taskbar
// @name            Notification Center Follows Taskbar
// @description     Keep Notification Center aligned with your taskbar, with optional placement swaps for notifications/calendar and media playback/Quick Settings.
// @version         1.1.0
// @author          Asteski
// @github          https://github.com/Asteski
// @include         explorer.exe
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lgdi32 -lcomctl32 -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Notification Center Follows Taskbar

![Notification Center at the top + Swap notifications and calendar enabled](https://raw.githubusercontent.com/Asteski/Windhawk-Mods/refs/heads/main/img/notification-center-follows-taskbar/notification-center-follows-taskbar.png)
![Swap media playback and Quick Settings enabled](https://raw.githubusercontent.com/Asteski/Windhawk-Mods/refs/heads/main/img/notification-center-follows-taskbar/notification-center-follows-taskbar-02.png)

Keeps the Windows 11 Notification Center (Win+N, including its calendar)
correctly positioned with top, bottom, left, and right taskbars. Detects the
taskbar on the flyout's monitor automatically. Adds the optional right-edge
repair after Explorer restarts. Auto-hidden taskbars use the monitor's
working area.

Install alongside your taskbar positioning mod. Close and reopen
Notification Center after enabling, disabling, or moving the taskbar.

Enable **Swap notifications and calendar** to put the calendar above the
notifications. The panels exchange their visual positions while Windows keeps
control of their native layout sizes and scrolling. The combined outer bounds
stay fixed: the swap doesn't move the host window or change row definitions.
No fixed notification height is used. Expanding/collapsing the calendar and
changing display size use Windows' normal layout calculations.

Remove any Notification Center Styler rules that swap these panels or force
their heights, row definitions, or row positions before enabling this option.
Cosmetic styling can remain. Switching the option off restores the original
transforms, including bindings. The positioning fix works without the swap.

## Other positioning mods

**Repair right-edge position (disable with Shell Flyout Positions)** is enabled
by default. It keeps Notification Center right-aligned with top taskbars,
including after Explorer restarts, while preserving the native panel inset.
Disable it when using Shell Flyout Positions so that mod controls horizontal
alignment. With repair disabled, this mod changes only the vertical position.

Enable **Swap media playback and Quick Settings** to put the media playback
panel below Quick Settings (Win+A), keeping the familiar Windows appearance
and spacing. Remove styling rules that already reorder these panels before
enabling the option. Switching it off restores their original arrangement.
Close and reopen Quick Settings after changing the option; no process restart
is needed.

This opt-in feature is a convenience for users who want both panel swaps
without installing a Styler or attaching XAML Diagnostics. If you already use
Windows 11 Notification Center Styler, you can use these equivalent rules
instead and leave this mod's media swap disabled:

```yaml
controlStyles:
  - target: Grid#ControlCenterRegion
    styles:
      - Grid.Row=0
  - target: ControlCenter.MediaTransportControls#MediaTransportControls
    styles:
      - Grid.Row=1
      - Margin=0,12,0,0
      - Canvas.ZIndex=1
```

ShellExperienceHost/ShellHost loads this mod only while panel swapping is enabled. Window
creation only schedules initialization; XAML is accessed later on its UI thread.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- swapPanels: false
  $name: Swap notifications and calendar
  $description: Put the calendar above notifications while keeping Windows' native panel sizes and scrolling. No fixed height limit or row sizing overrides.
- swapMediaPanels: false
  $name: Swap media playback and Quick Settings
  $description: Put media playback below Quick Settings while keeping the familiar Windows appearance and spacing. Switching this off restores the original arrangement.
- repairHorizontalPosition: true
  $name: Repair right-edge position (disable with Shell Flyout Positions)
  $description: Keep Notification Center right-aligned with top taskbars after Explorer restarts. Disable when using Shell Flyout Positions to preserve its horizontal alignment.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <limits>
#include <algorithm>
#include <cmath>
#include <optional>
#include <memory>
#include <windhawk_utils.h>
#include <commctrl.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Markup.h>

namespace {
decltype(&SetWindowRgn) SetWindowRgn_Original;
decltype(&DwmSetWindowAttribute) DwmSetWindowAttribute_Original;
std::atomic<bool> g_unloading{false};
std::atomic<bool> g_swapPanels{false};
std::atomic<bool> g_swapMediaPanels{false};
std::atomic<bool> g_repairHorizontalPosition{true};



bool g_shellHost = false;
bool g_quickSettingsHost = false;
thread_local bool g_adjusting = false;

struct Placement {
    LONG originalX;
    LONG originalY;
    LONG appliedX;
    LONG appliedY;
    DWORD processId;
    HMONITOR monitor;
};
std::mutex g_mutex;
std::unordered_map<HWND, Placement> g_placements;

bool IsNotificationCenter(HWND window) {
    WCHAR className[64];
    if (!GetClassNameW(window, className, ARRAYSIZE(className)) ||
        wcscmp(className, L"Windows.UI.Core.CoreWindow") != 0) {
        return false;
    }
    DWORD pid = 0;
    DWORD tid = GetWindowThreadProcessId(window, &pid);
    if (!tid || !pid) return false;
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return false;
    WCHAR path[MAX_PATH];
    DWORD length = ARRAYSIZE(path);
    BOOL queried = QueryFullProcessImageNameW(process, 0, path, &length);
    CloseHandle(process);
    if (!queried) return false;
    const WCHAR* name = wcsrchr(path, L'\\');
    if (!name || _wcsicmp(name + 1, L"ShellExperienceHost.exe") != 0)
        return false;

    HANDLE thread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, tid);
    if (!thread) return false;
    PWSTR description = nullptr;
    HRESULT hr = GetThreadDescription(thread, &description);
    bool matches = SUCCEEDED(hr) && description &&
                   wcscmp(description, L"ActionCenter") == 0;
    LocalFree(description);
    CloseHandle(thread);
    return matches;
}

bool GetTopTaskbarArea(HMONITOR monitor, MONITORINFO& info) {
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(monitor, &info)) return false;
    struct Search { HMONITOR monitor; HWND window; } search{monitor, nullptr};
    EnumWindows([](HWND window, LPARAM param) -> BOOL {
        auto& search = *reinterpret_cast<Search*>(param);
        WCHAR name[64];
        if (GetClassNameW(window, name, ARRAYSIZE(name)) &&
            (wcscmp(name, L"Shell_TrayWnd") == 0 ||
             wcscmp(name, L"Shell_SecondaryTrayWnd") == 0) &&
            MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST) == search.monitor) {
            search.window = window;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&search));
    RECT bar;
    if (!search.window || !GetWindowRect(search.window, &bar)) return false;
    // Works for visible bars and bars mostly outside the display during auto-hide.
    if (bar.right - bar.left <= bar.bottom - bar.top ||
        (static_cast<LONGLONG>(bar.top) + bar.bottom) >=
            (static_cast<LONGLONG>(info.rcMonitor.top) + info.rcMonitor.bottom))
        return false;
    return true;
}

void Restore(HWND window) {
    Placement saved;
    {
        std::lock_guard lock(g_mutex);
        auto it = g_placements.find(window);
        if (it == g_placements.end()) return;
        saved = it->second;
        g_placements.erase(it);
    }
    RECT rect;
    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    if (pid == saved.processId && GetWindowRect(window, &rect) &&
        MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST) == saved.monitor &&
        (rect.left == saved.appliedX || rect.top == saved.appliedY)) {
        // Another positioning mod may have changed X after our correction.
        // Restore only coordinates that still contain our applied value.
        LONG x = rect.left == saved.appliedX ? saved.originalX : rect.left;
        LONG y = rect.top == saved.appliedY ? saved.originalY : rect.top;
        SetWindowPos(window, nullptr, x, y, 0, 0,
                     SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

POINT CalculateTopTaskbarPosition(const MONITORINFO& info, const RECT& visible,
                                 UINT dpi, LONG nativeX = 0,
                                 bool repairHorizontalPosition = true) {
    LONG verticalGap = MulDiv(12, dpi ? dpi : 96, 96);
    // Use the visible region's right edge, not the outer host width: the host
    // can include transparent space. Don't retain a stale X after Explorer
    // recreates its taskbar or the surviving shell host is reused.
    LONG right = info.rcWork.right < info.rcMonitor.right
                     ? info.rcWork.right : info.rcMonitor.right;
    LONG top = info.rcWork.top > info.rcMonitor.top
                   ? info.rcWork.top : info.rcMonitor.top;
    // The shell region already includes the panel's native right-side inset.
    // Adding our vertical gap on X too doubles that spacing for top taskbars.
    return {repairHorizontalPosition ? right - visible.right : nativeX,
            top + verticalGap - visible.top};
}

void Adjust(HWND window, HRGN region) {
    if (g_unloading || g_adjusting || !region || !IsNotificationCenter(window))
        return;
    RECT visible, outer;
    int kind = GetRgnBox(region, &visible);
    if (kind == ERROR || kind == NULLREGION || !GetWindowRect(window, &outer))
        return;
    // ShellExperienceHost creates placeholder windows while rebuilding views.
    // Their geometry doesn't identify an actual flyout yet.
    if (visible.right - visible.left <= 1 || visible.bottom - visible.top <= 1 ||
        outer.right - outer.left <= 1 || outer.bottom - outer.top <= 1)
        return;
    RECT screen = visible;
    OffsetRect(&screen, outer.left, outer.top);
    HMONITOR monitor = MonitorFromRect(&screen, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info{};
    if (!GetTopTaskbarArea(monitor, info)) {
        Restore(window);
        return;
    }
    UINT dpi = GetDpiForWindow(window);
    POINT target = CalculateTopTaskbarPosition(info, visible, dpi, outer.left,
                                              g_repairHorizontalPosition);
    if (outer.left == target.x && outer.top == target.y) return;
    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    {
        std::lock_guard lock(g_mutex);
        // Remove dead handles so repeated Explorer sessions can't grow this map.
        for (auto it = g_placements.begin(); it != g_placements.end();) {
            if (!IsWindow(it->first)) it = g_placements.erase(it);
            else ++it;
        }
        auto [it, inserted] = g_placements.try_emplace(
            window, Placement{outer.left, outer.top, target.x, target.y, pid, monitor});
        if (!inserted) {
            bool newSession = it->second.processId != pid || it->second.monitor != monitor;
            if (newSession || outer.left != it->second.appliedX)
                it->second.originalX = outer.left;
            if (newSession || outer.top != it->second.appliedY)
                it->second.originalY = outer.top;
            it->second.appliedX = target.x;
            it->second.appliedY = target.y;
            it->second.processId = pid;
            it->second.monitor = monitor;
        }
    }
    g_adjusting = true;
    BOOL moved = SetWindowPos(window, nullptr, target.x, target.y, 0, 0,
                             SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    g_adjusting = false;
    if (!moved) {
        std::lock_guard lock(g_mutex);
        g_placements.erase(window);
    } else {
        Wh_Log(L"Notification Center: (%ld,%ld) -> (%ld,%ld), region (%ld,%ld)-(%ld,%ld)",
               outer.left, outer.top, target.x, target.y,
               visible.left, visible.top, visible.right, visible.bottom);
    }
}

int WINAPI SetWindowRgn_Hook(HWND window, HRGN region, BOOL redraw) {
    // Inspect before calling Windows: on success Windows owns the region.
    Adjust(window, region);
    return SetWindowRgn_Original(window, region, redraw);
}

HRESULT WINAPI DwmSetWindowAttribute_Hook(HWND window, DWORD attribute,
                                         LPCVOID value, DWORD size) {
    if (!g_unloading && attribute == DWMWA_CLOAK && value &&
        size == sizeof(BOOL) && !*static_cast<const BOOL*>(value) &&
        IsNotificationCenter(window)) {
        HRGN region = CreateRectRgn(0, 0, 0, 0);
        if (region) {
            int kind = GetWindowRgn(window, region);
            if (kind != ERROR && kind != NULLREGION) Adjust(window, region);
            DeleteObject(region);
        }
    }
    return DwmSetWindowAttribute_Original(window, attribute, value, size);
}

namespace PanelSwap {
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Data;

std::atomic<HWND> g_window{nullptr};
UINT g_message;
std::mutex g_candidateMutex;
std::vector<HWND> g_candidates;
std::unordered_map<HWND, HHOOK> g_pendingAttach;
enum : WPARAM { kInitialize = 1, kRefresh, kStop };
struct CoreWindowInterop : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE get_WindowHandle(HWND* window) = 0;
};
// XAML state is exclusively owned by the ActionCenter UI thread.
[[clang::no_destroy]] Grid g_grid{nullptr};
[[clang::no_destroy]] FrameworkElement g_notifications{nullptr}, g_calendar{nullptr};
[[clang::no_destroy]] TranslateTransform g_notificationShift{nullptr}, g_calendarShift{nullptr};
winrt::event_token g_layoutToken{};
bool g_hasLayoutToken = false, g_updating = false;
winrt::weak_ref<Grid> g_rejectedGrid;

struct CoreWindowEvents {
    HWND window = nullptr;
    winrt::Windows::UI::Core::CoreWindow core{nullptr};
    winrt::event_token activated{}, visible{};
    FrameworkElement discoveryRoot{nullptr};
    winrt::event_token discoveryToken{};
};
// Explicitly revoked/released on the owning UI thread during normal unload.
// Process exit deliberately leaves them for the OS; no XAML CRT teardown.
[[clang::no_destroy]] thread_local std::optional<CoreWindowEvents> t_events;

struct SavedProperty {
    DependencyObject object;
    DependencyProperty property;
    winrt::Windows::Foundation::IInspectable value;
    Binding binding{nullptr};
    SavedProperty(DependencyObject const& target, DependencyProperty const& dp)
        : object(target), property(dp), value(target.ReadLocalValue(dp)) {
        if (auto expression = value.try_as<BindingExpression>())
            binding = expression.ParentBinding();
    }
    void Restore() const {
        if (binding) BindingOperations::SetBinding(object, property, binding);
        else if (value == DependencyProperty::UnsetValue()) object.ClearValue(property);
        else object.SetValue(property, value);
    }
};
[[clang::no_destroy]] std::optional<std::vector<SavedProperty>> g_saved;

void RestoreLayout() noexcept {
    if (g_grid && g_hasLayoutToken) {
        try { g_grid.LayoutUpdated(g_layoutToken); } catch (...) {}
    }
    g_hasLayoutToken = false;
    if (g_saved) {
        for (auto it = g_saved->rbegin(); it != g_saved->rend(); ++it) {
            try { it->Restore(); } catch (...) {}
        }
    }
    g_saved.reset();
    g_notificationShift = nullptr;
    g_calendarShift = nullptr;
    g_notifications = nullptr;
    g_calendar = nullptr;
    g_grid = nullptr;
}

Grid FindPanelGrid(DependencyObject const& root, unsigned depth = 0) {
    if (!root || depth > 32) return nullptr;
    if (auto grid = root.try_as<Grid>(); grid && grid.Name() == L"RootContent") {
        bool notifications = false, calendar = false;
        for (auto const& child : grid.Children()) {
            auto element = child.try_as<FrameworkElement>();
            if (!element) continue;
            notifications |= element.Name() == L"NotificationCenterGrid";
            calendar |= element.Name() == L"CalendarCenterGrid";
        }
        if (notifications && calendar) return grid;
    }
    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto found = FindPanelGrid(VisualTreeHelper::GetChild(root, i), depth + 1);
        if (found) return found;
    }
    return nullptr;
}

void UpdateTransforms() noexcept {
    if (g_updating || !g_grid || !g_notificationShift || !g_calendarShift) return;
    g_updating = true;
    try {
        double notificationY = 0, calendarY = 0;
        if (g_notifications.Visibility() == Visibility::Visible &&
            g_calendar.Visibility() == Visibility::Visible &&
            g_notifications.ActualHeight() > 0 && g_calendar.ActualHeight() > 0) {
            // Subtract only our own translation to recover native geometry.
            // Do not change rows, margins, height constraints, or trigger Measure.
            auto nb = g_notifications.TransformToVisual(g_grid).TransformBounds(
                {0, 0, static_cast<float>(g_notifications.ActualWidth()),
                 static_cast<float>(g_notifications.ActualHeight())});
            auto cb = g_calendar.TransformToVisual(g_grid).TransformBounds(
                {0, 0, static_cast<float>(g_calendar.ActualWidth()),
                 static_cast<float>(g_calendar.ActualHeight())});
            double nt = nb.Y - g_notificationShift.Y();
            double ct = cb.Y - g_calendarShift.Y();
            // Keep the union's top and bottom fixed. Windows retains ownership
            // of sizing and its existing notification scrolling constraints.
            if (ct >= nt + nb.Height - 0.5) {
                calendarY = nt - ct;
                notificationY = ct + cb.Height - nt - nb.Height;
            }
        }
        if (std::abs(g_notificationShift.Y() - notificationY) > 0.05)
            g_notificationShift.Y(notificationY);
        if (std::abs(g_calendarShift.Y() - calendarY) > 0.05)
            g_calendarShift.Y(calendarY);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Panel swap transform failed: %08X", e.code().value);
    } catch (...) {}
    g_updating = false;
}

TranslateTransform AddTranslation(FrameworkElement const& element) {
    g_saved->emplace_back(element, UIElement::RenderTransformProperty());
    auto original = element.RenderTransform();
    TransformGroup group;
    if (original) group.Children().Append(original);
    TranslateTransform shift;
    group.Children().Append(shift);
    element.RenderTransform(group);
    return shift;
}

void ApplyGrid(Grid const& grid) {
    if (grid == g_grid && grid) {
        UpdateTransforms();
        return;
    }
    RestoreLayout();
    if (!grid) return;
    FrameworkElement notifications{nullptr}, calendar{nullptr};
    for (auto const& child : grid.Children()) {
        auto element = child.try_as<FrameworkElement>();
        if (!element) continue;
        if (element.Name() == L"NotificationCenterGrid") notifications = element;
        if (element.Name() == L"CalendarCenterGrid") calendar = element;
    }
    if (!notifications || !calendar) return;
    // An existing Styler row swap must be removed before enabling this option.
    if (Grid::GetRow(calendar) <= Grid::GetRow(notifications)) {
        if (g_rejectedGrid.get() != grid) {
            Wh_Log(L"Panel swap skipped: panels are already reordered by another style");
            g_rejectedGrid = winrt::make_weak(grid);
        }
        return;
    }
    g_grid = grid;
    g_notifications = notifications;
    g_calendar = calendar;
    g_saved.emplace();
    // Keep the lower notifications panel's shadow above the calendar after
    // swapping. The shared restoration path preserves any prior local value
    // or binding when the option is disabled or the mod is unloaded.
    g_saved->emplace_back(notifications, Canvas::ZIndexProperty());
    Canvas::SetZIndex(notifications, 1);
    g_notificationShift = AddTranslation(notifications);
    g_calendarShift = AddTranslation(calendar);
    g_layoutToken = grid.LayoutUpdated([](auto const&, auto const&) {
        if (!g_unloading && g_swapPanels) UpdateTransforms();
    });
    g_hasLayoutToken = true;
    UpdateTransforms();
    Wh_Log(L"Panel swap attached without changing native layout sizes");
}

void Refresh(HWND nativeWindow) noexcept;

void RevokeDiscovery() noexcept {
    if (!t_events || !t_events->discoveryRoot) return;
    try { t_events->discoveryRoot.LayoutUpdated(t_events->discoveryToken); } catch (...) {}
    t_events->discoveryRoot = nullptr;
    t_events->discoveryToken = {};
}

void UnregisterCoreWindowEvents(HWND window) noexcept {
    if (!t_events || t_events->window != window) return;
    RevokeDiscovery();
    try { t_events->core.Activated(t_events->activated); } catch (...) {}
    try { t_events->core.VisibilityChanged(t_events->visible); } catch (...) {}
    t_events.reset();
}

bool MatchesCoreWindow(winrt::Windows::UI::Core::CoreWindow const& core,
                       HWND window) {
    if (!core) return false;
    constexpr GUID iid{0x45d64a29, 0xa63e, 0x4cb6,
                       {0xb4, 0x98, 0x57, 0x81, 0xd2, 0x98, 0xcb, 0x4f}};
    winrt::com_ptr<CoreWindowInterop> interop;
    if (FAILED(winrt::get_unknown(core)->QueryInterface(iid, interop.put_void()))) return false;
    HWND actual = nullptr;
    return SUCCEEDED(interop->get_WindowHandle(&actual)) && actual == window;
}

void RegisterCoreWindowEvents(HWND window) noexcept {
    if (g_unloading) return;
    CoreWindowEvents state;
    try {
        auto core = winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread();
        if (!MatchesCoreWindow(core, window)) return;
        if (t_events && t_events->window == window && t_events->core == core) return;
        if (t_events) UnregisterCoreWindowEvents(t_events->window);
        state.window = window;
        state.core = core;
        state.activated = core.Activated([window](auto const&, auto const& args) {
            if (!g_unloading && args.WindowActivationState() !=
                    winrt::Windows::UI::Core::CoreWindowActivationState::Deactivated)
                PostMessageW(window, g_message, kRefresh, 0);
        });
        state.visible = core.VisibilityChanged([window](auto const&, auto const& args) {
            if (!g_unloading && args.Visible())
                PostMessageW(window, g_message, kRefresh, 0);
        });
        if (!g_unloading) {
            t_events.emplace(std::move(state));
            return;
        }
    } catch (...) {
        Wh_Log(L"Couldn't register CoreWindow events: %08X", winrt::to_hresult().value);
    }
    if (state.core) {
        if (state.activated.value) {
            try { state.core.Activated(state.activated); } catch (...) {}
        }
        if (state.visible.value) {
            try { state.core.VisibilityChanged(state.visible); } catch (...) {}
        }
    }
}

void WatchForContent(FrameworkElement const& root, HWND window) {
    if (!root || !t_events || t_events->window != window) return;
    if (t_events->discoveryRoot == root) return;
    RevokeDiscovery();
    t_events->discoveryRoot = root;
    t_events->discoveryToken = root.LayoutUpdated([window](auto const&, auto const&) {
        if (!g_unloading) Refresh(window);
    });
}

void Refresh(HWND nativeWindow) noexcept {
    try {
        HWND owner = g_window.load();
        if (owner && owner != nativeWindow) return;
        if (!g_swapPanels || g_unloading) {
            if (owner == nativeWindow) RestoreLayout();
            return;
        }
        auto window = Window::Current();
        if (!window) return;
        // A process can host several CoreWindows, including the toast page.
        // Bind only the actual Win+N XAML root and its matching native window.
        if (!MatchesCoreWindow(window.CoreWindow(), nativeWindow)) return;
        auto grid = FindPanelGrid(window.Content());
        if (!grid) {
            if (owner == nativeWindow) {
                RestoreLayout();
                g_window = nullptr;
            }
            WatchForContent(window.Content().try_as<FrameworkElement>(), nativeWindow);
            return;
        }
        RevokeDiscovery();
        if (g_unloading) return;
        HWND expected = nullptr;
        if (!owner && !g_window.compare_exchange_strong(expected, nativeWindow)) return;
        ApplyGrid(grid);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Panel swap: %08X", e.code().value);
        if (g_window == nativeWindow) RestoreLayout();
    } catch (...) {
        if (g_window == nativeWindow) RestoreLayout();
    }
}

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam,
                            LPARAM lParam, UINT_PTR subclassId, DWORD_PTR) {
    if (message == g_message) {
        if (wParam == kStop || g_unloading || !g_swapPanels) {
            if (g_window == window) {
                RestoreLayout();
                g_window = nullptr;
            }
            UnregisterCoreWindowEvents(window);
            RemoveWindowSubclass(window, WindowProc, subclassId);
        } else {
            RegisterCoreWindowEvents(window);
            Refresh(window);
        }
        return 0;
    }
    // If the initial posted message preceded CoreWindow initialization, the
    // first real show/activation schedules another safe registration attempt.
    if (!g_unloading && ((message == WM_SHOWWINDOW && wParam) ||
        (message == WM_ACTIVATE && LOWORD(wParam) != WA_INACTIVE)))
        PostMessageW(window, g_message, kInitialize, 0);
    if (message == WM_NCDESTROY) {
        if (g_window == window) {
            RestoreLayout();
            g_window = nullptr;
        }
        UnregisterCoreWindowEvents(window);
        RemoveWindowSubclass(window, WindowProc, subclassId);
        std::lock_guard lock(g_candidateMutex);
        std::erase(g_candidates, window);
    }
    return DefSubclassProc(window, message, wParam, lParam);
}

LRESULT CALLBACK InitializeOnThread(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION && wParam == PM_REMOVE) {
        auto message = reinterpret_cast<MSG*>(lParam);
        if (message->message == g_message && message->wParam == kInitialize) {
            HHOOK hook = nullptr;
            {
                std::lock_guard lock(g_candidateMutex);
                auto it = g_pendingAttach.find(message->hwnd);
                if (it != g_pendingAttach.end()) {
                    hook = it->second;
                    g_pendingAttach.erase(it);
                }
            }
            if (hook) {
                UnhookWindowsHookEx(hook);
                // Only Win32 work here. Registering XAML events happens when
                // this posted message is subsequently dispatched to the HWND.
                if (g_unloading || !SetWindowSubclass(message->hwnd, WindowProc,
                        reinterpret_cast<UINT_PTR>(WindowProc), 0)) {
                    std::lock_guard lock(g_candidateMutex);
                    std::erase(g_candidates, message->hwnd);
                }
            }
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

void Attach(HWND window) {
    DWORD pid = 0;
    DWORD thread = GetWindowThreadProcessId(window, &pid);
    WCHAR name[64];
    if (g_unloading || !g_swapPanels || !thread || pid != GetCurrentProcessId() ||
        !GetClassNameW(window, name, ARRAYSIZE(name)) ||
        wcscmp(name, L"Windows.UI.Core.CoreWindow") != 0) return;
    {
        std::lock_guard lock(g_candidateMutex);
        // Unload can begin after the initial check but before this lock.
        if (g_unloading) return;
        if (std::find(g_candidates.begin(), g_candidates.end(), window) != g_candidates.end()) return;
        g_candidates.push_back(window);
        HHOOK hook = SetWindowsHookExW(WH_GETMESSAGE, InitializeOnThread, nullptr, thread);
        if (!hook) {
            std::erase(g_candidates, window);
            return;
        }
        g_pendingAttach.emplace(window, hook);
    }
    if (!PostMessageW(window, g_message, kInitialize, 0)) {
        HHOOK hook = nullptr;
        {
            std::lock_guard lock(g_candidateMutex);
            auto it = g_pendingAttach.find(window);
            if (it != g_pendingAttach.end()) {
                hook = it->second;
                g_pendingAttach.erase(it);
            }
            std::erase(g_candidates, window);
        }
        if (hook) UnhookWindowsHookEx(hook);
    }
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD,
    int, int, int, int, HWND, HMENU, HINSTANCE, void*, DWORD);
using CreateWindowInBandEx_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD,
    int, int, int, int, HWND, HMENU, HINSTANCE, void*, DWORD, DWORD);
CreateWindowInBand_t CreateWindowInBand_Original;
CreateWindowInBandEx_t CreateWindowInBandEx_Original;

HWND WINAPI CreateWindowInBand_Hook(DWORD exStyle, LPCWSTR className,
    LPCWSTR title, DWORD style, int x, int y, int width, int height, HWND parent,
    HMENU menu, HINSTANCE instance, void* param, DWORD band) {
    HWND window = CreateWindowInBand_Original(exStyle, className, title, style,
        x, y, width, height, parent, menu, instance, param, band);
    if (window) Attach(window);
    return window;
}

HWND WINAPI CreateWindowInBandEx_Hook(DWORD exStyle, LPCWSTR className,
    LPCWSTR title, DWORD style, int x, int y, int width, int height, HWND parent,
    HMENU menu, HINSTANCE instance, void* param, DWORD band, DWORD flags) {
    HWND window = CreateWindowInBandEx_Original(exStyle, className, title, style,
        x, y, width, height, parent, menu, instance, param, band, flags);
    if (window) Attach(window);
    return window;
}

bool HookWindowCreation() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) return false;
    auto create = GetProcAddress(user32, "CreateWindowInBand");
    auto createEx = GetProcAddress(user32, "CreateWindowInBandEx");
    if (!create && !createEx) return false;
    if (create && !Wh_SetFunctionHook(reinterpret_cast<void*>(create),
            reinterpret_cast<void*>(CreateWindowInBand_Hook),
            reinterpret_cast<void**>(&CreateWindowInBand_Original))) return false;
    if (createEx && !Wh_SetFunctionHook(reinterpret_cast<void*>(createEx),
            reinterpret_cast<void*>(CreateWindowInBandEx_Hook),
            reinterpret_cast<void**>(&CreateWindowInBandEx_Original))) return false;
    return true;
}

void FindExistingWindows() {
    // One pass on load; creation hooks and CoreWindow events handle the rest.
    EnumWindows([](HWND window, LPARAM) -> BOOL {
        DWORD pid = 0;
        GetWindowThreadProcessId(window, &pid);
        if (pid == GetCurrentProcessId()) Attach(window);
        return TRUE;
    }, 0);
}

void Stop() {
    std::vector<HWND> candidates;
    std::vector<HHOOK> pending;
    {
        std::lock_guard lock(g_candidateMutex);
        candidates = g_candidates;
        for (auto const& entry : g_pendingAttach) pending.push_back(entry.second);
        g_pendingAttach.clear();
    }
    for (HHOOK hook : pending) UnhookWindowsHookEx(hook);
    // A synchronous message drains any initialization and revokes every WinRT
    // handler/reference on its UI thread before this DLL can be unloaded.
    for (HWND window : candidates) SendMessageW(window, g_message, kStop, 0);
    g_window = nullptr;
    g_rejectedGrid = {};
    { std::lock_guard lock(g_candidateMutex); g_candidates.clear(); }
}
}  // namespace PanelSwap

// ShellHost Quick Settings is observed at XAML activation, independently of
// Window::Current, CoreWindow HWNDs, and Notification Center discovery.
namespace QuickSettings {
using namespace PanelSwap;
using winrt::Windows::UI::Core::CoreDispatcherPriority;
// Saving a property must not keep a discarded flyout tree alive.
struct SavedProperty {
    winrt::weak_ref<DependencyObject> object;
    DependencyProperty property;
    winrt::Windows::Foundation::IInspectable value;
    Binding binding{nullptr};
    SavedProperty(DependencyObject const& target, DependencyProperty const& dp)
        : object(winrt::make_weak(target)), property(dp), value(target.ReadLocalValue(dp)) {
        if (auto expression = value.try_as<BindingExpression>()) binding = expression.ParentBinding();
        // A BindingExpression can retain its target. Keep only the binding
        // needed for restoration, not the expression attached to the old tree.
        if (binding) value = nullptr;
    }
    void Restore() const {
        auto target = object.get();
        if (!target) return;
        if (binding) BindingOperations::SetBinding(target, property, binding);
        else if (value == DependencyProperty::UnsetValue()) target.ClearValue(property);
        else target.SetValue(property, value);
    }
};
struct State {
    winrt::weak_ref<FrameworkElement> media;
    winrt::Windows::UI::Core::CoreDispatcher dispatcher{nullptr};
    DWORD threadId = GetCurrentThreadId();
    winrt::event_token loaded{};
    std::vector<SavedProperty> saved;
    bool applied = false;
};
std::mutex mutex;
[[clang::no_destroy]] std::optional<std::vector<std::shared_ptr<State>>> states;
using Activate_t = HRESULT(WINAPI*)(void*, void**);
Activate_t activateOriginal;
using Showing_t = HRESULT(WINAPI*)(void*);
Showing_t showingOriginal;
decltype(&LoadLibraryExW) loadOriginal;
std::atomic<bool> attempted{false};

void RevokeLoaded(std::shared_ptr<State> const& state, FrameworkElement const& media) {
    if (state->loaded.value) {
        media.Loaded(state->loaded);
        state->loaded = {};
    }
}

FrameworkElement FindRegion(DependencyObject const& root, unsigned depth = 0) {
    if (!root || depth > 40) return nullptr;
    if (auto grid = root.try_as<Grid>(); grid && grid.Name() == L"ControlCenterRegion")
        return grid;
    if (auto panel = root.try_as<Panel>()) {
        for (auto const& child : panel.Children())
            if (auto found = FindRegion(child, depth + 1)) return found;
        return nullptr;
    }
    for (int i = 0; i < VisualTreeHelper::GetChildrenCount(root); ++i)
        if (auto found = FindRegion(VisualTreeHelper::GetChild(root, i), depth + 1))
            return found;
    return nullptr;
}

void Apply(std::shared_ptr<State> const& state, DependencyObject const& layoutRoot = nullptr) noexcept {
    if (g_unloading || !g_swapMediaPanels || state->applied) return;
    try {
        auto media = state->media.get();
        if (!media) return;
        DependencyObject root = layoutRoot ? layoutRoot : media;
        for (;;) {
            auto parent = VisualTreeHelper::GetParent(root);
            if (!parent) {
                if (auto element = root.try_as<FrameworkElement>()) parent = element.Parent();
            }
            if (!parent) break;
            root = parent;
        }
        auto region = FindRegion(root);
        if (!region) {
            Wh_Log(L"Media found without ControlCenterRegion; retry on next load/show");
            return;
        }
        state->saved.emplace_back(region, Grid::RowProperty());
        state->saved.emplace_back(media, Grid::RowProperty());
        state->saved.emplace_back(media, FrameworkElement::MarginProperty());
        state->saved.emplace_back(media, Canvas::ZIndexProperty());
        state->applied = true;
        Grid::SetRow(region, 0);
        Grid::SetRow(media, 1);
        media.Margin({0,12,0,0});
        Canvas::SetZIndex(media, 1);
        RevokeLoaded(state, media);
        Wh_Log(L"ShellHost: applied Quick Settings/media styles on UI thread %lu", GetCurrentThreadId());
    } catch (...) {
        Wh_Log(L"Quick Settings styles failed: %08X", winrt::to_hresult().value);
        for (auto it = state->saved.rbegin(); it != state->saved.rend(); ++it) {
            try { it->Restore(); } catch (...) {}
        }
        state->saved.clear();
        state->applied = false;
        try { if (auto media = state->media.get()) RevokeLoaded(state, media); } catch (...) {}
    }
}

std::shared_ptr<State> Observe(FrameworkElement const& element) {
        if (!element || g_unloading) return {};
        {
            std::lock_guard lock(mutex);
            if (states) std::erase_if(*states, [](auto const& state) {
                return state->threadId == GetCurrentThreadId() && !state->media.get();
            });
            if (states) for (auto const& state : *states) {
                if (state->threadId == GetCurrentThreadId() && state->media.get() == element)
                    return state;
            }
        }
        auto state = std::make_shared<State>();
        state->media = winrt::make_weak(element);
        state->dispatcher = element.Dispatcher();
        std::weak_ptr<State> weak = state;
        state->loaded = element.Loaded([weak](auto const&, auto const&) {
            if (auto state = weak.lock()) Apply(state);
        });
        {
            std::lock_guard lock(mutex);
            if (!g_unloading) {
                if (!states) states.emplace();
                states->push_back(state);
            } else {
                element.Loaded(state->loaded);
            }
        }
        return state;
}

HRESULT WINAPI Activate(void* type, void** result) {
    auto hr = activateOriginal(type, result);
    if (FAILED(hr) || !result || !*result || g_unloading) return hr;
    try {
        winrt::Windows::Foundation::IInspectable object{nullptr};
        winrt::copy_from_abi(object, *result);
        auto element = object.try_as<FrameworkElement>();
        if (element && winrt::get_class_name(element) == L"ControlCenter.MediaTransportControls")
            Observe(element);
    } catch (...) { Wh_Log(L"Quick Settings activation failed: %08X", winrt::to_hresult().value); }
    return hr;
}

// Hook the COM ABI produce thunk: self is IMediaTransportControls*, not the
// implementation's C++ this pointer. QI retrieves the existing FrameworkElement.
HRESULT WINAPI Showing(void* self) {
    auto hr = showingOriginal(self);
    if (FAILED(hr) || g_unloading || !g_swapMediaPanels) return hr;
    try {
        winrt::Windows::Foundation::IInspectable object{nullptr};
        winrt::copy_from_abi(object, self);
        auto element = object.try_as<FrameworkElement>();
        if (element) {
            if (auto state = Observe(element)) Apply(state);
        } else Wh_Log(L"OnShowing object has no FrameworkElement interface");
    } catch (...) { Wh_Log(L"Quick Settings showing callback failed: %08X", winrt::to_hresult().value); }
    return hr;
}

bool HookShowing(HMODULE module) {
    // Exact ABI producer name verified in the running ShellHost. Keep the
    // struct qualifiers: they are part of Windhawk's undecorated symbol name.
    WindhawkUtils::SYMBOL_HOOK controlCenterDllHooks[] = {
        {{L"public: virtual int __cdecl winrt::impl::produce<struct winrt::ControlCenter::implementation::MediaTransportControls,struct winrt::ControlCenter::IMediaTransportControls>::OnShowing(void)"},
         &showingOriginal, Showing},
    };
    bool hooked = WindhawkUtils::HookSymbols(module, controlCenterDllHooks,
                                           ARRAYSIZE(controlCenterDllHooks));
    Wh_Log(L"%s", hooked ? L"OnShowing hooked" : L"OnShowing symbol hook unavailable");
    return hooked;
}

HMODULE Module() {
    return GetModuleHandleW(L"ControlCenter.dll");
}
void Hook(bool apply) {
    auto module = Module();
    if (!module || g_unloading || attempted.exchange(true)) return;
    auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    try {
        if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) winrt::throw_hresult(hr);
        auto factoryFn = reinterpret_cast<HRESULT(WINAPI*)(void*,void**)>(
            GetProcAddress(module, "DllGetActivationFactory"));
        if (!factoryFn) winrt::throw_hresult(E_NOINTERFACE);
        winrt::hstring name = L"ControlCenter.XamlMetaDataProvider";
        winrt::Windows::Foundation::IActivationFactory factory{nullptr};
        winrt::check_hresult(factoryFn(winrt::get_abi(name), winrt::put_abi(factory)));
        auto provider = factory.ActivateInstance<winrt::Windows::Foundation::IInspectable>()
            .as<winrt::Windows::UI::Xaml::Markup::IXamlMetadataProvider>();
        auto type = provider.GetXamlType(L"ControlCenter.MediaTransportControls");
        if (!type || !type.IsConstructible()) winrt::throw_hresult(E_NOINTERFACE);
        auto target = (*reinterpret_cast<void***>(winrt::get_abi(type)))[18];
        if (!Wh_SetFunctionHook(target, reinterpret_cast<void*>(Activate),
                reinterpret_cast<void**>(&activateOriginal)))
            winrt::throw_hresult(E_FAIL);
        if (!HookShowing(module))
            Wh_Log(L"Quick Settings showing hook unavailable; existing controls require host recreation");
        if (apply && !Wh_ApplyHookOperations()) winrt::throw_hresult(E_FAIL);
        Wh_Log(L"ShellHost: hooked Quick Settings XAML activation");
    } catch (...) { Wh_Log(L"Quick Settings hook failed: %08X", winrt::to_hresult().value); }
    if (SUCCEEDED(hr)) CoUninitialize();
}
HMODULE WINAPI Load(LPCWSTR name, HANDLE file, DWORD flags) {
    auto module = loadOriginal(name, file, flags);
    if (module && !attempted && module == Module()) Hook(true);
    return module;
}
bool Init() {
    Wh_Log(L"ShellHost initialized; waiting for ControlCenter module");
    auto base = GetModuleHandleW(L"kernelbase.dll");
    auto target = base ? GetProcAddress(base, "LoadLibraryExW") : nullptr;
    if (!target || !Wh_SetFunctionHook(reinterpret_cast<void*>(target),
            reinterpret_cast<void*>(Load), reinterpret_cast<void**>(&loadOriginal))) return false;
    Hook(false);
    return true;
}
void Stop() {
    std::vector<std::shared_ptr<State>> cleanup;
    { std::lock_guard lock(mutex); if (states) cleanup.swap(*states); states.reset(); }
    for (auto const& state : cleanup) {
        try {
            state->dispatcher.RunAsync(CoreDispatcherPriority::Low, [state] {
                if (auto media = state->media.get()) RevokeLoaded(state, media);
                for (auto it = state->saved.rbegin(); it != state->saved.rend(); ++it) it->Restore();
                state->saved.clear();
                state->media = {};
                state->dispatcher = nullptr;
            }).get();
        } catch (...) { Wh_Log(L"Quick Settings UI cleanup failed: %08X", winrt::to_hresult().value); }
    }
}
} // namespace QuickSettings
}  // namespace

BOOL Wh_ModInit() {
    g_repairHorizontalPosition = Wh_GetIntSetting(L"repairHorizontalPosition") != 0;
    g_swapPanels = Wh_GetIntSetting(L"swapPanels") != 0;
    g_swapMediaPanels = Wh_GetIntSetting(L"swapMediaPanels") != 0;

    WCHAR path[MAX_PATH];
    if (!GetModuleFileNameW(nullptr, path, ARRAYSIZE(path))) return FALSE;
    auto name = wcsrchr(path, L'\\');
    g_quickSettingsHost = name && _wcsicmp(name + 1, L"ShellHost.exe") == 0;
    g_shellHost = name && (_wcsicmp(name + 1, L"ShellExperienceHost.exe") == 0 ||
                           _wcsicmp(name + 1, L"ShellHost.exe") == 0);
    if (g_shellHost) {
        if (g_quickSettingsHost) return g_swapMediaPanels && QuickSettings::Init();
        // Media styling belongs exclusively to ShellHost.exe.
        g_swapMediaPanels = false;
        if (!g_swapPanels) return FALSE;
        PanelSwap::g_message = RegisterWindowMessageW(
            L"Windhawk_NotificationCenterFollowsTaskbar_PanelSwap");
        if (!PanelSwap::g_message) return FALSE;
        return PanelSwap::HookWindowCreation();
    }
    return Wh_SetFunctionHook(reinterpret_cast<void*>(SetWindowRgn),
                              reinterpret_cast<void*>(SetWindowRgn_Hook),
                              reinterpret_cast<void**>(&SetWindowRgn_Original)) &&
           Wh_SetFunctionHook(reinterpret_cast<void*>(DwmSetWindowAttribute),
                              reinterpret_cast<void*>(DwmSetWindowAttribute_Hook),
                              reinterpret_cast<void**>(&DwmSetWindowAttribute_Original));
}

void Wh_ModAfterInit() {
    if (g_quickSettingsHost) { QuickSettings::Hook(true); return; }
    if (g_shellHost) {
        PanelSwap::FindExistingWindows();
    }
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    g_repairHorizontalPosition = Wh_GetIntSetting(L"repairHorizontalPosition") != 0;
    g_swapPanels = Wh_GetIntSetting(L"swapPanels") != 0;
    g_swapMediaPanels = Wh_GetIntSetting(L"swapMediaPanels") != 0;

    // Reload removes the ShellExperienceHost half entirely when swapping is off.
    *reload = g_shellHost;
    return TRUE;
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    if (g_quickSettingsHost) { QuickSettings::Stop(); return; }
    if (g_shellHost) {
        PanelSwap::Stop();
        return;
    }
    // Never move windows while holding the state lock.
    for (;;) {
        HWND window;
        {
            std::lock_guard lock(g_mutex);
            if (g_placements.empty()) break;
            window = g_placements.begin()->first;
        }
        Restore(window);
    }
}
