// ==WindhawkMod==
// @id              taskbar-free-float
// @name            Taskbar Free Float (experimental)
// @description     Ctrl+drag the original Windows 11 app bar and tray/clock independently
// @version         0.1.1
// @author          Rikyz
// @github          https://github.com/RikyZ90
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lcomctl32
// @license         GPL-3.0-only
// ==/WindhawkMod==

// Taskbar host discovery adapted from m417z's GPL-3.0 taskbar-tray-show-on-hover.
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-tray-show-on-hover.wh.cpp

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

## Installation and removal

The mod attempts to attach to a running Explorer process and restore the native
frames when disabled. If the log reports an incomplete restore, restart Explorer
manually after saving any file operations. The mod never restarts Explorer.

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

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <limits>
#include <utility>

#include <windhawk_utils.h>
#include <shellapi.h>
#include <windowsx.h>

#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
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
std::atomic<HWND> g_taskbar{nullptr};
std::atomic<DWORD> g_uiThread{0};
UINT g_controlMessage;
constexpr UINT_PTR kTimer = 0x4652464C;

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
    Geometry::Position startPosition{};
    HWND previousForeground = nullptr;
    unsigned retries = 0;
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

bool AutoHide() {
    APPBARDATA data{sizeof(data)};
    return (SHAppBarMessage(ABM_GETSTATE, &data) & ABS_AUTOHIDE) != 0;
}

void Dock(Block& block) {
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

void EndDrag(bool cancel) {
    if (!g_state) return;
    if (g_state->dragging < 0) {
        if (g_state->dragCandidate < 0) return;
        g_state->dragCandidate = -1;
        g_state->inputWindow = nullptr;
        g_state->pointerInput = false;
        g_state->pointerId = 0;
        return;
    }
    const int index = std::exchange(g_state->dragging, -1);
    auto& block = g_state->blocks[index];
    const HWND captureWindow = std::exchange(g_state->captureWindow, nullptr);
    g_state->pointerInput = false;
    g_state->pointerId = 0;
    if (GetCapture() == captureWindow) ReleaseCapture();
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
                SetForegroundWindow(g_state->taskbar);
                SetCapture(g_state->taskbar);
                if (GetCapture() != g_state->taskbar) {
                    g_state->pointerInput = false;
                    g_state->pointerId = 0;
                    if (IsWindow(foreground)) SetForegroundWindow(foreground);
                    Wh_Log(L"Mouse capture refused; drag cancelled");
                    return true;
                }
                g_state->captureWindow = g_state->taskbar;
                g_state->dragging = index;
                g_state->wasFloating = wasFloating;
                g_state->previousForeground = foreground;
                if (!Float(g_state->blocks[index])) {
                    EndDrag(true);
                    return true;
                }
                Position(g_state->blocks[index],
                    {g_state->startPosition.x + cursor.x - g_state->startCursor.x,
                     g_state->startPosition.y + cursor.y - g_state->startCursor.y});
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
            const bool held = pointer
                ? IS_POINTER_INCONTACT_WPARAM(msg->wParam) &&
                  IS_POINTER_FIRSTBUTTON_WPARAM(msg->wParam)
                : (msg->wParam & MK_LBUTTON) != 0;
            if (!held) {
                EndDrag(false);
            } else {
                POINT cursor{};
                if (pointer) {
                    cursor = {GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam)};
                } else {
                    if (!GetCursorPos(&cursor)) return true;
                }
                Position(g_state->blocks[g_state->dragging],
                    {g_state->startPosition.x + cursor.x - g_state->startCursor.x,
                     g_state->startPosition.y + cursor.y - g_state->startCursor.y});
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
        if ((message == WM_CAPTURECHANGED || message == WM_CANCELMODE) &&
            g_state && g_state->dragging >= 0 &&
            (message == WM_CANCELMODE || reinterpret_cast<HWND>(lp) != hwnd)) {
            EndDrag(true);
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

BOOL Wh_ModInit() {
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

void Wh_ModAfterInit() { Attach(); }

void Wh_ModBeforeUninit() {
    g_unloading = true;
    const HWND hwnd = g_taskbar;
    if (hwnd && IsWindow(hwnd)) {
        if (!SendMessageW(hwnd, g_controlMessage, 1, 0)) {
            Wh_Log(L"Disable could not restore every taskbar frame; restart Explorer to recover");
        }
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, TaskbarProc);
    }
    g_taskbar = nullptr;
    g_uiThread = 0;
}
