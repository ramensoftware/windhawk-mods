// ==WindhawkMod==
// @id              taskbar-appearance-tuner
// @name            Taskbar appearance tuner
// @name:zh-CN      任务栏外观调节器
// @description     Adjust taskbar brightness and transparency, separately for the background and for the icons and text; optionally hide the taskbar top line and drag grip
// @description:zh-CN 分别调整任务栏背景与图标文字的亮度和不透明度，可隐藏任务栏顶部线和拖拽手柄
// @version         1.4.1
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
# Taskbar appearance tuner

> **English:** Dim the Windows 11 taskbar to a comfortable level instead of
> staring at full brightness all day — a healthier brightness for your eyes,
> and better protection for OLED displays against burn-in.
>
> **中文：** 将 Windows 11 任务栏调暗至舒适的水平，不必整日对着全亮的面板——
> 亮度更健康，护眼的同时也更好地保护 OLED 显示器，防止烧屏。

Fine-grained appearance control for the Windows 11 taskbar. Three independent
adjustments, each of which can be applied to three different targets:

* **Brightness** - from -100 (black) to 100 (white).
* **Opacity** - from 0 (fully transparent) to 100 (unchanged).

Each adjustment has its own target selector:

* **Taskbar background only** - the material behind the icons.
* **Icons and text only** - buttons, tray icons and labels.
* **Background, icons and text** - the whole taskbar.

Additionally:

* **Show the taskbar top line** - shows or hides the thin line at the top
  edge of the taskbar.
* **Hide the taskbar drag grip** - hides the small gray rounded handle at
  the top center of the taskbar. That handle is part of Windows itself
  (it shows while the taskbar is unlocked), it is not drawn by this mod,
  but it can be hidden here.

## How it works

The background is adjusted with a composition effect graph (a color matrix
shader over a live backdrop brush), which is pixel-accurate. The icons and
text are XAML content which cannot be filtered with shaders, so brightness
for that target is approximated with a black or white overlay which is drawn
on top of the icons. The overlay is a composition visual rather than a XAML
element, so it takes no part in the taskbar layout and cannot move anything.
Opacity is exact for all targets.

The icons and text layer is found by walking the taskbar visual tree: every
subtree which contains neither the background nor a flyout host is adjusted
as a unit. This covers all icon areas (the Start, search and Task View
buttons, the task buttons and the system tray) on every Windows build,
without depending on the class names of the individual containers.

## Notes

* Windows 11 only (the mod relies on the XAML taskbar visual tree).
* The background adjustments replace the stock taskbar material with a live
  backdrop of what is behind the taskbar. If the taskbar looks too plain with
  brightness set, consider combining with other taskbar mods.
* If nothing seems to happen, make sure at least one value differs from its
  neutral value (brightness 0, opacity 100).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- brightness: 0
  $name: Brightness
  $name:zh-CN: 亮度
  $description: >-
    Taskbar brightness adjustment, from -100 (black) to 100 (white), 0 means
    no change. For the icons and text target this is approximated with a
    black or white overlay.
  $description:zh-CN: 任务栏亮度调整，范围 -100（全黑）到 100（全白），0 表示不调整。当作用对象为“仅图标和文字”时，该效果用黑色或白色叠加层近似实现。
- brightnessTarget: both
  $name: Brightness target
  $name:zh-CN: 亮度作用对象
  $description: >-
    Which part of the taskbar the brightness adjustment applies to.
  $description:zh-CN: 亮度调整作用于任务栏的哪一部分。
  $options:
  - background: Taskbar background only
  - icons: Icons and text only
  - both: Background, icons and text
  $options:zh-CN:
  - background: 仅任务栏背景
  - icons: 仅图标和文字
  - both: 背景、图标和文字
- opacity: 100
  $name: Opacity
  $name:zh-CN: 不透明度
  $description: >-
    Taskbar opacity, from 0 (fully transparent) to 100 (unchanged).
  $description:zh-CN: 任务栏不透明度，范围 0（完全透明）到 100（不调整）。
- opacityTarget: both
  $name: Opacity target
  $name:zh-CN: 不透明度作用对象
  $description: >-
    Which part of the taskbar the opacity adjustment applies to.
  $description:zh-CN: 不透明度调整作用于任务栏的哪一部分。
  $options:
  - background: Taskbar background only
  - icons: Icons and text only
  - both: Background, icons and text
  $options:zh-CN:
  - background: 仅任务栏背景
  - icons: 仅图标和文字
  - both: 背景、图标和文字
- topLine: true
  $name: Show the taskbar top line
  $name:zh-CN: 显示任务栏顶部线
  $description: >-
    Whether to show the thin line at the top edge of the taskbar. When
    disabled, the line is hidden by clearing the taskbar border stroke.
  $description:zh-CN: 是否显示任务栏顶部的细线。关闭后通过清除任务栏边框描边来隐藏该线。
- hideGrip: false
  $name: Hide the taskbar drag grip
  $name:zh-CN: 隐藏任务栏拖拽手柄
  $description: >-
    Hides the small gray rounded handle at the top center of the taskbar
    (Rectangle#Gripper, a Windows element which shows while the taskbar is
    unlocked). It is part of Windows itself, not drawn by this mod.
  $description:zh-CN: 隐藏任务栏顶部中央的灰色圆角拖拽手柄（Rectangle#Gripper，Windows 自带元素，任务栏未锁定时显示，并非本 mod 绘制）。
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cwchar>
#include <functional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#undef GetCurrentTime

#include <d2d1_1.h>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>

namespace wuc = winrt::Windows::UI::Composition;
namespace wge = winrt::Windows::Graphics::Effects;

using namespace winrt::Windows::UI::Xaml;

////////////////////////////////////////////////////////////////////////////////
// windows.graphics.effects.interop.h (subset, from Windows 11 Taskbar Styler)
#ifndef BUILD_WINDOWS
namespace ABI {
#endif
namespace Windows {
namespace Graphics {
namespace Effects {

typedef interface IGraphicsEffectSource                         IGraphicsEffectSource;
typedef interface IGraphicsEffectD2D1Interop                    IGraphicsEffectD2D1Interop;

typedef enum GRAPHICS_EFFECT_PROPERTY_MAPPING
{
    GRAPHICS_EFFECT_PROPERTY_MAPPING_UNKNOWN,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORX,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORY,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORZ,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORW,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_RECT_TO_VECTOR4,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_RADIANS_TO_DEGREES,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_COLORMATRIX_ALPHA_MODE,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR3,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR4
} GRAPHICS_EFFECT_PROPERTY_MAPPING;

#undef INTERFACE
#define INTERFACE IGraphicsEffectD2D1Interop
DECLARE_INTERFACE_IID_(IGraphicsEffectD2D1Interop, IUnknown, "2FC57384-A068-44D7-A331-30982FCF7177")
{
    STDMETHOD(GetEffectId)(
        _Out_ GUID * id
        ) PURE;

    STDMETHOD(GetNamedPropertyMapping)(
        LPCWSTR name,
        _Out_ UINT * index,
        _Out_ GRAPHICS_EFFECT_PROPERTY_MAPPING * mapping
        ) PURE;

    STDMETHOD(GetPropertyCount)(
        _Out_ UINT * count
        ) PURE;

    STDMETHOD(GetProperty)(
        UINT index,
        _Outptr_ winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue> ** value
        ) PURE;

    STDMETHOD(GetSource)(
        UINT index,
        _Outptr_ IGraphicsEffectSource ** source
        ) PURE;

    STDMETHOD(GetSourceCount)(
        _Out_ UINT * count
        ) PURE;
};

} // namespace Effects
} // namespace Graphics
} // namespace Windows
#ifndef BUILD_WINDOWS
} // namespace ABI
#endif

template <> inline constexpr winrt::guid winrt::impl::guid_v<ABI::Windows::Graphics::Effects::IGraphicsEffectD2D1Interop>{
    0x2FC57384, 0xA068, 0x44D7, { 0xA3, 0x31, 0x30, 0x98, 0x2F, 0xCF, 0x71, 0x77 }
};

// Required for the ABI IPropertyValue pointer used by GetProperty(): the
// MinGW based compiler doesn't provide a uuid for the ABI projection of
// IPropertyValue, so it is aliased to the projected interface's uuid here.
template <> inline constexpr winrt::guid winrt::impl::guid_v<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>{
    winrt::impl::guid_v<winrt::Windows::Foundation::IPropertyValue>
};

namespace awge = ABI::Windows::Graphics::Effects;

// {921F03D6-641C-47DF-852D-B4BB6153AE11} - the Direct2D color matrix effect
// CLSID. It is duplicated here as a local constant instead of using the
// CLSID_D2D1ColorMatrix macro from d2d1effects.h, because that macro expands
// to an extern declaration which the Windhawk linker can't resolve.
constexpr GUID kColorMatrixEffectId{
    0x921f03d6, 0x641c, 0x47df, {0x85, 0x2d, 0xb4, 0xbb, 0x61, 0x53, 0xae, 0x11}};

////////////////////////////////////////////////////////////////////////////////
// ColorMatrixEffect (from Windows 11 Taskbar Styler)
struct ColorMatrixEffect
    : winrt::implements<ColorMatrixEffect,
                        wge::IGraphicsEffect,
                        wge::IGraphicsEffectSource,
                        awge::IGraphicsEffectD2D1Interop> {
    wge::IGraphicsEffectSource Source{nullptr};

    // D2D1_MATRIX_5X4_F: 5 rows x 4 columns (20 floats), identity by default.
    // Rows are the output RGBA channels, columns are the input RGBA channels,
    // and the last row holds the per-channel offsets.
    float Matrix[20] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
        0, 0, 0, 0,
    };

    uint32_t AlphaMode = D2D1_COLORMATRIX_ALPHA_MODE_PREMULTIPLIED;
    bool ClampOutput = false;

    // IGraphicsEffectD2D1Interop
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) {
            return E_INVALIDARG;
        }

        *id = kColorMatrixEffectId;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name,
        UINT* index,
        awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
        if (!index || !mapping) {
            return E_INVALIDARG;
        }

        const std::wstring_view nameView(name);
        if (nameView == L"ColorMatrix") {
            *index = D2D1_COLORMATRIX_PROP_COLOR_MATRIX;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }

        if (nameView == L"AlphaMode") {
            *index = D2D1_COLORMATRIX_PROP_ALPHA_MODE;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }

        if (nameView == L"ClampOutput") {
            *index = D2D1_COLORMATRIX_PROP_CLAMP_OUTPUT;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }

        return E_INVALIDARG;
    }

    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {
        if (!count) {
            return E_INVALIDARG;
        }

        *count = 3;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetProperty(
        UINT index,
        winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>**
            value) noexcept override try {
        if (!value) {
            return E_INVALIDARG;
        }

        switch (index) {
            case D2D1_COLORMATRIX_PROP_COLOR_MATRIX:
                *value = winrt::Windows::Foundation::PropertyValue::
                    CreateSingleArray(winrt::array_view<const float>(
                        Matrix, Matrix + 20))
                        .as<winrt::impl::abi_t<
                            winrt::Windows::Foundation::IPropertyValue>>()
                        .detach();
                break;

            case D2D1_COLORMATRIX_PROP_ALPHA_MODE:
                *value = winrt::Windows::Foundation::PropertyValue::
                    CreateUInt32(AlphaMode)
                        .as<winrt::impl::abi_t<
                            winrt::Windows::Foundation::IPropertyValue>>()
                        .detach();
                break;

            case D2D1_COLORMATRIX_PROP_CLAMP_OUTPUT:
                *value = winrt::Windows::Foundation::PropertyValue::
                    CreateBoolean(ClampOutput)
                        .as<winrt::impl::abi_t<
                            winrt::Windows::Foundation::IPropertyValue>>()
                        .detach();
                break;

            default:
                return E_BOUNDS;
        }

        return S_OK;
    }
    catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE GetSource(
        UINT index,
        awge::IGraphicsEffectSource** source) noexcept override {
        if (!source) {
            return E_INVALIDARG;
        }

        if (index == 0 && Source) {
            winrt::copy_to_abi(Source, *reinterpret_cast<void**>(source));
            return S_OK;
        }

        return E_BOUNDS;
    }

    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) {
            return E_INVALIDARG;
        }

        *count = 1;
        return S_OK;
    }

    // IGraphicsEffect
    winrt::hstring Name() { return m_name; }

    void Name(winrt::hstring name) { m_name = std::move(name); }

private:
    winrt::hstring m_name = L"ColorMatrixEffect";
};

////////////////////////////////////////////////////////////////////////////////
// Diagnostics
//
// Wh_Log() output is only visible with a debug output viewer attached. The same
// messages are mirrored into a small text file in the temp directory, which
// makes a field report a single file to look at. The file is truncated on every
// settings change, so it always holds the most recent run.

void AppendDebugFile(std::wstring const& text) {
    if (text.empty()) {
        return;
    }

    WCHAR tempPath[MAX_PATH];
    DWORD tempPathLength = GetTempPath(ARRAYSIZE(tempPath), tempPath);
    if (!tempPathLength || tempPathLength > ARRAYSIZE(tempPath) - 40) {
        return;
    }

    std::wstring path(tempPath);
    path += L"taskbar-appearance-tuner.txt";

    HANDLE file = CreateFileW(path.c_str(), FILE_APPEND_DATA,
                              FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                              OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return;
    }

    int utf8Length = WideCharToMultiByte(CP_UTF8, 0, text.c_str(),
                                         (int)text.size(), nullptr, 0, nullptr,
                                         nullptr);
    if (utf8Length > 0) {
        std::string utf8(utf8Length, '\0');
        WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(),
                            utf8.data(), utf8Length, nullptr, nullptr);

        DWORD written = 0;
        WriteFile(file, utf8.data(), (DWORD)utf8.size(), &written, nullptr);
        WriteFile(file, "\r\n", 2, &written, nullptr);
    }

    CloseHandle(file);
}

void ResetDebugFile() {
    WCHAR tempPath[MAX_PATH];
    DWORD tempPathLength = GetTempPath(ARRAYSIZE(tempPath), tempPath);
    if (!tempPathLength || tempPathLength > ARRAYSIZE(tempPath) - 40) {
        return;
    }

    std::wstring path(tempPath);
    path += L"taskbar-appearance-tuner.txt";
    DeleteFileW(path.c_str());
}

// Logs a line to Wh_Log() and to the debug file.
void DiagLine(std::wstring const& text) {
    Wh_Log(L"%s", text.c_str());
    AppendDebugFile(text);
}

// Logs a printf-style line (ASCII class names and numbers).
void Diag(const char* format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    WCHAR wideBuffer[1024];
    if (!MultiByteToWideChar(CP_UTF8, 0, buffer, -1, wideBuffer,
                             ARRAYSIZE(wideBuffer))) {
        return;
    }

    DiagLine(wideBuffer);
}

////////////////////////////////////////////////////////////////////////////////
// BackdropAdjustBrush: renders the live backdrop (what is behind the taskbar)
// with a brightness adjustment via a color matrix effect.
class BackdropAdjustBrush
    : public Media::XamlCompositionBrushBaseT<BackdropAdjustBrush> {
public:
    BackdropAdjustBrush(wuc::Compositor const& compositor, float brightness)
        : m_compositor(compositor),
          m_brightness(brightness) {}

    void OnConnected() {
        if (CompositionBrush()) {
            return;
        }

        CompositionBrush(CreateEffectBrush());
    }

    void OnDisconnected() {
        CompositionBrush(nullptr);
    }

private:
    wuc::CompositionBrush CreateEffectBrush() {
        auto backdropBrush = m_compositor.CreateBackdropBrush();

        // Brightness: negative scales toward black, positive fades toward
        // white (pivot at mid-gray), both linear and clamped.
        auto brightMatrix = winrt::make_self<ColorMatrixEffect>();
        brightMatrix->Source = wuc::CompositionEffectSourceParameter(L"source");

        float b = std::clamp(m_brightness, -1.0f, 1.0f);
        float scale = b < 0.0f ? 1.0f + b : 1.0f;
        float offset = b > 0.0f ? b : 0.0f;
        auto& bm = brightMatrix->Matrix;
        bm[0] = scale; bm[1] = 0.0f;  bm[2] = 0.0f;  bm[3] = 0.0f;
        bm[4] = 0.0f;  bm[5] = scale; bm[6] = 0.0f;  bm[7] = 0.0f;
        bm[8] = 0.0f;  bm[9] = 0.0f;  bm[10] = scale; bm[11] = 0.0f;
        bm[12] = 0.0f; bm[13] = 0.0f; bm[14] = 0.0f; bm[15] = 1.0f;
        bm[16] = offset; bm[17] = offset; bm[18] = offset; bm[19] = 0.0f;
        brightMatrix->ClampOutput = true;
        brightMatrix->Name(L"BrightnessEffect");

        Diag("Effect graph: brightness=%.2f (scale=%.2f, offset=%.2f)", b,
             scale, offset);

        auto factory = m_compositor.CreateEffectFactory(*brightMatrix);
        auto brush = factory.CreateBrush();
        brush.SetSourceParameter(L"source", backdropBrush);

        return brush;
    }

    wuc::Compositor m_compositor;
    float m_brightness;
};

////////////////////////////////////////////////////////////////////////////////
// Mod logic

// Which part of the taskbar an adjustment is applied to. The settings are
// string enumerations, which makes Windhawk render them as a drop-down list.
enum class AppearanceTarget {
    Background,
    Icons,
    Both,
};

// The legacy numeric values are still accepted, so that a configuration saved
// by an older version of the mod doesn't silently change behaviour.
AppearanceTarget ParseTarget(PCWSTR value) {
    if (!value) {
        return AppearanceTarget::Both;
    }
    if (wcscmp(value, L"background") == 0 || wcscmp(value, L"0") == 0) {
        return AppearanceTarget::Background;
    }
    if (wcscmp(value, L"icons") == 0 || wcscmp(value, L"1") == 0) {
        return AppearanceTarget::Icons;
    }
    return AppearanceTarget::Both;
}

struct {
    int brightness;  // -100..100, 0 = no change
    AppearanceTarget brightnessTarget;
    int opacity;  // 0..100, 100 = no change
    AppearanceTarget opacityTarget;
    bool topLine;  // false = hide the taskbar top line
    bool hideGrip;  // true = hide the taskbar drag grip handle
} g_settings;

std::atomic<bool> g_unloading{false};

// The taskbar XAML tree is dumped to the diagnostic file once per mod load,
// and only after it turned out to be populated.
std::atomic<bool> g_treeDumped{false};

// Narrow, because it is only used with the printf-style Diag() helper.
const char* TargetName(AppearanceTarget target) {
    switch (target) {
        case AppearanceTarget::Background:
            return "background";
        case AppearanceTarget::Icons:
            return "icons";
        default:
            return "both";
    }
}

struct AppearanceState {
    // The XamlRoot content element of the taskbar this state belongs to. It is
    // used to look the state up again when the settings change.
    winrt::weak_ref<FrameworkElement> root;
    winrt::weak_ref<FrameworkElement> backgroundFill;
    Media::Brush originalFill{nullptr};
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
    // Foreground elements which carry a brightness overlay visual. The visual
    // is attached as a composition child of the element, so the element itself
    // is what has to be remembered for the restore.
    std::vector<winrt::weak_ref<FrameworkElement>> overlays;
};

std::vector<AppearanceState> g_states;

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

// Recursively enumerates all descendants of the given element.
void EnumDescendants(FrameworkElement element,
                     std::function<bool(FrameworkElement)> enumCallback) {
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

        EnumDescendants(child, enumCallback);
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

// Describes an element for the log as "Class#Name".
// Converts a wide string to UTF-8, for the printf-style logging helper.
std::string Narrow(std::wstring const& text) {
    if (text.empty()) {
        return std::string();
    }

    int length = WideCharToMultiByte(CP_UTF8, 0, text.c_str(),
                                     (int)text.size(), nullptr, 0, nullptr,
                                     nullptr);
    if (length <= 0) {
        return std::string();
    }

    std::string result(length, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(),
                        result.data(), length, nullptr, nullptr);
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
// "(x, y w x h)", for the diagnostics file.
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
            DiagLine(L"Icon layer: " + DescribeElement(element) + L" " +
                     DescribeRect(element, content));
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
    std::wstring dump;
    int logged = 0;

    auto walk = [&](auto&& self, FrameworkElement element, int depth) -> void {
        if (!element || depth > maxDepth || logged >= maxElements) {
            return;
        }

        logged++;
        dump.append((size_t)depth * 2, L' ');
        dump += DescribeElement(element);
        dump += L"\r\n";

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
    AppendDebugFile(dump);
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

    Diag("Icon layers: %d, content %.0fx%.0f, visible band %.0f..%.0f",
         (int)elements.size(), content.ActualWidth(), content.ActualHeight(),
         bandTop, bandBottom);

    if (elements.empty()) {
        static bool dumped = false;
        if (!dumped) {
            dumped = true;
            DiagLine(L"Icon and text layer not found");
        }
    }

    return elements;
}

void RemoveBrightnessOverlay(FrameworkElement element);

void RestoreState(AppearanceState& state) {
    if (auto backgroundFill = state.backgroundFill.get()) {
        if (auto rect = backgroundFill.try_as<Shapes::Rectangle>()) {
            rect.Fill(state.originalFill);
        }
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

    for (auto& overlayTarget : state.overlays) {
        if (auto element = overlayTarget.get()) {
            RemoveBrightnessOverlay(element);
        }
    }
}

// Restores all tracked taskbars and forgets about them. Used when all settings
// are back to their defaults, or when the mod is unloading, so that nothing is
// left adjusted.
void RestoreAllStates() {
    for (auto& state : g_states) {
        RestoreState(state);
    }

    g_states.clear();
}

std::pair<AppearanceState*, size_t> FindStateForRoot(
    FrameworkElement root) {
    for (size_t i = 0; i < g_states.size(); i++) {
        if (auto stateRoot = g_states[i].root.get(); stateRoot &&
                                                    stateRoot == root) {
            return {&g_states[i], i};
        }
    }

    return {nullptr, 0};
}

bool ShouldAdjustBackground(AppearanceTarget target) {
    return target == AppearanceTarget::Background ||
           target == AppearanceTarget::Both;
}

bool ShouldAdjustForeground(AppearanceTarget target) {
    return target == AppearanceTarget::Icons || target == AppearanceTarget::Both;
}

bool IsNeutral() {
    return g_settings.brightness == 0 && g_settings.opacity == 100 &&
           g_settings.topLine && !g_settings.hideGrip;
}

// Applies the brightness approximation to one foreground element: a solid
// black or white visual is drawn on top of the element's content.
//
// The overlay deliberately is not an element of the XAML tree. The taskbar
// lays its content out in StackPanels, where an inserted child consumes layout
// space of its own: the panel then grows by the width of the overlay and every
// neighbouring element is pushed away (the icons visibly shift to the left).
// A composition child visual is invisible to the XAML layout while still being
// drawn on top of the element's content, so it cannot move anything.
bool ApplyBrightnessOverlay(FrameworkElement element,
                            BYTE r,
                            BYTE g,
                            BYTE b,
                            double alpha) {
    double width = element.ActualWidth();
    double height = element.ActualHeight();
    if (!(width > 0) || !(height > 0)) {
        return false;
    }

    try {
        auto compositor =
            winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::
                GetElementVisual(element)
                    .Compositor();

        BYTE alphaByte =
            (BYTE)std::lround(std::clamp(alpha, 0.0, 1.0) * 255.0);

        auto overlay = compositor.CreateSpriteVisual();
        overlay.Brush(compositor.CreateColorBrush(
            winrt::Windows::UI::Color{alphaByte, r, g, b}));
        overlay.Size({(float)width, (float)height});

        // SetElementChildVisual adds the visual as the last child of the
        // element's visual tree, which is the top of its z-order.
        winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::
            SetElementChildVisual(element, overlay);
        return true;
    } catch (winrt::hresult_error const& e) {
        Diag("  brightness overlay failed: %08X", (unsigned)e.code().value);
        return false;
    } catch (...) {
        Diag("  brightness overlay failed (unknown error)");
        return false;
    }
}

// Removes a brightness overlay which was previously attached to the element.
void RemoveBrightnessOverlay(FrameworkElement element) {
    try {
        winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::
            SetElementChildVisual(
                element, winrt::Windows::UI::Composition::Visual{nullptr});
    } catch (...) {
        // The element may already be gone, in which case there is nothing to
        // restore.
    }
}

// How much of a full black or white overlay corresponds to a brightness of
// -100 or 100 for the icons and text target. Below 1.0 so that the icons
// stay recognizable at the extremes.
constexpr double kForegroundBrightnessStrength = 0.8;

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
        Diag("BackgroundStroke not found");
        return;
    }

    auto topLineRect = topLineElem.try_as<Shapes::Rectangle>();
    if (!topLineRect) {
        Diag("BackgroundStroke is not a Rectangle");
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
        Diag("Top line hidden (original stroke thickness=%.1f)",
             state.originalTopLineThickness);
    }
}

// Records and optionally hides the drag grip handle: the small gray rounded
// handle at the top center of the taskbar (Rectangle#Gripper inside
// Taskbar.Gripper#GripperControl). It is part of Windows (it shows while the
// taskbar is unlocked) and is not affected by the brightness and opacity
// adjustments, but it can be hidden on request.
void ApplyGripStyle(FrameworkElement content, AppearanceState& state) {
    auto gripElem = FindDescendantByName(content, L"Gripper");
    if (!gripElem) {
        Diag("Grip handle not found");
        return;
    }

    state.grip = winrt::make_weak(gripElem);
    state.originalGripVisibility = gripElem.Visibility();

    if (g_settings.hideGrip) {
        gripElem.Visibility(Visibility::Collapsed);
        Diag("Grip handle hidden");
    }
}

// Adjusts the background rectangle: opacity directly, brightness by replacing
// the fill with a live backdrop brush filtered by a color matrix effect.
void ApplyBackgroundStyle(Shapes::Rectangle const& backgroundFill) {
    bool hasOpacity =
        ShouldAdjustBackground(g_settings.opacityTarget) &&
        g_settings.opacity != 100;
    bool hasBrightness =
        ShouldAdjustBackground(g_settings.brightnessTarget) &&
        g_settings.brightness != 0;

    if (hasOpacity) {
        backgroundFill.Opacity(g_settings.opacity / 100.0);
    }

    if (!hasBrightness) {
        Diag("Background: no effect requested");
        return;
    }

    try {
        auto compositor =
            winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::
                GetElementVisual(backgroundFill)
                    .Compositor();
        float brightness = g_settings.brightness / 100.0f;
        auto brush = winrt::make<BackdropAdjustBrush>(compositor, brightness);
        backgroundFill.Fill(brush);
        Diag("Background fill replaced (brightness=%.2f)", brightness);
    } catch (winrt::hresult_error const& e) {
        Diag("Failed to create backdrop brush: %08X (error)",
             (unsigned)e.code().value);
        // Keep going with the other adjustments.
    } catch (...) {
        Diag("Failed to create backdrop brush (unknown error)");
        // Keep going with the other adjustments.
    }
}

// Adjusts the icon and text elements: opacity directly, brightness with a
// black or white overlay visual on top of each element's content.
void ApplyForegroundStyle(std::vector<FrameworkElement> const& elements,
                          AppearanceState& state) {
    bool hasOpacity =
        ShouldAdjustForeground(g_settings.opacityTarget) &&
        g_settings.opacity != 100;
    bool hasBrightness =
        ShouldAdjustForeground(g_settings.brightnessTarget) &&
        g_settings.brightness != 0;

    Diag("Foreground elements found: %d", (int)elements.size());

    for (auto& element : elements) {
        Diag("Foreground element: %s",
             Narrow(DescribeElement(element)).c_str());

        if (hasOpacity) {
            state.foregroundOpacity.emplace_back(winrt::make_weak(element),
                                                 element.Opacity());
            element.Opacity(g_settings.opacity / 100.0);
        }

        if (!hasBrightness) {
            continue;
        }

        double b = g_settings.brightness / 100.0;
        bool black = b < 0;
        double alpha =
            (black ? -b : b) * kForegroundBrightnessStrength;
        BYTE color = black ? 0 : 255;

        if (ApplyBrightnessOverlay(element, color, color, color, alpha)) {
            state.overlays.push_back(winrt::make_weak(element));
            Diag("  brightness overlay: %s alpha=%.2f (%.0fx%.0f)",
                 black ? "black" : "white", alpha, element.ActualWidth(),
                 element.ActualHeight());
        } else {
            Diag("  brightness overlay skipped (element has no size)");
        }
    }
}

bool ApplyStyle(XamlRoot xamlRoot) {
    auto content = xamlRoot.Content().as<FrameworkElement>();
    if (!content) {
        return false;
    }

    Diag("Applying settings: brightness=%d (%s), opacity=%d (%s)",
         g_settings.brightness, TargetName(g_settings.brightnessTarget),
         g_settings.opacity, TargetName(g_settings.opacityTarget));

    // The visual tree is keyed by the root content element.
    // When all settings are back to their defaults, or when the mod is
    // unloading, everything must be restored to its original appearance.
    if (g_unloading || IsNeutral()) {
        Diag("%s, restoring original appearance",
             g_unloading ? "Mod is unloading" : "All settings are neutral");
        RestoreAllStates();
        return true;
    }

    auto [state, stateIndex] = FindStateForRoot(content);

    // Locate the taskbar background rectangle and the root grid.
    auto backgroundFillElem = FindDescendantByName(content, L"BackgroundFill");
    if (!backgroundFillElem) {
        Diag("BackgroundFill not found (unsupported taskbar?)");
        return false;
    }

    auto backgroundFill = backgroundFillElem.try_as<Shapes::Rectangle>();
    if (!backgroundFill) {
        Diag("BackgroundFill is not a Rectangle");
        return false;
    }

    // The tree is populated (the background rectangle exists), so dump it once
    // per mod load: a field report then always contains the real element names
    // and the real structure of the Windows build it was made on.
    if (!g_treeDumped.exchange(true)) {
        DumpVisualTree(content, 7, 260);
    }

    bool hasForegroundOpacity =
        ShouldAdjustForeground(g_settings.opacityTarget) &&
        g_settings.opacity != 100;
    bool hasForegroundBrightness =
        ShouldAdjustForeground(g_settings.brightnessTarget) &&
        g_settings.brightness != 0;

    // The icon and text layer is collected up front. If it is requested but
    // comes back empty, the taskbar content exists while not being laid out
    // yet, i.e. all of its elements are still zero-sized. In that case nothing
    // is modified at all, so that the retry which covers this state doesn't
    // make the background flicker by re-applying itself over and over.
    std::vector<FrameworkElement> foregroundElements;
    if (hasForegroundOpacity || hasForegroundBrightness) {
        auto [bandTop, bandBottom] =
            GetVisibleBand(content, backgroundFillElem);
        foregroundElements =
            CollectForegroundElements(content, bandTop, bandBottom);
        if (foregroundElements.empty()) {
            Diag("Icon and text layer is empty, taskbar not laid out yet");
            return false;
        }
    }

    // Restore any previous state for this root before re-applying.
    if (state) {
        RestoreState(*state);
        g_states.erase(g_states.begin() + stateIndex);
    }

    AppearanceState newState;
    newState.root = winrt::make_weak(content);
    newState.backgroundFill =
        winrt::make_weak(backgroundFill.as<FrameworkElement>());
    newState.originalFill = backgroundFill.Fill();
    newState.originalFillOpacity = backgroundFill.Opacity();

    ApplyTopLineStyle(content, backgroundFillElem, newState);

    ApplyGripStyle(content, newState);

    ApplyBackgroundStyle(backgroundFill);

    if (hasForegroundOpacity || hasForegroundBrightness) {
        ApplyForegroundStyle(foregroundElements, newState);
    }

    g_states.push_back(std::move(newState));
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

    // The debug file holds the report of the most recent pass only, so that a
    // field report always describes the current state.
    ResetDebugFile();

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
                // callback can outlive the mod.
                StopApplyRetry(hWnd);
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

// Only ever touched on the taskbar window thread.
ULONGLONG g_applyRetryDeadline = 0;

void CALLBACK ApplyRetryTimerProc(HWND hWnd, UINT, UINT_PTR idEvent, DWORD) {
    if (idEvent != kApplyRetryTimerId) {
        return;
    }

    ApplyPassResult result;
    ApplyPassOnTaskbarThread(&result);

    if ((result.sawTaskbar && result.allReady) ||
        GetTickCount64() >= g_applyRetryDeadline) {
        Wh_Log(L"Apply retry finished (tree ready: %s)",
               result.allReady ? L"yes" : L"no");
        KillTimer(hWnd, kApplyRetryTimerId);
        g_applyRetryDeadline = 0;
    }
}

// Must be called on the taskbar window thread.
void ScheduleApplyRetry(HWND hTaskbarWnd) {
    // The deadline is only ever set once. The passes driven by the timer itself
    // report the same "not ready" state, and must not extend the retry window,
    // otherwise the timer would never stop.
    if (g_applyRetryDeadline == 0) {
        g_applyRetryDeadline = GetTickCount64() + kApplyRetryTotalMs;
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
    g_applyRetryDeadline = 0;
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
    auto brightnessTarget =
        WindhawkUtils::StringSetting::make(L"brightnessTarget");
    auto opacityTarget =
        WindhawkUtils::StringSetting::make(L"opacityTarget");

    g_settings.brightness =
        std::clamp(Wh_GetIntSetting(L"brightness"), -100, 100);
    g_settings.brightnessTarget = ParseTarget(brightnessTarget.get());
    g_settings.opacity = std::clamp(Wh_GetIntSetting(L"opacity"), 0, 100);
    g_settings.opacityTarget = ParseTarget(opacityTarget.get());
    g_settings.topLine = Wh_GetIntSetting(L"topLine") != 0;
    g_settings.hideGrip = Wh_GetIntSetting(L"hideGrip") != 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    // Wh_ModInit can run again without the DLL having been unloaded.
    g_treeDumped = false;
    g_applyRetryDeadline = 0;

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
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }
}
