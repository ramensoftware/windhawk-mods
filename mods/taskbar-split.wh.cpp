// ==WindhawkMod==
// @id              taskbar-split
// @name            Taskbar Split: Running Left, Pinned Right
// @description     Places running apps on the left and closed pinned apps on the right, with flexible empty space between them (Windows 11).
// @version         0.3.18
// @author          Arkadiusz
// @github          https://github.com/Artllex
// @homepage        https://github.com/Artllex/taskbar-split
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject
// @license         GPL-3.0
// ==/WindhawkMod==

// Copyright (c) 2026 Arkadiusz
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
# Taskbar Split

Creates two dynamic application zones on the Windows 11 taskbar:

![Taskbar Split: running applications on the left and closed pinned applications on the right](https://raw.githubusercontent.com/Artllex/taskbar-split/main/assets/taskbar-split.png)

`[Start/System] [Running apps]  <flexible empty space>  [Closed pinned apps] [Tray/Clock]`

Launching a pinned app moves it to the left zone and restores its normal size.
Closing the app returns its pinned icon to the right zone, where icons can be
smaller and packed more densely. Newly running buttons are appended to the right end of the running zone.

Drag with the left mouse button to reorder within a section. The dragged
icon follows the mouse within that section and neighbours make room as it
crosses their centres. Escape cancels the move; sections cannot be crossed.
Left-drag is intentionally handled by this mod to keep reordering inside each
section. It replaces native left-drag for buttons with a recognized Appid.
Disable "Enable section dragging" to use native input instead; section
boundaries are then not enforced during dragging. The split layout stays active.

Per-app order is saved in Windhawk storage after a completed drag and restored
after Explorer restarts; Windows' persistent pin list is not changed. Buttons
without a recognized stable Appid retain native input and are not persisted.
Separate windows sharing one Appid share a rank; their relative window order
is not persisted. Up to 256 app identities per section are remembered.

Native Widgets space is reserved when system buttons retain their Windows positions.
For a left-aligned Start menu, also select Left taskbar alignment in Windows settings.

Targets the horizontal primary taskbar on Windows 11 x64 and ARM64.
Disable the mod to immediately return to the standard Windows layout.

Unlike "Taskbar Start Button Centered Origin", this mod splits by running
versus closed pinned apps, not by window position on the screen.
Do not combine with "Start button always on the left",
"Taskbar Start Button Centered Origin" (taskbar-centered-start-split-icons), or other mods that
reposition or scale taskbar buttons. They can override the same layout.
On crowded taskbars, reduced spacing can overlap buttons; reduce the pinned
icon size or the middle gap, or unpin applications to free space.

Buttons are positioned through their native XAML arrange rectangles. Closed
pinned buttons are additionally scaled, including their hover/focus visuals.
The overflow button follows the running group; Windows still decides which
applications appear in the overflow menu. Secondary taskbars stay unchanged.

This mod keeps Start at the left edge and uses the full space up to the tray
for a launcher/workspace split. Centered Origin instead organizes windows
around a centered Start button. Separate settings keep these distinct layouts
easy to configure; enable only one positioning mod at a time.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- leftPadding: 0
  $name: Left edge padding
  $description: Empty space before the first system button, in device-independent pixels (DIPs).
- runningGap: 0
  $name: Gap after system buttons
  $description: Space between Start/Search/Widgets/Task View and running apps.
- trayGap: 8
  $name: Gap before tray
  $description: Space between closed pinned apps and the notification area, or native Widgets placed before it.
- middleGap: 48
  $name: Minimum middle gap
  $description: Preferred minimum empty space between running and closed pinned groups. When crowded, this gap shrinks to zero before icon spacing is compressed.
- pinnedIconScale: 90
  $name: Closed pinned icon size
  $description: Size and packing density of icons in the right group, as a percentage from 50 to 100. Running icons always use 100%.
- sectionDragging: true
  $name: Enable section dragging
  $description: Reorder apps within each section with the left mouse button. Disable to retain native mouse handling; the split layout remains active, but drag boundaries are not enforced.
- systemButtonsLeft: true
  $name: Keep system buttons on the left
  $description: Put Start, Search, Widgets and Task View at the left edge. Recommended for the intended split layout.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <functional>
#include <optional>
#include <cmath>
#include <iterator>
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
    std::atomic<int> leftPadding{0};
    std::atomic<int> runningGap{0};
    std::atomic<int> trayGap{8};
    std::atomic<int> middleGap{48};
    std::atomic<int> pinnedIconScale{90};
    std::atomic<bool> systemButtonsLeft{true};
    std::atomic<bool> sectionDragging{true};
};

Settings g_settings;
std::atomic<bool> g_unloading{false};
std::atomic<bool> g_refreshQueued{false};
std::atomic<bool> g_settingsArrangeFollowup{false};
std::atomic<bool> g_viewHooksInstalled{false};
std::atomic<HWND> g_taskbarWindow{nullptr};
std::atomic<bool> g_taskbarSubclassed{false};
thread_local bool g_insideArrange = false;

void LoadSettings() {
    g_settings.sectionDragging = Wh_GetIntSetting(L"sectionDragging") != 0;
    g_settings.leftPadding = std::max(0, Wh_GetIntSetting(L"leftPadding"));
    g_settings.runningGap = std::max(0, Wh_GetIntSetting(L"runningGap"));
    g_settings.trayGap = std::max(0, Wh_GetIntSetting(L"trayGap"));
    g_settings.middleGap = std::max(0, Wh_GetIntSetting(L"middleGap"));
    g_settings.pinnedIconScale =
        std::clamp(Wh_GetIntSetting(L"pinnedIconScale"), 50, 100);
    g_settings.systemButtonsLeft =
        Wh_GetIntSetting(L"systemButtonsLeft") != 0;
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

void* CTaskBand_ITaskListWndSite_vftable = nullptr;
using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void*, void**);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;
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

XamlRoot TaskbarXamlRoot(HWND taskbarWindow) {
    HWND taskbandWindow = reinterpret_cast<HWND>(
        GetPropW(taskbarWindow, L"TaskbandHWND"));
    if (!taskbandWindow) {
        return nullptr;
    }

    auto taskBand = reinterpret_cast<void*>(GetWindowLongPtrW(taskbandWindow, 0));
    if (!taskBand) {
        return nullptr;
    }

    void* site = taskBand;
    bool found = false;
    for (int slot = 0; slot < 20; ++slot) {
        if (*reinterpret_cast<void**>(site) ==
            CTaskBand_ITaskListWndSite_vftable) {
            found = true;
            break;
        }
        site = reinterpret_cast<void**>(site) + 1;
    }
    if (!found) {
        return nullptr;
    }

    void* hostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(site, hostSharedPtr);
    SharedPtrGuard release{hostSharedPtr[1]};
    if (!hostSharedPtr[0]) {
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
        return nullptr;
    }

    FrameworkElement hostedElement{nullptr};
    unknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                            winrt::put_abi(hostedElement));
    return hostedElement ? hostedElement.XamlRoot() : nullptr;
}

FrameworkElement FindTaskbarRepeater(FrameworkElement const& root) {
    auto frame = FindDirectChildByClass(root, L"Taskbar.TaskbarFrame");
    auto grid = frame ? FindDirectChildByName(frame, L"RootGrid") : nullptr;
    return grid ? FindDirectChildByName(grid, L"TaskbarFrameRepeater")
                : nullptr;
}

winrt::weak_ref<FrameworkElement> g_repeaterCache;
std::atomic<bool> g_repeaterCacheInvalidated{false};

FrameworkElement GetTaskbarRepeater() {
    // Only the taskbar UI thread touches the weak cache itself.
    if (g_repeaterCacheInvalidated.exchange(false)) {
        g_repeaterCache = nullptr;
    }
    if (auto cached = g_repeaterCache.get()) {
        if (cached.XamlRoot()) {
            return cached;
        }
        g_repeaterCache = nullptr;
    }

    HWND window = g_taskbarWindow;
    auto root = window ? TaskbarXamlRoot(window) : nullptr;
    auto content = root ? root.Content().try_as<FrameworkElement>() : nullptr;
    auto repeater = content ? FindTaskbarRepeater(content) : nullptr;
    if (repeater) {
        g_repeaterCache = repeater;
    }
    return repeater;
}

double ElementWidth(FrameworkElement const& element) {
    if (element.Visibility() != Visibility::Visible) {
        return 0;
    }
    Thickness margin = element.Margin();
    double width = element.ActualWidth();
    if (width <= 0) {
        return std::max(0.0, static_cast<double>(element.DesiredSize().Width));
    }
    return std::max(0.0, margin.Left + width + margin.Right);
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

winrt::weak_ref<FrameworkElement> g_widgetsCache;
winrt::weak_ref<FrameworkElement> g_widgetsFrame;
ULONGLONG g_widgetsNextSearch = 0;
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
    auto frame = FindDirectChildByClass(content, L"Taskbar.TaskbarFrame");
    if (!frame) return;
    if (g_widgetsInvalidated.exchange(false) || g_widgetsFrame.get() != frame) {
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

using TaskListButton_GetIsRunning_t = HRESULT(WINAPI*)(void*, bool*);
TaskListButton_GetIsRunning_t TaskListButton_GetIsRunning_Original = nullptr;

bool ButtonIsRunning(FrameworkElement const& element) {
    if (!TaskListButton_GetIsRunning_Original) {
        return true;
    }
    bool running = false;
    HRESULT result = TaskListButton_GetIsRunning_Original(
        winrt::get_abi(element.as<winrt::Windows::Foundation::IUnknown>()),
        &running);
    return SUCCEEDED(result) ? running : true;
}

struct AppliedVisualState {
    winrt::weak_ref<FrameworkElement> element;
    numerics::float3 originalScale{};
    numerics::float3 originalCenterPoint{};
    bool scaleApplied = false;
};

// Only weak UI references and numeric values: no strong thread-affine Visual
// references are released by a global destructor during process shutdown.
std::unordered_map<void*, AppliedVisualState> g_visualStates;

AppliedVisualState* CurrentVisualState(FrameworkElement const& element) {
    auto found = g_visualStates.find(winrt::get_abi(element));
    if (found == g_visualStates.end()) {
        return nullptr;
    }
    auto storedElement = found->second.element.get();
    if (!storedElement || storedElement != element) {
        g_visualStates.erase(found);
        return nullptr;
    }
    return &found->second;
}

AppliedVisualState& EnsureVisualState(FrameworkElement const& element) {
    if (auto current = CurrentVisualState(element)) {
        return *current;
    }

    AppliedVisualState applied;
    applied.element = element;
    return g_visualStates.emplace(winrt::get_abi(element), std::move(applied))
        .first->second;
}

void ScaleElement(FrameworkElement const& element, double scaleValue) {
    if (scaleValue == 1.0 && !CurrentVisualState(element)) {
        return;
    }
    auto& applied = EnsureVisualState(element);

    if (scaleValue == 1.0) {
        if (applied.scaleApplied) {
            element.Scale(applied.originalScale);
            element.CenterPoint(applied.originalCenterPoint);
            applied.scaleApplied = false;
        }
        return;
    }
    if (!applied.scaleApplied) {
        applied.originalScale = element.Scale();
        applied.originalCenterPoint = element.CenterPoint();
        // Mark before writes so a partial failure can still be restored.
        applied.scaleApplied = true;
    }
    auto centerPoint = applied.originalCenterPoint;
    // Placement math assumes scaling from the left edge.
    centerPoint.x = 0;
    centerPoint.y = static_cast<float>(element.ActualHeight() / 2.0);
    element.CenterPoint(centerPoint);

    auto scale = applied.originalScale;
    scale.x *= static_cast<float>(scaleValue);
    scale.y *= static_cast<float>(scaleValue);
    element.Scale(scale);
}

void RestoreElementState(AppliedVisualState& applied) {
    if (auto element = applied.element.get()) {
        if (applied.scaleApplied) {
            element.Scale(applied.originalScale);
            element.CenterPoint(applied.originalCenterPoint);
            applied.scaleApplied = false;
        }
    }
}

void PruneVisualStates(std::vector<FrameworkElement> const& children) {
    std::unordered_set<void*> live;
    for (auto const& child : children) {
        live.insert(winrt::get_abi(child));
    }
    for (auto it = g_visualStates.begin(); it != g_visualStates.end();) {
        if (!live.count(it->first) || !it->second.element.get()) {
            // A detached but still alive element must be restored before
            // forgetting it, in case Windows later reuses the same object.
            try {
                RestoreElementState(it->second);
            } catch (winrt::hresult_error const&) {
            }
            it = g_visualStates.erase(it);
        } else {
            ++it;
        }
    }
}

void RestoreVisualStates() {
    for (auto& [key, applied] : g_visualStates) {
        try {
            RestoreElementState(applied);
        } catch (winrt::hresult_error const&) {
            // A disconnected element must not prevent restoring the others.
        }
    }
    g_visualStates.clear();
}

struct ButtonInfo {
    FrameworkElement element;
    double width;
    bool running;
    std::wstring appId;
};

// Weak lists are live gesture views only, rebuilt from app IDs on every layout.
std::vector<winrt::weak_ref<FrameworkElement>> g_runningOrder;
std::vector<winrt::weak_ref<FrameworkElement>> g_pinnedOrder;
std::vector<std::wstring> g_runningAppOrder;
std::vector<std::wstring> g_pinnedAppOrder;
std::unordered_set<std::wstring> g_previousPinnedApps;
bool g_orderLoaded = false;

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

// Bounded, length-prefixed format: IDs can contain punctuation/newlines.
std::wstring EncodeAppOrder(std::vector<std::wstring> const& ids) {
    std::wstring data = L"1;";
    for (auto const& id : ids) {
        if (id.empty() || id.size() > 1024) continue;
        auto part = std::to_wstring(id.size()) + L":" + id;
        if (data.size() + part.size() >= 65535) break;
        data += part;
    }
    return data;
}

std::vector<std::wstring> DecodeAppOrder(std::wstring const& data) {
    std::vector<std::wstring> ids;
    if (data.rfind(L"1;", 0) != 0 || data.size() >= 65535) return ids;
    size_t pos = 2;
    while (pos < data.size()) {
        size_t length = 0, digits = 0;
        while (pos < data.size() && data[pos] >= L'0' && data[pos] <= L'9') {
            if (++digits > 4) return {};
            length = length * 10 + (data[pos++] - L'0');
        }
        if (!digits || !length || length > 1024 || pos >= data.size() || data[pos++] != L':' ||
            length > data.size() - pos || ids.size() >= 256) return {};
        auto id = data.substr(pos, length);
        if (std::find(ids.begin(), ids.end(), id) == ids.end()) ids.push_back(id);
        pos += length;
    }
    return ids;
}

void LoadAppOrders() {
    if (g_orderLoaded) return;
    g_orderLoaded = true;
    auto read = [](wchar_t const* name) {
        std::vector<wchar_t> buffer(65536, L'\0');
        Wh_GetStringValue(name, buffer.data(), buffer.size());
        buffer.back() = L'\0';
        return DecodeAppOrder(std::wstring(buffer.data()));
    };
    g_runningAppOrder = read(L"running-app-order-v1");
    g_pinnedAppOrder = read(L"pinned-app-order-v1");
}

void SaveAppOrder(bool running) {
    auto const& ids = running ? g_runningAppOrder : g_pinnedAppOrder;
    auto data = EncodeAppOrder(ids);
    if (!Wh_SetStringValue(running ? L"running-app-order-v1" : L"pinned-app-order-v1", data.c_str()))
        Wh_Log(L"Could not save section order");
}

void RememberApp(std::vector<std::wstring>& ids, std::wstring const& id) {
    if (id.empty() || std::find(ids.begin(), ids.end(), id) != ids.end()) return;
    if (ids.size() == 256) ids.erase(ids.begin());
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

void OrderRunningButtons(std::vector<ButtonInfo*>& buttons) {
    for (auto button : buttons) {
        if (!button->appId.empty() && g_previousPinnedApps.count(button->appId)) {
            auto& ids = g_runningAppOrder;
            ids.erase(std::remove(ids.begin(), ids.end(), button->appId), ids.end());
            RememberApp(ids, button->appId); // A newly launched app goes last.
        }
    }
    ApplyAppOrder(buttons, g_runningAppOrder, g_runningOrder);
}

void OrderPinnedButtons(std::vector<ButtonInfo*>& buttons) {
    ApplyAppOrder(buttons, g_pinnedAppOrder, g_pinnedOrder);
    g_previousPinnedApps.clear();
    for (auto button : buttons) if (!button->appId.empty()) g_previousPinnedApps.insert(button->appId);
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

struct Placement {
    winrt::weak_ref<FrameworkElement> element;
    float x;
    double scale;
    double measuredWidth;
};
// Built before the original ArrangeOverride. The nested Arrange hook only
// performs an ABI identity lookup; it never traverses or mutates the tree.
thread_local std::unordered_map<void*, Placement> g_arrangePlan;
thread_local bool g_useArrangePlan = false;

void PlanElement(FrameworkElement const& element, double x, double scale) {
    // Rect.X is the margin-box origin, not the rendered border-box origin.
    g_arrangePlan.emplace(winrt::get_abi(element),
                          Placement{element, static_cast<float>(x), scale,
                                    element.ActualWidth()});
}

double StepScale(std::vector<ButtonInfo*> const& group,
                 double allocatedWidth,
                 double visualScale,
                 bool anchoredLeft) {
    if (group.size() < 2) {
        return 1.0;
    }
    double total = 0;
    for (auto item : group) {
        total += item->width * visualScale;
    }
    if (total <= allocatedWidth) {
        return 1.0;
    }
    double edgeWidth = (anchoredLeft ? group.back()->width
                                     : group.front()->width) * visualScale;
    double compressible = total - edgeWidth;
    double room = allocatedWidth - edgeWidth;
    return compressible > 0
               ? std::clamp(room / compressible, 0.0, 1.0)
               : 1.0;
}

bool BuildLayoutPlan() {
    g_arrangePlan.clear();
    try {
        auto repeater = GetTaskbarRepeater();
        auto content = repeater && repeater.XamlRoot()
                           ? repeater.XamlRoot().Content().try_as<FrameworkElement>()
                           : nullptr;
        if (!repeater || !content) {
            return false;
        }
        double repeaterX = ElementX(repeater, content);

        LoadAppOrders();
        auto children = RepeaterElements(repeater);
        PruneVisualStates(children);
        std::vector<ButtonInfo> buttons;
        std::vector<FrameworkElement> systemButtons;
        FrameworkElement overflow{nullptr};
        buttons.reserve(children.size());
        for (auto const& child : children) {
            if (child.Visibility() != Visibility::Visible) {
                continue;
            }
            if (IsTaskButton(child) && ElementWidth(child) > 0) {
                buttons.push_back({child, ElementWidth(child),
                                   ButtonIsRunning(child), ButtonAppId(child)});
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

        double leftEdge = g_settings.leftPadding.load();
        if (g_settings.systemButtonsLeft.load()) {
            for (auto const& button : systemButtons) {
                double width = SystemButtonWidth(button);
                if (width > 0) {
                    PlanElement(button, leftEdge - repeaterX, 1.0);
                    leftEdge += width;
                }
            }
        } else {
            for (auto const& button : systemButtons) {
                // Widgets are reserved separately, on their actual side.
                if (GetSystemButtonKind(button) == SystemButtonKind::Widgets) continue;
                double width = SystemButtonWidth(button);
                if (width <= 0) {
                    continue;
                }
                double nativeX = repeaterX + button.ActualOffset().x;
                leftEdge = std::max(leftEdge,
                                    nativeX + width);
            }
        }

        auto tray = FindDirectChildByClass(content,
                                            L"SystemTray.SystemTrayFrame");
        double rightEdge = tray ? ElementX(tray, content)
                                : static_cast<double>(content.ActualWidth());
        if (!g_settings.systemButtonsLeft.load()) {
            ReserveNativeWidgets(content, leftEdge, rightEdge);
        }
        leftEdge += g_settings.runningGap.load();
        rightEdge -= g_settings.trayGap.load();

        std::vector<ButtonInfo*> running;
        std::vector<ButtonInfo*> pinned;
        for (auto& button : buttons) {
            (button.running ? running : pinned).push_back(&button);
        }
        OrderRunningButtons(running);
        OrderPinnedButtons(pinned);
        ButtonInfo overflowInfo{overflow, overflow ? ElementWidth(overflow) : 0, true, {}};
        if (overflowInfo.width > 0) {
            running.push_back(&overflowInfo);
        }

        double pinnedVisualScale =
            g_settings.pinnedIconScale.load() / 100.0;
        auto totalWidth = [](std::vector<ButtonInfo*> const& group,
                             double scale) {
            double total = 0;
            for (auto item : group) {
                total += item->width * scale;
            }
            return total;
        };

        double runningWidth = totalWidth(running, 1.0);
        double pinnedWidth = totalWidth(pinned, pinnedVisualScale);
        double requested = runningWidth + pinnedWidth;
        double span = std::max(0.0, rightEdge - leftEdge);
        // Consume the gap before compressing steps between buttons.
        double gap = (running.empty() || pinned.empty()) ? 0.0 :
            std::min(static_cast<double>(g_settings.middleGap.load()),
                     std::max(0.0, span - requested));
        double available = span - gap;
        double runningAllocation = runningWidth;
        double pinnedAllocation = pinnedWidth;
        if (requested > available && requested > 0) {
            runningAllocation = available * runningWidth / requested;
            pinnedAllocation = available - runningAllocation;
        }

        double runningStep =
            StepScale(running, runningAllocation, 1.0, true);
        double pinnedStep = StepScale(pinned, pinnedAllocation,
                                      pinnedVisualScale, false);

        double x = leftEdge;
        for (auto item : running) {
            PlanElement(item->element, x - repeaterX, 1.0);
            x += item->width * runningStep;
        }

        x = rightEdge;
        for (auto item = pinned.rbegin(); item != pinned.rend(); ++item) {
            double visualWidth = (*item)->width * pinnedVisualScale;
            x -= visualWidth;
            PlanElement((*item)->element, x - repeaterX, pinnedVisualScale);
            x += visualWidth;
            x -= visualWidth * pinnedStep;
        }
        return true;
    } catch (...) {
        g_arrangePlan.clear();
        Wh_Log(L"Taskbar Split: layout pass failed safely");
        return false;
    }
}

using ArrangeOverride_t = HRESULT(WINAPI*)(
    void*, void*, winrt::Windows::Foundation::Size,
    winrt::Windows::Foundation::Size*);
ArrangeOverride_t ArrangeOverride_Original = nullptr;
HWND EnsureTaskbarWindow();
void RequestRefresh();

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
    IUIElement element = rectangle;
    auto vtable = *reinterpret_cast<void***>(winrt::get_abi(element));
    auto arrange = reinterpret_cast<ElementArrange_t>(vtable[92]);
    if (!WindhawkUtils::SetFunctionHook(arrange, ElementArrange_Hook,
                                      &ElementArrange_Original)) {
        return false;
    }
    Wh_ApplyHookOperations();
    return true;
}

void UpdateDragPreview();

HRESULT WINAPI ArrangeOverride_Hook(
    void* self, void* context, winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize) {
    if (g_unloading || g_insideArrange) {
        // Nested layout must not accidentally consume its caller's plan.
        bool saved = g_useArrangePlan;
        g_useArrangePlan = false;
        HRESULT result = ArrangeOverride_Original(self, context, size, resultSize);
        g_useArrangePlan = saved;
        return result;
    }
    struct ArrangeGuard {
        ArrangeGuard() { g_insideArrange = true; }
        ~ArrangeGuard() {
            g_useArrangePlan = false;
            g_arrangePlan.clear();
            g_insideArrange = false;
        }
    } guard;
    HWND window = EnsureTaskbarWindow();
    if (!window || !g_taskbarSubclassed || GetWindowThreadProcessId(window, nullptr) !=
                       GetCurrentThreadId()) {
        return ArrangeOverride_Original(self, context, size, resultSize);
    }
    // self is the ABI IVirtualizingLayoutOverrides pointer. Compare the
    // canonical Layout identity, not just the shared Explorer UI thread.
    try {
        winrt::Windows::Foundation::IUnknown caller{nullptr};
        winrt::copy_from_abi(caller, self);
        auto layout = caller.try_as<winrt::Microsoft::UI::Xaml::Controls::Layout>();
        auto element = GetTaskbarRepeater();
        auto repeater = element ? element.try_as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>() : nullptr;
        auto primary = repeater ? repeater.Layout() : nullptr;
        if (!layout || !primary ||
            layout.as<winrt::Windows::Foundation::IUnknown>() !=
                primary.as<winrt::Windows::Foundation::IUnknown>()) {
            return ArrangeOverride_Original(self, context, size, resultSize);
        }
    } catch (...) {
        return ArrangeOverride_Original(self, context, size, resultSize);
    }
    bool planReady = false;
    try {
        if (EnsureArrangeHook()) {
            planReady = BuildLayoutPlan();
            g_useArrangePlan = planReady;
        }
    } catch (...) {
        g_arrangePlan.clear();
        g_useArrangePlan = false;
    }
    HRESULT result = ArrangeOverride_Original(self, context, size, resultSize);
    g_useArrangePlan = false;
    // After toggling system-button placement, native widget positions only
    // become current after this pass. Recompute once from those positions.
    if (g_settingsArrangeFollowup.exchange(false)) RequestRefresh();
    if (!g_unloading && planReady) {
        try {
            // Scale after the native pass; section positioning uses Arrange.
            for (auto const& [key, placement] : g_arrangePlan) {
                if (auto element = placement.element.get()) {
                    ScaleElement(element, placement.scale);
                    if (std::abs(element.ActualWidth() - placement.measuredWidth) > 0.05) {
                        RequestRefresh();
                    }
                }
            }
            UpdateDragPreview();
            // Newly realized buttons weren't available when the plan was
            // built. One queued pass incorporates their measured dimensions.
            if (auto repeater = GetTaskbarRepeater()) {
                for (auto const& child : RepeaterElements(repeater)) {
                    if (IsTaskButton(child) && ElementWidth(child) > 0 &&
                        !g_arrangePlan.count(winrt::get_abi(child))) {
                        RequestRefresh();
                        break;
                    }
                }
            }
        } catch (...) {
            Wh_Log(L"Post-arrange scaling failed");
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
    bool running = false;
    bool dragged = false;
    unsigned int pointerId = 0;
    double startX = 0;
    double desiredX = 0;
    double minX = 0;
    double maxX = 0;
    double width = 0;
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
[[clang::no_destroy]] std::optional<SectionGesture> g_sectionGesture;

struct PendingDrop {
    SectionGesture gesture;
    winrt::Windows::UI::Composition::ImplicitAnimationCollection implicitAnimations{nullptr};
    bool arranged = false;
    ULONGLONG started = 0;
    unsigned int stableSamples = 0;
};
[[clang::no_destroy]] std::optional<PendingDrop> g_pendingDrop;
HWND g_dropSettleWindow = nullptr;

UINT_PTR DropSettleTimerId() {
    return reinterpret_cast<UINT_PTR>(&g_dropSettleWindow);
}
bool g_cancelingNativePress = false; // Taskbar thread only.

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
    if (auto source = gesture.source.get()) {
        if (gesture.previewPrepared) {
            try {
                // XAML Transitions alone do not disable Composition's
                // reposition animations. Keep the drag offset until layout
                // has reached its destination, then remove it without an
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
                visual.Properties().StopAnimation(L"Translation");
                if (auto repeater = GetTaskbarRepeater()) {
                    repeater.InvalidateArrange();
                    repeater.UpdateLayout();
                }
                // The layout pass can start an explicit Offset animation,
                // even with implicit animations disabled. Its final base
                // value already reflects the new arrange rectangle.
                visual.StopAnimation(L"Offset");
                visual.Properties().StopAnimation(L"Translation");
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
    if (!pending.arranged) {
        try { RestoreGestureVisual(pending.gesture); } catch (...) {}
    } else {
        // Layout is already committed. Do not invalidate it again just to
        // restore transitions; that could schedule another reposition.
        try {
            if (auto source = pending.gesture.source.get()) {
                source.Translation(pending.gesture.translation);
                source.Transitions(pending.gesture.transitions);
            }
        } catch (...) {}
    }
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
    try {
        auto visual = Hosting::ElementCompositionPreview::GetElementVisual(element);
        visual.StopAnimation(L"Offset");
        visual.Properties().StopAnimation(L"Translation");
        // The trace proved that StopAnimation alone leaves the old BASE
        // Offset (399.2) even after ActualOffset has changed to 355.2.
        // Commit that base value explicitly; preserve vertical/depth offsets.
        auto offset = visual.Offset();
        offset.x = element.ActualOffset().x;
        visual.Offset(offset);
        pending.arranged = true;
    } catch (...) {}
    try { element.Translation(pending.gesture.translation); } catch (...) {}
    // Do not re-enable transitions on Arrange return: the captured trace
    // starts the queued reposition at +219ms and finishes it at +563ms.
}

void SettlePendingDrop() {
    if (!g_pendingDrop) { FlushPendingDrop(); return; }
    try {
        auto source = g_pendingDrop->gesture.source.get();
        if (!source || g_unloading) { FlushPendingDrop(); return; }
        ULONGLONG elapsed = GetTickCount64() - g_pendingDrop->started;
        if (g_pendingDrop->arranged) {
            auto visual = Hosting::ElementCompositionPreview::GetElementVisual(source);
            double target = source.ActualOffset().x;
            double actual = visual.Offset().x;
            if (std::abs(actual - target) > 0.05) {
                CompletePendingDrop(source);
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
            Wh_Log(L"Drop did not settle in 2 seconds; restoring animation settings");
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
        g_dropSettleWindow = g_taskbarWindow;
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
    try {
        if (gesture.dragged) {
            (gesture.running ? g_runningOrder : g_pinnedOrder) = gesture.originalOrder;
            (gesture.running ? g_runningAppOrder : g_pinnedAppOrder) = gesture.originalAppOrder;
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

void FinishSectionReorder(SectionGesture& gesture,
                          Input::PointerRoutedEventArgs const&) {
    auto source = gesture.source.get();
    auto repeater = GetTaskbarRepeater();
    if (!source || !repeater) return;
    if (ButtonIsRunning(source) != gesture.running) {
        return;
    }
    // Use section slots, not hit-testing of overlapping scaled rectangles.
    // In 0.3.2 the dragged button itself could replace the intended target.
    auto& order = gesture.running ? g_runningOrder : g_pinnedOrder;
    std::vector<FrameworkElement> live;
    std::vector<double> widths;
    for (auto const& reference : order) {
        auto element = reference.get();
        if (!element || element.Visibility() != Visibility::Visible ||
            ButtonIsRunning(element) != gesture.running) continue;
        live.push_back(element);
        widths.push_back(ElementWidth(element) * (gesture.running ? 1.0 :
            g_settings.pinnedIconScale.load() / 100.0));
    }
    auto from = std::find(live.begin(), live.end(), source);
    if (from == live.end()) return;
    size_t current = static_cast<size_t>(from - live.begin());
    double relativeCentre = gesture.desiredX + gesture.width / 2 - gesture.minX;
    size_t targetIndex = gesture.running
        ? DragDestination(widths, current, relativeCentre)
        : PinnedDragDestination(widths, current, relativeCentre);
    if (targetIndex == current) {
        return;
    }
    auto target = live[targetIndex];
    if (!ReorderWithinSection(live, source, target, targetIndex > current)) {
        return;
    }
    std::vector<winrt::weak_ref<FrameworkElement>> nextOrder;
    for (auto const& element : live) nextOrder.emplace_back(element);
    std::vector<std::wstring> visibleIds;
    for (auto const& element : live) visibleIds.push_back(ButtonAppId(element));
    MergeVisibleAppOrder(gesture.running ? g_runningAppOrder : g_pinnedAppOrder, visibleIds);
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
    HWND window = g_taskbarWindow;
    return window && GetWindowThreadProcessId(window, nullptr) == GetCurrentThreadId();
}

HRESULT WINAPI PointerPressed_Hook(void* self, void* rawArgs) {
    if (g_unloading || !IsGestureThread()) return PointerPressed_Original(self, rawArgs);
    if (!g_settings.sectionDragging.load()) {
        CancelSectionGesture();
        FlushPendingDrop();
        return PointerPressed_Original(self, rawArgs);
    }
    FlushPendingDrop();
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
            auto repeater = GetTaskbarRepeater();
            bool inPrimary = false;
            if (repeater && g_taskbarSubclassed) {
                for (auto const& child : RepeaterElements(repeater)) {
                    if (child == source) { inPrimary = true; break; }
                }
            }
            if (inPrimary && props.IsLeftButtonPressed() &&
                !props.IsRightButtonPressed() && !props.IsMiddleButtonPressed()) {
                g_sectionGesture.reset();
                SectionGesture gesture;
                gesture.source = source;
                gesture.pointer = args.Pointer();
                gesture.origin = args.GetCurrentPoint(repeater).Position();
                gesture.appId = ButtonAppId(source);
                if (gesture.appId.empty()) return result;
                gesture.running = ButtonIsRunning(source);
                gesture.originalAppOrder = gesture.running ? g_runningAppOrder : g_pinnedAppOrder;
                gesture.pointerId = args.Pointer().PointerId();
                gesture.startX = gesture.desiredX = ElementX(source, repeater);
                gesture.translation = source.Translation();
                gesture.originalSlotX = Controls::Primitives::LayoutInformation::GetLayoutSlot(source).X;
                gesture.transitions = source.Transitions();
                double scale = gesture.running ? 1.0 : g_settings.pinnedIconScale.load() / 100.0;
                gesture.width = ElementWidth(source) * scale;
                gesture.minX = gesture.startX;
                double right = gesture.startX + gesture.width;
                gesture.originalOrder = gesture.running ? g_runningOrder : g_pinnedOrder;
                for (auto const& reference : gesture.originalOrder) {
                    if (auto item = reference.get()) {
                        double x = ElementX(item, repeater);
                        gesture.minX = std::min(gesture.minX, x);
                        right = std::max(right, x + ElementWidth(item) * scale);
                    }
                }
                gesture.maxX = std::max(gesture.minX, right - gesture.width);
                g_sectionGesture.emplace(std::move(gesture));
            }
        }
    } catch (...) {
        g_sectionGesture.reset();
    }
    return result;
}

HRESULT WINAPI PointerMoved_Hook(void* self, void* rawArgs) {
    if (!IsGestureThread()) return PointerMoved_Original(self, rawArgs);
    try {
        auto source = GestureInterface<FrameworkElement>(self);
        auto args = GestureInterface<Input::PointerRoutedEventArgs>(rawArgs);
        if (GestureMatches(source, args)) {
            args.Handled(true);
            auto repeater = GetTaskbarRepeater();
            if (g_unloading || !repeater || ButtonAppId(source) != g_sectionGesture->appId ||
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

HRESULT WINAPI PointerReleased_Hook(void* self, void* rawArgs) {
    if (!IsGestureThread()) return PointerReleased_Original(self, rawArgs);
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
            if (g_unloading || ButtonAppId(source) != gesture.appId || (GetAsyncKeyState(VK_ESCAPE) & 0x8000)) {
                (gesture.running ? g_runningOrder : g_pinnedOrder) = gesture.originalOrder;
                (gesture.running ? g_runningAppOrder : g_pinnedAppOrder) = gesture.originalAppOrder;
                RestoreGestureVisual(gesture);
                RequestRefresh();
            } else {
                try { SaveAppOrder(gesture.running); }
                catch (...) { Wh_Log(L"Could not save section order"); }
                QueueDrop(std::move(gesture));
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
        RegisterWindowMessageW(L"Windhawk_TaskbarSplit_Refresh_" WH_MOD_ID);
    return value;
}

UINT RestoreMessage() {
    static UINT value =
        RegisterWindowMessageW(L"Windhawk_TaskbarSplit_Restore_" WH_MOD_ID);
    return value;
}

LRESULT CALLBACK TaskbarSubclassProc(HWND window, UINT message, WPARAM wParam,
                                     LPARAM lParam, DWORD_PTR) {
    if (message == WM_TIMER && wParam == DropSettleTimerId()) {
        SettlePendingDrop();
        return 0;
    }
    if (message == WM_NCDESTROY) {
        CancelSectionGesture();
        FlushPendingDrop();
        g_widgetsInvalidated = true;
        g_taskbarWindow = nullptr;
        g_taskbarSubclassed = false;
        g_refreshQueued = false;
        g_repeaterCacheInvalidated = true;
    }
    if (message == RefreshMessage()) {
        g_refreshQueued = false;
        if (!g_unloading) {
            try {
                if (auto repeater = GetTaskbarRepeater()) {
                    repeater.InvalidateArrange();
                }
            } catch (...) {
                Wh_Log(L"Refresh failed: taskbar element disconnected");
            }
        }
        return 0;
    }
    if (message == RestoreMessage()) {
        CancelSectionGesture();
        FlushPendingDrop();
        RestoreVisualStates();
        try {
            if (auto repeater = GetTaskbarRepeater()) {
                // g_unloading disables rect rewriting during this layout.
                repeater.InvalidateArrange();
                repeater.UpdateLayout();
            }
        } catch (...) {
            Wh_Log(L"Native taskbar layout refresh failed during unload");
        }
        return 0;
    }
    return DefSubclassProc(window, message, wParam, lParam);
}

HWND EnsureTaskbarWindow() {
    HWND window = g_taskbarWindow;
    if (window && !IsWindow(window)) {
        g_taskbarWindow = nullptr;
        g_taskbarSubclassed = false;
        g_refreshQueued = false;
        g_repeaterCacheInvalidated = true;
        window = nullptr;
    }
    if (!window && !g_unloading) {
        window = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (window) {
            DWORD processId = 0;
            GetWindowThreadProcessId(window, &processId);
            if (processId != GetCurrentProcessId()) {
                window = nullptr;
            }
        }
        if (window) {
            g_taskbarWindow = window;
        }
    }
    if (window && !g_taskbarSubclassed && !g_unloading &&
        GetWindowThreadProcessId(window, nullptr) == GetCurrentThreadId() &&
        WindhawkUtils::SetWindowSubclassFromAnyThread(
            window, TaskbarSubclassProc, 0)) {
        g_taskbarSubclassed = true;
        // Unload may have completed its removal check during installation.
        if (g_unloading && g_taskbarSubclassed.exchange(false)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                window, TaskbarSubclassProc);
        }
    }
    return window;
}

void RequestRefresh() {
    if (g_unloading) {
        return;
    }
    HWND window = g_taskbarWindow;
    if (!window || !g_taskbarSubclassed || g_refreshQueued.exchange(true)) {
        return;
    }
    if (!PostMessageW(window, RefreshMessage(), 0, 0)) {
        g_refreshQueued = false;
    }
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void*);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original =
    nullptr;
struct RunningState {
    winrt::weak_ref<FrameworkElement> element;
    bool running;
};
std::unordered_map<void*, RunningState> g_lastRunningState;

void WINAPI TaskListButton_UpdateVisualStates_Hook(void* self) {
    TaskListButton_UpdateVisualStates_Original(self);

    // UpdateVisualStates also runs for hover, focus and press animations.
    // Invalidate layout only when the running state itself really changed.
    if (g_unloading || !TaskListButton_GetIsRunning_Original) {
        return;
    }
    try {
        // UpdateVisualStates receives the implementation address. The ABI
        // interface subobject starts three pointer-sized slots later.
        winrt::Windows::Foundation::IUnknown abiObject{nullptr};
        winrt::copy_from_abi(abiObject, static_cast<void**>(self) + 3);
        auto element = abiObject.as<FrameworkElement>();
        bool running = ButtonIsRunning(element);
        for (auto it = g_lastRunningState.begin();
             it != g_lastRunningState.end();) {
            if (!it->second.element.get()) {
                it = g_lastRunningState.erase(it);
            } else {
                ++it;
            }
        }
        auto [entry, inserted] = g_lastRunningState.emplace(
            self, RunningState{element, running});
        if (inserted || entry->second.element.get() != element ||
            entry->second.running != running) {
            entry->second = RunningState{element, running};
            RequestRefresh();
        }
    } catch (...) {
        // Do not let a disconnected XAML object unwind through Explorer.
    }
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
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
         &TaskbarHost_FrameHeight_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
         &RefCount_Decref_Original},
    };
    return WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                      ARRAYSIZE(taskbarDllHooks));
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
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Taskbar::ITaskListButton>::get_IsRunning(bool *))"},
         &TaskListButton_GetIsRunning_Original, nullptr, true},
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
         &TaskListButton_UpdateVisualStates_Original,
         TaskListButton_UpdateVisualStates_Hook, true},
    };
    bool result = WindhawkUtils::HookSymbols(
        module, taskbarViewHooks, ARRAYSIZE(taskbarViewHooks));
    if (!TaskListButton_GetIsRunning_Original) {
        Wh_Log(L"Taskbar Split: IsRunning unavailable; keeping task buttons left");
    }
    return result && ArrangeOverride_Original;
}

HMODULE CurrentTaskbarViewModule() {
    if (HMODULE module = GetModuleHandleW(L"Taskbar.View.dll")) {
        return module;
    }
    return GetModuleHandleW(L"ExplorerExtensions.dll");
}

void TryHookLoadedTaskbarView(HMODULE loadedModule) {
    HMODULE taskbarView = CurrentTaskbarViewModule();
    if (taskbarView && taskbarView == loadedModule &&
        !g_viewHooksInstalled.exchange(true)) {
        if (!HookTaskbarViewSymbols(taskbarView)) {
            Wh_Log(L"Taskbar Split: failed to hook taskbar view symbols");
        }
        Wh_ApplyHookOperations();
    }
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
        Wh_Log(L"Taskbar Split: failed to hook taskbar.dll");
        return FALSE;
    }
    if (HMODULE module = CurrentTaskbarViewModule()) {
        g_viewHooksInstalled = true;
        if (!HookTaskbarViewSymbols(module)) {
            return FALSE;
        }
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
    // This entry point is outside the LoadLibrary hook. It can marshal the
    // initial subclass installation to an already existing taskbar thread.
    HWND window = EnsureTaskbarWindow();
    if (window && !g_taskbarSubclassed &&
        WindhawkUtils::SetWindowSubclassFromAnyThread(
            window, TaskbarSubclassProc, 0)) {
        g_taskbarSubclassed = true;
        if (g_unloading && g_taskbarSubclassed.exchange(false)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                window, TaskbarSubclassProc);
        }
    }
    if (!g_viewHooksInstalled) {
        if (HMODULE module = CurrentTaskbarViewModule()) {
            TryHookLoadedTaskbarView(module);
        }
    }
    RequestRefresh();
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    HWND window = g_taskbarWindow;
    if (window && g_taskbarSubclassed) {
        SendMessageW(window, RestoreMessage(), 0, 0);
    }
    if (window && g_taskbarSubclassed.exchange(false)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            window, TaskbarSubclassProc);
    }
}

void Wh_ModUninit() {
    g_runningOrder.clear();
    g_pinnedOrder.clear();
    g_runningAppOrder.clear();
    g_pinnedAppOrder.clear();
    g_previousPinnedApps.clear();
    g_visualStates.clear();
    g_lastRunningState.clear();
    g_repeaterCache = nullptr;
    g_widgetsCache = nullptr;
    g_widgetsFrame = nullptr;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    g_widgetsInvalidated = true;
    g_settingsArrangeFollowup = true;
    RequestRefresh();
}
