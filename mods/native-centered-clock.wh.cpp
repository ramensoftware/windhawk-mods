// ==WindhawkMod==
// @id              native-centered-clock
// @name            Native Centered Taskbar Clock
// @description     Center the native taskbar clock and show a customizable single-line time/date
// @version         0.4.1
// @author          qwerty3i
// @github          https://github.com/qwerty3i
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Native Centered Taskbar Clock

Moves the actual Windows 11 clock/notification button into a centered layer
of the **existing native taskbar**. This is not a second taskbar, a Rainmeter
skin, or a synthetic clock. Clicks still target the native button.

**Before enabling:** In Windows Settings > Personalization > Taskbar >
Taskbar behaviors, choose **Left** for taskbar alignment. With centered app
icons, the clock and apps would compete for the same space.

Version 0.4.1 keeps the working v0.3 native-clock centering, with a single-line
clock readout inside the original Windows clock button. Default format:
`1639 FRI 09 OCT 2026` (the time/date are live, using your local timezone).
Change **Font size** in Windhawk Settings (default 14). Clock text stays on
one line, and clicking it still clicks the original Windows clock button.

The original two clock text elements are hidden, not deleted. Visibility
change callbacks keep them hidden when Windows refreshes the native clock.
Disabling the mod removes these callbacks, restores the original text and
removes the custom text and timer.
Open the mod's **Log** tab if the clock doesn't move. Look for lines
starting with `[NCC]`, then share those lines for build-specific diagnosis.

This remains an experimental prototype for Windows 11's modern XAML taskbar.
Centering and single-line text formatting have been tested on Windows 11
Insider build 26340.9616 (Windhawk 1.7.3). Other Windows builds are not
verified. Do not run simultaneously with Taskbar Clock to Left or another
mod which reparents the notification button.

Disabling the mod should restore the button to its native tray parent. If
Explorer has an issue, disable this mod in Windhawk and restart Explorer.

Author: [qwerty3i](https://github.com/qwerty3i). This mod was developed with
ChatGPT assistance and tested by the submitter.

The native XAML clock-button relocation approach was adapted from
[Taskbar Clock to Left](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-clock-to-left.wh.cpp)
by [pleromyst](https://github.com/pleromyst), licensed under GPL-3.0.
This mod is distributed under GPL-3.0 as well.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- FontSize: 14
  $name: Clock font size
  $description: Font size of the single-line clock text (10-32 recommended).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <winver.h>

#include <algorithm>
#include <cmath>
#include <atomic>
#include <optional>
#include <vector>
#include <cwchar>
#include <chrono>

#undef GetCurrentTime
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;
namespace Controls = winrt::Windows::UI::Xaml::Controls;
namespace Media = winrt::Windows::UI::Xaml::Media;

static std::atomic<bool> g_enabled{false};
static std::atomic<int> g_fontSize{14};
static std::atomic<bool> g_hooked{false};
static std::atomic<HMODULE> g_lastFailedModule{nullptr};
static std::atomic<int> g_diagnosticCount{0};
static std::atomic<int> g_templateCalls{0};
static std::atomic<int> g_viewModelCalls{0};
static std::atomic<int> g_visibilityResets{0};
static std::atomic<bool> g_reportedWaitingForModule{false};
static std::atomic<DWORD> g_uiThread{0};
static std::atomic_flag g_setupLock = ATOMIC_FLAG_INIT;

struct Pending {
    winrt::weak_ref<FrameworkElement> content;
    std::optional<winrt::event_token> loaded;
    std::optional<winrt::event_token> layout;
    unsigned attempts = 0;
};
struct Moved {
    winrt::weak_ref<FrameworkElement> clock;
    winrt::weak_ref<Controls::Panel> parent;
    winrt::weak_ref<Controls::Grid> root;
    Controls::Grid host{nullptr};
    uint32_t index{};
    int column{}, columnSpan{}, row{}, rowSpan{};
    HorizontalAlignment horizontal{};
    VerticalAlignment vertical{};
    Thickness margin{};

    // All are owned by the *native* notification button, never an overlay.
    winrt::weak_ref<Controls::StackPanel> textPanel;
    winrt::weak_ref<Controls::TextBlock> timeText;
    winrt::weak_ref<Controls::TextBlock> dateText;
    Visibility originalTimeVisibility{Visibility::Visible};
    Visibility originalDateVisibility{Visibility::Visible};
    std::optional<int64_t> timeVisibilityCallbackToken;
    std::optional<int64_t> dateVisibilityCallbackToken;
    Controls::TextBlock formattedText{nullptr};
    DispatcherTimer clockTimer{nullptr};
    std::optional<winrt::event_token> clockTick;
};

// Do not destroy XAML objects on Explorer's arbitrary DLL-unload thread.
[[clang::no_destroy]] static std::optional<std::vector<Pending>> g_pending{std::in_place};
[[clang::no_destroy]] static std::optional<std::vector<Moved>> g_moved{std::in_place};

using OnApplyTemplate_t = void(WINAPI*)(void*);
static OnApplyTemplate_t OnApplyTemplate_Original = nullptr;
using GetViewModel_t = HRESULT(WINAPI*)(void*, void*);
static GetViewModel_t GetViewModel_Original = nullptr;
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

// Limit XAML diagnostic output while LayoutUpdated may fire repeatedly.
static bool DiagnosticNow() {
    int count = ++g_diagnosticCount;
    return count <= 8 || count % 100 == 0;
}

static FrameworkElement FindClockButton(FrameworkElement content) {
    for (auto element = content; element;) {
        if (element.Name() == L"NotificationCenterButton" &&
            winrt::get_class_name(element) == L"SystemTray.OmniButton") {
            return element;
        }
        element = Media::VisualTreeHelper::GetParent(element)
                      .try_as<FrameworkElement>();
    }
    return nullptr;
}

static bool IsLeftAligned() {
    DWORD setting = 1;
    DWORD size = sizeof(setting);
    RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
        L"TaskbarAl", RRF_RT_REG_DWORD, nullptr, &setting, &size);
    return setting == 0;
}

// Format on the local Windows clock; the native clock button provides the
// click target. Text is deliberately fixed in English uppercase for the
// requested GNOME-like 24-hour output.
static winrt::hstring FormatClockLine() {
    static constexpr const wchar_t* days[] = {
        L"SUN", L"MON", L"TUE", L"WED", L"THU", L"FRI", L"SAT"
    };
    static constexpr const wchar_t* months[] = {
        L"JAN", L"FEB", L"MAR", L"APR", L"MAY", L"JUN",
        L"JUL", L"AUG", L"SEP", L"OCT", L"NOV", L"DEC"
    };
    SYSTEMTIME t{};
    GetLocalTime(&t);
    wchar_t result[64]{};
    swprintf_s(result, ARRAYSIZE(result), L"%02u%02u %ls %02u %ls %04u",
               t.wHour, t.wMinute, days[t.wDayOfWeek % 7], t.wDay,
               months[(t.wMonth >= 1 && t.wMonth <= 12) ? t.wMonth - 1 : 0],
               t.wYear);
    return result;
}

static FrameworkElement FindDescendant(DependencyObject parent,
                                        wchar_t const* target, int depth = 0) {
    if (!parent || depth > 12) return nullptr;
    const int count = Media::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; ++i) {
        auto child = Media::VisualTreeHelper::GetChild(parent, i);
        if (auto element = child.try_as<FrameworkElement>()) {
            if (element.Name() == target) return element;
        }
        if (auto found = FindDescendant(child, target, depth + 1)) return found;
    }
    return nullptr;
}

static void RestoreClockText(Moved& m) {
    if (m.clockTimer) {
        m.clockTimer.Stop();
        if (m.clockTick) m.clockTimer.Tick(*m.clockTick);
        m.clockTick.reset();
        m.clockTimer = nullptr;
    }
    if (auto panel = m.textPanel.get()) {
        if (m.formattedText) {
            auto children = panel.Children();
            uint32_t at{};
            if (children.IndexOf(m.formattedText.as<UIElement>(), at)) {
                children.RemoveAt(at);
            }
        }
    }
    // Deregister BEFORE restoring their original visibility. Otherwise the
    // callback would immediately collapse the Windows-owned clock text again.
    if (auto time = m.timeText.get()) {
        if (m.timeVisibilityCallbackToken) {
            time.UnregisterPropertyChangedCallback(
                UIElement::VisibilityProperty(), *m.timeVisibilityCallbackToken);
            m.timeVisibilityCallbackToken.reset();
        }
        time.Visibility(m.originalTimeVisibility);
    }
    if (auto date = m.dateText.get()) {
        if (m.dateVisibilityCallbackToken) {
            date.UnregisterPropertyChangedCallback(
                UIElement::VisibilityProperty(), *m.dateVisibilityCallbackToken);
            m.dateVisibilityCallbackToken.reset();
        }
        date.Visibility(m.originalDateVisibility);
    }
    m.formattedText = nullptr;
}

// Style only the clock's *existing* visual tree. Do not add any top bar or
// screen overlay. Keep the two Windows-owned text blocks intact for rollback.
static bool SetupClockText(FrameworkElement content, Moved& m) {
    if (m.formattedText) return true;
    auto time = FindDescendant(content, L"TimeInnerTextBlock")
                    .try_as<Controls::TextBlock>();
    auto date = FindDescendant(content, L"DateInnerTextBlock")
                    .try_as<Controls::TextBlock>();
    if (!time || !date) {
        if (DiagnosticNow()) Wh_Log(L"[NCC] Waiting for Time/DateInnerTextBlock");
        return false;
    }
    auto panel = Media::VisualTreeHelper::GetParent(time)
                    .try_as<Controls::StackPanel>();
    if (!panel || Media::VisualTreeHelper::GetParent(date) != panel.as<DependencyObject>()) {
        if (DiagnosticNow()) Wh_Log(L"[NCC] Clock text panel structure unexpected");
        return false;
    }

    try {
        Controls::TextBlock label;
        label.Name(L"WindhawkCenteredClockText");
        label.Text(FormatClockLine());
        label.FontSize(static_cast<double>(g_fontSize.load()));
        label.FontFamily(time.FontFamily());
        label.FontWeight(time.FontWeight());
        label.Foreground(time.Foreground());
        label.TextAlignment(TextAlignment::Center);
        label.TextWrapping(TextWrapping::NoWrap);
        label.TextTrimming(TextTrimming::None);
        label.HorizontalAlignment(HorizontalAlignment::Center);
        label.VerticalAlignment(VerticalAlignment::Center);
        label.IsHitTestVisible(false);  // Events still reach native clock button.

        m.textPanel = panel;
        m.timeText = time;
        m.dateText = date;
        m.originalTimeVisibility = time.Visibility();
        m.originalDateVisibility = date.Visibility();
        m.formattedText = label;
        panel.Children().Append(label.as<UIElement>());

        // On this Insider build, the native clock's bindings can set Visible
        // again after we collapse the original date/time text. Observe changes
        // and re-collapse. This is the approach used by Windhawk's established
        // Taskbar Clock Customization mod; it avoids removing native XAML nodes.
        auto keepHidden = [](DependencyObject const& sender,
                             DependencyProperty const&) {
            if (!g_enabled.load()) return;
            if (auto block = sender.try_as<Controls::TextBlock>()) {
                if (block.Visibility() != Visibility::Collapsed) {
                    const int attempts = ++g_visibilityResets;
                    if (attempts <= 5) {
                        Wh_Log(L"[NCC] Native clock text visibility reset; hiding again (%d)",
                               attempts);
                    }
                    block.Visibility(Visibility::Collapsed);
                }
            }
        };
        m.timeVisibilityCallbackToken = time.RegisterPropertyChangedCallback(
            UIElement::VisibilityProperty(), keepHidden);
        m.dateVisibilityCallbackToken = date.RegisterPropertyChangedCallback(
            UIElement::VisibilityProperty(), keepHidden);
        time.Visibility(Visibility::Collapsed);
        date.Visibility(Visibility::Collapsed);

        // Use a UI-thread XAML timer instead of changing the clock's bound
        // Windows text. The original bindings remain available when disabled.
        DispatcherTimer timer;
        timer.Interval(std::chrono::seconds(1));
        auto weakLabel = winrt::make_weak(label);
        m.clockTick = timer.Tick([weakLabel](auto&&, auto&&) {
            if (!g_enabled) return;
            if (auto label = weakLabel.get()) {
                const auto value = FormatClockLine();
                if (label.Text() != value) label.Text(value);
                const double font = static_cast<double>(g_fontSize.load());
                if (label.FontSize() != font) label.FontSize(font);
            }
        });
        m.clockTimer = timer;
        timer.Start();
        Wh_Log(L"[NCC] Native clock label created: one-line text, fontSize=%d",
               g_fontSize.load());
        return true;
    } catch (...) {
        Wh_Log(L"[NCC] Clock text setup failed: %08X", winrt::to_hresult());
        try { RestoreClockText(m); } catch (...) {}
        return false;
    }
}

static void RestoreOne(Moved& m) {
    RestoreClockText(m);
    auto clock = m.clock.get();
    auto parent = m.parent.get();
    auto root = m.root.get();
    if (clock) {
        auto item = clock.as<UIElement>();
        if (m.host) {
            uint32_t i;
            if (m.host.Children().IndexOf(item, i)) {
                m.host.Children().RemoveAt(i);
            }
        }
        // Reattach before changing Grid properties so a failed layout cannot
        // leave a click target stranded in a removed container.
        if (parent && !Media::VisualTreeHelper::GetParent(clock)) {
            auto children = parent.Children();
            children.InsertAt(std::min(m.index, children.Size()), item);
        }
        Controls::Grid::SetColumn(clock, m.column);
        Controls::Grid::SetColumnSpan(clock, m.columnSpan);
        Controls::Grid::SetRow(clock, m.row);
        Controls::Grid::SetRowSpan(clock, m.rowSpan);
        clock.HorizontalAlignment(m.horizontal);
        clock.VerticalAlignment(m.vertical);
        clock.Margin(m.margin);
    }
    if (m.host && root) {
        uint32_t i;
        if (root.Children().IndexOf(m.host.as<UIElement>(), i)) {
            root.Children().RemoveAt(i);
        }
    }
    m.host = nullptr;
}

// Windows Insider builds can insert wrappers between SystemTrayFrame and the
// full taskbar root. Do not assume its immediate parent is a Grid. Only use a
// Grid in the *same ancestor chain* that spans the XAML root width; choosing a
// narrow tray Grid would place the clock in the wrong "center".
static Controls::Grid FindFullWidthAncestorGrid(FrameworkElement trayFrame) {
    auto xamlRoot = trayFrame.XamlRoot();
    if (!xamlRoot) {
        if (DiagnosticNow()) Wh_Log(L"[NCC] Tray frame does not have a XamlRoot yet");
        return nullptr;
    }

    const double targetWidth = xamlRoot.Size().Width;
    const double targetHeight = xamlRoot.Size().Height;
    if (targetWidth <= 0 || targetHeight <= 0) {
        if (DiagnosticNow()) Wh_Log(L"[NCC] XamlRoot has no size: %.1fx%.1f",
                                    targetWidth, targetHeight);
        return nullptr;
    }

    Controls::Grid best{nullptr};
    // Prefer the highest qualifying Grid so the host spans the *whole* display
    // and lives above the taskbar button/tray layout. Stop at the XAML root.
    int depth = 0;
    for (auto current = trayFrame; current && depth < 32; ++depth) {
        if (auto grid = current.try_as<Controls::Grid>()) {
            const double w = grid.ActualWidth();
            const double h = grid.ActualHeight();
            const double tolerance = std::max(32.0, targetWidth * 0.07);
            if (grid.IsLoaded() && h > 10.0 &&
                std::abs(w - targetWidth) <= tolerance) {
                best = grid;
            }
        }
        current = Media::VisualTreeHelper::GetParent(current)
                      .try_as<FrameworkElement>();
    }

    if (!best && DiagnosticNow()) {
        Wh_Log(L"[NCC] No full-width Grid in taskbar ancestors; "
               L"XamlRoot=%.1fx%.1f. Parent chain:",
               targetWidth, targetHeight);
        auto current = trayFrame;
        for (int i = 0; current && i < 16; ++i) {
            Wh_Log(L"[NCC] ancestor[%d] type=%s name=%s size=%.1fx%.1f",
                   i, winrt::get_class_name(current).c_str(),
                   current.Name().c_str(), current.ActualWidth(),
                   current.ActualHeight());
            current = Media::VisualTreeHelper::GetParent(current)
                          .try_as<FrameworkElement>();
        }
    }
    return best;
}

static bool CenterClock(FrameworkElement content) {
    if (!g_enabled || !g_moved) return false;
    if (!IsLeftAligned()) {
        if (DiagnosticNow()) Wh_Log(L"[NCC] Taskbar alignment is centered; select Left in Windows taskbar settings");
        return false;
    }
    if (!content.IsLoaded()) {
        if (DiagnosticNow()) Wh_Log(L"[NCC] Waiting for clock content to load");
        return false;
    }
    auto clock = FindClockButton(content);
    if (!clock) {
        if (DiagnosticNow()) Wh_Log(L"[NCC] DateTime content has no NotificationCenterButton ancestor yet");
        return false;
    }
    for (auto& m : *g_moved) {
        if (m.clock.get() == clock) return true;
    }
    auto parent = Media::VisualTreeHelper::GetParent(clock)
                      .try_as<Controls::Panel>();
    if (!parent || parent.Name() != L"SystemTrayFrameGrid") {
        if (DiagnosticNow()) Wh_Log(L"[NCC] Clock parent unexpected: name=%s class=%s",
            parent ? parent.Name().c_str() : L"<null>",
            parent ? winrt::get_class_name(parent).c_str() : L"<null>");
        return false;
    }
    auto trayFrame = Media::VisualTreeHelper::GetParent(parent)
                         .try_as<FrameworkElement>();
    if (!trayFrame ||
        winrt::get_class_name(trayFrame) != L"SystemTray.SystemTrayFrame") {
        if (DiagnosticNow()) Wh_Log(L"[NCC] SystemTrayFrame not ready");
        return false;
    }
    auto root = FindFullWidthAncestorGrid(trayFrame);
    if (!root) return false;

    auto item = clock.as<UIElement>();
    uint32_t index{};
    if (!parent.Children().IndexOf(item, index)) {
        if (DiagnosticNow()) Wh_Log(L"[NCC] Clock is not in its original panel");
        return false;
    }

    Controls::Grid host;
    host.HorizontalAlignment(HorizontalAlignment::Center);
    host.VerticalAlignment(VerticalAlignment::Stretch);
    Controls::Grid::SetColumn(host, 0);
    Controls::Grid::SetColumnSpan(host,
        std::max(1, static_cast<int>(root.ColumnDefinitions().Size())));
    Controls::Grid::SetRow(host, 0);
    Controls::Grid::SetRowSpan(host,
        std::max(1, static_cast<int>(root.RowDefinitions().Size())));
    Controls::Canvas::SetZIndex(host, 100);

    g_moved->push_back(Moved{
        .clock = clock, .parent = parent, .root = root, .host = host,
        .index = index,
        .column = Controls::Grid::GetColumn(clock),
        .columnSpan = Controls::Grid::GetColumnSpan(clock),
        .row = Controls::Grid::GetRow(clock),
        .rowSpan = Controls::Grid::GetRowSpan(clock),
        .horizontal = clock.HorizontalAlignment(),
        .vertical = clock.VerticalAlignment(),
        .margin = clock.Margin(),
    });
    try {
        parent.Children().RemoveAt(index);
        Controls::Grid::SetColumn(clock, 0);
        Controls::Grid::SetColumnSpan(clock, 1);
        Controls::Grid::SetRow(clock, 0);
        Controls::Grid::SetRowSpan(clock, 1);
        clock.HorizontalAlignment(HorizontalAlignment::Center);
        clock.VerticalAlignment(VerticalAlignment::Stretch);
        clock.Margin(Thickness{0, 0, 0, 0});
        host.Children().Append(item);
        root.Children().Append(host.as<UIElement>());
        parent.InvalidateMeasure();
        root.InvalidateMeasure();
        Wh_Log(L"[NCC] Native clock moved to centered native host: "
               L"root=%s width=%.1f, XamlRoot width=%.1f",
            root.Name().c_str(), root.ActualWidth(),
            root.XamlRoot().Size().Width);
        return true;
    } catch (...) {
        Wh_Log(L"Clock centering failed: %08X", winrt::to_hresult());
        try { RestoreOne(g_moved->back()); } catch (...) {}
        g_moved->pop_back();
        return false;
    }
}

static Pending& GetPending(FrameworkElement content) {
    for (auto& p : *g_pending) {
        if (p.content.get() == content) return p;
    }
    g_pending->push_back(Pending{.content = content});
    return g_pending->back();
}

static void AttemptClock(FrameworkElement content) {
    if (!g_enabled || !g_pending || !g_moved) return;
    if (g_uiThread && g_uiThread != GetCurrentThreadId()) {
        if (DiagnosticNow()) Wh_Log(L"[NCC] Skipping UI callback on another thread");
        return;
    }
    g_uiThread = GetCurrentThreadId();
    // OnApplyTemplate can fire before the clock joins the visual tree.
    // Keep tracking its Loaded/LayoutUpdated events until its ancestors exist.
    auto& p = GetPending(content);
    if (content.IsLoaded() && CenterClock(content)) {
        for (auto& moved : *g_moved) {
            if (moved.clock.get() == FindClockButton(content) &&
                SetupClockText(content, moved)) {
                if (p.layout) { content.LayoutUpdated(*p.layout); p.layout.reset(); }
                if (p.loaded) { content.Loaded(*p.loaded); p.loaded.reset(); }
                return;
            }
        }
    }
    if (!content.IsLoaded()) {
        if (!p.loaded) {
            auto weak = winrt::make_weak(content);
            p.loaded = content.Loaded([weak](auto&&, auto&&) {
                if (auto c = weak.get()) {
                    try { AttemptClock(c); }
                    catch (...) { Wh_Log(L"Clock loaded callback failed"); }
                }
            });
        }
    } else if (IsLeftAligned() && !p.layout && p.attempts < 200) {
        auto weak = winrt::make_weak(content);
        p.layout = content.LayoutUpdated([weak](auto&&, auto&&) {
            if (auto c = weak.get()) {
                try {
                    auto& pending = GetPending(c);
                    if (++pending.attempts > 600) {
                        Wh_Log(L"[NCC] Layout wait expired after 600 attempts");
                        if (pending.layout) {
                            c.LayoutUpdated(*pending.layout);
                            pending.layout.reset();
                        }
                    } else {
                        AttemptClock(c);
                    }
                } catch (...) { Wh_Log(L"Clock layout callback failed"); }
            }
        });
    }
}

static void WINAPI OnApplyTemplate_Hook(void* self) {
    OnApplyTemplate_Original(self);
    if (++g_templateCalls <= 3) Wh_Log(L"[NCC] DateTimeIconContent template hook invoked");
    try {
        FrameworkElement content{nullptr};
        IUnknown* unk = self ? reinterpret_cast<IUnknown**>(self)[1] : nullptr;
        if (unk && SUCCEEDED(unk->QueryInterface(
                winrt::guid_of<FrameworkElement>(), winrt::put_abi(content)))) {
            AttemptClock(content);
        } else if (g_templateCalls <= 3) {
            Wh_Log(L"[NCC] Could not obtain clock FrameworkElement from template");
        }
    } catch (...) { Wh_Log(L"[NCC] OnApplyTemplate failed: %08X", winrt::to_hresult()); }
}
static HRESULT WINAPI GetViewModel_Hook(void* self, void* out) {
    HRESULT hr = GetViewModel_Original(self, out);
    if (!g_enabled || !self) return hr;
    try {
        winrt::Windows::Foundation::IInspectable obj{nullptr};
        if (SUCCEEDED(reinterpret_cast<IUnknown*>(self)->QueryInterface(
                winrt::guid_of<winrt::Windows::Foundation::IInspectable>(),
                winrt::put_abi(obj))) &&
            winrt::get_class_name(obj) == L"SystemTray.DateTimeIconContent") {
            if (++g_viewModelCalls <= 3) Wh_Log(L"[NCC] Clock ViewModel hook invoked");
            AttemptClock(obj.as<FrameworkElement>());
        }
    } catch (...) { Wh_Log(L"[NCC] ViewModel callback failed: %08X", winrt::to_hresult()); }
    return hr;
}

// Taskbar icons moved between modules in different Windows 11 releases.
// Follow the module-selection logic used by Taskbar Clock to Left.
static HMODULE GetClockModule() {
    if (HMODULE module = GetModuleHandleW(L"SystemTray.dll")) return module;
    if (HMODULE module = GetModuleHandleW(L"Taskbar.View.dll")) {
        HRSRC res = FindResourceW(module, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION);
        HGLOBAL resource = res ? LoadResource(module, res) : nullptr;
        void* versionData = resource ? LockResource(resource) : nullptr;
        void* fixedInfo = nullptr;
        UINT bytes = 0;
        if (versionData && VerQueryValueW(versionData, L"\\", &fixedInfo, &bytes) &&
            bytes >= sizeof(VS_FIXEDFILEINFO)) {
            auto info = static_cast<VS_FIXEDFILEINFO*>(fixedInfo);
            WORD major = HIWORD(info->dwFileVersionMS);
            if (major && major < 2604) return module;
        }
        // Newer Windows releases load the clock code from SystemTray.dll later.
        return nullptr;
    }
    return GetModuleHandleW(L"ExplorerExtensions.dll");
}

static void TryHookSystemTray(bool applyImmediately) {
    if (g_hooked ||
        g_setupLock.test_and_set(std::memory_order_acquire)) return;
    struct Unlock { ~Unlock() { g_setupLock.clear(std::memory_order_release); } } unlock;
    if (g_hooked) return;

    HMODULE module = GetClockModule();
    if (!module) {
        if (!g_reportedWaitingForModule.exchange(true)) {
            Wh_Log(L"[NCC] Waiting for the Windows clock module to load");
        }
        return;
    }
    if (g_lastFailedModule == module) return;
    wchar_t modulePath[MAX_PATH] = {};
    GetModuleFileNameW(module, modulePath, ARRAYSIZE(modulePath));
    Wh_Log(L"[NCC] Looking for clock hooks in %s", modulePath);

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(public: void __cdecl winrt::SystemTray::implementation::DateTimeIconContent::OnApplyTemplate(void))"},
            &OnApplyTemplate_Original, OnApplyTemplate_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::BadgeIconContent,struct winrt::SystemTray::IBadgeIconContent>::get_ViewModel(void * *))"},
            &GetViewModel_Original, GetViewModel_Hook, true,
        },
    };
    if (WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        g_hooked = true;
        if (applyImmediately) Wh_ApplyHookOperations();
        Wh_Log(L"[NCC] Clock hooks installed successfully");
    } else {
        g_lastFailedModule = module;
        Wh_Log(L"[NCC] Clock symbols not found in %s", modulePath);
    }
}

static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR path, HANDLE file, DWORD flags) {
    HMODULE result = LoadLibraryExW_Original(path, file, flags);
    if (result && !g_hooked) TryHookSystemTray(true);
    return result;
}

static void RefreshClock() {
    // Windows' own clock observes changes to this key. The temporary value is
    // immediately removed, leaving user preferences unchanged.
    HKEY key{};
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
            L"Control Panel\\TimeDate\\AdditionalClocks", 0,
            KEY_SET_VALUE, &key) != ERROR_SUCCESS) {
        Wh_Log(L"[NCC] Could not open the clock refresh registry key");
        return;
    }
    const WCHAR valueName[] = L"_windhawk_native_centered_clock_refresh";
    if (RegSetValueExW(key, valueName, 0, REG_SZ,
            reinterpret_cast<const BYTE*>(L""), sizeof(WCHAR)) == ERROR_SUCCESS) {
        RegDeleteValueW(key, valueName);
        Wh_Log(L"[NCC] Requested native clock refresh");
    }
    RegCloseKey(key);
}

using UiProc = void(*)(void*);
static bool RunOnTaskbarThread(UiProc proc, void* arg) {
    HWND hwnd = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hwnd) return false;
    DWORD tid = GetWindowThreadProcessId(hwnd, nullptr);
    if (!tid || (g_uiThread && tid != g_uiThread)) return false;
    if (tid == GetCurrentThreadId()) { proc(arg); return true; }
    static UINT msg = RegisterWindowMessageW(
        L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct Params { UiProc proc; void* arg; bool ran; } params{proc, arg, false};
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC,
        [](int code, WPARAM wp, LPARAM lp) -> LRESULT {
            if (code == HC_ACTION) {
                auto m = reinterpret_cast<const CWPSTRUCT*>(lp);
                if (m->message == msg) {
                    auto p = reinterpret_cast<Params*>(m->lParam);
                    p->proc(p->arg);
                    p->ran = true;
                }
            }
            return CallNextHookEx(nullptr, code, wp, lp);
        }, nullptr, tid);
    if (!hook) return false;
    SendMessageW(hwnd, msg, 0, reinterpret_cast<LPARAM>(&params));
    UnhookWindowsHookEx(hook);
    return params.ran;
}

static void RestoreAll(void*) {
    if (g_pending) {
        for (auto& p : *g_pending) {
            try {
                if (auto content = p.content.get()) {
                    if (p.loaded) content.Loaded(*p.loaded);
                    if (p.layout) content.LayoutUpdated(*p.layout);
                }
            } catch (...) {}
            p.loaded.reset(); p.layout.reset();
        }
    }
    if (g_moved) {
        for (auto& m : *g_moved) {
            try { RestoreOne(m); }
            catch (...) { Wh_Log(L"Clock restore failed: %08X", winrt::to_hresult()); }
        }
    }
    g_pending.reset();
    g_moved.reset();
}

static void LoadSettings() {
    g_fontSize = std::clamp(Wh_GetIntSetting(L"FontSize"), 8, 40);
}

static void ApplyTextSettings(void*) {
    if (!g_moved) return;
    for (auto& moved : *g_moved) {
        if (moved.formattedText) {
            moved.formattedText.FontSize(static_cast<double>(g_fontSize.load()));
            moved.formattedText.Text(FormatClockLine());
        }
    }
}

BOOL Wh_ModInit() {
    LoadSettings();
    g_enabled = true;
    Wh_Log(L"[NCC] Mod initialized; looking for taskbar clock");
    auto kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto load = reinterpret_cast<LoadLibraryExW_t>(
        GetProcAddress(kernelBase, "LoadLibraryExW"));
    if (!load || !WindhawkUtils::SetFunctionHook(load, LoadLibraryExW_Hook,
                                                &LoadLibraryExW_Original)) {
        return FALSE;
    }
    TryHookSystemTray(false);
    return TRUE;
}
void Wh_ModAfterInit() {
    TryHookSystemTray(true);
    if (g_hooked) RefreshClock();
    else Wh_Log(L"[NCC] No clock hook yet; waiting for clock module");
}
void Wh_ModSettingsChanged() {
    LoadSettings();
    if (!RunOnTaskbarThread(ApplyTextSettings, nullptr)) {
        Wh_Log(L"[NCC] Settings saved; font size will apply on next clock tick");
    }
}
void Wh_ModBeforeUninit() {
    g_enabled = false;
    if (RunOnTaskbarThread(RestoreAll, nullptr)) return;

    // Explorer can recreate Shell_TrayWnd during a mod update. Fall back to
    // the live clock element's XAML dispatcher to remove all event callbacks
    // and restore the original parent before the mod DLL is unloaded.
    if (g_pending) {
        for (const auto& p : *g_pending) {
            auto content = p.content.get();
            if (!content) continue;
            try {
                auto dispatcher = content.Dispatcher();
                if (!dispatcher) continue;
                if (dispatcher.HasThreadAccess()) {
                    RestoreAll(nullptr);
                } else {
                    dispatcher.RunAsync(
                        winrt::Windows::UI::Core::CoreDispatcherPriority::High,
                        [] { RestoreAll(nullptr); }).get();
                }
                return;
            } catch (...) {
                Wh_Log(L"Clock dispatcher cleanup failed: %08X",
                       winrt::to_hresult());
            }
        }
    }
    Wh_Log(L"WARNING: Could not restore the native clock on unload");
}
void Wh_ModUninit() {
    Wh_Log(L"[NCC] Mod unloaded");
}
