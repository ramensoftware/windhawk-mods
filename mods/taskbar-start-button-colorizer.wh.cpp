// ==WindhawkMod==
// @id              taskbar-start-button-colorizer
// @name            Start button colorizer
// @description     Recolor the Start button icon on the taskbar with a color preset or with hue, saturation, brightness and opacity effects, and change its size (Windows 11 only)
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Start button colorizer

Customize the Windows logo of the Start button on the taskbar:

* **Color**: recolor the icon with one of the built-in colors, such as red,
  green or pink.
* **Effects**: fine tune the icon with the hue, saturation, brightness and
  opacity effects.
* **Size**: set the size of the icon.

To use a custom image instead of the built-in icon, refer to the [Start Button
Replacer](https://windhawk.net/mods/start-button-replacer) mod.

Only Windows 11 is supported.

![Screenshot](https://i.imgur.com/GebA6hR.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- color: none
  $name: Color
  $description: >-
    Recolors the icon by setting the hue of its colors, keeping their original
    saturation and brightness.
  $options:
  - none: Original
  - red: Red
  - orange: Orange
  - yellow: Yellow
  - green: Green
  - teal: Teal
  - blue: Blue
  - purple: Purple
  - pink: Pink
- effects:
  - hue: 0
    $name: Hue shift
    $description: >-
      Rotates the colors of the icon by the given amount of degrees, from 0 to
      360, on top of the color which is selected above.
  - saturation: 100
    $name: Saturation
    $description: >-
      In percent, relative to the original colors. Zero makes the icon gray.
  - brightness: 100
    $name: Brightness
    $description: >-
      In percent, relative to the original colors. Zero makes the icon black,
      and values above 100 make the icon lighter, up to white.
  - opacity: 100
    $name: Opacity
    $description: In percent.
  $name: Effects
- size: 100
  $name: Size
  $description: The size of the icon in percent.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <functional>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

namespace Composition = winrt::Windows::UI::Composition;

struct {
    // The hue the icon is recolored to, or -1 to keep its original hue.
    int colorHue;
    int hue;
    double saturation;
    double brightness;
    double opacity;
    double size;
    bool customizeColors;
    bool customizeSize;
} g_settings;

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_unloading;

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

FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
            continue;
        }

        if (enumCallback(child)) {
            return child;
        }
    }

    return nullptr;
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    return EnumChildElements(element, [name](FrameworkElement child) {
        return child.Name() == name;
    });
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className) {
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

// The elements of the taskbar are nested differently in different Windows
// versions. The Start button, for example, holds its icon either directly in
// its root panel or in a grid within it, so it and its icon are looked up among
// all of the descendants and not only among the direct children.
FrameworkElement FindDescendant(
    FrameworkElement element,
    const std::function<bool(FrameworkElement)>& predicate) {
    FrameworkElement result = nullptr;

    EnumChildElements(element, [&predicate, &result](FrameworkElement child) {
        result = predicate(child) ? child : FindDescendant(child, predicate);
        return !!result;
    });

    return result;
}

FrameworkElement FindDescendantByName(FrameworkElement element, PCWSTR name) {
    return FindDescendant(element, [name](FrameworkElement child) {
        return child.Name() == name;
    });
}

bool IsStartButton(FrameworkElement element) {
    return winrt::get_class_name(element) ==
               L"Taskbar.ExperienceToggleButton" &&
           Automation::AutomationProperties::GetAutomationId(element) ==
               L"StartButton";
}

struct HslColor {
    double hue;         // [0, 360)
    double saturation;  // [0, 1]
    double lightness;   // [0, 1]
};

HslColor RgbToHsl(winrt::Windows::UI::Color color) {
    double r = color.R / 255.0;
    double g = color.G / 255.0;
    double b = color.B / 255.0;

    double max = std::max({r, g, b});
    double min = std::min({r, g, b});
    double delta = max - min;

    HslColor hsl{
        .hue = 0,
        .saturation = 0,
        .lightness = (max + min) / 2,
    };

    if (delta > 0) {
        hsl.saturation = delta / (1 - std::abs(2 * hsl.lightness - 1));

        if (max == r) {
            hsl.hue = (g - b) / delta;
        } else if (max == g) {
            hsl.hue = (b - r) / delta + 2;
        } else {
            hsl.hue = (r - g) / delta + 4;
        }

        hsl.hue *= 60;
        if (hsl.hue < 0) {
            hsl.hue += 360;
        }
    }

    return hsl;
}

winrt::Windows::UI::Color HslToRgb(HslColor hsl, uint8_t alpha) {
    double c = (1 - std::abs(2 * hsl.lightness - 1)) * hsl.saturation;
    double h = hsl.hue / 60;
    double x = c * (1 - std::abs(std::fmod(h, 2) - 1));
    double m = hsl.lightness - c / 2;

    double r = 0;
    double g = 0;
    double b = 0;
    if (h < 1) {
        r = c;
        g = x;
    } else if (h < 2) {
        r = x;
        g = c;
    } else if (h < 3) {
        g = c;
        b = x;
    } else if (h < 4) {
        g = x;
        b = c;
    } else if (h < 5) {
        r = x;
        b = c;
    } else {
        r = c;
        b = x;
    }

    auto toByte = [m](double value) {
        return static_cast<uint8_t>(
            std::lround(std::clamp(value + m, 0.0, 1.0) * 255));
    };

    return {alpha, toByte(r), toByte(g), toByte(b)};
}

winrt::Windows::UI::Color TransformColor(winrt::Windows::UI::Color color) {
    HslColor hsl = RgbToHsl(color);

    if (g_settings.colorHue >= 0) {
        hsl.hue = g_settings.colorHue;
    }

    hsl.hue = std::fmod(hsl.hue + g_settings.hue, 360);
    if (hsl.hue < 0) {
        hsl.hue += 360;
    }

    hsl.saturation =
        std::clamp(hsl.saturation * g_settings.saturation, 0.0, 1.0);
    hsl.lightness = std::clamp(hsl.lightness * g_settings.brightness, 0.0, 1.0);

    double alpha = std::clamp(color.A / 255.0 * g_settings.opacity, 0.0, 1.0);

    return HslToRgb(hsl, static_cast<uint8_t>(std::lround(alpha * 255)));
}

// The colors of the parts of the icon which take part in its animations are
// set by key frames, which the icon inserts anew every time it starts
// animating. They are customized as they are inserted, since a property which
// an animation drives can't be assigned to.
thread_local bool g_creatingIconAnimations;

using IColorKeyFrameAnimation_InsertKeyFrame_t =
    HRESULT(WINAPI*)(void* pThis,
                     float normalizedProgressKey,
                     winrt::Windows::UI::Color value);
IColorKeyFrameAnimation_InsertKeyFrame_t
    IColorKeyFrameAnimation_InsertKeyFrame_Original;
HRESULT WINAPI
IColorKeyFrameAnimation_InsertKeyFrame_Hook(void* pThis,
                                            float normalizedProgressKey,
                                            winrt::Windows::UI::Color value) {
    if (g_creatingIconAnimations) {
        value = TransformColor(value);
    }

    return IColorKeyFrameAnimation_InsertKeyFrame_Original(
        pThis, normalizedProgressKey, value);
}

using IColorKeyFrameAnimation_InsertKeyFrameWithEasingFunction_t =
    HRESULT(WINAPI*)(void* pThis,
                     float normalizedProgressKey,
                     winrt::Windows::UI::Color value,
                     void* easingFunction);
IColorKeyFrameAnimation_InsertKeyFrameWithEasingFunction_t
    IColorKeyFrameAnimation_InsertKeyFrameWithEasingFunction_Original;
HRESULT WINAPI IColorKeyFrameAnimation_InsertKeyFrameWithEasingFunction_Hook(
    void* pThis,
    float normalizedProgressKey,
    winrt::Windows::UI::Color value,
    void* easingFunction) {
    if (g_creatingIconAnimations) {
        value = TransformColor(value);
    }

    return IColorKeyFrameAnimation_InsertKeyFrameWithEasingFunction_Original(
        pThis, normalizedProgressKey, value, easingFunction);
}

// The vtable of Windows.UI.Composition.IColorKeyFrameAnimation starts with the
// six methods of IInspectable, followed by the getter and the setter of
// InterpolationColorSpace.
constexpr int kInsertKeyFrameVtableIndex = 8;
constexpr int kInsertKeyFrameWithEasingFunctionVtableIndex = 9;

// All of the color animations of the process share the hooked implementation,
// which is why the hooks only customize colors while the icon creates its
// animations.
void HookColorKeyFrameAnimation(Composition::Compositor compositor) {
    void** vtable =
        *(void***)winrt::get_abi(compositor.CreateColorKeyFrameAnimation());

    WindhawkUtils::SetFunctionHook(
        (IColorKeyFrameAnimation_InsertKeyFrame_t)
            vtable[kInsertKeyFrameVtableIndex],
        IColorKeyFrameAnimation_InsertKeyFrame_Hook,
        &IColorKeyFrameAnimation_InsertKeyFrame_Original);

    WindhawkUtils::SetFunctionHook(
        (IColorKeyFrameAnimation_InsertKeyFrameWithEasingFunction_t)
            vtable[kInsertKeyFrameWithEasingFunctionVtableIndex],
        IColorKeyFrameAnimation_InsertKeyFrameWithEasingFunction_Hook,
        &IColorKeyFrameAnimation_InsertKeyFrameWithEasingFunction_Original);

    Wh_ApplyHookOperations();
}

// The original color is kept in the property set of the brush or of the
// gradient stop it belongs to, so that applying the effects again, as well as
// restoring the icon, always start from it.
constexpr PCWSTR kOriginalColorProperty = L"WindhawkOriginalColor";

winrt::Windows::UI::Color CustomizedColor(
    Composition::CompositionObject colorOwner,
    winrt::Windows::UI::Color currentColor) {
    auto properties = colorOwner.Properties();

    winrt::Windows::UI::Color originalColor;
    if (properties.TryGetColor(kOriginalColorProperty, originalColor) !=
        Composition::CompositionGetValueStatus::Succeeded) {
        originalColor = currentColor;
        properties.InsertColor(kOriginalColorProperty, originalColor);
    }

    return g_unloading || !g_settings.customizeColors
               ? originalColor
               : TransformColor(originalColor);
}

void ApplyBrushColors(Composition::CompositionBrush brush) {
    if (auto colorBrush = brush.try_as<Composition::CompositionColorBrush>()) {
        colorBrush.Color(CustomizedColor(colorBrush, colorBrush.Color()));
        return;
    }

    if (auto gradientBrush =
            brush.try_as<Composition::CompositionGradientBrush>()) {
        for (const auto& stop : gradientBrush.ColorStops()) {
            stop.Color(CustomizedColor(stop, stop.Color()));
        }
    }
}

void EnumShapeBrushes(
    Composition::CompositionShape shape,
    const std::function<void(Composition::CompositionBrush)>& enumCallback) {
    if (auto containerShape =
            shape.try_as<Composition::CompositionContainerShape>()) {
        for (const auto& child : containerShape.Shapes()) {
            EnumShapeBrushes(child, enumCallback);
        }
        return;
    }

    auto spriteShape = shape.try_as<Composition::CompositionSpriteShape>();
    if (!spriteShape) {
        return;
    }

    for (const auto& brush :
         {spriteShape.FillBrush(), spriteShape.StrokeBrush()}) {
        if (brush) {
            enumCallback(brush);
        }
    }
}

// The icon is drawn by an animated visual, a tree of shape visuals which the
// animated visual player of the button creates.
void EnumVisualBrushes(
    Composition::Visual visual,
    const std::function<void(Composition::CompositionBrush)>& enumCallback) {
    if (auto shapeVisual = visual.try_as<Composition::ShapeVisual>()) {
        for (const auto& shape : shapeVisual.Shapes()) {
            EnumShapeBrushes(shape, enumCallback);
        }
    }

    if (auto containerVisual = visual.try_as<Composition::ContainerVisual>()) {
        for (const auto& child : containerVisual.Children()) {
            EnumVisualBrushes(child, enumCallback);
        }
    }
}

void ApplyIconColors(FrameworkElement icon) {
    [[maybe_unused]] static bool hooked = [&icon] {
        HookColorKeyFrameAnimation(
            Hosting::ElementCompositionPreview::GetElementVisual(icon)
                .Compositor());
        return true;
    }();

    // The player hosts the animated visual as the child visual of the icon
    // element, not among the children of the visual which backs the element.
    auto visual =
        Hosting::ElementCompositionPreview::GetElementChildVisual(icon);
    if (!visual) {
        Wh_Log(L"The icon has no animated visual");
        return;
    }

    int count = 0;
    EnumVisualBrushes(visual, [&count](Composition::CompositionBrush brush) {
        ApplyBrushColors(brush);
        count++;
    });

    Wh_Log(L"Applied the colors of %d brushes", count);
}

// The icon is scaled with a render transform, which, unlike its width and
// height, doesn't affect the layout of the button.
void ApplyIconScale(FrameworkElement icon) {
    if (g_unloading || !g_settings.customizeSize) {
        icon.ClearValue(UIElement::RenderTransformProperty());
        icon.ClearValue(UIElement::RenderTransformOriginProperty());
        return;
    }

    Media::ScaleTransform scaleTransform = nullptr;
    if (auto renderTransform = icon.RenderTransform()) {
        scaleTransform = renderTransform.try_as<Media::ScaleTransform>();
    }

    if (!scaleTransform) {
        scaleTransform = Media::ScaleTransform();
        icon.RenderTransform(scaleTransform);
    }

    icon.RenderTransformOrigin({0.5f, 0.5f});
    scaleTransform.ScaleX(g_settings.size);
    scaleTransform.ScaleY(g_settings.size);
}

void ApplyStartButtonStyle(FrameworkElement startButton) {
    FrameworkElement icon = FindDescendantByName(startButton, L"Icon");
    if (!icon) {
        Wh_Log(L"Failed to find the Start button icon");
        return;
    }

    ApplyIconColors(icon);
    ApplyIconScale(icon);
}

bool ApplyStyle(XamlRoot xamlRoot) {
    FrameworkElement child = xamlRoot.Content().try_as<FrameworkElement>();
    if (!child ||
        !(child = FindChildByClassName(child, L"Taskbar.TaskbarFrame")) ||
        !(child = FindChildByName(child, L"RootGrid")) ||
        !(child = FindChildByName(child, L"TaskbarFrameRepeater"))) {
        return false;
    }

    FrameworkElement startButton = FindDescendant(child, IsStartButton);
    if (!startButton) {
        Wh_Log(L"Failed to find the Start button");
        return false;
    }

    ApplyStartButtonStyle(startButton);

    return true;
}

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

    size_t taskbarElementIUnknownOffset = 0x10;

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
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        // fd030091 mov     fp, sp
        // 080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
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

void ApplySettingsFromTaskbarThread() {
    Wh_Log(L"Applying settings");

    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) -> BOOL {
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

            if (!xamlRoot) {
                Wh_Log(L"Getting XamlRoot failed");
                return TRUE;
            }

            if (!ApplyStyle(xamlRoot)) {
                Wh_Log(L"ApplyStyle failed");
                return TRUE;
            }

            return TRUE;
        },
        0);
}

void ApplySettings(HWND hTaskbarWnd) {
    RunFromWindowThread(
        hTaskbarWnd, [](void* pParam) { ApplySettingsFromTaskbarThread(); }, 0);
}

// The hooked methods below are members of the button implementation object, of
// which the second interface pointer is the XAML element. Returns null for the
// other taskbar buttons, which the mod doesn't customize.
FrameworkElement GetStartButtonElement(void* pThis) {
    FrameworkElement element = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(element));
    if (!element || !IsStartButton(element)) {
        return nullptr;
    }

    return element;
}

// Runs on every state change of the button, which is when the icon is
// customized again in case the player recreated the animated visual.
using ExperienceToggleButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
ExperienceToggleButton_UpdateVisualStates_t
    ExperienceToggleButton_UpdateVisualStates_Original;
void WINAPI ExperienceToggleButton_UpdateVisualStates_Hook(void* pThis) {
    Wh_Log(L">");

    ExperienceToggleButton_UpdateVisualStates_Original(pThis);

    if (auto startButton = GetStartButtonElement(pThis)) {
        ApplyStartButtonStyle(startButton);
    }
}

// Runs when the icon is created, with the original colors.
using ExperienceToggleButton_InitializeAnimatedVisualPlayer_t =
    void(WINAPI*)(void* pThis);
ExperienceToggleButton_InitializeAnimatedVisualPlayer_t
    ExperienceToggleButton_InitializeAnimatedVisualPlayer_Original;
void WINAPI
ExperienceToggleButton_InitializeAnimatedVisualPlayer_Hook(void* pThis) {
    Wh_Log(L">");

    ExperienceToggleButton_InitializeAnimatedVisualPlayer_Original(pThis);

    if (auto startButton = GetStartButtonElement(pThis)) {
        ApplyStartButtonStyle(startButton);
    }
}

// Runs whenever the icon recreates the animations which drive it, among them
// the color animations of its animated parts. There's an animated visual per
// theme, each with its own implementation.
using StartIconAnimatedVisual_CreateAnimations_t = void(WINAPI*)(void* pThis);

void CreateIconAnimations(void* pThis,
                          StartIconAnimatedVisual_CreateAnimations_t original) {
    struct FlagGuard {
        bool saved = g_creatingIconAnimations;
        ~FlagGuard() { g_creatingIconAnimations = saved; }
    } guard;

    g_creatingIconAnimations = !g_unloading && g_settings.customizeColors;
    original(pThis);
}

StartIconAnimatedVisual_CreateAnimations_t
    StartDarkAnimatedVisual_CreateAnimations_Original;
void WINAPI StartDarkAnimatedVisual_CreateAnimations_Hook(void* pThis) {
    Wh_Log(L">");

    CreateIconAnimations(pThis,
                         StartDarkAnimatedVisual_CreateAnimations_Original);
}

StartIconAnimatedVisual_CreateAnimations_t
    StartLightAnimatedVisual_CreateAnimations_Original;
void WINAPI StartLightAnimatedVisual_CreateAnimations_Hook(void* pThis) {
    Wh_Log(L">");

    CreateIconAnimations(pThis,
                         StartLightAnimatedVisual_CreateAnimations_Original);
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::UpdateVisualStates(void))"},
            &ExperienceToggleButton_UpdateVisualStates_Original,
            ExperienceToggleButton_UpdateVisualStates_Hook,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::InitializeAnimatedVisualPlayer(void))"},
            &ExperienceToggleButton_InitializeAnimatedVisualPlayer_Original,
            ExperienceToggleButton_InitializeAnimatedVisualPlayer_Hook,
        },
        {
            {LR"(public: void __cdecl winrt::AnimatedVisuals::implementation::StartDark_AnimatedVisual::CreateAnimations(void))"},
            &StartDarkAnimatedVisual_CreateAnimations_Original,
            StartDarkAnimatedVisual_CreateAnimations_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::AnimatedVisuals::implementation::StartLight_AnimatedVisual::CreateAnimations(void))"},
            &StartLightAnimatedVisual_CreateAnimations_Original,
            StartLightAnimatedVisual_CreateAnimations_Hook,
            true,
        },
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
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
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }

    return module;
}

void LoadSettings() {
    // The hue each of the color options recolors the icon to, in degrees.
    static const struct {
        PCWSTR name;
        int hue;
    } colors[] = {
        {L"red", 0},    {L"orange", 30}, {L"yellow", 55},  {L"green", 120},
        {L"teal", 175}, {L"blue", 215},  {L"purple", 275}, {L"pink", 325},
    };

    PCWSTR color = Wh_GetStringSetting(L"color");
    g_settings.colorHue = -1;
    for (const auto& [name, hue] : colors) {
        if (wcscmp(color, name) == 0) {
            g_settings.colorHue = hue;
            break;
        }
    }
    Wh_FreeStringSetting(color);

    g_settings.hue = Wh_GetIntSetting(L"effects.hue");
    g_settings.saturation = Wh_GetIntSetting(L"effects.saturation") / 100.0;
    g_settings.brightness = Wh_GetIntSetting(L"effects.brightness") / 100.0;
    g_settings.opacity = Wh_GetIntSetting(L"effects.opacity") / 100.0;

    g_settings.size = Wh_GetIntSetting(L"size") / 100.0;

    g_settings.customizeColors =
        g_settings.colorHue >= 0 || g_settings.hue != 0 ||
        g_settings.saturation != 1 || g_settings.brightness != 1 ||
        g_settings.opacity != 1;
    g_settings.customizeSize = g_settings.size != 1;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");

                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

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

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }
}
