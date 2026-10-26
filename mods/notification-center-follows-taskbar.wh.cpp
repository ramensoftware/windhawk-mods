// ==WindhawkMod==
// @id              notification-center-follows-taskbar
// @name            Notification Center Follows Taskbar
// @description     Correct Notification Center placement for all taskbar positions, with an optional calendar/notifications swap.
// @version         1.0.0
// @author          Asteski
// @github          https://github.com/Asteski
// @include         explorer.exe
// @include         ShellExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lgdi32 -lcomctl32 -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Notification Center Follows Taskbar

![Demonstration](https://raw.githubusercontent.com/Asteski/Windhawk-Mods/refs/heads/main/img/notification-center-follows-taskbar/notification-center-follows-taskbar.png)

Keeps the Windows 11 Notification Center (Win+N, including its calendar)
correctly positioned with top, bottom, left, and right taskbars. Detects the
taskbar on the flyout's monitor automatically. Adds the missing correction for
top taskbars, with an 8 logical pixel vertical gap; retains the horizontal
position chosen by Windows or other mods. Auto-hidden taskbars use the monitor's
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

The top-taskbar fix changes only Y by default, so horizontal alignment from
Shell Flyout Positions is preserved. This mod and Shell Flyout Positions both
observe the flyout being uncloaked; they should not both force its X coordinate.

**Repair right-edge position after Explorer restarts** is optional. It forces
right alignment for top taskbars and overrides a left/center alignment selected
in another mod. Leave it off when using Shell Flyout Positions. It is retained
because we observed a surviving shell host occasionally reopening on the left
after Explorer/taskbar recreation; forcing right alignment corrected that case.

ShellExperienceHost loads this mod only while panel swapping is enabled. Window
creation only schedules initialization; XAML is accessed later on its UI thread.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- swapPanels: false
  $name: Swap notifications and calendar
  $description: Put the calendar above notifications while keeping Windows' native panel sizes and scrolling. No fixed height limit or row sizing overrides.
- repairHorizontalPosition: false
  $name: Repair right-edge position after Explorer restarts
  $description: Force right alignment with top taskbars. Leave off to preserve horizontal alignment from Windows or Shell Flyout Positions.
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
#include <commctrl.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace {
decltype(&SetWindowRgn) SetWindowRgn_Original;
decltype(&DwmSetWindowAttribute) DwmSetWindowAttribute_Original;
std::atomic<bool> g_unloading{false};
std::atomic<bool> g_swapPanels{false};
std::atomic<bool> g_repairHorizontalPosition{false};
bool g_shellHost = false;
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
                                 UINT dpi, LONG nativeX,
                                 bool repairHorizontalPosition = false) {
    LONG verticalGap = MulDiv(8, dpi ? dpi : 96, 96);
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
    { std::lock_guard lock(g_candidateMutex); g_candidates.clear(); }
}
}  // namespace PanelSwap
}  // namespace

BOOL Wh_ModInit() {
    g_swapPanels = Wh_GetIntSetting(L"swapPanels") != 0;
    g_repairHorizontalPosition = Wh_GetIntSetting(L"repairHorizontalPosition") != 0;
    WCHAR path[MAX_PATH];
    if (!GetModuleFileNameW(nullptr, path, ARRAYSIZE(path))) return FALSE;
    auto name = wcsrchr(path, L'\\');
    g_shellHost = name && _wcsicmp(name + 1, L"ShellExperienceHost.exe") == 0;
    if (g_shellHost) {
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
    if (g_shellHost) {
        PanelSwap::FindExistingWindows();
    }
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    g_swapPanels = Wh_GetIntSetting(L"swapPanels") != 0;
    g_repairHorizontalPosition = Wh_GetIntSetting(L"repairHorizontalPosition") != 0;
    // Reload removes the ShellExperienceHost half entirely when swapping is off.
    *reload = g_shellHost;
    return TRUE;
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
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


