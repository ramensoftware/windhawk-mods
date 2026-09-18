// ==WindhawkMod==
// @id              taskbar-appearance-tuner
// @name            Taskbar opacity tuner
// @name:zh-CN      任务栏透明度调节器
// @description     Adjust the opacity of the taskbar background and of the icons and text, for a clean, beautiful taskbar which is easier on the eyes and on OLED displays
// @description:zh-CN 分别调整任务栏背景与图标文字的不透明度，定制出简洁漂亮的任务栏，也更护眼、更适合 OLED 显示器
// @version         1.7.0
// @author          lzxujun
// @homepage        https://github.com/lzxujun
// @license         GPL-3.0
// @github          https://github.com/lzxujun
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues

// ==WindhawkModReadme==
/*
# Taskbar opacity tuner

> **English:** Adjust the opacity of the taskbar background and of its icons
> and text to build a clean, beautiful taskbar that fits your desktop — and a
> taskbar which blends into it is also easier on the eyes and lighter on OLED
> displays.
>
> **中文：** 调整任务栏背景与图标文字的不透明度，定制出简洁漂亮的任务栏——
> 与桌面融为一体的任务栏，也更护眼、更适合 OLED 显示器。

The taskbar is adjusted in two independent layers, each displayed on a simple
0-100 scale:

* **Taskbar background** - the material behind the icons. 0 means fully
  transparent, 100 means unchanged.
* **Icons and text** - buttons, labels and the system tray. 0 means fully
  transparent, 100 means unchanged.

Additionally:

* **Show the taskbar top line** - shows or hides the thin line at the top
  edge of the taskbar.
* The small gray rounded drag handle at the top center of the taskbar is
  always hidden while the mod is active. That handle is part of Windows itself
  (it shows while the taskbar is unlocked), it is not drawn by this mod.

## Screenshots

**Task buttons area, transparency unchanged:**

![Taskbar task button area when transparency has not been modified](https://i.imgur.com/L37dzXL.png)

**Task buttons area, transparency set to 35%:**

![Taskbar task button area when transparency is set to 35%](https://i.imgur.com/QHkfKZJ.png)

**Task buttons area, transparency state:**

![Taskbar task button area transparency state](https://i.imgur.com/DrMjBOY.png)

**System tray area, transparency state:**

![Taskbar system tray area transparency state](https://i.imgur.com/ACFLOFC.png)

## How it works

The two layers are XAML elements of the taskbar visual tree, and their
opacity is adjusted exactly, with no visual tricks: the background blends
into whatever is behind the taskbar, and the icons and text blend into the
background.

The icons and text layer is found by walking the taskbar visual tree: every
subtree which contains neither the background nor a flyout host is adjusted
as a unit. This covers all icon areas (the Start, search and Task View
buttons, the task buttons and the system tray) on every Windows build,
without depending on the class names of the individual containers.

## Compared with other mods

* **Windows 11 Taskbar Styler** can restyle the top line, the drag grip and
  the background (and much more), but it takes style rules. This mod is a
  zero-configuration dial instead: two 0-100 values cover the whole taskbar
  at once, no rules to write, and the drag grip is hidden automatically.
* **Taskbar Background Helper** and **Dynamic Taskbar Transparency** adjust
  the background only (blur/acrylic/color, or per-shell-state opacity). This
  mod adjusts the background opacity as well, but additionally the icons and
  text as a whole layer.

## Notes

* Windows 11 only (the mod relies on the XAML taskbar visual tree).
* If nothing seems to happen, make sure at least one value differs from 100.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- backgroundOpacity: 100
  $name: Background opacity (0 = transparent, 100 = unchanged)
  $name:zh-CN: 背景不透明度（0 = 全透明，100 = 不变）
  $description: >-
    Opacity of the taskbar background material, from 0 (fully transparent)
    to 100 (unchanged).
  $description:zh-CN: 任务栏背景材质的不透明度，0 表示完全透明，100 表示不调整。
- iconsOpacity: 100
  $name: Icons and text opacity (0 = transparent, 100 = unchanged)
  $name:zh-CN: 图标和文字不透明度（0 = 全透明，100 = 不变）
  $description: >-
    Opacity of the task buttons, tray icons and labels, from 0 (fully
    transparent) to 100 (unchanged).
  $description:zh-CN: 任务按钮、托盘图标和文字标签的不透明度，0 表示完全透明，100 表示不调整。
- topLine: true
  $name: Show the taskbar top line
  $name:zh-CN: 显示任务栏顶部线
  $description: >-
    Whether to show the thin line at the top edge of the taskbar. When
    disabled, the line is hidden by clearing the taskbar border stroke.
  $description:zh-CN: 是否显示任务栏顶部的细线。关闭后通过清除任务栏边框描边来隐藏该线。
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cwchar>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;

////////////////////////////////////////////////////////////////////////////////
// Mod logic

// All values are shown to the user on a 0-100 scale.
struct {
    int backgroundOpacity;  // 0..100, 100 = no change
    int iconsOpacity;       // 0..100, 100 = no change
    bool topLine;           // false = hide the taskbar top line
} g_settings;

std::atomic<bool> g_unloading{false};

// The taskbar XAML tree is dumped to the log once per mod load, and only
// after it turned out to be populated.
std::atomic<bool> g_treeDumped{false};

// Finds a direct child by its XAML name.
FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (child) {
            try {
                if (child.Name() == name) {
                    return child;
                }
            } catch (...) {
            }
        }
    }
    return nullptr;
}

// Finds a direct child by its XAML class name.
FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (child) {
            try {
                if (winrt::get_class_name(child) == className) {
                    return child;
                }
            } catch (...) {
            }
        }
    }
    return nullptr;
}

struct AppearanceState {
    // The XamlRoot content element of the taskbar this state belongs to. It is
    // used to look the state up again when the settings change.
    winrt::weak_ref<FrameworkElement> root;
    winrt::weak_ref<FrameworkElement> backgroundFill;
    double originalFillOpacity = 1.0;
    // The taskbar top line rectangle and its original state.
    winrt::weak_ref<FrameworkElement> topLine;
    double originalTopLineThickness = 0.0;
    Media::Brush originalTopLineFill{nullptr};
    Visibility originalTopLineVisibility = Visibility::Visible;
    // The taskbar drag grip handle (Rectangle#Gripper) and its original state.
    winrt::weak_ref<FrameworkElement> grip;
    Visibility originalGripVisibility = Visibility::Visible;
    // Foreground elements with their original opacity.
    std::vector<std::pair<winrt::weak_ref<FrameworkElement>, double>>
        foregroundOpacity;
};

// The state keeps only weak XAML references (the only strong one,
// originalTopLineFill, is released with the state on the taskbar UI thread),
// which must not happen from the automatic destructor at process shutdown
// (Explorer's shutdown path runs global destructors after the XAML core is
// gone). States are released via RestoreAllStates() on the taskbar UI thread
// instead, and the buffer is dropped in Wh_ModUninit.
[[clang::no_destroy]] std::optional<std::vector<AppearanceState>> g_states;

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

// Recursively enumerates all descendants of the given element. The walk is
// bounded: an unexpectedly deep tree stops the enumeration instead of
// overflowing the stack. The callback is a template parameter so that no
// std::function is allocated per call.
template <typename T>
void EnumDescendants(FrameworkElement element, T&& enumCallback, int depth = 0) {
    constexpr int kMaxDepth = 12;
    if (depth > kMaxDepth) {
        return;
    }

    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }

        if (enumCallback(child)) {
            return;
        }

        EnumDescendants(child, enumCallback, depth + 1);
    }
}

FrameworkElement FindDescendantByName(FrameworkElement element, PCWSTR name) {
    FrameworkElement result = nullptr;
    EnumDescendants(element, [&](FrameworkElement child) {
        if (child.Name() == name) {
            result = child;
            return true;
        }
        return false;
    });
    return result;
}

std::wstring DescribeElement(FrameworkElement element) {
    std::wstring result;
    try {
        result = winrt::get_class_name(element).c_str();
    } catch (...) {
        result = L"<unknown>";
    }

    try {
        auto name = element.Name();
        if (!name.empty()) {
            result += L"#";
            result += name.c_str();
        }
    } catch (...) {
    }

    return result;
}

// Describes the bounds of the element relative to the content element as
// "(x, y w x h)", for the log.
std::wstring DescribeRect(FrameworkElement element, FrameworkElement content) {
    try {
        auto origin = element.TransformToVisual(content)
                          .TransformPoint(
                              winrt::Windows::Foundation::Point{0, 0});
        wchar_t buffer[128];
        swprintf_s(buffer, L"(%.0f,%.0f %.0fx%.0f)", origin.X, origin.Y,
                   element.ActualWidth(), element.ActualHeight());
        return buffer;
    } catch (...) {
        return L"(?)";
    }
}

// Flyouts (the hover window previews, the tray overflow menu, Alt+Tab, ...) are
// hosted by elements inside the taskbar tree and render outside of the visible
// taskbar. Adjusting them leaks the adjustment outside of the taskbar (a gray
// strip above it), so they are skipped.
bool IsFlyoutHost(FrameworkElement element) {
    std::wstring description = DescribeElement(element);
    std::wstring_view view(description);
    return view.find(L"Popup") != std::wstring_view::npos ||
           view.find(L"Switcher") != std::wstring_view::npos ||
           view.find(L"TaskbarExperienceHost") != std::wstring_view::npos ||
           view.find(L"Flyout") != std::wstring_view::npos;
}

// The background rectangles (the taskbar background and the background of the
// hover flyout) are not part of the icon and text layer: the taskbar background
// is adjusted separately, and the flyout background must not be touched at all.
bool IsBackgroundRectangle(FrameworkElement element) {
    try {
        auto name = element.Name();
        return name == L"BackgroundFill" || name == L"BackgroundStroke";
    } catch (...) {
        return false;
    }
}

// System chrome inside the taskbar which is not icon and text content: the
// drag grip handle (shown while the taskbar is unlocked) and the stroke which
// closes the taskbar at the bottom screen edge. Adjusting these has no useful
// effect and the grip would show up as a gray bar of its own.
bool IsSystemChromeElement(FrameworkElement element) {
    try {
        if (std::wstring_view(winrt::get_class_name(element).c_str())
                .starts_with(L"Taskbar.Gripper")) {
            return true;
        }
    } catch (...) {
    }
    try {
        if (element.Name() == L"ScreenEdgeStroke") {
            return true;
        }
    } catch (...) {
    }
    return false;
}

// Returns true when the subtree is pure icon and text content, i.e. it contains
// neither the background nor a flyout host, so that it can be adjusted as a
// single unit. The walk is bounded, an unexpectedly large subtree counts as
// impure, in which case the caller descends into it instead.
bool IsPureIconSubtree(FrameworkElement element) {
    constexpr int kMaxDepth = 12;
    constexpr int kMaxElements = 400;

    std::vector<std::pair<FrameworkElement, int>> stack;
    stack.push_back({element, 0});

    int visited = 0;
    while (!stack.empty()) {
        auto [current, depth] = stack.back();
        stack.pop_back();

        if (!current || IsBackgroundRectangle(current) ||
            IsFlyoutHost(current) || depth > kMaxDepth) {
            return false;
        }

        if (++visited > kMaxElements) {
            return false;
        }

        int childrenCount = 0;
        try {
            childrenCount = Media::VisualTreeHelper::GetChildrenCount(current);
        } catch (...) {
            return false;
        }

        for (int i = 0; i < childrenCount; i++) {
            FrameworkElement child = nullptr;
            try {
                child = Media::VisualTreeHelper::GetChild(current, i)
                            .try_as<FrameworkElement>();
            } catch (...) {
            }
            if (child) {
                stack.push_back({child, depth + 1});
            }
        }
    }

    return true;
}

bool ContainsElement(std::vector<FrameworkElement> const& elements,
                     FrameworkElement element) {
    return std::find(elements.begin(), elements.end(), element) !=
           elements.end();
}

bool HasCollectedAncestor(std::vector<FrameworkElement> const& elements,
                          FrameworkElement element) {
    try {
        auto parent = Media::VisualTreeHelper::GetParent(element)
                          .try_as<FrameworkElement>();
        while (parent) {
            if (ContainsElement(elements, parent)) {
                return true;
            }
            parent = Media::VisualTreeHelper::GetParent(parent)
                         .try_as<FrameworkElement>();
        }
    } catch (...) {
    }

    return false;
}

// The taskbar XAML tree looks roughly like this (Windows 11):
//
//   <content element of the taskbar XamlRoot>
//     Taskbar.TaskbarFrame
//       Grid#RootGrid
//         Taskbar.TaskbarBackground#BackgroundControl
//           Grid
//             Rectangle#BackgroundFill
//             Rectangle#BackgroundStroke
//         ... Start/Search/Task View buttons, task buttons, badges, gripper ...
//         Popup                    <- hover previews and other flyouts
//     SystemTray.SystemTrayFrame
//
// The icon and text layer is therefore everything below the content element
// except the background and except flyout hosts. The class names of the
// individual containers differ between Windows builds, so instead of matching a
// hard-coded list of class names the tree is walked and every subtree which
// contains neither the background nor a flyout host is adjusted as a unit.
void CollectIconLayers(FrameworkElement element,
                       FrameworkElement content,
                       double bandTop,
                       double bandBottom,
                       int depth,
                       std::vector<FrameworkElement>& elements) {
    constexpr int kMaxDepth = 12;

    if (!element || depth > kMaxDepth) {
        return;
    }

    if (IsFlyoutHost(element) || IsBackgroundRectangle(element) ||
        IsSystemChromeElement(element) ||
        ContainsElement(elements, element) ||
        HasCollectedAncestor(elements, element)) {
        return;
    }

    bool visible = true;
    try {
        visible = element.Visibility() == Visibility::Visible &&
                  element.ActualWidth() > 0 && element.ActualHeight() > 0;
    } catch (...) {
    }

    if (visible && IsPureIconSubtree(element)) {
        // Elements outside of the visible taskbar area leak the adjustment
        // outside of the taskbar (a gray strip above it), so only elements
        // fully inside the visible band qualify.
        bool withinVisibleTaskbar = true;
        try {
            auto origin = element.TransformToVisual(content)
                              .TransformPoint(
                                  winrt::Windows::Foundation::Point{0, 0});
            withinVisibleTaskbar =
                origin.X >= -1 && origin.Y >= -1 &&
                origin.X + element.ActualWidth() <= content.ActualWidth() + 1 &&
                origin.Y + element.ActualHeight() <= content.ActualHeight() + 1;

            // The island extends above the visible taskbar: the element must
            // also lie within the background area and not just fit into the
            // island.
            if (withinVisibleTaskbar && bandBottom > bandTop) {
                withinVisibleTaskbar =
                    origin.Y >= bandTop - 1 &&
                    origin.Y + element.ActualHeight() <= bandBottom + 1;
            }
        } catch (...) {
        }

        if (withinVisibleTaskbar) {
            elements.push_back(element);
            Wh_Log(L"Icon layer: %s %s", DescribeElement(element).c_str(),
                   DescribeRect(element, content).c_str());
            return;
        }
    }

    // Either a mixed container (it holds the background or a flyout), or an
    // element which is not fully inside the visible taskbar: descend into the
    // children.
    int childrenCount = 0;
    try {
        childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    } catch (...) {
        return;
    }

    for (int i = 0; i < childrenCount; i++) {
        FrameworkElement child = nullptr;
        try {
            child = Media::VisualTreeHelper::GetChild(element, i)
                        .try_as<FrameworkElement>();
        } catch (...) {
        }
        CollectIconLayers(child, content, bandTop, bandBottom, depth + 1,
                          elements);
    }
}

// Dumps a bounded part of the visual tree to the log, one line per element:
// depth, class name and (if set) the element name. Used for diagnostics when
// expected elements are missing on an unknown Windows build.
void DumpVisualTree(FrameworkElement root, int maxDepth, int maxElements) {
    int logged = 0;

    auto walk = [&](auto&& self, FrameworkElement element, int depth) -> void {
        if (!element || depth > maxDepth || logged >= maxElements) {
            return;
        }

        logged++;
        std::wstring line((size_t)depth * 2, L' ');
        line += DescribeElement(element);
        Wh_Log(L"%s", line.c_str());

        int childrenCount = 0;
        try {
            childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
        } catch (...) {
        }

        for (int i = 0; i < childrenCount && logged < maxElements; i++) {
            FrameworkElement child = nullptr;
            try {
                child = Media::VisualTreeHelper::GetChild(element, i)
                            .try_as<FrameworkElement>();
            } catch (...) {
            }
            self(self, child, depth + 1);
        }
    };

    walk(walk, root, 0);
}

// Collects the containers which form the icons and text layer. Elements which
// contain the background, or which are already covered by another collected
// element, are skipped, so that an adjustment is never applied twice and the
// background is never affected by a foreground adjustment.
//
// bandTop/bandBottom are the vertical bounds of the visible taskbar area in
// content coordinates, as given by the background rectangle. Elements outside
// of them are split up instead of being adjusted as a whole: the XAML island
// which hosts the taskbar is larger than the visible taskbar (the extra space
// above it hosts flyouts), and an adjustment which lands there shows up as a
// gray strip above the taskbar.
std::vector<FrameworkElement> CollectForegroundElements(
    FrameworkElement content, double bandTop, double bandBottom) {
    std::vector<FrameworkElement> elements;
    CollectIconLayers(content, content, bandTop, bandBottom, 0, elements);

    Wh_Log(L"Icon layers: %d, content %.0fx%.0f, visible band %.0f..%.0f",
           (int)elements.size(), content.ActualWidth(), content.ActualHeight(),
           bandTop, bandBottom);

    if (elements.empty()) {
        static bool dumped = false;
        if (!dumped) {
            dumped = true;
            Wh_Log(L"Icon and text layer not found");
        }
    }

    return elements;
}

void RestoreState(AppearanceState& state) {
    if (auto backgroundFill = state.backgroundFill.get()) {
        backgroundFill.Opacity(state.originalFillOpacity);
    }

    if (auto topLine = state.topLine.get()) {
        topLine.Visibility(state.originalTopLineVisibility);
        if (auto rect = topLine.try_as<Shapes::Rectangle>()) {
            rect.Fill(state.originalTopLineFill);
            rect.StrokeThickness(state.originalTopLineThickness);
        }
    }

    if (auto grip = state.grip.get()) {
        grip.Visibility(state.originalGripVisibility);
    }

    for (auto& [element, originalOpacity] : state.foregroundOpacity) {
        if (auto elem = element.get()) {
            elem.Opacity(originalOpacity);
        }
    }
}

// Restores all tracked taskbars and forgets about them. Used when all settings
// are back to their defaults, or when the mod is unloading, so that nothing is
// left adjusted.
void RestoreAllStates() {
    if (g_states) {
        for (auto& state : *g_states) {
            RestoreState(state);
        }

        g_states->clear();
    }
}

std::pair<AppearanceState*, size_t> FindStateForRoot(
    FrameworkElement root) {
    if (g_states) {
        for (size_t i = 0; i < g_states->size(); i++) {
            if (auto stateRoot = (*g_states)[i].root.get();
                stateRoot && stateRoot == root) {
                return {&(*g_states)[i], i};
            }
        }
    }

    return {nullptr, 0};
}

bool IsNeutral() {
    return g_settings.backgroundOpacity == 100 &&
           g_settings.iconsOpacity == 100 && g_settings.topLine;
}

// The vertical bounds of the visible taskbar area in content coordinates.
// The island which hosts the taskbar is larger than the visible taskbar (the
// extra space above it hosts flyouts), and an adjustment which lands there
// shows up as a gray strip above the taskbar. The background rectangle is the
// visible taskbar, so it defines the band.
std::pair<double, double> GetVisibleBand(FrameworkElement content,
                                         FrameworkElement backgroundFill) {
    double bandTop = 0.0;
    double bandBottom = content.ActualHeight();
    try {
        auto origin = backgroundFill.TransformToVisual(content)
                          .TransformPoint(
                              winrt::Windows::Foundation::Point{0, 0});
        bandTop = origin.Y;
        bandBottom = origin.Y + backgroundFill.ActualHeight();
    } catch (...) {
    }
    return {bandTop, bandBottom};
}

// Looks the thin line at the top edge of the taskbar up and hides it on
// request. The line is drawn by the taskbar border rectangle
// (Rectangle#BackgroundStroke, a sibling of Rectangle#BackgroundFill).
// Depending on the Windows version it can be drawn with a fill or with a
// stroke, so both are cleared when hiding, and the element itself is
// collapsed. It is looked up among the siblings of BackgroundFill first, as
// the taskbar flyouts contain rectangles with the same names. The original
// state is always recorded, so that RestoreState can bring it back.
void ApplyTopLineStyle(FrameworkElement content,
                       FrameworkElement backgroundFillElem,
                       AppearanceState& state) {
    FrameworkElement topLineElem = nullptr;
    if (auto parent =
            Media::VisualTreeHelper::GetParent(backgroundFillElem)
                .try_as<FrameworkElement>()) {
            int childrenCount =
                Media::VisualTreeHelper::GetChildrenCount(parent);
            for (int i = 0; i < childrenCount; i++) {
                auto child = Media::VisualTreeHelper::GetChild(parent, i)
                                 .try_as<FrameworkElement>();
                if (child && child.Name() == L"BackgroundStroke") {
                    topLineElem = child;
                    break;
                }
            }
        }
    if (!topLineElem) {
        topLineElem = FindDescendantByName(content, L"BackgroundStroke");
    }

    if (!topLineElem) {
        Wh_Log(L"BackgroundStroke not found");
        return;
    }

    auto topLineRect = topLineElem.try_as<Shapes::Rectangle>();
    if (!topLineRect) {
        Wh_Log(L"BackgroundStroke is not a Rectangle");
        return;
    }

    state.topLine = winrt::make_weak(topLineRect.as<FrameworkElement>());
    state.originalTopLineThickness = topLineRect.StrokeThickness();
    state.originalTopLineFill = topLineRect.Fill();
    state.originalTopLineVisibility = topLineRect.Visibility();

    if (!g_settings.topLine) {
        topLineRect.Fill(nullptr);
        topLineRect.StrokeThickness(0);
        topLineRect.Visibility(Visibility::Collapsed);
        Wh_Log(L"Top line hidden (original stroke thickness=%.1f)",
               state.originalTopLineThickness);
    }
}

// Records and hides the drag grip handle: the small gray rounded handle at the
// top center of the taskbar (Rectangle#Gripper inside Taskbar.Gripper#
// GripperControl). It is part of Windows (it shows while the taskbar is
// unlocked) and is not affected by the opacity adjustments, so it is always
// hidden while the mod is active - a gray handle would otherwise stick out of
// an otherwise adjusted taskbar. The original state is recorded so that
// RestoreState can bring it back.
void ApplyGripStyle(FrameworkElement content, AppearanceState& state) {
    auto gripElem = FindDescendantByName(content, L"Gripper");
    if (!gripElem) {
        Wh_Log(L"Grip handle not found");
        return;
    }

    state.grip = winrt::make_weak(gripElem);
    state.originalGripVisibility = gripElem.Visibility();

    gripElem.Visibility(Visibility::Collapsed);
    Wh_Log(L"Grip handle hidden");
}

// Adjusts the background rectangle: opacity directly.
void ApplyBackgroundStyle(Shapes::Rectangle const& backgroundFill) {
    if (g_settings.backgroundOpacity != 100) {
        backgroundFill.Opacity(g_settings.backgroundOpacity / 100.0);
    }
}

// Adjusts the icon and text elements: opacity directly.
void ApplyForegroundStyle(std::vector<FrameworkElement> const& elements,
                          AppearanceState& state) {
    Wh_Log(L"Foreground elements found: %d", (int)elements.size());

    if (g_settings.iconsOpacity == 100) {
        return;
    }

    for (auto& element : elements) {
        Wh_Log(L"Foreground element: %s", DescribeElement(element).c_str());

        state.foregroundOpacity.emplace_back(winrt::make_weak(element),
                                             element.Opacity());
        element.Opacity(g_settings.iconsOpacity / 100.0);
    }
}

// Locates the taskbar background rectangle. The known tree path is walked
// explicitly first: flyouts contain rectangles with the same names, and a
// depth-first search just takes whatever it reaches first. The search falls
// back to a bounded tree walk for Windows builds with a different structure.
FrameworkElement FindBackgroundFill(FrameworkElement content) {
    FrameworkElement child = content;
    if ((child = FindChildByName(child, L"RootGrid")) &&
        (child = FindChildByName(child, L"BackgroundControl")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.Grid")) &&
        (child = FindChildByName(child, L"BackgroundFill"))) {
        return child;
    }

    return FindDescendantByName(content, L"BackgroundFill");
}

bool ApplyStyle(XamlRoot xamlRoot) {
    auto content = xamlRoot.Content().as<FrameworkElement>();
    if (!content) {
        return false;
    }

    Wh_Log(L"Applying settings: background opacity=%d, icons opacity=%d",
           g_settings.backgroundOpacity, g_settings.iconsOpacity);

    // The visual tree is keyed by the root content element.
    // When all settings are back to their defaults, or when the mod is
    // unloading, everything must be restored to its original appearance.
    if (g_unloading || IsNeutral()) {
        Wh_Log(L"%s, restoring original appearance",
               g_unloading ? L"Mod is unloading" : L"All settings are neutral");
        RestoreAllStates();
        return true;
    }

    auto [state, stateIndex] = FindStateForRoot(content);

    // Locate the taskbar background rectangle.
    auto backgroundFillElem = FindBackgroundFill(content);
    if (!backgroundFillElem) {
        Wh_Log(L"BackgroundFill not found (unsupported taskbar?)");
        return false;
    }

    auto backgroundFill = backgroundFillElem.try_as<Shapes::Rectangle>();
    if (!backgroundFill) {
        Wh_Log(L"BackgroundFill is not a Rectangle");
        return false;
    }

    // The tree is populated (the background rectangle exists), so dump it once
    // per mod load: the log then always contains the real element names and
    // the real structure of the Windows build it was made on.
    if (!g_treeDumped.exchange(true)) {
        DumpVisualTree(content, 7, 260);
    }

    bool hasForegroundOpacity = g_settings.iconsOpacity != 100;

    // The icon and text layer is collected up front. If it is requested but
    // comes back empty, the taskbar content exists while not being laid out
    // yet, i.e. all of its elements are still zero-sized. In that case nothing
    // is modified at all, so that the retry which covers this state doesn't
    // make the background flicker by re-applying itself over and over.
    std::vector<FrameworkElement> foregroundElements;
    if (hasForegroundOpacity) {
        auto [bandTop, bandBottom] =
            GetVisibleBand(content, backgroundFillElem);
        foregroundElements =
            CollectForegroundElements(content, bandTop, bandBottom);
        if (foregroundElements.empty()) {
            Wh_Log(L"Icon and text layer is empty, taskbar not laid out yet");
            return false;
        }
    }

    // Restore any previous state for this root before re-applying.
    if (state) {
        RestoreState(*state);
        g_states->erase(g_states->begin() + stateIndex);
    }

    AppearanceState newState;
    newState.root = winrt::make_weak(content);
    newState.backgroundFill =
        winrt::make_weak(backgroundFill.as<FrameworkElement>());
    newState.originalFillOpacity = backgroundFill.Opacity();

    ApplyTopLineStyle(content, backgroundFillElem, newState);

    ApplyGripStyle(content, newState);

    ApplyBackgroundStyle(backgroundFill);

    if (hasForegroundOpacity) {
        ApplyForegroundStyle(foregroundElements, newState);
    }

    g_states->push_back(std::move(newState));
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Taskbar plumbing (from Taskbar tray auto-hide (show on hover) by m417z)

void* CTaskBand_ITaskListWndSite_vftable;

void* CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis,
                                                           void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x48;

#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#elif defined(_M_ARM64)
    // Just use the default offset which will hopefully work in most cases.
#else
#error "Unsupported architecture"
#endif

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd) {
    HWND hTaskSwWnd =
        (HWND)FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CSecondaryTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                               taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

// Defined further down, next to the retry timer which uses them.
void WINAPI ApplyPassOnTaskbarThread(void* parameter);
void ScheduleApplyRetry(HWND hTaskbarWnd);
void StopApplyRetry(HWND hTaskbarWnd);

// The outcome of one pass over the taskbar windows of the current thread. The
// taskbar XAML tree is created asynchronously, so a pass which runs too early
// finds the taskbar but no elements, and reports allReady == false.
struct ApplyPassResult {
    bool sawTaskbar = false;
    bool allReady = true;
};

void WINAPI ApplyPassOnTaskbarThread(void* parameter) {
    ApplyPassResult* result = (ApplyPassResult*)parameter;

    Wh_Log(L"Applying settings");

    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            ApplyPassResult* result = (ApplyPassResult*)lParam;

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            XamlRoot xamlRoot = nullptr;
            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
                xamlRoot = GetTaskbarXamlRoot(hWnd);
            } else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
                xamlRoot = GetSecondaryTaskbarXamlRoot(hWnd);
            } else {
                return TRUE;
            }

            result->sawTaskbar = true;

            if (g_unloading.load()) {
                // Stop a retry which is still in flight, so that no timer
                // callback can outlive the mod, and restore unconditionally:
                // RestoreAllStates() works off the stored weak refs and does
                // not need a XamlRoot. A skipped restore would leave the
                // taskbar holding a top line fill brush which lives in the
                // mod image, which crashes Explorer once the mod is unloaded.
                StopApplyRetry(hWnd);
                RestoreAllStates();
                return TRUE;
            }

            if (!xamlRoot) {
                Wh_Log(L"Getting XamlRoot failed");
                result->allReady = false;
            } else if (!ApplyStyle(xamlRoot)) {
                // The taskbar XAML tree is created asynchronously, so on a
                // fresh start-up the elements are still missing here. Retry
                // until they exist instead of silently doing nothing.
                Wh_Log(L"ApplyStyle failed (taskbar XAML tree not ready yet?)");
                result->allReady = false;
                ScheduleApplyRetry(hWnd);
            }

            return TRUE;
        },
        (LPARAM)result);
}

////////////////////////////////////////////////////////////////////////////////
// Deferred retry
//
// TrayUI::StartTaskbar returns while the taskbar XAML tree is still being
// created: at that point its root only holds an empty Taskbar.TaskbarFrame and
// an empty SystemTray.SystemTrayFrame, and neither the background rectangles
// nor the task buttons nor the tray icons exist yet. The same happens on the
// first pass after a recompile, because the taskbar is rebuilt from scratch.
//
// Such a pass is therefore repeated until the tree is populated. The retry runs
// as a timer of the taskbar window itself, so it fires on the taskbar window
// thread and the XAML work stays where it belongs. No other thread is involved,
// which also means that unloading can never deadlock against a retry in
// flight.

// Deliberately unusual: the ID is scoped to the taskbar window, which has
// timers of its own.
constexpr UINT_PTR kApplyRetryTimerId = 0x7A9C;
constexpr UINT kApplyRetryIntervalMs = 150;
constexpr ULONGLONG kApplyRetryTotalMs = 8000;

// The retry deadline is stored per taskbar window (there can be several on
// multi-monitor systems, and a shared global would make whichever timer fires
// first drive the retry window of all of them). Only ever touched on the
// taskbar window thread.
constexpr const wchar_t* kApplyRetryDeadlineProp =
    L"Windhawk_TaskbarAppearanceTuner_RetryDeadline";

ULONGLONG GetApplyRetryDeadline(HWND hWnd) {
    auto value = (ULONGLONG)(ULONG_PTR)GetProp(hWnd, kApplyRetryDeadlineProp);
    return value;
}

void SetApplyRetryDeadline(HWND hWnd, ULONGLONG deadline) {
    SetProp(hWnd, kApplyRetryDeadlineProp,
            (HANDLE)(ULONG_PTR)deadline);
}

void RemoveApplyRetryDeadline(HWND hWnd) {
    RemoveProp(hWnd, kApplyRetryDeadlineProp);
}

void CALLBACK ApplyRetryTimerProc(HWND hWnd, UINT, UINT_PTR idEvent, DWORD) {
    if (idEvent != kApplyRetryTimerId) {
        return;
    }

    ApplyPassResult result;
    ApplyPassOnTaskbarThread(&result);

    if ((result.sawTaskbar && result.allReady) ||
        GetTickCount64() >= GetApplyRetryDeadline(hWnd)) {
        Wh_Log(L"Apply retry finished (tree ready: %s)",
               result.allReady ? L"yes" : L"no");
        KillTimer(hWnd, kApplyRetryTimerId);
        RemoveApplyRetryDeadline(hWnd);
    }
}

// Must be called on the taskbar window thread.
void ScheduleApplyRetry(HWND hTaskbarWnd) {
    // The deadline is only ever set once. The passes driven by the timer itself
    // report the same "not ready" state, and must not extend the retry window,
    // otherwise the timer would never stop.
    if (GetApplyRetryDeadline(hTaskbarWnd) == 0) {
        SetApplyRetryDeadline(hTaskbarWnd,
                              GetTickCount64() + kApplyRetryTotalMs);
    }

    // Idempotent: re-arming a running timer just restarts it.
    if (SetTimer(hTaskbarWnd, kApplyRetryTimerId, kApplyRetryIntervalMs,
                 ApplyRetryTimerProc)) {
        Wh_Log(L"Taskbar XAML tree not ready, retrying every %u ms",
               kApplyRetryIntervalMs);
    } else {
        Wh_Log(L"Failed to start the apply retry timer");
    }
}

// Must be called on the taskbar window thread.
void StopApplyRetry(HWND hTaskbarWnd) {
    KillTimer(hTaskbarWnd, kApplyRetryTimerId);
    RemoveApplyRetryDeadline(hTaskbarWnd);
}

using TrayUI_StartTaskbar_t = void(WINAPI*)(void* pThis);
TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original;
void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    Wh_Log(L">");

    TrayUI_StartTaskbar_Original(pThis);

    // TrayUI::StartTaskbar runs on the taskbar window thread.
    ApplyPassResult result;
    ApplyPassOnTaskbarThread(&result);
}

using CSecondaryTray_GetTrayWindow_t = HWND(WINAPI*)(void* pThis);
CSecondaryTray_GetTrayWindow_t CSecondaryTray_GetTrayWindow_Original;

using CSecondaryTray_InitModelAndHost_t = void(WINAPI*)(void* pThis,
                                                        void* taskbarModel);
CSecondaryTray_InitModelAndHost_t CSecondaryTray_InitModelAndHost_Original;
void WINAPI CSecondaryTray_InitModelAndHost_Hook(void* pThis,
                                                 void* taskbarModel) {
    Wh_Log(L">");

    CSecondaryTray_InitModelAndHost_Original(pThis, taskbarModel);

    HWND taskbarWnd = CSecondaryTray_GetTrayWindow_Original(pThis);

    auto xamlRoot = GetSecondaryTaskbarXamlRoot(taskbarWnd);
    if (!xamlRoot) {
        Wh_Log(L"Getting XamlRoot failed");
        return;
    }

    if (!ApplyStyle(xamlRoot)) {
        // The secondary taskbar runs into the same start-up race as the main
        // one: its XAML tree is still being created.
        ScheduleApplyRetry(taskbarWnd);
    }
}

void ApplySettings(HWND hTaskbarWnd) {
    ApplyPassResult result;
    if (!RunFromWindowThread(hTaskbarWnd, ApplyPassOnTaskbarThread, &result)) {
        Wh_Log(L"RunFromWindowThread failed");
    }
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CSecondaryTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
            &TrayUI_StartTaskbar_Original,
            TrayUI_StartTaskbar_Hook,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CSecondaryTray::GetTrayWindow(void))"},
            &CSecondaryTray_GetTrayWindow_Original,
        },
        {
            {LR"(public: virtual void __cdecl CSecondaryTray::InitModelAndHost(struct winrt::WindowsUdk::UI::Shell::TaskbarModel))"},
            &CSecondaryTray_InitModelAndHost_Original,
            CSecondaryTray_InitModelAndHost_Hook,
        },
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

void LoadSettings() {
    // Values are clamped so that an out-of-range configuration cannot make
    // the taskbar more than fully transparent or more than fully opaque.
    g_settings.backgroundOpacity =
        std::clamp(Wh_GetIntSetting(L"backgroundOpacity"), 0, 100);
    g_settings.iconsOpacity =
        std::clamp(Wh_GetIntSetting(L"iconsOpacity"), 0, 100);
    g_settings.topLine = Wh_GetIntSetting(L"topLine") != 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    // Wh_ModInit can run again without the DLL having been unloaded.
    g_treeDumped = false;
    g_states.emplace();

    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    // RestoreAllStates() already ran on the taskbar UI thread during unload
    // (see ApplyPassOnTaskbarThread), so the vector is empty here. Dropping
    // the buffer keeps the automatic destructor from ever touching the strong
    // XAML references at process shutdown.
    g_states.reset();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }
}
