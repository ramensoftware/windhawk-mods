// ==WindhawkMod==
// @id              taskbar-count-badges
// @name            Taskbar Count Badges
// @description     Show customizable window-count badges or vertical dots on Windows 11 taskbar.
// @version         1.0.0
// @author          digART
// @github          https://github.com/digart11
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==
// Copyright (C) 2026 digART
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// Taskbar hook, AppResolver, and taskbar XAML/UI-thread infrastructure include
// techniques adapted from Windhawk mods by Michael Maltsev (m417z), including
// Taskbar Labels for Windows 11 and Taskbar Tray Show on Hover.



// ==WindhawkModReadme==
/*
# Taskbar Count Badges

See how many windows are open for each app directly on the Windows 11 taskbar.

Taskbar Count Badges adds a small customizable indicator to taskbar app buttons
when an app has multiple open windows. The indicator can be shown as either a
**number badge** or a compact stack of **vertical dots**.

## Screenshots

### Customization examples

![Taskbar Count Badges examples](https://raw.githubusercontent.com/digart11/TaskbarCountBadges/main/images/showcase.png)

### Settings

![Taskbar Count Badges settings](https://raw.githubusercontent.com/digart11/TaskbarCountBadges/main/images/settings.png)

## Display styles

### Number badge

Shows the exact number of open windows for an app.

The badge can be customized with:

- Circle, rounded-square, or square shape
- Badge size and position
- Horizontal and vertical offsets
- Background, text, and border colors
- Border thickness
- Font family, size, and weight
- Configurable maximum number, with larger values shown using `+`

### Vertical dots

Shows a minimal vertical stack of dots beside the app icon.

- Position the dots on the left or right
- Change dot size and color
- Up to five dots are shown; five dots means **five or more windows**

## Behavior

By default, no indicator is shown for a single open window. The indicator appears
when an app reaches two open windows.

The minimum window count can be changed in the settings.

Window counts update automatically as windows are opened and closed, and
settings are applied live.

## Compatibility

- Windows 11 only
- Supports x64 and ARM64 Windows
- Works with multiple monitors and secondary Windows taskbars

The mod is designed primarily for grouped/combined taskbar app buttons. When
taskbar buttons are configured to stay uncombined, Windows can expose multiple
buttons for the same app group, so the same app count may appear on more than
one button.

Third-party taskbar replacements or tools that replace the native Windows 11
taskbar may not be compatible.

## Notes

The mod counts real top-level application windows and maps them to their Windows
application IDs. Windows shell infrastructure windows are excluded from the
count.

The badge is visual only and doesn't change taskbar grouping, combining, window
ordering, or application behavior.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Display:
  - style: number
    $name: Display style
    $description: Choose how multiple open windows are shown.
    $options:
    - number: Number badge
    - dots: Vertical dots
  $name: Display

- VerticalDots:
  - position: right
    $name: Position
    $options:
    - left: Left side
    - right: Right side

  - size: 4
    $name: Dot size

  - color: "#FFFFFF"
    $name: Dot color
  $name: Vertical dots
  $description: These settings apply only when Display style is set to Vertical dots.

- Badge:
  - shape: circle
    $name: Shape
    $options:
    - circle: Circle
    - rounded: Rounded square
    - square: Square

  - size: 16
    $name: Size
    $description: Badge diameter or square size in pixels.

  - position: topRight
    $name: Position
    $options:
    - topLeft: Top left
    - topCenter: Top center
    - topRight: Top right
    - bottomLeft: Bottom left
    - bottomCenter: Bottom center
    - bottomRight: Bottom right

  - offsetX: -2
    $name: Horizontal offset
    $description: Positive moves right, negative moves left.

  - offsetY: -2
    $name: Vertical offset
    $description: Positive moves down, negative moves up.

  - backgroundColor: "#D90000"
    $name: Background color

  - borderColor: "#FFFFFF"
    $name: Border color

  - borderThickness: 0
    $name: Border thickness
    $description: 0 disables the border.
  $name: Number badge
  $description: These settings apply only when Display style is set to Number badge.

- Text:
  - color: "#FFFFFF"
    $name: Text color

  - fontFamily: "Segoe UI"
    $name: Font family
    $options:
    - "Segoe UI": Segoe UI
    - "Segoe UI Variable": Segoe UI Variable
    - "Arial": Arial
    - "Calibri": Calibri
    - "Tahoma": Tahoma
    - "Verdana": Verdana
    - "Trebuchet MS": Trebuchet MS
    - "Consolas": Consolas
    - "Cascadia Code": Cascadia Code
    - "Cascadia Mono": Cascadia Mono

  - fontSize: 10
    $name: Font size

  - fontWeight: semiBold
    $name: Font weight
    $options:
    - normal: Normal
    - semiBold: Semi-bold
    - bold: Bold
  $name: Number text
  $description: These settings apply only when Display style is set to Number badge.

- Behavior:
  - minimumCount: 2
    $name: Show from window count
    $description: Default 2 means one window has no badge or dots.

  - maximumNumber: 99
    $name: Maximum number
    $description: Number badge only. Higher counts are shown with a plus sign, for example 99+.
  $name: Behavior
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winstring.h>
#include <windhawk_utils.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/base.h>

#include <algorithm>
#include <atomic>
#include <cwctype>
#include <cstdint>
#include <limits>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------

enum class DisplayStyle {
    Number,
    Dots,
};

enum class BadgeShape {
    Circle,
    Rounded,
    Square,
};

enum class BadgePosition {
    TopLeft,
    TopCenter,
    TopRight,
    BottomLeft,
    BottomCenter,
    BottomRight,
};

enum class DotPosition {
    Left,
    Right,
};

enum class BadgeFontWeight {
    Normal,
    SemiBold,
    Bold,
};

struct Settings {
    DisplayStyle displayStyle =
        DisplayStyle::Number;

    BadgeShape badgeShape =
        BadgeShape::Circle;

    BadgePosition badgePosition =
        BadgePosition::TopRight;

    int badgeOffsetX = -2;
    int badgeOffsetY = -2;

    int badgeSize = 16;

    std::wstring badgeBackgroundColor =
        L"#D90000";

    std::wstring badgeBorderColor =
        L"#FFFFFF";

    int badgeBorderThickness = 0;

    std::wstring textColor =
        L"#FFFFFF";

    std::wstring fontFamily =
        L"Segoe UI";

    int fontSize = 10;

    BadgeFontWeight fontWeight =
        BadgeFontWeight::SemiBold;

    DotPosition dotPosition =
        DotPosition::Right;

    int dotSize = 4;

    std::wstring dotColor =
        L"#FFFFFF";

    int minimumCount = 2;
    int maximumNumber = 99;
};

Settings g_settings;

// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------

std::atomic<bool> g_taskbarViewHooked = false;
std::atomic<bool> g_unloading = false;
std::atomic<bool> g_recountQueued = false;
std::mutex g_queueMutex;

[[clang::no_destroy]] winrt::Windows::UI::Core::CoreDispatcher
    g_taskbarDispatcher{nullptr};

bool g_countsInitialized = false;

struct TrackedButton {
    void* identity;
    std::wstring automationId;
    winrt::weak_ref<FrameworkElement> element;
};

std::vector<TrackedButton> g_trackedButtons;

// Always contains REAL HWND-derived counts.
// ViewModelCount is only used as a change notification.
std::unordered_map<std::wstring, unsigned int>
    g_appCounts;

// -----------------------------------------------------------------------------
// taskbar.dll symbols used to access existing XamlRoot
// -----------------------------------------------------------------------------

void* g_CTaskBand_ITaskListWndSite_vftable = nullptr;

using CTaskBand_GetTaskbarHost_t =
    void*(__cdecl*)(
        void* pThis,
        void** result);

CTaskBand_GetTaskbarHost_t
    g_CTaskBand_GetTaskbarHost = nullptr;

void* g_TaskbarHost_FrameHeight = nullptr;

using RefCount_Decref_t =
    void(__cdecl*)(
        void* pThis);

RefCount_Decref_t
    g_RefCount_Decref = nullptr;

// -----------------------------------------------------------------------------
// COM compatibility
// -----------------------------------------------------------------------------

#if __cplusplus < 202302L
DECLARE_HANDLE(CO_MTA_USAGE_COOKIE);

WINOLEAPI CoIncrementMTAUsage(
    CO_MTA_USAGE_COOKIE* cookie);

WINOLEAPI CoDecrementMTAUsage(
    CO_MTA_USAGE_COOKIE cookie);
#endif

// -----------------------------------------------------------------------------
// Setting helpers
// -----------------------------------------------------------------------------

std::wstring ReadStringSetting(
    PCWSTR name) {
    PCWSTR value =
        Wh_GetStringSetting(name);

    std::wstring result =
        value ? value : L"";

    if (value) {
        Wh_FreeStringSetting(value);
    }

    return result;
}

void LoadSettings() {
    // Display ---------------------------------------------------------------

    std::wstring displayStyle =
        ReadStringSetting(
            L"Display.style");

    g_settings.displayStyle =
        displayStyle == L"dots"
            ? DisplayStyle::Dots
            : DisplayStyle::Number;

    // Badge -----------------------------------------------------------------

    std::wstring shape =
        ReadStringSetting(
            L"Badge.shape");

    if (shape == L"square") {
        g_settings.badgeShape =
            BadgeShape::Square;
    } else if (shape == L"rounded") {
        g_settings.badgeShape =
            BadgeShape::Rounded;
    } else {
        g_settings.badgeShape =
            BadgeShape::Circle;
    }

    std::wstring position =
        ReadStringSetting(
            L"Badge.position");

    if (position == L"topLeft") {
        g_settings.badgePosition =
            BadgePosition::TopLeft;
    } else if (position == L"topCenter") {
        g_settings.badgePosition =
            BadgePosition::TopCenter;
    } else if (position == L"bottomLeft") {
        g_settings.badgePosition =
            BadgePosition::BottomLeft;
    } else if (position == L"bottomCenter") {
        g_settings.badgePosition =
            BadgePosition::BottomCenter;
    } else if (position == L"bottomRight") {
        g_settings.badgePosition =
            BadgePosition::BottomRight;
    } else {
        g_settings.badgePosition =
            BadgePosition::TopRight;
    }

    g_settings.badgeOffsetX =
        Wh_GetIntSetting(
            L"Badge.offsetX");

    g_settings.badgeOffsetY =
        Wh_GetIntSetting(
            L"Badge.offsetY");

    g_settings.badgeSize =
        std::max(
            4,
            Wh_GetIntSetting(
                L"Badge.size"));


    g_settings.badgeBackgroundColor =
        ReadStringSetting(
            L"Badge.backgroundColor");

    g_settings.badgeBorderColor =
        ReadStringSetting(
            L"Badge.borderColor");

    g_settings.badgeBorderThickness =
        std::max(
            0,
            Wh_GetIntSetting(
                L"Badge.borderThickness"));

    // Text ------------------------------------------------------------------

    g_settings.textColor =
        ReadStringSetting(
            L"Text.color");

    g_settings.fontFamily =
        ReadStringSetting(
            L"Text.fontFamily");

    if (g_settings.fontFamily.empty()) {
        g_settings.fontFamily =
            L"Segoe UI";
    }

    g_settings.fontSize =
        std::max(
            1,
            Wh_GetIntSetting(
                L"Text.fontSize"));

    std::wstring fontWeight =
        ReadStringSetting(
            L"Text.fontWeight");

    if (fontWeight == L"bold") {
        g_settings.fontWeight =
            BadgeFontWeight::Bold;
    } else if (fontWeight == L"normal") {
        g_settings.fontWeight =
            BadgeFontWeight::Normal;
    } else {
        g_settings.fontWeight =
            BadgeFontWeight::SemiBold;
    }

    // Vertical dots ---------------------------------------------------------

    std::wstring dotPosition =
        ReadStringSetting(
            L"VerticalDots.position");

    g_settings.dotPosition =
        dotPosition == L"left"
            ? DotPosition::Left
            : DotPosition::Right;

    g_settings.dotSize =
        std::max(
            2,
            Wh_GetIntSetting(
                L"VerticalDots.size"));

    g_settings.dotColor =
        ReadStringSetting(
            L"VerticalDots.color");

    // Behavior --------------------------------------------------------------

    g_settings.minimumCount =
        std::max(
            1,
            Wh_GetIntSetting(
                L"Behavior.minimumCount"));

    g_settings.maximumNumber =
        std::max(
            1,
            Wh_GetIntSetting(
                L"Behavior.maximumNumber"));
}

// -----------------------------------------------------------------------------
// Generic helpers
// -----------------------------------------------------------------------------

std::wstring NormalizeId(
    std::wstring value) {
    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](wchar_t c) {
            return static_cast<wchar_t>(
                std::towlower(c));
        });

    return value;
}

// -----------------------------------------------------------------------------
// Color parsing
// -----------------------------------------------------------------------------

bool ParseHexByte(
    wchar_t high,
    wchar_t low,
    BYTE* result) {
    auto getValue =
        [](wchar_t c) -> int {
            if (c >= L'0' &&
                c <= L'9') {
                return c - L'0';
            }

            c =
                static_cast<wchar_t>(
                    std::towupper(c));

            if (c >= L'A' &&
                c <= L'F') {
                return c - L'A' + 10;
            }

            return -1;
        };

    int highValue =
        getValue(high);

    int lowValue =
        getValue(low);

    if (highValue < 0 ||
        lowValue < 0) {
        return false;
    }

    *result =
        static_cast<BYTE>(
            highValue * 16 +
            lowValue);

    return true;
}

winrt::Windows::UI::Color ParseColor(
    const std::wstring& value,
    winrt::Windows::UI::Color fallback) {
    std::wstring text =
        value;

    if (!text.empty() &&
        text[0] == L'#') {
        text.erase(
            text.begin());
    }

    BYTE a = 255;
    BYTE r = 0;
    BYTE g = 0;
    BYTE b = 0;

    if (text.length() == 6) {
        if (!ParseHexByte(
                text[0],
                text[1],
                &r) ||
            !ParseHexByte(
                text[2],
                text[3],
                &g) ||
            !ParseHexByte(
                text[4],
                text[5],
                &b)) {
            return fallback;
        }
    } else if (text.length() == 8) {
        if (!ParseHexByte(
                text[0],
                text[1],
                &a) ||
            !ParseHexByte(
                text[2],
                text[3],
                &r) ||
            !ParseHexByte(
                text[4],
                text[5],
                &g) ||
            !ParseHexByte(
                text[6],
                text[7],
                &b)) {
            return fallback;
        }
    } else {
        return fallback;
    }

    winrt::Windows::UI::Color color{};

    color.A = a;
    color.R = r;
    color.G = g;
    color.B = b;

    return color;
}

Media::SolidColorBrush CreateBrush(
    const std::wstring& value,
    winrt::Windows::UI::Color fallback) {
    return Media::SolidColorBrush(
        ParseColor(
            value,
            fallback));
}

// -----------------------------------------------------------------------------
// Position helpers
// -----------------------------------------------------------------------------

void ApplyNumberBadgePosition(
    Controls::Border badge) {
    HorizontalAlignment horizontal =
        HorizontalAlignment::Right;

    VerticalAlignment vertical =
        VerticalAlignment::Top;

    switch (g_settings.badgePosition) {
        case BadgePosition::TopLeft:
            horizontal = HorizontalAlignment::Left;
            vertical = VerticalAlignment::Top;
            break;

        case BadgePosition::TopCenter:
            horizontal = HorizontalAlignment::Center;
            vertical = VerticalAlignment::Top;
            break;

        case BadgePosition::TopRight:
            horizontal = HorizontalAlignment::Right;
            vertical = VerticalAlignment::Top;
            break;

        case BadgePosition::BottomLeft:
            horizontal = HorizontalAlignment::Left;
            vertical = VerticalAlignment::Bottom;
            break;

        case BadgePosition::BottomCenter:
            horizontal = HorizontalAlignment::Center;
            vertical = VerticalAlignment::Bottom;
            break;

        case BadgePosition::BottomRight:
            horizontal = HorizontalAlignment::Right;
            vertical = VerticalAlignment::Bottom;
            break;
    }

    badge.HorizontalAlignment(horizontal);
    badge.VerticalAlignment(vertical);
    badge.Margin(Thickness{});

    auto transform =
        badge.RenderTransform()
            .try_as<Media::TranslateTransform>();

    if (!transform) {
        transform =
            Media::TranslateTransform();

        badge.RenderTransform(transform);
    }

    transform.X(
        static_cast<double>(
            g_settings.badgeOffsetX));

    transform.Y(
        static_cast<double>(
            g_settings.badgeOffsetY));
}

void ApplyDotPosition(
    Controls::Border badge) {
    badge.VerticalAlignment(
        VerticalAlignment::Center);

    badge.Margin(Thickness{});

    if (g_settings.dotPosition ==
        DotPosition::Left) {
        badge.HorizontalAlignment(
            HorizontalAlignment::Left);
    } else {
        badge.HorizontalAlignment(
            HorizontalAlignment::Right);
    }

    auto transform =
        badge.RenderTransform()
            .try_as<Media::TranslateTransform>();

    if (!transform) {
        transform =
            Media::TranslateTransform();

        badge.RenderTransform(transform);
    }

    // IMPORTANT:
    // Keep dots inside IconPanel.
    // Moving them outside causes the taskbar XAML to clip them.
    //
    // 2 px gives a little separation from the panel edge.
    transform.X(
        g_settings.dotPosition ==
                DotPosition::Left
            ? 2.0
            : -2.0);

    transform.Y(0.0);
}

// -----------------------------------------------------------------------------
// Font helper
// -----------------------------------------------------------------------------

winrt::Windows::UI::Text::FontWeight
GetConfiguredFontWeight() {
    uint16_t weight = 600;

    switch (g_settings.fontWeight) {
        case BadgeFontWeight::Normal:
            weight = 400;
            break;

        case BadgeFontWeight::SemiBold:
            weight = 600;
            break;

        case BadgeFontWeight::Bold:
            weight = 700;
            break;
    }

    winrt::Windows::UI::Text::FontWeight result{};

    result.Weight =
        weight;

    return result;
}

// -----------------------------------------------------------------------------
// Main taskbar HWND
// -----------------------------------------------------------------------------

HWND FindCurrentProcessTaskbarWnd() {
    HWND result = nullptr;

    EnumWindows(
        [](HWND hWnd,
           LPARAM param) -> BOOL {
            DWORD pid = 0;
            WCHAR className[64] = {};

            GetWindowThreadProcessId(
                hWnd,
                &pid);

            if (pid !=
                GetCurrentProcessId()) {
                return TRUE;
            }

            if (!GetClassNameW(
                    hWnd,
                    className,
                    ARRAYSIZE(
                        className))) {
                return TRUE;
            }

            if (_wcsicmp(
                    className,
                    L"Shell_TrayWnd") !=
                0) {
                return TRUE;
            }

            *reinterpret_cast<HWND*>(
                param) =
                hWnd;

            return FALSE;
        },
        reinterpret_cast<LPARAM>(
            &result));

    return result;
}

// -----------------------------------------------------------------------------
// Taskbar.View module
// -----------------------------------------------------------------------------

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module =
        GetModuleHandleW(
            L"Taskbar.View.dll");

    if (!module) {
        module =
            GetModuleHandleW(
                L"ExplorerExtensions.dll");
    }

    return module;
}

// -----------------------------------------------------------------------------
// XAML helpers
// -----------------------------------------------------------------------------

FrameworkElement FindChildByName(
    FrameworkElement element,
    PCWSTR name) {
    int count =
        Media::VisualTreeHelper::
            GetChildrenCount(
                element);

    for (int i = 0;
         i < count;
         i++) {
        auto child =
            Media::VisualTreeHelper::
                GetChild(
                    element,
                    i)
                    .try_as<
                        FrameworkElement>();

        if (!child) {
            continue;
        }

        if (child.Name() ==
            name) {
            return child;
        }

        auto result =
            FindChildByName(
                child,
                name);

        if (result) {
            return result;
        }
    }

    return nullptr;
}

// -----------------------------------------------------------------------------
// Badge text / dot content
// -----------------------------------------------------------------------------

std::wstring MakeNumberText(
    unsigned int count) {
    if (count >
        static_cast<unsigned int>(
            g_settings.maximumNumber)) {
        return std::to_wstring(
                   g_settings.maximumNumber) +
               L"+";
    }

    return std::to_wstring(
        count);
}

unsigned int GetVisibleDotCount(
    unsigned int count) {
    // Five dots means five or more windows.
    return std::min<unsigned int>(
        count,
        5);
}

void RebuildDots(
    Controls::StackPanel dotStack,
    unsigned int count) {
    dotStack.Children().Clear();

    winrt::Windows::UI::Color
        defaultDotColor{};

    defaultDotColor.A = 255;
    defaultDotColor.R = 255;
    defaultDotColor.G = 255;
    defaultDotColor.B = 255;

    auto brush =
        CreateBrush(
            g_settings.dotColor,
            defaultDotColor);

    unsigned int dotCount =
        GetVisibleDotCount(
            count);

    constexpr double kDotSpacing = 2.0;

    for (unsigned int i = 0;
         i < dotCount;
         i++) {
        Controls::Border dot;

        dot.Width(
            static_cast<double>(
                g_settings.dotSize));

        dot.Height(
            static_cast<double>(
                g_settings.dotSize));

        double radius =
            static_cast<double>(
                g_settings.dotSize) /
            2.0;

        dot.CornerRadius(
            CornerRadius{
                radius,
                radius,
                radius,
                radius});

        dot.Background(
            brush);

        double topMargin =
            i == 0
                ? 0.0
                : kDotSpacing;

        dot.Margin(
            Thickness{
                0,
                topMargin,
                0,
                0});

        dot.IsHitTestVisible(
            false);

        dotStack.Children().Append(
            dot);
    }
}

// -----------------------------------------------------------------------------
// Apply visual style
// -----------------------------------------------------------------------------

void ApplyBadgeVisualStyle(
    Controls::Border badge,
    unsigned int count) {
    auto circleVisual =
        FindChildByName(
            badge,
            L"WindhawkCircleVisual")
            .try_as<
                Shapes::Ellipse>();

    auto badgeVisual =
        FindChildByName(
            badge,
            L"WindhawkBadgeVisual")
            .try_as<
                Controls::Border>();

    auto text =
        FindChildByName(
            badge,
            L"WindhawkCountText")
            .try_as<
                Controls::TextBlock>();

    auto dotStack =
        FindChildByName(
            badge,
            L"WindhawkDotStack")
            .try_as<
                Controls::StackPanel>();

    if (!circleVisual ||
        !badgeVisual ||
        !text ||
        !dotStack) {
        return;
    }

    winrt::Windows::UI::Color
        defaultBackground{};

    defaultBackground.A = 255;
    defaultBackground.R = 217;
    defaultBackground.G = 0;
    defaultBackground.B = 0;

    winrt::Windows::UI::Color
        defaultText{};

    defaultText.A = 255;
    defaultText.R = 255;
    defaultText.G = 255;
    defaultText.B = 255;

    winrt::Windows::UI::Color
        defaultBorder{};

    defaultBorder.A = 255;
    defaultBorder.R = 255;
    defaultBorder.G = 255;
    defaultBorder.B = 255;

    badge.IsHitTestVisible(false);

    Controls::Canvas::SetZIndex(
        badge,
        100);

    // ---------------------------------------------------------------------
    // NUMBER BADGE
    // ---------------------------------------------------------------------

    if (g_settings.displayStyle ==
        DisplayStyle::Number) {
        ApplyNumberBadgePosition(
            badge);

        dotStack.Visibility(
            Visibility::Collapsed);

        text.Visibility(
            Visibility::Visible);

        double size =
            static_cast<double>(
                g_settings.badgeSize);

        badge.Width(size);
        badge.Height(size);

        auto backgroundBrush =
            CreateBrush(
                g_settings.badgeBackgroundColor,
                defaultBackground);

        auto borderBrush =
            CreateBrush(
                g_settings.badgeBorderColor,
                defaultBorder);

        double borderThickness =
            static_cast<double>(
                g_settings.badgeBorderThickness);

        if (g_settings.badgeShape ==
            BadgeShape::Circle) {
            badgeVisual.Visibility(
                Visibility::Collapsed);

            circleVisual.Visibility(
                Visibility::Visible);

            circleVisual.Width(size);
            circleVisual.Height(size);

            circleVisual.Fill(
                backgroundBrush);

            if (g_settings.badgeBorderThickness > 0) {
                circleVisual.Stroke(
                    borderBrush);

                circleVisual.StrokeThickness(
                    borderThickness);
            } else {
                circleVisual.Stroke(
                    nullptr);

                circleVisual.StrokeThickness(
                    0);
            }
        } else {
            circleVisual.Visibility(
                Visibility::Collapsed);

            badgeVisual.Visibility(
                Visibility::Visible);

            badgeVisual.Width(size);
            badgeVisual.Height(size);

            badgeVisual.HorizontalAlignment(
                HorizontalAlignment::Center);

            badgeVisual.VerticalAlignment(
                VerticalAlignment::Center);

            double cornerRadius =
                g_settings.badgeShape ==
                        BadgeShape::Rounded
                    ? size / 4.0
                    : 0.0;

            badgeVisual.CornerRadius(
                CornerRadius{
                    cornerRadius,
                    cornerRadius,
                    cornerRadius,
                    cornerRadius});

            badgeVisual.Background(
                backgroundBrush);

            if (g_settings.badgeBorderThickness > 0) {
                badgeVisual.BorderBrush(
                    borderBrush);

                badgeVisual.BorderThickness(
                    Thickness{
                        borderThickness,
                        borderThickness,
                        borderThickness,
                        borderThickness});
            } else {
                badgeVisual.BorderBrush(
                    nullptr);

                badgeVisual.BorderThickness(
                    Thickness{
                        0,
                        0,
                        0,
                        0});
            }
        }

        text.Foreground(
            CreateBrush(
                g_settings.textColor,
                defaultText));

        text.FontSize(
            static_cast<double>(
                g_settings.fontSize));

        text.FontFamily(
            Media::FontFamily(
                winrt::hstring(
                    g_settings.fontFamily)));

        text.FontWeight(
            GetConfiguredFontWeight());

        text.HorizontalAlignment(
            HorizontalAlignment::Center);

        text.VerticalAlignment(
            VerticalAlignment::Center);

        text.TextAlignment(
            TextAlignment::Center);

        // Optical correction for the font baseline.
        text.Margin(
            Thickness{
                0,
                -2,
                0,
                0});

        text.Text(
            MakeNumberText(
                count));

        return;
    }

    // ---------------------------------------------------------------------
    // VERTICAL DOTS
    // ---------------------------------------------------------------------

    ApplyDotPosition(
        badge);

    circleVisual.Visibility(
        Visibility::Collapsed);

    badgeVisual.Visibility(
        Visibility::Collapsed);

    text.Visibility(
        Visibility::Collapsed);

    dotStack.Visibility(
        Visibility::Visible);

    badge.Width(
        std::numeric_limits<double>::
            quiet_NaN());

    badge.Height(
        std::numeric_limits<double>::
            quiet_NaN());

    badge.Background(
        nullptr);

    badge.BorderBrush(
        nullptr);

    badge.BorderThickness(
        Thickness{
            0,
            0,
            0,
            0});

    dotStack.Orientation(
        Controls::Orientation::Vertical);

    dotStack.HorizontalAlignment(
        HorizontalAlignment::Center);

    dotStack.VerticalAlignment(
        VerticalAlignment::Center);

    RebuildDots(
        dotStack,
        count);
}
// -----------------------------------------------------------------------------
// Badge
// -----------------------------------------------------------------------------

void RemoveBadgeFromPanel(
    Controls::Panel iconPanel,
    Controls::Border badge) {
    auto children =
        iconPanel.Children();

    for (uint32_t i = 0;
         i < children.Size();
         i++) {
        if (children.GetAt(i) ==
            badge) {
            children.RemoveAt(i);
            return;
        }
    }
}

Controls::Border CreateCountBadge(
    Controls::Panel iconPanel) {
    PCWSTR xaml =
        LR"(
<Border
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
    Name="WindhawkCountBadge"
    Background="Transparent"
    HorizontalAlignment="Right"
    VerticalAlignment="Top">

    <Grid>

        <Ellipse
            Name="WindhawkCircleVisual"
            HorizontalAlignment="Center"
            VerticalAlignment="Center"
            Visibility="Collapsed"/>

        <Border
            Name="WindhawkBadgeVisual"
            HorizontalAlignment="Center"
            VerticalAlignment="Center"
            Visibility="Collapsed"/>

        <TextBlock
            Name="WindhawkCountText"
            HorizontalAlignment="Center"
            VerticalAlignment="Center"
            TextAlignment="Center"/>

        <StackPanel
            Name="WindhawkDotStack"
            Orientation="Vertical"
            HorizontalAlignment="Center"
            VerticalAlignment="Center"
            Visibility="Collapsed"/>

    </Grid>

</Border>
)";

    auto badge =
        Markup::XamlReader::
            Load(xaml)
            .as<
                Controls::Border>();

    badge.IsHitTestVisible(
        false);

    Controls::Canvas::SetZIndex(
        badge,
        100);

    iconPanel.Children().Append(
        badge);

    return badge;
}

void UpdateCountBadge(
    FrameworkElement taskListButton,
    unsigned int count) {
    if (g_unloading) {
        return;
    }

    auto iconPanelElement =
        FindChildByName(
            taskListButton,
            L"IconPanel");

    if (!iconPanelElement) {
        return;
    }

    auto iconPanel =
        iconPanelElement
            .try_as<
                Controls::Panel>();

    if (!iconPanel) {
        return;
    }

    auto badge =
        FindChildByName(
            iconPanelElement,
            L"WindhawkCountBadge")
            .try_as<
                Controls::Border>();

    // Rebuild badges created by older visual-layout versions.
    if (badge) {
        auto circleVisual =
            FindChildByName(
                badge,
                L"WindhawkCircleVisual");

        auto badgeVisual =
            FindChildByName(
                badge,
                L"WindhawkBadgeVisual");

        auto countText =
            FindChildByName(
                badge,
                L"WindhawkCountText");

        auto dotStack =
            FindChildByName(
                badge,
                L"WindhawkDotStack");

        if (!circleVisual ||
            !badgeVisual ||
            !countText ||
            !dotStack) {
            RemoveBadgeFromPanel(
                iconPanel,
                badge);

            badge = nullptr;
        }
    }

    if (count <
        static_cast<unsigned int>(
            g_settings.minimumCount)) {
        if (badge) {
            badge.Visibility(
                Visibility::Collapsed);
        }

        return;
    }

    if (!badge) {
        badge =
            CreateCountBadge(
                iconPanel);
    }

    ApplyBadgeVisualStyle(
        badge,
        count);

    badge.Visibility(
        Visibility::Visible);
}
// -----------------------------------------------------------------------------
// Tracked buttons
// -----------------------------------------------------------------------------

void PruneDeadTrackedButtons() {
    for (auto it =
             g_trackedButtons.begin();
         it !=
         g_trackedButtons.end();) {
        if (!it->element.get()) {
            it =
                g_trackedButtons.erase(
                    it);
        } else {
            ++it;
        }
    }
}

void TrackTaskbarButton(
    FrameworkElement element) {
    PruneDeadTrackedButtons();
    auto unknown =
        element.as<
            winrt::Windows::
                Foundation::IUnknown>();

    void* identity =
        winrt::get_abi(
            unknown);

    auto automationId =
        winrt::Windows::UI::
            Xaml::Automation::
                AutomationProperties::
                    GetAutomationId(
                        element);

    if (automationId.empty()) {
        return;
    }

    auto existing =
        std::find_if(
            g_trackedButtons.begin(),
            g_trackedButtons.end(),
            [identity](
                const TrackedButton& item) {
                return item.identity ==
                       identity;
            });

    if (existing !=
        g_trackedButtons.end()) {
        existing->automationId =
            automationId.c_str();

        return;
    }

    g_trackedButtons.push_back(
        {
            identity,
            automationId.c_str(),
            winrt::make_weak(
                element),
        });
}

// -----------------------------------------------------------------------------
// Apply cached HWND count
// -----------------------------------------------------------------------------

void ApplyCountToButton(
    FrameworkElement element) {
    auto automationId =
        winrt::Windows::UI::
            Xaml::Automation::
                AutomationProperties::
                    GetAutomationId(
                        element);

    if (automationId.empty()) {
        return;
    }

    std::wstring key =
        NormalizeId(
            automationId.c_str());

    unsigned int count = 0;

    auto it =
        g_appCounts.find(
            key);

    if (it !=
        g_appCounts.end()) {
        count =
            it->second;
    }

    UpdateCountBadge(
        element,
        count);
}

void RefreshAllTrackedButtons() {
    PruneDeadTrackedButtons();

    for (auto& item :
         g_trackedButtons) {
        auto element =
            item.element.get();

        if (!element) {
            continue;
        }

        ApplyCountToButton(
            element);
    }
}

void RefreshChangedTrackedButtons(
    const std::unordered_set<
        std::wstring>& changedIds) {
    PruneDeadTrackedButtons();

    for (auto& item :
         g_trackedButtons) {
        std::wstring key =
            NormalizeId(
                item.automationId);

        if (!changedIds.contains(
                key)) {
            continue;
        }

        auto element =
            item.element.get();

        if (!element) {
            continue;
        }

        ApplyCountToButton(
            element);
    }
}

// -----------------------------------------------------------------------------
// Existing XAML tree enumeration
// -----------------------------------------------------------------------------

int EnumerateExistingTaskbarButtons(
    FrameworkElement element) {
    if (!element) {
        return 0;
    }

    int found = 0;

    try {
        if (element.Name() ==
            L"TaskListButton") {
            if (!g_taskbarDispatcher) {
                g_taskbarDispatcher =
                    element.Dispatcher();
            }

            TrackTaskbarButton(
                element);

            ApplyCountToButton(
                element);

            found++;
        }

        int children =
            Media::VisualTreeHelper::
                GetChildrenCount(
                    element);

        for (int i = 0;
             i < children;
             i++) {
            auto child =
                Media::VisualTreeHelper::
                    GetChild(
                        element,
                        i)
                    .try_as<
                        FrameworkElement>();

            if (!child) {
                continue;
            }

            found +=
                EnumerateExistingTaskbarButtons(
                    child);
        }
    } catch (...) {
    }

    return found;
}

// -----------------------------------------------------------------------------
// Existing taskbar XamlRoot
// -----------------------------------------------------------------------------

XamlRoot GetTaskbarXamlRoot(
    HWND taskbarWnd) {
    if (!taskbarWnd ||
        !g_CTaskBand_ITaskListWndSite_vftable ||
        !g_CTaskBand_GetTaskbarHost ||
        !g_TaskbarHost_FrameHeight ||
        !g_RefCount_Decref) {
        return nullptr;
    }

    HWND taskBandWnd =
        reinterpret_cast<HWND>(
            GetPropW(
                taskbarWnd,
                L"TaskbandHWND"));

    if (!taskBandWnd) {
        Wh_Log(
            L"Taskbar Count Badges: "
            L"TaskbandHWND not found");

        return nullptr;
    }

    void* taskBand =
        reinterpret_cast<void*>(
            GetWindowLongPtrW(
                taskBandWnd,
                0));

    if (!taskBand) {
        return nullptr;
    }

    void* site =
        taskBand;

    bool siteFound =
        false;

    for (int i = 0;
         i < 20;
         i++) {
        if (*reinterpret_cast<void**>(
                site) ==
            g_CTaskBand_ITaskListWndSite_vftable) {
            siteFound =
                true;

            break;
        }

        site =
            reinterpret_cast<void**>(
                site) +
            1;
    }

    if (!siteFound) {
        Wh_Log(
            L"Taskbar Count Badges: "
            L"ITaskListWndSite not found");

        return nullptr;
    }

    void* hostShared[2] = {};

    g_CTaskBand_GetTaskbarHost(
        site,
        hostShared);

    if (!hostShared[0]) {
        if (hostShared[1]) {
            g_RefCount_Decref(
                hostShared[1]);
        }

        return nullptr;
    }

    // Current Windows 11 TaskbarHost layout uses 0x48 on ARM64.
    // On x64, detect the offset from TaskbarHost::FrameHeight so the mod can
    // tolerate layout changes where possible.
    size_t elementOffset =
        0x48;

#if defined(_M_X64)

    const BYTE* code =
        reinterpret_cast<
            const BYTE*>(
            g_TaskbarHost_FrameHeight);

    if (code[0] == 0x48 &&
        code[1] == 0x83 &&
        code[2] == 0xEC &&
        code[4] == 0x48 &&
        code[5] == 0x83 &&
        code[6] == 0xC1 &&
        code[7] <= 0x7F) {
        elementOffset =
            code[7];
    } else {
        Wh_Log(
            L"Taskbar Count Badges: "
            L"couldn't detect TaskbarHost "
            L"XAML offset, using 0x48");
    }

#elif defined(_M_ARM64)

    // Use the known/default TaskbarHost XAML element offset.
    // This follows the ARM64 fallback used by current Windhawk taskbar mods.

#else

#error "Unsupported architecture"

#endif

    IUnknown* xamlUnknown =
        *reinterpret_cast<IUnknown**>(
            reinterpret_cast<BYTE*>(
                hostShared[0]) +
            elementOffset);

    FrameworkElement taskbarElement =
        nullptr;

    if (xamlUnknown) {
        xamlUnknown->QueryInterface(
            winrt::guid_of<
                FrameworkElement>(),
            winrt::put_abi(
                taskbarElement));
    }

    XamlRoot result =
        taskbarElement
            ? taskbarElement.XamlRoot()
            : nullptr;

    if (hostShared[1]) {
        g_RefCount_Decref(
            hostShared[1]);
    }

    return result;
}

// -----------------------------------------------------------------------------
// Run synchronously on taskbar UI thread
// -----------------------------------------------------------------------------

using TaskbarThreadProc =
    void(WINAPI*)(
        void* parameter);

bool RunOnTaskbarThread(
    HWND hWnd,
    TaskbarThreadProc proc,
    void* parameter) {
    if (!hWnd ||
        !proc) {
        return false;
    }

    DWORD threadId =
        GetWindowThreadProcessId(
            hWnd,
            nullptr);

    if (!threadId) {
        return false;
    }

    if (threadId ==
        GetCurrentThreadId()) {
        proc(
            parameter);

        return true;
    }

    static UINT message =
        RegisterWindowMessageW(
            L"Windhawk_TaskbarCountBadges_"
            L"RunOnTaskbarThread");

    if (!message) {
        return false;
    }

    struct Request {
        TaskbarThreadProc proc;
        void* parameter;
    };

    HHOOK hook =
        SetWindowsHookExW(
            WH_CALLWNDPROC,
            [](int code,
               WPARAM wParam,
               LPARAM lParam) -> LRESULT {
                if (code ==
                    HC_ACTION) {
                    auto data =
                        reinterpret_cast<
                            CWPSTRUCT*>(
                            lParam);

                    if (data->message ==
                        message) {
                        auto request =
                            reinterpret_cast<
                                Request*>(
                                data->lParam);

                        if (request &&
                            request->proc) {
                            request->proc(
                                request->parameter);
                        }
                    }
                }

                return CallNextHookEx(
                    nullptr,
                    code,
                    wParam,
                    lParam);
            },
            nullptr,
            threadId);

    if (!hook) {
        return false;
    }

    Request request{
        proc,
        parameter,
    };

    SendMessageW(
        hWnd,
        message,
        0,
        reinterpret_cast<LPARAM>(
            &request));

    UnhookWindowsHookEx(
        hook);

    return true;
}

// -----------------------------------------------------------------------------
// App resolver
// -----------------------------------------------------------------------------

constexpr winrt::guid
    CLSID_StartMenuCacheAndAppResolver{
        0x660B90C8,
        0x73A9,
        0x4B58,
        {
            0x8C,
            0xAE,
            0x35,
            0x5B,
            0x7F,
            0x55,
            0x34,
            0x1B,
        }};

constexpr winrt::guid
    IID_IAppResolver_8{
        0xDE25675A,
        0x72DE,
        0x44B4,
        {
            0x93,
            0x73,
            0x05,
            0x17,
            0x04,
            0x50,
            0xC1,
            0x40,
        }};

struct IAppResolver_8 :
    public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE
        GetAppIDForShortcut() = 0;

    virtual HRESULT STDMETHODCALLTYPE
        GetAppIDForShortcutObject() = 0;

    virtual HRESULT STDMETHODCALLTYPE
        GetAppIDForWindow(
            HWND hWnd,
            WCHAR** appId,
            void* unknown1,
            void* unknown2,
            void* unknown3) = 0;

    virtual HRESULT STDMETHODCALLTYPE
        GetAppIDForProcess(
            DWORD processId,
            WCHAR** appId,
            void* unknown1,
            void* unknown2,
            void* unknown3) = 0;
};

IAppResolver_8* g_appResolver = nullptr;
CO_MTA_USAGE_COOKIE g_appResolverMtaCookie{};
ULONGLONG g_appResolverRetryAfter = 0;

// A window keeps the same app ID for its lifetime. Cache the normalized ID so
// steady-state recounts don't repeatedly call the shell resolver for every HWND.
std::unordered_map<HWND, std::wstring> g_appIdCache;

bool EnsureAppResolver() {
    if (g_appResolver) {
        return true;
    }

    ULONGLONG now =
        GetTickCount64();

    if (now <
        g_appResolverRetryAfter) {
        return false;
    }

    CO_MTA_USAGE_COOKIE cookie{};

    bool mta =
        SUCCEEDED(
            CoIncrementMTAUsage(
                &cookie));

    IAppResolver_8* resolver =
        nullptr;

    HRESULT hr =
        CoCreateInstance(
            CLSID_StartMenuCacheAndAppResolver,
            nullptr,
            CLSCTX_INPROC_SERVER |
                CLSCTX_INPROC_HANDLER,
            IID_IAppResolver_8,
            reinterpret_cast<void**>(
                &resolver));

    if (FAILED(
            hr) ||
        !resolver) {
        Wh_Log(
            L"Taskbar Count Badges: "
            L"AppResolver failed "
            L"hr=0x%08X",
            static_cast<unsigned int>(
                hr));

        if (mta) {
            CoDecrementMTAUsage(
                cookie);
        }

        // Avoid retrying COM activation on every TaskListButton state update.
        // A later recount can retry after this short backoff.
        g_appResolverRetryAfter =
            now + 5000;

        return false;
    }

    g_appResolverRetryAfter =
        0;

    g_appResolver =
        resolver;

    if (mta) {
        g_appResolverMtaCookie =
            cookie;
    }

    return true;
}

void ReleaseAppResolver() {
    if (g_appResolver) {
        g_appResolver->Release();
        g_appResolver =
            nullptr;
    }

    if (g_appResolverMtaCookie) {
        CoDecrementMTAUsage(
            g_appResolverMtaCookie);

        g_appResolverMtaCookie =
            nullptr;
    }

    g_appResolverRetryAfter =
        0;

    g_appIdCache.clear();
}

// -----------------------------------------------------------------------------
// Which HWNDs count
// -----------------------------------------------------------------------------

bool IsCountableWindow(
    HWND hWnd) {
    if (!hWnd ||
        !IsWindowVisible(
            hWnd)) {
        return false;
    }

    WCHAR className[128] = {};

    if (GetClassNameW(
            hWnd,
            className,
            ARRAYSIZE(className))) {

        // Shell/taskbar infrastructure windows.
        // These are not real application taskbar windows.
        if (_wcsicmp(
                className,
                L"Shell_TrayWnd") == 0 ||
            _wcsicmp(
                className,
                L"Shell_SecondaryTrayWnd") == 0 ||
            _wcsicmp(
                className,
                L"Progman") == 0 ||
            _wcsicmp(
                className,
                L"WorkerW") == 0 ||
            _wcsicmp(
                className,
                L"ApplicationManager_ImmersiveShellWindow") == 0) {
            return false;
        }
    }

    LONG_PTR exStyle =
        GetWindowLongPtrW(
            hWnd,
            GWL_EXSTYLE);

    if ((exStyle &
         WS_EX_TOOLWINDOW) &&
        !(exStyle &
          WS_EX_APPWINDOW)) {
        return false;
    }

    HWND owner =
        GetWindow(
            hWnd,
            GW_OWNER);

    if (owner &&
        !(exStyle &
          WS_EX_APPWINDOW)) {
        return false;
    }

    using DwmGetWindowAttribute_t =
        HRESULT(WINAPI*)(
            HWND,
            DWORD,
            PVOID,
            DWORD);

    static DwmGetWindowAttribute_t
        getDwmWindowAttribute = nullptr;

    if (!getDwmWindowAttribute) {
        HMODULE dwmApi =
            GetModuleHandleW(
                L"dwmapi.dll");

        if (dwmApi) {
            getDwmWindowAttribute =
                reinterpret_cast<
                    DwmGetWindowAttribute_t>(
                    GetProcAddress(
                        dwmApi,
                        "DwmGetWindowAttribute"));
        }
    }

    if (getDwmWindowAttribute) {
        DWORD cloaked = 0;

        // DWMWA_CLOAKED = 14
        if (SUCCEEDED(
                getDwmWindowAttribute(
                    hWnd,
                    14,
                    &cloaked,
                    sizeof(cloaked))) &&
            cloaked) {
            return false;
        }
    }

    return true;
}

// -----------------------------------------------------------------------------
// Build HWND/AppID count map
// -----------------------------------------------------------------------------

struct WindowEnumContext {
    IAppResolver_8* resolver;

    std::unordered_map<
        std::wstring,
        unsigned int>* counts;

    std::unordered_set<HWND>*
        seenWindows;
};

BOOL CALLBACK EnumCountableWindows(
    HWND hWnd,
    LPARAM param) {
    if (!IsCountableWindow(
            hWnd)) {
        return TRUE;
    }

    auto context =
        reinterpret_cast<
            WindowEnumContext*>(
            param);

    if (!context ||
        !context->resolver ||
        !context->counts ||
        !context->seenWindows) {
        return TRUE;
    }

    context->seenWindows->insert(
        hWnd);

    auto cached =
        g_appIdCache.find(
            hWnd);

    if (cached !=
        g_appIdCache.end()) {
        (*context->counts)[
            cached->second]++;

        return TRUE;
    }

    WCHAR* appId =
        nullptr;

    HRESULT hr =
        context->resolver
            ->GetAppIDForWindow(
                hWnd,
                &appId,
                nullptr,
                nullptr,
                nullptr);

    if (FAILED(
            hr) ||
        !appId ||
        !*appId) {
        if (appId) {
            CoTaskMemFree(
                appId);
        }

        return TRUE;
    }

    std::wstring key =
        NormalizeId(
            L"Appid: " +
            std::wstring(
                appId));

    CoTaskMemFree(
        appId);

    g_appIdCache.emplace(
        hWnd,
        key);

    (*context->counts)[
        key]++;

    return TRUE;
}

// -----------------------------------------------------------------------------
// Rebuild counts
// -----------------------------------------------------------------------------

bool BuildRealWindowCounts(
    std::unordered_set<
        std::wstring>* changedIds,
    bool initialBuild) {
    if (!EnsureAppResolver()) {
        return false;
    }

    std::unordered_map<
        std::wstring,
        unsigned int>
        newCounts;

    std::unordered_set<HWND>
        seenWindows;

    WindowEnumContext context{
        g_appResolver,
        &newCounts,
        &seenWindows,
    };

    EnumWindows(
        EnumCountableWindows,
        reinterpret_cast<LPARAM>(
            &context));

    for (auto it =
             g_appIdCache.begin();
         it !=
         g_appIdCache.end();) {
        if (!seenWindows.contains(
                it->first)) {
            it =
                g_appIdCache.erase(
                    it);
        } else {
            ++it;
        }
    }

    if (initialBuild) {
        g_appCounts =
            std::move(
                newCounts);

        g_countsInitialized =
            true;

        size_t multiWindowApps =
            0;

        for (const auto& pair :
             g_appCounts) {
            if (pair.second >=
                static_cast<unsigned int>(
                    g_settings.minimumCount)) {
                multiWindowApps++;
            }
        }

        Wh_Log(
            L"Taskbar Count Badges: "
            L"initial window counts ready "
            L"apps=%zu visible=%zu",
            g_appCounts.size(),
            multiWindowApps);

        return true;
    }

    if (!changedIds) {
        return false;
    }

    changedIds->clear();

    for (const auto& oldPair :
         g_appCounts) {
        auto newIt =
            newCounts.find(
                oldPair.first);

        unsigned int newCount =
            newIt !=
                    newCounts.end()
                ? newIt->second
                : 0;

        if (oldPair.second ==
            newCount) {
            continue;
        }

        changedIds->insert(
            oldPair.first);

        Wh_Log(
            L"Taskbar Count Badges: "
            L"count changed id=\"%s\" "
            L"%u -> %u",
            oldPair.first.c_str(),
            oldPair.second,
            newCount);
    }

    for (const auto& newPair :
         newCounts) {
        if (g_appCounts.find(
                newPair.first) !=
            g_appCounts.end()) {
            continue;
        }

        changedIds->insert(
            newPair.first);

        Wh_Log(
            L"Taskbar Count Badges: "
            L"count changed id=\"%s\" "
            L"0 -> %u",
            newPair.first.c_str(),
            newPair.second);
    }

    g_appCounts =
        std::move(
            newCounts);

    g_countsInitialized =
        true;

    return true;
}

// -----------------------------------------------------------------------------
// Existing taskbar initialization
// -----------------------------------------------------------------------------

void WINAPI
InitializeExistingButtonsOnTaskbarThread(
    void* parameter) {
    try {
        HWND taskbarWnd =
            reinterpret_cast<HWND>(
                parameter);

        auto xamlRoot =
            GetTaskbarXamlRoot(
                taskbarWnd);

        if (!xamlRoot) {
            Wh_Log(
                L"Taskbar Count Badges: "
                L"failed to get taskbar XamlRoot");

            return;
        }

        auto content =
            xamlRoot.Content()
                .try_as<
                    FrameworkElement>();

        if (!content) {
            Wh_Log(
                L"Taskbar Count Badges: "
                L"XamlRoot content unavailable");

            return;
        }

        if (!g_taskbarDispatcher) {
            g_taskbarDispatcher =
                content.Dispatcher();
        }

        // Keep the count map on the taskbar UI thread. Live recounts and badge
        // updates also run on this thread, avoiding concurrent map access during
        // initialization.
        BuildRealWindowCounts(
            nullptr,
            true);

        int buttonCount =
            EnumerateExistingTaskbarButtons(
                content);

        Wh_Log(
            L"Taskbar Count Badges: "
            L"initial taskbar buttons=%d",
            buttonCount);
    } catch (...) {
        Wh_Log(
            L"Taskbar Count Badges: "
            L"initial taskbar UI setup failed");
    }
}

// -----------------------------------------------------------------------------
// Recount queue
// -----------------------------------------------------------------------------

void QueueRealWindowRecount() {
    std::lock_guard<std::mutex> guard(
        g_queueMutex);

    if (g_unloading ||
        !g_taskbarDispatcher) {
        return;
    }

    if (g_recountQueued.exchange(
            true)) {
        return;
    }

    try {
        g_taskbarDispatcher.RunAsync(
            winrt::Windows::UI::Core::
                CoreDispatcherPriority::
                    Normal,
            []() {
                try {
                    if (!g_unloading) {
                        std::unordered_set<
                            std::wstring>
                            changedIds;

                        if (BuildRealWindowCounts(
                                &changedIds,
                                false) &&
                            !changedIds.empty()) {
                            RefreshChangedTrackedButtons(
                                changedIds);
                        }
                    }
                } catch (...) {
                    Wh_Log(
                        L"Taskbar Count Badges: "
                        L"HWND recount failed");
                }

                // Keep the flag set until all enumeration and UI refresh work
                // is complete so getter bursts collapse into one recount.
                g_recountQueued =
                    false;
            });
    } catch (...) {
        g_recountQueued =
            false;

        Wh_Log(
            L"Taskbar Count Badges: "
            L"failed to queue HWND recount");
    }
}

// -----------------------------------------------------------------------------
// TaskListButton::UpdateVisualStates
// -----------------------------------------------------------------------------

using TaskListButton_UpdateVisualStates_t =
    void(__cdecl*)(
        void* pThis);

TaskListButton_UpdateVisualStates_t
    TaskListButton_UpdateVisualStates_Original =
        nullptr;

void __cdecl
TaskListButton_UpdateVisualStates_Hook(
    void* pThis) {
    TaskListButton_UpdateVisualStates_Original(
        pThis);

    if (g_unloading) {
        return;
    }

    try {
        void* taskListButtonIUnknownPtr =
            reinterpret_cast<void**>(
                pThis) +
            3;

        winrt::Windows::
            Foundation::IUnknown
                taskListButtonIUnknown;

        winrt::copy_from_abi(
            taskListButtonIUnknown,
            taskListButtonIUnknownPtr);

        auto element =
            taskListButtonIUnknown
                .try_as<
                    FrameworkElement>();

        if (!element) {
            return;
        }

        if (!g_taskbarDispatcher) {
            g_taskbarDispatcher =
                element.Dispatcher();
        }

        if (!g_countsInitialized) {
            BuildRealWindowCounts(
                nullptr,
                true);
        }

        TrackTaskbarButton(
            element);

        ApplyCountToButton(
            element);
    } catch (...) {
        Wh_Log(
            L"Taskbar Count Badges: "
            L"TaskListButton update failed");
    }
}

// -----------------------------------------------------------------------------
// ViewModelCount
//
// This value is NOT used as the displayed count.
// It only tells us that taskbar group state changed.
// -----------------------------------------------------------------------------

using GetViewModelCount_t =
    HRESULT(WINAPI*)(
        void* pThis,
        unsigned int* count);

GetViewModelCount_t
    GetViewModelCount_Original =
        nullptr;

HRESULT WINAPI
GetViewModelCount_Hook(
    void* pThis,
    unsigned int* count) {
    HRESULT hr =
        GetViewModelCount_Original(
            pThis,
            count);

    if (FAILED(
            hr) ||
        !count ||
        g_unloading) {
        return hr;
    }

    QueueRealWindowRecount();

    return hr;
}

// -----------------------------------------------------------------------------
// Apply settings live
// -----------------------------------------------------------------------------

void WINAPI
ApplySettingsOnTaskbarThread(
    void*) {
    if (g_unloading) {
        return;
    }

    try {
        LoadSettings();
        RefreshAllTrackedButtons();

        Wh_Log(
            L"Taskbar Count Badges: "
            L"settings applied");
    } catch (...) {
        Wh_Log(
            L"Taskbar Count Badges: "
            L"settings apply failed");
    }
}

// -----------------------------------------------------------------------------
// Cleanup
// -----------------------------------------------------------------------------

void CleanupOnTaskbarThread() {
    PruneDeadTrackedButtons();

    for (auto& item :
         g_trackedButtons) {
        auto element =
            item.element.get();

        if (!element) {
            continue;
        }

        auto iconPanelElement =
            FindChildByName(
                element,
                L"IconPanel");

        if (!iconPanelElement) {
            continue;
        }

        auto iconPanel =
            iconPanelElement
                .try_as<
                    Controls::Panel>();

        if (!iconPanel) {
            continue;
        }

        auto badge =
            FindChildByName(
                iconPanelElement,
                L"WindhawkCountBadge")
                .try_as<
                    Controls::Border>();

        if (badge) {
            RemoveBadgeFromPanel(
                iconPanel,
                badge);
        }
    }

    g_trackedButtons.clear();

    ReleaseAppResolver();

    g_recountQueued =
        false;

    // Release the strong XAML/UI-thread object on its owning thread.
    g_taskbarDispatcher =
        nullptr;

    Wh_Log(
        L"Taskbar Count Badges: "
        L"cleanup complete");
}

bool CleanupSynchronously() {
    if (!g_taskbarDispatcher) {
        return true;
    }

    try {
        if (g_taskbarDispatcher
                .HasThreadAccess()) {
            CleanupOnTaskbarThread();
            return true;
        }

        auto operation =
            g_taskbarDispatcher
                .RunAsync(
                    winrt::Windows::UI::
                        Core::
                            CoreDispatcherPriority::
                                Normal,
                    []() {
                        CleanupOnTaskbarThread();
                    });

        operation.get();

        return true;
    } catch (...) {
        return false;
    }
}

// -----------------------------------------------------------------------------
// Taskbar.View hooks
// -----------------------------------------------------------------------------

bool HookTaskbarView(
    HMODULE module) {
    if (g_taskbarViewHooked
            .exchange(
                true)) {
        return true;
    }

    WindhawkUtils::SYMBOL_HOOK
        hooks[] = {
            {
                {
                    LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))",
                },
                &TaskListButton_UpdateVisualStates_Original,
                TaskListButton_UpdateVisualStates_Hook,
            },
            {
                {
                    LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskListGroupViewModel>::get_ViewModelCount(unsigned int *))",
                },
                &GetViewModelCount_Original,
                GetViewModelCount_Hook,
            },
        };

    if (!WindhawkUtils::
            HookSymbols(
                module,
                hooks,
                ARRAYSIZE(
                    hooks))) {
        g_taskbarViewHooked =
            false;

        Wh_Log(
            L"Taskbar Count Badges: "
            L"Taskbar.View hooks failed");

        return false;
    }

    Wh_Log(
        L"Taskbar Count Badges: "
        L"Taskbar.View hooks installed");

    return true;
}

// -----------------------------------------------------------------------------
// taskbar.dll symbols for XamlRoot access
// -----------------------------------------------------------------------------

bool HookTaskbarDll() {
    HMODULE module =
        LoadLibraryExW(
            L"taskbar.dll",
            nullptr,
            LOAD_LIBRARY_SEARCH_SYSTEM32);

    if (!module) {
        Wh_Log(
            L"Taskbar Count Badges: "
            L"taskbar.dll load failed");

        return false;
    }

    WindhawkUtils::SYMBOL_HOOK
        hooks[] = {
            {
                {
                    LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})",
                },
                &g_CTaskBand_ITaskListWndSite_vftable,
            },
            {
                {
                    LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )",
                },
                &g_CTaskBand_GetTaskbarHost,
            },
            {
                {
                    LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )",
                },
                &g_TaskbarHost_FrameHeight,
            },
            {
                {
                    LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))",
                },
                &g_RefCount_Decref,
            },
        };

    if (!WindhawkUtils::
            HookSymbols(
                module,
                hooks,
                ARRAYSIZE(
                    hooks))) {
        Wh_Log(
            L"Taskbar Count Badges: "
            L"taskbar.dll symbols failed");

        return false;
    }

    Wh_Log(
        L"Taskbar Count Badges: "
        L"taskbar XamlRoot symbols installed");

    return true;
}

// -----------------------------------------------------------------------------
// Late Taskbar.View loading
// -----------------------------------------------------------------------------

using LoadLibraryExW_t =
    decltype(
        &LoadLibraryExW);

LoadLibraryExW_t
    LoadLibraryExW_Original =
        nullptr;

HMODULE WINAPI
LoadLibraryExW_Hook(
    LPCWSTR fileName,
    HANDLE file,
    DWORD flags) {
    HMODULE module =
        LoadLibraryExW_Original(
            fileName,
            file,
            flags);

    if (module &&
        !g_unloading &&
        !g_taskbarViewHooked) {
        HMODULE taskbarView =
            GetTaskbarViewModuleHandle();

        if (taskbarView &&
            taskbarView ==
                module) {
            if (HookTaskbarView(
                    taskbarView)) {
                Wh_ApplyHookOperations();
            }
        }
    }

    return module;
}

// -----------------------------------------------------------------------------
// Lifecycle
// -----------------------------------------------------------------------------

BOOL Wh_ModInit() {
    g_unloading =
        false;

    g_recountQueued =
        false;

    g_countsInitialized =
        false;

    LoadSettings();

    Wh_Log(
        L"Taskbar Count Badges: "
        L"init PID=%lu",
        GetCurrentProcessId());

    if (!HookTaskbarDll()) {
        return FALSE;
    }

    if (HMODULE module =
            GetTaskbarViewModuleHandle()) {
        if (!HookTaskbarView(
                module)) {
            return FALSE;
        }

        return TRUE;
    }

    HMODULE kernelBase =
        GetModuleHandleW(
            L"kernelbase.dll");

    if (!kernelBase) {
        return FALSE;
    }

    auto loadLibraryExW =
        reinterpret_cast<
            decltype(
                &LoadLibraryExW)>(
            GetProcAddress(
                kernelBase,
                "LoadLibraryExW"));

    if (!loadLibraryExW) {
        return FALSE;
    }

    WindhawkUtils::
        SetFunctionHook(
            loadLibraryExW,
            LoadLibraryExW_Hook,
            &LoadLibraryExW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(
        L"Taskbar Count Badges: "
        L"after init");

    HWND taskbarWnd =
        FindCurrentProcessTaskbarWnd();

    if (!taskbarWnd) {
        return;
    }

    if (!RunOnTaskbarThread(
            taskbarWnd,
            InitializeExistingButtonsOnTaskbarThread,
            taskbarWnd)) {
        Wh_Log(
            L"Taskbar Count Badges: "
            L"initial taskbar-thread execution failed");
    }
}

void Wh_ModSettingsChanged() {
    HWND taskbarWnd =
        FindCurrentProcessTaskbarWnd();

    if (!taskbarWnd) {
        // No taskbar UI exists yet, so there are no concurrent UI reads.
        LoadSettings();
        return;
    }

    if (!RunOnTaskbarThread(
            taskbarWnd,
            ApplySettingsOnTaskbarThread,
            nullptr)) {
        Wh_Log(
            L"Taskbar Count Badges: "
            L"failed to apply settings on taskbar thread");
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(
        L"Taskbar Count Badges: "
        L"before uninit");

    {
        std::lock_guard<std::mutex> guard(
            g_queueMutex);

        g_unloading =
            true;
    }

    bool ok =
        CleanupSynchronously();

    Wh_Log(
        L"Taskbar Count Badges: "
        L"cleanup=%s",
        ok
            ? L"OK"
            : L"FAILED");
}

void Wh_ModUninit() {
    g_recountQueued =
        false;

    g_countsInitialized =
        false;

    g_appCounts.clear();
    g_appIdCache.clear();
    g_trackedButtons.clear();

    Wh_Log(
        L"Taskbar Count Badges: "
        L"uninit");
}