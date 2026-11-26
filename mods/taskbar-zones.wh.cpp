// ==WindhawkMod==
// @id              taskbar-zones
// @name            Taskbar Zones
// @description     Organize Windows 11 taskbar apps into fixed left and right zones. Drag icons between sides and keep their positions when apps open or close.
// @version         1.0.3
// @author          Luiz Bizzio
// @github          https://github.com/luizbizzio
// @homepage        https://github.com/luizbizzio/taskbar-zones
// @donateUrl       https://github.com/sponsors/luizbizzio
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject
// @license         GPL-3.0
// ==/WindhawkMod==

// Copyright (c) 2026 Arkadiusz (original Taskbar Split)
// Modifications (c) 2026 Luiz Bizzio: fixed zones, cross-zone drag, multi-monitor support.
// Original: https://github.com/Artllex/taskbar-split @ 6920019b64f6ce1c64ace3ee0b5d4c5897958413
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License version 3 as published
// by the Free Software Foundation.
// This program is distributed WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See https://www.gnu.org/licenses/gpl-3.0.html for the full license.
//
// GPL-3.0 taskbar discovery, system-button identification and running-state
// techniques adapted from Michael Maltsev (m417z): taskbar-labels and
// taskbar-start-button-position; and Taskbar Start Button Centered Origin
// by rick/rycalvo. Sources:
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-labels.wh.cpp
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-start-button-position.wh.cpp
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-centered-start-split-icons.wh.cpp
// Pointer-handler ABI and symbol names follow m417z's GPL-3.0 mod:
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-reorder-right-drag.wh.cpp
// Jump List button anchoring adapted from m417z's Multirow taskbar (GPL-3.0):
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-multirow.wh.cpp
//
// Taskbar-host discovery also uses MIT-licensed code from Taskbar multi-tray
// by EDM115 and Island Media Controls by usho. The following MIT notice is
// retained for those portions, not as the license of this combined mod.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

// ==WindhawkModReadme==
/*
# Taskbar Zones

![Taskbar Zones](https://raw.githubusercontent.com/luizbizzio/taskbar-zones/main/assets/taskbar-zones-cover.png)

Keep your apps on the **left or right side** of the Windows 11 taskbar. Each app stays on the side you choose, even when you open or close it.

## Features

- **Two adaptive zones:** Choose the left or right side for each app. Icons use the available taskbar space.
- **Easy drag and drop:** Move apps between zones or change their order.
- **Icon spacing:** Adjust the space between app icons separately for each zone.
- **Ungrouped windows:** Windows of one app stay next to each other and move together, including with **Disable grouping on the taskbar**.
- **Saved layout:** Keep your app positions after restarting Explorer or your PC.
- **Custom layout:** Adjust the gap before the notification area.
- **Multiple monitors:** Each compatible taskbar calculates its own usable area.
- **Widget-aware layout:** Automatically detect Taskbar System Info and keep app icons clear of supported taskbar widgets without moving the widgets themselves.

## How to use

1. Set your Windows 11 taskbar alignment to **Left**.
2. Drag an app icon across the middle gap to move it to the other side.
3. Drag icons within the same zone to reorder them.
4. Open the mod settings to adjust the layout.

## Optional widget reservations

If a widget needs its own space, you can add its exact XAML `Name` under **Additional reserved widget names (advanced)**. Separate names with commas or semicolons (up to 12). You can leave this setting empty.

Taskbar System Info is detected automatically, so you do not need to add its name.

A named widget must be visible inside the taskbar's `RootGrid`, outside the app list and system tray. Its width must be between 8 DIP and the smaller of 650 DIP or 60% of the taskbar width. It must fit before the tray. Do not add names of icon decorations or overlays.

## Notes

- Your Windows pinned apps are not changed.
- Do not enable **Taskbar Split** together with this mod. Both mods reposition the same taskbar buttons.
- Other tools that move the same taskbar buttons or reserve their space may conflict with this mod.
- If the taskbar is too crowded to fit both zones, Windows' normal layout is used until space is available again.
- Negative icon spacing narrows the buttons. At large values the right edge of the hover highlight or of a badge can be cut off.
- Windows of the same app keep the order Windows gives them. They cannot be reordered among themselves or placed on different sides.
- Secondary monitors show only the app buttons that Windows puts on each taskbar.
- Windows updates may affect compatibility.

## Credits

**Taskbar Zones** is maintained by **Luiz Bizzio**. It is a separate, modified version of [Taskbar Split by Arkadiusz (Artllex)](https://github.com/Artllex/taskbar-split), with saved app zones, cross-zone dragging, and multi-monitor support.

Licensed under **GPL-3.0**. Full source attribution and copyright notices are retained in the code.
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- trayGap: 8
  $name: Gap before tray
  $description: Space between right-side apps and the notification area or a reserved widget. Negative values are treated as 0.
- leftZoneIconSpacing: 0
  $name: Left zone icon spacing adjustment
  $description: Extra space between app icons on the left, in pixels at 100% scaling. 0 keeps the Windows size. Negative values make the buttons, and their clickable areas, narrower. The reduction stops before it would cut the icon (about -8 with default icons) and does not apply to buttons with labels. Accepted range: -24 to 48.
- rightZoneIconSpacing: 0
  $name: Right zone icon spacing adjustment
  $description: Same as above, for app icons on the right. Accepted range: -24 to 48.
- sectionDragging: true
  $name: Enable dragging between zones
  $description: Drag an app to the other side of the gap, or reorder within a side. Disable to keep native Windows dragging.
- systemButtonsLeft: true
  $name: Keep system buttons on the left
  $description: Put Start, Search, Widgets and Task View at the left edge. Recommended for the intended split layout.
- extraReservedWidgets: ""
  $name: Additional reserved widget names (advanced)
  $description: Up to 12 comma- or semicolon-separated XAML Name values. Widgets must be visible in RootGrid, outside the app list and tray, 8 to 650 DIP wide (max 60% of taskbar width), and inside the area before the tray. Do not list icon overlays. Leave empty if unsure.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <functional>
#include <optional>
#include <cmath>
#include <iterator>
#include <mutex>
#include <cstdint>
#include <cwchar>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <commctrl.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Devices.Input.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

#define WH_WINRT_WINUI2
#include <winrt/Microsoft.UI.Xaml.Controls.h>

using namespace winrt::Windows::UI::Xaml;
namespace media = winrt::Windows::UI::Xaml::Media;
namespace numerics = winrt::Windows::Foundation::Numerics;

struct Settings {
    std::atomic<int> trayGap{8};
    std::atomic<int> leftIconSpacing{0};
    std::atomic<int> rightIconSpacing{0};
    std::atomic<bool> systemButtonsLeft{true};
    std::atomic<bool> sectionDragging{true};
};

Settings g_settings;
// A small, explicit list avoids mistaking compositor-positioned icon badges
// for taskbar reservations. Users may opt in other widgets by XAML Name.
std::mutex g_reservedWidgetNamesMutex;
std::vector<std::wstring> g_reservedWidgetNames;
std::atomic<bool> g_unloading{false};
std::atomic<bool> g_settingsArrangeFollowup{false};
std::atomic<bool> g_viewHooksInstalled{false};
std::atomic<bool> g_viewHooksInProgress{false};
std::atomic<HMODULE> g_viewHooksFailedModule{nullptr};
std::mutex g_windowsMutex;
std::unordered_set<HWND> g_subclassedTaskbars;
std::unordered_set<HWND> g_refreshPending;
thread_local HWND g_activeTaskbarWindow = nullptr;
thread_local bool g_insideArrange = false;
std::atomic<unsigned long long> g_configEpoch{1};
struct CachedLayoutOwner {
    winrt::weak_ref<winrt::Microsoft::UI::Xaml::Controls::Layout> layout;
    HWND window = nullptr;
};
[[clang::no_destroy]] thread_local std::unordered_map<void*, CachedLayoutOwner>
    g_layoutOwners;


void LoadSettings() {
    g_settings.sectionDragging = Wh_GetIntSetting(L"sectionDragging") != 0;
    g_settings.trayGap = std::max(0, Wh_GetIntSetting(L"trayGap"));
    g_settings.leftIconSpacing = std::clamp(
        static_cast<int>(Wh_GetIntSetting(L"leftZoneIconSpacing")), -24, 48);
    g_settings.rightIconSpacing = std::clamp(
        static_cast<int>(Wh_GetIntSetting(L"rightZoneIconSpacing")), -24, 48);
    g_settings.systemButtonsLeft =
        Wh_GetIntSetting(L"systemButtonsLeft") != 0;

    // The RAII helper releases the Windhawk setting string after copying.
    std::wstring names =
        WindhawkUtils::StringSetting::make(L"extraReservedWidgets").get();
    std::vector<std::wstring> parsed;
    size_t start = 0;
    while (start < names.size() && parsed.size() < 12) {
        size_t end = names.find_first_of(L",;", start);
        if (end == std::wstring::npos) end = names.size();
        auto name = names.substr(start, end - start);
        auto first = name.find_first_not_of(L" \t\r\n");
        if (first != std::wstring::npos) {
            auto last = name.find_last_not_of(L" \t\r\n");
            name = name.substr(first, last - first + 1);
            if (name.size() <= 100 && name != L"BlobShape" &&
                name != L"WindhawkDotStack" &&
                name != L"WindhawkTaskbarSystemInfo" &&
                std::find(parsed.begin(), parsed.end(), name) == parsed.end())
                parsed.push_back(std::move(name));
        }
        start = end + 1;
    }
    std::lock_guard<std::mutex> lock(g_reservedWidgetNamesMutex);
    g_reservedWidgetNames.swap(parsed);
}

FrameworkElement FindDirectChild(
    FrameworkElement const& parent,
    std::function<bool(FrameworkElement const&)> const& predicate) {
    if (!parent) {
        return nullptr;
    }

    int count = media::VisualTreeHelper::GetChildrenCount(parent);
    for (int index = 0; index < count; ++index) {
        auto child = media::VisualTreeHelper::GetChild(parent, index)
                         .try_as<FrameworkElement>();
        if (child && predicate(child)) {
            return child;
        }
    }
    return nullptr;
}

FrameworkElement FindDirectChildByName(FrameworkElement const& parent,
                                        wchar_t const* name) {
    return FindDirectChild(parent, [name](FrameworkElement const& child) {
        return child.Name() == name;
    });
}

FrameworkElement FindDirectChildByClass(FrameworkElement const& parent,
                                         wchar_t const* className) {
    return FindDirectChild(parent, [className](FrameworkElement const& child) {
        return winrt::get_class_name(child) == className;
    });
}

std::vector<FrameworkElement> RepeaterElements(FrameworkElement const& value) {
    std::vector<FrameworkElement> elements;
    auto repeater =
        value.try_as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>();
    if (!repeater) {
        return elements;
    }

    auto source = repeater.ItemsSourceView();
    int count = source ? source.Count() : 0;
    elements.reserve(count);
    for (int index = 0; index < count; ++index) {
        auto item = repeater.TryGetElement(index);
        auto element = item ? item.try_as<FrameworkElement>() : nullptr;
        if (element) {
            elements.push_back(element);
        }
    }
    return elements;
}

std::atomic<bool> g_secondaryUnavailableLogged{false};
void* CTaskBand_ITaskListWndSite_vftable = nullptr;
void* CSecondaryTaskBand_ITaskListWndSite_vftable = nullptr;
using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void*, void**);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;
CTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original = nullptr;
void* TaskbarHost_FrameHeight_Original = nullptr;
using RefCount_Decref_t = void(WINAPI*)(void*);
RefCount_Decref_t RefCount_Decref_Original = nullptr;

struct SharedPtrGuard {
    void* controlBlock;
    ~SharedPtrGuard() {
        if (controlBlock && RefCount_Decref_Original) {
            RefCount_Decref_Original(controlBlock);
        }
    }
};

// Keep startup/rebuild diagnostics useful without logging every arrange pass.
void LogDiscoveryFailure(unsigned slot, wchar_t const* reason, HWND window) {
    static thread_local ULONGLONG last[12]{};
    ULONGLONG now = GetTickCount64();
    if (slot >= ARRAYSIZE(last)) return;
    if (last[slot] && now - last[slot] < 10000) return;
    last[slot] = now;
    Wh_Log(L"Taskbar discovery: %ls (HWND %p)", reason, window);
}

XamlRoot TaskbarXamlRoot(HWND taskbarWindow) {
    wchar_t className[64]{};
    GetClassNameW(taskbarWindow, className, ARRAYSIZE(className));
    bool secondary = wcscmp(className, L"Shell_SecondaryTrayWnd") == 0;
    if (secondary && (!CSecondaryTaskBand_ITaskListWndSite_vftable ||
                      !CSecondaryTaskBand_GetTaskbarHost_Original)) {
        if (!g_secondaryUnavailableLogged.exchange(true))
            Wh_Log(L"Secondary taskbar unsupported: CSecondaryTaskBand symbols unavailable");
        return nullptr;
    }
    HWND taskbandWindow = secondary
        ? FindWindowExW(taskbarWindow, nullptr, L"WorkerW", nullptr)
        : reinterpret_cast<HWND>(GetPropW(taskbarWindow, L"TaskbandHWND"));
    if (!taskbandWindow) {
        LogDiscoveryFailure(0, L"taskband window not ready", taskbarWindow);
        return nullptr;
    }

    auto taskBand = reinterpret_cast<void*>(GetWindowLongPtrW(taskbandWindow, 0));
    if (!taskBand) {
        LogDiscoveryFailure(1, L"taskband object not ready", taskbarWindow);
        return nullptr;
    }

    void* site = taskBand;
    void* expectedSite = secondary ? CSecondaryTaskBand_ITaskListWndSite_vftable
                                   : CTaskBand_ITaskListWndSite_vftable;
    bool found = false;
    for (int slot = 0; slot < 20; ++slot) {
        if (*reinterpret_cast<void**>(site) == expectedSite) {
            found = true;
            break;
        }
        site = reinterpret_cast<void**>(site) + 1;
    }
    if (!found) {
        LogDiscoveryFailure(2, L"taskband site not found", taskbarWindow);
        return nullptr;
    }

    void* hostSharedPtr[2]{};
    (secondary ? CSecondaryTaskBand_GetTaskbarHost_Original
               : CTaskBand_GetTaskbarHost_Original)(site, hostSharedPtr);
    SharedPtrGuard release{hostSharedPtr[1]};
    if (!hostSharedPtr[0]) {
        LogDiscoveryFailure(3, L"taskbar host not ready", taskbarWindow);
        return nullptr;
    }

    size_t elementOffset = 0;
#if defined(_M_X64) || defined(__x86_64__)
    auto bytes = reinterpret_cast<BYTE const*>(TaskbarHost_FrameHeight_Original);
    if (bytes[0] == 0x48 && bytes[1] == 0x83 && bytes[2] == 0xEC &&
        bytes[4] == 0x48 && bytes[5] == 0x83 && bytes[6] == 0xC1 &&
        bytes[7] <= 0x7F) {
        elementOffset = bytes[7];
    } else {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight implementation");
        return nullptr;
    }
#elif defined(_M_ARM64) || defined(__aarch64__)
    // ARM64 host discovery from the MIT-licensed Taskbar multi-tray.
    const DWORD* instructions =
        static_cast<const DWORD*>(TaskbarHost_FrameHeight_Original);
    if (instructions[0] == 0xD503237F &&
        (instructions[1] & 0xFFC07FFF) == 0xA9807BFD &&
        instructions[2] == 0x910003FD &&
        (instructions[3] & 0xFFF00FE0) == 0xF8400C00) {
        elementOffset = (instructions[3] >> 12) & 0xFF;
    } else {
        Wh_Log(L"Unsupported ARM64 TaskbarHost::FrameHeight implementation");
        return nullptr;
    }
#else
    return nullptr;
#endif

    auto unknown = *reinterpret_cast<IUnknown**>(
        reinterpret_cast<BYTE*>(hostSharedPtr[0]) + elementOffset);
    if (!unknown) {
        LogDiscoveryFailure(4, L"hosted XAML element not ready", taskbarWindow);
        return nullptr;
    }

    FrameworkElement hostedElement{nullptr};
    unknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                            winrt::put_abi(hostedElement));
    auto root = hostedElement ? hostedElement.XamlRoot() : nullptr;
    if (!root)
        LogDiscoveryFailure(4, L"XamlRoot not ready", taskbarWindow);
    return root;
}

// This XAML state stays on the owning taskbar UI thread. Keep only the weak
// repeater reference; reconstruct its frame and grid from the current ancestry.
[[clang::no_destroy]] thread_local std::unordered_map<HWND,
    winrt::weak_ref<FrameworkElement>> g_repeaterByWindow;

struct TaskbarTree {
    FrameworkElement frame{nullptr};
    FrameworkElement grid{nullptr};
    FrameworkElement repeater{nullptr};
};

// Validate the real visual ancestry, not just names or a shared XamlRoot.
// This also rejects detached/recycled repeaters and another monitor's tree.
TaskbarTree ValidateTaskbarRepeater(FrameworkElement const& root,
                                   FrameworkElement const& element) {
    if (!root || !element || element.Name() != L"TaskbarFrameRepeater" ||
        !element.try_as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>())
        return {};
    auto xamlRoot = root.XamlRoot();
    if (!xamlRoot || element.XamlRoot() != xamlRoot) return {};

    TaskbarTree tree;
    auto node = element.as<DependencyObject>();
    for (unsigned depth = 0; node && depth <= 16; ++depth) {
        if (auto fe = node.try_as<FrameworkElement>()) {
            if (fe.Visibility() != Visibility::Visible) return {};
            if (node != element && !tree.frame) {
                if (winrt::get_class_name(fe) == L"Taskbar.TaskbarFrame") {
                    if (!tree.grid) return {};
                    tree.frame = fe;
                } else if (!tree.grid && fe.Name() == L"RootGrid" &&
                           fe.try_as<Controls::Grid>()) {
                    tree.grid = fe;
                }
            }
        }
        if (node == root) {
            if (!tree.frame || !tree.grid) return {};
            tree.repeater = element;
            return tree;
        }
        node = media::VisualTreeHelper::GetParent(node);
    }
    return {};
}

TaskbarTree FindTaskbarTree(FrameworkElement const& root, HWND window = nullptr) {
    if (!root) return {};
    // Fast path for the original Windows hierarchy, also when the content
    // itself is the frame. Do not broaden ordinary widget name searches.
    auto frame = winrt::get_class_name(root) == L"Taskbar.TaskbarFrame"
        ? root : FindDirectChildByClass(root, L"Taskbar.TaskbarFrame");
    auto grid = frame ? FindDirectChildByName(frame, L"RootGrid") : nullptr;
    auto direct = grid ? FindDirectChildByName(grid, L"TaskbarFrameRepeater")
                       : nullptr;
    if (auto tree = ValidateTaskbarRepeater(root, direct); tree.repeater) {
        if (window) {
            auto& cached = g_repeaterByWindow[window];
            if (cached.get() != tree.repeater) cached = tree.repeater;
        }
        return tree;
    }

    // Only reuse a cached repeater if its current ancestry still proves it
    // belongs to this taskbar. No weak reference is shared between UI threads.
    if (window) {
        auto cached = g_repeaterByWindow.find(window);
        if (cached != g_repeaterByWindow.end()) {
            auto tree = ValidateTaskbarRepeater(root, cached->second.get());
            if (tree.repeater) return tree;
            g_repeaterByWindow.erase(cached);
        }
    }

    // New Windows builds may insert ScrollViewer/Border or other containers.
    // Bounded breadth-first traversal, including non-FrameworkElement nodes.
    // Never descend into app repeaters, their templates, or the system tray.
    constexpr size_t kMaxNodes = 512;
    constexpr unsigned kMaxDepth = 16;
    struct PendingNode { DependencyObject node; unsigned depth; };
    std::vector<PendingNode> pending;
    pending.push_back({root.as<DependencyObject>(), 0});
    TaskbarTree match;
    for (size_t i = 0; i < pending.size(); ++i) {
        // Copy before push_back: vector growth can invalidate references.
        auto current = pending[i];
        if (auto fe = current.node.try_as<FrameworkElement>()) {
            if (fe.Visibility() != Visibility::Visible) continue;
            auto name = fe.Name();
            auto className = winrt::get_class_name(fe);
            if (name == L"SystemTrayFrame" ||
                className == L"SystemTray.SystemTrayFrame" ||
                className == L"Taskbar.TaskListButton") continue;
            if (fe.try_as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>()) {
                auto tree = ValidateTaskbarRepeater(root, fe);
                if (tree.repeater) {
                    if (match.repeater && match.repeater != tree.repeater) {
                        LogDiscoveryFailure(5, L"ambiguous TaskbarFrameRepeater", window);
                        return {};
                    }
                    match = tree;
                }
                continue;
            }
        }
        if (current.depth >= kMaxDepth) continue;
        int count = media::VisualTreeHelper::GetChildrenCount(current.node);
        for (int j = 0; j < count; ++j) {
            if (pending.size() >= kMaxNodes) {
                LogDiscoveryFailure(5, L"XAML discovery node limit reached", window);
                return {};
            }
            auto child = media::VisualTreeHelper::GetChild(current.node, j);
            if (child) pending.push_back({child, current.depth + 1});
        }
    }
    if (!match.repeater) {
        LogDiscoveryFailure(5, L"TaskbarFrame/RootGrid/ItemsRepeater not found", window);
        return {};
    }
    if (window) g_repeaterByWindow[window] = match.repeater;
    static std::atomic<bool> loggedNestedTree{false};
    if (!loggedNestedTree.exchange(true))
        Wh_Log(L"Taskbar discovery: nested TaskbarFrame/RootGrid/ItemsRepeater found");
    return match;
}

FrameworkElement FindTaskbarRepeater(FrameworkElement const& root,
                                     HWND window) {
    return FindTaskbarTree(root, window).repeater;
}
struct TaskbarUiState;
TaskbarUiState& Ui();

struct ScopedTaskbarContext {
    HWND previous;
    explicit ScopedTaskbarContext(HWND value) : previous(g_activeTaskbarWindow) {
        g_activeTaskbarWindow = value;
    }
    ~ScopedTaskbarContext() { g_activeTaskbarWindow = previous; }
};

FrameworkElement GetTaskbarRepeater() {
    HWND window = g_activeTaskbarWindow;
    if (!window || GetWindowThreadProcessId(window, nullptr) != GetCurrentThreadId())
        return nullptr;
    // Cache taskbar XAML references independently on each owning UI thread.
    // Revalidate the repeater against the active XamlRoot. A detached but
    // alive repeater must not remain in the cache after a monitor reconnect.
    auto root = TaskbarXamlRoot(window);
    auto content = root ? root.Content().try_as<FrameworkElement>() : nullptr;
    if (!content) {
        g_repeaterByWindow.erase(window);
        return nullptr;
    }
    return FindTaskbarTree(content, window).repeater;
}


// Prefer the full native hit area. On some taskbar rebuilds ActualWidth is
// temporarily inflated even though BOTH the Windows layout slot and desired
// width still agree on a normal-sized button. Only then use the slot width;
// intentional wide-label buttons keep their real width/hit target.
double ElementWidth(FrameworkElement const& element) {
    if (element.Visibility() != Visibility::Visible) return 0;
    Thickness margin = element.Margin();
    double actualWidth = element.ActualWidth();
    // XAML DesiredSize and LayoutSlot already contain the margins. Only
    // ActualWidth describes the border box and needs margins added.
    double outerWidth = std::isfinite(actualWidth) && actualWidth > 0
        ? actualWidth + margin.Left + margin.Right
        : element.DesiredSize().Width;
    if (!std::isfinite(outerWidth) || outerWidth <= 0) return 0;

    if (winrt::get_class_name(element) == L"Taskbar.TaskListButton") {
        try {
            double slotWidth = Controls::Primitives::LayoutInformation::
                GetLayoutSlot(element).Width;
            double desired = element.DesiredSize().Width;
            if (std::isfinite(slotWidth) && std::isfinite(desired) &&
                slotWidth >= 20 && desired >= 20 &&
                std::abs(slotWidth - desired) <= 8 &&
                outerWidth > slotWidth + 24 &&
                outerWidth > slotWidth * 1.5) {
                return slotWidth;
            }
        } catch (...) { /* Native width remains the safe default. */ }
    }
    return outerWidth;
}

// Content DesiredSize avoids the inflated ActualWidth caused by the shell's
// negative-margin collapse. Adapted from m417z's GetClusterButtonWidth (GPL).
double SystemButtonWidth(FrameworkElement const& element) {
    if (element.Visibility() != Visibility::Visible) {
        return 0;
    }
    double width = element.ActualWidth();
    if (media::VisualTreeHelper::GetChildrenCount(element) > 0) {
        auto child = media::VisualTreeHelper::GetChild(element, 0)
                         .try_as<FrameworkElement>();
        if (child) {
            width = child.DesiredSize().Width;
        }
    }
    auto margin = element.Margin();
    return std::max(0.0, width + margin.Left + margin.Right);
}

double ElementX(FrameworkElement const& element,
                FrameworkElement const& relativeTo) {
    return element.TransformToVisual(relativeTo).TransformPoint(
        winrt::Windows::Foundation::Point{0, 0}).X;
}

enum class SystemButtonKind { None, Start, Widgets, Search, TaskView };

SystemButtonKind GetSystemButtonKind(FrameworkElement const& element) {
    auto className = winrt::get_class_name(element);
    if (className == L"Taskbar.ExperienceToggleButton") {
        auto id = Automation::AutomationProperties::GetAutomationId(element);
        if (id == L"StartButton") {
            return SystemButtonKind::Start;
        }
        if (id == L"TaskViewButton") {
            return SystemButtonKind::TaskView;
        }
    } else if (className == L"Taskbar.AugmentedEntryPointButton" &&
               element.Name() == L"AugmentedEntryPointButton") {
        return SystemButtonKind::Widgets;
    } else if (className == L"Taskbar.TaskbarExtensionElement") {
        return SystemButtonKind::Search;
    }
    return SystemButtonKind::None;
}

thread_local winrt::weak_ref<FrameworkElement> g_widgetsCache;
thread_local winrt::weak_ref<FrameworkElement> g_widgetsFrame;
thread_local HWND g_widgetsWindow = nullptr;
thread_local ULONGLONG g_widgetsNextSearch = 0;
struct NamedElementCacheEntry {
    winrt::weak_ref<FrameworkElement> element;
    winrt::weak_ref<FrameworkElement> content;
    ULONGLONG nextLookup = 0;
};
[[clang::no_destroy]] thread_local std::unordered_map<HWND,
    NamedElementCacheEntry> g_traySearch;

std::atomic<bool> g_widgetsInvalidated{true};

FrameworkElement FindWidgets(FrameworkElement const& node, int depth = 0) {
    if (!node || depth > 16) return nullptr;
    if (GetSystemButtonKind(node) == SystemButtonKind::Widgets) return node;
    int count = media::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i) {
        auto child = media::VisualTreeHelper::GetChild(node, i).try_as<FrameworkElement>();
        if (auto found = FindWidgets(child, depth + 1)) return found;
    }
    return nullptr;
}

void ReserveNativeWidgets(FrameworkElement const& content,
                          double& leftEdge, double& rightEdge) {
    auto frame = FindTaskbarTree(content, g_activeTaskbarWindow).frame;
    if (!frame) return;
    if (g_widgetsWindow != g_activeTaskbarWindow ||
        g_widgetsInvalidated.exchange(false) || g_widgetsFrame.get() != frame) {
        g_widgetsWindow = g_activeTaskbarWindow;
        g_widgetsCache = nullptr;
        g_widgetsFrame = frame;
        g_widgetsNextSearch = 0;
    }
    auto widget = g_widgetsCache.get();
    if (widget) {
        // A detached but still live object must not reserve its former space.
        auto parent = widget.as<DependencyObject>();
        bool attached = false;
        bool visible = true;
        for (int depth = 0; parent && depth <= 16; ++depth) {
            if (auto ancestor = parent.try_as<FrameworkElement>()) {
                if (ancestor.Visibility() != Visibility::Visible) visible = false;
            }
            if (parent == frame) { attached = true; break; }
            parent = media::VisualTreeHelper::GetParent(parent);
        }
        if (!attached) { widget = nullptr; g_widgetsCache = nullptr; g_widgetsNextSearch = 0; }
        else if (!visible) return;
    }
    if (!widget) {
        auto now = GetTickCount64();
        if (now < g_widgetsNextSearch) return;
        g_widgetsNextSearch = now + 1000; // Also cache an unsuccessful lookup.
        widget = FindWidgets(frame);
        g_widgetsCache = widget;
    }
    if (!widget || widget.Visibility() != Visibility::Visible) return;
    double width = SystemButtonWidth(widget);
    if (width <= 0) return;
    double x = ElementX(widget, content);
    if (x + width / 2 >= content.ActualWidth() / 2)
        rightEdge = std::min(rightEdge, x);
    else
        leftEdge = std::max(leftEdge, x + width);
}

bool IsTaskButton(FrameworkElement const& element) {
    return winrt::get_class_name(element) == L"Taskbar.TaskListButton";
}

struct ButtonInfo {
    FrameworkElement element;
    double width;  // arrange slot width
    bool leftSide;
    std::wstring appId;
    double narrowedWidth = 0;  // equals width when narrowed, otherwise 0
    double nativeWidth = 0;
};

// Per-taskbar UI references must never be reused on another monitor.
struct TaskbarSlot {
    std::wstring appId;
    bool left = true;
    double x = 0;
    double width = 0;
};
// Last non-empty AppID seen on a live task button container.
struct RememberedAppId {
    winrt::weak_ref<FrameworkElement> element;
    std::wstring id;
    ULONGLONG seen = 0;
};
// A button this mod arranges narrower than its native width. Depending on the
// button template, ActualWidth may then report either width.
struct NarrowedButton {
    winrt::weak_ref<FrameworkElement> element;
    double nativeWidth = 0;
    double slotWidth = 0;
};
struct TaskbarUiState {
    std::unordered_map<void*, NarrowedButton> narrowed;
    // Arrange slot width of every app button in the latest plan.
    std::unordered_map<void*, double> slotWidths;
    double leftGap = 0, rightGap = 0;
    // App identity tracking. Session only; none of this is ever persisted.
    std::unordered_map<void*, RememberedAppId> rememberedIds;
    // Raw IDs used by the latest plan, so an in-place change is noticed.
    std::vector<std::pair<winrt::weak_ref<FrameworkElement>, std::wstring>>
        plannedIds;
    std::vector<std::wstring> nativeIds;  // task buttons in Windows' order
    unsigned long long geometryEpoch = 1;
    bool geometryInitialized = false;
    double lastContentWidth = 0, lastContentHeight = 0;
    double lastRepeaterX = 0, lastLeftBound = 0, lastRightBound = 0;
    double lastTrayX = 0;
    double lastLeftGroupWidth = 0, lastRightGroupWidth = 0;
    double lastDpi = 0;
    size_t lastButtonCount = 0;
    uint64_t lastButtonIdentity = 0;
    ULONGLONG lastCrowdingLog = 0;
    ULONGLONG lastCrowdingRetry = 0;
    ULONGLONG lastPostArrangeErrorLog = 0;
    double lastSystemStart = 0, lastSystemWidth = 0;
    std::vector<winrt::weak_ref<FrameworkElement>> leftLive;
    std::vector<winrt::weak_ref<FrameworkElement>> rightLive;
    std::vector<std::wstring> leftAppOrder;
    std::vector<std::wstring> rightAppOrder;
    std::vector<std::wstring> rightAppIds;
    std::vector<TaskbarSlot> slots;
    bool layoutValid = false;
    double leftStart = 0, leftEnd = 0, rightStart = 0, rightEnd = 0;
    unsigned long long stateRevision = 0;
};
[[clang::no_destroy]] thread_local std::unordered_map<HWND, TaskbarUiState> g_uiStates;
TaskbarUiState& Ui() { return g_uiStates[g_activeTaskbarWindow]; }

// The taskbar's usable region is discovered from known or explicitly named
// widgets. Never write coordinates or margins owned by another mod.
void RequestRefreshFor(HWND window);
bool IsTaskbarWindow(HWND window);
bool AppIdentityChanged(TaskbarUiState const& ui);
double TrayEdgeForContent(FrameworkElement const& content,
                          FrameworkElement const& relativeTo);
FrameworkElement FindNamedDescendant(FrameworkElement const& node,
                                     wchar_t const* name, int depth);

struct HorizontalSpan {
    double left = 0;
    double right = 0;
};
std::vector<HorizontalSpan> CollectExternalSpans(FrameworkElement const& content);
std::vector<HorizontalSpan> EffectiveExternalSpans(FrameworkElement const& content);
bool ResolveAvailableZoneBounds(double leftWidth, double rightWidth,
                                std::vector<HorizontalSpan> const& obstacles,
                                double& leftEdge, double& rightEdge,
                                double leftClearance, double rightClearance);

struct BoundaryWatch {
    winrt::weak_ref<FrameworkElement> content;
    bool observed = false;
    bool pollTimerArmed = false;
    UINT pollIntervalMs = 1000;
    double trayX = 0;
    double lastGoodContentWidth = 0;
    double lastScanContentWidth = 0;
    ULONGLONG lastGoodTick = 0;
    ULONGLONG lastScanTick = 0;
    std::vector<HorizontalSpan> spans;
    // Remember a widget's right-side slot when it is beside the tray. A
    // native tray expansion must not let two independently-positioning mods
    // chase each other by vacating and reclaiming that slot.
    bool rightDockTracked = false;
    double rightDockWidth = 0;
    double rightDockTrailingGap = 0;
    double rightDockObservedTray = 0;
    ULONGLONG rightDockHoldUntil = 0;
    // Do not publish another mod's intermediate placement during a tray
    // animation. Each taskbar owns one candidate and one accepted snapshot.
    bool pending = false;
    double pendingTrayX = 0;
    ULONGLONG pendingSince = 0;
    std::vector<HorizontalSpan> pendingSpans;
    // Weak XAML references are confined to their owning taskbar UI thread.
    std::unordered_map<std::wstring, winrt::weak_ref<FrameworkElement>> namedWidgets;
    ULONGLONG lastBoundaryLog = 0;
};
[[clang::no_destroy]] thread_local std::unordered_map<HWND, BoundaryWatch>
    g_boundaryWatches;

UINT_PTR BoundaryPollTimerId() {
    static int timerTag;
    return reinterpret_cast<UINT_PTR>(&timerTag);
}

void RemoveBoundaryWatch(HWND window) {
    auto it = g_boundaryWatches.find(window);
    if (it == g_boundaryWatches.end()) return;
    if (it->second.pollTimerArmed) KillTimer(window, BoundaryPollTimerId());
    g_boundaryWatches.erase(it);
}

// When an existing widget is rebuilt, keep its last measured space for at
// most 1.2 seconds. The same rule is used by both the poller and the planner.
std::vector<HorizontalSpan> RetainTransientSpans(
        FrameworkElement const& content, BoundaryWatch const& watch,
        std::vector<HorizontalSpan> spans, ULONGLONG now) {
    if (spans.empty() && watch.content.get() == content &&
        !watch.spans.empty() && watch.lastGoodTick &&
        now - watch.lastGoodTick < 1200 &&
        std::abs(watch.lastGoodContentWidth - content.ActualWidth()) < 1)
        return watch.spans;
    return spans;
}

// All geometry comparisons use the same tolerance. Transient layout passes
// and text rounding can move a measured edge by less than one DIP.
bool BoundaryGeometryChanged(double trayA,
                             std::vector<HorizontalSpan> const& spansA,
                             double trayB,
                             std::vector<HorizontalSpan> const& spansB) {
    auto differs = [](double a, double b) {
        return !std::isfinite(a) || !std::isfinite(b) ||
               std::abs(a - b) > 1.0;
    };
    if (differs(trayA, trayB) || spansA.size() != spansB.size()) return true;
    for (size_t i = 0; i < spansA.size(); ++i) {
        if (differs(spansA[i].left, spansB[i].left) ||
            differs(spansA[i].right, spansB[i].right)) return true;
    }
    return false;
}

// A widget that formerly forced a zone inward may have moved away. In that
// case we need one refresh to return the group to its natural boundary, even
// though the new widget position does not overlap the current group.
bool WasConstrainingZone(TaskbarUiState const& ui,
                        std::vector<HorizontalSpan> const& oldSpans) {
    if (!ui.layoutValid) return false;
    constexpr double kClearance = 6.0;
    constexpr double kTolerance = 1.5;
    for (auto const& span : oldSpans) {
        if (ui.lastRightGroupWidth > 0 &&
            std::abs(ui.lastRightBound - (span.left -
                std::max(kClearance, double(g_settings.trayGap.load())))) <= kTolerance)
            return true;
        if (ui.lastLeftGroupWidth > 0 &&
            std::abs(ui.lastLeftBound - (span.right + kClearance)) <= kTolerance)
            return true;
        if (ui.lastSystemWidth > 0 &&
            std::abs(ui.lastSystemStart - (span.right + kClearance)) <= kTolerance)
            return true;
    }
    return false;
}

// Only invalidate an in-progress drag when a changed obstacle actually
// collides with the frozen app or system-button rectangles. A live readout
// changing inside the middle gap should not cancel a drag.
bool BoundaryChangeBlocksLayout(TaskbarUiState const& ui, double trayX,
                                std::vector<HorizontalSpan> const& spans) {
    if (!ui.layoutValid || !ui.geometryInitialized) return false;
    double trayGap = std::max(0.0, double(g_settings.trayGap.load()));
    // A growing native tray must shift the right zone, while a shrinking tray
    // must also allow the icons to return. Comparing only the current right
    // bound would miss this second case.
    if (!std::isfinite(trayX) ||
        std::abs(ui.lastTrayX - trayX) > 1.0 ||
        ui.lastRightBound > trayX - trayGap + 0.5) return true;

    double left = ui.lastLeftBound, right = ui.lastRightBound;
    if (!ResolveAvailableZoneBounds(ui.lastLeftGroupWidth,
            ui.lastRightGroupWidth, spans, left, right, 6.0,
            std::max(6.0, trayGap))) return true;
    if (std::abs(left - ui.lastLeftBound) > 0.5 ||
        std::abs(right - ui.lastRightBound) > 0.5) return true;

    if (ui.lastSystemWidth > 0) {
        double systemStart = ui.lastSystemStart;
        double dummyRight = ui.lastRightBound;
        if (!ResolveAvailableZoneBounds(ui.lastSystemWidth, 0, spans,
                systemStart, dummyRight, 6.0, 6.0)) return true;
        if (std::abs(systemStart - ui.lastSystemStart) > 0.5) return true;
    }
    return false;
}

// A timer attached to each taskbar owner samples independently. One tray
// notification can make Windows reflow the tray and another mod remeasure its
// widget. Ignore intermediate widget positions until two samples agree.
void PollBoundaryGeometry(HWND window) {
    if (g_unloading || !IsTaskbarWindow(window) ||
        GetWindowThreadProcessId(window, nullptr) != GetCurrentThreadId()) return;
    auto it = g_boundaryWatches.find(window);
    if (it == g_boundaryWatches.end()) return;
    auto content = it->second.content.get();
    if (!content) return;
    try {
        auto xamlRoot = content.XamlRoot();
        if (!xamlRoot || xamlRoot.Content().try_as<FrameworkElement>() != content) return;
        if (auto state = g_uiStates.find(window);
            state != g_uiStates.end() && AppIdentityChanged(state->second))
            RequestRefreshFor(window);
        double trayX = TrayEdgeForContent(content, content);
        if (!std::isfinite(trayX) || trayX <= 0 ||
            !std::isfinite(content.ActualWidth()) || content.ActualWidth() <= 0)
            return;
        auto& watch = it->second;
        auto spans = CollectExternalSpans(content);
        ULONGLONG now = GetTickCount64();
        if (!spans.empty()) {
            watch.lastGoodTick = now;
            watch.lastGoodContentWidth = content.ActualWidth();
        }
        spans = RetainTransientSpans(content, watch, std::move(spans), now);
        watch.lastScanTick = now;
        watch.lastScanContentWidth = content.ActualWidth();
        auto& ui = g_uiStates[window];

        bool changed = !watch.observed || BoundaryGeometryChanged(
            trayX, spans, watch.trayX, watch.spans);
        if (!changed) {
            watch.pending = false;
            watch.pendingSpans.clear();
            if (!ui.layoutValid && now - ui.lastCrowdingRetry > 1000) {
                ui.lastCrowdingRetry = now;
                RequestRefreshFor(window);
            }
        } else {
            // The initial sample is safe to accept immediately: no older
            // geometry exists. Later changes must persist across two polls.
            if (watch.observed) {
                if (!watch.pending || BoundaryGeometryChanged(trayX, spans,
                        watch.pendingTrayX, watch.pendingSpans)) {
                    watch.pending = true;
                    watch.pendingTrayX = trayX;
                    watch.pendingSpans = std::move(spans);
                    watch.pendingSince = now;
                } else if (now - watch.pendingSince >= 180) {
                    watch.pending = false;
                    spans = std::move(watch.pendingSpans);
                    watch.pendingSpans.clear();
                }
            }
            if (!watch.pending) {
                bool affectsLayout = !watch.observed || !ui.layoutValid ||
                    BoundaryChangeBlocksLayout(ui, trayX, spans) ||
                    WasConstrainingZone(ui, watch.spans);
                if (!watch.lastBoundaryLog || now - watch.lastBoundaryLog > 30000) {
                    watch.lastBoundaryLog = now;
                    double firstReserved = spans.empty() ? -1.0 : spans.front().left;
                    Wh_Log(L"Taskbar %p: tray X %.1f DIP, reserved %zu, first X %.1f DIP",
                           window, trayX, spans.size(), firstReserved);
                }
                watch.observed = true;
                watch.trayX = trayX;
                watch.spans = std::move(spans);
                // A moving widget in the empty middle gap does not affect
                // either app zone; never trigger a no-op arrange in that case.
                if (affectsLayout) {
                    ++ui.geometryEpoch;
                    RequestRefreshFor(window);
                }
            }
        }

        UINT interval = watch.pending ? 200 :
            ((!watch.spans.empty() || !ui.layoutValid) ? 400 : 1600);
        if (watch.pollTimerArmed && watch.pollIntervalMs != interval &&
            SetTimer(window, BoundaryPollTimerId(), interval, nullptr))
            watch.pollIntervalMs = interval;
    } catch (...) {
        // A taskbar XAML tree may disappear during monitor reconnects.
    }
}

void EnsureBoundaryWatch(FrameworkElement const& content) {
    HWND window = g_activeTaskbarWindow;
    if (!window || !content) return;
    auto found = g_boundaryWatches.find(window);
    if (found != g_boundaryWatches.end() &&
        found->second.content.get() == content && found->second.pollTimerArmed)
        return;
    RemoveBoundaryWatch(window);
    auto& state = g_boundaryWatches[window];
    state.content = content;
    state.pollTimerArmed = SetTimer(window, BoundaryPollTimerId(), 400, nullptr) != 0;
    if (!state.pollTimerArmed)
        Wh_Log(L"Could not schedule geometry watcher for taskbar %p", window);
    PollBoundaryGeometry(window);
}

struct PersistedAppState {
    std::vector<std::wstring> leftOrder;
    std::vector<std::wstring> rightOrder;
    std::vector<std::wstring> rightIds;
};
std::mutex g_storeMutex;
PersistedAppState g_sharedState;
bool g_sharedLoaded = false;
unsigned long long g_sharedRevision = 1;
std::wstring g_committedRecord;
std::wstring g_committedBankManifest;
wchar_t g_activeStorageBank = 0;


std::wstring ButtonAppId(FrameworkElement const& element) {
    // Taskbar exposes "Appid: <AUMID or executable identity>". Read every
    // time: a recycled XAML container may now represent a different app.
    auto id = Automation::AutomationProperties::GetAutomationId(element);
    std::wstring value(id.c_str(), id.size());
    constexpr wchar_t prefix[] = L"Appid: ";
    if (value.rfind(prefix, 0) != 0 || value.size() <= 7 || value.size() > 1031)
        return {}; // Never persist captions, HWNDs or container addresses.
    return value.substr(7);
}

bool IsFixedRight(std::wstring const& id) {
    return !id.empty() && std::find(Ui().rightAppIds.begin(),
        Ui().rightAppIds.end(), id) != Ui().rightAppIds.end();
}

// [grouping ids]
// "Disable grouping on the taskbar" (taskbar-grouping by m417z, GPL-3.0) gives
// each extra window its own AppID: the original one plus "~Wh~", a type letter
// (w, c or p) and 8 uppercase hex digits. Only that exact marker is removed;
// AppIDs are never matched by executable name or window title.
std::wstring StripGroupingSuffix(std::wstring const& id) {
    constexpr size_t kSuffixLength = 13;
    if (id.size() <= kSuffixLength) return id;
    size_t start = id.size() - kSuffixLength;
    if (id.compare(start, 4, L"~Wh~") != 0) return id;
    wchar_t type = id[start + 4];
    if (type != L'w' && type != L'c' && type != L'p') return id;
    for (size_t i = start + 5; i < id.size(); ++i) {
        if (!((id[i] >= L'0' && id[i] <= L'9') ||
              (id[i] >= L'A' && id[i] <= L'F'))) return id;
    }
    return id.substr(0, start);
}
// [end grouping ids]

// [app identity]
// XAML can briefly report no AppID while a button is being updated. Keep the
// same container's previous ID for a short time instead of treating it as an
// unknown (left-zone) app. Containers are recycled, so this must expire.
constexpr ULONGLONG kEmptyAppIdGraceMs = 3000;

std::wstring CurrentAppId(FrameworkElement const& element) {
    auto id = ButtonAppId(element);
    if (!id.empty()) return id;
    auto& remembered = Ui().rememberedIds;
    auto found = remembered.find(winrt::get_abi(element));
    if (found != remembered.end() &&
        found->second.element.get() == element &&
        GetTickCount64() - found->second.seen < kEmptyAppIdGraceMs)
        return found->second.id;
    return {};
}

// The identity used for zones, saved order and dragging. Windows of one app
// share it, so they stay adjacent and move between zones together.
std::wstring ButtonKey(FrameworkElement const& element) {
    return StripGroupingSuffix(CurrentAppId(element));
}

bool ContainsId(std::vector<std::wstring> const& ids, std::wstring const& id) {
    return std::find(ids.begin(), ids.end(), id) != ids.end();
}
// [end app identity]

bool ButtonIsLeftSide(FrameworkElement const& element) {
    return !IsFixedRight(ButtonKey(element));
}

// Traverse the taskbar XAML tree to find the native tray, even when its
// structure changes. Widget positions are handled separately below.
FrameworkElement FindNamedDescendant(FrameworkElement const& node,
                                     wchar_t const* name, int depth = 0) {
    if (!node || depth > 12) return nullptr;
    if (node.Name() == name ||
        (wcscmp(name, L"SystemTrayFrame") == 0 &&
         winrt::get_class_name(node) == L"SystemTray.SystemTrayFrame"))
        return node;
    int count = media::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i) {
        auto child = media::VisualTreeHelper::GetChild(node, i).try_as<FrameworkElement>();
        if (auto found = FindNamedDescendant(child, name, depth + 1)) return found;
    }
    return nullptr;
}

FrameworkElement CachedNamedDescendant(FrameworkElement const& content,
                                       wchar_t const* name,
                                       std::unordered_map<HWND, NamedElementCacheEntry>& cache) {
    if (!content || !g_activeTaskbarWindow) return nullptr;
    auto& entry = cache[g_activeTaskbarWindow];
    if (entry.content.get() != content) {
        entry.content = content;
        entry.element = nullptr;
        entry.nextLookup = 0;
    }
    if (auto found = entry.element.get()) {
        // A widget can detach/reparent without being destructed, or remain
        // hidden while another instance is inserted for the active display.
        if (found.Visibility() == Visibility::Visible) {
            auto ancestor = found.as<DependencyObject>();
            for (unsigned d = 0; ancestor && d < 24; ++d) {
                if (ancestor == content) return found;
                ancestor = media::VisualTreeHelper::GetParent(ancestor);
            }
        }
        entry.element = nullptr;
        entry.nextLookup = 0;
    }
    ULONGLONG now = GetTickCount64();
    if (now < entry.nextLookup) return nullptr;
    entry.nextLookup = now + 750;
    auto found = FindNamedDescendant(content, name);
    if (found) entry.element = found;
    return found;
}

FrameworkElement FindNativeTrayFrame(FrameworkElement const& content) {
    auto tray = CachedNamedDescendant(content, L"SystemTrayFrame", g_traySearch);
    if (!tray) tray = FindDirectChildByClass(content, L"SystemTray.SystemTrayFrame");
    return tray;
}

// Recent taskbar layouts may put the tray inside a repeater item. Never
// treat the item containing the native tray as an app or system button.
FrameworkElement TrayHostInRepeater(FrameworkElement const& tray,
                                     FrameworkElement const& repeater) {
    if (!tray || !repeater || tray.XamlRoot() != repeater.XamlRoot()) return nullptr;
    auto node = tray.as<DependencyObject>();
    for (unsigned depth = 0; node && depth < 24; ++depth) {
        auto parent = media::VisualTreeHelper::GetParent(node);
        if (parent == repeater) return node.try_as<FrameworkElement>();
        if (!parent || node == repeater) break;
        node = parent;
    }
    return nullptr;
}

// A real right-docked Win32 notification window can independently corroborate
// a transient XAML coordinate. Never mix screen pixels and XAML DIPs unless
// the taskbar client width agrees with the XAML root's width and scale.
double NativeTrayEdge(FrameworkElement const& content) {
    HWND window = g_activeTaskbarWindow;
    if (!window) return 0;
    HWND trayWindow = FindWindowExW(window, nullptr, L"TrayNotifyWnd", nullptr);
    if (!trayWindow || !IsWindowVisible(trayWindow)) return 0;
    RECT client{}, trayRect{};
    POINT origin{};
    if (!GetClientRect(window, &client) || !ClientToScreen(window, &origin) ||
        !GetWindowRect(trayWindow, &trayRect)) return 0;
    auto root = content.XamlRoot();
    double scale = root ? root.RasterizationScale() : 0.0;
    double width = content.ActualWidth();
    if (!std::isfinite(scale) || scale <= 0 || !std::isfinite(width) ||
        std::abs((client.right - client.left) / scale - width) > 4.0)
        return 0;
    double left = (trayRect.left - origin.x) / scale;
    double right = (trayRect.right - origin.x) / scale;
    if (trayRect.top >= origin.y + client.bottom || trayRect.bottom <= origin.y ||
        !std::isfinite(left) || left <= 0 || right <= left ||
        std::abs(right - width) > 32.0) return 0;
    return left;
}

// Both the poller and the planner must use the same validated boundary.
// Do not accept a small X coordinate for a tray which is not right-docked:
// that transient measurement used to trigger native/custom layout oscillation.
double TrayEdgeForContent(FrameworkElement const& content,
                          FrameworkElement const& relativeTo) {
    double contentLeft = ElementX(content, relativeTo);
    double width = content.ActualWidth();
    double edge = contentLeft + width;
    if (!std::isfinite(edge) || !std::isfinite(width) || width <= 0) return 0;
    auto tray = FindNativeTrayFrame(content);
    if (tray && tray.Visibility() == Visibility::Visible) {
        double tx = ElementX(tray, content);
        double tw = tray.ActualWidth();
        // A large tray is legitimate if its right edge is still at the
        // right side. A small tray temporarily near the left is not.
        bool finite = std::isfinite(tx) && std::isfinite(tw) && tw > 0;
        bool rightDocked = finite && tx > 0 && tx <= width &&
            (tx >= width / 2.0 || std::abs(tx + tw - width) <= 48.0);
        if (rightDocked) return contentLeft + tx;
        double native = NativeTrayEdge(content);
        if (native > 0) return contentLeft + native;
        static thread_local ULONGLONG lastInvalidTrayLog = 0;
        auto now = GetTickCount64();
        if (!lastInvalidTrayLog || now - lastInvalidTrayLog >= 60000) {
            lastInvalidTrayLog = now;
            Wh_Log(L"TZ TRAY unverified geometry: x=%.1f width=%.1f rootWidth=%.1f; using native layout if no verified edge",
                   tx, tw, width);
        }
        // A missing trustworthy boundary must not be interpreted as free
        // space: that would allow apps to overlap a real notification area.
        return 0;
    }
    if (double native = NativeTrayEdge(content); native > 0)
        return contentLeft + native;
    // Secondary taskbars may legitimately have no notification area.
    if (g_activeTaskbarWindow &&
        g_activeTaskbarWindow == FindWindowW(L"Shell_TrayWnd", nullptr))
        return 0;
    return edge;
}

// Only explicitly identified widgets are eligible to reserve space. The
// previous generic tree walk was unsafe: other taskbar mods create overlays
// whose XAML rectangle stays at the frame origin while a Composition transform
// renders them on top of app buttons. Those are NOT layout obstacles.
//
// System Info is auto-detected. It has its OWN placement solver and can sit
// anywhere in the taskbar: never pretend that its measured width occupies
// a slot directly before the tray. Read its actual XAML rectangle after the
// other mod applies its Margin.Left, then avoid that rectangle. Invalid or
// out-of-bounds measurements are ignored, never converted into a fabricated
// reservation. The per-taskbar poller remeasures when placement changes.
// Additional widgets can be configured by exact XAML Name.
bool IsUsableReservedWidget(FrameworkElement const& element,
                            FrameworkElement const& root, double maxWidth,
                            double maxHeight) {
    if (!element || element.Visibility() != Visibility::Visible ||
        element.Opacity() < 0.05) return false;
    double width = element.ActualWidth();
    double height = element.ActualHeight();
    if (!std::isfinite(width) || width < 8 || width > maxWidth ||
        !std::isfinite(height) || height < 8 || height > maxHeight * 1.5)
        return false;
    // Never accept an overlay that is part of the app list or system tray,
    // including when it happens to carry a user-configured element name.
    auto parent = element.as<DependencyObject>();
    for (int depth = 0; parent && depth < 24; ++depth) {
        if (parent == root) return true;
        if (auto fe = parent.try_as<FrameworkElement>()) {
            auto name = fe.Name();
            if (name == L"TaskbarFrameRepeater" ||
                name == L"SystemTrayFrame" ||
                name == L"BlobShape" || name == L"WindhawkDotStack" ||
                winrt::get_class_name(fe) == L"SystemTray.SystemTrayFrame")
                return false;
        }
        parent = media::VisualTreeHelper::GetParent(parent);
    }
    return false;
}

// An advanced widget name must never resolve inside a task button template
// or the system tray. Also reject decorative overlays which have no reserved
// taskbar space. Keep searching after an unusable name match: another valid
// element with the same Name may be inserted later in RootGrid.
FrameworkElement FindReservedWidgetByName(
        FrameworkElement const& node, FrameworkElement const& root,
        std::wstring const& requested, double maxWidth, double maxHeight,
        int depth = 0) {
    if (!node || depth > 16) return nullptr;
    auto name = node.Name();
    if (name == L"TaskbarFrameRepeater" || name == L"SystemTrayFrame" ||
        name == L"BlobShape" || name == L"WindhawkDotStack" ||
        winrt::get_class_name(node) == L"SystemTray.SystemTrayFrame")
        return nullptr;
    if (node != root && name == requested.c_str() &&
        IsUsableReservedWidget(node, root, maxWidth, maxHeight))
        return node;
    int count = media::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i) {
        auto child = media::VisualTreeHelper::GetChild(node, i)
                         .try_as<FrameworkElement>();
        if (auto match = FindReservedWidgetByName(
                child, root, requested, maxWidth, maxHeight, depth + 1))
            return match;
    }
    return nullptr;
}

// Cache only verified named widgets. Invalid, detached, or renamed elements
// are removed and rediscovered, so rebuilding one monitor can't reuse the
// widget from another monitor or from an obsolete XAML subtree.
FrameworkElement CachedReservedWidgetByName(
        FrameworkElement const& content, FrameworkElement const& root,
        std::wstring const& name, double maxWidth, double maxHeight) {
    BoundaryWatch* watch = nullptr;
    auto owner = g_boundaryWatches.find(g_activeTaskbarWindow);
    if (owner != g_boundaryWatches.end() &&
        owner->second.content.get() == content)
        watch = &owner->second;
    if (watch) {
        auto cached = watch->namedWidgets.find(name);
        if (cached != watch->namedWidgets.end()) {
            auto element = cached->second.get();
            if (element && element.Name() == name &&
                IsUsableReservedWidget(element, root, maxWidth, maxHeight))
                return element;
            watch->namedWidgets.erase(cached);
        }
    }
    auto element = FindReservedWidgetByName(
        root, root, name, maxWidth, maxHeight);
    if (watch && element) watch->namedWidgets[name] = element;
    return element;
}

std::vector<HorizontalSpan> CollectExternalSpans(FrameworkElement const& content) {
    std::vector<HorizontalSpan> spans;
    if (!content) return spans;
    double width = content.ActualWidth(), height = content.ActualHeight();
    if (!std::isfinite(width) || !std::isfinite(height) ||
        width < 100 || height < 16) return spans;
    auto tree = FindTaskbarTree(content, g_activeTaskbarWindow);
    auto root = tree.grid;
    if (!root) return spans;
    double tray = TrayEdgeForContent(content, content);
    if (!std::isfinite(tray) || tray <= 0 || tray > width) return spans;

    // Read the real widget rectangle, but preserve a previously verified
    // right-docked slot while the system tray is changing width. Otherwise
    // the external widget can jump to the left of the icons, making our right
    // zone reclaim the empty slot, which sends the widget to the right again.
    // With extra containers, the RootGrid nearest to the app list may differ
    // from TaskbarFrame's own RootGrid, which is where that mod attaches.
    auto monitorRoot = root;
    auto monitor = FindDirectChildByName(root, L"WindhawkTaskbarSystemInfo");
    if (!monitor) {
        auto frameGrid = FindDirectChildByName(tree.frame, L"RootGrid");
        if (frameGrid && frameGrid != root) {
            monitor = FindDirectChildByName(frameGrid,
                                            L"WindhawkTaskbarSystemInfo");
            if (monitor) monitorRoot = frameGrid;
        }
    }
    bool hasMonitor = false;
    HorizontalSpan monitorSpan{};
    if (IsUsableReservedWidget(monitor, monitorRoot,
            std::min(650.0, width * 0.60), height)) {
        try {
            double left = ElementX(monitor, content);
            double right = left + monitor.ActualWidth();
            if (std::isfinite(left) && std::isfinite(right) &&
                left >= -0.5 && right > left + 8.0 &&
                right <= tray + 0.5) {
                monitorSpan = {std::max(0.0, left), right};
                hasMonitor = true;
            }
        } catch (...) {
            // Do not trust a detached XAML element's old position.
        }
    }

    auto owner = g_boundaryWatches.find(g_activeTaskbarWindow);
    BoundaryWatch* watch = owner != g_boundaryWatches.end() &&
        owner->second.content.get() == content ? &owner->second : nullptr;
    auto& ui = Ui();
    ULONGLONG now = GetTickCount64();
    bool useProjectedDock = false;
    if (watch) {
        // A real tray change starts a short handover. Do not start it for a
        // widget-only move; manual widget positioning must remain possible.
        if (watch->rightDockTracked &&
            std::abs(tray - watch->rightDockObservedTray) > 1.0) {
            watch->rightDockObservedTray = tray;
            watch->rightDockHoldUntil = now + 2600;
            Wh_Log(L"Taskbar %p: tray moved; holding right widget slot",
                   g_activeTaskbarWindow);
        }

        bool actualRightDocked = false;
        if (hasMonitor) {
            double trailing = tray - monitorSpan.right;
            bool afterRightApps = !ui.geometryInitialized ||
                ui.lastRightGroupWidth <= 0 ||
                monitorSpan.left >= ui.lastRightBound - 3.0;
            actualRightDocked = afterRightApps &&
                monitorSpan.left >= width * 0.45 &&
                trailing >= -0.5 && trailing <= 100.0;
        }

        if (actualRightDocked && now >= watch->rightDockHoldUntil) {
            // Update the anchor only when the handover has ended. A native
            // tray expansion may briefly make the *old* widget coordinates
            // appear valid with a smaller trailing gap. Accepting that gap
            // would erase the displacement we need to anticipate.
            watch->rightDockTracked = true;
            watch->rightDockWidth = monitorSpan.right - monitorSpan.left;
            watch->rightDockTrailingGap = std::max(0.0, tray - monitorSpan.right);
            watch->rightDockObservedTray = tray;
        }

        if (watch->rightDockTracked &&
            now < watch->rightDockHoldUntil) {
            double predictedRight = tray - watch->rightDockTrailingGap;
            double predictedLeft = predictedRight - watch->rightDockWidth;
            // If there genuinely isn't enough space for the existing zones
            // and the widget, use the normal safe layout fallback instead of
            // manufacturing a reservation that cannot be honored.
            bool fits = predictedLeft >= 0.0 &&
                        predictedRight <= tray + 0.5;
            if (fits && ui.geometryInitialized) {
                double minimumStart = ui.lastLeftBound +
                    ui.lastLeftGroupWidth + ui.lastRightGroupWidth + 12.0;
                fits = predictedLeft >= minimumStart - 1.0;
            }
            if (fits) {
                monitorSpan = {predictedLeft, predictedRight};
                useProjectedDock = true;
            } else {
                watch->rightDockHoldUntil = 0;
                watch->rightDockTracked = false;
                Wh_Log(L"Taskbar %p: no room for right widget reservation",
                       g_activeTaskbarWindow);
            }
        } else if (!actualRightDocked && watch->rightDockTracked) {
            // A stable/manual move to the middle or left is still supported.
            watch->rightDockTracked = false;
            watch->rightDockHoldUntil = 0;
        }
    }
    if (hasMonitor || useProjectedDock)
        spans.push_back(monitorSpan);

    std::vector<std::wstring> customNames;
    {
        std::lock_guard<std::mutex> lock(g_reservedWidgetNamesMutex);
        customNames = g_reservedWidgetNames;
    }
    // Drop names removed in settings so the per-window cache stays bounded.
    if (auto watch = g_boundaryWatches.find(g_activeTaskbarWindow);
        watch != g_boundaryWatches.end() && watch->second.content.get() == content) {
        for (auto it = watch->second.namedWidgets.begin();
             it != watch->second.namedWidgets.end();) {
            if (std::find(customNames.begin(), customNames.end(), it->first) ==
                customNames.end())
                it = watch->second.namedWidgets.erase(it);
            else
                ++it;
        }
    }
    for (auto const& name : customNames) {
        auto element = CachedReservedWidgetByName(content, root, name,
            std::min(650.0, width * 0.60), height);
        if (!element) continue;
        try {
            double x = ElementX(element, content);
            double w = element.ActualWidth();
            if (std::isfinite(x) && x >= 0 && x + w <= tray + 0.5)
                spans.push_back({x, x + w});
        } catch (...) { /* Detaching widget: skip this sample. */ }
    }

    std::sort(spans.begin(), spans.end(), [](auto const& a, auto const& b) {
        return a.left < b.left || (a.left == b.left && a.right < b.right);
    });
    std::vector<HorizontalSpan> merged;
    for (auto const& span : spans) {
        if (!merged.empty() && span.left <= merged.back().right + 0.5)
            merged.back().right = std::max(merged.back().right, span.right);
        else
            merged.push_back(span);
    }
    return merged;
}

// Preserve the last measured widget for a short XAML detach/rebuild only.
// The identity and screen width must still match this taskbar; reconnecting a
// monitor or changing resolution must never reuse a different monitor's area.
std::vector<HorizontalSpan> EffectiveExternalSpans(FrameworkElement const& content) {
    auto found = g_boundaryWatches.find(g_activeTaskbarWindow);
    if (found == g_boundaryWatches.end() || found->second.content.get() != content)
        return CollectExternalSpans(content);
    auto const& watch = found->second;
    ULONGLONG now = GetTickCount64();
    // A native tray width change must move the reserved right-side widget
    // slot *before* the other mod can flee from the newly crowded area. The
    // reservation is projected using the last verified right-docked width
    // and trailing gap. Ordinary widget-only movement uses accepted samples.
    if (watch.rightDockTracked) {
        double liveTray = TrayEdgeForContent(content, content);
        if (std::isfinite(liveTray) && liveTray > 0 &&
            (std::abs(liveTray - watch.rightDockObservedTray) > 1.0 ||
             now < watch.rightDockHoldUntil))
            return CollectExternalSpans(content);
    }
    if (watch.observed &&
        std::abs(watch.lastScanContentWidth - content.ActualWidth()) < 1)
        return watch.spans;
    return RetainTransientSpans(content, watch, CollectExternalSpans(content), now);
}

// Pack whole zones into the nearest free space, never placing individual app
// buttons across a third-party widget. An obstruction not touching either
// zone remains untouched; when a zone grows into it, move the whole group.
// This routine is deliberately independent from XAML and testable with spans.
bool ResolveAvailableZoneBounds(double leftWidth, double rightWidth,
                                std::vector<HorizontalSpan> const& obstacles,
                                double& leftEdge, double& rightEdge,
                                double leftClearance = 6.0,
                                double rightClearance = 6.0) {
    if (!std::isfinite(leftEdge) || !std::isfinite(rightEdge) ||
        !std::isfinite(leftWidth) || !std::isfinite(rightWidth) ||
        leftWidth < 0 || rightWidth < 0) return false;
    auto overlaps = [](double a, double b, double c, double d) {
        return a < d - 0.5 && c < b - 0.5;
    };
    for (size_t attempt = 0; attempt <= obstacles.size(); ++attempt) {
        bool moved = false;
        for (auto const& obstacle : obstacles) {
            if (!std::isfinite(obstacle.left) || !std::isfinite(obstacle.right) ||
                obstacle.right <= obstacle.left) continue;
            if (leftWidth > 0 && overlaps(leftEdge, leftEdge + leftWidth,
                    obstacle.left - leftClearance, obstacle.right + leftClearance)) {
                leftEdge = std::max(leftEdge, obstacle.right + leftClearance);
                moved = true;
            }
            if (rightWidth > 0 && overlaps(rightEdge - rightWidth, rightEdge,
                    obstacle.left - rightClearance, obstacle.right + rightClearance)) {
                rightEdge = std::min(rightEdge, obstacle.left - rightClearance);
                moved = true;
            }
        }
        if (leftEdge + leftWidth > rightEdge - rightWidth + 0.05)
            return false;
        if (!moved) return true;
    }
    return false; // An unstable/cyclic geometry falls back to Windows.
}


std::wstring ReadStoredString(wchar_t const* key) {
    std::vector<wchar_t> buffer(65536, L'\0');
    Wh_GetStringValue(key, buffer.data(), buffer.size());
    buffer.back() = L'\0';
    return std::wstring(buffer.data());
}

// V3 saves complete bounded lists in alternating banks of small Windhawk
// values. A single commit marker selects the active bank. A failed write
// never overwrites the active bank; backup refers to the previous marker.
constexpr size_t kStateChunkChars = 16000;
constexpr size_t kStateMaxChars = 1700000;
constexpr size_t kStateMaxChunks = (kStateMaxChars + kStateChunkChars - 1) / kStateChunkChars;

bool EncodeWideAppOrder(std::vector<std::wstring> const& ids,
                        std::wstring& output) {
    if (ids.size() > 512) return false;
    output = L"1;";
    std::unordered_set<std::wstring> seen;
    for (auto const& id : ids) {
        if (id.empty() || id.size() > 1024 ||
            id.find(L'\0') != std::wstring::npos || !seen.insert(id).second)
            return false;
        output += std::to_wstring(id.size()) + L":" + id;
        if (output.size() > 530000) return false;
    }
    return true;
}

bool DecodeWideAppOrder(std::wstring const& input,
                        std::vector<std::wstring>& output) {
    if (input.rfind(L"1;", 0) != 0 || input.size() > 530000) return false;
    std::vector<std::wstring> ids;
    std::unordered_set<std::wstring> seen;
    size_t pos = 2;
    while (pos < input.size()) {
        size_t length = 0, digits = 0;
        while (pos < input.size() && input[pos] >= L'0' && input[pos] <= L'9') {
            if (++digits > 4) return false;
            length = length * 10 + (input[pos++] - L'0');
        }
        if (!digits || !length || length > 1024 || pos >= input.size() ||
            input[pos++] != L':' || length > input.size() - pos || ids.size() >= 512)
            return false;
        auto id = input.substr(pos, length);
        if (id.find(L'\0') != std::wstring::npos || !seen.insert(id).second)
            return false;
        ids.push_back(std::move(id));
        pos += length;
    }
    output.swap(ids);
    return true;
}

bool EncodeWideState(PersistedAppState const& state, std::wstring& record) {
    std::wstring left, right, fixed;
    if (!EncodeWideAppOrder(state.leftOrder, left) ||
        !EncodeWideAppOrder(state.rightOrder, right) ||
        !EncodeWideAppOrder(state.rightIds, fixed)) return false;
    record = L"3;";
    for (auto const* part : {&left, &right, &fixed}) {
        record += std::to_wstring(part->size()) + L":" + *part;
        if (record.size() > kStateMaxChars) return false;
    }
    return true;
}

bool DecodeWideState(std::wstring const& record, PersistedAppState& state) {
    if (record.rfind(L"3;", 0) != 0 || record.size() > kStateMaxChars)
        return false;
    size_t pos = 2;
    std::vector<std::wstring> parts;
    for (int i = 0; i < 3; ++i) {
        size_t length = 0, digits = 0;
        while (pos < record.size() && record[pos] >= L'0' && record[pos] <= L'9') {
            if (++digits > 7) return false;
            length = length * 10 + (record[pos++] - L'0');
        }
        if (!digits || length < 2 || length > 530000 || pos >= record.size() ||
            record[pos++] != L':' || length > record.size() - pos)
            return false;
        parts.push_back(record.substr(pos, length));
        pos += length;
    }
    if (pos != record.size()) return false;
    PersistedAppState next;
    if (!DecodeWideAppOrder(parts[0], next.leftOrder) ||
        !DecodeWideAppOrder(parts[1], next.rightOrder) ||
        !DecodeWideAppOrder(parts[2], next.rightIds)) return false;
    // Reject inconsistencies rather than silently accepting partial membership.
    for (auto const& id : next.rightIds) {
        if (std::find(next.rightOrder.begin(), next.rightOrder.end(), id) ==
            next.rightOrder.end()) return false;
    }
    for (auto const& id : next.rightOrder) {
        if (std::find(next.rightIds.begin(), next.rightIds.end(), id) ==
            next.rightIds.end()) return false;
    }
    for (auto const& id : next.leftOrder) {
        if (std::find(next.rightIds.begin(), next.rightIds.end(), id) !=
            next.rightIds.end()) return false;
    }
    state = std::move(next);
    return true;
}

uint64_t StateHash(std::wstring const& record) {
    uint64_t hash = 14695981039346656037ULL;
    for (wchar_t value : record) {
        hash ^= static_cast<uint16_t>(value);
        hash *= 1099511628211ULL;
    }
    return hash;
}

std::wstring StateChunkKey(wchar_t bank, size_t index) {
    return L"fixed-sides-v3-" + std::wstring(1, bank) + L"-" +
           std::to_wstring(index);
}

struct BankManifest {
    wchar_t bank = 0;
    size_t count = 0, length = 0;
    uint64_t hash = 0;
};

bool ParseBankManifest(std::wstring const& text, BankManifest& manifest) {
    if (text.size() < 10 || text.size() > 110 || text.rfind(L"3;", 0) != 0)
        return false;
    std::wstring fields[4];
    size_t start = 2;
    for (int i = 0; i < 4; ++i) {
        size_t end = text.find(L';', start);
        if (end == std::wstring::npos) end = text.size();
        if (end == start || (i < 3 && end == text.size()) ||
            (i == 3 && end != text.size())) return false;
        fields[i] = text.substr(start, end - start);
        start = end + 1;
    }
    if (fields[0].size() != 1 ||
        (fields[0][0] != L'A' && fields[0][0] != L'B')) return false;
    for (int i = 1; i < 4; ++i) {
        if (fields[i].size() > 20) return false;
        for (wchar_t c : fields[i]) if (c < L'0' || c > L'9') return false;
    }
    try {
        auto count = std::stoull(fields[1]);
        auto length = std::stoull(fields[2]);
        auto hash = std::stoull(fields[3]);
        if (!count || count > kStateMaxChunks || length < 8 ||
            length > kStateMaxChars ||
            count != (length + kStateChunkChars - 1) / kStateChunkChars)
            return false;
        manifest = {fields[0][0], static_cast<size_t>(count),
                    static_cast<size_t>(length), hash};
        return true;
    } catch (...) { return false; }
}

bool ReadBankState(std::wstring const& text, PersistedAppState& state,
                   std::wstring& record, wchar_t& bank) {
    BankManifest manifest;
    if (!ParseBankManifest(text, manifest)) return false;
    std::wstring combined;
    combined.reserve(manifest.length);
    for (size_t i = 0; i < manifest.count; ++i) {
        auto key = StateChunkKey(manifest.bank, i);
        std::vector<wchar_t> buffer(kStateChunkChars + 1, L'\0');
        Wh_GetStringValue(key.c_str(), buffer.data(), buffer.size());
        buffer.back() = L'\0';
        std::wstring part(buffer.data());
        size_t required = std::min(kStateChunkChars,
                                   manifest.length - combined.size());
        if (part.size() != required) return false;
        combined += part;
    }
    if (combined.size() != manifest.length || StateHash(combined) != manifest.hash ||
        !DecodeWideState(combined, state)) return false;
    record = std::move(combined);
    bank = manifest.bank;
    return true;
}

bool SaveBankState(wchar_t bank, std::wstring const& record,
                   std::wstring& manifest) {
    size_t count = (record.size() + kStateChunkChars - 1) / kStateChunkChars;
    if (!count || count > kStateMaxChunks) return false;
    for (size_t i = 0; i < count; ++i) {
        auto key = StateChunkKey(bank, i);
        auto part = record.substr(i * kStateChunkChars, kStateChunkChars);
        if (!Wh_SetStringValue(key.c_str(), part.c_str())) return false;
    }
    manifest = L"3;" + std::wstring(1, bank) + L";" +
        std::to_wstring(count) + L";" + std::to_wstring(record.size()) + L";" +
        std::to_wstring(StateHash(record));
    // Verify each chunk is readable BEFORE publishing the commit pointer.
    PersistedAppState verified;
    std::wstring rebuilt;
    wchar_t checkedBank = 0;
    if (!ReadBankState(manifest, verified, rebuilt, checkedBank) ||
        checkedBank != bank || rebuilt != record) {
        Wh_Log(L"Inactive bank verification failed; not publishing state");
        return false;
    }
    return true;
}

void RememberRightId(std::vector<std::wstring>& ids,
                             std::wstring const& id) {
    if (id.empty() || id.size() > 1024 ||
        id.find(L'\0') != std::wstring::npos || ids.size() >= 512) return;
    if (std::find(ids.begin(), ids.end(), id) == ids.end()) ids.push_back(id);
}

// [saved ids]
// Older versions could save per-window IDs created by the grouping mod. Fold
// them into the base ID. State without such entries is left exactly as saved.
// A saved base ID wins over its per-window variants; otherwise a right-zone
// variant wins, because only dragging puts an ID there.
void NormalizeGroupedIds(PersistedAppState& state) {
    auto hasSuffix = [](std::vector<std::wstring> const& ids) {
        return std::any_of(ids.begin(), ids.end(), [](auto const& id) {
            return StripGroupingSuffix(id) != id;
        });
    };
    if (!hasSuffix(state.leftOrder) && !hasSuffix(state.rightOrder)) return;
    auto contains = [](std::vector<std::wstring> const& ids,
                       std::wstring const& id) {
        return std::find(ids.begin(), ids.end(), id) != ids.end();
    };
    std::vector<std::wstring> right, left;
    for (auto const& id : state.rightOrder) {
        auto base = StripGroupingSuffix(id);
        if (contains(state.leftOrder, base) &&
            !contains(state.rightOrder, base)) continue;
        if (!contains(right, base)) right.push_back(base);
    }
    for (auto const& id : state.leftOrder) {
        auto base = StripGroupingSuffix(id);
        if (!contains(right, base) && !contains(left, base))
            left.push_back(base);
    }
    state.leftOrder = std::move(left);
    state.rightOrder = right;
    state.rightIds = std::move(right);
}
// [end saved ids]

void EnsureSharedStateLoadedLocked() {
    if (g_sharedLoaded) return;
    g_sharedLoaded = true;
    PersistedAppState next;
    std::wstring rebuilt;
    wchar_t active = 0;
    auto marker = ReadStoredString(L"fixed-sides-commit-v3");
    if (ReadBankState(marker, next, rebuilt, active)) {
        NormalizeGroupedIds(next);
        g_sharedState = std::move(next);
        g_committedRecord = std::move(rebuilt);
        g_committedBankManifest = marker;
        g_activeStorageBank = active;
        return;
    }
    marker = ReadStoredString(L"fixed-sides-backup-v3");
    if (ReadBankState(marker, next, rebuilt, active)) {
        Wh_Log(L"Restored last valid segmented taskbar preferences");
        NormalizeGroupedIds(next);
        g_sharedState = std::move(next);
        g_committedRecord = std::move(rebuilt);
        g_committedBankManifest = marker;
        g_activeStorageBank = active;
        return;
    }
}

void LoadAppOrders() {
    auto& ui = Ui();
    std::lock_guard<std::mutex> lock(g_storeMutex);
    EnsureSharedStateLoadedLocked();
    if (ui.stateRevision == g_sharedRevision) return;
    ui.leftAppOrder = g_sharedState.leftOrder;
    ui.rightAppOrder = g_sharedState.rightOrder;
    ui.rightAppIds = g_sharedState.rightIds;
    ui.stateRevision = g_sharedRevision;
}

// Remember each button's AppID for short XAML gaps and for change detection,
// and log identity changes. Nothing here is saved, and no identity is ever
// guessed from titles, executables or positions.
void TrackAppIdentities(std::vector<FrameworkElement> const& children) {
    auto& ui = Ui();
    ULONGLONG now = GetTickCount64();
    std::unordered_map<void*, RememberedAppId> remembered;
    std::vector<std::wstring> nativeIds;
    ui.plannedIds.clear();
    for (auto const& child : children) {
        if (!IsTaskButton(child)) continue;
        auto id = ButtonAppId(child);
        ui.plannedIds.emplace_back(child, id);
        void* key = winrt::get_abi(child);
        auto old = ui.rememberedIds.find(key);
        bool sameElement = old != ui.rememberedIds.end() &&
                           old->second.element.get() == child;
        if (!id.empty()) {
            if (sameElement && old->second.id != id)
                Wh_Log(L"TZ ID container %p changed '%ls' -> '%ls'", key,
                       old->second.id.c_str(), id.c_str());
            remembered[key] = RememberedAppId{child, id, now};
        } else if (sameElement && now - old->second.seen < kEmptyAppIdGraceMs) {
            Wh_Log(L"TZ ID container %p temporarily empty, keeping '%ls'",
                   key, old->second.id.c_str());
            id = old->second.id;
            remembered[key] = old->second;
        }
        nativeIds.push_back(StripGroupingSuffix(id));
    }
    ui.rememberedIds.swap(remembered);

    if (nativeIds != ui.nativeIds) {
        for (size_t i = 0; i < ui.nativeIds.size(); ++i)
            if (!ContainsId(nativeIds, ui.nativeIds[i]))
                Wh_Log(L"TZ ID gone: slot %zu '%ls'", i,
                       ui.nativeIds[i].c_str());
        for (size_t i = 0; i < nativeIds.size(); ++i)
            if (!ContainsId(ui.nativeIds, nativeIds[i]))
                Wh_Log(L"TZ ID new: slot %zu '%ls'", i,
                       nativeIds[i].c_str());
        ui.nativeIds = nativeIds;
    }
}

// XAML does not necessarily arrange again when only an AutomationId changes.
bool AppIdentityChanged(TaskbarUiState const& ui) {
    for (auto const& [reference, planned] : ui.plannedIds) {
        auto element = reference.get();
        if (element && ButtonAppId(element) != planned) return true;
    }
    return false;
}

// Storage is committed by a bounded taskbar HWND timer, not by a background
// thread that could remain in this DLL after unload. A failed write NEVER
// clears the pending state. Keep retrying on a live taskbar until it succeeds.
// A drag is accepted into memory first; durable completion is asynchronous.
UINT StorageKickMessage();
UINT_PTR StorageTimerId();
void RequestStorageRetry();
std::wstring g_dirtyRecord;
bool g_storageDirty = false;            // Protected by g_storeMutex.
unsigned g_storageFailures = 0;        // Protected by g_storeMutex.
ULONGLONG g_storageNextAttempt = 0;    // Protected by g_storeMutex.
ULONGLONG g_storageLastErrorLog = 0;    // Protected by g_storeMutex.

bool CommitRecordToStorage(std::wstring const& record) {
    if (record == g_committedRecord) return true;
    if (!g_committedBankManifest.empty() &&
        !Wh_SetStringValue(L"fixed-sides-backup-v3",
                           g_committedBankManifest.c_str())) {
        return false;
    }
    wchar_t inactive = g_activeStorageBank == L'A' ? L'B' : L'A';
    std::wstring manifest;
    if (!SaveBankState(inactive, record, manifest)) return false;
    if (!Wh_SetStringValue(L"fixed-sides-commit-v3", manifest.c_str())) return false;
    g_committedRecord = record;
    g_committedBankManifest = std::move(manifest);
    g_activeStorageBank = inactive;
    return true;
}

// No background thread, and no blocking join at unload. This performs at
// most ONE attempt per call; failures are retried by the owner HWND timer.
bool FlushPendingStorage(bool force = false) {
    std::lock_guard<std::mutex> lock(g_storeMutex);
    if (!g_storageDirty) return true;
    ULONGLONG now = GetTickCount64();
    if (!force && now < g_storageNextAttempt) return false;
    bool committed = false;
    try { committed = CommitRecordToStorage(g_dirtyRecord); }
    catch (...) { committed = false; }
    if (committed) {
        g_storageDirty = false;
        g_dirtyRecord.clear();
        g_storageFailures = 0;
        g_storageNextAttempt = 0;
        return true;
    }
    if (g_storageFailures < 12) ++g_storageFailures;
    // Bounded backoff: 1, 2, 4, 8, 16, 30 seconds. Keep the record until
    // it is committed or the mod is unloaded. Never silently discard it.
    DWORD delay = std::min<DWORD>(30000u,
        1000u << std::min(g_storageFailures - 1, 5u));
    g_storageNextAttempt = now + delay;
    if (!g_storageLastErrorLog || now - g_storageLastErrorLog >= 30000) {
        g_storageLastErrorLog = now;
        Wh_Log(L"saving preferences failed, will retry in %lu ms", delay);
    }
    return false;
}

bool SaveStateLocked(PersistedAppState const& next) {
    PersistedAppState normalized = next;
    for (auto const& id : normalized.rightIds)
        RememberRightId(normalized.rightOrder, id);
    for (auto const& id : normalized.rightOrder)
        RememberRightId(normalized.rightIds, id);
    for (auto const& id : normalized.rightIds)
        normalized.leftOrder.erase(std::remove(normalized.leftOrder.begin(),
            normalized.leftOrder.end(), id), normalized.leftOrder.end());
    std::wstring record;
    if (!EncodeWideState(normalized, record)) {
        Wh_Log(L"Invalid fixed-side record; existing preferences preserved");
        return false;
    }
    // This is invoked on the taskbar owner thread under g_storeMutex. Check
    // that a usable HWND exists BEFORE accepting a deferred save.
    HWND window = g_activeTaskbarWindow;
    if (!window || !IsTaskbarWindow(window) ||
        GetWindowThreadProcessId(window, nullptr) != GetCurrentThreadId()) {
        Wh_Log(L"no owner taskbar for storage scheduling");
        return false;
    }
    std::wstring oldRecord = g_dirtyRecord;
    bool oldDirty = g_storageDirty;
    unsigned oldFailures = g_storageFailures;
    ULONGLONG oldNextAttempt = g_storageNextAttempt;
    g_dirtyRecord = std::move(record);
    g_storageDirty = true;
    g_storageFailures = 0;
    g_storageNextAttempt = GetTickCount64() + 350;
    if (!SetTimer(window, StorageTimerId(), 400, nullptr)) {
        // No false acknowledgment when no timer can flush the record.
        g_dirtyRecord = std::move(oldRecord);
        g_storageDirty = oldDirty;
        g_storageFailures = oldFailures;
        g_storageNextAttempt = oldNextAttempt;
        Wh_Log(L"cannot start storage timer");
        return false;
    }
    g_sharedState = std::move(normalized);
    ++g_sharedRevision;
    return true; // Accepted for durable retry; see g_storageDirty.
}

void FlushStorageBeforeUninit() {
    bool ok = FlushPendingStorage(true);
    if (!ok) Wh_Log(L"WARNING: latest fixed-side preferences could not be saved before unload");
}

void RememberApp(std::vector<std::wstring>& ids, std::wstring const& id) {
    if (id.empty() || id.size() > 1024 || id.find(L'\0') != std::wstring::npos ||
        std::find(ids.begin(), ids.end(), id) != ids.end()) return;
    if (ids.size() >= 512) {
        Wh_Log(L"AppID order capacity reached (512); no existing preference evicted");
        return;
    }
    ids.push_back(id);
}

void ApplyAppOrder(std::vector<ButtonInfo*>& buttons,
                   std::vector<std::wstring>& ids,
                   std::vector<winrt::weak_ref<FrameworkElement>>& liveOrder) {
    struct Ranked { ButtonInfo* button; size_t rank; };
    std::vector<Ranked> ranked;
    for (auto button : buttons) RememberApp(ids, button->appId);
    for (auto button : buttons) {
        auto found = std::find(ids.begin(), ids.end(), button->appId);
        ranked.push_back({button, size_t(found - ids.begin())});
    }
    std::stable_sort(ranked.begin(), ranked.end(),
        [](auto const& a, auto const& b) { return a.rank < b.rank; });
    std::vector<winrt::weak_ref<FrameworkElement>> next;
    for (size_t i = 0; i < ranked.size(); ++i) {
        buttons[i] = ranked[i].button;
        next.emplace_back(buttons[i]->element);
    }
    liveOrder.swap(next);
}

// Reorder the visible subset without discarding saved ranks of overflow apps.
void MergeVisibleAppOrder(std::vector<std::wstring>& saved,
                          std::vector<std::wstring> const& visible) {
    std::vector<std::wstring> unique;
    for (auto const& id : visible) RememberApp(unique, id);
    size_t next = 0;
    for (auto& id : saved) {
        if (std::find(unique.begin(), unique.end(), id) != unique.end()) id = unique[next++];
    }
    while (next < unique.size()) RememberApp(saved, unique[next++]);
}

// [icon spacing]
constexpr double kLabeledButtonWidth = 60.0;  // wider buttons carry a label
constexpr double kIconSideClearance = 2.0;
constexpr double kDefaultIconWidth = 24.0;

// How much narrower a centred-icon button may become without cutting the icon.
double MaxButtonReduction(double nativeWidth, double iconWidth) {
    if (!std::isfinite(nativeWidth) || nativeWidth > kLabeledButtonWidth)
        return 0;
    if (!std::isfinite(iconWidth) || iconWidth <= 0)
        iconWidth = kDefaultIconWidth;
    return std::max(0.0,
        (nativeWidth - iconWidth) / 2.0 - kIconSideClearance);
}

// Recover the native width of a button this mod may currently be narrowing.
// A measurement equal to the narrowed slot is our own result, not a new size.
double ResolveNativeWidth(double measured, double previousNative,
                          double previousSlot) {
    if (previousNative > 0 && previousSlot > 0 &&
        previousSlot < previousNative &&
        std::abs(measured - previousSlot) <= 1.0)
        return previousNative;
    return measured;
}

double ZoneContentWidth(std::vector<double> const& widths, double gap) {
    double total = 0;
    unsigned count = 0;
    for (double width : widths) {
        if (width <= 0) continue;
        total += width;
        ++count;
    }
    return count > 1 ? total + gap * (count - 1) : total;
}
// [end icon spacing]

double IconWidthOf(FrameworkElement const& button) {
    auto icon = FindNamedDescendant(button, L"Icon", 0);
    return icon && icon.Visibility() == Visibility::Visible
        ? icon.ActualWidth() : 0.0;
}

// The width a button occupies in the current plan (narrowed or native).
double SlotWidth(FrameworkElement const& element) {
    auto& widths = Ui().slotWidths;
    auto found = widths.find(winrt::get_abi(element));
    return found != widths.end() ? found->second : ElementWidth(element);
}

struct Placement {
    winrt::weak_ref<FrameworkElement> element;
    float x;
    double measuredWidth;
    float width;  // 0 keeps the width Windows chose
};
// Built before the original ArrangeOverride. The nested Arrange hook only
// performs an ABI identity lookup; it never traverses or mutates the tree.
thread_local std::unordered_map<void*, Placement> g_arrangePlan;
thread_local bool g_useArrangePlan = false;

void PlanElement(FrameworkElement const& element, double x, double width = 0) {
    // Rect.X is the margin-box origin, not the rendered border-box origin.
    g_arrangePlan.emplace(winrt::get_abi(element),
                          Placement{element, static_cast<float>(x),
                                    element.ActualWidth(),
                                    static_cast<float>(width)});
}

bool BuildLayoutPlan(FrameworkElement const& knownRepeater) {
    g_arrangePlan.clear();
    Ui().layoutValid = false;
    Ui().slots.clear();
    Ui().slotWidths.clear();
    try {
        auto repeater = knownRepeater ? knownRepeater : GetTaskbarRepeater();
        auto content = repeater && repeater.XamlRoot()
                           ? repeater.XamlRoot().Content().try_as<FrameworkElement>()
                           : nullptr;
        if (!repeater || !content) {
            LogDiscoveryFailure(10, L"layout content not ready", g_activeTaskbarWindow);
            return false;
        }
        // Vertical or malformed layouts are left to Windows.
        if (content.ActualWidth() < content.ActualHeight() * 2.0) {
            LogDiscoveryFailure(10, L"layout content is vertical or malformed", g_activeTaskbarWindow);
            return false;
        }
        double repeaterX = ElementX(repeater, content);
        EnsureBoundaryWatch(content);

        LoadAppOrders();
        auto nativeTray = FindNativeTrayFrame(content);
        auto trayHost = TrayHostInRepeater(nativeTray, repeater);
        auto children = RepeaterElements(repeater);
        TrackAppIdentities(children);
        std::vector<ButtonInfo> buttons;
        std::vector<FrameworkElement> systemButtons;
        FrameworkElement overflow{nullptr};
        buttons.reserve(children.size());
        for (auto const& child : children) {
            if (child.Visibility() != Visibility::Visible) {
                continue;
            }
            // Some Windows layouts expose the notification area through
            // TaskbarExtensionElement. It is not the Search button, even
            // when its outer runtime class is identical.
            if (child == trayHost || child == nativeTray ||
                (winrt::get_class_name(child) == L"Taskbar.TaskbarExtensionElement" &&
                 FindNamedDescendant(child, L"SystemTrayFrame"))) {
                static thread_local bool loggedTrayHost = false;
                if (!loggedTrayHost) {
                    loggedTrayHost = true;
                    Wh_Log(L"TZ TRAY native repeater item excluded: class=%ls name=%ls",
                           winrt::get_class_name(child).c_str(), child.Name().c_str());
                }
                continue;
            }
            double nativeWidth = ElementWidth(child);
            if (IsTaskButton(child) && nativeWidth > 0) {
                auto id = ButtonKey(child);
                buttons.push_back({child, nativeWidth,
                                   !IsFixedRight(id), std::move(id)});
            } else if (GetSystemButtonKind(child) != SystemButtonKind::None) {
                systemButtons.push_back(child);
            } else if (winrt::get_class_name(child) == L"Taskbar.OverflowToggleButton" ||
                       child.Name() == L"OverflowButton") {
                overflow = child;
            }
        }

        auto rank = [](FrameworkElement const& element) {
            switch (GetSystemButtonKind(element)) {
                case SystemButtonKind::Start: return 0;
                case SystemButtonKind::Search: return 1;
                case SystemButtonKind::TaskView: return 2;
                default: return 3;
            }
        };
        std::stable_sort(systemButtons.begin(), systemButtons.end(),
            [&](auto const& a, auto const& b) { return rank(a) < rank(b); });

        auto obstacles = EffectiveExternalSpans(content);
        double leftEdge = 0.0;
        double systemStart = 0, systemWidth = 0;
        if (g_settings.systemButtonsLeft.load()) {
            for (auto const& button : systemButtons)
                systemWidth += SystemButtonWidth(button);
            // External left-anchored elements may precede the Start button.
            // Move the whole native button group, not its individual icons.
            double unusedRight = content.ActualWidth();
            if (!ResolveAvailableZoneBounds(systemWidth, 0, obstacles,
                                            leftEdge, unusedRight)) {
                return false;
            }
            systemStart = leftEdge;
            for (auto const& button : systemButtons) {
                double width = SystemButtonWidth(button);
                if (width > 0) {
                    PlanElement(button, leftEdge - repeaterX);
                    leftEdge += width;
                }
            }
        } else {
            for (auto const& button : systemButtons) {
                // Widgets are reserved separately, on their actual side.
                if (GetSystemButtonKind(button) == SystemButtonKind::Widgets) continue;
                double width = SystemButtonWidth(button);
                if (width <= 0) continue;
                double nativeX = repeaterX + button.ActualOffset().x;
                leftEdge = std::max(leftEdge, nativeX + width);
            }
        }

        // Windows owns the tray. Only verified widget reservations change
        // these edges; random XAML decorations must never move app zones.
        double rightEdge = TrayEdgeForContent(content, content);
        if (!std::isfinite(rightEdge) || rightEdge <= 0) {
            g_arrangePlan.clear();
            return false;
        }
        if (auto found = g_boundaryWatches.find(g_activeTaskbarWindow);
            found != g_boundaryWatches.end() &&
            found->second.content.get() == content && found->second.observed) {
            // Honor new tray space immediately when it becomes smaller to
            // avoid overlap. Only release space on an expansion after the
            // geometry watcher accepts the stable edge.
            rightEdge = std::min(rightEdge, found->second.trayX);
        }
        double acceptedTrayX = rightEdge;
        if (!g_settings.systemButtonsLeft.load())
            ReserveNativeWidgets(content, leftEdge, rightEdge);
        rightEdge -= g_settings.trayGap.load();

        std::vector<ButtonInfo*> leftButtons;
        std::vector<ButtonInfo*> rightButtons;
        for (auto& button : buttons) {
            (button.leftSide ? leftButtons : rightButtons).push_back(&button);
        }
        ApplyAppOrder(leftButtons, Ui().leftAppOrder, Ui().leftLive);
        ApplyAppOrder(rightButtons, Ui().rightAppOrder, Ui().rightLive);
        ButtonInfo overflowInfo{overflow, overflow ? ElementWidth(overflow) : 0, true, {}};
        if (overflowInfo.width > 0) {
            leftButtons.push_back(&overflowInfo);
        }

        // Positive spacing adds a gap between slots. Negative spacing makes
        // the slots narrower; hit rectangles never overlap in either case.
        int leftSpacing = g_settings.leftIconSpacing.load();
        int rightSpacing = g_settings.rightIconSpacing.load();
        double leftGap = std::max(0, leftSpacing);
        double rightGap = std::max(0, rightSpacing);
        {
            auto& state = Ui();
            std::unordered_map<void*, NarrowedButton> narrowed;
            state.slotWidths.clear();
            for (auto& button : buttons) {
                void* key = winrt::get_abi(button.element);
                auto old = state.narrowed.find(key);
                bool known = old != state.narrowed.end() &&
                    old->second.element.get() == button.element;
                double native = known
                    ? ResolveNativeWidth(button.width, old->second.nativeWidth,
                                         old->second.slotWidth)
                    : button.width;
                int spacing = button.leftSide ? leftSpacing : rightSpacing;
                double slot = native;
                if (spacing < 0) {
                    slot -= std::min(double(-spacing), MaxButtonReduction(
                        native, IconWidthOf(button.element)));
                }
                button.width = slot;
                button.narrowedWidth = slot < native - 0.5 ? slot : 0;
                button.nativeWidth = native;
                // Keep one pass after narrowing ends, until XAML reports the
                // native width again.
                if (button.narrowedWidth > 0 ||
                    (known && old->second.slotWidth < native - 0.5))
                    narrowed[key] = NarrowedButton{button.element, native, slot};
                state.slotWidths[key] = slot;
            }
            state.narrowed.swap(narrowed);
        }

        auto totalWidth = [](std::vector<ButtonInfo*> const& group, double gap) {
            std::vector<double> widths;
            for (auto item : group) widths.push_back(item->width);
            return ZoneContentWidth(widths, gap);
        };

        // A narrowed button can keep its native ActualWidth, with the excess
        // only clipped. Other mods (Taskbar System Info) measure that size, so
        // the last button of each zone still needs room for it. Without this
        // it used up the clearance next to a widget, which then kept moving.
        auto trailingOverhang = [](std::vector<ButtonInfo*> const& group) {
            for (auto item = group.rbegin(); item != group.rend(); ++item) {
                if ((*item)->width <= 0) continue;
                return (*item)->narrowedWidth > 0
                    ? (*item)->nativeWidth - (*item)->width : 0.0;
            }
            return 0.0;
        };
        double rightOverhang = trailingOverhang(rightButtons);

        double leftWidth = totalWidth(leftButtons, leftGap) +
                           trailingOverhang(leftButtons);
        double rightWidth = totalWidth(rightButtons, rightGap) + rightOverhang;
        double requested = leftWidth + rightWidth;
        if (!ResolveAvailableZoneBounds(leftWidth, rightWidth, obstacles,
                                        leftEdge, rightEdge, 6.0,
                                        std::max(6.0, double(g_settings.trayGap.load())))) {
            auto& ui = Ui();
            ui.layoutValid = false;
            ULONGLONG now = GetTickCount64();
            ui.lastCrowdingRetry = now;
            if (!ui.lastCrowdingLog || now - ui.lastCrowdingLog > 30000) {
                ui.lastCrowdingLog = now;
                Wh_Log(L"Taskbar %p: blocked by occupied regions; native layout retained",
                       g_activeTaskbarWindow);
            }
            g_arrangePlan.clear();
            return false;
        }
        double span = rightEdge - leftEdge;
        // Each native button has its own hit rectangle. Never overlap those
        // rectangles to fake more room: use Windows' native overflow layout.
        if (!std::isfinite(span) || span < 0 ||
            !std::isfinite(requested) || requested > span) {
            // True crowding cannot be solved without overlapping native hit
            // targets. Keep the saved sides intact, use native placement, and
            // periodically remeasure to recover as soon as space returns.
            auto& ui = Ui();
            ui.layoutValid = false;
            ULONGLONG now = GetTickCount64();
            ui.lastCrowdingRetry = now;
            if (!ui.lastCrowdingLog || now - ui.lastCrowdingLog > 30000) {
                ui.lastCrowdingLog = now;
                Wh_Log(L"Taskbar %p crowded: required %.1f DIP, available %.1f DIP; native layout retained",
                       g_activeTaskbarWindow, requested, span);
            }
            g_arrangePlan.clear();
            return false;
        }
        auto& ui = Ui();
        ui.leftStart = leftEdge - repeaterX;
        double x = leftEdge;
        double leftItemEnd = leftEdge;
        for (auto item : leftButtons) {
            PlanElement(item->element, x - repeaterX, item->narrowedWidth);
            if (!item->appId.empty())
                ui.slots.push_back({item->appId, true, x - repeaterX, item->width});
            if (item->width <= 0) continue;
            leftItemEnd = x + item->width;
            x = leftItemEnd + leftGap;
        }
        ui.leftEnd = leftItemEnd - repeaterX;

        double rightSlotsEnd = rightEdge - rightOverhang;
        x = rightSlotsEnd;
        bool hasItemOnTheRight = false;
        for (auto item = rightButtons.rbegin(); item != rightButtons.rend(); ++item) {
            // The slot is the button's whole hit rectangle.
            double slotWidth = (*item)->width;
            if (slotWidth > 0 && hasItemOnTheRight) x -= rightGap;
            x -= slotWidth;
            if (slotWidth > 0) hasItemOnTheRight = true;
            PlanElement((*item)->element, x - repeaterX, (*item)->narrowedWidth);
            if (!(*item)->appId.empty())
                ui.slots.push_back({(*item)->appId, false, x - repeaterX, slotWidth});
        }
        ui.rightStart = x - repeaterX;
        ui.rightEnd = rightSlotsEnd - repeaterX;
        ui.leftGap = leftGap;
        ui.rightGap = rightGap;
        double dpi = 1.0;
        if (auto root = repeater.XamlRoot()) dpi = root.RasterizationScale();
        // Stable order-insensitive identity fingerprint: reordering alone
        // must not cancel a drag, but replacing a task button must.
        uint64_t buttonIdentity = 0;
        for (auto const& button : buttons) {
            auto idHash = StateHash(button.appId);
            // Addition preserves duplicate AppIDs (XOR would cancel pairs).
            buttonIdentity += ((idHash << 1) | (idHash >> 63)) +
                              0x9e3779b97f4a7c15ULL;
        }
        auto changed = [](double a, double b) {
            return !std::isfinite(a) || !std::isfinite(b) ||
                   std::abs(a - b) > 0.5;
        };
        if (!ui.geometryInitialized ||
            changed(ui.lastContentWidth, content.ActualWidth()) ||
            changed(ui.lastContentHeight, content.ActualHeight()) ||
            changed(ui.lastRepeaterX, repeaterX) ||
            changed(ui.lastLeftBound, leftEdge) ||
            changed(ui.lastRightBound, rightEdge) ||
            changed(ui.lastLeftGroupWidth, leftWidth) ||
            changed(ui.lastRightGroupWidth, rightWidth) ||
            changed(ui.lastSystemStart, systemStart) ||
            changed(ui.lastSystemWidth, systemWidth) ||
            changed(ui.lastDpi, dpi) ||
            ui.lastButtonCount != buttons.size() ||
            ui.lastButtonIdentity != buttonIdentity) {
            ++ui.geometryEpoch;
            ui.geometryInitialized = true;
        }
        ui.lastContentWidth = content.ActualWidth();
        ui.lastContentHeight = content.ActualHeight();
        ui.lastRepeaterX = repeaterX;
        ui.lastTrayX = acceptedTrayX;
        ui.lastLeftBound = leftEdge;
        ui.lastRightBound = rightEdge;
        ui.lastLeftGroupWidth = leftWidth;
        ui.lastRightGroupWidth = rightWidth;
        ui.lastSystemStart = systemStart;
        ui.lastSystemWidth = systemWidth;
        ui.lastDpi = dpi;
        ui.lastButtonCount = buttons.size();
        ui.lastButtonIdentity = buttonIdentity;
        ui.layoutValid = true;
        return true;
    } catch (...) {
        g_arrangePlan.clear();
        Wh_Log(L"layout pass failed safely");
        return false;
    }
}

using ArrangeOverride_t = HRESULT(WINAPI*)(
    void*, void*, winrt::Windows::Foundation::Size,
    winrt::Windows::Foundation::Size*);
ArrangeOverride_t ArrangeOverride_Original = nullptr;
std::vector<HWND> AllTaskbarWindows();
bool IsTaskbarWindow(HWND window);
bool EnsureTaskbarSubclass(HWND window);
HWND FindTaskbarWindowForElement(FrameworkElement const& element);
void RequestRefresh();
void RequestRefreshAll();

using ElementArrange_t = HRESULT(WINAPI*)(void*, winrt::Windows::Foundation::Rect);
ElementArrange_t ElementArrange_Original = nullptr;
void KeepDraggedSlot(FrameworkElement const& element,
                     winrt::Windows::Foundation::Rect& rect);
void CompletePendingDrop(FrameworkElement const& element);

HRESULT WINAPI ElementArrange_Hook(void* self, winrt::Windows::Foundation::Rect rect) {
    FrameworkElement arrangedElement{nullptr};
    if (g_useArrangePlan && !g_unloading) {
        try {
            FrameworkElement element{nullptr};
            reinterpret_cast<::IUnknown*>(self)->QueryInterface(
                winrt::guid_of<FrameworkElement>(), winrt::put_abi(element));
            if (element) {
                auto found = g_arrangePlan.find(winrt::get_abi(element));
                if (found != g_arrangePlan.end()) {
                    rect.X = found->second.x;
                    // Only the layout slot (and so the hit area) gets narrower;
                    // no element property is written, so nothing needs undoing.
                    if (found->second.width > 0)
                        rect.Width = found->second.width;
                    KeepDraggedSlot(element, rect);
                    arrangedElement = element;
                }
            }
        } catch (...) {
            // Preserve the native rect if COM lookup fails.
        }
    }
    HRESULT result = ElementArrange_Original(self, rect);
    if (SUCCEEDED(result) && arrangedElement) {
        static std::atomic<bool> loggedCustomArrange{false};
        if (!loggedCustomArrange.exchange(true))
            Wh_Log(L"Taskbar discovery: custom element arrangement applied");
        try { CompletePendingDrop(arrangedElement); } catch (...) {}
    }
    return result;
}

bool EnsureArrangeHook() {
    if (ElementArrange_Original) {
        return true;
    }
    if (g_unloading) return false;
    // Same IUIElement ABI entry point used by m417z's positioning mod.
    Shapes::Rectangle rectangle;
    IUIElement uiElement = rectangle;
    auto vtable = *reinterpret_cast<void***>(winrt::get_abi(uiElement));
    auto arrange = reinterpret_cast<ElementArrange_t>(vtable[92]);
    if (!WindhawkUtils::SetFunctionHook(arrange, ElementArrange_Hook,
                                      &ElementArrange_Original)) {
        LogDiscoveryFailure(6, L"IUIElement::Arrange hook unavailable", g_activeTaskbarWindow);
        return false;
    }
    Wh_ApplyHookOperations();
    Wh_Log(L"Taskbar discovery: IUIElement::Arrange hook installed");
    return true;
}

void UpdateDragPreview();

HRESULT WINAPI ArrangeOverride_Hook(
    void* self, void* context, winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize) {
    if (g_unloading || g_insideArrange) {
        // Nested layouts belong to Windows, not the outer taskbar plan.
        bool original = g_useArrangePlan;
        g_useArrangePlan = false;
        struct NestedPlanGuard {
            bool previous;
            ~NestedPlanGuard() { g_useArrangePlan = previous; }
        } nested{original};
        return ArrangeOverride_Original(self, context, size, resultSize);
    }
    struct ArrangeGuard {
        ArrangeGuard() { g_insideArrange = true; }
        ~ArrangeGuard() {
            g_useArrangePlan = false;
            g_arrangePlan.clear();
            g_insideArrange = false;
        }
    } guard;

    static std::atomic<bool> loggedArrange{false};
    if (!loggedArrange.exchange(true))
        Wh_Log(L"Taskbar discovery: ArrangeOverride entered");

    // A primary and a secondary taskbar have distinct XAML roots and layouts.
    // Compare the exact layout ABI identity instead of assuming one taskbar.
    HWND window = nullptr;
    FrameworkElement arrangeRepeater{nullptr};
    try {
        winrt::Windows::Foundation::IUnknown caller{nullptr};
        winrt::copy_from_abi(caller, self);
        auto layout = caller.try_as<winrt::Microsoft::UI::Xaml::Controls::Layout>();
        if (!layout) {
            LogDiscoveryFailure(7, L"ArrangeOverride caller is not a Layout", nullptr);
            return ArrangeOverride_Original(self, context, size, resultSize);
        }
        void* identity = winrt::get_abi(layout);
        if (auto cached = g_layoutOwners.find(identity); cached != g_layoutOwners.end()) {
            bool currentOwner = false;
            FrameworkElement currentElement{nullptr};
            if (cached->second.layout.get() == layout &&
                IsTaskbarWindow(cached->second.window) &&
                GetWindowThreadProcessId(cached->second.window, nullptr) == GetCurrentThreadId()) {
                // A detached-but-alive Layout must not be reused for another
                // taskbar after XAML hosts are recreated.
                ScopedTaskbarContext currentContext(cached->second.window);
                currentElement = GetTaskbarRepeater();
                auto currentRepeater = currentElement
                    ? currentElement.try_as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>()
                    : nullptr;
                auto currentLayout = currentRepeater ? currentRepeater.Layout() : nullptr;
                currentOwner = currentLayout &&
                    layout.as<winrt::Windows::Foundation::IUnknown>() ==
                    currentLayout.as<winrt::Windows::Foundation::IUnknown>();
            }
            if (currentOwner) {
                window = cached->second.window;
                arrangeRepeater = currentElement;
            } else g_layoutOwners.erase(cached);
        }
        if (!window) for (HWND candidate : AllTaskbarWindows()) {
            if (GetWindowThreadProcessId(candidate, nullptr) != GetCurrentThreadId())
                continue;
            auto root = TaskbarXamlRoot(candidate);
            auto content = root ? root.Content().try_as<FrameworkElement>() : nullptr;
            auto element = content ? FindTaskbarRepeater(content, candidate) : nullptr;
            auto repeater = element
                ? element.try_as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>()
                : nullptr;
            auto nativeLayout = repeater ? repeater.Layout() : nullptr;
            if (nativeLayout && layout.as<winrt::Windows::Foundation::IUnknown>() ==
                nativeLayout.as<winrt::Windows::Foundation::IUnknown>()) {
                window = candidate;
                arrangeRepeater = element;
                g_layoutOwners[identity] = CachedLayoutOwner{layout, candidate};
                break;
            }
        }
    } catch (...) {
        LogDiscoveryFailure(8, L"XAML layout owner lookup threw an exception", nullptr);
        return ArrangeOverride_Original(self, context, size, resultSize);
    }
    if (!window) {
        LogDiscoveryFailure(8, L"no matching taskbar layout owner", nullptr);
        return ArrangeOverride_Original(self, context, size, resultSize);
    }
    ScopedTaskbarContext taskbarContext(window);
    if (!EnsureTaskbarSubclass(window)) {
        LogDiscoveryFailure(9, L"taskbar subclass unavailable", window);
        return ArrangeOverride_Original(self, context, size, resultSize);
    }
    static std::atomic<bool> loggedLayoutOwner{false};
    if (!loggedLayoutOwner.exchange(true))
        Wh_Log(L"Taskbar discovery: taskbar layout identity verified (HWND %p)", window);

    bool planReady = false;
    try {
        if (EnsureArrangeHook()) {
            planReady = BuildLayoutPlan(arrangeRepeater);
            g_useArrangePlan = planReady;
        }
    } catch (...) {
        g_arrangePlan.clear();
        g_useArrangePlan = false;
    }
    HRESULT result = ArrangeOverride_Original(self, context, size, resultSize);
    g_useArrangePlan = false;
    if (g_settingsArrangeFollowup.exchange(false)) RequestRefreshAll();
    if (!g_unloading && planReady) {
        // Windows owns task button visuals. Keep the post-arrange checks
        // but never change Scale or CenterPoint on protected XAML elements.
        auto logPostArrangeError = [&](wchar_t const* phase, HRESULT hr) {
            auto& ui = Ui();
            ULONGLONG now = GetTickCount64();
            if (!ui.lastPostArrangeErrorLog ||
                now - ui.lastPostArrangeErrorLog >= 10000) {
                ui.lastPostArrangeErrorLog = now;
                Wh_Log(L"Post-arrange %ls failed on taskbar %p "
                       L"(HRESULT 0x%08X)",
                       phase, window, static_cast<unsigned int>(hr));
            }
        };
        winrt::Windows::UI::Xaml::XamlRoot expectedRoot{nullptr};
        try {
            if (arrangeRepeater) expectedRoot = arrangeRepeater.XamlRoot();
        } catch (...) {
            logPostArrangeError(L"root lookup", winrt::to_hresult());
        }
        for (auto const& [key, placement] : g_arrangePlan) {
            FrameworkElement element{nullptr};
            try {
                element = placement.element.get();
                if (!element) continue;
                // The native Arrange call can detach/recycle a planned
                // element before this post-arrange pass reaches it.
                auto elementRoot = element.XamlRoot();
                if (!elementRoot || !expectedRoot ||
                    elementRoot != expectedRoot) continue;
            } catch (...) {
                logPostArrangeError(L"element root lookup",
                                    winrt::to_hresult());
                continue;
            }
            try {
                if (std::abs(element.ActualWidth() -
                             placement.measuredWidth) > 0.05) {
                    RequestRefresh();
                }
            } catch (...) {
                logPostArrangeError(L"width refresh",
                                    winrt::to_hresult());
            }
        }
        try {
            UpdateDragPreview();
        } catch (...) {
            logPostArrangeError(L"drag preview", winrt::to_hresult());
        }
        try {
            if (arrangeRepeater) {
                for (auto const& child : RepeaterElements(arrangeRepeater)) {
                    if (IsTaskButton(child) && ElementWidth(child) > 0 &&
                        !g_arrangePlan.count(winrt::get_abi(child))) {
                        RequestRefresh();
                        break;
                    }
                }
            }
        } catch (...) {
            logPostArrangeError(L"button list check", winrt::to_hresult());
        }
    }
    return result;
}


// Deliver the real press immediately. Once the drag threshold is crossed,
// cancel that native press and own the drag; never replay old event args.
struct SectionGesture {
    winrt::weak_ref<FrameworkElement> source;
    Input::Pointer pointer{nullptr};
    winrt::Windows::Foundation::Point origin{};
    bool leftSide = false;
    bool dragged = false;
    HWND window = nullptr;
    unsigned long long stateRevision = 0;
    unsigned long long geometryEpoch = 0;
    unsigned long long configEpoch = 0;
    double sectionStart = 0, sectionEnd = 0, switchThreshold = 0;
    std::vector<TaskbarSlot> frozenTargetSlots;
    unsigned int pointerId = 0;
    double startX = 0;
    double desiredX = 0;
    double minX = 0;
    double maxX = 0;
    double width = 0;
    double sectionGap = 0;
    numerics::float3 translation{};
    float originalSlotX = 0;
    media::Animation::TransitionCollection transitions{nullptr};
    bool previewPrepared = false;
    bool ownsPointerCapture = false;
    std::wstring appId;
    std::vector<std::wstring> originalAppOrder;
    std::vector<winrt::weak_ref<FrameworkElement>> originalOrder;
};
// The pointer object is released on the UI thread, not at CRT shutdown.
[[clang::no_destroy]] thread_local std::optional<SectionGesture> g_sectionGesture;

struct PendingDrop {
    SectionGesture gesture;
    winrt::Windows::UI::Composition::ImplicitAnimationCollection implicitAnimations{nullptr};
    bool arranged = false;
    ULONGLONG started = 0;
    unsigned int stableSamples = 0;
    double lastX = -1e9;  // last observed layout position
};
[[clang::no_destroy]] thread_local std::optional<PendingDrop> g_pendingDrop;
thread_local HWND g_dropSettleWindow = nullptr;

UINT_PTR DropSettleTimerId() {
    return reinterpret_cast<UINT_PTR>(&g_dropSettleWindow);
}
thread_local bool g_cancelingNativePress = false; // Each taskbar UI thread.

void KeepDraggedSlot(FrameworkElement const& element,
                     winrt::Windows::Foundation::Rect& rect) {
    if (g_sectionGesture && g_sectionGesture->dragged &&
        g_sectionGesture->source.get() == element) {
        // Neighbours get their new slots, but the held button's layout slot
        // must not move underneath its pointer-relative visual offset.
        rect.X = g_sectionGesture->originalSlotX;
    }
}

double DragTranslationX(double originalTranslation, double startX, double desiredX) {
    return originalTranslation + desiredX - startX;
}

void UpdateDragPreview() {
    if (!g_sectionGesture || !g_sectionGesture->dragged) return;
    auto source = g_sectionGesture->source.get();
    if (!source) return;
    auto translation = g_sectionGesture->translation;
    translation.x = static_cast<float>(DragTranslationX(translation.x,
        g_sectionGesture->startX, g_sectionGesture->desiredX));
    // Absolute displacement from the frozen slot. Never integrate the
    // current animated visual position back into the next frame.
    source.Translation(translation);
}

void RestoreGestureVisual(SectionGesture const& gesture) {
    ScopedTaskbarContext ownerContext(gesture.window);
    if (auto source = gesture.source.get()) {
        if (gesture.previewPrepared) {
            try {
                // XAML Transitions alone do not disable Composition's
                // reposition animations. Remove the drag offset without an
                // animated return to the former slot.
                auto visual = Hosting::ElementCompositionPreview::GetElementVisual(source);
                auto implicitAnimations = visual.ImplicitAnimations();
                struct RestoreImplicitAnimations {
                    winrt::Windows::UI::Composition::Visual visual;
                    winrt::Windows::UI::Composition::ImplicitAnimationCollection animations;
                    ~RestoreImplicitAnimations() {
                        try { visual.ImplicitAnimations(animations); } catch (...) {}
                    }
                } restoreImplicit{visual, implicitAnimations};
                visual.ImplicitAnimations(nullptr);
                visual.StopAnimation(L"Offset");
                // Do not stop a "Translation" composition property here: the
                // hand-out visual has none, so that call throws. Callers
                // request the next layout pass themselves.
                source.Translation(gesture.translation);
            } catch (...) {
                try { source.Translation(gesture.translation); } catch (...) {}
            }
            try { source.Transitions(gesture.transitions); } catch (...) {}
        }
        if (gesture.ownsPointerCapture) {
            try { source.ReleasePointerCapture(gesture.pointer); } catch (...) {}
        }
    }
}

void FlushPendingDrop() {
    if (g_dropSettleWindow) KillTimer(g_dropSettleWindow, DropSettleTimerId());
    g_dropSettleWindow = nullptr;
    if (!g_pendingDrop) return;
    auto pending = std::move(*g_pendingDrop);
    g_pendingDrop.reset();
    ScopedTaskbarContext pendingContext(pending.gesture.window);
    try { RestoreGestureVisual(pending.gesture); } catch (...) {}
    try {
        if (auto source = pending.gesture.source.get()) {
            Hosting::ElementCompositionPreview::GetElementVisual(source)
                .ImplicitAnimations(pending.implicitAnimations);
        }
    } catch (...) {}
}

void CompletePendingDrop(FrameworkElement const& element) {
    if (!g_pendingDrop || g_pendingDrop->gesture.source.get() != element) return;
    // Called only AFTER this button's real, successful native Arrange with
    // the final split-layout rect. An invalidation request is not sufficient.
    auto& pending = *g_pendingDrop;
    pending.arranged = true;
    try {
        // XAML owns the hand-out visual's Offset. Writing it here left real
        // taskbar buttons drawn away from their layout slot, so only stop a
        // running reposition animation.
        Hosting::ElementCompositionPreview::GetElementVisual(element)
            .StopAnimation(L"Offset");
    } catch (...) {}
    try { element.Translation(pending.gesture.translation); } catch (...) {}
    // Keep transitions disabled until the pending drop has settled.
}

void SettlePendingDrop() {
    if (!g_pendingDrop) { FlushPendingDrop(); return; }
    try {
        auto source = g_pendingDrop->gesture.source.get();
        if (!source || g_unloading) { FlushPendingDrop(); return; }
        ULONGLONG elapsed = GetTickCount64() - g_pendingDrop->started;
        if (g_pendingDrop->arranged) {
            double position = source.ActualOffset().x;
            if (std::abs(position - g_pendingDrop->lastX) > 0.05) {
                g_pendingDrop->lastX = position;
                g_pendingDrop->stableSamples = 0;
            } else {
                ++g_pendingDrop->stableSamples;
            }
            // Bounded guard covering the observed delayed transition, not
            // a sleep on Explorer's UI thread. New presses/unload flush it.
            if (elapsed >= 750 && g_pendingDrop->stableSamples >= 3) {
                FlushPendingDrop();
                return;
            }
        }
        if (elapsed >= 2000) {
            Wh_Log(L"Drop did not settle in 2 seconds; restoring animation settings "
                   L"(arranged=%d stableSamples=%u)",
                   int(g_pendingDrop->arranged), g_pendingDrop->stableSamples);
            FlushPendingDrop();
        }
    } catch (...) { FlushPendingDrop(); }
}

void QueueDrop(SectionGesture gesture) {
    FlushPendingDrop();
    auto source = gesture.source.get();
    if (!source) return;
    try {
        auto visual = Hosting::ElementCompositionPreview::GetElementVisual(source);
        auto implicitAnimations = visual.ImplicitAnimations();
        g_pendingDrop.emplace(PendingDrop{gesture, implicitAnimations});
        g_pendingDrop->started = GetTickCount64();
        visual.ImplicitAnimations(nullptr);
        g_dropSettleWindow = gesture.window;
        if (!g_dropSettleWindow ||
            !SetTimer(g_dropSettleWindow, DropSettleTimerId(), 16, nullptr)) {
            FlushPendingDrop();
            return;
        }
        // Keep Translation and disabled XAML transitions intact until the
        // Arrange callback commits the destination.
        if (gesture.ownsPointerCapture) {
            source.ReleasePointerCapture(gesture.pointer);
            g_pendingDrop->gesture.ownsPointerCapture = false;
        }
        RequestRefresh();
    } catch (...) {
        if (g_pendingDrop) FlushPendingDrop();
        else RestoreGestureVisual(gesture);
    }
}

void CancelSectionGesture() {
    if (!g_sectionGesture) return;
    auto gesture = std::move(*g_sectionGesture);
    g_sectionGesture.reset(); // CaptureLost can re-enter below.
    ScopedTaskbarContext gestureContext(gesture.window);
    try {
        if (gesture.dragged) {
            (gesture.leftSide ? Ui().leftLive : Ui().rightLive) = gesture.originalOrder;
            (gesture.leftSide ? Ui().leftAppOrder : Ui().rightAppOrder) = gesture.originalAppOrder;
        }
        RestoreGestureVisual(gesture);
        RequestRefresh();
    } catch (...) {
    }
}

template<typename T>
T GestureInterface(void* value) {
    T result{nullptr};
    if (value) {
        reinterpret_cast<::IUnknown*>(value)->QueryInterface(
            winrt::guid_of<T>(), winrt::put_abi(result));
    }
    return result;
}

bool GestureMatches(FrameworkElement const& source,
                    Input::PointerRoutedEventArgs const& args) {
    return g_sectionGesture && source && args &&
           g_sectionGesture->source.get() == source &&
           g_sectionGesture->pointerId == args.Pointer().PointerId();
}

template<typename T>
bool ReorderWithinSection(std::vector<T>& order, T const& source,
                          T const& target, bool after) {
    // A target outside this section is rejected without changing any order.
    auto from = std::find(order.begin(), order.end(), source);
    auto to = std::find(order.begin(), order.end(), target);
    if (source == target || from == order.end() || to == order.end()) return false;
    order.erase(from);
    to = std::find(order.begin(), order.end(), target);
    order.insert(after ? std::next(to) : to, source);
    return true;
}

size_t DragDestination(std::vector<double> const& widths, size_t current,
                       double relativeCentre) {
    size_t target = current;
    double x = 0;
    for (size_t i = 0; i < widths.size(); ++i) {
        double midpoint = x + widths[i] / 2;
        if (i < current && relativeCentre <= midpoint) return i;
        if (i > current && relativeCentre >= midpoint) target = i;
        x += widths[i];
    }
    return target;
}

size_t PinnedDragDestination(std::vector<double> const& widths, size_t current,
                            double relativeCentre) {
    if (current >= widths.size()) return current;
    // Choose the nearest SLOT, not an exact crossing of the neighbour's
    // centre. At a clamped section edge the latter may be unreachable due
    // to float XAML coordinates versus double fractional scaled widths.
    double currentCentre = widths[current] / 2;
    for (size_t i = 0; i < current; ++i) currentCentre += widths[i];
    size_t target = current;
    double bestDistance = std::abs(relativeCentre - currentCentre);
    double x = 0;
    for (size_t i = 0; i < widths.size(); ++i) {
        double distance = std::abs(relativeCentre - (x + widths[i] / 2));
        // A quarter DIP dead band prevents switching back and forth at ties.
        if (distance + 0.25 < bestDistance) {
            bestDistance = distance;
            target = i;
        }
        x += widths[i];
    }
    return target;
}

// [block reorder]
// Position, among all block orders, whose left edge is nearest to the dragged
// block. Candidate positions depend only on the other blocks, so a reorder does
// not change them and the result cannot oscillate.
size_t BlockDestination(std::vector<double> const& blockWidths, size_t current,
                        double movedLeft) {
    if (current >= blockWidths.size()) return current;
    double currentLeft = 0;
    for (size_t i = 0; i < current; ++i) currentLeft += blockWidths[i];
    size_t target = current;
    double best = std::abs(movedLeft - currentLeft);
    double left = 0;
    size_t position = 0;
    for (size_t i = 0;; ++i) {
        if (i == current) continue;
        // A quarter DIP dead band prevents switching back and forth at ties.
        double distance = std::abs(movedLeft - left);
        if (distance + 0.25 < best) {
            best = distance;
            target = position;
        }
        if (i >= blockWidths.size()) break;
        left += blockWidths[i];
        ++position;
    }
    return target;
}

// Sizes of the runs of adjacent buttons which share one identity.
std::vector<size_t> AdjacentRuns(std::vector<std::wstring> const& keys) {
    std::vector<size_t> runs;
    for (size_t i = 0; i < keys.size(); ++i) {
        if (i > 0 && !keys[i].empty() && keys[i] == keys[i - 1]) ++runs.back();
        else runs.push_back(1);
    }
    return runs;
}

// Move the whole run containing |source| to |dragLeft| (the dragged button's
// left edge, relative to the zone). Returns false if nothing moves.
bool PlanBlockMove(std::vector<size_t> const& runs,
                   std::vector<double> const& widths, size_t source,
                   double dragLeft, std::vector<size_t>& newOrder) {
    std::vector<double> blockWidths;
    std::vector<size_t> blockStart;
    size_t index = 0, moved = runs.size();
    double offsetInBlock = 0;
    for (size_t block = 0; block < runs.size(); ++block) {
        blockStart.push_back(index);
        double total = 0;
        for (size_t i = 0; i < runs[block]; ++i, ++index) {
            if (index >= widths.size()) return false;
            if (index == source) {
                moved = block;
                offsetInBlock = total;
            }
            total += widths[index];
        }
        blockWidths.push_back(total);
    }
    if (moved == runs.size()) return false;
    size_t target = BlockDestination(blockWidths, moved,
                                     dragLeft - offsetInBlock);
    if (target == moved) return false;
    std::vector<size_t> blocks;
    for (size_t block = 0; block < runs.size(); ++block)
        if (block != moved) blocks.push_back(block);
    blocks.insert(blocks.begin() + target, moved);
    newOrder.clear();
    for (size_t block : blocks)
        for (size_t i = 0; i < runs[block]; ++i)
            newOrder.push_back(blockStart[block] + i);
    return true;
}
// [end block reorder]

void FinishSectionReorder(SectionGesture& gesture,
                          Input::PointerRoutedEventArgs const&) {
    auto source = gesture.source.get();
    auto repeater = GetTaskbarRepeater();
    if (!source || !repeater) return;
    if (ButtonIsLeftSide(source) != gesture.leftSide) {
        return;
    }
    // Use section slots instead of hit-testing animated rectangles.
    auto& order = gesture.leftSide ? Ui().leftLive : Ui().rightLive;
    std::vector<FrameworkElement> live;
    std::vector<double> widths;
    for (auto const& reference : order) {
        auto element = reference.get();
        if (!element || element.Visibility() != Visibility::Visible ||
            ButtonIsLeftSide(element) != gesture.leftSide) continue;
        live.push_back(element);
        // Each cell is a slot plus the gap which follows it.
        widths.push_back(SlotWidth(element) + gesture.sectionGap);
    }
    auto from = std::find(live.begin(), live.end(), source);
    if (from == live.end()) return;
    size_t current = static_cast<size_t>(from - live.begin());
    double centre = gesture.desiredX + gesture.width / 2;
    // Do not modify the source order while the icon crosses the empty gap.
    if (centre < gesture.sectionStart - 2 || centre > gesture.sectionEnd + 2)
        return;
    double relativeCentre = centre - gesture.sectionStart + gesture.sectionGap / 2;
    std::vector<std::wstring> keys;
    for (auto const& element : live) keys.push_back(ButtonKey(element));
    auto runs = AdjacentRuns(keys);
    if (runs.size() == live.size()) {
        size_t targetIndex = gesture.leftSide
            ? DragDestination(widths, current, relativeCentre)
            : PinnedDragDestination(widths, current, relativeCentre);
        if (targetIndex == current) {
            return;
        }
        auto target = live[targetIndex];
        if (!ReorderWithinSection(live, source, target,
                                  targetIndex > current)) {
            return;
        }
    } else {
        // Moving one window past another app must move all of its windows,
        // otherwise the next layout pass would immediately regroup them.
        std::vector<size_t> newOrder;
        if (!PlanBlockMove(runs, widths, current,
                           gesture.desiredX - gesture.sectionStart, newOrder))
            return;
        std::vector<FrameworkElement> reordered;
        for (size_t index : newOrder) reordered.push_back(live[index]);
        live.swap(reordered);
    }
    std::vector<winrt::weak_ref<FrameworkElement>> nextOrder;
    for (auto const& element : live) nextOrder.emplace_back(element);
    std::vector<std::wstring> visibleIds;
    for (auto const& element : live) visibleIds.push_back(ButtonKey(element));
    MergeVisibleAppOrder(gesture.leftSide ? Ui().leftAppOrder : Ui().rightAppOrder, visibleIds);
    order.swap(nextOrder);
    RequestRefresh();
}

using PointerHandler_t = HRESULT(WINAPI*)(void*, void*);
PointerHandler_t PointerPressed_Original = nullptr;
PointerHandler_t PointerMoved_Original = nullptr;
PointerHandler_t PointerReleased_Original = nullptr;
PointerHandler_t PointerCaptureLost_Original = nullptr;
PointerHandler_t PointerCanceled_Original = nullptr;

bool IsGestureThread() {
    return g_sectionGesture && IsTaskbarWindow(g_sectionGesture->window) &&
           GetWindowThreadProcessId(g_sectionGesture->window, nullptr) ==
               GetCurrentThreadId();
}

HRESULT WINAPI PointerPressed_Hook(void* self, void* rawArgs) {
    if (g_unloading) return PointerPressed_Original(self, rawArgs);
    if (!g_settings.sectionDragging.load()) {
        CancelSectionGesture();
        FlushPendingDrop();
        return PointerPressed_Original(self, rawArgs);
    }
    FlushPendingDrop();
    // An unusual missing Release/CaptureLost must not orphan translations.
    if (g_sectionGesture) CancelSectionGesture();
    // Windows receives the original press at the original time, exactly once.
    HRESULT result = PointerPressed_Original(self, rawArgs);
    if (FAILED(result)) return result;
    try {
        auto source = GestureInterface<FrameworkElement>(self);
        auto args = GestureInterface<Input::PointerRoutedEventArgs>(rawArgs);
        if (source && args && IsTaskButton(source) &&
            args.Pointer().PointerDeviceType() ==
                winrt::Windows::Devices::Input::PointerDeviceType::Mouse) {
            auto props = args.GetCurrentPoint(source).Properties();
            HWND owner = FindTaskbarWindowForElement(source);
            if (!owner || !EnsureTaskbarSubclass(owner)) return result;
            ScopedTaskbarContext taskbarContext(owner);
            auto repeater = GetTaskbarRepeater();
            if (!repeater) return result;
            LoadAppOrders();
            if (!Ui().layoutValid) return result;
            if (props.IsLeftButtonPressed() &&
                !props.IsRightButtonPressed() && !props.IsMiddleButtonPressed()) {
                SectionGesture gesture;
                gesture.source = source;
                gesture.pointer = args.Pointer();
                gesture.origin = args.GetCurrentPoint(repeater).Position();
                gesture.appId = ButtonKey(source);
                if (gesture.appId.empty()) return result;
                gesture.window = owner;
                gesture.stateRevision = Ui().stateRevision;
                gesture.geometryEpoch = Ui().geometryEpoch;
                gesture.configEpoch = g_configEpoch.load();
                gesture.leftSide = ButtonIsLeftSide(source);
                gesture.sectionStart = gesture.leftSide ? Ui().leftStart : Ui().rightStart;
                gesture.sectionEnd = gesture.leftSide ? Ui().leftEnd : Ui().rightEnd;
                gesture.switchThreshold = (Ui().leftEnd + Ui().rightStart) / 2;
                if (!std::isfinite(gesture.switchThreshold) ||
                    gesture.sectionEnd < gesture.sectionStart) return result;
                for (auto const& slot : Ui().slots)
                    if (slot.left != gesture.leftSide) gesture.frozenTargetSlots.push_back(slot);
                gesture.originalAppOrder = gesture.leftSide ? Ui().leftAppOrder : Ui().rightAppOrder;
                gesture.pointerId = args.Pointer().PointerId();
                gesture.startX = gesture.desiredX = ElementX(source, repeater);
                gesture.translation = source.Translation();
                gesture.originalSlotX = Controls::Primitives::LayoutInformation::GetLayoutSlot(source).X;
                gesture.transitions = source.Transitions();
                gesture.width = SlotWidth(source);
                gesture.sectionGap = gesture.leftSide ? Ui().leftGap : Ui().rightGap;
                gesture.minX = gesture.startX;
                double right = gesture.startX + gesture.width;
                gesture.originalOrder = gesture.leftSide ? Ui().leftLive : Ui().rightLive;
                for (auto const& reference : gesture.originalOrder) {
                    if (auto item = reference.get()) {
                        double x = ElementX(item, repeater);
                        gesture.minX = std::min(gesture.minX, x);
                        right = std::max(right, x + SlotWidth(item));
                    }
                }
                gesture.maxX = std::max(gesture.minX, right - gesture.width);
                // Allow dragging to either zone while retaining native
                // pointer capture and animation safeguards.
                try {
                    auto content = repeater.XamlRoot().Content().try_as<FrameworkElement>();
                    if (content) {
                        // Use the same verified reservations as the layout.
                        // Even an empty right zone must avoid widgets inserted
                        // between this press and the eventual drop.
                        double farRight = Ui().rightEnd;
                        double candidateRight = TrayEdgeForContent(content, content) -
                                                g_settings.trayGap.load();
                        double unusedLeft = 0;
                        if (ResolveAvailableZoneBounds(0, gesture.width,
                                EffectiveExternalSpans(content), unusedLeft, candidateRight,
                                6.0, std::max(6.0, double(g_settings.trayGap.load())))) {
                            farRight = std::min(farRight,
                                candidateRight - ElementX(repeater, content));
                        }
                        gesture.minX = std::min(gesture.minX, Ui().leftStart);
                        if (std::isfinite(farRight)) {
                            gesture.maxX = std::max(gesture.maxX,
                                farRight - gesture.width);
                        }
                    }
                } catch (...) {
                    // Keep the within-group drag range if geometry is unavailable.
                }
                g_sectionGesture.emplace(std::move(gesture));
            }
        }
    } catch (...) {
        CancelSectionGesture();
    }
    return result;
}

HRESULT WINAPI PointerMoved_Hook(void* self, void* rawArgs) {
    if (!IsGestureThread()) return PointerMoved_Original(self, rawArgs);
    ScopedTaskbarContext taskbarContext(g_sectionGesture->window);
    try {
        auto source = GestureInterface<FrameworkElement>(self);
        auto args = GestureInterface<Input::PointerRoutedEventArgs>(rawArgs);
        if (GestureMatches(source, args)) {
            args.Handled(true);
            auto repeater = GetTaskbarRepeater();
            if (g_unloading || !repeater || ButtonKey(source) != g_sectionGesture->appId ||
                !args.GetCurrentPoint(source).Properties().IsLeftButtonPressed() ||
                (GetAsyncKeyState(VK_ESCAPE) & 0x8000)) {
                CancelSectionGesture();
                return S_OK;
            }
            auto position = args.GetCurrentPoint(repeater).Position();
            // Coordinates are DIPs; use a small DPI-independent dead zone.
            if (!g_sectionGesture->dragged &&
                std::abs(position.X - g_sectionGesture->origin.X) >= 5) {
                g_sectionGesture->dragged = true;
                g_sectionGesture->previewPrepared = true;
                // Only the held button loses reposition transitions; its
                // neighbours remain free to animate into their new slots.
                source.Transitions(media::Animation::TransitionCollection());
                struct NativeCancelGuard {
                    NativeCancelGuard() { g_cancelingNativePress = true; }
                    ~NativeCancelGuard() { g_cancelingNativePress = false; }
                } guard;
                // End IsPressed/native capture before owning the gesture.
                PointerCanceled_Original(self, rawArgs);
                if (!source.CapturePointer(args.Pointer())) {
                    CancelSectionGesture();
                    return S_OK;
                }
                g_sectionGesture->ownsPointerCapture = true;
            }
            if (g_sectionGesture && g_sectionGesture->dragged) {
                g_sectionGesture->desiredX = std::clamp(
                    g_sectionGesture->startX + position.X - g_sectionGesture->origin.X,
                    g_sectionGesture->minX, g_sectionGesture->maxX);
                FinishSectionReorder(*g_sectionGesture, args);
                UpdateDragPreview();
            }
            return S_OK;
        }
    } catch (...) {
        if (g_sectionGesture) { CancelSectionGesture(); return S_OK; }
    }
    return PointerMoved_Original(self, rawArgs);
}

// The target insertion point is derived from frozen XAML layout slots,
// never from animated ElementX positions during a drag.
void InsertAppAtDrop(std::vector<std::wstring>& order,
                     std::wstring const& id,
                     std::vector<TaskbarSlot> slots,
                     double centre) {
    order.erase(std::remove(order.begin(), order.end(), id), order.end());
    std::sort(slots.begin(), slots.end(), [](auto const& a, auto const& b) {
        return a.x < b.x;
    });
    // Treat an app's adjacent windows as one target.
    std::vector<TaskbarSlot> targets;
    for (auto const& slot : slots) {
        if (!targets.empty() && targets.back().appId == slot.appId)
            targets.back().width = slot.x + slot.width - targets.back().x;
        else
            targets.push_back(slot);
    }
    for (auto const& slot : targets) {
        if (slot.appId == id) continue;
        if (centre < slot.x + slot.width / 2) {
            auto target = std::find(order.begin(), order.end(), slot.appId);
            if (target != order.end()) {
                if (order.size() < 512) order.insert(target, id);
                return;
            }
        }
    }
    RememberApp(order, id);
}

bool CommitFixedSideDrop(SectionGesture const& gesture) {
    // The pointer may be released before the next 400 ms poll. Re-sample
    // external widget bounds so a move/resize cannot commit a stale zone.
    PollBoundaryGeometry(gesture.window);
    if (gesture.appId.empty() || !IsTaskbarWindow(gesture.window) ||
        !Ui().layoutValid || Ui().geometryEpoch != gesture.geometryEpoch ||
        gesture.configEpoch != g_configEpoch.load())
        return false;
    auto repeater = GetTaskbarRepeater();
    if (!repeater || !gesture.source.get() ||
        GetWindowThreadProcessId(gesture.window, nullptr) != GetCurrentThreadId())
        return false;

    double centre = gesture.desiredX + gesture.width / 2;
    // A 6-DIP dead band avoids switching sides accidentally near the centre.
    bool moveRight = gesture.leftSide
        ? centre >= gesture.switchThreshold + 6
        : centre > gesture.switchThreshold - 6;
    bool changingSide = moveRight == gesture.leftSide;
    bool committed = false;
    {
        std::lock_guard<std::mutex> lock(g_storeMutex);
        EnsureSharedStateLoadedLocked();
        // If another monitor changed the layout during this gesture, discard
        // this stale drag instead of overwriting newer saved preferences.
        if (gesture.stateRevision != g_sharedRevision) return false;
        PersistedAppState next = g_sharedState;
        auto& originalSide = gesture.leftSide ? next.leftOrder : next.rightOrder;
        auto& localLive = gesture.leftSide ? Ui().leftLive : Ui().rightLive;
        std::vector<std::wstring> visibleIds;
        for (auto const& weak : localLive) {
            auto item = weak.get();
            if (item && item.Visibility() == Visibility::Visible &&
                ButtonIsLeftSide(item) == gesture.leftSide)
                RememberApp(visibleIds, ButtonKey(item));
        }
        MergeVisibleAppOrder(originalSide, visibleIds);

        if (changingSide) {
            // One AppID, one side, even if several windows share this AppID.
            next.leftOrder.erase(std::remove(next.leftOrder.begin(),
                next.leftOrder.end(), gesture.appId), next.leftOrder.end());
            next.rightOrder.erase(std::remove(next.rightOrder.begin(),
                next.rightOrder.end(), gesture.appId), next.rightOrder.end());
            next.rightIds.erase(std::remove(next.rightIds.begin(),
                next.rightIds.end(), gesture.appId), next.rightIds.end());
            if (moveRight) {
                RememberApp(next.rightIds, gesture.appId);
                InsertAppAtDrop(next.rightOrder, gesture.appId,
                    gesture.frozenTargetSlots, centre);
            } else {
                InsertAppAtDrop(next.leftOrder, gesture.appId,
                    gesture.frozenTargetSlots, centre);
            }
            // Never evict existing preferences or silently drop the new rank.
            auto const& target = moveRight ? next.rightOrder : next.leftOrder;
            if (std::find(target.begin(), target.end(), gesture.appId) == target.end())
                return false;
            if (moveRight && std::find(next.rightIds.begin(),
                    next.rightIds.end(), gesture.appId) == next.rightIds.end())
                return false;
        }
        committed = SaveStateLocked(next);
    }
    if (!committed) return false;
    LoadAppOrders();
    RequestRefreshAll();
    return true;
}


HRESULT WINAPI PointerReleased_Hook(void* self, void* rawArgs) {
    if (!IsGestureThread()) return PointerReleased_Original(self, rawArgs);
    ScopedTaskbarContext taskbarContext(g_sectionGesture->window);
    try {
        auto source = GestureInterface<FrameworkElement>(self);
        auto args = GestureInterface<Input::PointerRoutedEventArgs>(rawArgs);
        if (GestureMatches(source, args)) {
            if (!g_sectionGesture->dragged) {
                g_sectionGesture.reset();
                // Do not release native capture or change Handled before
                // Windows processes the real release of a normal click.
                return PointerReleased_Original(self, rawArgs);
            }
            auto gesture = std::move(*g_sectionGesture);
            g_sectionGesture.reset();
            args.Handled(true);
            if (g_unloading || ButtonKey(source) != gesture.appId || (GetAsyncKeyState(VK_ESCAPE) & 0x8000)) {
                (gesture.leftSide ? Ui().leftLive : Ui().rightLive) = gesture.originalOrder;
                (gesture.leftSide ? Ui().leftAppOrder : Ui().rightAppOrder) = gesture.originalAppOrder;
                RestoreGestureVisual(gesture);
                RequestRefresh();
            } else {
                bool saved = false;
                try { saved = CommitFixedSideDrop(gesture); }
                catch (...) { Wh_Log(L"Could not commit app side/order"); }
                if (saved) {
                    QueueDrop(std::move(gesture));
                } else {
                    (gesture.leftSide ? Ui().leftLive : Ui().rightLive) = gesture.originalOrder;
                    (gesture.leftSide ? Ui().leftAppOrder : Ui().rightAppOrder) =
                        gesture.originalAppOrder;
                    RestoreGestureVisual(gesture);
                    RequestRefresh();
                }
            }
            return S_OK; // Native press was canceled at drag start.
        }
    } catch (...) {
        CancelSectionGesture();
        return S_OK; // Never turn a failed drag into an accidental click.
    }
    return PointerReleased_Original(self, rawArgs);
}

HRESULT WINAPI PointerCaptureLost_Hook(void* self, void* rawArgs) {
    if (!IsGestureThread()) return PointerCaptureLost_Original(self, rawArgs);
    ScopedTaskbarContext taskbarContext(g_sectionGesture->window);
    try {
        if (!g_cancelingNativePress && GestureMatches(GestureInterface<FrameworkElement>(self),
                           GestureInterface<Input::PointerRoutedEventArgs>(rawArgs))) {
            CancelSectionGesture();
        }
    } catch (...) {
    }
    return PointerCaptureLost_Original(self, rawArgs);
}

HRESULT WINAPI PointerCanceled_Hook(void* self, void* rawArgs) {
    if (!IsGestureThread()) return PointerCanceled_Original(self, rawArgs);
    ScopedTaskbarContext taskbarContext(g_sectionGesture->window);
    try {
        if (GestureMatches(GestureInterface<FrameworkElement>(self),
                           GestureInterface<Input::PointerRoutedEventArgs>(rawArgs))) {
            CancelSectionGesture();
        }
    } catch (...) {
    }
    return PointerCanceled_Original(self, rawArgs);
}

UINT RefreshMessage() {
    static UINT value =
        RegisterWindowMessageW(L"Windhawk_TaskbarZones_Refresh_" WH_MOD_ID);
    return value;
}

UINT RestoreMessage() {
    static UINT value =
        RegisterWindowMessageW(L"Windhawk_TaskbarZones_Restore_" WH_MOD_ID);
    return value;
}

UINT StorageKickMessage() {
    static UINT value = RegisterWindowMessageW(
        L"Windhawk_TaskbarZones_StorageKick_" WH_MOD_ID);
    return value;
}

UINT_PTR StorageTimerId() {
    static int tag;
    return reinterpret_cast<UINT_PTR>(&tag);
}

void RequestStorageRetry() {
    // Messages are posted, never sent synchronously between taskbar threads.
    for (HWND window : AllTaskbarWindows()) {
        if (IsTaskbarWindow(window)) PostMessageW(window, StorageKickMessage(), 0, 0);
    }
}

bool IsTaskbarWindow(HWND window) {
    if (!window || !IsWindow(window)) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    if (pid != GetCurrentProcessId()) return false;
    wchar_t name[64]{};
    GetClassNameW(window, name, ARRAYSIZE(name));
    return wcscmp(name, L"Shell_TrayWnd") == 0 ||
           wcscmp(name, L"Shell_SecondaryTrayWnd") == 0;
}

std::vector<HWND> AllTaskbarWindows() {
    std::vector<HWND> windows;
    EnumWindows([](HWND window, LPARAM data) -> BOOL {
        if (IsTaskbarWindow(window))
            reinterpret_cast<std::vector<HWND>*>(data)->push_back(window);
        return TRUE;
    }, reinterpret_cast<LPARAM>(&windows));
    return windows;
}

LRESULT CALLBACK TaskbarSubclassProc(HWND, UINT, WPARAM, LPARAM, DWORD_PTR);

bool EnsureTaskbarSubclass(HWND window) {
    if (g_unloading || !IsTaskbarWindow(window)) return false;
    {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        if (g_subclassedTaskbars.count(window)) return true;
    }
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
            window, TaskbarSubclassProc, 0)) return false;
    // Serialize the final unload check and insertion with the unload snapshot.
    // Otherwise a late subclass could be installed after the snapshot.
    bool removeAfterInstall = false;
    {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        if (g_unloading) removeAfterInstall = true;
        else g_subclassedTaskbars.insert(window);
    }
    if (removeAfterInstall) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(window, TaskbarSubclassProc);
        return false;
    }
    Wh_Log(L"Attached to %ls taskbar (HWND %p)",
        FindWindowW(L"Shell_TrayWnd", nullptr) == window ? L"primary" : L"secondary",
        window);
    return true;
}

void RequestRefreshFor(HWND window) {
    if (g_unloading || !IsTaskbarWindow(window)) return;
    {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        if (!g_subclassedTaskbars.count(window) ||
            !g_refreshPending.insert(window).second) return;
    }
    if (!PostMessageW(window, RefreshMessage(), 0, 0)) {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        g_refreshPending.erase(window);
    }
}

void RequestRefreshAll() {
    if (g_unloading) return;
    for (HWND window : AllTaskbarWindows()) RequestRefreshFor(window);
}

void RequestRefresh() {
    if (g_activeTaskbarWindow) RequestRefreshFor(g_activeTaskbarWindow);
    else RequestRefreshAll();
}

// Works on a secondary taskbar too; the source element identifies its owner.
struct CachedOwner {
    winrt::weak_ref<FrameworkElement> element;
    HWND hwnd = nullptr;
};
[[clang::no_destroy]] thread_local std::unordered_map<void*, CachedOwner> owners;
void ClearCachedOwnersForWindow(HWND window) {
    for (auto it = owners.begin(); it != owners.end();) {
        if (it->second.hwnd == window || !it->second.element.get())
            it = owners.erase(it);
        else ++it;
    }
}
HWND FindTaskbarWindowForElement(FrameworkElement const& element) {
    if (!element) return nullptr;
    void* key = winrt::get_abi(element);
    if (auto it = owners.find(key); it != owners.end()) {
        HWND cachedWindow = it->second.hwnd;
        bool valid = it->second.element.get() == element &&
            IsTaskbarWindow(cachedWindow) &&
            GetWindowThreadProcessId(cachedWindow, nullptr) == GetCurrentThreadId();
        if (valid) {
            ScopedTaskbarContext currentContext(cachedWindow);
            auto repeater = GetTaskbarRepeater();
            valid = repeater && element.XamlRoot() == repeater.XamlRoot();
            if (valid) {
                // XamlRoot equality alone is insufficient after a container
                // was detached: prove the item still belongs to this repeater.
                valid = false;
                auto parent = element.as<DependencyObject>();
                for (int d = 0; parent && d < 12; ++d) {
                    if (parent == repeater) { valid = true; break; }
                    parent = media::VisualTreeHelper::GetParent(parent);
                }
            }
        }
        if (valid) return cachedWindow;
        owners.erase(it);
    }
    if (owners.size() > 512) owners.clear();
    for (HWND window : AllTaskbarWindows()) {
        if (GetWindowThreadProcessId(window, nullptr) != GetCurrentThreadId())
            continue;
        try {
            auto root = TaskbarXamlRoot(window);
            auto content = root ? root.Content().try_as<FrameworkElement>() : nullptr;
            auto repeater = content ? FindTaskbarRepeater(content, window) : nullptr;
            if (!repeater) continue;
            for (auto const& candidate : RepeaterElements(repeater)) {
                if (candidate == element) {
                    owners[key] = CachedOwner{element, window};
                    return window;
                }
            }
        } catch (...) { /* A disappearing taskbar is not an Explorer failure. */ }
    }
    return nullptr;
}

// Must run on the taskbar's owning UI thread: all these caches are thread-local.
// Destruction on the Windhawk worker thread would leave the UI copies behind.
void ReleaseTaskbarUiCaches(HWND window) {
    g_uiStates.erase(window);
    g_repeaterByWindow.erase(window);
    RemoveBoundaryWatch(window);
    g_traySearch.erase(window);
    ClearCachedOwnersForWindow(window);
    for (auto it = g_layoutOwners.begin(); it != g_layoutOwners.end();) {
        if (it->second.window == window) it = g_layoutOwners.erase(it);
        else ++it;
    }
    if (g_widgetsWindow == window || g_unloading) {
        g_widgetsCache = nullptr;
        g_widgetsFrame = nullptr;
        g_widgetsWindow = nullptr;
        g_widgetsNextSearch = 0;
        g_widgetsInvalidated = true;
    }
    // Clear caches only after all taskbars on this UI thread are gone.
    if (!g_unloading || g_insideArrange || g_useArrangePlan ||
        !g_uiStates.empty() || !g_layoutOwners.empty() ||
        !g_repeaterByWindow.empty() || !g_traySearch.empty() ||
        !g_boundaryWatches.empty() || !owners.empty())
        return;
    // swap destroys weak COM references and releases hash buckets on the UI
    // thread, while the DLL is still loaded. clear() alone retains buckets.
    decltype(g_layoutOwners){}.swap(g_layoutOwners);
    decltype(g_repeaterByWindow){}.swap(g_repeaterByWindow);
    decltype(g_traySearch){}.swap(g_traySearch);
    decltype(g_uiStates){}.swap(g_uiStates);
    decltype(g_boundaryWatches){}.swap(g_boundaryWatches);
    decltype(owners){}.swap(owners);
    decltype(g_arrangePlan){}.swap(g_arrangePlan);
}

LRESULT CALLBACK TaskbarSubclassProc(HWND window, UINT message, WPARAM wParam,
                                     LPARAM lParam, DWORD_PTR) {
    ScopedTaskbarContext context(window);
    if (message == WM_TIMER && window == g_dropSettleWindow &&
        wParam == DropSettleTimerId()) {
        SettlePendingDrop();
        return 0;
    }
    if (message == WM_TIMER && wParam == BoundaryPollTimerId()) {
        PollBoundaryGeometry(window);
        return 0;
    }
    if (message == StorageKickMessage()) {
        if (!g_unloading) SetTimer(window, StorageTimerId(), 500, nullptr);
        return 0;
    }
    if (message == WM_TIMER && wParam == StorageTimerId()) {
        if (g_unloading || FlushPendingStorage()) KillTimer(window, StorageTimerId());
        return 0;
    }
    if (message == WM_NCDESTROY) {
        KillTimer(window, StorageTimerId());
        // If this was the last writer, another live monitor can finish saving.
        if (!g_unloading) RequestStorageRetry();
        if (g_sectionGesture && g_sectionGesture->window == window)
            CancelSectionGesture();
        if (g_dropSettleWindow == window) FlushPendingDrop();
        ReleaseTaskbarUiCaches(window);
        {
            std::lock_guard<std::mutex> lock(g_windowsMutex);
            g_subclassedTaskbars.erase(window);
            g_refreshPending.erase(window);
        }
        return DefSubclassProc(window, message, wParam, lParam);
    }
    if (message == RefreshMessage()) {
        {
            std::lock_guard<std::mutex> lock(g_windowsMutex);
            g_refreshPending.erase(window);
        }
        if (!g_unloading) {
            try {
                if (auto repeater = GetTaskbarRepeater())
                    repeater.InvalidateArrange();
            } catch (...) {
                Wh_Log(L"Taskbar refresh failed on monitor (disconnected XAML)");
            }
        }
        return 0;
    }
    if (message == RestoreMessage()) {
        KillTimer(window, StorageTimerId());
        if (g_sectionGesture && g_sectionGesture->window == window)
            CancelSectionGesture();
        if (g_dropSettleWindow == window) FlushPendingDrop();
        bool nativeLayoutUpdated = false;
        try {
            if (auto repeater = GetTaskbarRepeater()) {
                // Return to native Windows layout before unloading.
                repeater.InvalidateArrange();
                repeater.UpdateLayout();
                nativeLayoutUpdated = true;
            }
        } catch (...) {
            Wh_Log(L"Native layout restoration failed on one monitor");
        }
        // Keep the final attempt behavior if native layout restoration fails.
        if (g_unloading && (nativeLayoutUpdated || wParam == 1))
            ReleaseTaskbarUiCaches(window);
        return nativeLayoutUpdated ? 1 : 0;
    }
    if (message == WM_DISPLAYCHANGE || message == WM_DPICHANGED ||
        message == WM_SETTINGCHANGE) {
        g_widgetsInvalidated = true;
        Ui().layoutValid = false;
        ++Ui().geometryEpoch;  // invalidates in-progress drag snapshots
        g_repeaterByWindow.erase(window);
        g_traySearch.erase(window);
        // A previous monitor's widget sizes must not survive a resize,
        // reconnection, or DPI change into the current layout.
        RemoveBoundaryWatch(window);
        // The next arrange pass recreates the watch with fresh geometry.
        // Taskbar windows may be added or removed when displays change.
        // Windows will arrange each new secondary taskbar; the layout hook
        // installs its subclass when that taskbar becomes ready.
        RequestRefreshAll();
    }
    return DefSubclassProc(window, message, wParam, lParam);
}


// Right-click Jump Lists use taskbar.dll's native button-group rectangle,
// which can predate our XAML arrangement. For context-menu requests only,
// substitute the actual bounds of the right-zone button in the same XAML root.
struct JumpListAnchor {
    winrt::Windows::Foundation::Rect bounds;
    HWND taskbar = nullptr;
};
thread_local std::optional<JumpListAnchor> g_jumpListAnchor;
thread_local bool g_computingJumpViewPosition = false;

std::optional<JumpListAnchor> CaptureJumpListAnchor(UIElement const& sender) {
    try {
        auto button = sender.try_as<FrameworkElement>();
        if (!button || !IsTaskButton(button)) return std::nullopt;
        HWND taskbar = FindTaskbarWindowForElement(button);
        if (!taskbar) return std::nullopt;
        ScopedTaskbarContext context(taskbar);
        if (!Ui().layoutValid || ButtonIsLeftSide(button)) return std::nullopt;

        auto root = button.XamlRoot();
        auto content = root ? root.Content() : nullptr;
        if (!content) return std::nullopt;
        auto bounds = button.TransformToVisual(content).TransformBounds(
            winrt::Windows::Foundation::Rect{
                0, 0, static_cast<float>(button.ActualWidth()),
                static_cast<float>(button.ActualHeight())});
        if (!std::isfinite(bounds.X) || !std::isfinite(bounds.Y) ||
            !std::isfinite(bounds.Width) || !std::isfinite(bounds.Height) ||
            bounds.Width <= 0 || bounds.Height <= 0) return std::nullopt;
        return JumpListAnchor{bounds, taskbar};
    } catch (...) {
        return std::nullopt;
    }
}

struct ScopedJumpListAnchor {
    std::optional<JumpListAnchor> previous;
    explicit ScopedJumpListAnchor(UIElement const& sender)
        : previous(g_jumpListAnchor) {
        g_jumpListAnchor = CaptureJumpListAnchor(sender);
    }
    ~ScopedJumpListAnchor() { g_jumpListAnchor = previous; }
};

using TaskbarResources_ContextRequested_t =
    void(WINAPI*)(void*, UIElement const&, void*);
TaskbarResources_ContextRequested_t TaskbarResources_ContextRequested_Original = nullptr;
void WINAPI TaskbarResources_ContextRequested_Hook(
    void* self, UIElement const& sender, void* args) {
    if (g_unloading) return TaskbarResources_ContextRequested_Original(self, sender, args);
    ScopedJumpListAnchor context(sender);
    TaskbarResources_ContextRequested_Original(self, sender, args);
}

using TaskListButtonHandlers_ContextRequested_t =
    void(WINAPI*)(UIElement const&, void*);
TaskListButtonHandlers_ContextRequested_t
    TaskListButtonHandlers_ContextRequested_Original = nullptr;
void WINAPI TaskListButtonHandlers_ContextRequested_Hook(
    UIElement const& sender, void* args) {
    if (g_unloading) return TaskListButtonHandlers_ContextRequested_Original(sender, args);
    ScopedJumpListAnchor context(sender);
    TaskListButtonHandlers_ContextRequested_Original(sender, args);
}

using ComputeJumpViewPosition_t = HRESULT(WINAPI*)(
    void*, void*, int, winrt::Windows::Foundation::Point*,
    HorizontalAlignment*, VerticalAlignment*);
ComputeJumpViewPosition_t ComputeJumpViewPosition_Original = nullptr;
HRESULT WINAPI ComputeJumpViewPosition_Hook(
    void* self, void* group, int mode,
    winrt::Windows::Foundation::Point* point,
    HorizontalAlignment* horizontal, VerticalAlignment* vertical) {
    bool previous = g_computingJumpViewPosition;
    g_computingJumpViewPosition = true;
    struct RestoreFlag {
        bool oldValue;
        ~RestoreFlag() { g_computingJumpViewPosition = oldValue; }
    } restore{previous};
    return ComputeJumpViewPosition_Original(self, group, mode, point,
                                            horizontal, vertical);
}

using ToRawDesktopPixels_t = RECT(WINAPI*)(
    winrt::Windows::Foundation::Rect const&, UINT, HWND);
ToRawDesktopPixels_t ToRawDesktopPixels_Original = nullptr;
RECT WINAPI ToRawDesktopPixels_Hook(
    winrt::Windows::Foundation::Rect const& rect, UINT dpi, HWND window) {
    if (g_unloading || !g_computingJumpViewPosition || !g_jumpListAnchor)
        return ToRawDesktopPixels_Original(rect, dpi, window);

    // Do not transfer one monitor's anchor to a different taskbar.
    HWND root = window ? GetAncestor(window, GA_ROOT) : nullptr;
    if (IsTaskbarWindow(root) && root != g_jumpListAnchor->taskbar)
        return ToRawDesktopPixels_Original(rect, dpi, window);

    auto corrected = rect;
    corrected.X = g_jumpListAnchor->bounds.X;
    corrected.Width = g_jumpListAnchor->bounds.Width;
    return ToRawDesktopPixels_Original(corrected, dpi, window);
}

bool HookTaskbarHostSymbols() {
    HMODULE module =
        LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        return false;
    }
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &CTaskBand_GetTaskbarHost_Original},
        {{LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CSecondaryTaskBand_ITaskListWndSite_vftable, nullptr, true},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
         &CSecondaryTaskBand_GetTaskbarHost_Original, nullptr, true},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
         &TaskbarHost_FrameHeight_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
         &RefCount_Decref_Original},
        {{LR"(protected: long __cdecl CTaskListWnd::_ComputeJumpViewPosition(struct ITaskBtnGroup *,int,struct Windows::Foundation::Point &,enum Windows::UI::Xaml::HorizontalAlignment &,enum Windows::UI::Xaml::VerticalAlignment &)const )"},
         &ComputeJumpViewPosition_Original, ComputeJumpViewPosition_Hook, true},
        {{LR"(struct tagRECT __cdecl ToRawPixelsInDesktopCoordinates(struct winrt::Windows::Foundation::Rect const &,unsigned int,struct HWND__ *))"},
         &ToRawDesktopPixels_Original, ToRawDesktopPixels_Hook, true},
    };
    bool ok = WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                         ARRAYSIZE(taskbarDllHooks));
    if (ok && (!CSecondaryTaskBand_ITaskListWndSite_vftable ||
               !CSecondaryTaskBand_GetTaskbarHost_Original))
        Wh_Log(L"Secondary taskbar symbols unavailable on this Windows build");
    if (ok && (!ComputeJumpViewPosition_Original || !ToRawDesktopPixels_Original))
        Wh_Log(L"Jump List positioning symbols unavailable; native anchor retained");
    return ok;
}

bool HookTaskbarViewSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerPressed(void *))"},
         &PointerPressed_Original, PointerPressed_Hook},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"},
         &PointerMoved_Original, PointerMoved_Hook},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerReleased(void *))"},
         &PointerReleased_Original, PointerReleased_Hook},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerCaptureLost(void *))"},
         &PointerCaptureLost_Original, PointerCaptureLost_Hook},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerCanceled(void *))"},
         &PointerCanceled_Original, PointerCanceled_Hook},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarCollapsibleLayout,struct winrt::Microsoft::UI::Xaml::Controls::IVirtualizingLayoutOverrides>::ArrangeOverride(void *,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
         &ArrangeOverride_Original, ArrangeOverride_Hook},
        {{LR"(public: void __cdecl winrt::Taskbar::implementation::TaskbarResources::OnTaskListButtonContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))"},
         &TaskbarResources_ContextRequested_Original,
         TaskbarResources_ContextRequested_Hook, true},
        {{LR"(public: static void __cdecl winrt::Taskbar::implementation::TaskListButtonHandlers::HandleContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))"},
         &TaskListButtonHandlers_ContextRequested_Original,
         TaskListButtonHandlers_ContextRequested_Hook, true},
    };
    bool result = WindhawkUtils::HookSymbols(
        module, taskbarViewHooks, ARRAYSIZE(taskbarViewHooks));
    if (!result) Wh_Log(L"Private Taskbar.View symbols failed: drag/layout hooks may be unavailable");
    if (result && !TaskbarResources_ContextRequested_Original &&
        !TaskListButtonHandlers_ContextRequested_Original)
        Wh_Log(L"Jump List context symbols unavailable; native anchor retained");
    return result && ArrangeOverride_Original;
}

HMODULE CurrentTaskbarViewModule() {
    if (HMODULE module = GetModuleHandleW(L"Taskbar.View.dll")) {
        return module;
    }
    return GetModuleHandleW(L"ExplorerExtensions.dll");
}

void TryHookLoadedTaskbarView(HMODULE loadedModule) {
    HMODULE module = CurrentTaskbarViewModule();
    if (!module || module != loadedModule || g_viewHooksInstalled ||
        g_viewHooksFailedModule == module ||
        g_viewHooksInProgress.exchange(true)) return;
    bool ok = HookTaskbarViewSymbols(module);
    if (ok) {
        Wh_ApplyHookOperations();
        g_viewHooksInstalled = true;
    } else {
        // Do not apply a partially resolved hook table, and do not
        // repeatedly queue it for the same unsupported DLL build.
        Wh_Log(L"Failed to resolve taskbar symbols; skipping this module");
        g_viewHooksFailedModule = module;
    }
    g_viewHooksInProgress = false;
}


using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);
    if (module) {
        TryHookLoadedTaskbarView(module);
    }
    return module;
}

BOOL Wh_ModInit() {
    LoadSettings();
    if (!HookTaskbarHostSymbols()) {
        Wh_Log(L"failed to hook taskbar.dll");
        return FALSE;
    }
    if (HMODULE module = CurrentTaskbarViewModule()) {
        if (!HookTaskbarViewSymbols(module)) return FALSE;
        g_viewHooksInstalled = true;
    } else {
        HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
        auto loadLibrary = kernelBase
                               ? reinterpret_cast<LoadLibraryExW_t>(
                                     GetProcAddress(kernelBase, "LoadLibraryExW"))
                               : nullptr;
        if (!loadLibrary ||
            !WindhawkUtils::SetFunctionHook(loadLibrary, LoadLibraryExW_Hook,
                                             &LoadLibraryExW_Original)) {
            return FALSE;
        }
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    // No process or window restarts. Register existing primary + secondary
    // windows; future taskbars are detected by the Arrange hook.
    auto windows = AllTaskbarWindows();
    Wh_Log(L"Taskbar discovery: %zu taskbar windows at initialization", windows.size());
    for (HWND window : windows) EnsureTaskbarSubclass(window);
    if (!g_viewHooksInstalled) {
        if (HMODULE module = CurrentTaskbarViewModule())
            TryHookLoadedTaskbarView(module);
    }
    RequestRefreshAll();
    RequestStorageRetry();
}

void Wh_ModBeforeUninit() {
    std::vector<HWND> windows;
    {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        g_unloading = true;
        windows.assign(g_subclassedTaskbars.begin(), g_subclassedTaskbars.end());
    }
    // One final verified storage attempt; no worker join or sleeping.
    FlushStorageBeforeUninit();
    for (HWND window : windows) {
        if (!IsWindow(window)) continue;
        DWORD_PTR acknowledged = 0;
        auto restore = [&](UINT timeout, bool finalAttempt) {
            acknowledged = 0;
            return SendMessageTimeoutW(window, RestoreMessage(),
                finalAttempt ? 1 : 0, 0,
                SMTO_ABORTIFHUNG | SMTO_BLOCK, timeout, &acknowledged) != 0 &&
                acknowledged == 1;
        };
        bool restored = restore(1200, false);
        if (!restored && IsTaskbarWindow(window))
            restored = restore(600, true);
        if (!restored) {
            // Never leave a callback into an unloaded DLL installed: removing
            // the subclass is mandatory, even if a window is unresponsive.
            Wh_Log(L"WARNING: could not restore HWND %p on UI thread before unload", window);
            // A synthetic WM_SIZE with lParam=0 can corrupt tray geometry.
            // Request redraw only; the next native XAML layout owns placement.
            InvalidateRect(window, nullptr, TRUE);
        }
        // Native HWND timers do not hold function pointers, but they should
        // still be canceled even when the UI owner missed RestoreMessage().
        KillTimer(window, StorageTimerId());
        KillTimer(window, BoundaryPollTimerId());
        // RemoveWindowSubclassFromAnyThread returns void.
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(window, TaskbarSubclassProc);
    }
    std::lock_guard<std::mutex> lock(g_windowsMutex);
    g_subclassedTaskbars.clear();
    g_refreshPending.clear();
}


void Wh_ModSettingsChanged() {
    LoadSettings();
    ++g_configEpoch;
    g_widgetsInvalidated = true;
    g_settingsArrangeFollowup = true;
    for (HWND window : AllTaskbarWindows()) EnsureTaskbarSubclass(window);
    RequestRefreshAll();
    RequestStorageRetry();
}
