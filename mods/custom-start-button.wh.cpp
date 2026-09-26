// ==WindhawkMod==
// @id              custom-start-button
// @name            Custom Start Button
// @description     Replace the Windows Start button with your own image.
// @version         1.0.0
// @author          RobsHs
// @github          https://github.com/RobsHs
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luxtheme -lgdi32 -luser32 -lgdiplus -lcomctl32 -lshlwapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Custom Start Button

Replace the Windows Start button on your taskbar with your own custom image or photo,
while preserving complete Start Menu functionality.

## Features

* **Custom Image**: Use your own PNG, JPG, JPEG, BMP, GIF, or WEBP image.
* **Preserve Functionality**:
  * Left-click opens the Windows Start Menu as usual.
  * Right-click opens the Win+X Quick Link menu.
  * Keyboard shortcuts (Windows key, Win+A, Win+S, Win+X) work seamlessly.
* **Image Scaling**: Fit (Contain), Fill (Cover), Stretch, or Original size.
* **Custom Sizing**: Auto or custom pixel width and height (DPI-aware).
* **Border Radius**: 0% (square), 25%, 50% (circular), or any custom rounding.
* **Hover & Pressed Effects**: Scale, brightness, or opacity transitions.
* **Alpha Transparency**: Full support for PNG alpha channels.
* **Multi-Monitor Support**: Automatically applies to secondary taskbars.
* **Safe Fallback**: Reverts to the default Windows logo if the image path is missing or invalid.
* **Dual Architecture**: Supports both Windows 11 (XAML Island) and Windows 10 (Win32 Shell).

## Setting Up

1. Open Windhawk and open the **Custom Start Button** settings.
2. Enter the full path to your image (e.g. `C:\Users\YourName\Pictures\start.png` or `%USERPROFILE%\Pictures\start.png`).
3. Choose your desired scaling mode, size, and border radius.
4. Save settings: your Start button will update immediately.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- imagePath: ""
  $name: Custom image path
  $description: >-
    Full path to your image file (PNG, JPG, BMP, GIF, WEBP). Environment variables
    like %USERPROFILE% are supported. Leave empty to use the default Windows logo.
- enableCustomImage: true
  $name: Enable custom image
  $description: When disabled, the default Windows logo is restored.
- scalingMode: contain
  $name: Image scaling mode
  $description: How the image fits within the Start button frame.
  $options:
    - contain: Contain (Fit within bounds, keep aspect ratio)
    - cover: Cover (Fill entire bounds, keep aspect ratio)
    - stretch: Stretch (Fill bounds, stretch aspect ratio)
    - original: Original (Centered at original dimensions)
- customWidth: 0
  $name: Custom width (pixels)
  $description: Button icon width in pixels (0 for automatic default size).
- customHeight: 0
  $name: Custom height (pixels)
  $description: Button icon height in pixels (0 for automatic default size).
- borderRadius: 0
  $name: Border radius (percent)
  $description: Corner rounding from 0% (square) to 50% (circle or pill).
- hoverEffect: brightness
  $name: Hover effect
  $description: Visual transition when the mouse cursor hovers over the button.
  $options:
    - none: None
    - brightness: Brightness
    - scale: Scale
    - opacity: Opacity
- hoverScale: 105
  $name: Hover scale (percent)
  $description: Scaling factor when hovered (e.g. 105 for 1.05x).
- hoverBrightness: 15
  $name: Hover brightness (percent)
  $description: Brightness shift when hovered (-100 to 100 percent).
- hoverOpacity: 100
  $name: Hover opacity (percent)
  $description: Opacity when hovered (0 to 100 percent).
- pressedEffect: scale
  $name: Pressed effect
  $description: Visual transition when clicking the button.
  $options:
    - none: None
    - scale: Scale down
    - opacity: Opacity change
- pressedScale: 95
  $name: Pressed scale (percent)
  $description: Scaling factor when pressed (e.g. 95 for 0.95x).
- enableAnimation: true
  $name: Enable animations
  $description: Smooth hardware-accelerated transitions for hover and press.
- animationDuration: 120
  $name: Animation duration (ms)
  $description: Transition time in milliseconds (50 to 500).
- useAlphaChannel: true
  $name: Preserve image transparency
  $description: Maintain alpha transparency for PNG and transparent images.
- customTooltip: ""
  $name: Custom tooltip
  $description: Tooltip text shown when hovering (leave blank for Windows default).
- applyToSecondaryTaskbars: true
  $name: Apply to all taskbars
  $description: Enable custom Start button on secondary monitors.
- fallbackToDefault: true
  $name: Fallback to Windows logo on error
  $description: Automatically show default Windows logo if image fails to load.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <commctrl.h>
#include <gdiplus.h>
#include <shlwapi.h>
#include <uxtheme.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

namespace wf = winrt::Windows::Foundation;
namespace wuc = winrt::Windows::UI::Composition;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxa = winrt::Windows::UI::Xaml::Automation;
namespace wuxap = winrt::Windows::UI::Xaml::Automation::Peers;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxcp = winrt::Windows::UI::Xaml::Controls::Primitives;
namespace wuxh = winrt::Windows::UI::Xaml::Hosting;
namespace wuxi = winrt::Windows::UI::Xaml::Input;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuxmi = winrt::Windows::UI::Xaml::Media::Imaging;

// =============================================================================
// 1. Configuration & Settings
// =============================================================================

enum class ScalingMode {
    Contain,   // Fit within bounds preserving aspect ratio
    Cover,     // Fill bounds preserving aspect ratio
    Stretch,   // Stretch to fill exact bounds
    Original,  // Centered at original size
};

enum class HoverEffect {
    None,
    Brightness,
    Scale,
    Opacity,
};

enum class PressedEffect {
    None,
    Scale,
    Opacity,
};

struct ModSettings {
    std::wstring imagePath;
    bool enableCustomImage = true;
    ScalingMode scalingMode = ScalingMode::Contain;
    int customWidth = 0;
    int customHeight = 0;
    int borderRadius = 0;  // 0% to 50%
    HoverEffect hoverEffect = HoverEffect::Brightness;
    int hoverScale = 105;
    int hoverBrightness = 15;
    int hoverOpacity = 100;
    PressedEffect pressedEffect = PressedEffect::Scale;
    int pressedScale = 95;
    bool enableAnimation = true;
    int animationDuration = 120;
    bool useAlphaChannel = true;
    std::wstring customTooltip;
    bool applyToSecondaryTaskbars = true;
    bool fallbackToDefault = true;
};

static ModSettings g_settings;
static std::mutex g_settingsMutex;
static std::atomic<uint64_t> g_settingsGen{0};
static std::atomic<bool> g_unloading{false};

static std::wstring GetStringSettingSafe(const wchar_t* name) {
    auto value = WindhawkUtils::StringSetting::make(name);
    return value.get() ? value.get() : L"";
}

static ModSettings ReadSettingsFromWindhawk() {
    ModSettings s;
    s.imagePath = GetStringSettingSafe(L"imagePath");
    s.enableCustomImage = Wh_GetIntSetting(L"enableCustomImage") != 0;

    std::wstring mode = GetStringSettingSafe(L"scalingMode");
    if (_wcsicmp(mode.c_str(), L"cover") == 0) {
        s.scalingMode = ScalingMode::Cover;
    } else if (_wcsicmp(mode.c_str(), L"stretch") == 0) {
        s.scalingMode = ScalingMode::Stretch;
    } else if (_wcsicmp(mode.c_str(), L"original") == 0) {
        s.scalingMode = ScalingMode::Original;
    } else {
        s.scalingMode = ScalingMode::Contain;
    }

    s.customWidth = std::clamp(Wh_GetIntSetting(L"customWidth"), 0, 256);
    s.customHeight = std::clamp(Wh_GetIntSetting(L"customHeight"), 0, 256);
    s.borderRadius = std::clamp(Wh_GetIntSetting(L"borderRadius"), 0, 50);

    std::wstring hover = GetStringSettingSafe(L"hoverEffect");
    if (_wcsicmp(hover.c_str(), L"none") == 0) {
        s.hoverEffect = HoverEffect::None;
    } else if (_wcsicmp(hover.c_str(), L"scale") == 0) {
        s.hoverEffect = HoverEffect::Scale;
    } else if (_wcsicmp(hover.c_str(), L"opacity") == 0) {
        s.hoverEffect = HoverEffect::Opacity;
    } else {
        s.hoverEffect = HoverEffect::Brightness;
    }

    s.hoverScale = std::clamp(Wh_GetIntSetting(L"hoverScale"), 100, 200);
    s.hoverBrightness = std::clamp(Wh_GetIntSetting(L"hoverBrightness"), -100, 100);
    s.hoverOpacity = std::clamp(Wh_GetIntSetting(L"hoverOpacity"), 0, 100);

    std::wstring pressed = GetStringSettingSafe(L"pressedEffect");
    if (_wcsicmp(pressed.c_str(), L"none") == 0) {
        s.pressedEffect = PressedEffect::None;
    } else if (_wcsicmp(pressed.c_str(), L"opacity") == 0) {
        s.pressedEffect = PressedEffect::Opacity;
    } else {
        s.pressedEffect = PressedEffect::Scale;
    }

    s.pressedScale = std::clamp(Wh_GetIntSetting(L"pressedScale"), 50, 100);
    s.enableAnimation = Wh_GetIntSetting(L"enableAnimation") != 0;
    s.animationDuration = std::clamp(Wh_GetIntSetting(L"animationDuration"), 50, 1000);
    s.useAlphaChannel = Wh_GetIntSetting(L"useAlphaChannel") != 0;
    s.customTooltip = GetStringSettingSafe(L"customTooltip");
    s.applyToSecondaryTaskbars = Wh_GetIntSetting(L"applyToSecondaryTaskbars") != 0;
    s.fallbackToDefault = Wh_GetIntSetting(L"fallbackToDefault") != 0;

    return s;
}

static void LoadSettings() {
    ModSettings s = ReadSettingsFromWindhawk();
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = std::move(s);
    g_settingsGen.fetch_add(1, std::memory_order_relaxed);
}

static ModSettings GetSettingsSnapshot() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

// =============================================================================
// 2. Windows Compatibility & Helper Functions
// =============================================================================

namespace WindowsCompat {

static DWORD GetBuildNumber() {
    // Read build number directly from shared user data for maximum reliability
    const auto* sharedUserData = reinterpret_cast<const BYTE*>(0x7FFE0000);
    return *reinterpret_cast<const DWORD*>(sharedUserData + 0x0260);
}

static bool IsWindows11OrGreater() {
    return GetBuildNumber() >= 22000;
}

}  // namespace WindowsCompat

namespace ImageLoader {

static std::wstring ExpandPath(const std::wstring& source) {
    if (source.empty()) {
        return L"";
    }

    DWORD required = ExpandEnvironmentStringsW(source.c_str(), nullptr, 0);
    if (!required) {
        return source;
    }

    std::wstring result(required, L'\0');
    DWORD written = ExpandEnvironmentStringsW(source.c_str(), result.data(), required);
    if (!written || written > required) {
        return source;
    }

    result.resize(written - 1);
    return result;
}

static bool FileExists(const std::wstring& path) {
    if (path.empty()) {
        return false;
    }
    DWORD attribs = GetFileAttributesW(path.c_str());
    return (attribs != INVALID_FILE_ATTRIBUTES && !(attribs & FILE_ATTRIBUTE_DIRECTORY));
}

static std::wstring MakeFileUri(const std::wstring& rawPath) {
    std::wstring path = ExpandPath(rawPath);
    if (path.empty()) {
        return L"";
    }

    // If already a URI, return as-is
    if (_wcsnicmp(path.c_str(), L"http://", 7) == 0 ||
        _wcsnicmp(path.c_str(), L"https://", 8) == 0 ||
        _wcsnicmp(path.c_str(), L"file:///", 8) == 0) {
        return path;
    }

    std::wstring result;
    result.reserve(path.size() + 16);
    result = L"file:///";

    for (wchar_t c : path) {
        switch (c) {
            case L'\\':
                result += L'/';
                break;
            case L' ':
                result += L"%20";
                break;
            case L'#':
                result += L"%23";
                break;
            case L'%':
                result += L"%25";
                break;
            default:
                result += c;
                break;
        }
    }

    return result;
}

}  // namespace ImageLoader

// =============================================================================
// 3. Thread Synchronization Helper
// =============================================================================

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

static bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_PARAM {
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
                const auto* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    auto* param = reinterpret_cast<RUN_PARAM*>(cwp->lParam);
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);

    if (!hook) {
        return false;
    }

    RUN_PARAM param{proc, procParam};
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, reinterpret_cast<LPARAM>(&param));
    UnhookWindowsHookEx(hook);
    return true;
}

// =============================================================================
// 4. Windows 11 Subsystem (XAML Island Hooks)
// =============================================================================

namespace Win11Subsystem {

struct StartButtonInstance {
    DWORD threadId = 0;
    HWND hOwnerWnd = nullptr;

    winrt::weak_ref<wux::FrameworkElement> startButton;
    winrt::weak_ref<wux::FrameworkElement> stockIcon;
    winrt::weak_ref<wuxc::Panel> hostPanel;
    winrt::weak_ref<wuxc::Grid> customImageHost;
    winrt::weak_ref<wuxc::Image> customImage;

    double originalStockOpacity = 1.0;
    bool isPointerOver = false;
    bool isPressed = false;
    bool imageOpened = false;

    winrt::event_token imageOpenedToken{};
    winrt::event_token imageFailedToken{};
    winrt::event_token pointerEnteredToken{};
    winrt::event_token pointerExitedToken{};
    winrt::event_token pointerPressedToken{};
    winrt::event_token pointerReleasedToken{};
    winrt::event_token pointerCaptureLostToken{};
    winrt::event_token pointerCanceledToken{};
    bool eventsAttached = false;
};

static std::vector<std::shared_ptr<StartButtonInstance>> g_instances;
static std::mutex g_instancesMutex;

static bool MotionAllowed() {
    BOOL animationsEnabled = TRUE;
    if (!SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &animationsEnabled, 0)) {
        return true;
    }
    return animationsEnabled != FALSE;
}

static void UpdateVisualEffects(const std::shared_ptr<StartButtonInstance>& inst) {
    if (!inst) {
        return;
    }

    auto customHost = inst->customImageHost.get();
    if (!customHost) {
        return;
    }

    ModSettings s = GetSettingsSnapshot();

    float targetScale = 1.0f;
    float targetOpacity = 1.0f;

    if (inst->isPressed) {
        if (s.pressedEffect == PressedEffect::Scale) {
            targetScale = static_cast<float>(s.pressedScale) / 100.0f;
        } else if (s.pressedEffect == PressedEffect::Opacity) {
            targetOpacity = 0.7f;
        }
    } else if (inst->isPointerOver) {
        if (s.hoverEffect == HoverEffect::Scale) {
            targetScale = static_cast<float>(s.hoverScale) / 100.0f;
        } else if (s.hoverEffect == HoverEffect::Opacity) {
            targetOpacity = static_cast<float>(s.hoverOpacity) / 100.0f;
        } else if (s.hoverEffect == HoverEffect::Brightness) {
            targetOpacity = 1.0f;
        }
    }

    try {
        auto visual = wuxh::ElementCompositionPreview::GetElementVisual(customHost);
        if (!visual) {
            return;
        }

        float w = static_cast<float>(customHost.ActualWidth());
        float h = static_cast<float>(customHost.ActualHeight());
        if (w <= 0.0f) w = 34.0f;
        if (h <= 0.0f) h = 34.0f;
        visual.CenterPoint({w / 2.0f, h / 2.0f, 0.0f});

        if (!s.enableAnimation || !MotionAllowed()) {
            visual.StopAnimation(L"Scale");
            visual.StopAnimation(L"Opacity");
            visual.Scale({targetScale, targetScale, 1.0f});
            visual.Opacity(targetOpacity);
            return;
        }

        auto compositor = visual.Compositor();
        auto easing = compositor.CreateCubicBezierEasingFunction({0.20f, 0.0f}, {0.0f, 1.0f});

        // Scale animation
        auto scaleAnim = compositor.CreateVector3KeyFrameAnimation();
        scaleAnim.InsertKeyFrame(1.0f, {targetScale, targetScale, 1.0f}, easing);
        scaleAnim.Duration(std::chrono::milliseconds(s.animationDuration));
        visual.StartAnimation(L"Scale", scaleAnim);

        // Opacity animation
        auto opacityAnim = compositor.CreateScalarKeyFrameAnimation();
        opacityAnim.InsertKeyFrame(1.0f, targetOpacity, easing);
        opacityAnim.Duration(std::chrono::milliseconds(s.animationDuration));
        visual.StartAnimation(L"Opacity", opacityAnim);

    } catch (...) {
        // Safe failover
    }
}

static void DetachInstance(const std::shared_ptr<StartButtonInstance>& inst) {
    if (!inst) {
        return;
    }

    try {
        auto startBtn = inst->startButton.get();
        if (startBtn && inst->eventsAttached) {
            startBtn.PointerEntered(inst->pointerEnteredToken);
            startBtn.PointerExited(inst->pointerExitedToken);
            startBtn.PointerPressed(inst->pointerPressedToken);
            startBtn.PointerReleased(inst->pointerReleasedToken);
            startBtn.PointerCaptureLost(inst->pointerCaptureLostToken);
            startBtn.PointerCanceled(inst->pointerCanceledToken);
            inst->eventsAttached = false;
        }

        auto stock = inst->stockIcon.get();
        if (stock) {
            stock.Opacity(inst->originalStockOpacity);
        }

        auto host = inst->customImageHost.get();
        auto panel = inst->hostPanel.get();
        if (panel && host) {
            uint32_t index = 0;
            if (panel.Children().IndexOf(host, index)) {
                panel.Children().RemoveAt(index);
            }
        }
    } catch (...) {
    }
}

static void DetachAllInstances() {
    std::lock_guard<std::mutex> lock(g_instancesMutex);
    for (auto& inst : g_instances) {
        DetachInstance(inst);
    }
    g_instances.clear();
}

static wux::FrameworkElement FindDescendant(
    wux::FrameworkElement element,
    const std::function<bool(wux::FrameworkElement)>& predicate) {
    if (!element) return nullptr;

    int count = wuxm::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; i++) {
        auto child = wuxm::VisualTreeHelper::GetChild(element, i).try_as<wux::FrameworkElement>();
        if (!child) continue;

        if (predicate(child)) {
            return child;
        }

        auto found = FindDescendant(child, predicate);
        if (found) {
            return found;
        }
    }
    return nullptr;
}

static bool IsStartButtonElement(wux::FrameworkElement element) {
    if (!element) return false;
    return winrt::get_class_name(element) == L"Taskbar.ExperienceToggleButton" &&
           wuxa::AutomationProperties::GetAutomationId(element) == L"StartButton";
}

static wux::FrameworkElement FindStockStartIcon(wux::FrameworkElement startButton) {
    return FindDescendant(startButton, [](wux::FrameworkElement child) {
        return child.Name() == L"Icon";
    });
}

static wuxc::Panel FindHostPanel(wux::FrameworkElement stockIcon, wux::FrameworkElement startButton) {
    if (!stockIcon) return nullptr;
    auto parent = wuxm::VisualTreeHelper::GetParent(stockIcon);
    auto panel = parent.try_as<wuxc::Panel>();
    if (panel) return panel;

    return FindDescendant(startButton, [](wux::FrameworkElement child) {
        return child.try_as<wuxc::Panel>() != nullptr;
    }).try_as<wuxc::Panel>();
}

static bool AttachToStartButton(wux::FrameworkElement startButton, HWND hOwnerWnd) {
    if (!startButton || g_unloading) {
        return false;
    }

    ModSettings s = GetSettingsSnapshot();

    auto stock = FindStockStartIcon(startButton);
    if (!stock) {
        Wh_Log(L"[Win11] Stock icon not found");
        return false;
    }

    // Check if already attached
    {
        std::lock_guard<std::mutex> lock(g_instancesMutex);
        for (const auto& existing : g_instances) {
            if (existing->startButton.get() == startButton) {
                return true;
            }
        }
    }

    std::wstring expandedPath = ImageLoader::ExpandPath(s.imagePath);
    bool shouldShowCustom = s.enableCustomImage && !expandedPath.empty() && ImageLoader::FileExists(expandedPath);

    if (!shouldShowCustom) {
        stock.Opacity(1.0);
        return true;
    }

    auto panel = FindHostPanel(stock, startButton);
    if (!panel) {
        Wh_Log(L"[Win11] Host panel for Start icon not found");
        return false;
    }

    try {
        auto inst = std::make_shared<StartButtonInstance>();
        inst->threadId = GetCurrentThreadId();
        inst->hOwnerWnd = hOwnerWnd;
        inst->startButton = winrt::make_weak(startButton);
        inst->stockIcon = winrt::make_weak(stock);
        inst->hostPanel = winrt::make_weak(panel);
        inst->originalStockOpacity = stock.Opacity();

        // Calculate dimensions
        double iconWidth = (s.customWidth > 0) ? s.customWidth : 34.0;
        double iconHeight = (s.customHeight > 0) ? s.customHeight : 34.0;

        wuxc::Grid imageHost;
        imageHost.Width(iconWidth);
        imageHost.Height(iconHeight);
        imageHost.HorizontalAlignment(wux::HorizontalAlignment::Center);
        imageHost.VerticalAlignment(wux::VerticalAlignment::Center);
        imageHost.IsHitTestVisible(false);  // Crucial: pass all clicks to the start button!
        wuxa::AutomationProperties::SetAccessibilityView(imageHost, wuxap::AccessibilityView::Raw);

        // Apply Border Radius clipping via Composition RectangleClip or Border
        if (s.borderRadius > 0) {
            try {
                auto visual = wuxh::ElementCompositionPreview::GetElementVisual(imageHost);
                if (visual) {
                    auto compositor = visual.Compositor();
                    auto rectClip = compositor.CreateRectangleClip();
                    float minDim = static_cast<float>(std::min(iconWidth, iconHeight));
                    float r = (minDim * s.borderRadius) / 100.0f;
                    rectClip.TopLeftRadius({r, r});
                    rectClip.TopRightRadius({r, r});
                    rectClip.BottomLeftRadius({r, r});
                    rectClip.BottomRightRadius({r, r});
                    visual.Clip(rectClip);
                }
            } catch (...) {
            }
        }

        wuxc::Image img;
        img.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
        img.VerticalAlignment(wux::VerticalAlignment::Stretch);
        img.IsHitTestVisible(false);

        switch (s.scalingMode) {
            case ScalingMode::Cover:
                img.Stretch(wuxm::Stretch::UniformToFill);
                break;
            case ScalingMode::Stretch:
                img.Stretch(wuxm::Stretch::Fill);
                break;
            case ScalingMode::Original:
                img.Stretch(wuxm::Stretch::None);
                break;
            case ScalingMode::Contain:
            default:
                img.Stretch(wuxm::Stretch::Uniform);
                break;
        }

        std::wstring uriString = ImageLoader::MakeFileUri(expandedPath);
        wuxmi::BitmapImage bmp{wf::Uri{uriString}};

        std::weak_ptr<StartButtonInstance> weakInst = inst;
        inst->imageOpenedToken = bmp.ImageOpened([weakInst](auto&&, auto&&) {
            if (auto strong = weakInst.lock()) {
                strong->imageOpened = true;
                if (auto stockElem = strong->stockIcon.get()) {
                    stockElem.Opacity(0.0);  // Hide stock icon only after image successfully loads
                }
                Wh_Log(L"[Win11] Custom Start button image opened successfully");
            }
        });

        inst->imageFailedToken = bmp.ImageFailed([weakInst](auto&&, auto&&) {
            if (auto strong = weakInst.lock()) {
                strong->imageOpened = false;
                if (auto stockElem = strong->stockIcon.get()) {
                    stockElem.Opacity(1.0);  // Revert to stock icon on error
                }
                Wh_Log(L"[Win11] Failed to load Start button image; reverted to default logo");
            }
        });

        img.Source(bmp);
        imageHost.Children().Append(img);
        wuxc::Canvas::SetZIndex(imageHost, 10000);

        inst->customImageHost = winrt::make_weak(imageHost);
        inst->customImage = winrt::make_weak(img);

        // Pointer event listeners for hover and pressed effects
        inst->pointerEnteredToken = startButton.PointerEntered([weakInst](auto&&, auto&&) {
            if (auto strong = weakInst.lock()) {
                strong->isPointerOver = true;
                UpdateVisualEffects(strong);
            }
        });

        inst->pointerExitedToken = startButton.PointerExited([weakInst](auto&&, auto&&) {
            if (auto strong = weakInst.lock()) {
                strong->isPointerOver = false;
                strong->isPressed = false;
                UpdateVisualEffects(strong);
            }
        });

        inst->pointerPressedToken = startButton.PointerPressed([weakInst](auto&&, auto&&) {
            if (auto strong = weakInst.lock()) {
                strong->isPressed = true;
                UpdateVisualEffects(strong);
            }
        });

        inst->pointerReleasedToken = startButton.PointerReleased([weakInst](auto&&, auto&&) {
            if (auto strong = weakInst.lock()) {
                strong->isPressed = false;
                UpdateVisualEffects(strong);
            }
        });

        inst->pointerCaptureLostToken = startButton.PointerCaptureLost([weakInst](auto&&, auto&&) {
            if (auto strong = weakInst.lock()) {
                strong->isPressed = false;
                UpdateVisualEffects(strong);
            }
        });

        inst->pointerCanceledToken = startButton.PointerCanceled([weakInst](auto&&, auto&&) {
            if (auto strong = weakInst.lock()) {
                strong->isPressed = false;
                UpdateVisualEffects(strong);
            }
        });

        inst->eventsAttached = true;

        // Custom tooltip if specified
        if (!s.customTooltip.empty()) {
            wuxc::ToolTipService::SetToolTip(startButton, winrt::box_value(s.customTooltip));
        }

        panel.Children().Append(imageHost);

        {
            std::lock_guard<std::mutex> lock(g_instancesMutex);
            g_instances.push_back(inst);
        }

        Wh_Log(L"[Win11] Attached custom image to Start button");
        return true;

    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"[Win11] Error attaching custom Start button: 0x%08X %s", e.code().value, e.message().c_str());
        return false;
    } catch (...) {
        Wh_Log(L"[Win11] Unknown exception in AttachToStartButton");
        return false;
    }
}

// -----------------------------------------------------------------------------
// XamlRoot & TaskbarHost Symbol Interop
// -----------------------------------------------------------------------------

static void* CTaskBand_ITaskListWndSite_vftable = nullptr;
static void* CSecondaryTaskBand_ITaskListWndSite_vftable = nullptr;
using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
static CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;
static void* TaskbarHost_FrameHeight_Original = nullptr;
using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
static CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original = nullptr;
using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
static std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original = nullptr;

static wux::XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t taskbarElementOffset = 0x10;
#if defined(_M_X64)
    if (TaskbarHost_FrameHeight_Original) {
        const auto* b = reinterpret_cast<const BYTE*>(TaskbarHost_FrameHeight_Original);
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementOffset = b[7];
        }
    }
#endif

    auto* elemUnknown = *reinterpret_cast<IUnknown**>(
        reinterpret_cast<BYTE*>(taskbarHostSharedPtr[0]) + taskbarElementOffset);

    wux::FrameworkElement taskbarElem = nullptr;
    if (elemUnknown) {
        elemUnknown->QueryInterface(winrt::guid_of<wux::FrameworkElement>(),
                                    winrt::put_abi(taskbarElem));
    }

    auto result = taskbarElem ? taskbarElem.XamlRoot() : nullptr;
    if (std__Ref_count_base__Decref_Original && taskbarHostSharedPtr[1]) {
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
    }

    return result;
}

static wux::XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    if (!CTaskBand_ITaskListWndSite_vftable || !CTaskBand_GetTaskbarHost_Original) {
        return nullptr;
    }

    HWND hTaskSwWnd = reinterpret_cast<HWND>(GetProp(hTaskbarWnd, L"TaskbandHWND"));
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtr(hTaskSwWnd, 0));
    void* taskBandSite = taskBand;
    for (int i = 0; taskBandSite && *reinterpret_cast<void**>(taskBandSite) != CTaskBand_ITaskListWndSite_vftable; i++) {
        if (i >= 20) return nullptr;
        taskBandSite = reinterpret_cast<void**>(taskBandSite) + 1;
    }

    if (!taskBandSite) return nullptr;

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandSite, taskbarHostSharedPtr);
    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

static wux::XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd) {
    if (!CSecondaryTaskBand_ITaskListWndSite_vftable || !CSecondaryTaskBand_GetTaskbarHost_Original) {
        return nullptr;
    }

    HWND hTaskSwWnd = FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hTaskSwWnd) return nullptr;

    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtr(hTaskSwWnd, 0));
    void* taskBandSite = taskBand;
    for (int i = 0; taskBandSite && *reinterpret_cast<void**>(taskBandSite) != CSecondaryTaskBand_ITaskListWndSite_vftable; i++) {
        if (i >= 20) return nullptr;
        taskBandSite = reinterpret_cast<void**>(taskBandSite) + 1;
    }

    if (!taskBandSite) return nullptr;

    void* taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(taskBandSite, taskbarHostSharedPtr);
    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

static bool ApplyStyleToXamlRoot(wux::XamlRoot xamlRoot, HWND hWnd) {
    if (!xamlRoot) return false;

    auto content = xamlRoot.Content().try_as<wux::FrameworkElement>();
    if (!content) return false;

    auto startButton = FindDescendant(content, IsStartButtonElement);
    if (!startButton) {
        return false;
    }

    return AttachToStartButton(startButton, hWnd);
}

static void ApplySettingsFromTaskbarThread() {
    DetachAllInstances();

    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM) -> BOOL {
            WCHAR szClassName[64];
            if (GetClassNameW(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            wux::XamlRoot root = nullptr;
            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
                root = GetTaskbarXamlRoot(hWnd);
            } else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
                ModSettings s = GetSettingsSnapshot();
                if (s.applyToSecondaryTaskbars) {
                    root = GetSecondaryTaskbarXamlRoot(hWnd);
                }
            }

            if (root) {
                ApplyStyleToXamlRoot(root, hWnd);
            }

            return TRUE;
        },
        0);
}

// ExperienceToggleButton hook to catch recreation of Start button elements
using ExperienceToggleButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
static ExperienceToggleButton_UpdateButtonPadding_t ExperienceToggleButton_UpdateButtonPadding_Original = nullptr;

static void WINAPI ExperienceToggleButton_UpdateButtonPadding_Hook(void* pThis) {
    if (ExperienceToggleButton_UpdateButtonPadding_Original) {
        ExperienceToggleButton_UpdateButtonPadding_Original(pThis);
    }

    if (g_unloading) return;

    try {
        wux::FrameworkElement elem = nullptr;
        reinterpret_cast<IUnknown**>(pThis)[1]->QueryInterface(
            winrt::guid_of<wux::FrameworkElement>(), winrt::put_abi(elem));

        if (elem && IsStartButtonElement(elem)) {
            AttachToStartButton(elem, nullptr);
        }
    } catch (...) {
    }
}

static bool HookTaskbarViewSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK taskbar_view_dll_hooks[] = {
        {
            {LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::UpdateButtonPadding(void))"},
            reinterpret_cast<void**>(&ExperienceToggleButton_UpdateButtonPadding_Original),
            reinterpret_cast<void*>(ExperienceToggleButton_UpdateButtonPadding_Hook),
            true,
        },
    };

    return HookSymbols(module, taskbar_view_dll_hooks, ARRAYSIZE(taskbar_view_dll_hooks));
}

static bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"[Win11] Failed to load taskbar.dll");
        return false;
    }

    // taskbar.dll
    WindhawkUtils::SYMBOL_HOOK taskbar_dll_hooks[] = {
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
            reinterpret_cast<void**>(&CTaskBand_GetTaskbarHost_Original),
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            reinterpret_cast<void**>(&CSecondaryTaskBand_GetTaskbarHost_Original),
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            reinterpret_cast<void**>(&std__Ref_count_base__Decref_Original),
        },
    };

    return HookSymbols(module, taskbar_dll_hooks, ARRAYSIZE(taskbar_dll_hooks));
}

}  // namespace Win11Subsystem

// =============================================================================
// 5. Windows 10 Subsystem (Win32 Subclassing & GDI+ Double Buffering)
// =============================================================================

namespace Win10Subsystem {

static const UINT_PTR START_BUTTON_SUBCLASS_ID = 0x53545254; // 'STRT'
static ULONG_PTR g_gdiplusToken = 0;
static Gdiplus::Bitmap* g_pCachedBitmap = nullptr;
static std::wstring g_cachedImagePath;
static FILETIME g_cachedFileTime{};
static std::mutex g_gdiMutex;

static void EnsureGdiplusInitialized() {
    if (!g_gdiplusToken) {
        Gdiplus::GdiplusStartupInput input;
        Gdiplus::GdiplusStartup(&g_gdiplusToken, &input, nullptr);
    }
}

static void ReleaseCachedBitmap() {
    std::lock_guard<std::mutex> lock(g_gdiMutex);
    delete g_pCachedBitmap;
    g_pCachedBitmap = nullptr;
    g_cachedImagePath.clear();
    ZeroMemory(&g_cachedFileTime, sizeof(g_cachedFileTime));
}

static void EnsureGdiplusShutdown() {
    ReleaseCachedBitmap();
    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}

static Gdiplus::Bitmap* GetOrLoadBitmap(const std::wstring& rawPath) {
    std::wstring path = ImageLoader::ExpandPath(rawPath);
    if (!ImageLoader::FileExists(path)) {
        return nullptr;
    }

    HANDLE hFile = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        return nullptr;
    }

    FILETIME lastWriteTime{};
    GetFileTime(hFile, nullptr, nullptr, &lastWriteTime);
    CloseHandle(hFile);

    std::lock_guard<std::mutex> lock(g_gdiMutex);
    if (g_pCachedBitmap && g_cachedImagePath == path &&
        memcmp(&g_cachedFileTime, &lastWriteTime, sizeof(FILETIME)) == 0) {
        return g_pCachedBitmap;
    }

    delete g_pCachedBitmap;
    g_pCachedBitmap = new Gdiplus::Bitmap(path.c_str());
    if (!g_pCachedBitmap || g_pCachedBitmap->GetLastStatus() != Gdiplus::Ok) {
        delete g_pCachedBitmap;
        g_pCachedBitmap = nullptr;
        return nullptr;
    }

    g_cachedImagePath = path;
    g_cachedFileTime = lastWriteTime;
    return g_pCachedBitmap;
}

struct Win10SubclassContext {
    bool isHovered = false;
    bool isPressed = false;
    bool trackingMouse = false;
};

static LRESULT CALLBACK StartButtonSubclassProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {

    auto* ctx = reinterpret_cast<Win10SubclassContext*>(dwRefData);

    switch (uMsg) {
        case WM_MOUSEMOVE: {
            if (!ctx->isHovered) {
                ctx->isHovered = true;
                TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hWnd, 0};
                TrackMouseEvent(&tme);
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            break;
        }

        case WM_MOUSELEAVE: {
            ctx->isHovered = false;
            ctx->isPressed = false;
            InvalidateRect(hWnd, nullptr, FALSE);
            break;
        }

        case WM_LBUTTONDOWN: {
            ctx->isPressed = true;
            InvalidateRect(hWnd, nullptr, FALSE);
            break;
        }

        case WM_LBUTTONUP: {
            ctx->isPressed = false;
            InvalidateRect(hWnd, nullptr, FALSE);
            break;
        }

        case WM_PAINT: {
            ModSettings s = GetSettingsSnapshot();
            Gdiplus::Bitmap* bmp = nullptr;

            if (s.enableCustomImage) {
                bmp = GetOrLoadBitmap(s.imagePath);
            }

            if (!bmp) {
                // Fall back directly to default Windows 10 rendering
                return DefSubclassProc(hWnd, uMsg, wParam, lParam);
            }

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            if (!hdc) break;

            RECT clientRect;
            GetClientRect(hWnd, &clientRect);
            int btnW = clientRect.right - clientRect.left;
            int btnH = clientRect.bottom - clientRect.top;

            if (btnW <= 0 || btnH <= 0) {
                EndPaint(hWnd, &ps);
                return 0;
            }

            // Create double buffer to eliminate flicker
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, btnW, btnH);
            HBITMAP oldBitmap = reinterpret_cast<HBITMAP>(SelectObject(memDC, memBitmap));

            // Paint default background first
            DefSubclassProc(hWnd, WM_ERASEBKGND, reinterpret_cast<WPARAM>(memDC), 0);

            {
                Gdiplus::Graphics g(memDC);
                g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
                g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
                g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);

                int imgW = (s.customWidth > 0) ? s.customWidth : btnW;
                int imgH = (s.customHeight > 0) ? s.customHeight : btnH;

                float scale = 1.0f;
                if (ctx->isPressed && s.pressedEffect == PressedEffect::Scale) {
                    scale = static_cast<float>(s.pressedScale) / 100.0f;
                } else if (ctx->isHovered && s.hoverEffect == HoverEffect::Scale) {
                    scale = static_cast<float>(s.hoverScale) / 100.0f;
                }

                float drawW = imgW * scale;
                float drawH = imgH * scale;
                float drawX = (btnW - drawW) / 2.0f;
                float drawY = (btnH - drawH) / 2.0f;

                // Border radius clipping
                if (s.borderRadius > 0) {
                    float minDim = std::min(drawW, drawH);
                    float radius = (minDim * s.borderRadius) / 100.0f;
                    Gdiplus::GraphicsPath path;
                    path.AddArc(drawX, drawY, radius * 2, radius * 2, 180, 90);
                    path.AddArc(drawX + drawW - radius * 2, drawY, radius * 2, radius * 2, 270, 90);
                    path.AddArc(drawX + drawW - radius * 2, drawY + drawH - radius * 2, radius * 2, radius * 2, 0, 90);
                    path.AddArc(drawX, drawY + drawH - radius * 2, radius * 2, radius * 2, 90, 90);
                    path.CloseFigure();
                    g.SetClip(&path);
                }

                // Hover Brightness / Opacity
                Gdiplus::ImageAttributes imgAttr;
                if (ctx->isHovered && s.hoverEffect == HoverEffect::Brightness && s.hoverBrightness != 0) {
                    float b = static_cast<float>(s.hoverBrightness) / 100.0f;
                    Gdiplus::ColorMatrix cm = {
                        1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                        b,    b,    b,    0.0f, 1.0f
                    };
                    imgAttr.SetColorMatrix(&cm, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);
                } else if (ctx->isHovered && s.hoverEffect == HoverEffect::Opacity && s.hoverOpacity < 100) {
                    float alpha = static_cast<float>(s.hoverOpacity) / 100.0f;
                    Gdiplus::ColorMatrix cm = {
                        1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, alpha, 0.0f,
                        0.0f, 0.0f, 0.0f, 0.0f, 1.0f
                    };
                    imgAttr.SetColorMatrix(&cm, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);
                }

                g.DrawImage(bmp, Gdiplus::RectF(drawX, drawY, drawW, drawH),
                            0, 0, bmp->GetWidth(), bmp->GetHeight(),
                            Gdiplus::UnitPixel, &imgAttr);
            }

            BitBlt(hdc, 0, 0, btnW, btnH, memDC, 0, 0, SRCCOPY);
            SelectObject(memDC, oldBitmap);
            DeleteObject(memBitmap);
            DeleteDC(memDC);

            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_NCDESTROY: {
            RemoveWindowSubclass(hWnd, StartButtonSubclassProc, uIdSubclass);
            delete ctx;
            break;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static void AttachToStartButtonWnd(HWND hStartWnd) {
    if (!hStartWnd || !IsWindow(hStartWnd)) return;

    auto* ctx = new Win10SubclassContext();
    if (!SetWindowSubclass(hStartWnd, StartButtonSubclassProc, START_BUTTON_SUBCLASS_ID, reinterpret_cast<DWORD_PTR>(ctx))) {
        delete ctx;
    } else {
        InvalidateRect(hStartWnd, nullptr, TRUE);
        Wh_Log(L"[Win10] Subclassed Start button HWND %p", hStartWnd);
    }
}

static void DetachFromStartButtonWnd(HWND hStartWnd) {
    if (hStartWnd && IsWindow(hStartWnd)) {
        RemoveWindowSubclass(hStartWnd, StartButtonSubclassProc, START_BUTTON_SUBCLASS_ID);
        InvalidateRect(hStartWnd, nullptr, TRUE);
    }
}

static void EnumerateAndSubclassTaskbars() {
    HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hTray) {
        HWND hStart = FindWindowExW(hTray, nullptr, L"Start", nullptr);
        if (hStart) {
            AttachToStartButtonWnd(hStart);
        }
    }

    // Secondary monitors
    HWND hSecondaryTray = nullptr;
    while ((hSecondaryTray = FindWindowExW(nullptr, hSecondaryTray, L"Shell_SecondaryTrayWnd", nullptr)) != nullptr) {
        HWND hStart = FindWindowExW(hSecondaryTray, nullptr, L"Start", nullptr);
        if (hStart) {
            AttachToStartButtonWnd(hStart);
        }
    }
}

static void EnumerateAndDetachTaskbars() {
    HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hTray) {
        HWND hStart = FindWindowExW(hTray, nullptr, L"Start", nullptr);
        if (hStart) {
            DetachFromStartButtonWnd(hStart);
        }
    }

    HWND hSecondaryTray = nullptr;
    while ((hSecondaryTray = FindWindowExW(nullptr, hSecondaryTray, L"Shell_SecondaryTrayWnd", nullptr)) != nullptr) {
        HWND hStart = FindWindowExW(hSecondaryTray, nullptr, L"Start", nullptr);
        if (hStart) {
            DetachFromStartButtonWnd(hStart);
        }
    }
}

}  // namespace Win10Subsystem

// =============================================================================
// 6. Mod Lifecycle & Dynamic Re-application
// =============================================================================

static HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcId = 0;
            WCHAR className[64];
            if (GetWindowThreadProcessId(hWnd, &dwProcId) &&
                dwProcId == GetCurrentProcessId() &&
                GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

static std::atomic<bool> g_taskbarViewDllLoaded{false};

static HMODULE GetTaskbarViewModuleHandle() {
    HMODULE mod = GetModuleHandleW(L"Taskbar.View.dll");
    if (!mod) {
        mod = GetModuleHandleW(L"ExplorerExtensions.dll");
    }
    return mod;
}

static void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"[Win11] Detected taskbar view module: %s", lpLibFileName);
        if (Win11Subsystem::HookTaskbarViewSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t LoadLibraryExW_Original = nullptr;
static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE mod = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (mod) {
        HandleLoadedModuleIfTaskbarView(mod, lpLibFileName);
    }
    return mod;
}

static void ApplyAllSettings() {
    if (WindowsCompat::IsWindows11OrGreater()) {
        HWND hTaskbar = FindCurrentProcessTaskbarWnd();
        if (hTaskbar) {
            RunFromWindowThread(hTaskbar, [](void*) {
                Win11Subsystem::ApplySettingsFromTaskbarThread();
            }, nullptr);
        }
    } else {
        Win10Subsystem::EnumerateAndSubclassTaskbars();
    }
}

// -----------------------------------------------------------------------------
// Windhawk Exported Callbacks
// -----------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"[CustomStartButton] Initializing version 1.0.0");
    DWORD buildNumber = WindowsCompat::GetBuildNumber();
    Wh_Log(L"[CustomStartButton] Detected Windows build: %u (%s)",
           buildNumber,
           WindowsCompat::IsWindows11OrGreater() ? L"Windows 11" : L"Windows 10");

    LoadSettings();

    if (WindowsCompat::IsWindows11OrGreater()) {
        if (!Win11Subsystem::HookTaskbarDllSymbols()) {
            Wh_Log(L"[CustomStartButton] Warning: Failed to hook taskbar.dll symbols");
        }

        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            g_taskbarViewDllLoaded = true;
            if (!Win11Subsystem::HookTaskbarViewSymbols(taskbarViewModule)) {
                Wh_Log(L"[CustomStartButton] Warning: Failed to hook Taskbar.View.dll symbols");
            }
        } else {
            Wh_Log(L"[CustomStartButton] Taskbar.View.dll not loaded yet; monitoring LoadLibraryExW");
            HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
            auto pLoadLibraryExW = reinterpret_cast<LoadLibraryExW_t>(
                GetProcAddress(kernelBase, "LoadLibraryExW"));
            if (pLoadLibraryExW) {
                WindhawkUtils::SetFunctionHook(pLoadLibraryExW, LoadLibraryExW_Hook,
                                               &LoadLibraryExW_Original);
            }
        }
    } else {
        Win10Subsystem::EnsureGdiplusInitialized();
    }

    Wh_Log(L"[CustomStartButton] Init complete");
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"[CustomStartButton] AfterInit - applying settings to Start button");
    ApplyAllSettings();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"[CustomStartButton] BeforeUninit - cleaning up resources and restoring default logo");
    g_unloading = true;

    if (WindowsCompat::IsWindows11OrGreater()) {
        HWND hTaskbar = FindCurrentProcessTaskbarWnd();
        if (hTaskbar) {
            RunFromWindowThread(hTaskbar, [](void*) {
                Win11Subsystem::DetachAllInstances();
            }, nullptr);
        }
    } else {
        Win10Subsystem::EnumerateAndDetachTaskbars();
        Win10Subsystem::EnsureGdiplusShutdown();
    }
}

void Wh_ModUninit() {
    Wh_Log(L"[CustomStartButton] Uninit finished");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"[CustomStartButton] SettingsChanged - reloading settings");
    LoadSettings();
    ApplyAllSettings();
}
