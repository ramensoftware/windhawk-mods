// ==WindhawkMod==
// @id              taskbar-split
// @name            Taskbar Split: Running Left, Pinned Right
// @description     Places running apps on the left and closed pinned apps on the right, with flexible empty space between them (Windows 11).
// @version         0.2.2
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

`[Start/System] [Running apps]  <flexible empty space>  [Closed pinned apps] [Tray/Clock]`

Launching a pinned app moves it to the left zone and restores its normal size.
Closing it returns it to the right zone, where pinned icons can be made smaller
and packed more densely. The persistent Windows pin list is not changed.

Targets the horizontal primary taskbar on Windows 11 x64 and ARM64.
Disable the mod to immediately return to the standard Windows layout.

Unlike "Taskbar Start Button Centered Origin", this mod splits by running
versus closed pinned apps, not by window position on the screen.
Do not combine with "Start button always on the left",
"Taskbar Start Button Centered Origin" (taskbar-centered-start-split-icons), or other mods that
reposition or scale taskbar buttons. They can override the same layout.
On crowded taskbars, reduced spacing can overlap buttons; reduce the pinned
icon size or the middle gap, or unpin applications to free space.

Positioning and pinned scaling currently use XAML render properties. Native
overflow decisions retain the original layout, and the overflow button is not
repositioned. Click targets, previews, jump lists and drag insertion positions
still require verification on Windows before this version is considered ready.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- leftPadding: 8
  $name: Left edge padding
  $description: Empty space before the first system button, in device-independent pixels (DIPs).
- runningGap: 8
  $name: Gap after system buttons
  $description: Space between Start/Search/Widgets/Task View and running apps.
- trayGap: 8
  $name: Gap before tray
  $description: Space between closed pinned apps and the notification area.
- middleGap: 48
  $name: Minimum middle gap
  $description: Preferred minimum empty space between running and closed pinned groups. When crowded, icon spacing is compressed before this gap is reduced.
- pinnedIconScale: 100
  $name: Closed pinned icon size
  $description: Size and packing density of icons in the right group, as a percentage from 50 to 100. Running icons always use 100%.
- systemButtonsLeft: true
  $name: Keep system buttons on the left
  $description: Put Start, Search, Widgets and Task View at the left edge. Recommended for the intended split layout.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <commctrl.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

#define WH_WINRT_WINUI2
#include <winrt/Microsoft.UI.Xaml.Controls.h>

using namespace winrt::Windows::UI::Xaml;
namespace media = winrt::Windows::UI::Xaml::Media;
namespace numerics = winrt::Windows::Foundation::Numerics;

struct Settings {
    std::atomic<int> leftPadding{8};
    std::atomic<int> runningGap{8};
    std::atomic<int> trayGap{8};
    std::atomic<int> middleGap{48};
    std::atomic<int> pinnedIconScale{100};
    std::atomic<bool> systemButtonsLeft{true};
};

Settings g_settings;
std::atomic<bool> g_unloading{false};
std::atomic<bool> g_refreshQueued{false};
std::atomic<bool> g_viewHooksInstalled{false};
std::atomic<HWND> g_taskbarWindow{nullptr};
std::atomic<bool> g_taskbarSubclassed{false};
thread_local bool g_insideArrange = false;

void LoadSettings() {
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
    Thickness margin = element.Margin();
    return margin.Left + element.ActualWidth() + margin.Right;
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
    numerics::float3 originalTranslation{};
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
    applied.originalTranslation = element.Translation();
    return g_visualStates.emplace(winrt::get_abi(element), std::move(applied))
        .first->second;
}

void PlaceElement(FrameworkElement const& element,
                  double nativeX,
                  double targetVisualX,
                  double scaleValue) {
    auto& applied = EnsureVisualState(element);

    auto translation = applied.originalTranslation;
    translation.x += static_cast<float>(targetVisualX - nativeX);
    element.Translation(translation);

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
        element.Translation(applied.originalTranslation);
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
};

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

void ApplySplitLayout() {
    try {
        auto repeater = GetTaskbarRepeater();
        auto content = repeater && repeater.XamlRoot()
                           ? repeater.XamlRoot().Content().try_as<FrameworkElement>()
                           : nullptr;
        if (!repeater || !content) {
            return;
        }
        double repeaterX = ElementX(repeater, content);

        auto children = RepeaterElements(repeater);
        PruneVisualStates(children);
        std::vector<ButtonInfo> buttons;
        std::vector<FrameworkElement> systemButtons;
        buttons.reserve(children.size());
        for (auto const& child : children) {
            if (IsTaskButton(child)) {
                buttons.push_back({child, ElementWidth(child),
                                   ButtonIsRunning(child)});
            } else if (GetSystemButtonKind(child) != SystemButtonKind::None) {
                systemButtons.push_back(child);
            }
        }

        double leftEdge = g_settings.leftPadding.load();
        if (g_settings.systemButtonsLeft.load()) {
            for (auto const& button : systemButtons) {
                if (button.ActualWidth() > 0) {
                    double nativeX = repeaterX + button.ActualOffset().x;
                    PlaceElement(button, nativeX, leftEdge, 1.0);
                    leftEdge += ElementWidth(button);
                }
            }
        } else {
            for (auto const& button : systemButtons) {
                if (button.ActualWidth() <= 0) {
                    continue;
                }
                double nativeX = repeaterX + button.ActualOffset().x;
                PlaceElement(button, nativeX, nativeX, 1.0);
                leftEdge = std::max(leftEdge,
                                    nativeX + ElementWidth(button));
            }
        }
        leftEdge += g_settings.runningGap.load();

        auto tray = FindDirectChildByClass(content,
                                            L"SystemTray.SystemTrayFrame");
        double rightEdge = tray ? ElementX(tray, content)
                                : static_cast<double>(content.ActualWidth());
        rightEdge -= g_settings.trayGap.load();

        std::vector<ButtonInfo*> running;
        std::vector<ButtonInfo*> pinned;
        for (auto& button : buttons) {
            (button.running ? running : pinned).push_back(&button);
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
        double available = std::max(
            0.0, rightEdge - leftEdge - g_settings.middleGap.load());
        double requested = runningWidth + pinnedWidth;
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
            if (item->element.ActualWidth() > 0) {
                double nativeX = repeaterX + item->element.ActualOffset().x;
                PlaceElement(item->element, nativeX, x, 1.0);
            }
            x += item->width * runningStep;
        }

        x = rightEdge;
        for (auto item = pinned.rbegin(); item != pinned.rend(); ++item) {
            double visualWidth = (*item)->width * pinnedVisualScale;
            x -= visualWidth;
            if ((*item)->element.ActualWidth() > 0) {
                double nativeX =
                    repeaterX + (*item)->element.ActualOffset().x;
                PlaceElement((*item)->element, nativeX, x,
                             pinnedVisualScale);
            }
            x += visualWidth;
            x -= visualWidth * pinnedStep;
        }
    } catch (...) {
        Wh_Log(L"Taskbar Split: layout pass failed safely");
    }
}

using ArrangeOverride_t = HRESULT(WINAPI*)(
    void*, void*, winrt::Windows::Foundation::Size,
    winrt::Windows::Foundation::Size*);
ArrangeOverride_t ArrangeOverride_Original = nullptr;
HWND EnsureTaskbarWindow();

HRESULT WINAPI ArrangeOverride_Hook(
    void* self, void* context, winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize) {
    HRESULT result = ArrangeOverride_Original(self, context, size, resultSize);
    if (g_unloading || g_insideArrange) {
        return result;
    }
    struct ArrangeGuard {
        ArrangeGuard() { g_insideArrange = true; }
        ~ArrangeGuard() { g_insideArrange = false; }
    } guard;
    HWND window = EnsureTaskbarWindow();
    if (!window || !g_taskbarSubclassed || GetWindowThreadProcessId(window, nullptr) !=
                       GetCurrentThreadId()) {
        return result;
    }
    ApplySplitLayout();
    return result;
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
    if (message == RefreshMessage()) {
        g_refreshQueued = false;
        if (!g_unloading) {
            try {
                if (auto repeater = GetTaskbarRepeater()) {
                    repeater.InvalidateArrange();
                }
            } catch (winrt::hresult_error const&) {
                Wh_Log(L"Refresh failed: taskbar element disconnected");
            }
        }
        return 0;
    }
    if (message == RestoreMessage()) {
        RestoreVisualStates();
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
    } catch (winrt::hresult_error const&) {
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
    g_visualStates.clear();
    g_lastRunningState.clear();
    g_repeaterCache = nullptr;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    RequestRefresh();
}
