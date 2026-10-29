// ==WindhawkMod==
// @id              taskbar-free-float
// @name            Taskbar Free Float (experimental)
// @description     Ctrl+drag the original Windows 11 app bar and tray/clock independently
// @version         0.1.3
// @author          Rikyz
// @github          https://github.com/RikyZ90
// @include         explorer.exe
// @include         StartMenuExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lcomctl32
// @license         GPL-3.0-only
// ==/WindhawkMod==

// Taskbar host discovery adapted from m417z's GPL-3.0 taskbar-tray-show-on-hover.
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-tray-show-on-hover.wh.cpp
// Start menu initialization adapted from m417z's GPL-3.0 shell-flyout-positions.
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/shell-flyout-positions.wh.cpp

// ==WindhawkModReadme==
/*
# Taskbar Free Float

A separate local mod; does not edit other mods, themes or Windows settings.
Moves the original Windows 11 app block and tray/clock into independent native
XAML popup windows. The tray block includes its system icons and clock.

- Ctrl + left mouse drag: float/move the app block or the entire tray/clock block.
- Drag a floating block over its original slot and release to dock it there.
- Escape while dragging: cancel this move.
- A Ctrl+click without dragging is passed through to the taskbar as usual.

The optional **Anchor Start menu to floating Start button** setting is
experimental and disabled by default. It positions Start near its button when
opened, preferring above, then below, then beside it, within the work area.
It supports the classic and redesigned Windows 11 Start layouts on the primary
monitor. It does not follow a drag while the menu stays open. Start menu settings
in Shell Flyout Positions must remain at **Windows default** with **zero shifts**;
its Notification Center and Action Center settings can remain enabled.

## Installation and removal

The mod attempts to attach to a running Explorer process and restore the native
frames when disabled. If the log reports an incomplete restore, restart Explorer
manually after saving any file operations. The mod never restarts Explorer.

For a manual restart, use Task Manager or the optional
[Taskbar Restart Explorer](https://windhawk.net/mods/taskbar-restart-explorer)
mod, which adds a command to the taskbar context menu and remains available
after disabling this mod.

Only the primary Windows 11 XAML taskbar is targeted. Auto-hide is unsupported;
the mod refuses to detach anything while it is enabled. Movement stays on the
taskbar's monitor. Native taskbar space remains reserved at the original edge.
Use a split/dock theme for compact panels. No theme is installed or changed.

The popup containers keep the initial layout size. Dock and float again after
large changes to the icon count or theme. A display/DPI change docks the blocks
before rebuilding their placement. Windows still reserves taskbar space at the
original edge.

This is an experimental release. Native menus, full-screen apps and compatibility
with other mods have not been systematically verified. Changing the visual
ancestors of native controls can affect these features.

Taskbar host discovery is adapted from m417z's taskbar-tray-show-on-hover mod,
published under GNU GPL version 3. This mod is released under GPL-3.0-only.

## Illustrated controls

![Illustrated independent dragging and release-to-finish](https://raw.githubusercontent.com/RikyZ90/windhawk-mods/taskbar-free-float-demo/taskbar-free-float-demo.gif)

Illustrated interaction, not a screen recording. The native taskbar background
and reserved space remain at the original edge.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- anchorStartMenu: false
  $name: Anchor Start menu to floating Start button (experimental)
  $description: >-
    Reposition Start when it opens near the floating Start button. Keep Shell
    Flyout Positions' Start menu settings at Windows default with zero shifts.
*/
// ==/WindhawkModSettings==

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <limits>
#include <utility>

#include <windhawk_utils.h>
#include <shellapi.h>
#include <roapi.h>
#include <winstring.h>
#include <windowsx.h>

#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace Geometry {
struct Position { LONG x; LONG y; };

Position Clamp(Position p, LONG width, LONG height, const RECT& monitor) {
    const auto right = std::max<int64_t>(monitor.left,
        static_cast<int64_t>(monitor.right) - std::max<LONG>(width, 1));
    const auto bottom = std::max<int64_t>(monitor.top,
        static_cast<int64_t>(monitor.bottom) - std::max<LONG>(height, 1));
    return {static_cast<LONG>(std::clamp<int64_t>(p.x, monitor.left, right)),
            static_cast<LONG>(std::clamp<int64_t>(p.y, monitor.top, bottom))};
}

Position AnchorStartMenu(const RECT& button, LONG width, LONG height,
                         const RECT& work, LONG gap) {
    const int64_t above = static_cast<int64_t>(button.top) - gap - height;
    const int64_t below = static_cast<int64_t>(button.bottom) + gap;
    int64_t x = (static_cast<int64_t>(button.left) + button.right - width) / 2;
    int64_t y = above;
    if (above < work.top) {
        y = below;
        if (below + height > work.bottom) {
            const int64_t left = static_cast<int64_t>(button.left) - work.left;
            const int64_t right = static_cast<int64_t>(work.right) - button.right;
            x = right >= left ? static_cast<int64_t>(button.right) + gap
                              : static_cast<int64_t>(button.left) - gap - width;
            y = (static_cast<int64_t>(button.top) + button.bottom - height) / 2;
        }
    }
    x = std::clamp<int64_t>(x, std::numeric_limits<LONG>::min(),
                            std::numeric_limits<LONG>::max());
    y = std::clamp<int64_t>(y, std::numeric_limits<LONG>::min(),
                            std::numeric_limits<LONG>::max());
    return Clamp({static_cast<LONG>(x), static_cast<LONG>(y)}, width, height, work);
}

size_t HostOffsetX64(const BYTE* code) {
    // Fail closed when the private TaskbarHost layout no longer matches.
    if (code[0] == 0x48 && code[1] == 0x83 && code[2] == 0xEC &&
        code[4] == 0x48 && code[5] == 0x83 && code[6] == 0xC1 &&
        code[7] >= sizeof(void*) && code[7] <= 0x7F &&
        code[7] % sizeof(void*) == 0) {
        return code[7];
    }
    return 0;
}

size_t HostOffsetArm64(const BYTE* code) {
    // Match the known ARM64 prologue and decode its pre-indexed host pointer offset.
    const auto* p = reinterpret_cast<const DWORD*>(code);
    if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
        p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
        return (p[3] >> 12) & 0xFF;
    }
    return 0;
}

size_t HostOffset(const BYTE* code) {
#if defined(_M_X64) || defined(__x86_64__)
    return HostOffsetX64(code);
#elif defined(_M_ARM64) || defined(__aarch64__)
    return HostOffsetArm64(code);
#else
    return 0;
#endif
}

size_t HostOffsetSize() {
#if defined(_M_X64) || defined(__x86_64__)
    return 8;
#elif defined(_M_ARM64) || defined(__aarch64__)
    return 16;
#else
    return 0;
#endif
}

bool DragThresholdExceeded(POINT start, POINT current, LONG cx, LONG cy) {
    const int64_t dx = static_cast<int64_t>(current.x) - start.x;
    const int64_t dy = static_cast<int64_t>(current.y) - start.y;
    cx = std::max<LONG>(cx, 1);
    cy = std::max<LONG>(cy, 1);
    return dx <= -static_cast<int64_t>(cx) || dx >= cx ||
           dy <= -static_cast<int64_t>(cy) || dy >= cy;
}
}

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Controls::Primitives;
using winrt::Windows::Foundation::Rect;

std::atomic<bool> g_unloading{false};
std::atomic<bool> g_callbackCleanupFailed{false};
bool g_anchorStartMenu = false;
bool g_isStartHost = false;
uintptr_t g_anchorSequence = 0;
std::atomic<HWND> g_taskbar{nullptr};
std::atomic<DWORD> g_uiThread{0};
UINT g_controlMessage;
constexpr UINT_PTR kTimer = 0x4652464C;
constexpr UINT_PTR kDragTimer = 0x4652464D;
constexpr PCWSTR kAnchorProperties[] = {
    L"Windhawk.FreeFloat.Start.left", L"Windhawk.FreeFloat.Start.top",
    L"Windhawk.FreeFloat.Start.right", L"Windhawk.FreeFloat.Start.bottom",
    L"Windhawk.FreeFloat.Start.sequence"};

void ClearStartAnchor(HWND hwnd) {
    if (hwnd) {
        RemovePropW(hwnd, kAnchorProperties[4]);
        for (size_t i = 0; i < 4; ++i) RemovePropW(hwnd, kAnchorProperties[i]);
    }
}

void PublishStartAnchor();

void KeepFailedCallbacksMapped() {
    if (!g_callbackCleanupFailed) return;
    // shortcut: failed UI-thread cleanup retains this DLL until the host exits.
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN,
        reinterpret_cast<PCWSTR>(&g_callbackCleanupFailed), &module);
    Wh_Log(L"XAML callback cleanup incomplete; restart the affected shell process");
}

void* g_taskBandVtable;
using GetHost_t = void*(WINAPI*)(void*, void**);
GetHost_t g_getHost;
void* g_frameHeight;
using Decref_t = void(WINAPI*)(void*);
Decref_t g_decref;

struct SavedPosition {
    uint32_t version = 1;
    Geometry::Position point{};
};

struct Block {
    FrameworkElement frame{nullptr};
    Panel parent{nullptr};
    Grid placeholder{nullptr};
    Grid host{nullptr};
    Popup popup{nullptr};
    HWND popupWindow = nullptr;
    PCWSTR storageKey = nullptr;
    uint32_t originalIndex = 0;
};

struct State {
    HWND taskbar = nullptr;
    HWND bridge = nullptr;
    XamlRoot root{nullptr};
    std::array<Block, 2> blocks;
    int dragging = -1;
    int dragCandidate = -1;
    HWND inputWindow = nullptr;
    HWND captureWindow = nullptr;
    bool pointerInput = false;
    UINT32 pointerId = 0;
    bool wasFloating = false;
    POINT startCursor{};
    POINT lastCursor{};
    Geometry::Position startPosition{};
    HWND previousForeground = nullptr;
    unsigned retries = 0;
    FrameworkElement layoutSource{nullptr};
    winrt::event_token layoutToken{};
    RECT anchorBounds{};
    bool anchorPublished = false;

    void RevokeLayout() {
        if (layoutSource && layoutToken.value) {
            try { layoutSource.LayoutUpdated(layoutToken); }
            catch (...) { g_callbackCleanupFailed = true; }
        }
        layoutToken = {};
        layoutSource = nullptr;
    }

    ~State() {
        RevokeLayout();
        ClearStartAnchor(taskbar);
    }
};

// Owned and destroyed exclusively on the taskbar's UI thread.
State* g_state = nullptr;

bool Readable(const void* address, size_t size) {
    MEMORY_BASIC_INFORMATION info{};
    if (!address || VirtualQuery(address, &info, sizeof(info)) != sizeof(info) ||
        info.State != MEM_COMMIT || (info.Protect & (PAGE_GUARD | PAGE_NOACCESS))) {
        return false;
    }
    const auto offset = reinterpret_cast<uintptr_t>(address) -
                        reinterpret_cast<uintptr_t>(info.BaseAddress);
    return offset <= info.RegionSize && size <= info.RegionSize - offset;
}

FrameworkElement FindFrame(DependencyObject node, PCWSTR className, int depth = 0) {
    if (!node || depth > 16) return nullptr;
    if (winrt::get_class_name(node) == className) {
        return node.try_as<FrameworkElement>();
    }
    const int count = Media::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i) {
        auto result = FindFrame(Media::VisualTreeHelper::GetChild(node, i),
                                className, depth + 1);
        if (result) return result;
    }
    return nullptr;
}

XamlRoot TaskbarRoot(HWND hwnd) {
    HWND band = reinterpret_cast<HWND>(GetPropW(hwnd, L"TaskbandHWND"));
    auto slots = reinterpret_cast<void**>(band ? GetWindowLongPtrW(band, 0) : 0);
    void* site = nullptr;
    for (size_t i = 0; i < 20; ++i) {
        if (!Readable(slots ? slots + i : nullptr, sizeof(void*))) break;
        if (slots[i] == g_taskBandVtable) { site = slots + i; break; }
    }
    if (!site) return nullptr;

    void* shared[2]{};
    g_getHost(site, shared);
    struct Release {
        void* control;
        ~Release() { if (control) g_decref(control); }
    } release{shared[1]};
    const size_t offset = Geometry::HostOffset(static_cast<const BYTE*>(g_frameHeight));
    if (!shared[0] || !shared[1] || !offset ||
        !Readable(static_cast<BYTE*>(shared[0]) + offset, sizeof(void*))) {
        return nullptr;
    }
    auto object = *reinterpret_cast<IUnknown**>(static_cast<BYTE*>(shared[0]) + offset);
    if (!Readable(object, sizeof(void*))) return nullptr;
    FrameworkElement frame{nullptr};
    if (FAILED(object->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                      winrt::put_abi(frame)))) return nullptr;
    return frame.XamlRoot();
}

HWND FindTaskbar() {
    HWND found = nullptr;
    EnumWindows([](HWND hwnd, LPARAM param) -> BOOL {
        DWORD pid = 0;
        WCHAR name[64]{};
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == GetCurrentProcessId() &&
            GetClassNameW(hwnd, name, ARRAYSIZE(name)) &&
            wcscmp(name, L"Shell_TrayWnd") == 0) {
            *reinterpret_cast<HWND*>(param) = hwnd;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&found));
    return found;
}

HWND FindBridge(HWND shell) {
    HWND bridge = nullptr;
    EnumChildWindows(shell, [](HWND hwnd, LPARAM param) -> BOOL {
        WCHAR name[96]{};
        GetClassNameW(hwnd, name, ARRAYSIZE(name));
        if (wcscmp(name, L"Windows.UI.Composition.DesktopWindowContentBridge") == 0 ||
            wcscmp(name, L"Microsoft.UI.Content.DesktopChildSiteBridge") == 0) {
            *reinterpret_cast<HWND*>(param) = hwnd;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&bridge));
    return bridge;
}

double Scale() {
    const UINT dpi = GetDpiForWindow(g_state->bridge);
    return dpi ? dpi / 96.0 : g_state->root.RasterizationScale();
}

RECT Bounds(FrameworkElement frame) {
    const double scale = Scale();
    if (!std::isfinite(scale) || scale <= 0) winrt::throw_hresult(E_UNEXPECTED);
    auto content = g_state->root.Content();
    auto rect = frame.TransformToVisual(content).TransformBounds(
        Rect{0, 0, static_cast<float>(frame.ActualWidth()),
                   static_cast<float>(frame.ActualHeight())});
    POINT origin{};
    if (!ClientToScreen(g_state->bridge, &origin) ||
        !std::isfinite(rect.X) || !std::isfinite(rect.Y) ||
        !std::isfinite(rect.Width) || !std::isfinite(rect.Height) ||
        rect.Width <= 0 || rect.Height <= 0 ||
        std::abs(rect.X) > 1000000 || std::abs(rect.Y) > 1000000 ||
        rect.Width > 100000 || rect.Height > 100000) {
        Wh_Log(L"Invalid frame bounds: %.2f,%.2f %.2fx%.2f (scale %.3f)",
               rect.X, rect.Y, rect.Width, rect.Height, scale);
        winrt::throw_hresult(E_UNEXPECTED);
    }
    return {origin.x + static_cast<LONG>(std::floor(rect.X * scale)),
            origin.y + static_cast<LONG>(std::floor(rect.Y * scale)),
            origin.x + static_cast<LONG>(std::ceil((rect.X + rect.Width) * scale)),
            origin.y + static_cast<LONG>(std::ceil((rect.Y + rect.Height) * scale))};
}

RECT Bounds(Block& block) { return Bounds(block.frame); }

FrameworkElement FindStartButton(DependencyObject node, int depth = 0) {
    if (!node || depth > 16) return nullptr;
    if (auto element = node.try_as<FrameworkElement>()) {
        if (Automation::AutomationProperties::GetAutomationId(element) == L"StartButton" ||
            element.Name() == L"LaunchListButton") return element;
    }
    const int count = Media::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i) {
        auto button = FindStartButton(Media::VisualTreeHelper::GetChild(node, i), depth + 1);
        if (button) return button;
    }
    return nullptr;
}

void PublishStartAnchor() {
    if (!g_state) return;
    try {
        auto& state = *g_state;
        if (!g_anchorStartMenu || g_unloading || !state.blocks[0].popup) {
            ClearStartAnchor(state.taskbar);
            state.anchorPublished = false;
            return;
        }
        auto button = FindStartButton(state.blocks[0].frame);
        if (!button || button.Visibility() != Visibility::Visible) {
            ClearStartAnchor(state.taskbar);
            state.anchorPublished = false;
            return;
        }
        const RECT rect = Bounds(button);
        if (state.anchorPublished && EqualRect(&rect, &state.anchorBounds)) return;
        if (!SetPropW(state.taskbar, kAnchorProperties[4], nullptr)) {
            ClearStartAnchor(state.taskbar);
            state.anchorPublished = false;
            return;
        }
        const LONG values[]{rect.left, rect.top, rect.right, rect.bottom};
        for (size_t i = 0; i < ARRAYSIZE(values); ++i) {
            if (!SetPropW(state.taskbar, kAnchorProperties[i],
                          reinterpret_cast<HANDLE>(static_cast<INT_PTR>(values[i])))) {
                ClearStartAnchor(state.taskbar);
                state.anchorPublished = false;
                return;
            }
        }
        if (++g_anchorSequence == 0) ++g_anchorSequence;
        state.anchorPublished = SetPropW(state.taskbar, kAnchorProperties[4],
            reinterpret_cast<HANDLE>(g_anchorSequence));
        state.anchorBounds = rect;
        if (!state.anchorPublished) ClearStartAnchor(state.taskbar);
    } catch (...) {
        ClearStartAnchor(g_state->taskbar);
        g_state->anchorPublished = false;
    }
}

bool AutoHide() {
    APPBARDATA data{sizeof(data)};
    return (SHAppBarMessage(ABM_GETSTATE, &data) & ABS_AUTOHIDE) != 0;
}

void Dock(Block& block) {
    if (g_state && &block == &g_state->blocks[0]) {
        ClearStartAnchor(g_state->taskbar);
        g_state->anchorPublished = false;
    }
    if (!block.popup) return;
    block.popup.IsOpen(false);
    auto hosted = block.host.Children();
    uint32_t hostedIndex = 0;
    if (hosted.IndexOf(block.frame, hostedIndex)) hosted.RemoveAt(hostedIndex);
    auto children = block.parent.Children();
    uint32_t at = 0;
    try {
        if (!children.IndexOf(block.frame, at)) {
            if (children.IndexOf(block.placeholder, at)) {
                children.SetAt(at, block.frame);
            } else {
                children.InsertAt(std::min(block.originalIndex, children.Size()), block.frame);
            }
        } else if (children.IndexOf(block.placeholder, at)) {
            children.RemoveAt(at);
        }
    } catch (...) {
        // Keep the original control reachable if its old container rejects it.
        try {
            if (!hosted.IndexOf(block.frame, hostedIndex)) hosted.Append(block.frame);
            block.popup.IsOpen(true);
        } catch (...) {}
        throw;
    }
    block.popup.Child(nullptr);
    if (children.IndexOf(block.popup, at)) children.RemoveAt(at);
    block.popup = nullptr;
    block.placeholder = nullptr;
    block.host = nullptr;
    block.parent = nullptr;
    block.popupWindow = nullptr;
    block.frame.UpdateLayout();
}

void Position(Block& block, Geometry::Position desired) {
    RECT current = Bounds(block);
    MONITORINFO monitor{sizeof(monitor)};
    if (!GetMonitorInfoW(MonitorFromWindow(g_state->taskbar, MONITOR_DEFAULTTOPRIMARY),
                         &monitor)) winrt::throw_last_error();
    desired = Geometry::Clamp(desired, current.right - current.left,
                               current.bottom - current.top, monitor.rcMonitor);
    const double scale = Scale();
    block.popup.HorizontalOffset(block.popup.HorizontalOffset() +
                                  (desired.x - current.left) / scale);
    block.popup.VerticalOffset(block.popup.VerticalOffset() +
                                (desired.y - current.top) / scale);
    block.frame.UpdateLayout();
    if (&block == &g_state->blocks[0]) PublishStartAnchor();
}

HWND FindPopupWindow(const RECT& bounds) {
    struct Search { RECT bounds; HWND found; int64_t area; } search{bounds, nullptr,
        std::numeric_limits<int64_t>::max()};
    EnumThreadWindows(GetCurrentThreadId(), [](HWND hwnd, LPARAM param) -> BOOL {
        auto& s = *reinterpret_cast<Search*>(param);
        WCHAR name[96]{};
        RECT rect{};
        GetClassNameW(hwnd, name, ARRAYSIZE(name));
        if (wcscmp(name, L"Xaml_WindowedPopupClass") != 0 ||
            !IsWindowVisible(hwnd) || !GetWindowRect(hwnd, &rect)) return TRUE;
        const POINT middle{(s.bounds.left + s.bounds.right) / 2,
                           (s.bounds.top + s.bounds.bottom) / 2};
        const int64_t area = static_cast<int64_t>(rect.right - rect.left) *
                             (rect.bottom - rect.top);
        if (PtInRect(&rect, middle) && area > 0 && area < s.area) {
            s.found = hwnd;
            s.area = area;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&search));
    return search.found;
}

bool Float(Block& block) {
    if (block.popup) return true;
    if (AutoHide()) {
        Wh_Log(L"Refusing to detach: native taskbar auto-hide is enabled");
        return false;
    }
    const RECT before = Bounds(block);
    auto parent = Media::VisualTreeHelper::GetParent(block.frame).try_as<Panel>();
    uint32_t index;
    if (!parent || !parent.Children().IndexOf(block.frame, index)) {
        Wh_Log(L"Unsupported frame parent; no changes made");
        return false;
    }

    Popup popup;
    popup.XamlRoot(g_state->root);
    popup.ShouldConstrainToRootBounds(false);
    popup.IsLightDismissEnabled(false);
    popup.HorizontalAlignment(HorizontalAlignment::Left);
    popup.VerticalAlignment(VerticalAlignment::Top);
    popup.DataContext(block.frame.DataContext());
    Grid placeholder;
    placeholder.Width(block.frame.ActualWidth());
    placeholder.Height(block.frame.ActualHeight());
    placeholder.Margin(block.frame.Margin());
    placeholder.HorizontalAlignment(block.frame.HorizontalAlignment());
    placeholder.VerticalAlignment(block.frame.VerticalAlignment());
    placeholder.IsHitTestVisible(false);
    Grid::SetColumn(placeholder, Grid::GetColumn(block.frame));
    Grid::SetRow(placeholder, Grid::GetRow(block.frame));
    Grid::SetColumnSpan(placeholder, Grid::GetColumnSpan(block.frame));
    Grid::SetRowSpan(placeholder, Grid::GetRowSpan(block.frame));

    Grid host;
    // Preserve Auto measurement; a star column can collapse native taskbar items into overflow.
    ColumnDefinition contentColumn;
    contentColumn.Width(GridLength{1, GridUnitType::Auto});
    host.ColumnDefinitions().Append(contentColumn);
    const auto margin = block.frame.Margin();
    // shortcut: preserve the initial layout slot; re-dock/re-float after a major layout change.
    host.Width(std::max(1.0, block.frame.ActualWidth() + margin.Left + margin.Right));
    host.Height(std::max(1.0, block.frame.ActualHeight() + margin.Top + margin.Bottom));

    block.parent = parent;
    block.originalIndex = index;
    block.placeholder = placeholder;
    block.host = host;
    block.popup = popup;
    try {
        parent.Children().SetAt(index, placeholder);
        parent.Children().Append(popup);
        host.Children().Append(block.frame);
        popup.Child(host);
        popup.IsOpen(true);
        block.frame.UpdateLayout();
        if (popup.IsConstrainedToRootBounds()) {
            Wh_Log(L"Windows kept this popup constrained to the original island");
            winrt::throw_hresult(E_NOTIMPL);
        }
        Position(block, {before.left, before.top});
        const RECT after = Bounds(block);
        block.popupWindow = FindPopupWindow(after);
        if (!block.popupWindow) Wh_Log(L"Popup HWND not realized yet");
        Wh_Log(L"Floating %s: %ld,%ld %ldx%ld", block.storageKey,
               after.left, after.top, after.right - after.left, after.bottom - after.top);
        return true;
    } catch (...) {
        Wh_Log(L"Popup setup failed; restoring original frame");
        Dock(block);
        throw;
    }
}

void Save(Block& block) {
    const RECT rect = Bounds(block);
    const SavedPosition saved{1, {rect.left, rect.top}};
    if (!Wh_SetBinaryValue(block.storageKey, &saved, sizeof(saved))) {
        Wh_Log(L"Could not save %s position", block.storageKey);
    }
}

bool LeftButtonDown() {
    const int button = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
    return (GetAsyncKeyState(button) & 0x8000) != 0;
}

void MoveDrag(POINT cursor) {
    if (!g_state || g_state->dragging < 0 ||
        (cursor.x == g_state->lastCursor.x && cursor.y == g_state->lastCursor.y)) {
        return;
    }
    g_state->lastCursor = cursor;
    Position(g_state->blocks[g_state->dragging],
        {g_state->startPosition.x + cursor.x - g_state->startCursor.x,
         g_state->startPosition.y + cursor.y - g_state->startCursor.y});
}

void EndDrag(bool cancel) {
    if (!g_state) return;
    if (g_state->dragging < 0) {
        g_state->dragCandidate = -1;
        g_state->inputWindow = nullptr;
        g_state->pointerInput = false;
        g_state->pointerId = 0;
        return;
    }
    const int index = std::exchange(g_state->dragging, -1);
    KillTimer(g_state->taskbar, kDragTimer);
    auto& block = g_state->blocks[index];
    const HWND captureWindow = std::exchange(g_state->captureWindow, nullptr);
    g_state->pointerInput = false;
    g_state->pointerId = 0;
    if (captureWindow && GetCapture() == captureWindow) ReleaseCapture();
    const HWND foreground = std::exchange(g_state->previousForeground, nullptr);
    if (IsWindow(foreground)) SetForegroundWindow(foreground);
    if (cancel) {
        if (g_state->wasFloating) Position(block, g_state->startPosition);
        else Dock(block);
    } else {
        POINT cursor{};
        if (block.placeholder && GetCursorPos(&cursor)) {
            const RECT slot = Bounds(block.placeholder);
            if (PtInRect(&slot, cursor)) {
                Dock(block);
                Wh_DeleteValue(block.storageKey);
                return;
            }
        }
        Save(block);
    }
}

bool DockAll() {
    bool restored = true;
    try { EndDrag(true); }
    catch (...) {
        if (g_state && GetCapture() == g_state->taskbar) ReleaseCapture();
        Wh_Log(L"Could not cancel last move; continuing with docking");
    }
    if (!g_state) return true;
    for (auto& block : g_state->blocks) {
        try { Dock(block); }
        catch (const winrt::hresult_error& e) {
            restored = false;
            Wh_Log(L"Restore failed for %s: %08X", block.storageKey,
                   static_cast<unsigned>(e.code().value));
        } catch (...) {
            restored = false;
            Wh_Log(L"Restore failed for %s", block.storageKey);
        }
    }
    return restored;
}

void RestoreSavedPositions() {
    if (!g_state || !g_state->root || AutoHide()) return;
    for (auto& block : g_state->blocks) {
        SavedPosition saved;
        if (Wh_GetBinaryValue(block.storageKey, &saved, sizeof(saved)) == sizeof(saved) &&
            saved.version == 1 && std::abs(static_cast<int64_t>(saved.point.x)) <= 1000000 &&
            std::abs(static_cast<int64_t>(saved.point.y)) <= 1000000) {
            try { if (Float(block)) Position(block, saved.point); }
            catch (...) { Dock(block); Wh_Log(L"Saved position could not be restored"); }
        }
    }
}

bool Initialize(HWND hwnd) {
    if (g_state && g_state->root) return true;
    auto root = TaskbarRoot(hwnd);
    HWND bridge = FindBridge(hwnd);
    if (!root || !bridge) return false;
    auto apps = FindFrame(root.Content(), L"Taskbar.TaskbarFrame");
    auto tray = FindFrame(root.Content(), L"SystemTray.SystemTrayFrame");
    if (!apps || !tray || apps.ActualWidth() <= 0 || tray.ActualWidth() <= 0) return false;

    if (!g_state) g_state = new State;
    g_state->taskbar = hwnd;
    g_state->bridge = bridge;
    g_state->root = root;
    g_state->blocks[0].frame = apps;
    g_state->blocks[0].storageKey = L"apps.position";
    g_state->blocks[1].frame = tray;
    g_state->blocks[1].storageKey = L"tray.position";
    if (g_anchorStartMenu) {
        g_state->layoutSource = root.Content().as<FrameworkElement>();
        g_state->layoutToken = g_state->layoutSource.LayoutUpdated(
            [](auto const&, auto const&) { PublishStartAnchor(); });
    }
    Wh_Log(L"Ready: Ctrl+drag apps or tray; release to finish");
    RestoreSavedPositions();
    return true;
}

bool MouseInput(const MSG* msg) {
    if (!g_state || !g_state->root) return false;
    const bool pointer = msg->message == WM_POINTERDOWN ||
                         msg->message == WM_POINTERUPDATE ||
                         msg->message == WM_POINTERUP;
    const UINT32 pointerId = pointer ? GET_POINTERID_WPARAM(msg->wParam) : 0;
    const bool down = msg->message == WM_LBUTTONDOWN ||
                      msg->message == WM_LBUTTONDBLCLK ||
                      msg->message == WM_POINTERDOWN;
    const bool move = msg->message == WM_MOUSEMOVE ||
                      msg->message == WM_POINTERUPDATE;
    const bool up = msg->message == WM_LBUTTONUP ||
                    msg->message == WM_POINTERUP;

    if (g_state->dragCandidate >= 0) {
        if (msg->message == WM_KEYDOWN && msg->wParam == VK_ESCAPE) {
            EndDrag(true);
            return true;
        }
        if (down) {
            EndDrag(false);
        } else if (pointer == g_state->pointerInput &&
                   (!pointer || pointerId == g_state->pointerId)) {
            if (up) {
                EndDrag(false);
                return false;
            }
            if (move) {
                const bool held = pointer
                    ? IS_POINTER_INCONTACT_WPARAM(msg->wParam) &&
                      IS_POINTER_FIRSTBUTTON_WPARAM(msg->wParam)
                    : (msg->wParam & MK_LBUTTON) != 0;
                if (!held) {
                    EndDrag(false);
                    return false;
                }
                POINT cursor{};
                if (pointer) {
                    cursor = {GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam)};
                } else if (!GetCursorPos(&cursor)) {
                    return false;
                }
                if (!Geometry::DragThresholdExceeded(
                        g_state->startCursor, cursor, GetSystemMetrics(SM_CXDRAG),
                        GetSystemMetrics(SM_CYDRAG))) return false;

                const int index = g_state->dragCandidate;
                const HWND clickTarget = g_state->inputWindow;
                const bool wasFloating = static_cast<bool>(g_state->blocks[index].popup);
                const HWND foreground = GetForegroundWindow();
                g_state->dragCandidate = -1;
                g_state->inputWindow = nullptr;
                if (IsWindow(clickTarget)) SendMessageW(clickTarget, WM_CANCELMODE, 0, 0);
                // Opening the popup can cancel capture, so acquire it afterward.
                if (!Float(g_state->blocks[index])) {
                    EndDrag(true);
                    return true;
                }
                SetForegroundWindow(g_state->taskbar);
                SetCapture(g_state->taskbar);
                g_state->captureWindow = GetCapture() == g_state->taskbar
                    ? g_state->taskbar : nullptr;
                g_state->dragging = index;
                g_state->wasFloating = wasFloating;
                g_state->previousForeground = foreground;
                g_state->lastCursor = g_state->startCursor;
                if (!SetTimer(g_state->taskbar, kDragTimer, 16, nullptr)) {
                    EndDrag(true);
                    return true;
                }
                MoveDrag(cursor);
                return true;
            }
            return false;
        } else {
            return false;
        }
    }
    if (g_state->dragging >= 0) {
        if (msg->message == WM_KEYDOWN && msg->wParam == VK_ESCAPE) {
            EndDrag(true);
            return true;
        }
        if (pointer && (!g_state->pointerInput || pointerId != g_state->pointerId)) {
            return false;
        }
        if (!pointer && (move || up || down) &&
            msg->hwnd != g_state->captureWindow) return false;
        if (move) {
            if (!LeftButtonDown()) {
                EndDrag(false);
            } else {
                POINT cursor{};
                if (GetCursorPos(&cursor)) MoveDrag(cursor);
            }
            return true;
        }
        if (up) {
            EndDrag(false);
            return true;
        }
        if (down) return true;
        return false;
    }
    if (!down || !IsWindow(msg->hwnd)) return false;

    POINT cursor{};
    if (pointer) {
        POINTER_INFO info{};
        if (!IS_POINTER_FIRSTBUTTON_WPARAM(msg->wParam) ||
            !GetPointerInfo(pointerId, &info) || info.pointerType != PT_MOUSE ||
            !(info.dwKeyStates & POINTER_MOD_CTRL)) return false;
        cursor = {GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam)};
    } else {
        if (!(msg->wParam & MK_CONTROL) || !(msg->wParam & MK_LBUTTON) ||
            !GetCursorPos(&cursor)) return false;
    }
    const HWND inputRoot = GetAncestor(msg->hwnd, GA_ROOT);
    // The tray is more specific when a theme gives the app frame the full width.
    for (int i = static_cast<int>(g_state->blocks.size()) - 1; i >= 0; --i) {
        auto& block = g_state->blocks[i];
        if (block.popup) {
            block.popupWindow = FindPopupWindow(Bounds(block));
        }
        if (inputRoot != (block.popup ? block.popupWindow : g_state->taskbar)) continue;
        RECT rect = Bounds(block);
        if (!PtInRect(&rect, cursor)) continue;
        // Keep the native click untouched until the drag threshold is crossed.
        g_state->dragCandidate = static_cast<int>(i);
        g_state->inputWindow = msg->hwnd;
        g_state->pointerInput = pointer;
        g_state->pointerId = pointerId;
        g_state->startCursor = cursor;
        g_state->startPosition = {rect.left, rect.top};
        return false;
    }
    return false;
}

using Dispatch_t = decltype(&DispatchMessageW);
Dispatch_t g_dispatch;
LRESULT WINAPI DispatchHook(const MSG* msg) {
    if (!g_unloading && GetCurrentThreadId() == g_uiThread) {
        try { if (MouseInput(msg)) return 0; }
        catch (const winrt::hresult_error& e) {
            Wh_Log(L"Input failed: %08X; docking panels", static_cast<unsigned>(e.code().value));
            DockAll();
            return 0;
        } catch (...) {
            Wh_Log(L"Input failed; docking panels");
            DockAll();
            return 0;
        }
    }
    return g_dispatch(msg);
}

LRESULT CALLBACK TaskbarProc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp,
                             DWORD_PTR) {
    try {
        if (message == g_controlMessage) {
            if (wp == 1) {
                KillTimer(hwnd, kTimer);
                if (g_state) g_state->RevokeLayout();
                ClearStartAnchor(hwnd);
                if (DockAll()) {
                    delete g_state;
                    g_state = nullptr;
                    return 1;
                } else {
                    Wh_Log(L"Incomplete restore; at least one original frame remains in its popup");
                }
                return 0;
            }
            if (!g_unloading) SetTimer(hwnd, kTimer, 500, nullptr);
            return 0;
        }
        if (message == WM_TIMER && wp == kTimer) {
            if (g_unloading) return 0;
            if (!g_state) { g_state = new State; g_state->taskbar = hwnd; }
            const unsigned retries = ++g_state->retries;
            bool initialized = false;
            try { initialized = Initialize(hwnd); }
            catch (...) { Wh_Log(L"Taskbar initialization attempt failed"); }
            if (initialized || retries >= 20) {
                KillTimer(hwnd, kTimer);
                if (!g_state->root) Wh_Log(L"Native taskbar not ready/unsupported; no changes made");
            }
            return 0;
        }
        // XAML can retake capture; track until physical release or Escape.
        if (message == WM_TIMER && wp == kDragTimer) {
            if (!g_state || g_state->dragging < 0) {
                KillTimer(hwnd, kDragTimer);
            } else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                EndDrag(true);
            } else if (!LeftButtonDown()) {
                EndDrag(false);
            } else {
                POINT cursor{};
                if (GetCursorPos(&cursor)) MoveDrag(cursor);
            }
            return 0;
        }
        if (message == WM_DISPLAYCHANGE || message == WM_DPICHANGED) {
            if (DockAll()) { delete g_state; g_state = nullptr; }
            if (!g_unloading) SetTimer(hwnd, kTimer, 500, nullptr);
        }
        if (message == WM_SETTINGCHANGE) {
            if (AutoHide()) DockAll();
            else if (!g_state ||
                     (g_state->dragging < 0 && g_state->dragCandidate < 0)) {
                RestoreSavedPositions();
            }
        }
        if (message == WM_NCDESTROY) {
            KillTimer(hwnd, kTimer);
            if (g_state) g_state->RevokeLayout();
            ClearStartAnchor(hwnd);
            const bool restored = DockAll();
            State* oldState = std::exchange(g_state, nullptr);
            if (restored) delete oldState;
            g_taskbar = nullptr;
            g_uiThread = 0;
        }
    } catch (...) {
        Wh_Log(L"Taskbar callback failed; cancelling floating layout");
        DockAll();
    }
    return DefSubclassProc(hwnd, message, wp, lp);
}

void Attach() {
    if (g_unloading) return;
    HWND hwnd = FindTaskbar();
    if (!hwnd) {
        Wh_Log(L"Taskbar window not found; waiting for StartTaskbar");
        return;
    }
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, TaskbarProc, 0)) {
        Wh_Log(L"Could not subclass the running taskbar window");
        return;
    }
    g_taskbar = hwnd;
    g_uiThread = GetWindowThreadProcessId(hwnd, nullptr);
    SendMessageW(hwnd, g_controlMessage, 0, 0);
}

using Start_t = void(WINAPI*)(void*);
Start_t g_start;
void WINAPI StartHook(void* self) {
    g_start(self);
    Attach();
}

namespace StartMenu {
std::atomic<HWND> g_window{nullptr};
struct State {
    HWND hwnd = nullptr;
    Window window{nullptr};
    FrameworkElement content{nullptr};
    FrameworkElement frame{nullptr};
    Media::Transform originalTransform{nullptr};
    winrt::Windows::Foundation::IInspectable originalLocalValue{nullptr};
    Media::TransformGroup transform{nullptr};
    Media::TranslateTransform offset{nullptr};
    winrt::event_token visibilityToken{};
    winrt::event_token layoutToken{};
    winrt::event_token closedToken{};
    bool updating = false;
    bool bindingLogged = false;
};
State* g_state = nullptr;

HWND FindCoreWindow() {
    HWND result = nullptr;
    EnumWindows([](HWND hwnd, LPARAM param) -> BOOL {
        DWORD processId = 0;
        GetWindowThreadProcessId(hwnd, &processId);
        WCHAR name[64]{};
        if (processId == GetCurrentProcessId() &&
            GetClassNameW(hwnd, name, ARRAYSIZE(name)) &&
            wcscmp(name, L"Windows.UI.Core.CoreWindow") == 0) {
            *reinterpret_cast<HWND*>(param) = hwnd;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&result));
    return result;
}

bool ReadAnchor(RECT* rect) {
    const HWND taskbar = ::FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!taskbar) return false;
    const HANDLE sequence = GetPropW(taskbar, kAnchorProperties[4]);
    if (!sequence) return false;
    LONG* values[]{&rect->left, &rect->top, &rect->right, &rect->bottom};
    for (size_t i = 0; i < ARRAYSIZE(values); ++i) {
        const INT_PTR value = reinterpret_cast<INT_PTR>(GetPropW(taskbar, kAnchorProperties[i]));
        if (value < -1000000 || value > 1000000) return false;
        *values[i] = static_cast<LONG>(value);
    }
    return GetPropW(taskbar, kAnchorProperties[4]) == sequence &&
           rect->right > rect->left && rect->bottom > rect->top &&
           rect->right - rect->left <= 100000 && rect->bottom - rect->top <= 100000;
}

void Restore() {
    if (!g_state) return;
    if (g_state->frame && g_state->transform && g_state->frame.RenderTransform() == g_state->transform) {
        if (g_state->originalLocalValue == DependencyProperty::UnsetValue()) {
            g_state->frame.ClearValue(UIElement::RenderTransformProperty());
        } else {
            g_state->frame.SetValue(UIElement::RenderTransformProperty(), g_state->originalLocalValue);
        }
    }
    g_state->frame = nullptr;
    g_state->transform = nullptr;
    g_state->originalTransform = nullptr;
    g_state->originalLocalValue = nullptr;
    g_state->offset = nullptr;
}

bool ApplyTransform(FrameworkElement frame) {
    if (!g_state || !frame) return false;
    if (frame.GetBindingExpression(UIElement::RenderTransformProperty())) {
        Restore();
        if (!g_state->bindingLogged) {
            Wh_Log(L"Start menu RenderTransform has a binding; anchoring skipped");
            g_state->bindingLogged = true;
        }
        return false;
    }
    if (frame == g_state->frame && frame.RenderTransform() == g_state->transform) return true;
    Restore();
    g_state->frame = frame;
    g_state->originalLocalValue = frame.ReadLocalValue(UIElement::RenderTransformProperty());
    g_state->originalTransform = frame.RenderTransform();
    g_state->offset = Media::TranslateTransform();
    g_state->transform = Media::TransformGroup();
    if (g_state->originalTransform) g_state->transform.Children().Append(g_state->originalTransform);
    g_state->transform.Children().Append(g_state->offset);
    frame.RenderTransform(g_state->transform);
    return true;
}

void Update();

void UpdateImpl() {
    if (g_unloading || !g_state || !g_state->window) return;
    auto content = g_state->window.Content().try_as<FrameworkElement>();
    if (!content) { Restore(); return; }
    if (g_state->content != content) {
        if (g_state->content && g_state->layoutToken.value) g_state->content.LayoutUpdated(g_state->layoutToken);
        g_state->content = content;
        g_state->layoutToken = content.LayoutUpdated([](auto const&, auto const&) { Update(); });
    }
    RECT button{};
    if (!ReadAnchor(&button)) { Restore(); return; }
    if (!g_state->window.Visible()) return;
    const HMONITOR monitor = MonitorFromRect(&button, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info{sizeof(info)};
    if (MonitorFromWindow(g_state->hwnd, MONITOR_DEFAULTTONEAREST) != monitor ||
        !GetMonitorInfoW(monitor, &info)) { Restore(); return; }

    FrameworkElement frame{nullptr};
    const auto className = winrt::get_class_name(content);
    if (className == L"Windows.UI.Xaml.Controls.Canvas") {
        frame = FindFrame(content, L"StartDocked.StartSizingFrame");
    } else if (className == L"StartMenu.StartBlendedFlexFrame") {
        const int count = Media::VisualTreeHelper::GetChildrenCount(content);
        for (int i = 0; i < count; ++i) {
            auto child = Media::VisualTreeHelper::GetChild(content, i).try_as<FrameworkElement>();
            if (child && child.Name() == L"FrameRoot") { frame = child; break; }
        }
    }
    if (!frame) { Restore(); return; }
    if (!ApplyTransform(frame)) return;

    const auto root = content.XamlRoot();
    const double scale = root ? root.RasterizationScale() : 0;
    const auto bounds = frame.TransformToVisual(content).TransformBounds(
        Rect{0, 0, static_cast<float>(frame.ActualWidth()),
                   static_cast<float>(frame.ActualHeight())});
    POINT origin{};
    if (!std::isfinite(scale) || scale <= 0 ||
        !ClientToScreen(g_state->hwnd, &origin) ||
        !std::isfinite(bounds.X) || !std::isfinite(bounds.Y) ||
        !std::isfinite(bounds.Width) || !std::isfinite(bounds.Height) ||
        !std::isfinite(bounds.X * scale) || !std::isfinite(bounds.Y * scale) ||
        std::abs(bounds.X * scale) > 1000000 || std::abs(bounds.Y * scale) > 1000000 ||
        bounds.Width <= 0 || bounds.Height <= 0 ||
        bounds.Width * scale > 100000 || bounds.Height * scale > 100000) {
        Restore();
        return;
    }
    const RECT current{
        origin.x + static_cast<LONG>(std::floor(bounds.X * scale)),
        origin.y + static_cast<LONG>(std::floor(bounds.Y * scale)),
        origin.x + static_cast<LONG>(std::ceil((bounds.X + bounds.Width) * scale)),
        origin.y + static_cast<LONG>(std::ceil((bounds.Y + bounds.Height) * scale))};
    const auto target = Geometry::AnchorStartMenu(button, current.right - current.left,
        current.bottom - current.top, info.rcWork,
        std::max<LONG>(1, std::lround(std::min(100000.0, 8 * scale))));
    if (current.left != target.x) g_state->offset.X(g_state->offset.X() + (target.x - current.left) / scale);
    if (current.top != target.y) g_state->offset.Y(g_state->offset.Y() + (target.y - current.top) / scale);
}

void Update() {
    if (!g_state || g_state->updating) return;
    State* state = g_state;
    state->updating = true;
    try { UpdateImpl(); }
    catch (...) {
        Wh_Log(L"Start menu anchor unavailable; keeping native placement");
        try { Restore(); } catch (...) {}
    }
    if (g_state == state) state->updating = false;
}

void Uninit();

void Init() {
    if (g_unloading || g_state) return;
    try {
        auto window = Window::Current();
        if (!window) return;
        const HWND hwnd = FindCoreWindow();
        if (!hwnd || GetWindowThreadProcessId(hwnd, nullptr) != GetCurrentThreadId()) return;
        g_state = new State;
        g_state->hwnd = hwnd;
        g_window = hwnd;
        g_state->window = window;
        g_state->visibilityToken = window.VisibilityChanged(
            [](auto const&, winrt::Windows::UI::Core::VisibilityChangedEventArgs const& args) {
                if (args.Visible()) Update();
            });
        g_state->closedToken = window.Closed([](auto const&, auto const&) { Uninit(); });
        Update();
    } catch (...) {
        Uninit();
        Wh_Log(L"Start menu anchor initialization failed");
    }
}

void Uninit() {
    if (!g_state) return;
    try {
        if (g_state->content && g_state->layoutToken.value) g_state->content.LayoutUpdated(g_state->layoutToken);
    } catch (...) { g_callbackCleanupFailed = true; }
    try {
        if (g_state->window && g_state->visibilityToken.value) g_state->window.VisibilityChanged(g_state->visibilityToken);
    } catch (...) { g_callbackCleanupFailed = true; }
    try {
        if (g_state->window && g_state->closedToken.value) g_state->window.Closed(g_state->closedToken);
    } catch (...) { g_callbackCleanupFailed = true; }
    g_state->layoutToken = {};
    g_state->visibilityToken = {};
    g_state->closedToken = {};
    try { Restore(); } catch (...) {}
    delete std::exchange(g_state, nullptr);
    g_window = nullptr;
}

bool RunFromWindowThread(HWND hwnd, void (*proc)()) {
    static const UINT message = RegisterWindowMessageW(L"Windhawk.FreeFloat.StartThread." WH_MOD_ID);
    DWORD processId = 0;
    const DWORD threadId = GetWindowThreadProcessId(hwnd, &processId);
    if (!message || !threadId || processId != GetCurrentProcessId()) return false;
    if (threadId == GetCurrentThreadId()) { proc(); return true; }
    struct Call { void (*proc)(); bool completed = false; } call{proc};
    static std::atomic<Call*> pending{nullptr};
    const HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC,
        [](int code, WPARAM wp, LPARAM lp) -> LRESULT {
            if (code == HC_ACTION) {
                const auto* data = reinterpret_cast<const CWPSTRUCT*>(lp);
                auto* call = pending.load();
                if (call && data->message == message &&
                    data->lParam == reinterpret_cast<LPARAM>(call)) {
                    call->proc();
                    call->completed = true;
                }
            }
            return CallNextHookEx(nullptr, code, wp, lp);
        }, nullptr, threadId);
    if (!hook) return false;
    pending = &call;
    SendMessageW(hwnd, message, 0, reinterpret_cast<LPARAM>(&call));
    pending = nullptr;
    UnhookWindowsHookEx(hook);
    return call.completed;
}

using Activation_t = decltype(&RoGetActivationFactory);
Activation_t g_activation;
HRESULT WINAPI ActivationHook(HSTRING id, REFIID iid, void** factory) {
    thread_local bool inHook = false;
    if (!g_unloading && !inHook &&
        wcscmp(WindowsGetStringRawBuffer(id, nullptr), L"Windows.UI.Xaml.Hosting.XamlIsland") == 0) {
        inHook = true;
        Init();
        inHook = false;
    }
    return g_activation(id, iid, factory);
}
}

BOOL Wh_ModInit() {
    g_anchorStartMenu = Wh_GetIntSetting(L"anchorStartMenu") != 0;
    WCHAR path[MAX_PATH]{};
    const DWORD pathLength = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (!pathLength || pathLength >= ARRAYSIZE(path)) return FALSE;
    PCWSTR name = wcsrchr(path, L'\\');
    g_isStartHost = name && _wcsicmp(name + 1, L"StartMenuExperienceHost.exe") == 0;
    if (g_isStartHost) {
        return g_anchorStartMenu && WindhawkUtils::SetFunctionHook(
            RoGetActivationFactory, StartMenu::ActivationHook, &StartMenu::g_activation);
    }
    HMODULE taskbar = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!taskbar) return FALSE;
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"}, &g_taskBandVtable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"}, &g_getHost},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"}, &g_frameHeight},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"}, &g_decref},
        {{LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"}, &g_start, StartHook},
    };
    if (!WindhawkUtils::HookSymbols(taskbar, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks)) ||
        !Geometry::HostOffsetSize() ||
        !Readable(g_frameHeight, Geometry::HostOffsetSize()) ||
        !Geometry::HostOffset(static_cast<const BYTE*>(g_frameHeight))) {
        Wh_Log(L"Unsupported taskbar symbols/ABI; refusing to load");
        return FALSE;
    }
    g_controlMessage = RegisterWindowMessageW(L"Windhawk.FreeFloat." WH_MOD_ID);
    if (!g_controlMessage || !WindhawkUtils::SetFunctionHook(
            DispatchMessageW, DispatchHook, &g_dispatch)) return FALSE;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_isStartHost) {
        if (HWND hwnd = StartMenu::FindCoreWindow()) StartMenu::RunFromWindowThread(hwnd, StartMenu::Init);
    } else {
        Attach();
    }
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    if (g_isStartHost) {
        if (HWND hwnd = StartMenu::g_window) {
            if (!StartMenu::RunFromWindowThread(hwnd, StartMenu::Uninit)) {
                g_callbackCleanupFailed = true;
            }
        }
        KeepFailedCallbacksMapped();
        return;
    }
    const HWND hwnd = g_taskbar;
    if (hwnd && IsWindow(hwnd)) {
        if (!SendMessageW(hwnd, g_controlMessage, 1, 0)) {
            Wh_Log(L"Disable could not restore every taskbar frame; restart Explorer to recover");
        }
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, TaskbarProc);
    }
    g_taskbar = nullptr;
    g_uiThread = 0;
    KeepFailedCallbacksMapped();
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = TRUE;
    return TRUE;
}
