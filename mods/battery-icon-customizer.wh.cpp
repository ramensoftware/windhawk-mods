// ==WindhawkMod==
// @id              battery-icon-customizer
// @name            Taskbar Battery Icon Customizer
// @description     Restyle the Windows 11 taskbar battery icon: Apple iOS or classic look, percentage only, your own colors, charging animations, size and spacing - all from simple dropdowns
// @version         1.0
// @author          Faizaan
// @github          https://github.com/LoneFaizaan
// @homepage        https://github.com/LoneFaizaan/battery-icon-customizer
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// ==/WindhawkMod==

// Taskbar integration (finding the tray XAML tree, hooking icon creation) is
// based on "Taskbar tray system icon tweaks" by m417z (GPL-3.0):
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-tray-system-icon-tweaks.wh.cpp

// ==WindhawkModReadme==
/*
# Taskbar Battery Icon Customizer

Give the Windows 11 taskbar battery icon a new look - no coding needed.

![Preview of the available looks](https://raw.githubusercontent.com/LoneFaizaan/battery-icon-customizer/8c7af22c99a4fc36276e0a709ddfc13248930a09/images/preview.png)

![Charger and charging animations](https://raw.githubusercontent.com/LoneFaizaan/battery-icon-customizer/8c7af22c99a4fc36276e0a709ddfc13248930a09/images/animations.gif)

* **Apple iOS style** - a solid pill with the percentage cut out of it, green
  while charging, yellow in battery saver and red when low.
* **Classic style** - the older, compact single-color Windows 11 battery.
* **Percentage only** - replace the icon with just the number, with your
  choice of size, weight, font, % sign, charging bolt and an optional colored
  or outlined pill behind it.
* **Your own colors** - pick a color for every state (on battery, charging,
  plugged in, battery saver, low, very low) from simple lists.
* **Battery percentage** - next to the icon or inside the battery.
* **Animations** - bounce, zoom or flash when you plug in; shake, drop or
  flash when you unplug; a "filling up" or breathing effect while charging;
  and a pulse or blink when the battery is low.
* **Size and spacing** - make it bigger or line it up with the Wi-Fi and
  volume icons.

## Getting started

1. Open the **Settings** tab.
2. Choose a look from **Quick look** (for example *Apple iOS*) and click
   **Save settings**. The taskbar updates right away.

Want something else? Set **Quick look** to **Build my own**, then choose:

* **Icon style** - Windows 11, Classic Windows 11, Apple iOS, or Percentage
  only (then fine-tune it under **Percentage-only style**).
* **Color mode** - *Automatic*, *Single color*, or *My own colors* (then pick a
  color for each battery state from the lists under **Colors**).
* **Show battery percentage** - off, next to the icon, or inside it.
* **Size and spacing** and **Animations** - these apply to every look,
  including Quick looks. Turn on "Preview the charger animations" to see the
  plug-in and unplug animations each time you save.

Need an exact shade? Pick **Custom** in a color list and type a hex code such
as `#FF4545` in the matching field of **Exact colors** at the bottom.

## Tips

* Single color but red when the battery is low: Color mode *My own colors*,
  set every color to *Same as the taskbar text*, and **Low battery** to *Red*.
* *Automatic* colors in the Apple style follow iOS: white (or black on a light
  taskbar) normally, green when charging, yellow in battery saver and red at or
  below the low battery level.
* If Windows' own "Battery percentage" option is on, the mod hides it while it
  shows its own percentage, so it never appears twice.

Requires a Windows 11 build with the new colored battery icon (Windows 11
24H2/25H2 builds from late 2025 onwards).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- preset: custom
  $name: Quick look
  $description: "Pick a ready-made look in one click. Choose \"Build my own\" to use the style, color and percentage options below instead. Size and spacing always apply."
  $options:
  - custom: "Build my own (use the options below)"
  - windows: "Windows default"
  - windowsPercent: "Windows default + percentage"
  - apple: "Apple iOS - colored pill with the percentage inside"
  - appleMono: "Apple iOS - single-color pill"
  - classic: "Classic Windows 11 battery"
  - classicPercent: "Classic Windows 11 battery + percentage"
  - minimal: "Minimal - single color, percentage inside"
  - text: "Percentage only - just the number, no icon"
  - textPill: "Percentage only - number in a colored pill"
- style: modern
  $name: Icon style
  $description: "Used when Quick look is \"Build my own\"."
  $options:
  - modern: "Windows 11 (outline with a colored fill)"
  - classic: "Classic Windows 11 (compact, single color)"
  - apple: "Apple iOS (pill with the percentage inside)"
  - text: "Percentage only (just the number, no icon)"
- colors:
  - mode: windows
    $name: Color mode
    $description: "Used when Quick look is \"Build my own\"."
    $options:
    - windows: "Automatic (Windows / iOS colors)"
    - monochrome: "Single color (same as the taskbar text)"
    - custom: "My own colors (choose them below)"
  - normal: default
    $name: "On battery"
    $options:
    - default: "Automatic"
    - text: "Same as the taskbar text"
    - accent: "My Windows accent color"
    - white: "🤍 White"
    - black: "🖤 Black"
    - gray: "🩶 Gray"
    - red: "❤️ Red"
    - orange: "🧡 Orange"
    - yellow: "💛 Yellow"
    - green: "💚 Green"
    - blue: "💙 Blue"
    - cyan: "🩵 Light blue"
    - purple: "💜 Purple"
    - pink: "🩷 Pink"
    - custom: "Custom (set it in \"Exact colors\" at the bottom)"
  - charging: default
    $name: "Charging"
    $options:
    - default: "Automatic"
    - text: "Same as the taskbar text"
    - accent: "My Windows accent color"
    - white: "🤍 White"
    - black: "🖤 Black"
    - gray: "🩶 Gray"
    - red: "❤️ Red"
    - orange: "🧡 Orange"
    - yellow: "💛 Yellow"
    - green: "💚 Green"
    - blue: "💙 Blue"
    - cyan: "🩵 Light blue"
    - purple: "💜 Purple"
    - pink: "🩷 Pink"
    - custom: "Custom (set it in \"Exact colors\" at the bottom)"
  - pluggedIn: default
    $name: "Plugged in, not charging"
    $options:
    - default: "Automatic"
    - text: "Same as the taskbar text"
    - accent: "My Windows accent color"
    - white: "🤍 White"
    - black: "🖤 Black"
    - gray: "🩶 Gray"
    - red: "❤️ Red"
    - orange: "🧡 Orange"
    - yellow: "💛 Yellow"
    - green: "💚 Green"
    - blue: "💙 Blue"
    - cyan: "🩵 Light blue"
    - purple: "💜 Purple"
    - pink: "🩷 Pink"
    - custom: "Custom (set it in \"Exact colors\" at the bottom)"
  - batterySaver: default
    $name: "Battery saver on"
    $options:
    - default: "Automatic"
    - text: "Same as the taskbar text"
    - accent: "My Windows accent color"
    - white: "🤍 White"
    - black: "🖤 Black"
    - gray: "🩶 Gray"
    - red: "❤️ Red"
    - orange: "🧡 Orange"
    - yellow: "💛 Yellow"
    - green: "💚 Green"
    - blue: "💙 Blue"
    - cyan: "🩵 Light blue"
    - purple: "💜 Purple"
    - pink: "🩷 Pink"
    - custom: "Custom (set it in \"Exact colors\" at the bottom)"
  - low: default
    $name: "Low battery"
    $description: "Used on battery at or below the level set next."
    $options:
    - default: "Automatic"
    - text: "Same as the taskbar text"
    - accent: "My Windows accent color"
    - white: "🤍 White"
    - black: "🖤 Black"
    - gray: "🩶 Gray"
    - red: "❤️ Red"
    - orange: "🧡 Orange"
    - yellow: "💛 Yellow"
    - green: "💚 Green"
    - blue: "💙 Blue"
    - cyan: "🩵 Light blue"
    - purple: "💜 Purple"
    - pink: "🩷 Pink"
    - custom: "Custom (set it in \"Exact colors\" at the bottom)"
  - lowThreshold: 20
    $name: "Low battery starts at (%)"
  - critical: default
    $name: "Very low battery"
    $options:
    - default: "Automatic"
    - text: "Same as the taskbar text"
    - accent: "My Windows accent color"
    - white: "🤍 White"
    - black: "🖤 Black"
    - gray: "🩶 Gray"
    - red: "❤️ Red"
    - orange: "🧡 Orange"
    - yellow: "💛 Yellow"
    - green: "💚 Green"
    - blue: "💙 Blue"
    - cyan: "🩵 Light blue"
    - purple: "💜 Purple"
    - pink: "🩷 Pink"
    - custom: "Custom (set it in \"Exact colors\" at the bottom)"
  - criticalThreshold: 10
    $name: "Very low battery starts at (%)"
  - outline: default
    $name: "Outline"
    $description: "Windows 11 style: the battery outline. Apple style: the empty part of the pill."
    $options:
    - default: "Automatic"
    - text: "Same as the taskbar text"
    - accent: "My Windows accent color"
    - white: "🤍 White"
    - black: "🖤 Black"
    - gray: "🩶 Gray"
    - red: "❤️ Red"
    - orange: "🧡 Orange"
    - yellow: "💛 Yellow"
    - green: "💚 Green"
    - blue: "💙 Blue"
    - cyan: "🩵 Light blue"
    - purple: "💜 Purple"
    - pink: "🩷 Pink"
    - custom: "Custom (set it in \"Exact colors\" at the bottom)"
  - chargingIndicator: default
    $name: "Charging bolt / plug"
    $options:
    - default: "Automatic"
    - text: "Same as the taskbar text"
    - accent: "My Windows accent color"
    - white: "🤍 White"
    - black: "🖤 Black"
    - gray: "🩶 Gray"
    - red: "❤️ Red"
    - orange: "🧡 Orange"
    - yellow: "💛 Yellow"
    - green: "💚 Green"
    - blue: "💙 Blue"
    - cyan: "🩵 Light blue"
    - purple: "💜 Purple"
    - pink: "🩷 Pink"
    - custom: "Custom (set it in \"Exact colors\" at the bottom)"
  $name: Colors
  $description: "These colors are used when \"Color mode\" is \"My own colors\"."
- percentage:
  - position: none
    $name: Show battery percentage
    $description: "Used when Quick look is \"Build my own\"."
    $options:
    - none: "Off"
    - right: "Right of the icon"
    - left: "Left of the icon"
    - inside: "Inside the icon"
  - fontSize: 12
    $name: "Text size - next to the icon"
  - insideFontSize: 9
    $name: "Text size - inside the icon"
  - appleNumber: true
    $name: "Apple style - show the number inside the pill"
  - appleFontSize: 11
    $name: "Apple style - number size"
  - bold: false
    $name: "Bold numbers"
  - percentSign: true
    $name: "Show the % sign"
    $description: "Only used when the percentage is next to the icon."
  - color: auto
    $name: "Text color"
    $options:
    - auto: "Automatic"
    - fill: "Same as the battery fill"
    - text: "Same as the taskbar text"
    - accent: "My Windows accent color"
    - white: "🤍 White"
    - black: "🖤 Black"
    - gray: "🩶 Gray"
    - red: "❤️ Red"
    - orange: "🧡 Orange"
    - yellow: "💛 Yellow"
    - green: "💚 Green"
    - blue: "💙 Blue"
    - cyan: "🩵 Light blue"
    - purple: "💜 Purple"
    - pink: "🩷 Pink"
    - custom: "Custom (set it in \"Exact colors\" at the bottom)"
  - spacing: 3
    $name: "Space between icon and percentage (px)"
  $name: Battery percentage
- textOnly:
  - fontSize: 13
    $name: "Text size"
  - weight: semibold
    $name: "Text weight"
    $options:
    - regular: "Regular"
    - semibold: "Semibold"
    - bold: "Bold"
  - font: default
    $name: "Font"
    $options:
    - default: "Segoe UI Variable (Windows default)"
    - segoe: "Segoe UI"
    - bahnschrift: "Bahnschrift (narrow)"
    - cascadia: "Cascadia Code"
    - consolas: "Consolas (monospace)"
  - percentSign: true
    $name: "Show the % sign"
  - chargingIcon: after
    $name: "Charging bolt"
    $description: "Shown while the charger is plugged in."
    $options:
    - after: "After the number"
    - before: "Before the number"
    - none: "Don't show"
  - background: none
    $name: "Background"
    $options:
    - none: "None - just the number"
    - pill: "Colored pill (number cut out)"
    - outline: "Outlined pill"
  $name: Percentage-only style
  $description: "Used when the icon is shown as a percentage only (Quick look or Icon style \"Percentage only\"). Its colors follow Color mode and the Colors section."
- animations:
  - plugIn: bounce
    $name: "When the charger is plugged in"
    $options:
    - none: "No animation"
    - bounce: "Bounce"
    - zoom: "Zoom in"
    - flash: "Flash"
  - unplug: shake
    $name: "When the charger is unplugged"
    $options:
    - none: "No animation"
    - shake: "Shake"
    - drop: "Drop"
    - flash: "Flash"
  - whileCharging: none
    $name: "While charging"
    $options:
    - none: "No animation"
    - fillUp: "Filling up - the fill rises to full, then repeats"
    - breathing: "Breathing - the fill gently fades in and out"
  - lowBattery: pulse
    $name: "When the battery is low"
    $description: "On battery, at or below \"Low battery starts at (%)\" under Colors."
    $options:
    - none: "No animation"
    - pulse: "Pulse"
    - blink: "Blink"
  - speed: normal
    $name: "Animation speed"
    $options:
    - slow: "Slow"
    - normal: "Normal"
    - fast: "Fast"
  - preview: true
    $name: "Preview the charger animations when saving settings"
    $description: "Plays the plug-in and unplug animations once each time you click Save settings, so you can try them without unplugging."
  $name: Animations
  $description: "These apply to every look, including Quick looks."
- size:
  - scale: 100
    $name: "Icon size (%)"
    $description: "100 = normal size. Try 110-130 to make it bigger."
  - marginLeft: 0
    $name: "Extra space on the left (px)"
    $description: "Can be negative to move icons closer together."
  - marginRight: 0
    $name: "Extra space on the right (px)"
  - verticalOffset: 0
    $name: "Move up / down (px)"
    $description: "Positive values move the icon down."
  $name: Size and spacing
- customColors:
  - normal: "#FFFFFF"
    $name: "On battery"
  - charging: "#4CD964"
    $name: "Charging"
  - pluggedIn: "#4CD964"
    $name: "Plugged in, not charging"
  - batterySaver: "#FFD60A"
    $name: "Battery saver on"
  - low: "#FF9F1A"
    $name: "Low battery"
  - critical: "#FF4545"
    $name: "Very low battery"
  - outline: "#FFFFFF"
    $name: "Outline"
  - chargingIndicator: "#FFD60A"
    $name: "Charging bolt / plug"
  - percentage: "#FFFFFF"
    $name: "Percentage text"
  $name: Exact colors (optional)
  $description: "Only used for colors set to \"Custom\" above. Enter a hex code such as #FF4545 - you can copy one from any online color picker. Add two more digits in front for transparency, e.g. #80FF4545."
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <functional>
#include <list>
#include <string>
#include <string_view>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;
using winrt::Windows::Foundation::IInspectable;

////////////////////////////////////////////////////////////////////////////////
// Settings

enum class IconStyle { modern, classic, apple, text };
enum class TextChargingIcon { none, before, after };
enum class TextBackground { none, pill, outline };
enum class PlugAnimation { none, bounce, zoom, flash };
enum class UnplugAnimation { none, shake, drop, flash };
enum class ChargingAnimation { none, fillUp, breathing };
enum class LowAnimation { none, pulse, blink };
enum class ColorMode { windows, monochrome, custom };
enum class PercentPosition { none, right, left, inside };

struct ColorSpec {
    enum class Kind { Default, Text, Fill, Value };
    Kind kind = Kind::Default;
    winrt::Windows::UI::Color color{};
};

struct {
    IconStyle style;
    ColorMode colorMode;
    ColorSpec outline;
    ColorSpec normal;
    ColorSpec charging;
    ColorSpec pluggedIn;
    ColorSpec batterySaver;
    ColorSpec low;
    ColorSpec critical;
    ColorSpec chargingIndicator;
    int lowThreshold;
    int criticalThreshold;
    PercentPosition percentPosition;
    int percentFontSize;
    int insideFontSize;
    bool appleNumber;
    int appleFontSize;
    bool percentBold;
    bool percentSign;
    ColorSpec percentColor;
    int percentSpacing;
    int scale;
    int marginLeft;
    int marginRight;
    int verticalOffset;
    int textFontSize;
    int textWeight;
    std::wstring textFont;
    bool textPercentSign;
    TextChargingIcon textChargingIcon;
    TextBackground textBackground;
    PlugAnimation plugAnimation;
    UnplugAnimation unplugAnimation;
    ChargingAnimation chargingAnimation;
    LowAnimation lowAnimation;
    double animationSpeed;  // multiplier for animation durations
    bool animationPreview;
} g_settings;

std::atomic<bool> g_unloading;

std::wstring TrimLower(PCWSTR s) {
    std::wstring v = s ? s : L"";
    auto notSpace = [](wchar_t c) { return !iswspace(c); };
    v.erase(v.begin(), std::find_if(v.begin(), v.end(), notSpace));
    v.erase(std::find_if(v.rbegin(), v.rend(), notSpace).base(), v.end());
    std::transform(v.begin(), v.end(), v.begin(), towlower);
    return v;
}

bool ParseHexColor(std::wstring_view s, winrt::Windows::UI::Color& out) {
    if (s.size() < 2 || s[0] != L'#') {
        return false;
    }
    s.remove_prefix(1);

    uint8_t d[8];
    if (s.size() > 8) {
        return false;
    }
    for (size_t i = 0; i < s.size(); i++) {
        wchar_t c = s[i];
        if (c >= L'0' && c <= L'9') {
            d[i] = (uint8_t)(c - L'0');
        } else if (c >= L'a' && c <= L'f') {
            d[i] = (uint8_t)(c - L'a' + 10);
        } else {
            return false;
        }
    }

    uint8_t a = 255, r, g, b;
    switch (s.size()) {
        case 3:
            r = d[0] * 17, g = d[1] * 17, b = d[2] * 17;
            break;
        case 4:
            a = d[0] * 17, r = d[1] * 17, g = d[2] * 17, b = d[3] * 17;
            break;
        case 6:
            r = d[0] * 16 + d[1], g = d[2] * 16 + d[3], b = d[4] * 16 + d[5];
            break;
        case 8:
            a = d[0] * 16 + d[1], r = d[2] * 16 + d[3];
            g = d[4] * 16 + d[5], b = d[6] * 16 + d[7];
            break;
        default:
            return false;
    }

    out = winrt::Windows::UI::Color{a, r, g, b};
    return true;
}

ColorSpec ParseColorSpec(PCWSTR raw, PCWSTR settingName) {
    struct NamedColor {
        PCWSTR name;
        uint32_t argb;
    };
    static const NamedColor kNamedColors[] = {
        {L"white", 0xFFFFFFFF},  {L"black", 0xFF000000},
        {L"gray", 0xFF9E9E9E},   {L"grey", 0xFF9E9E9E},
        {L"red", 0xFFFF4545},    {L"orange", 0xFFFF9F1A},
        {L"yellow", 0xFFFFD60A}, {L"green", 0xFF4CD964},
        {L"lime", 0xFF9FD89F},   {L"blue", 0xFF3B9CFF},
        {L"cyan", 0xFF32D7E6},   {L"purple", 0xFFB38CFF},
        {L"pink", 0xFFFF6FAE},   {L"transparent", 0x00000000},
    };

    ColorSpec spec;
    std::wstring v = TrimLower(raw);

    if (v.empty() || v == L"default" || v == L"auto" || v == L"windows") {
        return spec;
    }

    if (v == L"text" || v == L"inherit") {
        spec.kind = ColorSpec::Kind::Text;
        return spec;
    }

    if (v == L"fill") {
        spec.kind = ColorSpec::Kind::Fill;
        return spec;
    }

    if (v == L"accent") {
        try {
            spec.color =
                winrt::Windows::UI::ViewManagement::UISettings().GetColorValue(
                    winrt::Windows::UI::ViewManagement::UIColorType::Accent);
            spec.kind = ColorSpec::Kind::Value;
        } catch (...) {
            Wh_Log(L"Failed to get the accent color");
        }
        return spec;
    }

    for (const auto& named : kNamedColors) {
        if (v == named.name) {
            spec.kind = ColorSpec::Kind::Value;
            spec.color = winrt::Windows::UI::Color{
                (uint8_t)(named.argb >> 24), (uint8_t)(named.argb >> 16),
                (uint8_t)(named.argb >> 8), (uint8_t)named.argb};
            return spec;
        }
    }

    if (ParseHexColor(v, spec.color)) {
        spec.kind = ColorSpec::Kind::Value;
        return spec;
    }

    Wh_Log(L"Invalid color for %s: %s", settingName, raw);
    return spec;
}

// Reads a color dropdown. "custom" means: use the matching entry of the
// "Exact colors" section (customColors.<key>).
ColorSpec GetColorSetting(PCWSTR name, PCWSTR customKey) {
    PCWSTR value = Wh_GetStringSetting(name);
    ColorSpec spec;
    if (TrimLower(value) == L"custom") {
        std::wstring customName = std::wstring(L"customColors.") + customKey;
        PCWSTR customValue = Wh_GetStringSetting(customName.c_str());
        spec = ParseColorSpec(customValue, customName.c_str());
        Wh_FreeStringSetting(customValue);
    } else {
        spec = ParseColorSpec(value, name);
    }
    Wh_FreeStringSetting(value);
    return spec;
}

// Ready-made looks from the "Quick look" dropdown. They override the style,
// color mode and percentage options; size and spacing still apply.
void ApplyPreset(PCWSTR preset) {
    auto& s = g_settings;
    auto set = [&](IconStyle style, ColorMode mode, PercentPosition position) {
        s.style = style;
        s.colorMode = mode;
        s.percentPosition = position;
        s.percentColor = ColorSpec{};
        s.appleNumber = true;
    };

    if (wcscmp(preset, L"windows") == 0) {
        set(IconStyle::modern, ColorMode::windows, PercentPosition::none);
    } else if (wcscmp(preset, L"windowsPercent") == 0) {
        set(IconStyle::modern, ColorMode::windows, PercentPosition::right);
    } else if (wcscmp(preset, L"apple") == 0) {
        set(IconStyle::apple, ColorMode::windows, PercentPosition::none);
    } else if (wcscmp(preset, L"appleMono") == 0) {
        set(IconStyle::apple, ColorMode::monochrome, PercentPosition::none);
    } else if (wcscmp(preset, L"classic") == 0) {
        set(IconStyle::classic, ColorMode::windows, PercentPosition::none);
    } else if (wcscmp(preset, L"classicPercent") == 0) {
        set(IconStyle::classic, ColorMode::windows, PercentPosition::right);
    } else if (wcscmp(preset, L"minimal") == 0) {
        set(IconStyle::modern, ColorMode::monochrome, PercentPosition::inside);
    } else if (wcscmp(preset, L"text") == 0) {
        set(IconStyle::text, ColorMode::windows, PercentPosition::none);
    } else if (wcscmp(preset, L"textPill") == 0) {
        set(IconStyle::text, ColorMode::windows, PercentPosition::none);
        s.textBackground = TextBackground::pill;
    }
}

void LoadSettings() {
    PCWSTR style = Wh_GetStringSetting(L"style");
    g_settings.style = IconStyle::modern;
    if (wcscmp(style, L"classic") == 0) {
        g_settings.style = IconStyle::classic;
    } else if (wcscmp(style, L"apple") == 0) {
        g_settings.style = IconStyle::apple;
    } else if (wcscmp(style, L"text") == 0) {
        g_settings.style = IconStyle::text;
    }
    Wh_FreeStringSetting(style);

    PCWSTR mode = Wh_GetStringSetting(L"colors.mode");
    g_settings.colorMode = ColorMode::windows;
    if (wcscmp(mode, L"monochrome") == 0) {
        g_settings.colorMode = ColorMode::monochrome;
    } else if (wcscmp(mode, L"custom") == 0) {
        g_settings.colorMode = ColorMode::custom;
    }
    Wh_FreeStringSetting(mode);

    g_settings.outline = GetColorSetting(L"colors.outline", L"outline");
    g_settings.normal = GetColorSetting(L"colors.normal", L"normal");
    g_settings.charging = GetColorSetting(L"colors.charging", L"charging");
    g_settings.pluggedIn = GetColorSetting(L"colors.pluggedIn", L"pluggedIn");
    g_settings.batterySaver = GetColorSetting(L"colors.batterySaver", L"batterySaver");
    g_settings.low = GetColorSetting(L"colors.low", L"low");
    g_settings.critical = GetColorSetting(L"colors.critical", L"critical");
    g_settings.chargingIndicator = GetColorSetting(L"colors.chargingIndicator", L"chargingIndicator");
    g_settings.lowThreshold = Wh_GetIntSetting(L"colors.lowThreshold");
    g_settings.criticalThreshold = Wh_GetIntSetting(L"colors.criticalThreshold");

    PCWSTR position = Wh_GetStringSetting(L"percentage.position");
    g_settings.percentPosition = PercentPosition::none;
    if (wcscmp(position, L"right") == 0) {
        g_settings.percentPosition = PercentPosition::right;
    } else if (wcscmp(position, L"left") == 0) {
        g_settings.percentPosition = PercentPosition::left;
    } else if (wcscmp(position, L"inside") == 0) {
        g_settings.percentPosition = PercentPosition::inside;
    }
    Wh_FreeStringSetting(position);

    g_settings.percentFontSize =
        std::clamp(Wh_GetIntSetting(L"percentage.fontSize"), 4, 40);
    g_settings.insideFontSize =
        std::clamp(Wh_GetIntSetting(L"percentage.insideFontSize"), 4, 40);
    g_settings.appleNumber = Wh_GetIntSetting(L"percentage.appleNumber");
    g_settings.appleFontSize =
        std::clamp(Wh_GetIntSetting(L"percentage.appleFontSize"), 4, 40);
    g_settings.percentBold = Wh_GetIntSetting(L"percentage.bold");
    g_settings.percentSign = Wh_GetIntSetting(L"percentage.percentSign");
    g_settings.percentColor = GetColorSetting(L"percentage.color", L"percentage");
    g_settings.percentSpacing = Wh_GetIntSetting(L"percentage.spacing");

    g_settings.scale = std::clamp(Wh_GetIntSetting(L"size.scale"), 25, 400);
    g_settings.marginLeft = Wh_GetIntSetting(L"size.marginLeft");
    g_settings.marginRight = Wh_GetIntSetting(L"size.marginRight");
    g_settings.verticalOffset = Wh_GetIntSetting(L"size.verticalOffset");

    // 0 means the setting is missing (e.g. saved by an older version).
    int textFontSize = Wh_GetIntSetting(L"textOnly.fontSize");
    g_settings.textFontSize =
        textFontSize > 0 ? std::clamp(textFontSize, 4, 48) : 13;
    PCWSTR weight = Wh_GetStringSetting(L"textOnly.weight");
    g_settings.textWeight = 600;
    if (wcscmp(weight, L"regular") == 0) {
        g_settings.textWeight = 400;
    } else if (wcscmp(weight, L"bold") == 0) {
        g_settings.textWeight = 700;
    }
    Wh_FreeStringSetting(weight);
    PCWSTR font = Wh_GetStringSetting(L"textOnly.font");
    g_settings.textFont = L"Segoe UI Variable";
    if (wcscmp(font, L"segoe") == 0) {
        g_settings.textFont = L"Segoe UI";
    } else if (wcscmp(font, L"bahnschrift") == 0) {
        g_settings.textFont = L"Bahnschrift";
    } else if (wcscmp(font, L"cascadia") == 0) {
        g_settings.textFont = L"Cascadia Code";
    } else if (wcscmp(font, L"consolas") == 0) {
        g_settings.textFont = L"Consolas";
    }
    Wh_FreeStringSetting(font);
    g_settings.textPercentSign = Wh_GetIntSetting(L"textOnly.percentSign");
    PCWSTR chargingIcon = Wh_GetStringSetting(L"textOnly.chargingIcon");
    g_settings.textChargingIcon = TextChargingIcon::after;
    if (wcscmp(chargingIcon, L"before") == 0) {
        g_settings.textChargingIcon = TextChargingIcon::before;
    } else if (wcscmp(chargingIcon, L"none") == 0) {
        g_settings.textChargingIcon = TextChargingIcon::none;
    }
    Wh_FreeStringSetting(chargingIcon);
    PCWSTR background = Wh_GetStringSetting(L"textOnly.background");
    g_settings.textBackground = TextBackground::none;
    if (wcscmp(background, L"pill") == 0) {
        g_settings.textBackground = TextBackground::pill;
    } else if (wcscmp(background, L"outline") == 0) {
        g_settings.textBackground = TextBackground::outline;
    }
    Wh_FreeStringSetting(background);

    // Returns the index of the chosen option, or `fallback` if missing.
    auto readChoice = [](PCWSTR name, std::initializer_list<PCWSTR> options,
                         int fallback) {
        PCWSTR value = Wh_GetStringSetting(name);
        int result = fallback;
        int index = 0;
        for (PCWSTR option : options) {
            if (wcscmp(value, option) == 0) {
                result = index;
                break;
            }
            index++;
        }
        Wh_FreeStringSetting(value);
        return result;
    };
    g_settings.plugAnimation = (PlugAnimation)readChoice(
        L"animations.plugIn", {L"none", L"bounce", L"zoom", L"flash"}, 1);
    g_settings.unplugAnimation = (UnplugAnimation)readChoice(
        L"animations.unplug", {L"none", L"shake", L"drop", L"flash"}, 1);
    g_settings.chargingAnimation = (ChargingAnimation)readChoice(
        L"animations.whileCharging", {L"none", L"fillUp", L"breathing"}, 0);
    g_settings.lowAnimation = (LowAnimation)readChoice(
        L"animations.lowBattery", {L"none", L"pulse", L"blink"}, 1);
    int speed = readChoice(L"animations.speed",
                           {L"slow", L"normal", L"fast"}, 1);
    g_settings.animationSpeed = speed == 0 ? 1.6 : speed == 2 ? 0.6 : 1.0;
    g_settings.animationPreview = Wh_GetIntSetting(L"animations.preview");

    PCWSTR preset = Wh_GetStringSetting(L"preset");
    ApplyPreset(preset);
    Wh_FreeStringSetting(preset);
}

////////////////////////////////////////////////////////////////////////////////
// XAML tree helpers

FrameworkElement EnumChildElements(
    DependencyObject element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (child && enumCallback(child)) {
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

FrameworkElement FindDescendantByClassName(FrameworkElement element,
                                           PCWSTR className,
                                           int maxDepth) {
    if (maxDepth <= 0) {
        return nullptr;
    }
    FrameworkElement found = nullptr;
    EnumChildElements(element, [&](FrameworkElement child) {
        if (winrt::get_class_name(child) == className) {
            found = child;
        } else {
            found = FindDescendantByClassName(child, className, maxDepth - 1);
        }
        return !!found;
    });
    return found;
}

bool IsChildOfElementByName(FrameworkElement element, PCWSTR name) {
    auto parent = element;
    while ((parent = Media::VisualTreeHelper::GetParent(parent)
                         .try_as<FrameworkElement>())) {
        if (parent.Name() == name) {
            return true;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// Battery state

// Glyphs of the "SysBatt Fluent Icons" font used by the new battery icon.
constexpr wchar_t kGlyphOutlineDischarging = L'';
constexpr wchar_t kGlyphOutlineCharging = L'';
constexpr wchar_t kGlyphOutlinePluggedIn = L'';
constexpr wchar_t kGlyphOutlineSaver = L'';

// Classic glyphs from Segoe Fluent Icons (MobBattery0-10 and friends).
constexpr wchar_t kClassicBattery0 = L'';
constexpr wchar_t kClassicCharging0 = L'';
constexpr wchar_t kClassicSaver0 = L'';

// Geometry of the glyphs, relative to the glyph box (advance x line height).
// Modern: 2560 x 2048 font units, classic: 2048 x 2048 font units.
constexpr double kModernAspect = 1.25;
constexpr double kModernInteriorLeft = 128.0 / 2560;
constexpr double kModernInteriorRight = 2304.0 / 2560;
constexpr double kClassicInteriorLeft = 128.0 / 2048;
constexpr double kClassicInteriorRight = 1664.0 / 2048;
constexpr double kInteriorTop = (2048.0 - 1536) / 2048;
constexpr double kInteriorBottom = (2048.0 - 512) / 2048;
// Region of the charging bolt / plug that sticks out of the top of the outline.
constexpr double kIndicatorLeft = 860.0 / 2560;
constexpr double kIndicatorRight = 1600.0 / 2560;
constexpr double kIndicatorBottom = (2048.0 - 880) / 2048;

enum class BatteryState { Unknown, Discharging, Charging, PluggedIn, Saver };

BatteryState StateFromOutlineGlyph(std::wstring_view text) {
    if (text.size() != 1) {
        return BatteryState::Unknown;
    }
    switch (text[0]) {
        case kGlyphOutlineDischarging:
            return BatteryState::Discharging;
        case kGlyphOutlineCharging:
            return BatteryState::Charging;
        case kGlyphOutlinePluggedIn:
            return BatteryState::PluggedIn;
        case kGlyphOutlineSaver:
            return BatteryState::Saver;
    }
    return BatteryState::Unknown;
}

// Fallback level (0-10) from the fill glyph, used if the percentage is unknown.
int LevelFromFillGlyph(std::wstring_view text) {
    if (text.size() != 1) {
        return 0;
    }
    wchar_t c = text[0];
    if (c >= L'' && c <= L'') {
        return c - L'';
    }
    if (c >= L'' && c <= L'') {
        return c - L'' + 3;
    }
    if (c >= L'' && c <= L'') {
        return c - L'' + 3;
    }
    if (c >= L'' && c <= L'') {
        return c - L'' + 3;
    }
    return 0;
}

int GetBatteryPercent() {
    SYSTEM_POWER_STATUS status;
    if (!GetSystemPowerStatus(&status) || status.BatteryLifePercent > 100) {
        return -1;
    }
    return status.BatteryLifePercent;
}

////////////////////////////////////////////////////////////////////////////////
// Per battery icon state

struct SavedProperty {
    winrt::weak_ref<DependencyObject> object;
    DependencyProperty property{nullptr};
    IInspectable value{nullptr};
};

struct BatteryIcon {
    winrt::weak_ref<FrameworkElement> content;
    winrt::weak_ref<Controls::StackPanel> stackPanel;
    winrt::weak_ref<Controls::Grid> glyphGrid;
    winrt::weak_ref<Controls::TextBlock> sysOutline;
    winrt::weak_ref<Controls::TextBlock> sysFill;
    winrt::weak_ref<Controls::TextBlock> sysText;

    // Elements added by the mod.
    Controls::TextBlock outline{nullptr};
    Controls::TextBlock indicator{nullptr};
    Controls::TextBlock fill{nullptr};
    Controls::TextBlock classic{nullptr};
    Controls::Grid insideGrid{nullptr};
    Controls::TextBlock insideText{nullptr};
    Controls::TextBlock percent{nullptr};

    // Apple style: [pill with the number] [nub] [bolt].
    Controls::StackPanel appleRoot{nullptr};
    Controls::Border appleBody{nullptr};
    Controls::TextBlock appleText{nullptr};       // over the filled part
    Controls::TextBlock appleTextTrack{nullptr};  // over the empty part
    Controls::Border appleNub{nullptr};
    Shapes::Polygon appleBolt{nullptr};
    std::wstring appleKey;

    // Percentage-only style: [bolt] [pill with the number] [bolt].
    Controls::StackPanel textRoot{nullptr};
    Shapes::Polygon textBoltBefore{nullptr};
    Controls::Border textPill{nullptr};
    Controls::TextBlock textNumber{nullptr};
    Shapes::Polygon textBoltAfter{nullptr};
    std::wstring textKey;

    // Animations.
    Media::CompositeTransform fxTransform{nullptr};
    Media::Animation::Storyboard oneShot{nullptr};
    Media::Animation::Storyboard oneShot2{nullptr};
    Media::Animation::Storyboard loop{nullptr};
    UIElement loopTarget{nullptr};
    int loopKind = 0;
    DispatcherTimer fillUpTimer{nullptr};
    int fillUpStep = 0;
    bool hasLastState = false;
    BatteryState lastState = BatteryState::Unknown;
    // Last Apple-style colors, reused by the "filling up" animation.
    winrt::Windows::UI::Color appleFill{}, appleTrack{}, appleOnFill{},
        appleOnTrack{};
    double appleWidth = 0;

    int64_t contentForegroundToken = 0;
    int64_t outlineTextToken = 0;
    int64_t fillTextToken = 0;
    int64_t fillForegroundToken = 0;
    int64_t fillVisibilityToken = 0;
    DispatcherTimer timer{nullptr};

    double baseFontSize = 16;
    std::vector<SavedProperty> saved;
    bool dead = false;
};

std::list<BatteryIcon> g_batteryIcons;

void RestoreSavedProperty(const SavedProperty& saved) {
    auto object = saved.object.get();
    if (!object) {
        return;
    }
    if (saved.value == DependencyProperty::UnsetValue()) {
        object.ClearValue(saved.property);
    } else {
        object.SetValue(saved.property, saved.value);
    }
}

// Sets a property on a system element, remembering its original local value
// so it can be restored when the mod is disabled.
void OverrideProperty(BatteryIcon& icon,
                      DependencyObject const& object,
                      DependencyProperty const& property,
                      IInspectable const& value) {
    bool alreadySaved = std::any_of(
        icon.saved.begin(), icon.saved.end(), [&](const SavedProperty& s) {
            return s.property == property && s.object.get() == object;
        });
    if (!alreadySaved) {
        icon.saved.push_back(SavedProperty{
            winrt::make_weak(object), property, object.ReadLocalValue(property)});
    }
    object.SetValue(property, value);
}

void RestoreProperty(BatteryIcon& icon,
                     DependencyObject const& object,
                     DependencyProperty const& property) {
    for (auto it = icon.saved.begin(); it != icon.saved.end(); ++it) {
        if (it->property == property && it->object.get() == object) {
            RestoreSavedProperty(*it);
            icon.saved.erase(it);
            return;
        }
    }
}

void SetForeground(Controls::TextBlock const& textBlock,
                   Media::Brush const& brush) {
    auto current = textBlock.ReadLocalValue(Controls::TextBlock::ForegroundProperty())
                       .try_as<Media::Brush>();
    if (!brush) {
        if (current) {
            textBlock.ClearValue(Controls::TextBlock::ForegroundProperty());
        }
        return;
    }
    if (current == brush) {
        return;
    }
    auto currentSolid = current.try_as<Media::SolidColorBrush>();
    auto newSolid = brush.try_as<Media::SolidColorBrush>();
    if (currentSolid && newSolid && currentSolid.Color() == newSolid.Color() &&
        currentSolid.Opacity() == newSolid.Opacity()) {
        return;
    }
    textBlock.Foreground(brush);
}

void SetTextIfChanged(Controls::TextBlock const& textBlock,
                      winrt::hstring const& text) {
    if (textBlock.Text() != text) {
        textBlock.Text(text);
    }
}

void SetFontSizeIfChanged(Controls::TextBlock const& textBlock, double size) {
    if (textBlock.FontSize() != size) {
        textBlock.FontSize(size);
    }
}

void SetVisible(UIElement const& element, bool visible) {
    auto value = visible ? Visibility::Visible : Visibility::Collapsed;
    if (element.Visibility() != value) {
        element.Visibility(value);
    }
}

// Resolves a color setting. nullptr means "inherit the taskbar text color".
Media::Brush ResolveColor(const ColorSpec& spec,
                          Media::Brush const& defaultBrush,
                          Media::Brush const& fillBrush) {
    switch (spec.kind) {
        case ColorSpec::Kind::Default:
            return defaultBrush;
        case ColorSpec::Kind::Text:
            return nullptr;
        case ColorSpec::Kind::Fill:
            return fillBrush;
        case ColorSpec::Kind::Value:
            return Media::SolidColorBrush(spec.color);
    }
    return defaultBrush;
}

bool NeedsPeriodicUpdate() {
    const auto& s = g_settings;
    return s.style != IconStyle::modern ||
           s.lowAnimation != LowAnimation::none ||
           s.percentPosition != PercentPosition::none ||
           (s.colorMode == ColorMode::custom &&
            (s.low.kind != ColorSpec::Kind::Default ||
             s.critical.kind != ColorSpec::Kind::Default));
}

void DetachBatteryIcon(BatteryIcon& icon);
void UpdateBatteryIcon(BatteryIcon& icon);

////////////////////////////////////////////////////////////////////////////////
// Apple style

using Color = winrt::Windows::UI::Color;

double Luminance(Color c) {
    return 0.2126 * c.R + 0.7152 * c.G + 0.0722 * c.B;
}

Color WithAlpha(Color c, uint8_t alpha) {
    return Color{(uint8_t)(c.A * alpha / 255), c.R, c.G, c.B};
}

Color ColorFromArgb(uint32_t argb) {
    return Color{(uint8_t)(argb >> 24), (uint8_t)(argb >> 16),
                 (uint8_t)(argb >> 8), (uint8_t)argb};
}

Media::LinearGradientBrush MakeSplitBrush(Color left,
                                          Color right,
                                          double split,
                                          double absoluteWidth) {
    Media::LinearGradientBrush brush;
    if (absoluteWidth > 0) {
        brush.MappingMode(Media::BrushMappingMode::Absolute);
        brush.StartPoint({0, 0});
        brush.EndPoint({(float)absoluteWidth, 0});
    } else {
        brush.StartPoint({0, 0.5f});
        brush.EndPoint({1, 0.5f});
    }
    auto stops = brush.GradientStops();
    for (auto [color, offset] : {std::pair{left, 0.0}, std::pair{left, split},
                                 std::pair{right, split},
                                 std::pair{right, 1.0}}) {
        Media::GradientStop stop;
        stop.Color(color);
        stop.Offset(offset);
        stops.Append(stop);
    }
    return brush;
}

// Fill level of the Apple pill (0..1): the fill/track split and the matching
// clips of the two number layers.
void ApplyAppleLevel(BatteryIcon& icon, double level) {
    level = std::clamp(level, 0.0, 1.0);
    icon.appleBody.Background(MakeSplitBrush(icon.appleFill, icon.appleTrack,
                                             level, /*absoluteWidth=*/0));
    float width = (float)icon.appleWidth;
    float splitX = (float)(icon.appleWidth * level);
    for (bool overFill : {true, false}) {
        auto& tb = overFill ? icon.appleText : icon.appleTextTrack;
        Media::RectangleGeometry clip;
        clip.Rect(overFill ? winrt::Windows::Foundation::Rect{-50, -50,
                                                              splitX + 50, 200}
                           : winrt::Windows::Foundation::Rect{
                                 splitX, -50, width + 50 - splitX, 200});
        tb.Clip(clip);
    }
}

// Colors shared by the Apple and percentage-only styles.
struct StatusColors {
    Color text;  // taskbar text color
    Color fill;  // color for the current battery state
};

Color ResolveColorValue(const ColorSpec& spec, Color textColor, Color fallback) {
    switch (spec.kind) {
        case ColorSpec::Kind::Text:
            return textColor;
        case ColorSpec::Kind::Value:
            return spec.color;
        default:
            return fallback;
    }
}

StatusColors GetStatusColors(FrameworkElement const& content,
                             BatteryState state,
                             int percent) {
    const auto& s = g_settings;

    // Taskbar text color: white on a dark taskbar, black on a light one.
    Color textColor = ColorFromArgb(0xFFFFFFFF);
    if (auto control = content.try_as<Controls::Control>()) {
        if (auto brush =
                control.Foreground().try_as<Media::SolidColorBrush>()) {
            textColor = brush.Color();
        }
    }
    bool darkTaskbar = Luminance(textColor) >= 128;

    // iOS system colors (dark / light variants).
    Color green = ColorFromArgb(darkTaskbar ? 0xFF30D158 : 0xFF34C759);
    Color yellow = ColorFromArgb(darkTaskbar ? 0xFFFFD60A : 0xFFFFCC00);
    Color red = ColorFromArgb(darkTaskbar ? 0xFFFF453A : 0xFFFF3B30);

    bool charging = state == BatteryState::Charging ||
                    state == BatteryState::PluggedIn;

    Color automatic = textColor;
    if (charging) {
        automatic = green;
    } else if (state == BatteryState::Saver) {
        automatic = yellow;
    } else if (percent >= 0 && percent <= s.lowThreshold) {
        automatic = red;
    }

    Color fillColor = automatic;
    if (s.colorMode == ColorMode::monochrome) {
        fillColor = textColor;
    } else if (s.colorMode == ColorMode::custom) {
        const ColorSpec* spec = &s.normal;
        if (state == BatteryState::Charging) {
            spec = &s.charging;
        } else if (state == BatteryState::PluggedIn) {
            spec = &s.pluggedIn;
        } else if (percent >= 0 &&
                   s.critical.kind != ColorSpec::Kind::Default &&
                   percent <= s.criticalThreshold) {
            spec = &s.critical;
        } else if (percent >= 0 && s.low.kind != ColorSpec::Kind::Default &&
                   percent <= s.lowThreshold) {
            spec = &s.low;
        } else if (state == BatteryState::Saver) {
            spec = &s.batterySaver;
        }
        fillColor = ResolveColorValue(*spec, textColor, automatic);
    }

    return StatusColors{textColor, fillColor};
}

// Dark text on a light background, light text on a dark one.
Color ContrastColor(Color background, Color fallback) {
    if (background.A < 128) {
        return fallback;
    }
    return Luminance(background) >= 150 ? ColorFromArgb(0xFF000000)
                                        : ColorFromArgb(0xFFFFFFFF);
}

// A filled lightning bolt polygon.
void SetBoltShape(Shapes::Polygon const& bolt, double height) {
    static const float kBolt[][2] = {
        {0.58f, 0.00f}, {0.06f, 0.58f}, {0.44f, 0.58f}, {0.30f, 1.00f},
        {0.94f, 0.36f}, {0.56f, 0.36f}, {0.86f, 0.00f},
    };
    double width = height * 0.62;
    Media::PointCollection points;
    for (const auto& p : kBolt) {
        points.Append({(float)(p[0] * width), (float)(p[1] * height)});
    }
    bolt.Points(points);
    bolt.Width(width);
    bolt.Height(height);
}

void UpdateAppleIcon(BatteryIcon& icon,
                     FrameworkElement const& content,
                     BatteryState state,
                     int percent,
                     int levelFallback,
                     double fontSize) {
    const auto& s = g_settings;

    StatusColors colors = GetStatusColors(content, state, percent);
    Color textColor = colors.text;
    Color fillColor = colors.fill;

    bool charging = state == BatteryState::Charging ||
                    state == BatteryState::PluggedIn;

    auto resolve = [&](const ColorSpec& spec, Color fallback) {
        return ResolveColorValue(spec, textColor, fallback);
    };

    // Empty part of the pill and the nub: translucent text color.
    Color trackColor = WithAlpha(textColor, 90);
    if (s.colorMode == ColorMode::custom &&
        s.outline.kind == ColorSpec::Kind::Value) {
        trackColor = s.outline.color;
    }
    Color nubColor = WithAlpha(textColor, 110);

    // The number is "cut out": dark on a light fill, light on a dark fill,
    // and in the text color over the empty part.
    Color onFill = Luminance(fillColor) >= 150 ? ColorFromArgb(0xFF000000)
                                               : ColorFromArgb(0xFFFFFFFF);
    if (fillColor.A < 128) {
        onFill = textColor;
    }
    Color onTrack = textColor;
    if (s.percentColor.kind == ColorSpec::Kind::Value) {
        onFill = onTrack = s.percentColor.color;
    } else if (s.percentColor.kind == ColorSpec::Kind::Text) {
        onFill = onTrack = textColor;
    }

    Color boltColor = textColor;
    if (s.colorMode == ColorMode::custom) {
        boltColor = resolve(s.chargingIndicator, textColor);
    }

    double level = percent >= 0 ? percent / 100.0 : levelFallback / 10.0;
    bool showNumber = percent >= 0 &&
                      (s.appleNumber ||
                       s.percentPosition == PercentPosition::inside);

    double height = std::round(fontSize * 0.75);
    double width = std::round(fontSize * 1.625);
    double radius = height * 0.36;
    double nubWidth = std::max(1.5, std::round(fontSize * 0.11 * 2) / 2);
    double nubHeight = std::round(height * 0.42);
    double gap = std::max(1.0, std::round(fontSize * 0.07));
    double boltHeight = std::round(fontSize * 0.6);
    double boltWidth = boltHeight * 0.62;

    auto argb = [](Color c) {
        return (unsigned)((c.A << 24) | (c.R << 16) | (c.G << 8) | c.B);
    };
    wchar_t key[256];
    swprintf(key, ARRAYSIZE(key),
             L"%08X|%08X|%08X|%08X|%08X|%08X|%.3f|%.2f|%d|%d|%d|%d",
             argb(fillColor), argb(trackColor), argb(nubColor), argb(onFill),
             argb(onTrack), argb(boltColor), level, fontSize,
             showNumber ? percent : -1, charging, s.appleFontSize,
             s.percentBold);
    if (icon.appleKey == key) {
        return;
    }
    icon.appleKey = key;

    icon.appleBody.Width(width);
    icon.appleBody.Height(height);
    icon.appleBody.CornerRadius(CornerRadius{radius, radius, radius, radius});
    icon.appleFill = fillColor;
    icon.appleTrack = trackColor;
    icon.appleOnFill = onFill;
    icon.appleOnTrack = onTrack;
    icon.appleWidth = width;

    for (bool overFill : {true, false}) {
        auto& tb = overFill ? icon.appleText : icon.appleTextTrack;
        SetVisible(tb, showNumber);
        if (!showNumber) {
            continue;
        }
        SetTextIfChanged(tb, winrt::to_hstring(percent));
        SetFontSizeIfChanged(tb, s.appleFontSize * s.scale / 100.0);
        // Semibold by default, like iOS; "Bold numbers" goes heavier.
        tb.FontWeight(winrt::Windows::UI::Text::FontWeight{
            (uint16_t)(s.percentBold ? 800 : 600)});
        tb.Foreground(Media::SolidColorBrush(overFill ? onFill : onTrack));
    }
    ApplyAppleLevel(icon, level);

    icon.appleNub.Width(nubWidth);
    icon.appleNub.Height(nubHeight);
    icon.appleNub.Margin(Thickness{gap * 0.75, 0, 0, 0});
    icon.appleNub.CornerRadius(
        CornerRadius{0, nubWidth, nubWidth, 0});
    icon.appleNub.Background(Media::SolidColorBrush(nubColor));

    SetVisible(icon.appleBolt, charging);
    if (charging) {
        // A filled lightning bolt, drawn as a polygon.
        static const float kBolt[][2] = {
            {0.58f, 0.00f}, {0.06f, 0.58f}, {0.44f, 0.58f}, {0.30f, 1.00f},
            {0.94f, 0.36f}, {0.56f, 0.36f}, {0.86f, 0.00f},
        };
        Media::PointCollection points;
        for (const auto& p : kBolt) {
            points.Append({(float)(p[0] * boltWidth), (float)(p[1] * boltHeight)});
        }
        icon.appleBolt.Points(points);
        icon.appleBolt.Width(boltWidth);
        icon.appleBolt.Height(boltHeight);
        icon.appleBolt.Margin(Thickness{gap * 1.5, 0, 0, 0});
        icon.appleBolt.Fill(Media::SolidColorBrush(boltColor));
    }
}

////////////////////////////////////////////////////////////////////////////////
// Percentage-only style

void UpdateTextIcon(BatteryIcon& icon,
                    FrameworkElement const& content,
                    BatteryState state,
                    int percent) {
    const auto& s = g_settings;

    StatusColors colors = GetStatusColors(content, state, percent);
    Color numberColor = colors.fill;
    if (s.percentColor.kind == ColorSpec::Kind::Value) {
        numberColor = s.percentColor.color;
    } else if (s.percentColor.kind == ColorSpec::Kind::Text) {
        numberColor = colors.text;
    }

    bool charging = state == BatteryState::Charging ||
                    state == BatteryState::PluggedIn;
    Color boltColor = numberColor;
    if (s.colorMode == ColorMode::custom) {
        boltColor =
            ResolveColorValue(s.chargingIndicator, colors.text, numberColor);
    }

    bool pill = s.textBackground == TextBackground::pill;
    bool outlined = s.textBackground == TextBackground::outline;
    bool showBolt = charging && s.textChargingIcon != TextChargingIcon::none;
    double fontSize = s.textFontSize * s.scale / 100.0;

    std::wstring text = std::to_wstring(percent);
    if (s.textPercentSign) {
        text += L'%';
    }

    auto argb = [](Color c) {
        return (unsigned)((c.A << 24) | (c.R << 16) | (c.G << 8) | c.B);
    };
    wchar_t key[512];
    swprintf(key, ARRAYSIZE(key), L"%s|%08X|%08X|%08X|%.2f|%d|%s|%d|%d|%d",
             text.c_str(), argb(numberColor), argb(boltColor),
             argb(colors.text), fontSize, s.textWeight, s.textFont.c_str(),
             (int)s.textBackground, (int)s.textChargingIcon, showBolt);
    if (icon.textKey == key) {
        return;
    }
    icon.textKey = key;

    // The number. On a filled pill it is "cut out" in a contrasting color.
    SetTextIfChanged(icon.textNumber, winrt::hstring(text));
    SetFontSizeIfChanged(icon.textNumber, fontSize);
    icon.textNumber.FontWeight(
        winrt::Windows::UI::Text::FontWeight{(uint16_t)s.textWeight});
    icon.textNumber.FontFamily(Media::FontFamily(winrt::hstring(s.textFont)));
    icon.textNumber.Foreground(Media::SolidColorBrush(
        pill ? ContrastColor(numberColor, colors.text) : numberColor));

    // Optional pill / outlined pill behind the number.
    double padH = (pill || outlined) ? std::round(fontSize * 0.4) : 0;
    double padV = (pill || outlined) ? std::round(fontSize * 0.2) : 0;
    // Rounded corners also clip the content, so only round a visible pill.
    double radius = (pill || outlined) ? std::round(fontSize * 0.56) : 0;
    double border = outlined ? std::max(1.0, std::round(fontSize * 0.1)) : 0;
    icon.textPill.Padding(Thickness{padH, padV, padH, padV});
    icon.textPill.CornerRadius(CornerRadius{radius, radius, radius, radius});
    icon.textPill.BorderThickness(Thickness{border, border, border, border});
    if (pill) {
        icon.textPill.Background(Media::SolidColorBrush(numberColor));
    } else {
        icon.textPill.ClearValue(Controls::Border::BackgroundProperty());
    }
    if (outlined) {
        icon.textPill.BorderBrush(Media::SolidColorBrush(numberColor));
    } else {
        icon.textPill.ClearValue(Controls::Border::BorderBrushProperty());
    }

    // Charging bolt before or after the number.
    double gap = std::max(1.0, std::round(fontSize * 0.2));
    for (bool before : {true, false}) {
        auto& bolt = before ? icon.textBoltBefore : icon.textBoltAfter;
        bool visible =
            showBolt && s.textChargingIcon == (before ? TextChargingIcon::before
                                                      : TextChargingIcon::after);
        SetVisible(bolt, visible);
        if (visible) {
            SetBoltShape(bolt, std::round(fontSize * 0.8));
            bolt.Fill(Media::SolidColorBrush(boltColor));
            bolt.Margin(before ? Thickness{0, 0, gap, 0}
                               : Thickness{gap, 0, 0, 0});
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Animations

namespace Anim = winrt::Windows::UI::Xaml::Media::Animation;

enum LoopKind { kLoopNone, kLoopBreathing, kLoopFillUp, kLoopLowPulse, kLoopLowBlink };

winrt::Windows::Foundation::TimeSpan ScaledMs(double ms) {
    return std::chrono::milliseconds((int64_t)(ms * g_settings.animationSpeed));
}

// Adds a key-framed animation of `property` on `target`. Frames are
// {milliseconds, value}; times are scaled by the animation speed setting.
void AddKeyFrames(Anim::Storyboard const& storyboard,
                  DependencyObject const& target,
                  PCWSTR property,
                  std::initializer_list<std::pair<double, double>> frames) {
    Anim::DoubleAnimationUsingKeyFrames animation;
    for (auto [ms, value] : frames) {
        Anim::SineEase ease;
        ease.EasingMode(Anim::EasingMode::EaseInOut);
        Anim::EasingDoubleKeyFrame frame;
        frame.KeyTime(Anim::KeyTimeHelper::FromTimeSpan(ScaledMs(ms)));
        frame.Value(value);
        frame.EasingFunction(ease);
        animation.KeyFrames().Append(frame);
    }
    Anim::Storyboard::SetTarget(animation, target);
    Anim::Storyboard::SetTargetProperty(animation, property);
    storyboard.Children().Append(animation);
}

void StopStoryboard(Anim::Storyboard& storyboard) {
    if (storyboard) {
        storyboard.Stop();
        storyboard = nullptr;
    }
}

// The one-shot animations move and scale the whole battery visual.
void EnsureFxTransform(BatteryIcon& icon, UIElement const& glyphGrid) {
    if (!icon.fxTransform) {
        icon.fxTransform = Media::CompositeTransform();
    }
    if (glyphGrid.RenderTransform() != icon.fxTransform) {
        OverrideProperty(icon, glyphGrid, UIElement::RenderTransformProperty(),
                         icon.fxTransform);
        OverrideProperty(icon, glyphGrid,
                         UIElement::RenderTransformOriginProperty(),
                         winrt::box_value(winrt::Windows::Foundation::Point{0.5f, 0.5f}));
    }
}

Anim::Storyboard BuildChargerAnimation(BatteryIcon& icon,
                                       UIElement const& glyphGrid,
                                       bool pluggedIn) {
    const auto& s = g_settings;
    auto t = icon.fxTransform;
    Anim::Storyboard sb;
    if (pluggedIn) {
        switch (s.plugAnimation) {
            case PlugAnimation::bounce:
                for (PCWSTR p : {L"ScaleX", L"ScaleY"}) {
                    AddKeyFrames(sb, t, p,
                                 {{0, 1}, {120, 1.3}, {260, 0.9}, {380, 1.07},
                                  {480, 1}});
                }
                break;
            case PlugAnimation::zoom:
                for (PCWSTR p : {L"ScaleX", L"ScaleY"}) {
                    AddKeyFrames(sb, t, p, {{0, 0.3}, {280, 1.12}, {420, 1}});
                }
                AddKeyFrames(sb, glyphGrid, L"Opacity", {{0, 0}, {200, 1}});
                break;
            case PlugAnimation::flash:
                AddKeyFrames(sb, glyphGrid, L"Opacity",
                             {{0, 1}, {120, 0.15}, {240, 1}, {360, 0.15}, {480, 1}});
                break;
            case PlugAnimation::none:
                return nullptr;
        }
    } else {
        switch (s.unplugAnimation) {
            case UnplugAnimation::shake:
                AddKeyFrames(sb, t, L"TranslateX",
                             {{0, 0}, {70, -5}, {140, 5}, {210, -4}, {280, 4},
                              {350, -2}, {420, 2}, {490, 0}});
                break;
            case UnplugAnimation::drop:
                AddKeyFrames(sb, t, L"TranslateY", {{0, 0}, {150, 4}, {320, 0}});
                AddKeyFrames(sb, t, L"ScaleY", {{0, 1}, {150, 0.8}, {320, 1}});
                break;
            case UnplugAnimation::flash:
                AddKeyFrames(sb, glyphGrid, L"Opacity",
                             {{0, 1}, {120, 0.15}, {240, 1}, {360, 0.15}, {480, 1}});
                break;
            case UnplugAnimation::none:
                return nullptr;
        }
    }
    sb.FillBehavior(Anim::FillBehavior::Stop);
    return sb;
}

// Plays the plug-in or unplug animation. `thenUnplug` chains the unplug
// animation after the plug-in one (used for the preview).
void PlayChargerAnimation(BatteryIcon& icon,
                          UIElement const& glyphGrid,
                          bool pluggedIn,
                          bool thenUnplug) {
    StopStoryboard(icon.oneShot);
    StopStoryboard(icon.oneShot2);
    EnsureFxTransform(icon, glyphGrid);

    if (auto sb = BuildChargerAnimation(icon, glyphGrid, pluggedIn)) {
        sb.Begin();
        icon.oneShot = sb;
    }
    if (thenUnplug) {
        if (auto sb = BuildChargerAnimation(icon, glyphGrid, false)) {
            sb.BeginTime(winrt::box_value(ScaledMs(900))
                             .as<winrt::Windows::Foundation::IReference<
                                 winrt::Windows::Foundation::TimeSpan>>());
            sb.Begin();
            icon.oneShot2 = sb;
        }
    }
}

void StopLoop(BatteryIcon& icon) {
    StopStoryboard(icon.loop);
    if (icon.fillUpTimer) {
        icon.fillUpTimer.Stop();
        icon.fillUpTimer = nullptr;
    }
    icon.loopKind = kLoopNone;
    icon.loopTarget = nullptr;
}

// Fill glyph of the modern style for a level (0-10) in a given state.
wchar_t ModernFillGlyph(BatteryState state, int level) {
    if (level <= 0) {
        return 0;
    }
    if (level >= 3 && state == BatteryState::Charging) {
        return (wchar_t)(L'' + (level - 3));
    }
    if (level >= 3 && state == BatteryState::PluggedIn) {
        return (wchar_t)(L'' + (level - 3));
    }
    return (wchar_t)(L'' + level);
}

// "Filling up": the fill rises from the current level to full, holds, and
// starts over.
void FillUpTick(BatteryIcon& icon) {
    if (icon.dead || g_unloading) {
        return;
    }
    auto sysOutline = icon.sysOutline.get();
    auto sysFill = icon.sysFill.get();
    if (!sysOutline || !sysFill) {
        return;
    }

    try {
        const auto& s = g_settings;
        BatteryState state =
            StateFromOutlineGlyph(std::wstring(sysOutline.Text()));
        int percent = GetBatteryPercent();
        int baseLevel = percent >= 0 ? std::clamp((percent + 5) / 10, 0, 10)
                                     : LevelFromFillGlyph(sysFill.Text());
        if (s.style == IconStyle::apple) {
            // Smooth rise, 4% per tick, then a short hold at full.
            double real = percent >= 0 ? percent / 100.0 : baseLevel / 10.0;
            int rise = (int)std::ceil((1.0 - real) / 0.04);
            int steps = std::max(rise + 1 + 3, 5);
            icon.fillUpStep = (icon.fillUpStep + 1) % steps;
            ApplyAppleLevel(icon, std::min(1.0, real + icon.fillUpStep * 0.04));
            icon.appleKey.clear();  // the next regular update restores it
            return;
        }

        int steps = std::max((10 - baseLevel) + 3, 4);  // rise, brief hold
        icon.fillUpStep = (icon.fillUpStep + 1) % steps;
        int level = std::min(10, baseLevel + icon.fillUpStep);

        if (s.style == IconStyle::classic) {
            icon.classic.Text(winrt::hstring(
                std::wstring(1, (wchar_t)(kClassicCharging0 + level))));
        } else {
            wchar_t glyph = ModernFillGlyph(state, level);
            icon.fill.Text(glyph ? winrt::hstring(std::wstring(1, glyph))
                                 : winrt::hstring());
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Fill-up animation failed: %08X", (unsigned)e.code());
    }
}

void StartLoop(BatteryIcon& icon, int kind, UIElement const& target) {
    icon.loopKind = kind;
    icon.loopTarget = target;

    if (kind == kLoopFillUp) {
        BatteryIcon* iconPtr = &icon;
        icon.fillUpStep = 0;
        icon.fillUpTimer = DispatcherTimer();
        icon.fillUpTimer.Interval(
            ScaledMs(g_settings.style == IconStyle::apple ? 80 : 220));
        icon.fillUpTimer.Tick([iconPtr](IInspectable const&, IInspectable const&) {
            FillUpTick(*iconPtr);
        });
        icon.fillUpTimer.Start();
        return;
    }

    Anim::Storyboard sb;
    switch (kind) {
        case kLoopBreathing:
            AddKeyFrames(sb, target, L"Opacity", {{0, 1}, {1100, 0.35}});
            sb.AutoReverse(true);
            break;
        case kLoopLowPulse:
            AddKeyFrames(sb, target, L"Opacity", {{0, 1}, {800, 0.3}});
            sb.AutoReverse(true);
            break;
        case kLoopLowBlink:
            AddKeyFrames(sb, target, L"Opacity",
                         {{0, 1}, {380, 1}, {420, 0.1}, {760, 0.1}, {800, 1}});
            break;
    }
    sb.RepeatBehavior(Anim::RepeatBehaviorHelper::Forever());
    sb.Begin();
    icon.loop = sb;
}

void UpdateAnimations(BatteryIcon& icon,
                      BatteryState state,
                      int percent,
                      UIElement const& glyphGrid,
                      UIElement const& stackPanel,
                      bool classic,
                      bool apple,
                      bool textOnly,
                      bool inside) {
    const auto& s = g_settings;
    bool known = state != BatteryState::Unknown;
    bool connected = state == BatteryState::Charging ||
                     state == BatteryState::PluggedIn;

    // One-shot animation when the charger is plugged in or unplugged.
    if (known) {
        if (icon.hasLastState) {
            bool wasConnected = icon.lastState == BatteryState::Charging ||
                                icon.lastState == BatteryState::PluggedIn;
            if (connected != wasConnected) {
                PlayChargerAnimation(icon, glyphGrid, connected, false);
            }
        }
        icon.lastState = state;
        icon.hasLastState = true;
    }

    // Looping animation while charging or when the battery is low.
    int kind = kLoopNone;
    UIElement target{nullptr};
    if (known && connected &&
        s.chargingAnimation != ChargingAnimation::none) {
        bool fillUp = s.chargingAnimation == ChargingAnimation::fillUp &&
                      !textOnly && !inside;
        kind = fillUp ? kLoopFillUp : kLoopBreathing;
        if (textOnly) {
            target = icon.textRoot;
        } else if (apple) {
            target = icon.appleBody;
        } else if (classic) {
            target = icon.classic;
        } else if (inside) {
            target = icon.insideText;
        } else {
            target = icon.fill;
        }
    } else if (known && !connected && percent >= 0 &&
               percent <= s.lowThreshold &&
               s.lowAnimation != LowAnimation::none) {
        kind = s.lowAnimation == LowAnimation::blink ? kLoopLowBlink
                                                     : kLoopLowPulse;
        target = stackPanel;
    }

    if (kind != icon.loopKind || target != icon.loopTarget) {
        StopLoop(icon);
        if (kind != kLoopNone) {
            StartLoop(icon, kind, target);
        }
    }
}

void UpdateBatteryIconUnsafe(BatteryIcon& icon,
                             FrameworkElement const& content,
                             Controls::TextBlock const& sysOutline,
                             Controls::TextBlock const& sysFill,
                             Controls::Grid const& glyphGrid,
                             Controls::StackPanel const& stackPanel) {
    const auto& s = g_settings;

    std::wstring outlineText{sysOutline.Text()};
    std::wstring fillText{sysFill.Text()};
    BatteryState state = StateFromOutlineGlyph(outlineText);
    int percent = GetBatteryPercent();

    bool knownState = state != BatteryState::Unknown;
    bool classic = s.style == IconStyle::classic && knownState;
    bool apple = s.style == IconStyle::apple && knownState;
    bool textOnly = s.style == IconStyle::text && knownState && percent >= 0;
    bool inside = s.percentPosition == PercentPosition::inside &&
                  percent >= 0 && knownState && !apple && !textOnly;
    bool beside = (s.percentPosition == PercentPosition::left ||
                   s.percentPosition == PercentPosition::right) &&
                  percent >= 0 && !textOnly;
    bool charging = state == BatteryState::Charging ||
                    state == BatteryState::PluggedIn;

    double fontSize = icon.baseFontSize * s.scale / 100.0;

    // The system glyphs are hidden and take no space; the mod draws its own
    // copies on top so their colors and size can be changed freely.
    for (const auto& tb : {sysOutline, sysFill}) {
        OverrideProperty(icon, tb, UIElement::OpacityProperty(),
                         winrt::box_value(0.0));
        OverrideProperty(icon, tb, FrameworkElement::MaxWidthProperty(),
                         winrt::box_value(0.0));
        OverrideProperty(icon, tb, FrameworkElement::MaxHeightProperty(),
                         winrt::box_value(0.0));
    }

    // Colors. A null brush means "inherit the taskbar text color".
    bool sysFillHasLocalBrush =
        sysFill.ReadLocalValue(Controls::TextBlock::ForegroundProperty()) !=
        DependencyProperty::UnsetValue();
    Media::Brush windowsFillBrush =
        sysFillHasLocalBrush ? sysFill.Foreground() : nullptr;

    const ColorSpec* stateSpec = nullptr;
    switch (state) {
        case BatteryState::Charging:
            stateSpec = &s.charging;
            break;
        case BatteryState::PluggedIn:
            stateSpec = &s.pluggedIn;
            break;
        case BatteryState::Discharging:
        case BatteryState::Saver:
            if (percent >= 0 && s.critical.kind != ColorSpec::Kind::Default &&
                percent <= s.criticalThreshold) {
                stateSpec = &s.critical;
            } else if (percent >= 0 && s.low.kind != ColorSpec::Kind::Default &&
                       percent <= s.lowThreshold) {
                stateSpec = &s.low;
            } else {
                stateSpec = state == BatteryState::Saver ? &s.batterySaver
                                                          : &s.normal;
            }
            break;
        case BatteryState::Unknown:
            break;
    }

    Media::Brush fillBrush{nullptr};
    Media::Brush outlineBrush{nullptr};
    Media::Brush indicatorBrush{nullptr};
    Media::Brush classicBrush{nullptr};
    bool showIndicator = false;

    switch (s.colorMode) {
        case ColorMode::windows:
            fillBrush = windowsFillBrush;
            break;
        case ColorMode::monochrome:
            break;
        case ColorMode::custom:
            fillBrush = stateSpec
                            ? ResolveColor(*stateSpec, windowsFillBrush, nullptr)
                            : windowsFillBrush;
            outlineBrush = ResolveColor(s.outline, nullptr, nullptr);
            classicBrush =
                stateSpec ? ResolveColor(*stateSpec, nullptr, nullptr) : nullptr;
            if (s.chargingIndicator.kind != ColorSpec::Kind::Default) {
                indicatorBrush =
                    ResolveColor(s.chargingIndicator, nullptr, fillBrush);
                showIndicator = true;
            }
            break;
    }

    showIndicator = showIndicator && charging && !classic && !inside && !apple &&
                    !textOnly;

    // Modern outline.
    SetFontSizeIfChanged(icon.outline, fontSize);
    SetTextIfChanged(icon.outline,
                     inside ? winrt::hstring(std::wstring(1, kGlyphOutlineDischarging))
                            : winrt::hstring(outlineText));
    SetForeground(icon.outline, outlineBrush);
    SetVisible(icon.outline, !classic && !apple && !textOnly);

    // Charging bolt / plug, drawn over the outline and clipped to that area.
    if (showIndicator) {
        SetFontSizeIfChanged(icon.indicator, fontSize);
        SetTextIfChanged(icon.indicator, winrt::hstring(outlineText));
        SetForeground(icon.indicator, indicatorBrush);
        double w = fontSize * kModernAspect;
        Media::RectangleGeometry clip;
        clip.Rect(winrt::Windows::Foundation::Rect{
            (float)(w * kIndicatorLeft), 0.0f,
            (float)(w * (kIndicatorRight - kIndicatorLeft)),
            (float)(fontSize * kIndicatorBottom)});
        icon.indicator.Clip(clip);
    }
    SetVisible(icon.indicator, showIndicator);

    // Modern fill.
    SetFontSizeIfChanged(icon.fill, fontSize);
    SetTextIfChanged(icon.fill, winrt::hstring(fillText));
    SetForeground(icon.fill, fillBrush);
    SetVisible(icon.fill, !classic && !inside && !apple && !textOnly &&
                              sysFill.Visibility() == Visibility::Visible);

    // Apple style pill.
    if (apple) {
        UpdateAppleIcon(icon, content, state, percent,
                        LevelFromFillGlyph(fillText), fontSize);
    }
    SetVisible(icon.appleRoot, apple);

    // Percentage only.
    if (textOnly) {
        UpdateTextIcon(icon, content, state, percent);
    }
    SetVisible(icon.textRoot, textOnly);

    // Classic glyph.
    if (classic) {
        int level = percent >= 0 ? std::clamp((percent + 5) / 10, 0, 10)
                                 : LevelFromFillGlyph(fillText);
        wchar_t base = kClassicBattery0;
        if (charging) {
            base = kClassicCharging0;
        } else if (state == BatteryState::Saver) {
            base = kClassicSaver0;
        }
        if (inside) {
            base = kClassicBattery0;
            level = 0;
        }
        SetFontSizeIfChanged(icon.classic, fontSize);
        SetTextIfChanged(icon.classic,
                         winrt::hstring(std::wstring(1, (wchar_t)(base + level))));
        SetForeground(icon.classic, classicBrush);
    }
    SetVisible(icon.classic, classic);

    // Percentage inside the battery.
    auto fontWeight = winrt::Windows::UI::Text::FontWeight{
        (uint16_t)(s.percentBold ? 700 : 400)};
    if (inside) {
        double w = classic ? fontSize : fontSize * kModernAspect;
        double left = classic ? kClassicInteriorLeft : kModernInteriorLeft;
        double right = classic ? kClassicInteriorRight : kModernInteriorRight;
        icon.insideGrid.Width(w * (right - left));
        icon.insideGrid.Height(fontSize * (kInteriorBottom - kInteriorTop));
        icon.insideGrid.Margin(Thickness{w * left, fontSize * kInteriorTop, 0, 0});

        SetFontSizeIfChanged(icon.insideText, s.insideFontSize * s.scale / 100.0);
        icon.insideText.FontWeight(fontWeight);
        SetTextIfChanged(icon.insideText, winrt::to_hstring(percent));
        SetForeground(icon.insideText,
                      ResolveColor(s.percentColor,
                                   classic ? classicBrush : fillBrush,
                                   classic ? classicBrush : fillBrush));
    }
    SetVisible(icon.insideGrid, inside);

    // Percentage next to the battery.
    if (beside) {
        bool left = s.percentPosition == PercentPosition::left;
        auto children = stackPanel.Children();
        uint32_t percentIndex = 0, glyphIndex = 0;
        bool inPanel = children.IndexOf(icon.percent, percentIndex);
        bool glyphFound = children.IndexOf(glyphGrid, glyphIndex);
        bool placed = inPanel && glyphFound &&
                      (left ? percentIndex + 1 == glyphIndex
                            : percentIndex == glyphIndex + 1);
        if (!placed) {
            if (inPanel) {
                children.RemoveAt(percentIndex);
            }
            if (children.IndexOf(glyphGrid, glyphIndex)) {
                children.InsertAt(left ? glyphIndex : glyphIndex + 1,
                                  icon.percent);
            } else {
                children.Append(icon.percent);
            }
        }

        std::wstring text = std::to_wstring(percent);
        if (s.percentSign) {
            text += L'%';
        }
        SetFontSizeIfChanged(icon.percent, s.percentFontSize);
        icon.percent.FontWeight(fontWeight);
        SetTextIfChanged(icon.percent, winrt::hstring(text));
        SetForeground(icon.percent,
                      ResolveColor(s.percentColor, nullptr,
                                   classic ? classicBrush : fillBrush));
        double spacing = s.percentSpacing;
        icon.percent.Margin(left ? Thickness{0, 0, spacing, 0}
                                 : Thickness{spacing, 0, 0, 0});
    }
    SetVisible(icon.percent, beside);

    // Windows' own percentage text: hide it while the mod shows one.
    if (auto sysText = icon.sysText.get()) {
        if (s.percentPosition != PercentPosition::none ||
            (apple && s.appleNumber) || textOnly) {
            OverrideProperty(icon, sysText, UIElement::OpacityProperty(),
                             winrt::box_value(0.0));
            OverrideProperty(icon, sysText, FrameworkElement::MaxWidthProperty(),
                             winrt::box_value(0.0));
            OverrideProperty(icon, sysText, FrameworkElement::MarginProperty(),
                             winrt::box_value(Thickness{}));
        } else {
            RestoreProperty(icon, sysText, UIElement::OpacityProperty());
            RestoreProperty(icon, sysText, FrameworkElement::MaxWidthProperty());
            RestoreProperty(icon, sysText, FrameworkElement::MarginProperty());
        }
    }

    // Spacing and vertical offset.
    if (s.marginLeft || s.marginRight) {
        Thickness margin{(double)s.marginLeft, 0, (double)s.marginRight, 0};
        if (stackPanel.Margin() != margin) {
            OverrideProperty(icon, stackPanel, FrameworkElement::MarginProperty(),
                             winrt::box_value(margin));
        }
    } else {
        RestoreProperty(icon, stackPanel, FrameworkElement::MarginProperty());
    }

    if (s.verticalOffset) {
        auto current =
            stackPanel.ReadLocalValue(UIElement::RenderTransformProperty())
                .try_as<Media::TranslateTransform>();
        if (!current || current.Y() != s.verticalOffset) {
            Media::TranslateTransform transform;
            transform.Y(s.verticalOffset);
            OverrideProperty(icon, stackPanel, UIElement::RenderTransformProperty(),
                             transform);
        }
    } else {
        RestoreProperty(icon, stackPanel, UIElement::RenderTransformProperty());
    }

    UpdateAnimations(icon, state, percent, glyphGrid, stackPanel, classic,
                     apple, textOnly, inside);

    // Keep the percentage and low/critical colors up to date.
    if (NeedsPeriodicUpdate()) {
        if (!icon.timer) {
            BatteryIcon* iconPtr = &icon;
            icon.timer = DispatcherTimer();
            icon.timer.Interval(std::chrono::seconds(15));
            icon.timer.Tick([iconPtr](IInspectable const&, IInspectable const&) {
                UpdateBatteryIcon(*iconPtr);
            });
            icon.timer.Start();
        }
    } else if (icon.timer) {
        icon.timer.Stop();
        icon.timer = nullptr;
    }
}

void UpdateBatteryIcon(BatteryIcon& icon) {
    if (icon.dead || g_unloading) {
        return;
    }

    auto sysOutline = icon.sysOutline.get();
    auto sysFill = icon.sysFill.get();
    auto glyphGrid = icon.glyphGrid.get();
    auto stackPanel = icon.stackPanel.get();
    auto content = icon.content.get();
    if (!sysOutline || !sysFill || !glyphGrid || !stackPanel || !content) {
        Wh_Log(L"Battery icon is gone");
        DetachBatteryIcon(icon);
        return;
    }

    try {
        UpdateBatteryIconUnsafe(icon, content, sysOutline, sysFill, glyphGrid,
                                stackPanel);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Update failed: %08X %s", (unsigned)e.code(),
               e.message().c_str());
    }
}

void DetachBatteryIcon(BatteryIcon& icon) {
    if (icon.dead) {
        return;
    }
    icon.dead = true;

    try {
        if (icon.timer) {
            icon.timer.Stop();
            icon.timer = nullptr;
        }
        StopStoryboard(icon.oneShot);
        StopStoryboard(icon.oneShot2);
        StopLoop(icon);

        if (auto tb = icon.sysOutline.get()) {
            tb.UnregisterPropertyChangedCallback(
                Controls::TextBlock::TextProperty(), icon.outlineTextToken);
        }
        if (auto tb = icon.sysFill.get()) {
            tb.UnregisterPropertyChangedCallback(
                Controls::TextBlock::TextProperty(), icon.fillTextToken);
            tb.UnregisterPropertyChangedCallback(
                Controls::TextBlock::ForegroundProperty(),
                icon.fillForegroundToken);
            tb.UnregisterPropertyChangedCallback(UIElement::VisibilityProperty(),
                                                 icon.fillVisibilityToken);
        }
        if (icon.contentForegroundToken) {
            if (auto control =
                    icon.content.get().try_as<Controls::Control>()) {
                control.UnregisterPropertyChangedCallback(
                    Controls::Control::ForegroundProperty(),
                    icon.contentForegroundToken);
            }
        }

        auto removeFrom = [](Controls::Panel const& panel,
                             UIElement const& element) {
            if (!panel || !element) {
                return;
            }
            uint32_t index;
            if (panel.Children().IndexOf(element, index)) {
                panel.Children().RemoveAt(index);
            }
        };

        if (auto glyphGrid = icon.glyphGrid.get()) {
            removeFrom(glyphGrid, icon.outline);
            removeFrom(glyphGrid, icon.indicator);
            removeFrom(glyphGrid, icon.fill);
            removeFrom(glyphGrid, icon.classic);
            removeFrom(glyphGrid, icon.insideGrid);
            removeFrom(glyphGrid, icon.appleRoot);
            removeFrom(glyphGrid, icon.textRoot);
        }
        if (auto stackPanel = icon.stackPanel.get()) {
            removeFrom(stackPanel, icon.percent);
        }

        for (const auto& saved : icon.saved) {
            RestoreSavedProperty(saved);
        }
        icon.saved.clear();
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Detach failed: %08X %s", (unsigned)e.code(),
               e.message().c_str());
    }
}

Controls::TextBlock CreateTextBlock(PCWSTR name) {
    Controls::TextBlock tb;
    tb.Name(name);
    tb.IsHitTestVisible(false);
    Automation::AutomationProperties::SetAccessibilityView(
        tb, Automation::Peers::AccessibilityView::Raw);
    return tb;
}

Controls::TextBlock CreateGlyphTextBlock(PCWSTR name,
                                         Controls::TextBlock const& model) {
    auto tb = CreateTextBlock(name);
    tb.FontFamily(model.FontFamily());
    tb.FontSize(model.FontSize());
    tb.HorizontalAlignment(model.HorizontalAlignment());
    tb.VerticalAlignment(model.VerticalAlignment());
    tb.TextAlignment(model.TextAlignment());
    return tb;
}

bool IsModElementName(winrt::hstring const& name) {
    return std::wstring_view(name).starts_with(L"WhBattery");
}

void EnsureBatteryIcon(FrameworkElement batteryIconContent) {
    for (auto it = g_batteryIcons.begin(); it != g_batteryIcons.end();) {
        if (it->dead) {
            it = g_batteryIcons.erase(it);
            continue;
        }
        if (it->content.get() == batteryIconContent) {
            UpdateBatteryIcon(*it);
            return;
        }
        ++it;
    }

    // SystemTray.BatteryIconContent > Grid#ContainerGrid >
    // StackPanel#BatteryStackPanel > Grid#BatteryGlyphGrid > TextBlock x2
    FrameworkElement containerGrid =
        FindChildByName(batteryIconContent, L"ContainerGrid");
    if (!containerGrid) {
        Wh_Log(L"ContainerGrid not found");
        return;
    }

    FrameworkElement stackPanelElement =
        FindChildByName(containerGrid, L"BatteryStackPanel");
    if (!stackPanelElement) {
        stackPanelElement = FindChildByClassName(
            containerGrid, L"Windows.UI.Xaml.Controls.StackPanel");
    }
    auto stackPanel = stackPanelElement.try_as<Controls::StackPanel>();
    if (!stackPanel) {
        Wh_Log(L"Battery StackPanel not found");
        return;
    }

    FrameworkElement glyphGridElement =
        FindChildByName(stackPanel, L"BatteryGlyphGrid");
    if (!glyphGridElement) {
        glyphGridElement =
            FindChildByClassName(stackPanel, L"Windows.UI.Xaml.Controls.Grid");
    }
    auto glyphGrid = glyphGridElement.try_as<Controls::Grid>();
    if (!glyphGrid) {
        Wh_Log(L"Battery glyph Grid not found");
        return;
    }

    std::vector<Controls::TextBlock> systemTextBlocks;
    EnumChildElements(glyphGrid, [&](FrameworkElement child) {
        auto tb = child.try_as<Controls::TextBlock>();
        if (tb && !IsModElementName(tb.Name())) {
            systemTextBlocks.push_back(tb);
        }
        return false;
    });
    if (systemTextBlocks.size() != 2) {
        Wh_Log(L"Unexpected battery layout: %zu glyph layers",
               systemTextBlocks.size());
        return;
    }

    auto sysOutline = systemTextBlocks[0];
    auto sysFill = systemTextBlocks[1];
    auto sysText =
        FindChildByName(stackPanel, L"BatteryTextBlock").try_as<Controls::TextBlock>();

    Wh_Log(L"Attaching to battery icon");

    auto& icon = g_batteryIcons.emplace_back();
    icon.content = batteryIconContent;
    icon.stackPanel = stackPanel;
    icon.glyphGrid = glyphGrid;
    icon.sysOutline = sysOutline;
    icon.sysFill = sysFill;
    if (sysText) {
        icon.sysText = sysText;
    }
    icon.baseFontSize = sysOutline.FontSize() > 0 ? sysOutline.FontSize() : 16;

    try {
        icon.outline = CreateGlyphTextBlock(L"WhBatteryOutline", sysOutline);
        icon.indicator = CreateGlyphTextBlock(L"WhBatteryIndicator", sysOutline);
        icon.indicator.HorizontalAlignment(HorizontalAlignment::Center);
        icon.indicator.TextAlignment(TextAlignment::Left);
        icon.fill = CreateGlyphTextBlock(L"WhBatteryFill", sysFill);

        icon.classic = CreateTextBlock(L"WhBatteryClassic");
        icon.classic.FontFamily(Media::FontFamily(L"Segoe Fluent Icons"));
        icon.classic.HorizontalAlignment(HorizontalAlignment::Center);
        icon.classic.VerticalAlignment(VerticalAlignment::Center);
        icon.classic.TextAlignment(TextAlignment::Center);

        auto textFont = sysText ? sysText.FontFamily()
                                : Media::FontFamily(L"Segoe UI Variable Text");

        icon.insideGrid = Controls::Grid();
        icon.insideGrid.Name(L"WhBatteryInsideGrid");
        icon.insideGrid.IsHitTestVisible(false);
        icon.insideGrid.HorizontalAlignment(HorizontalAlignment::Left);
        icon.insideGrid.VerticalAlignment(VerticalAlignment::Top);
        icon.insideText = CreateTextBlock(L"WhBatteryInsideText");
        icon.insideText.FontFamily(textFont);
        icon.insideText.HorizontalAlignment(HorizontalAlignment::Center);
        icon.insideText.VerticalAlignment(VerticalAlignment::Center);
        icon.insideText.TextAlignment(TextAlignment::Center);
        icon.insideText.TextLineBounds(TextLineBounds::Tight);
        icon.insideGrid.Children().Append(icon.insideText);

        icon.percent = CreateTextBlock(L"WhBatteryPercent");
        icon.percent.FontFamily(textFont);
        icon.percent.VerticalAlignment(VerticalAlignment::Center);
        icon.percent.TextLineBounds(TextLineBounds::Tight);
        icon.percent.Visibility(Visibility::Collapsed);

        icon.appleRoot = Controls::StackPanel();
        icon.appleRoot.Name(L"WhBatteryApple");
        icon.appleRoot.IsHitTestVisible(false);
        icon.appleRoot.Orientation(Controls::Orientation::Horizontal);
        icon.appleRoot.HorizontalAlignment(HorizontalAlignment::Center);
        icon.appleRoot.VerticalAlignment(VerticalAlignment::Center);
        icon.appleRoot.Visibility(Visibility::Collapsed);
        icon.appleBody = Controls::Border();
        icon.appleBody.VerticalAlignment(VerticalAlignment::Center);
        // Two copies of the number, each clipped to one side of the fill
        // edge, give an exact "cut out" look.
        Controls::Grid appleTextGrid;
        for (auto* tb : {&icon.appleText, &icon.appleTextTrack}) {
            *tb = CreateTextBlock(L"WhBatteryAppleText");
            tb->FontFamily(textFont);
            tb->HorizontalAlignment(HorizontalAlignment::Stretch);
            tb->VerticalAlignment(VerticalAlignment::Center);
            tb->TextAlignment(TextAlignment::Center);
            tb->TextLineBounds(TextLineBounds::Tight);
            appleTextGrid.Children().Append(*tb);
        }
        icon.appleBody.Child(appleTextGrid);
        icon.appleNub = Controls::Border();
        icon.appleNub.VerticalAlignment(VerticalAlignment::Center);
        icon.appleBolt = Shapes::Polygon();
        icon.appleBolt.VerticalAlignment(VerticalAlignment::Center);
        icon.appleBolt.Visibility(Visibility::Collapsed);
        icon.appleRoot.Children().Append(icon.appleBody);
        icon.appleRoot.Children().Append(icon.appleNub);
        icon.appleRoot.Children().Append(icon.appleBolt);

        auto children = glyphGrid.Children();
        children.Append(icon.outline);
        children.Append(icon.fill);
        children.Append(icon.indicator);
        children.Append(icon.classic);
        children.Append(icon.insideGrid);
        children.Append(icon.appleRoot);

        icon.textRoot = Controls::StackPanel();
        icon.textRoot.Name(L"WhBatteryText");
        icon.textRoot.IsHitTestVisible(false);
        icon.textRoot.Orientation(Controls::Orientation::Horizontal);
        icon.textRoot.HorizontalAlignment(HorizontalAlignment::Center);
        icon.textRoot.VerticalAlignment(VerticalAlignment::Center);
        icon.textRoot.Visibility(Visibility::Collapsed);
        icon.textBoltBefore = Shapes::Polygon();
        icon.textBoltAfter = Shapes::Polygon();
        for (auto& bolt : {icon.textBoltBefore, icon.textBoltAfter}) {
            bolt.VerticalAlignment(VerticalAlignment::Center);
            bolt.Visibility(Visibility::Collapsed);
        }
        icon.textPill = Controls::Border();
        icon.textPill.VerticalAlignment(VerticalAlignment::Center);
        icon.textNumber = CreateTextBlock(L"WhBatteryTextNumber");
        icon.textNumber.VerticalAlignment(VerticalAlignment::Center);
        icon.textNumber.TextLineBounds(TextLineBounds::Tight);
        icon.textPill.Child(icon.textNumber);
        icon.textRoot.Children().Append(icon.textBoltBefore);
        icon.textRoot.Children().Append(icon.textPill);
        icon.textRoot.Children().Append(icon.textBoltAfter);
        children.Append(icon.textRoot);

        BatteryIcon* iconPtr = &icon;
        auto onChange = [iconPtr](DependencyObject const&,
                                  DependencyProperty const&) {
            UpdateBatteryIcon(*iconPtr);
        };
        icon.outlineTextToken = sysOutline.RegisterPropertyChangedCallback(
            Controls::TextBlock::TextProperty(), onChange);
        icon.fillTextToken = sysFill.RegisterPropertyChangedCallback(
            Controls::TextBlock::TextProperty(), onChange);
        icon.fillForegroundToken = sysFill.RegisterPropertyChangedCallback(
            Controls::TextBlock::ForegroundProperty(), onChange);
        icon.fillVisibilityToken = sysFill.RegisterPropertyChangedCallback(
            UIElement::VisibilityProperty(), onChange);
        // Light/dark taskbar switches change the text color.
        if (auto control = batteryIconContent.try_as<Controls::Control>()) {
            icon.contentForegroundToken = control.RegisterPropertyChangedCallback(
                Controls::Control::ForegroundProperty(), onChange);
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Attach failed: %08X %s", (unsigned)e.code(),
               e.message().c_str());
        DetachBatteryIcon(icon);
        return;
    }

    UpdateBatteryIcon(icon);
}

void ApplyToTrayFrame(FrameworkElement systemTrayFrameGrid) {
    auto batteryIconContent = FindDescendantByClassName(
        systemTrayFrameGrid, L"SystemTray.BatteryIconContent", 14);
    if (batteryIconContent) {
        EnsureBatteryIcon(batteryIconContent);
    } else {
        Wh_Log(L"No battery icon found in the tray");
    }
}

////////////////////////////////////////////////////////////////////////////////
// Taskbar plumbing

std::atomic<bool> g_systemTrayModuleHooked;

using FrameworkElementLoadedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement,
    &winrt::impl::abi<IFrameworkElement>::type::remove_Loaded>;

std::list<FrameworkElementLoadedEventRevoker> g_autoRevokerList;

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

void* CTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

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

// Runs on the taskbar thread.
void ApplySettingsFromTaskbarThread(HWND hTaskbarWnd, bool reloadSettings) {
    g_autoRevokerList.clear();

    if (reloadSettings) {
        LoadSettings();
    }

    for (auto& icon : g_batteryIcons) {
        if (g_unloading) {
            DetachBatteryIcon(icon);
        } else {
            if (reloadSettings && !icon.dead) {
                // Restart looping animations with the new settings.
                StopLoop(icon);
            }
            UpdateBatteryIcon(icon);
            if (reloadSettings && !icon.dead && g_settings.animationPreview) {
                if (auto glyphGrid = icon.glyphGrid.get()) {
                    try {
                        PlayChargerAnimation(icon, glyphGrid, true, true);
                    } catch (winrt::hresult_error const& e) {
                        Wh_Log(L"Preview failed: %08X", (unsigned)e.code());
                    }
                }
            }
        }
    }
    std::erase_if(g_batteryIcons, [](const BatteryIcon& icon) { return icon.dead; });

    if (g_unloading) {
        return;
    }

    auto xamlRoot = GetTaskbarXamlRoot(hTaskbarWnd);
    if (!xamlRoot) {
        Wh_Log(L"Getting XamlRoot failed");
        return;
    }

    FrameworkElement child = xamlRoot.Content().try_as<FrameworkElement>();
    if (child &&
        (child = FindChildByClassName(child, L"SystemTray.SystemTrayFrame")) &&
        (child = FindChildByName(child, L"SystemTrayFrameGrid"))) {
        ApplyToTrayFrame(child);
    } else {
        Wh_Log(L"SystemTrayFrameGrid not found");
    }
}

void ApplySettings(bool reloadSettings) {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        Wh_Log(L"No taskbar found");
        return;
    }

    struct Param {
        HWND hTaskbarWnd;
        bool reloadSettings;
    } param{hTaskbarWnd, reloadSettings};

    RunFromWindowThread(
        hTaskbarWnd,
        [](void* pParam) {
            auto& param = *(Param*)pParam;
            try {
                ApplySettingsFromTaskbarThread(param.hTaskbarWnd,
                                               param.reloadSettings);
            } catch (winrt::hresult_error const& e) {
                Wh_Log(L"Apply failed: %08X %s", (unsigned)e.code(),
                       e.message().c_str());
            }
        },
        &param);
}

////////////////////////////////////////////////////////////////////////////////
// Hooks

using IconView_IconView_t = void*(WINAPI*)(void* pThis);
IconView_IconView_t IconView_IconView_Original;
void* WINAPI IconView_IconView_Hook(void* pThis) {
    void* ret = IconView_IconView_Original(pThis);

    FrameworkElement iconView = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(iconView));
    if (!iconView) {
        return ret;
    }

    g_autoRevokerList.emplace_back();
    auto autoRevokerIt = g_autoRevokerList.end();
    --autoRevokerIt;

    *autoRevokerIt = iconView.Loaded(
        winrt::auto_revoke_t{},
        [autoRevokerIt](IInspectable const& sender, RoutedEventArgs const& e) {
            g_autoRevokerList.erase(autoRevokerIt);

            if (g_unloading) {
                return;
            }

            auto iconView = sender.try_as<FrameworkElement>();
            if (!iconView || iconView.Name() != L"SystemTrayIcon") {
                return;
            }

            try {
                auto batteryIconContent = FindDescendantByClassName(
                    iconView, L"SystemTray.BatteryIconContent", 4);
                if (batteryIconContent) {
                    Wh_Log(L"Battery icon view loaded");
                    EnsureBatteryIcon(batteryIconContent);
                }
            } catch (winrt::hresult_error const& e) {
                Wh_Log(L"Loaded handler failed: %08X %s", (unsigned)e.code(),
                       e.message().c_str());
            }
        });

    return ret;
}

bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
            &IconView_IconView_Original,
            IconView_IconView_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

// SystemTray.dll hosts winrt::SystemTray::* on newer builds; older builds
// have it in Taskbar.View.dll.
HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            VS_FIXEDFILEINFO* fixedFileInfo =
                GetModuleVersionInfo(module, nullptr);
            WORD moduleMajor =
                fixedFileInfo ? HIWORD(fixedFileInfo->dwFileVersionMS) : 0;
            if (!moduleMajor || moduleMajor >= 2604) {
                module = nullptr;
            }
        }
    }

    return module;
}

void HandleLoadedModuleIfSystemTray(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_systemTrayModuleHooked && GetSystemTrayModuleHandle() == module &&
        !g_systemTrayModuleHooked.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookSystemTraySymbols(module)) {
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
        HandleLoadedModuleIfSystemTray(module, lpLibFileName);
    }

    return module;
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
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

////////////////////////////////////////////////////////////////////////////////
// Mod entry points

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
        g_systemTrayModuleHooked = true;
        if (!HookSystemTraySymbols(systemTrayModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"System tray module not loaded yet");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_systemTrayModuleHooked) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                if (HookSystemTraySymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    ApplySettings(/*reloadSettings=*/false);
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    ApplySettings(/*reloadSettings=*/false);
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    ApplySettings(/*reloadSettings=*/true);
}
