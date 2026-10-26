// ==WindhawkMod==
// @id              notification-center-follows-taskbar
// @name            Notification Center Follows Taskbar
// @description     Correct Notification Center placement for all taskbar positions, with an optional calendar/notifications swap.
// @version         1.0.0
// @author          Asteski
// @include         explorer.exe
// @include         ShellExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lgdi32 -lcomctl32 -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Notification Center Follows Taskbar

Keeps the Windows 11 Notification Center (Win+N, including its calendar)
correctly positioned with top, bottom, left, and right taskbars. Detects the
taskbar on the flyout's monitor automatically. Adds the missing correction for
top taskbars, anchored to the right with an 8 logical pixel vertical gap; retains
Windows' placement for the other edges. Auto-hidden taskbars use the monitor's
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
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- swapPanels: false
  $name: Swap notifications and calendar
  $description: Put the calendar above notifications while keeping Windows' native panel sizes and scrolling. No fixed height limit or row sizing overrides.
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
#include <windhawk_utils.h>
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
        rect.left == saved.appliedX && rect.top == saved.appliedY) {
        SetWindowPos(window, nullptr, saved.originalX, saved.originalY, 0, 0,
                     SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

POINT CalculateTopTaskbarPosition(const MONITORINFO& info, const RECT& visible,
                                 UINT dpi) {
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
    return {right - visible.right, top + verticalGap - visible.top};
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
    POINT target = CalculateTopTaskbarPosition(info, visible, dpi);
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
            if (it->second.processId != pid || it->second.monitor != monitor ||
                outer.left != it->second.appliedX || outer.top != it->second.appliedY) {
                it->second.originalX = outer.left;
                it->second.originalY = outer.top;
            }
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
HANDLE g_stopEvent = nullptr, g_worker = nullptr;
std::mutex g_candidateMutex;
std::vector<HWND> g_candidates;
struct CoreWindowInterop : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE get_WindowHandle(HWND* window) = 0;
};
// XAML state is exclusively owned by the ActionCenter UI thread.
Grid g_grid{nullptr};
FrameworkElement g_notifications{nullptr}, g_calendar{nullptr};
TranslateTransform g_notificationShift{nullptr}, g_calendarShift{nullptr};
winrt::event_token g_layoutToken{};
bool g_hasLayoutToken = false, g_updating = false;

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
std::vector<SavedProperty> g_saved;

void RestoreLayout() noexcept {
    if (g_grid && g_hasLayoutToken) {
        try { g_grid.LayoutUpdated(g_layoutToken); } catch (...) {}
    }
    g_hasLayoutToken = false;
    for (auto it = g_saved.rbegin(); it != g_saved.rend(); ++it) {
        try { it->Restore(); } catch (...) {}
    }
    g_saved.clear();
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
    g_saved.emplace_back(element, UIElement::RenderTransformProperty());
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
        Wh_Log(L"Panel swap skipped: panels are already reordered by another style");
        return;
    }
    g_grid = grid;
    g_notifications = notifications;
    g_calendar = calendar;
    // Keep the lower notifications panel's shadow above the calendar after
    // swapping. The shared restoration path preserves any prior local value
    // or binding when the option is disabled or the mod is unloaded.
    g_saved.emplace_back(notifications, Canvas::ZIndexProperty());
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
        constexpr GUID iid{0x45d64a29, 0xa63e, 0x4cb6,
                           {0xb4, 0x98, 0x57, 0x81, 0xd2, 0x98, 0xcb, 0x4f}};
        winrt::com_ptr<CoreWindowInterop> interop;
        if (FAILED(winrt::get_unknown(window.CoreWindow())->QueryInterface(iid, interop.put_void()))) return;
        HWND actual = nullptr;
        if (FAILED(interop->get_WindowHandle(&actual)) || actual != nativeWindow) return;
        auto grid = FindPanelGrid(window.Content());
        if (!grid) {
            if (owner == nativeWindow) {
                RestoreLayout();
                g_window = nullptr;
            }
            return;
        }
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
                            LPARAM lParam, DWORD_PTR) {
    if (message == g_message) {
        if (g_unloading || !g_swapPanels) {
            KillTimer(window, reinterpret_cast<UINT_PTR>(&g_window));
            if (g_window == window) RestoreLayout();
        } else {
            // This message is posted, never called from CreateWindowEx or a
            // loader callback. XAML is only touched from the normal UI loop.
            SetTimer(window, reinterpret_cast<UINT_PTR>(&g_window), 250, nullptr);
            Refresh(window);
        }
        return 0;
    }
    if (message == WM_TIMER && wParam == reinterpret_cast<UINT_PTR>(&g_window)) {
        BOOL cloaked = FALSE;
        if (!g_swapPanels || (IsWindowVisible(window) &&
            SUCCEEDED(DwmGetWindowAttribute(window, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && !cloaked))
            Refresh(window);
        return 0;
    }
    if (message == WM_NCDESTROY) {
        KillTimer(window, reinterpret_cast<UINT_PTR>(&g_window));
        if (g_window == window) {
            RestoreLayout();
            g_window = nullptr;
        }
        std::lock_guard lock(g_candidateMutex);
        std::erase(g_candidates, window);
    }
    return DefSubclassProc(window, message, wParam, lParam);
}

void Attach(HWND window) {
    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    WCHAR name[64];
    if (g_unloading || pid != GetCurrentProcessId() ||
        !GetClassNameW(window, name, ARRAYSIZE(name)) ||
        wcscmp(name, L"Windows.UI.Core.CoreWindow") != 0) return;
    {
        std::lock_guard lock(g_candidateMutex);
        if (std::find(g_candidates.begin(), g_candidates.end(), window) != g_candidates.end()) return;
        g_candidates.push_back(window);
    }
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(window, WindowProc, 0)) {
        std::lock_guard lock(g_candidateMutex);
        std::erase(g_candidates, window);
        return;
    }
    PostMessageW(window, g_message, 0, 0);
}

DWORD WINAPI Worker(void*) {
    // No CreateWindowEx hook: calling XAML from within window construction can
    // re-enter initialization and deadlock the host.
    while (WaitForSingleObject(g_stopEvent, 250) == WAIT_TIMEOUT) {
        if (g_swapPanels && !g_window) {
            EnumWindows([](HWND window, LPARAM) -> BOOL {
                Attach(window);
                return TRUE;
            }, 0);
        }
    }
    return 0;
}

void Stop() {
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_worker) {
        WaitForSingleObject(g_worker, INFINITE);
        CloseHandle(g_worker);
        g_worker = nullptr;
    }
    std::vector<HWND> candidates;
    {
        std::lock_guard lock(g_candidateMutex);
        candidates = g_candidates;
    }
    for (HWND window : candidates) {
        SendMessageW(window, g_message, 0, 0);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(window, WindowProc);
    }
    g_window = nullptr;
    { std::lock_guard lock(g_candidateMutex); g_candidates.clear(); }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}
}  // namespace PanelSwap
}  // namespace

BOOL Wh_ModInit() {
    g_swapPanels = Wh_GetIntSetting(L"swapPanels") != 0;
    WCHAR path[MAX_PATH];
    if (!GetModuleFileNameW(nullptr, path, ARRAYSIZE(path))) return FALSE;
    auto name = wcsrchr(path, L'\\');
    g_shellHost = name && _wcsicmp(name + 1, L"ShellExperienceHost.exe") == 0;
    if (g_shellHost) {
        PanelSwap::g_message = RegisterWindowMessageW(
            L"Windhawk_NotificationCenterFollowsTaskbar_PanelSwap");
        if (!PanelSwap::g_message) return FALSE;
        PanelSwap::g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        return PanelSwap::g_stopEvent != nullptr;
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
        PanelSwap::g_worker = CreateThread(nullptr, 0, PanelSwap::Worker, nullptr, 0, nullptr);
        if (!PanelSwap::g_worker) Wh_Log(L"Panel swap discovery thread failed: %lu", GetLastError());
    }
}

void Wh_ModSettingsChanged() {
    g_swapPanels = Wh_GetIntSetting(L"swapPanels") != 0;
    if (g_shellHost) {
        std::vector<HWND> candidates;
        {
            std::lock_guard lock(PanelSwap::g_candidateMutex);
            candidates = PanelSwap::g_candidates;
        }
        for (HWND window : candidates)
            PostMessageW(window, PanelSwap::g_message, 0, 0);
    }
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

