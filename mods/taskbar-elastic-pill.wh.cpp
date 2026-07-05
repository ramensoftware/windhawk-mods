// ==WindhawkMod==
// @id              taskbar-elastic-pill
// @name            Taskbar Elastic WinUI Pill
// @description     Injects an animated sliding pill for active taskbar items.
// @version         1.0.0
// @author          Lockframe
// @github          https://github.com/Lockframe
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Elastic WinUI Pill

Replaces the default active app pill in Windows 11's taskbar with an animated and customizable one.

![](https://i.imgur.com/PkPhNiH.gif)

---

## Contributions
Stretch animation by [Dan](https://github.com/crazyboyybs)

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- PillMarginBottom: 0
  $name: Pill margin bottom
  $description: Bottom margin for the sliding pill. Use to align with custom taskbar themes.
- PillMarginHorizontal: 0
  $name: Pill margin horizontal
  $description: Left margin for the sliding pill (Right margin is mirrored).
- PillWidth: 16
  $name: Pill width
  $description: The width of the sliding pill in pixels.
- PillHeight: 3
  $name: Pill height
  $description: The height of the sliding pill in pixels.
- PillRadius: '1.5'
  $name: Pill corner radius
  $description: The corner radius for the pill (e.g., 1.5).
- AnimationStyle: stretch
  $name: Animation style
  $options:
  - stretch: Stretch (WinUI accurate)
  - squish: Squish
  - stretchsquish: Stretch-Squish
  - bounce: Bounce
  - linear: Linear
  - easein: Ease-In
  - easeout: Ease-Out
  - easeinout: Ease-In-Out
  - stretchsquish: Stretch-Squish
- FadeTransition: false
  $name: Fade transition
  $description: Fade out and in during movement.
- HideInactiveDots: false
  $name: Hide inactive app dots
  $description: Hide the native small dot indicator for inactive apps.
- CustomColor: ""
  $name: Custom pill color
  $description: Hex color code, supports light-mode & dark-mode specific colors separated by a comma. Leave empty for system accent.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/base.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <mutex>
#include <vector>
#include <optional>
#include <cmath>
#include <limits>
#include <map>
#include <memory>

struct Settings {
    int PillMarginBottom = 0;
    int PillMarginHorizontal = 0;
    int PillWidth = 16;
    int PillHeight = 3;
    double PillRadius = 1.5;
    int AnimationStyle = 0;
    bool FadeTransition = false;
    bool HideInactiveDots = true;
    std::wstring CustomColor;
    std::optional<winrt::Windows::UI::Color> ParsedLightColor;
    std::optional<winrt::Windows::UI::Color> ParsedDarkColor;
    std::optional<winrt::Windows::UI::Color> ParsedSolidColor;
} g_settings;

std::mutex g_settingsMutex;
std::mutex g_pillsMutex;
std::vector<winrt::weak_ref<winrt::Windows::UI::Xaml::Shapes::Rectangle>> g_injectedPills;
std::atomic<bool> g_unloading{false};

struct EasingCache {
    winrt::Windows::UI::Composition::Compositor compositor{nullptr};
    winrt::Windows::UI::Composition::CompositionEasingFunction stretchLeadEase{nullptr};
    winrt::Windows::UI::Composition::CompositionEasingFunction stretchTrailEase{nullptr};
    winrt::Windows::UI::Composition::CompositionEasingFunction squishEase{nullptr};
    winrt::Windows::UI::Composition::CompositionEasingFunction linearEase{nullptr};
    winrt::Windows::UI::Composition::CompositionEasingFunction easeIn{nullptr};
    winrt::Windows::UI::Composition::CompositionEasingFunction easeOut{nullptr};
    winrt::Windows::UI::Composition::CompositionEasingFunction easeInOut{nullptr};
};

std::map<DWORD, EasingCache> g_easingCaches;
std::mutex g_easingMutex;

std::optional<winrt::Windows::UI::Color> ParseHexColor(std::wstring_view hexView) {
    if (hexView.empty()) return std::nullopt;
    std::wstring hex(hexView);
    if (hex[0] == L'#') hex.erase(0, 1);
    if (hex.length() == 6) hex = L"FF" + hex;
    if (hex.length() != 8) return std::nullopt;
    try {
        uint32_t val = std::stoul(hex, nullptr, 16);
        return winrt::Windows::UI::Color{
            (uint8_t)((val >> 24) & 0xFF),
            (uint8_t)((val >> 16) & 0xFF),
            (uint8_t)((val >> 8) & 0xFF),
            (uint8_t)(val & 0xFF)
        };
    } catch (...) {
        return std::nullopt;
    }
}

void LoadSettings() {
    std::lock_guard<std::mutex> settingsLock(g_settingsMutex);
    g_settings.PillMarginBottom = Wh_GetIntSetting(L"PillMarginBottom");
    g_settings.PillMarginHorizontal = Wh_GetIntSetting(L"PillMarginHorizontal");
    g_settings.PillWidth = Wh_GetIntSetting(L"PillWidth");
    g_settings.PillHeight = Wh_GetIntSetting(L"PillHeight");
    
    PCWSTR radiusStr = Wh_GetStringSetting(L"PillRadius");
    if (radiusStr) {
        try {
            g_settings.PillRadius = std::stod(radiusStr);
        } catch (...) { g_settings.PillRadius = 1.5; }
        Wh_FreeStringSetting(radiusStr);
    }

    PCWSTR animStr = Wh_GetStringSetting(L"AnimationStyle");
    g_settings.AnimationStyle = 0;
    if (animStr) {
        if (wcscmp(animStr, L"bounce") == 0) g_settings.AnimationStyle = 1;
        else if (wcscmp(animStr, L"linear") == 0) g_settings.AnimationStyle = 2;
        else if (wcscmp(animStr, L"squish") == 0) g_settings.AnimationStyle = 3;
        else if (wcscmp(animStr, L"easein") == 0) g_settings.AnimationStyle = 4;
        else if (wcscmp(animStr, L"easeout") == 0) g_settings.AnimationStyle = 5;
        else if (wcscmp(animStr, L"easeinout") == 0) g_settings.AnimationStyle = 6;
        else if (wcscmp(animStr, L"stretchsquish") == 0) g_settings.AnimationStyle = 7;
        Wh_FreeStringSetting(animStr);
    }
    g_settings.FadeTransition = Wh_GetIntSetting(L"FadeTransition") != 0;
    g_settings.HideInactiveDots = Wh_GetIntSetting(L"HideInactiveDots") != 0;
    g_settings.ParsedLightColor = std::nullopt;
    g_settings.ParsedDarkColor = std::nullopt;
    g_settings.ParsedSolidColor = std::nullopt;
    PCWSTR colorStr = Wh_GetStringSetting(L"CustomColor");
    if (colorStr) {
        g_settings.CustomColor = colorStr;
        Wh_FreeStringSetting(colorStr);
        
        std::wstring colorStrWs = g_settings.CustomColor;
        size_t commaPos = colorStrWs.find(L',');
        if (commaPos != std::wstring::npos) {
            std::wstring lightCol = colorStrWs.substr(0, commaPos);
            std::wstring darkCol = colorStrWs.substr(commaPos + 1);
            
            auto trim = [](std::wstring& s) {
                size_t start = s.find_first_not_of(L" \t");
                if (start == std::wstring::npos) { s = L""; return; }
                s.erase(0, start);
                s.erase(s.find_last_not_of(L" \t") + 1);
            };
            trim(lightCol);
            trim(darkCol);
            
            g_settings.ParsedLightColor = ParseHexColor(lightCol);
            g_settings.ParsedDarkColor = ParseHexColor(darkCol);
        } else {
            g_settings.ParsedSolidColor = ParseHexColor(g_settings.CustomColor);
        }
    } else {
        g_settings.CustomColor = L"";
    }
}

winrt::Windows::UI::Color GetPillColor(const Settings& localSettings) {
    bool isLight = (winrt::Windows::UI::Xaml::Application::Current().RequestedTheme() == winrt::Windows::UI::Xaml::ApplicationTheme::Light);
    
    if (!localSettings.CustomColor.empty()) {
        if (localSettings.ParsedLightColor.has_value() || localSettings.ParsedDarkColor.has_value()) {
            auto c = isLight ? localSettings.ParsedLightColor : localSettings.ParsedDarkColor;
            if (c.has_value()) return c.value();
        } else if (localSettings.ParsedSolidColor.has_value()) {
            return localSettings.ParsedSolidColor.value();
        }
    }
    
    auto res = winrt::Windows::UI::Xaml::Application::Current().Resources();
    auto resName = isLight ? L"SystemAccentColorDark1" : L"SystemAccentColorLight2";
    if (res.HasKey(winrt::box_value(resName))) {
        return winrt::unbox_value<winrt::Windows::UI::Color>(res.Lookup(winrt::box_value(resName)));
    }
    return {255, 0, 120, 212}; 
}

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Shapes;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Hosting;
using namespace winrt::Windows::UI::Composition;

FrameworkElement GetFrameworkElementFromNative(void* pThis) {
    if (!pThis) return nullptr;
    try {
        void* iUnknownPtr = (void**)pThis + 3;
        winrt::Windows::Foundation::IUnknown iUnknown;
        winrt::copy_from_abi(iUnknown, iUnknownPtr);
        return iUnknown.try_as<FrameworkElement>();
    } catch (...) {
        return nullptr;
    }
}

FrameworkElement FindChildByName(FrameworkElement const& parent, std::wstring_view name) {
    if (!parent) return nullptr;
    int count = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (child && child.Name() == name) return child;
    }
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (child) {
            auto result = FindChildByName(child, name);
            if (result) return result;
        }
    }
    return nullptr;
}

void RestoreNativeIndicators(FrameworkElement const& parent) {
    if (!parent) return;
    int count = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (child) {
            if (winrt::get_class_name(child) == L"Taskbar.TaskListButton") {
                auto indicator = FindChildByName(child, L"RunningIndicator");
                if (indicator) {
                    indicator.Opacity(1.0);
                }
            }
            RestoreNativeIndicators(child);
        }
    }
}

VisualStateGroup GetVisualStateGroup(FrameworkElement const& root, std::wstring_view groupName) {
    auto groups = VisualStateManager::GetVisualStateGroups(root);
    for (auto const& group : groups) {
        if (group.Name() == groupName) return group;
    }
    return nullptr;
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void*);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;

void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    TaskListButton_UpdateVisualStates_Original(pThis);
    if (g_unloading) return;

    auto elem = GetFrameworkElementFromNative(pThis);
    if (!elem) return;

    auto dispatcher = elem.Dispatcher();
    auto weakElem = winrt::make_weak(elem);

    dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Low, [weakElem]() {
        try {
            Settings localSettings;
            { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }

            auto button = weakElem.get();
            if (!button) return;

            auto iconPanel = FindChildByName(button, L"IconPanel");
            auto group = iconPanel ? GetVisualStateGroup(iconPanel, L"RunningIndicatorStates") : nullptr;
            auto currentState = group ? group.CurrentState() : nullptr;
            bool isActive = (currentState && currentState.Name() == L"ActiveRunningIndicator");

            // Hide native RunningIndicator
            auto runningIndicator = FindChildByName(button, L"RunningIndicator");
            if (runningIndicator) {
                if (localSettings.HideInactiveDots || isActive) {
                    runningIndicator.Opacity(0.0);
                } else {
                    runningIndicator.Opacity(1.0);
                }
            }

            // Find TaskbarFrame -> RootGrid
            FrameworkElement current = button;
            FrameworkElement rootGrid = nullptr;
            while (current) {
                if (winrt::get_class_name(current) == L"Taskbar.TaskbarFrame") {
                    rootGrid = FindChildByName(current, L"RootGrid");
                    break;
                }
                auto parent = VisualTreeHelper::GetParent(current);
                current = parent ? parent.try_as<FrameworkElement>() : nullptr;
            }

            if (!rootGrid) return;
            auto grid = rootGrid.try_as<Grid>();
            if (!grid) return;

            // Ensure ElasticPill exists in RootGrid
            auto pill = FindChildByName(grid, L"ElasticPill").try_as<winrt::Windows::UI::Xaml::Shapes::Rectangle>();
            auto visual = pill ? ElementCompositionPreview::GetElementVisual(pill) : nullptr;
            
            if (!pill || !pill.Tag()) {
                if (!pill) {
                    pill = winrt::Windows::UI::Xaml::Shapes::Rectangle();
                    pill.Name(L"ElasticPill");
                    pill.IsHitTestVisible(false);
                    pill.HorizontalAlignment(HorizontalAlignment::Left);
                    pill.VerticalAlignment(VerticalAlignment::Bottom);
                    Canvas::SetZIndex(pill, 999);
                    grid.Children().Append(pill);
                    visual = ElementCompositionPreview::GetElementVisual(pill);
                }
                if (!visual) return;
                visual.Properties().InsertScalar(L"LeftX", 0.0f);
                visual.Properties().InsertScalar(L"RightX", (float)localSettings.PillWidth);
                visual.Properties().InsertScalar(L"ShowOpacity", 1.0f);
                visual.Properties().InsertScalar(L"FadeOpacity", 1.0f);

                auto opacityExp = ElementCompositionPreview::GetElementVisual(pill).Compositor().CreateExpressionAnimation(L"props.ShowOpacity * props.FadeOpacity");
                opacityExp.SetReferenceParameter(L"props", visual.Properties());
                visual.StartAnimation(L"Opacity", opacityExp);

                pill.Tag(winrt::box_value(0.0f));
            }

            if (!visual) return;
            
            // Ensure visual properties are up-to-date with settings
            localSettings.PillHeight = (std::max)(1, localSettings.PillHeight);
            localSettings.PillWidth = (std::max)(1, localSettings.PillWidth);
            
            float lastW = -1.0f, lastH = -1.0f, lastR = -1.0f, lastMB = -1.0f, lastMH = -1.0f;
            visual.Properties().TryGetScalar(L"LayoutW", lastW);
            visual.Properties().TryGetScalar(L"LayoutH", lastH);
            visual.Properties().TryGetScalar(L"LayoutR", lastR);
            visual.Properties().TryGetScalar(L"LayoutMB", lastMB);
            visual.Properties().TryGetScalar(L"LayoutMH", lastMH);

            if (std::abs(lastW - (float)localSettings.PillWidth) > 0.001f || 
                std::abs(lastH - (float)localSettings.PillHeight) > 0.001f || 
                std::abs(lastR - (float)localSettings.PillRadius) > 0.001f || 
                std::abs(lastMB - (float)localSettings.PillMarginBottom) > 0.001f || 
                std::abs(lastMH - (float)localSettings.PillMarginHorizontal) > 0.001f) {
                
                visual.Properties().InsertScalar(L"LayoutW", (float)localSettings.PillWidth);
                visual.Properties().InsertScalar(L"LayoutH", (float)localSettings.PillHeight);
                visual.Properties().InsertScalar(L"LayoutR", (float)localSettings.PillRadius);
                visual.Properties().InsertScalar(L"LayoutMB", (float)localSettings.PillMarginBottom);
                visual.Properties().InsertScalar(L"LayoutMH", (float)localSettings.PillMarginHorizontal);
                
                pill.Height(localSettings.PillHeight);
                pill.Width(localSettings.PillWidth);
                pill.RadiusX(localSettings.PillRadius);
                pill.RadiusY(localSettings.PillRadius);
                pill.Margin({(double)localSettings.PillMarginHorizontal, 0, 0, (double)localSettings.PillMarginBottom});
            }

            // ALWAYS ensure pill is tracked, even if created by previous mod instance
            {
                std::lock_guard<std::mutex> lock(g_pillsMutex);
                auto it = std::find_if(g_injectedPills.begin(), g_injectedPills.end(), [&](const auto& wp){ return wp.get() == pill; });
                if (it == g_injectedPills.end()) {
                    if (g_injectedPills.size() > 50) {
                        g_injectedPills.erase(
                            std::remove_if(g_injectedPills.begin(), g_injectedPills.end(),
                                [](auto& wp) { return wp.get() == nullptr; }),
                            g_injectedPills.end());
                    }
                    g_injectedPills.push_back(pill);
                }
            }

            auto brush = pill.Fill().try_as<winrt::Windows::UI::Xaml::Media::SolidColorBrush>();
            if (!brush) {
                brush = winrt::Windows::UI::Xaml::Media::SolidColorBrush(GetPillColor(localSettings));
                pill.Fill(brush);
            } else {
                brush.Color(GetPillColor(localSettings));
            }

            bool anyActive = isActive;
            if (!isActive) {
                if (auto parent = winrt::Windows::UI::Xaml::Media::VisualTreeHelper::GetParent(button)) {
                    int count = winrt::Windows::UI::Xaml::Media::VisualTreeHelper::GetChildrenCount(parent);
                    for (int i = 0; i < count; i++) {
                        auto sibling = winrt::Windows::UI::Xaml::Media::VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
                        if (sibling && winrt::get_class_name(sibling) == L"Taskbar.TaskListButton") {
                            auto sIconPanel = FindChildByName(sibling, L"IconPanel");
                            auto sGroup = sIconPanel ? GetVisualStateGroup(sIconPanel, L"RunningIndicatorStates") : nullptr;
                            auto sState = sGroup ? sGroup.CurrentState() : nullptr;
                            if (sState && sState.Name() == L"ActiveRunningIndicator") {
                                anyActive = true;
                                break;
                            }
                        }
                    }
                }
            }

            auto compositor = visual.Compositor();

            float targetOpacity = -1.0f;
            if (!anyActive) targetOpacity = 0.0f;
            else if (isActive) targetOpacity = 1.0f;

            if (targetOpacity >= 0.0f) {
                float lastOpacityTarget = -1.0f;
                visual.Properties().TryGetScalar(L"LastShowOpacityTarget", lastOpacityTarget);
                if (lastOpacityTarget != targetOpacity) {
                    visual.Properties().InsertScalar(L"LastShowOpacityTarget", targetOpacity);
                    auto fadeAnim = compositor.CreateScalarKeyFrameAnimation();
                    fadeAnim.InsertKeyFrame(1.0f, targetOpacity);
                    fadeAnim.Duration(std::chrono::milliseconds(150));
                    visual.Properties().StartAnimation(L"ShowOpacity", fadeAnim);
                }
            }

            if (isActive) {
                auto transform = button.TransformToVisual(grid);
                auto point = transform.TransformPoint({0, 0});

                double pillW = pill.ActualWidth() > 0 ? pill.ActualWidth() : pill.Width();
                if (pillW < 1.0) pillW = 1.0;
                float targetX = (float)(point.X + (button.ActualWidth() / 2.0) - (pillW / 2.0)) + (float)localSettings.PillMarginHorizontal;
                
                float lastTargetX = std::numeric_limits<float>::quiet_NaN();
                visual.Properties().TryGetScalar(L"LastTargetX", lastTargetX);

                if (std::isnan(lastTargetX) || std::abs(lastTargetX - targetX) > 0.1f) {
                    visual.Properties().InsertScalar(L"LastTargetX", targetX);

                    if (std::isnan(lastTargetX)) {
                        // First appearance: snap to position without animation
                        visual.Offset(winrt::Windows::Foundation::Numerics::float3(targetX, visual.Offset().y, 0));
                        visual.Properties().InsertScalar(L"LeftX", targetX);
                        visual.Properties().InsertScalar(L"RightX", targetX + (float)pillW);
                        visual.StopAnimation(L"Scale");
                        visual.Scale(winrt::Windows::Foundation::Numerics::float3(1.0f, 1.0f, 1.0f));
                    } else {
                        // Cached easing functions (persist across calls on UI thread)
                        DWORD tid = GetCurrentThreadId();
                        EasingCache cache;
                        {
                            std::lock_guard<std::mutex> lock(g_easingMutex);
                            if (g_easingCaches.size() > 10) {
                                for (auto it = g_easingCaches.begin(); it != g_easingCaches.end(); ) {
                                    if (it->first == tid) { ++it; continue; }
                                    HANDLE hThread = OpenThread(SYNCHRONIZE, FALSE, it->first);
                                    if (hThread) {
                                        if (WaitForSingleObject(hThread, 0) == WAIT_OBJECT_0) {
                                            it = g_easingCaches.erase(it);
                                        } else {
                                            ++it;
                                        }
                                        CloseHandle(hThread);
                                    } else {
                                        it = g_easingCaches.erase(it);
                                    }
                                }
                            }
                            cache = g_easingCaches[tid];
                        }

                        if (cache.compositor != compositor) {
                            cache.compositor = compositor;
                            cache.stretchLeadEase = compositor.CreateCubicBezierEasingFunction({0.0f, 0.0f}, {0.0f, 1.0f});
                            cache.stretchTrailEase = compositor.CreateCubicBezierEasingFunction({0.5f, 0.0f}, {0.2f, 1.0f});
                            cache.squishEase = compositor.CreateCubicBezierEasingFunction({0.25f, 0.1f}, {0.25f, 1.0f});
                            cache.linearEase = compositor.CreateLinearEasingFunction();
                            cache.easeIn = compositor.CreateCubicBezierEasingFunction({0.5f, 0.0f}, {1.0f, 1.0f});
                            cache.easeOut = compositor.CreateCubicBezierEasingFunction({0.0f, 0.0f}, {0.5f, 1.0f});
                            cache.easeInOut = compositor.CreateCubicBezierEasingFunction({0.5f, 0.0f}, {0.5f, 1.0f});
                            
                            std::lock_guard<std::mutex> lock(g_easingMutex);
                            g_easingCaches[tid] = cache;
                        }

                        if (localSettings.FadeTransition) {
                            auto opacityAnim = compositor.CreateScalarKeyFrameAnimation();
                            opacityAnim.InsertKeyFrame(0.0f, 1.0f);
                            opacityAnim.InsertKeyFrame(0.5f, 0.0f);
                            opacityAnim.InsertKeyFrame(1.0f, 1.0f);
                            opacityAnim.Duration(std::chrono::milliseconds(300));
                            visual.Properties().StartAnimation(L"FadeOpacity", opacityAnim);
                        }

                        int animStyle = localSettings.AnimationStyle;

                        // Reset scale for non-scale-animating styles
                        if (animStyle == 1 || animStyle == 2 || (animStyle >= 4 && animStyle <= 6)) {
                            visual.StopAnimation(L"Scale");
                            visual.Scale(winrt::Windows::Foundation::Numerics::float3(1.0f, 1.0f, 1.0f));
                        }

                        if (animStyle == 1) { // Bounce
                            auto anim = compositor.CreateSpringScalarAnimation();
                            anim.Target(L"Offset.X");
                            anim.FinalValue(targetX);
                            anim.DampingRatio(0.6f);
                            anim.Period(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(50)));
                            visual.StartAnimation(L"Offset.X", anim);
                        } else if (animStyle == 2 || (animStyle >= 4 && animStyle <= 6)) { // Linear, Ease-In, Ease-Out, Ease-In-Out
                            auto anim = compositor.CreateScalarKeyFrameAnimation();
                            CompositionEasingFunction easing = nullptr;
                            if (animStyle == 2) easing = cache.linearEase;
                            else if (animStyle == 4) easing = cache.easeIn;
                            else if (animStyle == 5) easing = cache.easeOut;
                            else if (animStyle == 6) easing = cache.easeInOut;

                            anim.InsertKeyFrame(1.0f, targetX, easing);
                            anim.Duration(std::chrono::milliseconds(200));
                            visual.StartAnimation(L"Offset.X", anim);
                        } else if (animStyle == 3) { // Squish
                            visual.StopAnimation(L"Scale");
                            
                            auto scaleAnim = compositor.CreateVector3KeyFrameAnimation();
                            scaleAnim.InsertKeyFrame(0.0f, winrt::Windows::Foundation::Numerics::float3(1.0f, 1.0f, 1.0f));
                            scaleAnim.InsertKeyFrame(0.5f, winrt::Windows::Foundation::Numerics::float3(1.5f, 0.5f, 1.0f), cache.squishEase);
                            scaleAnim.InsertKeyFrame(1.0f, winrt::Windows::Foundation::Numerics::float3(1.0f, 1.0f, 1.0f), cache.squishEase);
                            scaleAnim.Duration(std::chrono::milliseconds(300));

                            visual.CenterPoint(winrt::Windows::Foundation::Numerics::float3((float)pillW / 2.0f, (float)localSettings.PillHeight / 2.0f, 0));
                            visual.StartAnimation(L"Scale", scaleAnim);

                            auto moveAnim = compositor.CreateScalarKeyFrameAnimation();
                            moveAnim.InsertKeyFrame(1.0f, targetX, cache.squishEase);
                            moveAnim.Duration(std::chrono::milliseconds(300));
                            visual.StartAnimation(L"Offset.X", moveAnim);
                        } else { // Stretch (0) or Stretch-Squish (7)
                            float targetLeft = targetX;
                            float targetRight = targetX + (float)pillW;
                            
                            float currentLeft = lastTargetX;
                            bool movingRight = targetLeft > currentLeft;

                            auto propSet = visual.Properties();

                            auto leftAnim = compositor.CreateScalarKeyFrameAnimation();
                            leftAnim.InsertKeyFrame(1.0f, targetLeft, movingRight ? cache.stretchTrailEase : cache.stretchLeadEase);
                            leftAnim.Duration(std::chrono::milliseconds(300));

                            auto rightAnim = compositor.CreateScalarKeyFrameAnimation();
                            rightAnim.InsertKeyFrame(1.0f, targetRight, movingRight ? cache.stretchLeadEase : cache.stretchTrailEase);
                            rightAnim.Duration(std::chrono::milliseconds(300));

                            propSet.StartAnimation(L"LeftX", leftAnim);
                            propSet.StartAnimation(L"RightX", rightAnim);

                            auto offsetExp = compositor.CreateExpressionAnimation(L"props.LeftX");
                            offsetExp.SetReferenceParameter(L"props", propSet);
                            visual.StartAnimation(L"Offset.X", offsetExp);

                            if (animStyle == 7) { // Stretch-Squish
                                auto squishAnim = compositor.CreateScalarKeyFrameAnimation();
                                squishAnim.InsertKeyFrame(0.0f, 1.0f);
                                squishAnim.InsertKeyFrame(0.5f, 0.5f, cache.squishEase);
                                squishAnim.InsertKeyFrame(1.0f, 1.0f, cache.squishEase);
                                squishAnim.Duration(std::chrono::milliseconds(300));
                                propSet.InsertScalar(L"SquishY", 1.0f);
                                propSet.StartAnimation(L"SquishY", squishAnim);
                                
                                auto scaleExp = compositor.CreateExpressionAnimation(L"Vector3((props.RightX - props.LeftX) / props.LayoutW, props.SquishY, 1.0)");
                                scaleExp.SetReferenceParameter(L"props", propSet);
                                
                                visual.CenterPoint(winrt::Windows::Foundation::Numerics::float3(0, (float)localSettings.PillHeight / 2.0f, 0));
                                visual.StartAnimation(L"Scale", scaleExp);
                            } else {
                                auto scaleExp = compositor.CreateExpressionAnimation(L"Vector3((props.RightX - props.LeftX) / props.LayoutW, 1.0, 1.0)");
                                scaleExp.SetReferenceParameter(L"props", propSet);
                                
                                visual.CenterPoint(winrt::Windows::Foundation::Numerics::float3(0, 0, 0));
                                visual.StartAnimation(L"Scale", scaleExp);
                            }
                        }
                    }
                }
            }
        } catch (...) {
            Wh_Log(L"Exception in UpdateVisualStates hook");
        }
    });
}

std::atomic<bool> g_taskbarViewDllLoaded = false;
HMODULE GetTaskbarViewModuleHandle() {
    HMODULE m = GetModuleHandle(L"Taskbar.View.dll");
    return m ? m : GetModuleHandle(L"ExplorerExtensions.dll");
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
        }
    };

    if (!WindhawkUtils::HookSymbols(module, taskbarViewHooks, ARRAYSIZE(taskbarViewHooks))) {
        Wh_Log(L"Failed to hook Taskbar.View.dll symbols");
        return false;
    }
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !g_taskbarViewDllLoaded &&
        GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {

        Wh_Log(L"Taskbar View DLL loaded: %s", lpLibFileName);
        if (HookTaskbarViewDllSymbols(module)) Wh_ApplyHookOperations();
    }
    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Taskbar Elastic Pill Mod");
    LoadSettings();

    if (HMODULE m = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(m)) return FALSE;
    } else {
        HMODULE kb = GetModuleHandle(L"kernelbase.dll");
        auto pLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(kb, "LoadLibraryExW");
        if (!WindhawkUtils::Wh_SetFunctionHookT(pLoadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original)) {
            Wh_Log(L"Failed to hook LoadLibraryExW");
            return FALSE;
        }
    }
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Taskbar Elastic Pill Mod");
    g_unloading = true;
    
    std::vector<winrt::weak_ref<winrt::Windows::UI::Xaml::Shapes::Rectangle>> localPills;
    {
        std::lock_guard<std::mutex> lock(g_pillsMutex);
        localPills = g_injectedPills;
        g_injectedPills.clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_easingMutex);
        g_easingCaches.clear();
    }

    if (localPills.empty()) return;

    std::shared_ptr<void> eventLifetime(CreateEvent(nullptr, TRUE, FALSE, nullptr), [](HANDLE h) { if(h) CloseHandle(h); });
    auto pending = std::make_shared<std::atomic<int>>((int)localPills.size());

    for (auto& weakPill : localPills) {
        if (auto pill = weakPill.get()) {
            auto dispatcher = pill.Dispatcher();
            if (dispatcher) {
                if (dispatcher.HasThreadAccess()) {
                    try {
                        if (auto parent = VisualTreeHelper::GetParent(pill)) {
                            if (auto grid = parent.try_as<Grid>()) {
                                uint32_t index;
                                if (grid.Children().IndexOf(pill, index)) {
                                    grid.Children().RemoveAt(index);
                                }
                                RestoreNativeIndicators(grid);
                            }
                        }
                    } catch (...) { Wh_Log(L"Exception during pill cleanup"); }
                    
                    if (pending->fetch_sub(1) == 1 && eventLifetime.get()) {
                        SetEvent(eventLifetime.get());
                    }
                } else {
                    dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High, [pill, pending, eventLifetime]() {
                        try {
                            if (auto parent = VisualTreeHelper::GetParent(pill)) {
                                if (auto grid = parent.try_as<Grid>()) {
                                    uint32_t index;
                                    if (grid.Children().IndexOf(pill, index)) {
                                        grid.Children().RemoveAt(index);
                                    }
                                    RestoreNativeIndicators(grid);
                                }
                            }
                        } catch (...) { Wh_Log(L"Exception during pill cleanup"); }
                        
                        if (pending->fetch_sub(1) == 1 && eventLifetime.get()) {
                            SetEvent(eventLifetime.get());
                        }
                    });
                }
            } else {
                if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
            }
        } else {
            if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
        }
    }

    if (pending->load() > 0 && eventLifetime.get()) {
        if (WaitForSingleObject(eventLifetime.get(), 2000) == WAIT_TIMEOUT) {
            Wh_Log(L"Timeout waiting for pill cleanup on UI threads");
        }
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}