// ==WindhawkMod==
// @id              taskbar-zones
// @name            Taskbar Zones
// @description     Organize Windows 11 taskbar apps into fixed left and right zones. Drag icons between sides and keep their positions when apps open or close.
// @version         1.0.0
// @author          Luiz Bizzio
// @github          https://github.com/luizbizzio
// @homepage        https://github.com/luizbizzio/taskbar-zones
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

- **Two fixed zones:** Choose the left or right side for each app.
- **Easy drag and drop:** Move apps between zones or change their order.
- **Saved layout:** Keep your app positions after restarting Explorer or your PC.
- **Custom layout:** Adjust spacing and the size of icons on the right.
- **Multiple monitors:** Supports compatible secondary taskbars.

## How to use

1. Set your Windows 11 taskbar alignment to **Left**.
2. Drag an app icon across the middle gap to move it to the other side.
3. Drag icons within the same zone to reorder them.
4. Open the mod settings to adjust the layout.

## Notes

- Your Windows pinned apps are not changed.
- Do not enable **Taskbar Split** or other mods that reposition the same icons at the same time.
- If the taskbar is too crowded to fit both zones, Windows' normal layout is used until space is available again.
- Secondary monitors show only the app buttons that Windows puts on each taskbar.
- Windows updates may affect compatibility.

## Project

View the source code, report issues, or suggest improvements on [GitHub](https://github.com/luizbizzio/taskbar-zones).

**Taskbar Zones** is based on [Taskbar Split by Arkadiusz (Artllex)](https://github.com/Artllex/taskbar-split), with additional features and independent development.

Author: **Luiz Bizzio**. Licensed under **GPL-3.0**.
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- leftPadding: 0
  $name: Left edge padding
  $description: Empty space before the first system button, in device-independent pixels (DIPs).
- runningGap: 0
  $name: Gap after system buttons
  $description: Space between Start/Search/Widgets/Task View and the left app zone.
- trayGap: 8
  $name: Gap before tray
  $description: Space between right-side apps and the notification area or system monitoring widget.
- pinnedIconScale: 100
  $name: Right-zone icon size
  $description: Visual size of right-zone icons, from 50 to 100 percent. Click targets keep their native width.
- sectionDragging: true
  $name: Enable dragging between zones
  $description: Drag an app to the other side of the gap, or reorder within a side. Disable to keep native Windows dragging.
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
    std::atomic<int> leftPadding{0};
    std::atomic<int> runningGap{0};
    std::atomic<int> trayGap{8};
    std::atomic<int> pinnedIconScale{100};
    std::atomic<bool> systemButtonsLeft{true};
    std::atomic<bool> sectionDragging{true};
};

Settings g_settings;
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
    g_settings.leftPadding = std::max(0, Wh_GetIntSetting(L"leftPadding"));
    g_settings.runningGap = std::max(0, Wh_GetIntSetting(L"runningGap"));
    g_settings.trayGap = std::max(0, Wh_GetIntSetting(L"trayGap"));
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
        return nullptr;
    }

    auto taskBand = reinterpret_cast<void*>(GetWindowLongPtrW(taskbandWindow, 0));
    if (!taskBand) {
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
        return nullptr;
    }

    void* hostSharedPtr[2]{};
    (secondary ? CSecondaryTaskBand_GetTaskbarHost_Original
               : CTaskBand_GetTaskbarHost_Original)(site, hostSharedPtr);
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

// This XAML state stays on the owning taskbar UI thread.
[[clang::no_destroy]] thread_local std::unordered_map<HWND,
    winrt::weak_ref<FrameworkElement>> g_repeaterByWindow;
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
    auto repeater = content ? FindTaskbarRepeater(content) : nullptr;
    if (auto found = g_repeaterByWindow.find(window); found != g_repeaterByWindow.end()) {
        if (repeater && found->second.get() == repeater) return repeater;
        g_repeaterByWindow.erase(found);
    }
    if (repeater) g_repeaterByWindow[window] = repeater;
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
    auto frame = FindDirectChildByClass(content, L"Taskbar.TaskbarFrame");
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

struct AppliedVisualState {
    winrt::weak_ref<FrameworkElement> element;
    HWND owner = nullptr;
    numerics::float3 originalScale{};
    numerics::float3 originalCenterPoint{};
    bool scaleApplied = false;
};

// Only weak UI references and numeric values: no strong thread-affine Visual
// references are released by a global destructor during process shutdown.
[[clang::no_destroy]] thread_local std::unordered_map<void*, AppliedVisualState> g_visualStates;

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
    // Recycled XAML/ABI addresses must not inherit a different taskbar's scale.
    if (found->second.owner != g_activeTaskbarWindow) return nullptr;
    return &found->second;
}

void RestoreElementState(AppliedVisualState& applied);
AppliedVisualState& EnsureVisualState(FrameworkElement const& element) {
    if (auto current = CurrentVisualState(element)) {
        return *current;
    }

    AppliedVisualState applied;
    applied.element = element;
    applied.owner = g_activeTaskbarWindow;
    void* key = winrt::get_abi(element);
    // Address reuse across XAML roots must not silently keep an old owner.
    if (auto existing = g_visualStates.find(key); existing != g_visualStates.end()) {
        try { RestoreElementState(existing->second); } catch (...) {}
        g_visualStates.erase(existing);
    }
    return g_visualStates.emplace(key, std::move(applied)).first->second;
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
        if (it->second.owner != g_activeTaskbarWindow) {
            ++it;
            continue;
        }
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

void RestoreVisualStatesForChildren(std::vector<FrameworkElement> const& children) {
    for (auto const& child : children) {
        auto it = g_visualStates.find(winrt::get_abi(child));
        if (it == g_visualStates.end() || it->second.owner != g_activeTaskbarWindow) continue;
        try { RestoreElementState(it->second); } catch (...) {}
        g_visualStates.erase(it);
    }
}

bool RestoreVisualStatesForWindow(HWND window) {
    bool restored = true;
    for (auto it = g_visualStates.begin(); it != g_visualStates.end();) {
        if (it->second.owner != window) { ++it; continue; }
        try {
            RestoreElementState(it->second);
            it = g_visualStates.erase(it);
        } catch (...) {
            // Keep the state for a future layout/unload retry if still attached.
            restored = false;
            Wh_Log(L"Could not restore one taskbar button's visual state");
            ++it;
        }
    }
    return restored;
}


struct ButtonInfo {
    FrameworkElement element;
    double width;
    bool leftSide;
    std::wstring appId;
};

// Per-taskbar UI references must never be reused on another monitor.
struct TaskbarSlot {
    std::wstring appId;
    bool left = true;
    double x = 0;
    double width = 0;
};
struct TaskbarUiState {
    unsigned long long geometryEpoch = 1;
    bool geometryInitialized = false;
    double lastContentWidth = 0, lastContentHeight = 0;
    double lastRepeaterX = 0, lastLeftBound = 0, lastRightBound = 0;
    double lastLeftGroupWidth = 0, lastRightGroupWidth = 0;
    double lastDpi = 0;
    size_t lastButtonCount = 0;
    uint64_t lastButtonIdentity = 0;
    ULONGLONG lastCrowdingLog = 0;
    ULONGLONG lastCrowdingRetry = 0;
    ULONGLONG lastValidLayoutTick = 0;
    unsigned long long lastValidConfigEpoch = 0;
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

// The monitor widget is owned by another Windhawk mod. This mod never writes
// its XAML Margin, Width or position: doing so would create competing layout
// owners. Instead the icon group uses a fixed-width slot before the tray.
void RequestRefreshFor(HWND window);
bool IsTaskbarWindow(HWND window);
double TrayEdgeForContent(FrameworkElement const& content,
                          FrameworkElement const& relativeTo);
FrameworkElement FindNamedDescendant(FrameworkElement const& node,
                                     wchar_t const* name, int depth);
struct WidgetWatch {
    winrt::weak_ref<FrameworkElement> root;
    bool observed = false;
    bool visible = false;
    bool pollTimerArmed = false;
    UINT pollIntervalMs = 400;
    double width = 0;
    double x = 0;
    double trayX = 0;
    double stableX = 0;
    double lastGoodWidth = 0;
    ULONGLONG lastGoodTick = 0;
    ULONGLONG lastExpiredRefresh = 0;
    unsigned stableSamples = 0;
    ULONGLONG lastMismatchLog = 0;
};
[[clang::no_destroy]] thread_local std::unordered_map<HWND, WidgetWatch> g_widgetWatches;

UINT_PTR WidgetPollTimerId() {
    static int timerTag;
    return reinterpret_cast<UINT_PTR>(&timerTag);
}

void RemoveWidgetWatch(HWND window) {
    auto it = g_widgetWatches.find(window);
    if (it == g_widgetWatches.end()) return;
    if (it->second.pollTimerArmed) KillTimer(window, WidgetPollTimerId());
    g_widgetWatches.erase(it);
}

// Polling on the taskbar owner HWND observes Taskbar System Info injection,
// detachment, position and width without registering a XAML event delegate.
// No retained XAML delegate points into this DLL. HWND timer cleanup is
// attempted on every removal path, including failed owner-thread restores.
// Geometry updates are limited to 400 ms; we never react to our own layout
// by writing new widget coordinates or creating a recursive invalidation.
void PollWidgetGeometry(HWND window) {
    if (g_unloading || !IsTaskbarWindow(window) ||
        GetWindowThreadProcessId(window, nullptr) != GetCurrentThreadId()) return;
    auto it = g_widgetWatches.find(window);
    if (it == g_widgetWatches.end()) return;
    auto root = it->second.root.get();
    if (!root) return;
    try {
        auto monitor = FindDirectChildByName(root, L"WindhawkTaskbarSystemInfo");
        bool visible = monitor && monitor.Visibility() == Visibility::Visible;
        double width = 0, x = 0;
        if (visible) {
            width = monitor.ActualWidth();
            if (!std::isfinite(width) || width <= 2) width = monitor.Width();
            if (!std::isfinite(width) || width <= 2) width = 0;
            x = ElementX(monitor, root);
            if (!std::isfinite(x)) x = 0;
        }
        auto xaml = root.XamlRoot();
        auto content = xaml ? xaml.Content().try_as<FrameworkElement>() : nullptr;
        if (!content || !std::isfinite(content.ActualWidth()) ||
            content.ActualWidth() <= 0) return; // XAML host is rebuilding.
        double trayX = TrayEdgeForContent(content, root);
        if (!std::isfinite(trayX) || trayX <= 0) return;
        auto& state = it->second;
        // Poll actively when the widget is present or the split is crowded.
        // In the ordinary no-widget case, use a slower watch for late injection.
        UINT interval = (visible || state.lastGoodWidth > 2 ||
                         !g_uiStates[window].layoutValid) ? 400 : 1600;
        if (state.pollTimerArmed && state.pollIntervalMs != interval &&
            SetTimer(window, WidgetPollTimerId(), interval, nullptr))
            state.pollIntervalMs = interval;
        auto moved = [](double a, double b) {
            return !std::isfinite(a) || !std::isfinite(b) ||
                std::abs(a-b) > 1.0;
        };
        ULONGLONG now = GetTickCount64();
        bool widthChanged = !state.observed || state.visible != visible ||
                            moved(state.width, width);
        bool positionChanged = !state.observed || moved(state.x, x);
        bool trayChanged = !state.observed || moved(state.trayX, trayX);
        if (visible && width > 2 && std::isfinite(x) && x >= 0) {
            state.lastGoodWidth = width;
            state.lastGoodTick = now;
            if (positionChanged) state.stableSamples = 0;
            else if (state.stableSamples < 3) ++state.stableSamples;
        } else state.stableSamples = 0;
        bool stableChanged = visible && width > 2 && state.stableSamples == 2 &&
            moved(state.stableX, x);
        if (stableChanged) state.stableX = x;
        bool reservationExpired = !visible && state.lastGoodWidth > 2 &&
            now - state.lastGoodTick >= 1800 &&
            (!state.lastExpiredRefresh || now - state.lastExpiredRefresh > 30000);
        if (reservationExpired) {
            state.lastGoodWidth = 0;
            state.lastExpiredRefresh = now;
        }
        if (!widthChanged && !positionChanged && !trayChanged &&
            !stableChanged && !reservationExpired) {
            // Recover after a temporary crowded/native fallback, even if the
            // widget's dimensions have not changed since the last poll.
            auto& ui = g_uiStates[window];
            if (!ui.layoutValid && now - ui.lastCrowdingRetry > 1000) {
                ui.lastCrowdingRetry = now;
                RequestRefreshFor(window);
            }
            return;
        }
        state.observed = true;
        state.visible = visible;
        state.width = width;
        state.x = x;
        state.trayX = trayX;
        // A widget configured in the middle cannot be moved by this mod:
        // report the mismatch without chasing its animated X coordinate.
        if (visible && width > 2 && std::isfinite(trayX) &&
            std::abs(x - (trayX - width)) > 32 &&
            (state.lastMismatchLog == 0 || now - state.lastMismatchLog > 30000)) {
            state.lastMismatchLog = now;
            Wh_Log(L"Taskbar System Info HWND %p is %.1f DIP away from right-side slot; change its own placement if icons overlap",
                   window, x - (trayX - width));
        }
        // Only boundary changes invalidate a frozen drag. Ordinary XAML
        // animations of the widget cannot cancel an unrelated user gesture.
        auto& ui = g_uiStates[window];
        if (widthChanged || trayChanged || stableChanged || reservationExpired) {
            ++ui.geometryEpoch;
            RequestRefreshFor(window);
        }
    } catch (...) {
        // XAML can be replaced while Windows rebuilds secondary taskbars.
    }
}

void EnsureWidgetWatch(FrameworkElement const& content) {
    HWND window = g_activeTaskbarWindow;
    if (!window || !content) return;
    auto frame = FindDirectChildByClass(content, L"Taskbar.TaskbarFrame");
    auto root = frame ? FindDirectChildByName(frame, L"RootGrid") : nullptr;
    if (!root) return;
    auto found = g_widgetWatches.find(window);
    if (found != g_widgetWatches.end() &&
        found->second.root.get() == root && found->second.pollTimerArmed) return;
    RemoveWidgetWatch(window);
    auto& state = g_widgetWatches[window];
    state.root = root;
    state.pollTimerArmed = SetTimer(window, WidgetPollTimerId(), 400, nullptr) != 0;
    if (!state.pollTimerArmed)
        Wh_Log(L"Could not schedule widget geometry watcher for HWND %p", window);
    PollWidgetGeometry(window);
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

bool ButtonIsLeftSide(FrameworkElement const& element) {
    return !IsFixedRight(ButtonAppId(element));
}

// Calculate the right edge in the coordinate space requested by the caller.
// Reserve space for both the Windows tray and Taskbar System Info when present.
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

// Use the same locator for the periodic observer and the layout planner.
// SystemTrayFrame is sometimes nested in the Windows 11 XAML content tree.
double TrayEdgeForContent(FrameworkElement const& content,
                          FrameworkElement const& relativeTo) {
    double contentLeft = ElementX(content, relativeTo);
    double contentWidth = content.ActualWidth();
    double edge = contentLeft + contentWidth;
    if (!std::isfinite(edge)) return 0;
    auto tray = CachedNamedDescendant(content, L"SystemTrayFrame", g_traySearch);
    if (!tray) tray = FindDirectChildByClass(content, L"SystemTray.SystemTrayFrame");
    if (tray && tray.Visibility() == Visibility::Visible) {
        double tx = ElementX(tray, relativeTo);
        if (std::isfinite(tx) && tx > contentLeft && tx <= edge)
            edge = std::min(edge, tx);
    }
    return edge;
}

// The hardware monitor chooses its own X position. Respect its ACTUAL
// occupied interval once stable, rather than assuming a width reservation
// forces another mod to move. Until its X is stable, retain the predictable
// reserved slot to avoid a transient drift into the middle on startup.
double FixedRightEdge(FrameworkElement const& content,
                      FrameworkElement const& relativeTo) {
    double contentLeft = ElementX(content, relativeTo);
    if (!std::isfinite(contentLeft)) return 0;
    double trayEdge = TrayEdgeForContent(content, relativeTo);
    if (!std::isfinite(trayEdge) || trayEdge <= contentLeft) return contentLeft;
    double widgetWidth = 0, widgetX = 0;
    bool useActualX = false;
    auto frame = FindDirectChildByClass(content, L"Taskbar.TaskbarFrame");
    auto grid = frame ? FindDirectChildByName(frame, L"RootGrid") : nullptr;
    auto monitor = grid ? FindDirectChildByName(grid, L"WindhawkTaskbarSystemInfo") : nullptr;
    if (monitor && monitor.Visibility() == Visibility::Visible) {
        widgetWidth = monitor.ActualWidth();
        if (!std::isfinite(widgetWidth) || widgetWidth <= 2) widgetWidth = monitor.Width();
        if (!std::isfinite(widgetWidth) || widgetWidth <= 2) widgetWidth = 0;
        if (widgetWidth > 2) {
            widgetX = ElementX(monitor, relativeTo);
            useActualX = std::isfinite(widgetX) && widgetX >= contentLeft &&
                         widgetX + widgetWidth <= contentLeft + content.ActualWidth() + 2;
        }
    }
    auto it = g_widgetWatches.find(g_activeTaskbarWindow);
    ULONGLONG now = GetTickCount64();
    if (it != g_widgetWatches.end()) {
        auto& watch = it->second;
        // Brief XAML reconstruction/collapse is not an instruction to remove
        // the reservation immediately; keep the last known valid width.
        if (widgetWidth <= 2 && watch.lastGoodWidth > 2 &&
            now - watch.lastGoodTick < 1800) {
            widgetWidth = watch.lastGoodWidth;
            useActualX = false;
        }
    }
    double right = trayEdge - (widgetWidth > 2 ? widgetWidth + 6 : 0);
    if (useActualX && it != g_widgetWatches.end()) {
        auto const& watch = it->second;
        // An X-only animation is not used as an anchor until two timer
        // samples have agreed. Stable unexpected placement is an obstacle.
        if (watch.stableSamples >= 2 && watch.stableX > 0 &&
            widgetX < right - 10) {
            right = std::min(right, widgetX - 6);
        }
    }
    return std::clamp(right, contentLeft, trayEdge);
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

void EnsureSharedStateLoadedLocked() {
    if (g_sharedLoaded) return;
    g_sharedLoaded = true;
    PersistedAppState next;
    std::wstring rebuilt;
    wchar_t active = 0;
    auto marker = ReadStoredString(L"fixed-sides-commit-v3");
    if (ReadBankState(marker, next, rebuilt, active)) {
        g_sharedState = std::move(next);
        g_committedRecord = std::move(rebuilt);
        g_committedBankManifest = marker;
        g_activeStorageBank = active;
        return;
    }
    marker = ReadStoredString(L"fixed-sides-backup-v3");
    if (ReadBankState(marker, next, rebuilt, active)) {
        Wh_Log(L"Restored last valid segmented taskbar preferences");
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

bool BuildLayoutPlan(FrameworkElement const& knownRepeater) {
    g_arrangePlan.clear();
    Ui().layoutValid = false;
    Ui().slots.clear();
    try {
        auto repeater = knownRepeater ? knownRepeater : GetTaskbarRepeater();
        auto content = repeater && repeater.XamlRoot()
                           ? repeater.XamlRoot().Content().try_as<FrameworkElement>()
                           : nullptr;
        if (!repeater || !content) {
            RestoreVisualStatesForWindow(g_activeTaskbarWindow);
            return false;
        }
        // Vertical or malformed layouts are left to Windows, including scale.
        if (content.ActualWidth() < content.ActualHeight() * 2.0) {
            RestoreVisualStatesForWindow(g_activeTaskbarWindow);
            return false;
        }
        double repeaterX = ElementX(repeater, content);
        EnsureWidgetWatch(content);

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
                auto id = ButtonAppId(child);
                buttons.push_back({child, ElementWidth(child),
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

        double rightEdge = FixedRightEdge(content, content);
        if (!g_settings.systemButtonsLeft.load()) {
            ReserveNativeWidgets(content, leftEdge, rightEdge);
        }
        leftEdge += g_settings.runningGap.load();
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

        double rightVisualScale =
            g_settings.pinnedIconScale.load() / 100.0;
        auto totalWidth = [](std::vector<ButtonInfo*> const& group) {
            double total = 0;
            for (auto item : group) total += item->width;
            return total;
        };

        double leftWidth = totalWidth(leftButtons);
        double rightWidth = totalWidth(rightButtons);
        double requested = leftWidth + rightWidth;
        double span = rightEdge - leftEdge;
        // During a transient widget measurement its declared width can grow
        // before its actual X position is updated by Taskbar System Info.
        // Reuse the PREVIOUSLY SAFE right boundary for at most 1.8 seconds,
        // but only when the real widget remains strictly to its right. This
        // prevents the whole right group briefly dropping to the native middle
        // layout due only to a provisional width measurement. Real crowding
        // still falls back to Windows; never overlap another widget.
        auto& stableUi = Ui();
        if (std::isfinite(requested) && std::isfinite(span) &&
            requested > span + 0.05 && stableUi.lastValidLayoutTick &&
            GetTickCount64() - stableUi.lastValidLayoutTick < 1800 &&
            stableUi.lastValidConfigEpoch == g_configEpoch.load() &&
            stableUi.lastButtonCount == buttons.size() &&
            std::abs(stableUi.lastContentWidth - content.ActualWidth()) < 1 &&
            std::abs(stableUi.lastLeftBound - leftEdge) < 1 &&
            stableUi.lastRightBound > rightEdge &&
            requested <= stableUi.lastRightBound - leftEdge + 0.05) {
            auto frame = FindDirectChildByClass(content, L"Taskbar.TaskbarFrame");
            auto grid = frame ? FindDirectChildByName(frame, L"RootGrid") : nullptr;
            auto monitor = grid
                ? FindDirectChildByName(grid, L"WindhawkTaskbarSystemInfo") : nullptr;
            if (monitor && monitor.Visibility() == Visibility::Visible) {
                double widgetX = ElementX(monitor, content);
                double trayX = TrayEdgeForContent(content, content);
                if (std::isfinite(widgetX) && std::isfinite(trayX) &&
                    stableUi.lastRightBound + 6 <= widgetX &&
                    stableUi.lastRightBound + g_settings.trayGap.load() <= trayX) {
                    rightEdge = stableUi.lastRightBound;
                    span = rightEdge - leftEdge;
                }
            }
        }
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
            RestoreVisualStatesForChildren(children);
            g_arrangePlan.clear();
            return false;
        }
        auto& ui = Ui();
        ui.leftStart = leftEdge - repeaterX;
        double x = leftEdge;
        for (auto item : leftButtons) {
            PlanElement(item->element, x - repeaterX, 1.0);
            if (!item->appId.empty())
                ui.slots.push_back({item->appId, true, x - repeaterX, item->width});
            x += item->width;
        }
        ui.leftEnd = x - repeaterX;

        x = rightEdge;
        for (auto item = rightButtons.rbegin(); item != rightButtons.rend(); ++item) {
            // Reserve the full native hit rectangle, even when an icon's
            // drawing is scaled down. Otherwise hit areas overlap invisibly.
            double nativeWidth = (*item)->width;
            x -= nativeWidth;
            PlanElement((*item)->element, x - repeaterX, rightVisualScale);
            if (!(*item)->appId.empty())
                ui.slots.push_back({(*item)->appId, false, x - repeaterX, nativeWidth});
            // Keep full-size hit rectangles even when the icon is scaled.
        }
        ui.rightStart = x - repeaterX;
        ui.rightEnd = rightEdge - repeaterX;
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
            changed(ui.lastDpi, dpi) ||
            ui.lastButtonCount != buttons.size() ||
            ui.lastButtonIdentity != buttonIdentity) {
            ++ui.geometryEpoch;
            ui.geometryInitialized = true;
        }
        ui.lastContentWidth = content.ActualWidth();
        ui.lastContentHeight = content.ActualHeight();
        ui.lastRepeaterX = repeaterX;
        ui.lastLeftBound = leftEdge;
        ui.lastRightBound = rightEdge;
        ui.lastLeftGroupWidth = leftWidth;
        ui.lastRightGroupWidth = rightWidth;
        ui.lastDpi = dpi;
        ui.lastButtonCount = buttons.size();
        ui.lastButtonIdentity = buttonIdentity;
        ui.lastValidLayoutTick = GetTickCount64();
        ui.lastValidConfigEpoch = g_configEpoch.load();
        ui.layoutValid = true;
        return true;
    } catch (...) {
        g_arrangePlan.clear();
        RestoreVisualStatesForWindow(g_activeTaskbarWindow);
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
    IUIElement uiElement = rectangle;
    auto vtable = *reinterpret_cast<void***>(winrt::get_abi(uiElement));
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

    // A primary and a secondary taskbar have distinct XAML roots and layouts.
    // Compare the exact layout ABI identity instead of assuming one taskbar.
    HWND window = nullptr;
    FrameworkElement arrangeRepeater{nullptr};
    try {
        winrt::Windows::Foundation::IUnknown caller{nullptr};
        winrt::copy_from_abi(caller, self);
        auto layout = caller.try_as<winrt::Microsoft::UI::Xaml::Controls::Layout>();
        if (!layout) return ArrangeOverride_Original(self, context, size, resultSize);
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
            auto element = content ? FindTaskbarRepeater(content) : nullptr;
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
        return ArrangeOverride_Original(self, context, size, resultSize);
    }
    if (!window) return ArrangeOverride_Original(self, context, size, resultSize);
    ScopedTaskbarContext taskbarContext(window);
    if (!EnsureTaskbarSubclass(window)) {
        RestoreVisualStatesForWindow(window);
        return ArrangeOverride_Original(self, context, size, resultSize);
    }

    bool planReady = false;
    try {
        if (EnsureArrangeHook()) {
            planReady = BuildLayoutPlan(arrangeRepeater);
            g_useArrangePlan = planReady;
        } else {
            RestoreVisualStatesForWindow(window);
        }
    } catch (...) {
        g_arrangePlan.clear();
        g_useArrangePlan = false;
        RestoreVisualStatesForWindow(window);
    }
    HRESULT result = ArrangeOverride_Original(self, context, size, resultSize);
    g_useArrangePlan = false;
    if (g_settingsArrangeFollowup.exchange(false)) RequestRefreshAll();
    if (!g_unloading && planReady) {
        try {
            for (auto const& [key, placement] : g_arrangePlan) {
                if (auto element = placement.element.get()) {
                    ScaleElement(element, placement.scale);
                    if (std::abs(element.ActualWidth() - placement.measuredWidth) > 0.05)
                        RequestRefresh();
                }
            }
            UpdateDragPreview();
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
            Wh_Log(L"Post-arrange scaling failed for one taskbar");
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
    ScopedTaskbarContext pendingContext(pending.gesture.window);
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
        // StopAnimation may leave the old base offset behind. Commit the
        // actual X position while preserving the Y and Z offsets.
        auto offset = visual.Offset();
        offset.x = element.ActualOffset().x;
        visual.Offset(offset);
        pending.arranged = true;
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
        widths.push_back(ElementWidth(element)); // Full native arrange slots.
    }
    auto from = std::find(live.begin(), live.end(), source);
    if (from == live.end()) return;
    size_t current = static_cast<size_t>(from - live.begin());
    double centre = gesture.desiredX + gesture.width / 2;
    // Do not modify the source order while the icon crosses the empty gap.
    if (centre < gesture.sectionStart - 2 || centre > gesture.sectionEnd + 2)
        return;
    double relativeCentre = centre - gesture.sectionStart;
    size_t targetIndex = gesture.leftSide
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
                gesture.appId = ButtonAppId(source);
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
                double scale = gesture.leftSide ? 1.0 : g_settings.pinnedIconScale.load() / 100.0;
                gesture.width = ElementWidth(source) * scale;
                gesture.minX = gesture.startX;
                double right = gesture.startX + gesture.width;
                gesture.originalOrder = gesture.leftSide ? Ui().leftLive : Ui().rightLive;
                for (auto const& reference : gesture.originalOrder) {
                    if (auto item = reference.get()) {
                        double x = ElementX(item, repeater);
                        gesture.minX = std::min(gesture.minX, x);
                        right = std::max(right, x + ElementWidth(item) * scale);
                    }
                }
                gesture.maxX = std::max(gesture.minX, right - gesture.width);
                // Allow dragging to either zone while retaining native
                // pointer capture and animation safeguards.
                try {
                    auto content = repeater.XamlRoot().Content().try_as<FrameworkElement>();
                    if (content) {
                        double farRight = FixedRightEdge(content, repeater) -
                                          g_settings.trayGap.load();
                        gesture.minX = std::min(gesture.minX, 0.0);
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
    for (auto const& slot : slots) {
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
    // the live widget bounds so a move/resize cannot commit a stale zone.
    PollWidgetGeometry(gesture.window);
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
                RememberApp(visibleIds, ButtonAppId(item));
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
            if (g_unloading || ButtonAppId(source) != gesture.appId || (GetAsyncKeyState(VK_ESCAPE) & 0x8000)) {
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
            auto repeater = content ? FindTaskbarRepeater(content) : nullptr;
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
void ReleaseTaskbarUiCaches(HWND window, bool dropUnrestoredVisuals) {
    g_uiStates.erase(window);
    g_repeaterByWindow.erase(window);
    RemoveWidgetWatch(window);
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
    if (dropUnrestoredVisuals) {
        for (auto it = g_visualStates.begin(); it != g_visualStates.end();) {
            if (it->second.owner == window) it = g_visualStates.erase(it);
            else ++it;
        }
    }

    // Another taskbar on the same UI thread may still need its saved visual
    // state for native-layout restoration. Only release the bucket arrays after
    // every window-owned entry is gone and no arrange call is active.
    if (!g_unloading || g_insideArrange || g_useArrangePlan ||
        !g_uiStates.empty() || !g_layoutOwners.empty() ||
        !g_repeaterByWindow.empty() || !g_visualStates.empty() ||
        !g_traySearch.empty() || !g_widgetWatches.empty() || !owners.empty())
        return;
    // swap destroys weak COM references and releases hash buckets on the UI
    // thread, while the DLL is still loaded. clear() alone retains buckets.
    decltype(g_layoutOwners){}.swap(g_layoutOwners);
    decltype(g_repeaterByWindow){}.swap(g_repeaterByWindow);
    decltype(g_visualStates){}.swap(g_visualStates);
    decltype(g_traySearch){}.swap(g_traySearch);
    decltype(g_uiStates){}.swap(g_uiStates);
    decltype(g_widgetWatches){}.swap(g_widgetWatches);
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
    if (message == WM_TIMER && wParam == WidgetPollTimerId()) {
        PollWidgetGeometry(window);
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
        RestoreVisualStatesForWindow(window);
        ReleaseTaskbarUiCaches(window, true);
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
        bool visualStatesRestored = RestoreVisualStatesForWindow(window);
        bool nativeLayoutUpdated = false;
        try {
            if (auto repeater = GetTaskbarRepeater()) {
                // Restore original XAML geometry while Arrange hooks bypass
                // split positioning during unload, then restore visuals once
                // more if a late native state transition reintroduced a scale.
                repeater.InvalidateArrange();
                repeater.UpdateLayout();
                visualStatesRestored = RestoreVisualStatesForWindow(window) &&
                                       visualStatesRestored;
                nativeLayoutUpdated = true;
            }
        } catch (...) {
            Wh_Log(L"Native layout restoration failed on one monitor");
        }
        bool restored = nativeLayoutUpdated && visualStatesRestored;
        // On the first attempt retain failed visual states for retry. On the
        // final attempt release all other UI-owned caches even on failure.
        if (g_unloading && (restored || wParam == 1))
            ReleaseTaskbarUiCaches(window, !restored);
        return restored ? 1 : 0;
    }
    if (message == WM_DISPLAYCHANGE || message == WM_DPICHANGED ||
        message == WM_SETTINGCHANGE) {
        g_widgetsInvalidated = true;
        Ui().layoutValid = false;
        ++Ui().geometryEpoch;  // invalidates in-progress drag snapshots
        g_traySearch.erase(window);
        PollWidgetGeometry(window);
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
    for (HWND window : AllTaskbarWindows()) EnsureTaskbarSubclass(window);
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
        KillTimer(window, WidgetPollTimerId());
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
