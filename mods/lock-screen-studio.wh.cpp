// ==WindhawkMod==
// @id              lock-screen-studio
// @name            Lock Screen Studio
// @description     Make your lock screen your own with clock, weather and media themes, custom fonts and flexible layouts.
// @version         0.0.1
// @author          Mitsu
// @github          https://github.com/MitsuSarkar
// @license         MIT
// @include         LockApp.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Lock Screen Studio

Your lock screen, your layout.

I wanted more choice than the standard Windows lock screen, so I made Lock Screen Studio: a set of clock, weather and media themes with the controls to make them your own. Keep it clean, build a desktop-style layout, or go all in on pixel art.

## A little inspiration

### Pixel clock and weather

An upper-right pixel clock and date, paired with a compact weather card on the left. The card uses pixel lettering, angular weather icons and a stepped border.

![Pixel clock and weather with a stepped card border](https://raw.githubusercontent.com/MitsuSarkar/lock-screen-studio/main/docs/screenshots/pixel-lock-screen.png)

### Modern Flip

A large split-flap clock in the center, with the date underneath, a dark weather card above and media controls along the bottom.

![Modern Flip clock with a dark weather card and media controls](https://raw.githubusercontent.com/MitsuSarkar/lock-screen-studio/main/docs/screenshots/modern-flip.png)

### Sonoma

A large, clean clock with a separate date pill, a dark weather card and compact media controls.

![Sonoma clock with a date pill, weather and media controls](https://raw.githubusercontent.com/MitsuSarkar/lock-screen-studio/main/docs/screenshots/sonoma.png)

These are screenshots of my setup. The wallpaper is set separately in Windows and isn't included with the mod. Colors and layout can be changed in Settings.

## Pick your starting point

**Clock themes:** Pixel Coffee, Dusk, Sky, Night, Sakura, Rain, Hollow and Sunset; GNOME Minimal, KDE Breeze, Hyprland Terminal, Nord, Catppuccin Mocha and Gruvbox; LED Matrix, Modern Flip, Vintage Flip, Sonoma, Cyberpunk and Aura. You can also keep the Windows clock.

**Weather/widget and media themes:** Cozy Pixel, Mac Light, Mac Dark, Smoke, Minimal Outline, Nord, Catppuccin and Gruvbox, plus Windows default.

Nord, Catppuccin Mocha and Gruvbox use their published palettes. Mac Light and Mac Dark are inspired by macOS. Each Studio widget theme starts with its own colors, border, opacity and corners. Turn on **Customize widget theme colors and surface** when you want to use your own values.

## Make it yours

- **Clock and date:** choose separate fonts, clock size, 12/24-hour time and seconds. Studio layouts support separate anchors and X/Y offsets for the clock and date, plus date formatting, color and panel controls. The eight Pixel layouts include embedded pixel lettering, so no font installation is needed.
- **Weather and widgets:** use Studio cards for free positioning outside Windows' widget container. Choose an anchor, X/Y offsets, card width, font, text size and screen-edge padding. Customize background, foreground, accent, opacity and corners, or keep the theme defaults.
- **Weather icons:** automatic sun, cloud, rain, storm, snow, sleet, fog, wind and night icons when Windows supplies a recognized condition. Pixel uses angular artwork; other themes use matching colors and finishes. Show or remove the icon, choose a side or corner, and adjust its size and X/Y offsets. A manual condition override is available too.
- **Media:** choose a card theme, font, colors, opacity, corners and text size. Move or hide the media panel while keeping Windows' album art and playback controls.
- **Finishing touches:** add battery and static text elements to Studio layouts, adjust wallpaper dimming, or hide the native widgets. Advanced element rules can change the position, visibility, opacity and supported text properties of named Windows elements.

Colors accept `#RRGGBB` or `#AARRGGBB` (alpha first). Positive X moves right; positive Y moves down. Custom fonts must already be installed and available to the lock screen. Some decorative clock themes keep their own layout and styling.

## Getting started

1. Enable the mod and open its **Settings** tab.
2. Pick a clock theme and a weather/widget theme. Choose **Studio cards** under **Widget rendering mode** for the replacement weather cards and their positioning/icon controls.
3. Keep Weather enabled in Windows' lock-screen settings: Windows still provides the data. Set your wallpaper there too.
4. Lock your PC to see your layout. Start with zero offsets, then adjust the placement to suit your screen.

Selecting **Windows default** for widgets restores the actual Windows card. Studio icon and placement controls apply to the replacement cards. Avoid running another lock-screen clock or styling mod at the same time.

## Compatibility and details

Tested on my Windows 11 26H2 setup with Windhawk 1.7.3, x64. Windows updates may change the lock-screen elements this mod relies on.

Studio cards mirror Windows' available widget text and images; they aren't a separate weather service. Forecast/menu buttons aren't copied. Automatic themed icons depend on readable English condition descriptions; if none are available, the original image is used when possible, otherwise a neutral thermometer appears. Native widget/media discovery can vary by Windows build.

This mod customizes the lock screen, not the password/PIN sign-in screen. Wallpaper selection stays in Windows Settings; animated wallpapers aren't included. Disable the mod to restore the standard appearance.

## Credits and licenses

Lock Screen Studio is maintained by **Mitsu**. It builds on the MIT-licensed Lock Screen Clock Styler engine. The original copyright and permission notice are retained in the source. Pixelify Sans and its embedded outlines retain their SIL Open Font License 1.1 notices. Code: MIT; font data: OFL-1.1.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- clockStyle: nord
  $name: Theme
  $options:
    - pixelCoffee: Pixel Coffee (bottom-left)
    - pixelDusk: Pixel Dusk (top-left)
    - pixelSky: Pixel Sky (top-right)
    - pixelNight: Pixel Night (two-tone)
    - pixelSakura: Pixel Sakura (light)
    - pixelRain: Pixel Rain (cool blue)
    - pixelHollow: Pixel Hollow (bottom strip)
    - pixelSunset: Pixel Sunset (warm shadow)
    - gnome: GNOME Minimal
    - kde: KDE Breeze
    - hyprland: Hyprland Terminal
    - nord: Nord
    - catppuccin: Catppuccin Mocha
    - gruvbox: Gruvbox
    - pixel: Original LED Matrix
    - flip: Original Modern Flip
    - vintage: Original Vintage Flip
    - macos: Original Sonoma
    - cyberpunk: Original Cyberpunk
    - aura: Original Aura
    - defaultOnly: Windows stock clock
- timeFormat: 24h
  $options:
    - 24h: 24 hour
    - 12h: 12 hour
- showSeconds: false
- clockFontSize: 140
- fontFamily: ""
  $name: Legacy shared font family
  $description: Shared fallback for typography themes. Prefer the independent clock/date font fields below.
- clockFontFamily: ""
  $name: Clock font family
  $description: Type an installed font family, e.g. Cascadia Code. Empty uses the theme default. Overrides the embedded font in the eight Pixel themes.
- dateFontFamily: ""
  $name: Date font family
  $description: Independent installed font family for the date. Empty uses the theme default.
- widgetFontFamily: ""
  $name: Weather and widget font family
  $description: Type an installed font family. Empty uses native/pixel theme text. Install custom fonts for all users so LockApp can read them.
- mediaFontFamily: ""
  $name: Media font family
  $description: Independent installed font family for song and artist labels. Icons retain their symbol fonts.
- fontWeight: 300
  $description: Studio clock weight, 100 to 900.
- foreground: ""
  $description: Studio clock color. Empty uses theme default.
- accent: ""
  $description: Studio date and text color. Empty uses theme default.
- panelColor: ""
  $description: Studio card background. Empty uses theme default.
- panelOpacity: 72
  $description: Studio card opacity, 0 to 100. GNOME starts without a card.
- cornerRadius: 24
- backgroundDim: 15
  $description: Studio wallpaper darkening, 0 to 90 percent.
- clockAnchor: theme
  $description: theme, topLeft, top, topRight, left, center, right, bottomLeft, bottom, bottomRight.
- clockX: 0
- clockY: 0
- dateAnchor: theme
- dateX: 0
- dateY: 0
- dateFontSize: 26
- dateFormat: "dddd, d MMMM"
  $description: Windows date format, e.g. ddd, dd MMM yyyy. Empty uses long date.
- pixelDateLocale: en-US
  $description: Locale for the eight Pixel themes. Use system for your Windows locale. Font includes Latin and Cyrillic; unsupported characters fall back to a system TextBlock.
- pixelDateTracking: 2
  $description: Extra spacing between pixel date characters, in DIPs, 0 to 12.
- pixelShadow: 2
  $description: Pixel clock and date shadow offset, in DIPs, 0 to 8.
- showDateCapsule: true
- showBatteryStatus: true
- batteryAnchor: bottom
- batteryX: 0
- batteryY: -60
- customText: ""
  $description: Optional static text widget; visible while locked.
- textAnchor: bottom
- textX: 0
- textY: -110
- textFontSize: 20
- hideDefaultWidgets: false
- widgetMode: native
  $name: Widget rendering mode
  $options:
    - native: Style Windows widgets (legacy movement)
    - studio: Studio cards (free positioning, mirrors Windows text)
  $description: Studio cards draw outside Windows' clipped widget container and hide its original visuals after data is available. Windows remains the data source; its buttons are not copied.
- widgetAnchor: bottom
  $name: Studio widget anchor
  $options:
    - topLeft: Top left
    - top: Top center
    - topRight: Top right
    - left: Center left
    - center: Center
    - right: Center right
    - bottomLeft: Bottom left
    - bottom: Bottom center
    - bottomRight: Bottom right
- widgetCardWidth: 320
  $description: Studio card width in DIPs, 180 to 600. The row scales down to fit smaller displays.
- weatherIconVisible: true
  $name: Show weather icon (turn off to remove)
- weatherIconCondition: auto
  $name: Weather icon condition
  $description: Auto reads Windows condition labels and accessibility descriptions. If Windows supplies neither, keeps its original image when available. Manual choices are overrides, not live forecasts.
  $options:
    - auto: Automatic from Windows
    - sun: Sunny
    - partly: Partly cloudy (day)
    - cloud: Cloudy
    - rain: Rain
    - storm: Thunderstorm
    - snow: Snow
    - sleet: Sleet
    - fog: Fog
    - wind: Wind
    - night: Clear night
    - partlyNight: Partly cloudy (night)
- weatherIconPosition: left
  $name: Weather icon placement inside Studio card
  $options:
    - left: Left of temperature and details
    - right: Right of temperature and details
    - topLeft: Top left
    - top: Top center
    - topRight: Top right
    - bottomLeft: Bottom left
    - bottom: Bottom center
    - bottomRight: Bottom right
- weatherIconX: 0
  $description: Horizontal icon offset in DIPs. Positive moves right. Large offsets can put the icon outside its card.
- weatherIconY: 0
  $description: Vertical icon offset in DIPs. Positive moves down.
- weatherIconSize: 72
  $description: Studio weather icon size in DIPs, 24 to 160. These icon options apply to Studio cards mode.
- widgetX: 0
  $description: Offset the native widget group when found; can move it off screen.
- widgetY: 0
- widgetTarget: auto
  $description: Auto detects WidgetGroupPanel and older containers. Override with an exact XAML name from diagnostics if Windows changes it.
- widgetTheme: macosDark
  $name: Weather and widget card theme
  $options:
    - stock: Windows default
    - pixel: Cozy Pixel
    - macos: Mac Light
    - macosDark: Mac Dark
    - smoke: Smoke
    - minimal: Minimal Outline
    - nord: Nord
    - catppuccin: Catppuccin
    - gruvbox: Gruvbox
- widgetBackground: ""
- widgetCustomStyle: false
  $name: Customize widget theme colors and surface
  $description: Off uses the selected theme's original palette, opacity, borders and corners. Turn on to apply your saved background, foreground, accent, opacity and corner radius. Fonts, position and icon settings work independently. Windows original restores the actual Windows card.
- widgetForeground: ""
- widgetAccent: ""
- widgetOpacity: 92
- widgetCornerRadius: 24
- widgetTextSize: 0
  $description: 0 keeps native text sizes and weather temperature hierarchy; otherwise 10 to 40 DIPs.
- widgetKeepOnScreen: true
  $description: Clamp the widget group within the screen instead of allowing it to disappear beyond an edge.
- widgetMoveContainer: outer
  $description: outer moves the whole widget-only viewport to avoid inner clipping; panel uses legacy direct movement.
  $options:
    - outer: Outer widget container (recommended)
    - panel: Inner widget panel (legacy)
- widgetScreenPadding: 16
  $description: Minimum space from screen edges, 0 to 100 DIPs.
- hideMedia: false
- mediaX: 0
- mediaY: 0
- mediaTheme: smoke
  $name: Media card theme
  $options:
    - stock: Windows default
    - smoke: Smoke card
    - minimal: Minimal outline
    - pixel: Cozy pixel card
    - macos: Mac Light
    - macosDark: Mac Dark
    - nord: Nord
    - catppuccin: Catppuccin
    - gruvbox: Gruvbox
- mediaBackground: ""
  $description: "Optional #RRGGBB or #AARRGGBB override."
- mediaForeground: ""
- mediaAccent: ""
- mediaOpacity: 88
  $description: Card background opacity, 0 to 100. Playback controls stay opaque.
- mediaCornerRadius: 18
  $description: Smoke/minimal card corners. Pixel theme always uses square corners.
- mediaTextSize: 16
  $description: Text size inside the native media card, 10 to 40.
- diagnostics: false
  $description: Log newly discovered visual tree names, including widgets loaded later, to identify build changes.
- elementRules:
  - - name: ""
    - x: 0
    - y: 0
    - opacity: 100
    - hide: false
    - fontSize: 0
    - color: ""
  $description: Advanced native element rules by exact XAML name. Leave first name empty to disable. Do not target the root or credential UI. Use diagnostics to discover names. fontSize/color affect TextBlocks only.
- hideDefaultClock: true
- clockAlignment: auto
  $description: Original designs only. auto, center, left, right, bottomLeft.
- verticalOffset: 0
  $description: Original designs only.
- glassOpacity: 45
  $description: Original Flip design only, 0 to 255.
*/
// ==/WindhawkModSettings==

// Derived from Lock Screen Clock Styler v0.5.0, Copyright (c) 2026 ali0zdas.
/*
MIT License
Copyright (c) 2026 ali0zdas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <windows.h>
#include <tlhelp32.h>
#undef GetCurrentTime
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Documents.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <string>
#include <vector>

namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxd = winrt::Windows::UI::Xaml::Documents;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuxs = winrt::Windows::UI::Xaml::Shapes;
namespace wu = winrt::Windows::UI;

// Mod Configuration Settings
bool IsStudioPixelStyle(std::wstring const& style) {
    return style == L"pixelCoffee" || style == L"pixelDusk" || style == L"pixelSky" || style == L"pixelNight" || style == L"pixelSakura" || style == L"pixelRain" || style == L"pixelHollow" || style == L"pixelSunset";
}
bool IsStudioStyle(std::wstring const& style) {
    return IsStudioPixelStyle(style) || style == L"gnome" || style == L"kde" || style == L"hyprland" ||
           style == L"nord" || style == L"catppuccin" || style == L"gruvbox";
}
struct ModSettings {
    std::wstring clockStyle;
    std::wstring timeFormat;
    bool showSeconds;
    int glassOpacity;
    int clockFontSize;
    int verticalOffset;
    std::wstring clockAlignment;
    bool hideDefaultWidgets;
    bool hideDefaultClock;
    bool showDateCapsule;
    bool showBatteryStatus;
} g_settings;

// Loads every setting. Before the UI hook is installed this may run on any
// thread; afterwards it must run on the lock screen UI thread.
void LoadStudioSettings();
void LoadSettings() {
    LoadStudioSettings();
    auto style = WindhawkUtils::StringSetting::make(L"clockStyle");
    g_settings.clockStyle = (style.get() && *style.get()) ? style.get() : L"pixel";
    if (g_settings.clockStyle != L"pixel" && g_settings.clockStyle != L"flip" &&
        g_settings.clockStyle != L"vintage" && g_settings.clockStyle != L"macos" &&
        g_settings.clockStyle != L"cyberpunk" && g_settings.clockStyle != L"aura" &&
        g_settings.clockStyle != L"defaultOnly" && !IsStudioStyle(g_settings.clockStyle)) {
        g_settings.clockStyle = L"pixel";
    }

    auto fmt = WindhawkUtils::StringSetting::make(L"timeFormat");
    g_settings.timeFormat = (fmt.get() && *fmt.get()) ? fmt.get() : L"24h";
    if (g_settings.timeFormat != L"24h" && g_settings.timeFormat != L"12h") {
        g_settings.timeFormat = L"24h";
    }

    // The settings UI is the single source of truth for seconds. The flip
    // clocks have no seconds display, so the toggle is ignored for them.
    bool supportsSeconds =
        (g_settings.clockStyle != L"flip" && g_settings.clockStyle != L"vintage");
    g_settings.showSeconds = supportsSeconds && Wh_GetIntSetting(L"showSeconds") != 0;

    g_settings.glassOpacity = Wh_GetIntSetting(L"glassOpacity");
    if (g_settings.glassOpacity < 0) g_settings.glassOpacity = 0;
    if (g_settings.glassOpacity > 255) g_settings.glassOpacity = 255;

    int fontSetting = Wh_GetIntSetting(L"clockFontSize");
    if (fontSetting == 0) fontSetting = 140;
    if (fontSetting < 20) fontSetting = 20;
    if (fontSetting > 300) fontSetting = 300;
    g_settings.clockFontSize = fontSetting;

    g_settings.verticalOffset = Wh_GetIntSetting(L"verticalOffset");
    if (g_settings.verticalOffset < -200) g_settings.verticalOffset = -200;
    if (g_settings.verticalOffset > 400) g_settings.verticalOffset = 400;

    auto align = WindhawkUtils::StringSetting::make(L"clockAlignment");
    std::wstring rawAlign = (align.get() && *align.get()) ? align.get() : L"auto";
    if (rawAlign != L"auto" && rawAlign != L"center" && rawAlign != L"bottomLeft" &&
        rawAlign != L"left" && rawAlign != L"right") {
        rawAlign = L"auto";
    }

    // "auto" maps to the theme default; an explicit choice is always honored.
    g_settings.clockAlignment = (rawAlign == L"auto")
        ? (g_settings.clockStyle == L"macos" ? L"bottomLeft" : L"center")
        : rawAlign;

    g_settings.hideDefaultWidgets = Wh_GetIntSetting(L"hideDefaultWidgets") != 0;
    g_settings.hideDefaultClock = Wh_GetIntSetting(L"hideDefaultClock") != 0;
    g_settings.showDateCapsule = Wh_GetIntSetting(L"showDateCapsule") != 0;
    g_settings.showBatteryStatus = Wh_GetIntSetting(L"showBatteryStatus") != 0;

    Wh_Log(L"Settings loaded: style=%s, fmt=%s, showSec=%d, opacity=%d, font=%d, vertOffset=%d, align=%s, hideWidgets=%d, hideClock=%d, showBattery=%d",
           g_settings.clockStyle.c_str(), g_settings.timeFormat.c_str(), g_settings.showSeconds, g_settings.glassOpacity,
           g_settings.clockFontSize, g_settings.verticalOffset, g_settings.clockAlignment.c_str(), g_settings.hideDefaultWidgets, g_settings.hideDefaultClock, g_settings.showBatteryStatus);
}

// Strong XAML references. These belong to the lock screen UI thread and must
// never be released by CRT static destructors at process exit. They are marked
// no_destroy and released manually from the UI thread in TeardownOnUiThread().
[[clang::no_destroy]] wuxc::Grid g_lockRootGrid{ nullptr };
[[clang::no_destroy]] wux::UIElement g_activeClockElement{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_hourTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_hourBotTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_minTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_minBotTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_secTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_ampmTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_auraTimeTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_auraShadowTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_dateTextBlock{ nullptr };

// Minimalist Sonoma (Clean Typography) Theme State
[[clang::no_destroy]] wuxc::TextBlock g_macosTimeTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_macosShadowTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_macosDateTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_macosDateShadowBlock{ nullptr };

// Battery Capsule State
[[clang::no_destroy]] wuxc::Border g_batteryCapsule{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_batteryIconText{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_batteryStatusText{ nullptr };

// Cyberpunk HUD Pulsing State
[[clang::no_destroy]] wuxc::TextBlock g_cyberSlash1{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_cyberSlash2{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_cyberLiveLabel{ nullptr };

// Elements whose Visibility this mod changed. The original local value is kept
// so it can be restored without overwriting Windows' own bindings/state.
struct TrackedElement {
    wux::FrameworkElement element{ nullptr };
    winrt::Windows::Foundation::IInspectable originalValue{ nullptr };
    bool hadLocalValue = false;
};

// no_destroy: releasing these vectors in a CRT destructor at process exit would
// release XAML objects off the UI thread. They are cleared on the UI thread.
[[clang::no_destroy]] std::vector<TrackedElement> g_hiddenClockElements;
[[clang::no_destroy]] std::vector<TrackedElement> g_hiddenWidgetElements;

// Dirty-checking time cache for high-efficiency rendering (eliminates redundant DirectWrite invalidations)
struct TimeCache {
    int lastDisplayHour = -1;
    int lastMinute = -1;
    int lastSecond = -1;
    int lastDay = -1;
    int lastMonth = -1;
    int lastYear = -1;
    std::wstring lastAmPm;
    std::wstring lastAuraTime;
    std::wstring lastDateStr;
    std::wstring lastMacosTime;
    std::wstring lastMacosDate;
    int lastBatteryPct = -1;
    bool lastIsCharging = false;

    void Invalidate() {
        lastDisplayHour = -1;
        lastMinute = -1;
        lastSecond = -1;
        lastDay = -1;
        lastMonth = -1;
        lastYear = -1;
        lastAmPm.clear();
        lastAuraTime.clear();
        lastDateStr.clear();
        lastMacosTime.clear();
        lastMacosDate.clear();
        lastBatteryPct = -1;
        lastIsCharging = false;
    }
} g_timeCache;

// 7x11 Dot Matrix Font Table for Digits 0 to 9 with 2-Dot Stroke Thickness
// Each digit consists of 11 rows; each row uses the lower 7 bits (bit 6 = col 0, bit 0 = col 6).
static const uint8_t FONT_7x11[10][11] = {
    // 0
    { 0b0111110,
      0b1111111,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1111111,
      0b0111110 },
    // 1
    { 0b0011100,
      0b0111100,
      0b0001100,
      0b0001100,
      0b0001100,
      0b0001100,
      0b0001100,
      0b0001100,
      0b0001100,
      0b0111110,
      0b0111110 },
    // 2
    { 0b0111110,
      0b1111111,
      0b0000011,
      0b0000011,
      0b0000011,
      0b0111111,
      0b1111110,
      0b1100000,
      0b1100000,
      0b1111111,
      0b1111111 },
    // 3
    { 0b0111110,
      0b1111111,
      0b0000011,
      0b0000011,
      0b0000011,
      0b0011110,
      0b0000011,
      0b0000011,
      0b0000011,
      0b1111111,
      0b0111110 },
    // 4
    { 0b1100011,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1111111,
      0b1111111,
      0b0000011,
      0b0000011,
      0b0000011,
      0b0000011 },
    // 5
    { 0b1111111,
      0b1111111,
      0b1100000,
      0b1100000,
      0b1111110,
      0b0111111,
      0b0000011,
      0b0000011,
      0b0000011,
      0b1111111,
      0b0111110 },
    // 6
    { 0b0111110,
      0b1111111,
      0b1100000,
      0b1100000,
      0b1111110,
      0b1111111,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1111111,
      0b0111110 },
    // 7
    { 0b1111111,
      0b1111111,
      0b0000011,
      0b0000011,
      0b0000110,
      0b0001100,
      0b0011000,
      0b0011000,
      0b0110000,
      0b0110000,
      0b0110000 },
    // 8
    { 0b0111110,
      0b1111111,
      0b1100011,
      0b1100011,
      0b1100011,
      0b0111110,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1111111,
      0b0111110 },
    // 9
    { 0b0111110,
      0b1111111,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1111111,
      0b0111111,
      0b0000011,
      0b0000011,
      0b1111111,
      0b0111110 }
};

// Pixel Clock State
struct PixelDigitGrid {
    std::vector<wuxs::Ellipse> dots; // 77 dots per digit (7 cols x 11 rows)
    int lastVal = -1; // Cached last rendered digit to avoid redundant updates
};
// no_destroy: the six dot vectors hold XAML Ellipse references and must only be
// cleared from the UI thread (see ResetClockElementReferences()).
[[clang::no_destroy]] PixelDigitGrid g_pixelDigits[6]; // H1, H2, M1, M2, S1, S2
[[clang::no_destroy]] wuxm::SolidColorBrush g_brushActiveDot{ nullptr };
[[clang::no_destroy]] wuxm::SolidColorBrush g_brushInactiveDot{ nullptr };

// Win32 Battery Status Helper
struct BatteryInfo {
    bool hasBattery = false;
    bool isCharging = false;
    int percentage = -1;
};

BatteryInfo GetCurrentBatteryInfo() {
    BatteryInfo info;
    SYSTEM_POWER_STATUS sps;
    if (GetSystemPowerStatus(&sps)) {
        // BatteryFlag 128 means "no system battery"; the unknown value (255)
        // also has that bit set. BatteryFlag 8 means "charging".
        if ((sps.BatteryFlag & 128) == 0 && sps.BatteryLifePercent <= 100) {
            info.hasBattery = true;
            info.percentage = (int)sps.BatteryLifePercent;
            info.isCharging = (sps.BatteryFlag & 8) != 0;
        }
    }
    return info;
}

// Build Universal Frosted Battery & Power Status Capsule
wux::UIElement CreateBatteryCapsule(bool alignLeft, bool isDarkTheme = false) {
    auto batteryInfo = GetCurrentBatteryInfo();
    if (!batteryInfo.hasBattery) {
        return nullptr; // Desktop PC without battery: do not render
    }

    wuxc::Border capsule;
    capsule.Name(L"WindhawkBatteryCapsule");
    if (isDarkTheme) {
        capsule.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(210, 14, 14, 14)));
        capsule.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(40, 255, 255, 255)));
    } else {
        capsule.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(32, 255, 255, 255)));
        capsule.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(55, 255, 255, 255)));
    }
    capsule.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
    capsule.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(10.0));
    capsule.Padding(wux::Thickness{ 12.0, 4.0, 14.0, 4.0 });
    capsule.Margin(wux::Thickness{ alignLeft ? 6.0 : 0.0, 12.0, 0.0, 0.0 });
    capsule.HorizontalAlignment(alignLeft ? wux::HorizontalAlignment::Left : wux::HorizontalAlignment::Center);

    wuxc::StackPanel panel;
    panel.Orientation(wuxc::Orientation::Horizontal);
    panel.VerticalAlignment(wux::VerticalAlignment::Center);

    wuxc::TextBlock iconText;
    iconText.Name(L"WindhawkBatteryIconText");
    iconText.Text(batteryInfo.isCharging ? L"⚡ " : L"🔋 ");
    iconText.FontSize(12.5);
    iconText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    iconText.Foreground(batteryInfo.isCharging
        ? wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 252, 238, 10)) // Gold / Yellow
        : wuxm::SolidColorBrush(wu::Colors::White()));
    iconText.VerticalAlignment(wux::VerticalAlignment::Center);
    panel.Children().Append(iconText);

    wuxc::TextBlock statusText;
    statusText.Name(L"WindhawkBatteryStatusText");
    WCHAR buf[32];
    swprintf_s(buf, L"%d%%", batteryInfo.percentage);
    statusText.Text(buf);
    statusText.FontSize(12.5);
    statusText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Text"));
    statusText.FontWeight(winrt::Windows::UI::Text::FontWeights::Medium());
    statusText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(235, 255, 255, 255)));
    statusText.VerticalAlignment(wux::VerticalAlignment::Center);
    panel.Children().Append(statusText);

    capsule.Child(panel);
    g_batteryCapsule = capsule;
    g_batteryIconText = iconText;
    g_batteryStatusText = statusText;
    return capsule;
}

// Studio extension. All XAML operations run on the LockApp UI thread.
struct StudioSettings {
    std::wstring font, foreground, accent, panel, dateFormat, text;
    std::wstring clockFont, dateFont, widgetFont, mediaFont;
    std::wstring clockAnchor, dateAnchor, batteryAnchor, textAnchor;
    int weight, opacity, radius, dim, dateSize, textSize;
    int clockX, clockY, dateX, dateY, batteryX, batteryY, textX, textY;
    int widgetX, widgetY, mediaX, mediaY;
    std::wstring widgetTarget, mediaTheme, mediaBackground, mediaForeground, mediaAccent;
    int mediaOpacity, mediaRadius, mediaTextSize;
    std::wstring widgetTheme, widgetBackground, widgetForeground, widgetAccent, widgetMoveContainer;
    int widgetOpacity, widgetRadius, widgetTextSize, widgetScreenPadding;
    bool widgetKeepOnScreen;
    std::wstring widgetMode, widgetAnchor;
    int widgetCardWidth;
    bool weatherIconVisible;
    bool widgetCustomStyle;
    std::wstring weatherIconPosition,weatherIconCondition;
    int weatherIconX,weatherIconY,weatherIconSize;
    std::wstring pixelDateLocale;
    int pixelDateTracking, pixelShadow;
    bool hideMedia, diagnostics;
} studio;
bool studioDumpDone = false;
struct StudioRule {
    std::wstring name, color;
    int x, y, opacity, fontSize;
    bool hide;
};
std::vector<StudioRule> studioRules;

std::wstring StudioString(PCWSTR key) {
    auto value = WindhawkUtils::StringSetting::make(key);
    return value.get() ? value.get() : L"";
}
int StudioInt(PCWSTR key, int low, int high) {
    return (std::clamp)(Wh_GetIntSetting(key), low, high);
}
void LoadStudioSettings() {
    studio.font = StudioString(L"fontFamily");
    studio.clockFont = StudioString(L"clockFontFamily");
    studio.dateFont = StudioString(L"dateFontFamily");
    studio.widgetFont = StudioString(L"widgetFontFamily");
    studio.mediaFont = StudioString(L"mediaFontFamily");
    studio.foreground = StudioString(L"foreground");
    studio.accent = StudioString(L"accent");
    studio.panel = StudioString(L"panelColor");
    studio.dateFormat = StudioString(L"dateFormat");
    studio.pixelDateLocale = StudioString(L"pixelDateLocale");
    studio.pixelDateTracking = StudioInt(L"pixelDateTracking", 0, 12);
    studio.pixelShadow = StudioInt(L"pixelShadow", 0, 8);
    studio.text = StudioString(L"customText");
    studio.clockAnchor = StudioString(L"clockAnchor");
    studio.dateAnchor = StudioString(L"dateAnchor");
    studio.batteryAnchor = StudioString(L"batteryAnchor");
    studio.textAnchor = StudioString(L"textAnchor");
    studio.weight = StudioInt(L"fontWeight", 100, 900);
    studio.opacity = StudioInt(L"panelOpacity", 0, 100);
    studio.radius = StudioInt(L"cornerRadius", 0, 100);
    studio.dim = StudioInt(L"backgroundDim", 0, 90);
    studio.dateSize = StudioInt(L"dateFontSize", 8, 160);
    studio.textSize = StudioInt(L"textFontSize", 8, 160);
    studio.clockX = StudioInt(L"clockX", -4000, 4000);
    studio.clockY = StudioInt(L"clockY", -4000, 4000);
    studio.dateX = StudioInt(L"dateX", -4000, 4000);
    studio.dateY = StudioInt(L"dateY", -4000, 4000);
    studio.batteryX = StudioInt(L"batteryX", -4000, 4000);
    studio.batteryY = StudioInt(L"batteryY", -4000, 4000);
    studio.textX = StudioInt(L"textX", -4000, 4000);
    studio.textY = StudioInt(L"textY", -4000, 4000);
    studio.widgetX = StudioInt(L"widgetX", -4000, 4000);
    studio.widgetY = StudioInt(L"widgetY", -4000, 4000);
    studio.mediaX = StudioInt(L"mediaX", -4000, 4000);
    studio.mediaY = StudioInt(L"mediaY", -4000, 4000);
    studio.widgetTarget = StudioString(L"widgetTarget");
    studio.mediaTheme = StudioString(L"mediaTheme");
    studio.mediaBackground = StudioString(L"mediaBackground");
    studio.mediaForeground = StudioString(L"mediaForeground");
    studio.mediaAccent = StudioString(L"mediaAccent");
    studio.mediaOpacity = StudioInt(L"mediaOpacity", 0, 100);
    studio.mediaRadius = StudioInt(L"mediaCornerRadius", 0, 80);
    studio.mediaTextSize = StudioInt(L"mediaTextSize", 10, 40);
    studio.widgetTheme = StudioString(L"widgetTheme");
    studio.widgetBackground = StudioString(L"widgetBackground");
    studio.widgetForeground = StudioString(L"widgetForeground");
    studio.widgetAccent = StudioString(L"widgetAccent");
    studio.widgetMoveContainer = StudioString(L"widgetMoveContainer");
    studio.widgetOpacity = StudioInt(L"widgetOpacity", 0, 100);
    studio.widgetRadius = StudioInt(L"widgetCornerRadius", 0, 80);
    studio.widgetTextSize = StudioInt(L"widgetTextSize", 0, 40);
    studio.widgetKeepOnScreen = Wh_GetIntSetting(L"widgetKeepOnScreen") != 0;
    studio.widgetScreenPadding = StudioInt(L"widgetScreenPadding", 0, 100);
    studio.widgetMode = StudioString(L"widgetMode");
    studio.widgetAnchor = StudioString(L"widgetAnchor");
    studio.widgetCardWidth = StudioInt(L"widgetCardWidth", 180, 600);
    studio.widgetCustomStyle = Wh_GetIntSetting(L"widgetCustomStyle") != 0;
    if(!studio.widgetCustomStyle) {
        studio.widgetBackground.clear(); studio.widgetForeground.clear(); studio.widgetAccent.clear();
        studio.widgetOpacity=studio.widgetTheme==L"smoke" ? 78 : 100;
        studio.widgetRadius=studio.widgetTheme==L"macos" || studio.widgetTheme==L"macosDark" ? 24 :
            studio.widgetTheme==L"catppuccin" ? 20 : studio.widgetTheme==L"nord" ? 8 :
            studio.widgetTheme==L"gruvbox" ? 4 : studio.widgetTheme==L"minimal" ? 12 : 18;
    }
    studio.weatherIconVisible = Wh_GetIntSetting(L"weatherIconVisible") != 0;
    studio.weatherIconPosition = StudioString(L"weatherIconPosition");
    studio.weatherIconCondition = StudioString(L"weatherIconCondition");
    studio.weatherIconX = StudioInt(L"weatherIconX", -1000, 1000);
    studio.weatherIconY = StudioInt(L"weatherIconY", -1000, 1000);
    studio.weatherIconSize = StudioInt(L"weatherIconSize", 24, 160);
    studio.hideMedia = Wh_GetIntSetting(L"hideMedia") != 0;
    studio.diagnostics = Wh_GetIntSetting(L"diagnostics") != 0;
    studioRules.clear();
    for (int i = 0; i < 100; ++i) {
        std::wstring prefix = L"elementRules[" + std::to_wstring(i) + L"].";
        auto name = StudioString((prefix + L"name").c_str());
        if (name.empty()) break;
        StudioRule rule;
        rule.name = name;
        rule.color = StudioString((prefix + L"color").c_str());
        rule.x = StudioInt((prefix + L"x").c_str(), -4000, 4000);
        rule.y = StudioInt((prefix + L"y").c_str(), -4000, 4000);
        rule.opacity = StudioInt((prefix + L"opacity").c_str(), 0, 100);
        rule.fontSize = StudioInt((prefix + L"fontSize").c_str(), 0, 300);
        rule.hide = Wh_GetIntSetting((prefix + L"hide").c_str()) != 0;
        studioRules.push_back(std::move(rule));
    }
}
wu::Color StudioColor(std::wstring const& input, unsigned long fallback) {
    auto color = fallback;
    if (input.size() == 7 || input.size() == 9) {
        bool valid = input[0] == L'#';
        for (size_t i = 1; i < input.size(); ++i)
            valid = valid && ((input[i] >= L'0' && input[i] <= L'9') ||
                     (input[i] >= L'a' && input[i] <= L'f') ||
                     (input[i] >= L'A' && input[i] <= L'F'));
        if (valid) {
            color = wcstoul(input.c_str() + 1, nullptr, 16);
            if (input.size() == 7) color |= 0xFF000000;
        } else Wh_Log(L"Invalid Studio color: %s", input.c_str());
    } else if (!input.empty()) Wh_Log(L"Invalid Studio color: %s", input.c_str());
    return wu::ColorHelper::FromArgb((color >> 24) & 255, (color >> 16) & 255,
                                   (color >> 8) & 255, color & 255);
}
void StudioPlace(wux::FrameworkElement const& element, std::wstring anchor, double x, double y) {
    bool left = anchor == L"left" || anchor == L"topLeft" || anchor == L"bottomLeft";
    bool right = anchor == L"right" || anchor == L"topRight" || anchor == L"bottomRight";
    bool top = anchor == L"top" || anchor == L"topLeft" || anchor == L"topRight";
    bool bottom = anchor == L"bottom" || anchor == L"bottomLeft" || anchor == L"bottomRight";
    element.HorizontalAlignment(left ? wux::HorizontalAlignment::Left :
        right ? wux::HorizontalAlignment::Right : wux::HorizontalAlignment::Center);
    element.VerticalAlignment(top ? wux::VerticalAlignment::Top :
        bottom ? wux::VerticalAlignment::Bottom : wux::VerticalAlignment::Center);
    wuxm::TranslateTransform transform;
    transform.X(x); transform.Y(y);
    element.RenderTransform(transform);
}
// Font outlines: Pixelify Sans Regular; licensed under SIL OFL 1.1.
/*
Copyright 2021 The Pixelify Sans Project Authors (https://github.com/eifetx/Pixelify-Sans)

This Font Software is licensed under the SIL Open Font License, Version 1.1.
This license is copied below, and is also available with a FAQ at:
https://scripts.sil.org/OFL


-----------------------------------------------------------
SIL OPEN FONT LICENSE Version 1.1 - 26 February 2007
-----------------------------------------------------------

PREAMBLE
The goals of the Open Font License (OFL) are to stimulate worldwide
development of collaborative font projects, to support the font creation
efforts of academic and linguistic communities, and to provide a free and
open framework in which fonts may be shared and improved in partnership
with others.

The OFL allows the licensed fonts to be used, studied, modified and
redistributed freely as long as they are not sold by themselves. The
fonts, including any derivative works, can be bundled, embedded,
redistributed and/or sold with any software provided that any reserved
names are not used by derivative works. The fonts and derivatives,
however, cannot be released under any other type of license. The
requirement for fonts to remain under this license does not apply
to any document created using the fonts or their derivatives.

DEFINITIONS
"Font Software" refers to the set of files released by the Copyright
Holder(s) under this license and clearly marked as such. This may
include source files, build scripts and documentation.

"Reserved Font Name" refers to any names specified as such after the
copyright statement(s).

"Original Version" refers to the collection of Font Software components as
distributed by the Copyright Holder(s).

"Modified Version" refers to any derivative made by adding to, deleting,
or substituting -- in part or in whole -- any of the components of the
Original Version, by changing formats or by porting the Font Software to a
new environment.

"Author" refers to any designer, engineer, programmer, technical
writer or other person who contributed to the Font Software.

PERMISSION & CONDITIONS
Permission is hereby granted, free of charge, to any person obtaining
a copy of the Font Software, to use, study, copy, merge, embed, modify,
redistribute, and sell modified and unmodified copies of the Font
Software, subject to the following conditions:

1) Neither the Font Software nor any of its individual components,
in Original or Modified Versions, may be sold by itself.

2) Original or Modified Versions of the Font Software may be bundled,
redistributed and/or sold with any software, provided that each copy
contains the above copyright notice and this license. These can be
included either as stand-alone text files, human-readable headers or
in the appropriate machine-readable metadata fields within text or
binary files as long as those fields can be easily viewed by the user.

3) No Modified Version of the Font Software may use the Reserved Font
Name(s) unless explicit written permission is granted by the corresponding
Copyright Holder. This restriction only applies to the primary font name as
presented to the users.

4) The name(s) of the Copyright Holder(s) or the Author(s) of the Font
Software shall not be used to promote, endorse or advertise any
Modified Version, except to acknowledge the contribution(s) of the
Copyright Holder(s) and the Author(s) or with their explicit written
permission.

5) The Font Software, modified or unmodified, in part or in whole,
must be distributed entirely under this license, and must not be
distributed under any other license. The requirement for fonts to
remain under this license does not apply to any document created
using the Font Software.

TERMINATION
This license becomes null and void if any of the above conditions are
not met.

DISCLAIMER
THE FONT SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT
OF COPYRIGHT, PATENT, TRADEMARK, OR OTHER RIGHT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
INCLUDING ANY GENERAL, SPECIAL, INDIRECT, INCIDENTAL, OR CONSEQUENTIAL
DAMAGES, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF THE USE OR INABILITY TO USE THE FONT SOFTWARE OR FROM
OTHER DEALINGS IN THE FONT SOFTWARE.

*/
constexpr double kStudioPixelEm = 1000;
constexpr double kStudioPixelHeight = 1200;
struct StudioPixelGlyph { wchar_t character; int advance; const wchar_t* data; };
static const StudioPixelGlyph kStudioPixelGlyphs[] = {
    {static_cast<wchar_t>(32), 200, L""},
    {static_cast<wchar_t>(33), 222, L"M60 751V289H161V751ZM60 932V832H161V932Z"},
    {static_cast<wchar_t>(34), 465, L"M304 480V424H350V390H258V289H360V380H404V434H360V480ZM105 480V424H151V390H60V289H161V380H206V434H161V480Z"},
    {static_cast<wchar_t>(35), 677, L"M151 932V842H60V741H151V570H60V470H151V289H251V470H423V289H525V470H616V570H525V741H616V842H525V932H423V842H251V932ZM251 741H423V570H251Z"},
    {static_cast<wchar_t>(36), 586, L"M250 1023V932H151V842H60V741H161V832H423V660H151V570H60V380H151V289H250V198H352V289H433V380H525V480H423V390H161V561H433V650H525V842H433V932H352V1023Z"},
    {static_cast<wchar_t>(37), 867, L"M109 932V832H201V741H291V650H383V561H473V470H564V380H656V289H756V390H666V480H574V570H483V660H393V751H301V842H211V932ZM151 570V480H60V380H151V289H251V380H343V480H251V570ZM614 932V842H523V741H614V650H715V741H806V842H715V932Z"},
    {static_cast<wchar_t>(38), 677, L"M151 932V842H60V741H151V650H241V570H151V480H60V380H151V289H433V380H525V480H433V570H343V650H433V741H525V832H616V932H515V842H433V932ZM161 832H423V751H333V660H251V751H161ZM251 561H333V470H423V390H161V470H251Z"},
    {static_cast<wchar_t>(39), 267, L"M105 480V424H151V390H60V289H161V380H206V434H161V480Z"},
    {static_cast<wchar_t>(40), 374, L"M241 1080V896H151V716H60V525H151V343H241V163H343V353H251V535H161V706H251V889H343V1080Z"},
    {static_cast<wchar_t>(41), 374, L"M30 1080V889H121V706H212V535H121V353H30V163H131V343H222V525H313V716H222V896H131V1080Z"},
    {static_cast<wchar_t>(42), 267, L"M60 480V424H105V390H60V335H105V289H161V335H206V390H161V424H206V480H151V434H115V480Z"},
    {static_cast<wchar_t>(43), 600, L"M248 833V663H67V563H248V371H350V563H532V663H350V833Z"},
    {static_cast<wchar_t>(44), 267, L"M105 1010V954H151V920H60V819H161V910H206V964H161V1010Z"},
    {static_cast<wchar_t>(45), 333, L"M23 720V619H305V720Z"},
    {static_cast<wchar_t>(46), 222, L"M60 920V819H161V920Z"},
    {static_cast<wchar_t>(47), 529, L"M60 932 378 289H467L149 932Z"},
    {static_cast<wchar_t>(48), 586, L"M151 932V842H60V380H151V289H433V380H525V842H433V932ZM161 832H423V390H161Z"},
    {static_cast<wchar_t>(49), 404, L"M241 932V390H161V480H60V380H151V289H251V380H343V932Z"},
    {static_cast<wchar_t>(50), 586, L"M151 932V842H60V650H151V561H423V390H161V480H60V380H151V289H433V380H525V570H433V660H161V832H423V741H525V842H433V932Z"},
    {static_cast<wchar_t>(51), 586, L"M151 932V842H60V741H161V832H423V660H151V561H423V390H161V480H60V380H151V289H433V380H525V570H433V650H525V842H433V932Z"},
    {static_cast<wchar_t>(52), 586, L"M423 932V751H60V561H151V470H241V380H333V289H525V932ZM161 650H423V390H343V480H251V570H161Z"},
    {static_cast<wchar_t>(53), 586, L"M151 932V842H60V741H161V832H423V751H241V660H151V570H60V380H151V289H433V380H525V570H423V390H161V561H251V650H433V741H525V842H433V932Z"},
    {static_cast<wchar_t>(54), 586, L"M151 932V842H60V380H151V289H433V380H525V480H423V390H161V561H433V650H525V842H433V932ZM161 832H423V660H161Z"},
    {static_cast<wchar_t>(55), 586, L"M423 932V390H161V480H60V380H151V289H433V380H525V932Z"},
    {static_cast<wchar_t>(56), 586, L"M151 932V842H60V650H151V570H60V380H151V289H433V380H525V570H433V650H525V842H433V932ZM161 832H423V660H161ZM161 561H423V390H161Z"},
    {static_cast<wchar_t>(57), 586, L"M151 932V842H60V741H161V832H423V660H151V570H60V380H151V289H433V380H525V842H433V932ZM161 561H423V390H161Z"},
    {static_cast<wchar_t>(58), 222, L"M60 839V738H161V839ZM60 661V561H161V661Z"},
    {static_cast<wchar_t>(59), 267, L"M105 929V874H151V839H60V738H161V829H206V883H161V929ZM60 661V561H161V661Z"},
    {static_cast<wchar_t>(60), 600, L"M430 1027V936H340V845H248V755H158V664H67V565H158V474H248V383H340V293H430V202H532V303H440V393H350V484H258V574H168V654H258V745H350V835H440V926H532V1027Z"},
    {static_cast<wchar_t>(61), 600, L"M109 630V529H491V630ZM109 785V685H491V785Z"},
    {static_cast<wchar_t>(62), 600, L"M67 1027V926H158V835H249V745H340V654H431V574H340V484H249V393H158V303H67V202H168V293H259V383H350V474H440V565H532V664H440V755H350V845H259V936H168V1027Z"},
    {static_cast<wchar_t>(63), 586, L"M241 751V561H423V390H161V570H60V380H151V289H433V380H525V570H433V660H343V751ZM241 932V832H343V932Z"},
    {static_cast<wchar_t>(64), 586, L"M151 858V812H105V767H60V531H105V487H151V441H388V487H433V531H479V576H525V677H479V722H433V767H381V729H350V767H212V729H170V607H212V568H350V607H391V720H423V667H469V586H423V541H378V496H161V541H115V757H161V802H343V858ZM222 720H340V617H222Z"},
    {static_cast<wchar_t>(65), 586, L"M60 932V470H151V380H241V289H343V380H433V470H525V932H423V751H161V932ZM161 650H423V480H333V390H251V480H161Z"},
    {static_cast<wchar_t>(66), 586, L"M151 932V842H60V380H151V289H433V380H525V570H433V650H525V842H433V932ZM161 832H423V660H241V561H423V390H161Z"},
    {static_cast<wchar_t>(67), 586, L"M151 932V842H60V380H151V289H433V380H525V570H423V390H161V832H423V650H525V842H433V932Z"},
    {static_cast<wchar_t>(68), 586, L"M60 932V289H343V380H433V470H525V751H433V842H343V932ZM161 832H333V741H423V480H333V390H161Z"},
    {static_cast<wchar_t>(69), 586, L"M151 932V842H60V380H151V289H433V380H525V480H423V390H161V561H343V660H161V832H423V741H525V842H433V932Z"},
    {static_cast<wchar_t>(70), 586, L"M60 932V289H433V380H525V480H423V390H161V561H343V660H161V932Z"},
    {static_cast<wchar_t>(71), 586, L"M151 932V842H60V380H151V289H433V380H525V570H423V390H161V832H423V751H333V660H241V561H343V650H525V842H433V932Z"},
    {static_cast<wchar_t>(72), 586, L"M60 932V289H161V561H241V470H343V561H423V289H525V932H423V660H161V932Z"},
    {static_cast<wchar_t>(73), 404, L"M60 932V832H151V390H60V289H343V390H251V832H343V932Z"},
    {static_cast<wchar_t>(74), 586, L"M151 932V842H60V650H161V832H423V390H151V289H433V380H525V842H433V932Z"},
    {static_cast<wchar_t>(75), 586, L"M60 932V380H151V289H251V390H161V561H241V470H333V380H423V289H525V390H433V480H343V570H251V650H343V741H433V832H525V932H423V842H333V751H241V660H161V932Z"},
    {static_cast<wchar_t>(76), 586, L"M151 932V842H60V289H161V832H423V741H525V842H433V932Z"},
    {static_cast<wchar_t>(77), 767, L"M60 932V380H151V289H251V380H343V561H423V380H515V289H616V380H706V932H606V390H525V570H433V842H333V570H241V390H161V932Z"},
    {static_cast<wchar_t>(78), 586, L"M60 932V289H161V380H251V470H343V741H423V289H525V932H423V842H333V751H241V480H161V932Z"},
    {static_cast<wchar_t>(79), 586, L"M151 932V842H60V380H151V289H433V380H525V842H433V932ZM161 832H423V390H161Z"},
    {static_cast<wchar_t>(80), 586, L"M60 932V380H151V289H433V380H525V660H433V751H161V932ZM161 650H423V390H161Z"},
    {static_cast<wchar_t>(81), 586, L"M333 1112V1023H241V932H151V842H60V380H151V289H433V380H525V842H433V932H343V1013H433V1112ZM161 832H423V390H161Z"},
    {static_cast<wchar_t>(82), 586, L"M60 932V380H151V289H433V380H525V660H433V751H389V832H479V932H379V842H287V751H161V932ZM161 650H423V390H161Z"},
    {static_cast<wchar_t>(83), 586, L"M151 932V842H60V741H161V832H423V660H151V570H60V380H151V289H433V380H525V480H423V390H161V561H433V650H525V842H433V932Z"},
    {static_cast<wchar_t>(84), 586, L"M241 932V390H161V480H60V380H151V289H433V380H525V480H423V390H343V932Z"},
    {static_cast<wchar_t>(85), 586, L"M151 932V842H60V289H161V832H423V289H525V842H433V932Z"},
    {static_cast<wchar_t>(86), 586, L"M241 932V842H151V660H60V289H161V650H251V832H333V650H423V289H525V660H433V842H343V932Z"},
    {static_cast<wchar_t>(87), 767, L"M151 932V842H60V289H161V832H241V650H333V380H433V650H525V832H606V289H706V842H616V932H515V842H423V660H343V842H251V932Z"},
    {static_cast<wchar_t>(88), 586, L"M60 932V832H151V650H241V570H151V390H60V289H161V380H251V561H333V380H423V289H525V390H433V570H343V650H433V832H525V932H423V842H333V660H251V842H161V932Z"},
    {static_cast<wchar_t>(89), 586, L"M241 932V570H151V480H60V289H161V470H251V561H333V470H423V289H525V480H433V570H343V932Z"},
    {static_cast<wchar_t>(90), 586, L"M151 932V842H60V650H151V561H423V390H161V480H60V289H433V380H525V570H433V660H161V832H423V741H525V932Z"},
    {static_cast<wchar_t>(91), 374, L"M151 981V890H60V338H151V248H343V348H161V880H343V981Z"},
    {static_cast<wchar_t>(92), 529, L"M378 932 60 289H149L467 932Z"},
    {static_cast<wchar_t>(93), 374, L"M30 981V880H212V348H30V248H222V338H313V890H222V981Z"},
    {static_cast<wchar_t>(94), 267, L"M60 390V335H105V289H161V335H206V390H151V344H115V390Z"},
    {static_cast<wchar_t>(95), 442, L"M60 920V819H382V920Z"},
    {static_cast<wchar_t>(96), 266, L"M151 396V352H105V306H60V251H115V297H161V342H206V396Z"},
    {static_cast<wchar_t>(97), 677, L"M151 932V842H60V561H151V470H433V561H525V832H616V932H515V842H433V932ZM161 832H423V570H161Z"},
    {static_cast<wchar_t>(98), 586, L"M151 932V842H60V289H161V470H433V561H525V842H433V932ZM161 832H423V570H161Z"},
    {static_cast<wchar_t>(99), 586, L"M151 932V842H60V561H151V470H433V561H525V660H423V570H161V832H423V741H525V842H433V932Z"},
    {static_cast<wchar_t>(100), 586, L"M151 932V842H60V561H151V470H424V289H525V842H433V932ZM161 832H423V570H161Z"},
    {static_cast<wchar_t>(101), 586, L"M151 932V842H60V561H151V470H433V561H525V660H423V570H161V650H343V751H161V832H423V740H525V841H433V932Z"},
    {static_cast<wchar_t>(102), 586, L"M60 932V380H151V289H433V380H525V480H423V390H161V561H343V660H161V932Z"},
    {static_cast<wchar_t>(103), 586, L"M151 1113V1023H60V923H161V1013H424V842H151V751H60V561H151V470H434V561H525V1023H434V1113ZM161 741H424V570H161Z"},
    {static_cast<wchar_t>(104), 586, L"M60 932V289H161V561H241V470H433V561H525V932H423V570H251V660H161V932Z"},
    {static_cast<wchar_t>(105), 222, L"M60 932V470H161V932ZM60 390V289H161V390Z"},
    {static_cast<wchar_t>(106), 495, L"M151 933V842H60V741H161V832H333V470H434V842H343V933Z"},
    {static_cast<wchar_t>(107), 586, L"M60 932V289H161V650H241V561H423V470H525V570H433V660H251V741H433V832H525V932H423V842H241V751H161V932Z"},
    {static_cast<wchar_t>(108), 222, L"M60 932V289H161V932Z"},
    {static_cast<wchar_t>(109), 767, L"M60 932V470H343V561H423V470H616V561H706V932H606V570H433V932H333V570H161V932Z"},
    {static_cast<wchar_t>(110), 586, L"M60 932V470H433V561H525V932H423V570H161V932Z"},
    {static_cast<wchar_t>(111), 586, L"M151 932V842H60V561H151V470H433V561H525V842H433V932ZM161 832H423V570H161Z"},
    {static_cast<wchar_t>(112), 586, L"M60 1108V561H151V470H433V561H525V842H433V932H161V1108ZM161 832H423V570H161Z"},
    {static_cast<wchar_t>(113), 586, L"M423 1108V932H151V842H60V561H151V470H433V561H525V1108ZM161 832H423V570H161Z"},
    {static_cast<wchar_t>(114), 586, L"M60 932V561H151V470H433V561H525V660H423V570H161V932Z"},
    {static_cast<wchar_t>(115), 586, L"M60 932V832H423V751H151V660H60V561H151V470H525V570H161V650H433V741H525V842H433V932Z"},
    {static_cast<wchar_t>(116), 677, L"M241 932V842H151V570H60V470H151V289H251V470H525V570H251V832H515V741H616V842H525V932Z"},
    {static_cast<wchar_t>(117), 586, L"M151 932V842H60V470H161V832H423V470H525V842H433V932Z"},
    {static_cast<wchar_t>(118), 576, L"M232 932V842H141V660H50V470H151V650H241V832H323V650H413V470H515V660H423V842H333V932Z"},
    {static_cast<wchar_t>(119), 767, L"M60 932V470H161V832H333V470H433V832H606V470H706V842H616V932H423V842H343V932Z"},
    {static_cast<wchar_t>(120), 586, L"M60 932V832H151V741H241V660H151V570H60V470H161V561H251V650H333V561H423V470H525V570H433V660H343V741H433V832H525V932H423V842H333V751H251V842H161V932Z"},
    {static_cast<wchar_t>(121), 586, L"M151 1113V1023H60V923H161V1013H424V842H151V751H60V470H161V741H424V470H525V1023H434V1113Z"},
    {static_cast<wchar_t>(122), 586, L"M60 932V832H151V741H241V650H333V570H60V470H525V570H433V660H343V751H251V832H525V932Z"},
    {static_cast<wchar_t>(123), 464, L"M212 1027V936H121V664H30V565H121V293H212V202H313V293H403V393H303V303H222V574H131V654H222V926H303V835H403V936H313V1027Z"},
    {static_cast<wchar_t>(124), 222, L"M60 932V289H161V932Z"},
    {static_cast<wchar_t>(125), 464, L"M120 1027V936H30V835H130V926H212V654H302V574H212V303H130V393H30V293H120V202H222V293H312V565H403V664H312V936H222V1027Z"},
    {static_cast<wchar_t>(126), 600, L"M176 723V620H328V666H365V620H423V723H270V676H233V723Z"},
    {static_cast<wchar_t>(160), 200, L""},
    {static_cast<wchar_t>(161), 222, L"M60 932V470H161V932ZM60 389V289H161V389Z"},
    {static_cast<wchar_t>(162), 586, L"M238 920V822H151V732H60V451H151V360H238V261H347V360H433V451H525V551H423V461H161V722H423V632H525V732H433V822H347V920Z"},
    {static_cast<wchar_t>(163), 676, L"M60 932V832H151V380H241V289H525V380H615V480H515V390H251V561H433V660H251V832H515V741H615V842H525V932Z"},
    {static_cast<wchar_t>(164), 454, L"M152 734V688H106V540H152V494H301V540H347V688H301V734ZM60 780V724H116V780ZM60 504V448H116V504ZM162 678H291V550H162ZM337 780V724H394V780ZM337 504V448H394V504Z"},
    {static_cast<wchar_t>(165), 586, L"M241 932V850H151V750H241V715H151V614H241V570H151V480H60V289H161V470H251V561H333V470H423V289H525V480H433V570H343V614H433V715H343V750H433V850H343V932Z"},
    {static_cast<wchar_t>(167), 586, L"M151 932V842H60V650H151V570H60V380H151V289H433V380H525V480H423V390H161V561H433V650H525V751H161V832H525V932Z"},
    {static_cast<wchar_t>(168), 300, L"M-9 437V336H93V437ZM204 437V336H305V437Z"},
    {static_cast<wchar_t>(169), 409, L"M115 566V520H70V344H115V299H292V344H338V520H292V566ZM125 510H282V354H125ZM175 482V465H157V403H175V384H237V403H254V429H227V413H185V455H227V436H254V465H237V482Z"},
    {static_cast<wchar_t>(170), 398, L"M106 524V479H60V338H106V293H247V338H293V474H338V524H288V479H247V524ZM111 474H242V343H111ZM247 601V550H106V601Z"},
    {static_cast<wchar_t>(171), 873, L"M711 1027V936H621V845H530V755H439V664H348V565H439V474H530V383H621V293H711V202H813V303H721V393H631V484H540V574H449V654H540V745H631V835H721V926H813V1027ZM394 1027V936H303V845H212V755H121V664H30V565H121V474H212V383H303V293H394V202H495V303H403V393H313V484H222V574H131V654H222V745H313V835H403V926H495V1027Z"},
    {static_cast<wchar_t>(172), 600, L"M389 721V630H109V529H490V721Z"},
    {static_cast<wchar_t>(174), 409, L"M115 566V520H70V344H115V299H292V344H338V520H292V566ZM125 510H282V354H125ZM171 469V420H183V408H221V420H233V433H220V421H185V469Z"},
    {static_cast<wchar_t>(175), 300, L"M13 379V278H295V379Z"},
    {static_cast<wchar_t>(176), 313, L"M96 528V491H60V373H96V336H216V373H252V491H216V528ZM106 482H206V383H106Z"},
    {static_cast<wchar_t>(177), 600, L"M246 765V645H101V549H246V429H354V549H500V645H354V765ZM101 927V827H500V927Z"},
    {static_cast<wchar_t>(178), 364, L"M106 626V580H60V477H106V430H246V347H116V394H60V337H106V291H256V337H303V440H256V487H116V570H246V523H303V580H256V626Z"},
    {static_cast<wchar_t>(179), 364, L"M107 624V577H60V521H117V567H245V484H107V428H245V346H117V392H60V336H107V289H255V336H303V438H255V474H303V577H255V624Z"},
    {static_cast<wchar_t>(180), 300, L"M60 380V326H105V281H151V235H206V290H161V336H115V380Z"},
    {static_cast<wchar_t>(181), 586, L"M60 1108V470H161V832H423V470H525V842H433V932H161V1108Z"},
    {static_cast<wchar_t>(182), 766, L"M423 1113V749H151V659H60V380H151V289H705V390H655V1113H603V390H475V1113Z"},
    {static_cast<wchar_t>(183), 222, L"M60 654V554H161V654Z"},
    {static_cast<wchar_t>(184), 300, L"M90 1120V1075H44V1020H100V1065H226V1030H180V985H135V883H190V975H236V1020H281V1075H236V1120Z"},
    {static_cast<wchar_t>(185), 271, L"M153 624V345H116V392H60V336H106V289H163V336H210V624Z"},
    {static_cast<wchar_t>(186), 353, L"M106 475V520H247V475H293V334H247V289H106V334H60V475ZM111 339H242V470H111ZM247 601V550H106V601Z"},
    {static_cast<wchar_t>(187), 853, L"M328 1027V926H419V835H510V745H601V654H692V574H601V484H510V393H419V303H328V202H429V293H520V383H611V474H702V565H793V664H702V755H611V845H520V936H429V1027ZM10 1027V926H101V835H192V745H283V654H374V574H283V484H192V393H101V303H10V202H111V293H202V383H293V474H384V565H475V664H384V755H293V845H202V936H111V1027Z"},
    {static_cast<wchar_t>(188), 877, L"M217 932V764H296V606H376V527H455V368H536V289H625V457H546V537H465V695H386V774H306V932ZM153 624V345H116V392H60V336H106V289H163V336H210V624ZM759 933V840H572V737H619V691H666V645H712V598H816V933ZM629 784H759V654H722V701H676V747H629Z"},
    {static_cast<wchar_t>(189), 816, L"M167 932V764H246V606H326V527H405V368H486V289H575V457H496V537H415V695H336V774H256V932ZM153 624V345H116V392H60V336H106V289H163V336H210V624ZM560 933V887H513V734H560V687H700V604H570V651H513V595H560V548H710V595H756V697H710V744H570V877H700V780H756V887H710V933Z"},
    {static_cast<wchar_t>(190), 956, L"M296 932V764H376V606H455V527H535V368H615V289H705V457H625V537H545V695H465V774H386V932ZM107 624V577H60V521H117V567H245V484H107V428H245V346H117V392H60V336H107V289H255V336H303V438H255V474H303V577H255V624ZM839 933V840H652V737H699V691H745V645H792V598H895V933ZM708 784H839V654H802V701H755V747H708Z"},
    {static_cast<wchar_t>(191), 586, L"M151 932V841H60V650H151V561H241V470H343V660H161V831H423V650H525V841H433V932ZM241 389V289H343V389Z"},
    {static_cast<wchar_t>(192), 586, L"M60 932V470H151V380H241V289H343V380H433V470H525V932H423V751H161V932ZM161 650H423V480H333V390H251V480H161ZM266 216V172H220V126H175V71H230V117H276V162H321V216Z"},
    {static_cast<wchar_t>(193), 586, L"M60 932V470H151V380H241V289H343V380H433V470H525V932H423V751H161V932ZM161 650H423V480H333V390H251V480H161ZM205 216V162H250V117H296V71H351V126H306V172H260V216Z"},
    {static_cast<wchar_t>(194), 586, L"M60 932V470H151V380H241V289H343V380H433V470H525V932H423V751H161V932ZM161 650H423V480H333V390H251V480H161ZM220 216V161H265V115H321V161H366V216H311V170H275V216Z"},
    {static_cast<wchar_t>(195), 586, L"M60 932V470H151V380H241V289H343V380H433V470H525V932H423V751H161V932ZM161 650H423V480H333V390H251V480H161ZM162 216V113H314V159H351V113H409V216H256V169H219V216Z"},
    {static_cast<wchar_t>(196), 586, L"M60 932V470H151V380H241V289H343V380H433V470H525V932H423V751H161V932ZM161 650H423V480H333V390H251V480H161ZM140 216V115H242V216ZM353 216V115H454V216Z"},
    {static_cast<wchar_t>(197), 586, L"M60 932V470H151V380H241V289H343V380H433V470H525V932H423V751H161V932ZM161 650H423V480H333V390H251V480H161ZM241 216V115H342V216ZM274 183H309V148H274Z"},
    {static_cast<wchar_t>(198), 949, L"M60 932V470H151V380H241V289H343V380H515V289H797V380H888V480H787V390H525V561H706V660H525V832H787V741H888V842H797V932H423V751H161V932ZM161 650H423V480H333V390H251V480H161Z"},
    {static_cast<wchar_t>(199), 586, L"M151 932V842H60V380H151V289H433V380H525V570H423V390H161V832H423V650H525V842H433V932ZM221 1120V1075H175V1020H231V1065H357V1030H311V985H266V883H321V975H367V1020H412V1075H367V1120Z"},
    {static_cast<wchar_t>(200), 586, L"M151 932V842H60V380H151V289H433V380H525V480H423V390H161V561H343V660H161V832H423V741H525V842H433V932ZM266 215V171H220V125H175V70H230V116H276V161H321V215Z"},
    {static_cast<wchar_t>(201), 586, L"M151 932V842H60V380H151V289H433V380H525V480H423V390H161V561H343V660H161V832H423V741H525V842H433V932ZM205 215V161H250V116H296V70H351V125H306V171H260V215Z"},
    {static_cast<wchar_t>(202), 586, L"M151 932V842H60V380H151V289H433V380H525V480H423V390H161V561H343V660H161V832H423V741H525V842H433V932ZM220 215V160H265V114H321V160H366V215H311V169H275V215Z"},
    {static_cast<wchar_t>(203), 586, L"M151 932V842H60V380H151V289H433V380H525V480H423V390H161V561H343V660H161V832H423V741H525V842H433V932ZM140 215V114H242V215ZM353 215V114H454V215Z"},
    {static_cast<wchar_t>(204), 404, L"M60 932V832H151V390H60V289H343V390H251V832H343V932ZM175 215V171H129V125H84V70H139V116H185V161H230V215Z"},
    {static_cast<wchar_t>(205), 404, L"M60 932V832H151V390H60V289H343V390H251V832H343V932ZM114 215V161H159V116H205V70H260V125H215V171H169V215Z"},
    {static_cast<wchar_t>(206), 404, L"M60 932V832H151V390H60V289H343V390H251V832H343V932ZM129 215V160H174V114H230V160H275V215H220V169H184V215Z"},
    {static_cast<wchar_t>(207), 404, L"M60 932V832H151V390H60V289H343V390H251V832H343V932ZM49 215V114H151V215ZM262 215V114H363V215Z"},
    {static_cast<wchar_t>(208), 586, L"M60 932V679H22V578H60V289H343V380H433V470H525V751H433V842H343V932ZM161 832H333V741H423V480H333V390H161V578H254V679H161Z"},
    {static_cast<wchar_t>(209), 586, L"M60 932V289H161V380H251V470H343V741H423V289H525V932H423V842H333V751H241V480H161V932ZM162 215V112H314V158H351V112H409V215H256V168H219V215Z"},
    {static_cast<wchar_t>(210), 586, L"M151 932V842H60V380H151V289H433V380H525V842H433V932ZM161 832H423V390H161ZM266 215V171H220V125H175V70H230V116H276V161H321V215Z"},
    {static_cast<wchar_t>(211), 586, L"M151 932V842H60V380H151V289H433V380H525V842H433V932ZM161 832H423V390H161ZM205 215V161H250V116H296V70H351V125H306V171H260V215Z"},
    {static_cast<wchar_t>(212), 586, L"M151 932V842H60V380H151V289H433V380H525V842H433V932ZM161 832H423V390H161ZM220 215V160H265V114H321V160H366V215H311V169H275V215Z"},
    {static_cast<wchar_t>(213), 586, L"M151 932V842H60V380H151V289H433V380H525V842H433V932ZM161 832H423V390H161ZM162 215V112H314V158H351V112H409V215H256V168H219V215Z"},
    {static_cast<wchar_t>(214), 586, L"M151 932V842H60V380H151V289H433V380H525V842H433V932ZM161 832H423V390H161ZM140 215V114H242V215ZM353 215V114H454V215Z"},
    {static_cast<wchar_t>(215), 600, L"M174 808 103 737 232 610 96 475 167 404 303 539 432 411 503 482 375 610 495 729 424 800 303 681Z"},
    {static_cast<wchar_t>(216), 623, L"M60 972 460 250H562L162 972ZM168 932V842H77V380H168V289H450V380H542V842H450V932ZM178 832H440V390H178Z"},
    {static_cast<wchar_t>(217), 586, L"M151 932V842H60V289H161V832H423V289H525V842H433V932ZM266 215V171H220V125H175V70H230V116H276V161H321V215Z"},
    {static_cast<wchar_t>(218), 586, L"M151 932V842H60V289H161V832H423V289H525V842H433V932ZM205 215V161H250V116H296V70H351V125H306V171H260V215Z"},
    {static_cast<wchar_t>(219), 586, L"M151 932V842H60V289H161V832H423V289H525V842H433V932ZM220 215V160H265V114H321V160H366V215H311V169H275V215Z"},
    {static_cast<wchar_t>(220), 586, L"M151 932V842H60V289H161V832H423V289H525V842H433V932ZM140 215V114H242V215ZM353 215V114H454V215Z"},
    {static_cast<wchar_t>(221), 586, L"M241 932V570H151V480H60V289H161V470H251V561H333V470H423V289H525V480H433V570H343V932ZM205 215V161H250V116H296V70H351V125H306V171H260V215Z"},
    {static_cast<wchar_t>(222), 586, L"M60 932V289H161V391H433V482H525V763H433V853H161V932ZM161 753H423V491H161Z"},
    {static_cast<wchar_t>(223), 586, L"M60 932V289H433V380H525V570H433V650H525V842H433V932H241V832H423V660H241V561H423V390H161V932Z"},
    {static_cast<wchar_t>(224), 677, L"M151 932V842H60V561H151V470H433V561H525V832H616V932H515V842H433V932ZM161 832H423V570H161ZM275 396V352H229V306H184V251H239V297H285V342H330V396Z"},
    {static_cast<wchar_t>(225), 677, L"M151 932V842H60V561H151V470H433V561H525V832H616V932H515V842H433V932ZM161 832H423V570H161ZM214 396V342H259V297H305V251H360V306H315V352H269V396Z"},
    {static_cast<wchar_t>(226), 677, L"M151 932V842H60V561H151V470H433V561H525V832H616V932H515V842H433V932ZM161 832H423V570H161ZM229 396V341H274V295H330V341H375V396H320V350H284V396Z"},
    {static_cast<wchar_t>(227), 677, L"M151 932V842H60V561H151V470H433V561H525V832H616V932H515V842H433V932ZM161 832H423V570H161ZM171 396V293H323V339H360V293H418V396H265V349H228V396Z"},
    {static_cast<wchar_t>(228), 677, L"M151 932V842H60V561H151V470H433V561H525V832H616V932H515V842H433V932ZM161 832H423V570H161ZM149 396V295H251V396ZM362 396V295H463V396Z"},
    {static_cast<wchar_t>(229), 677, L"M151 932V842H60V561H151V470H433V561H525V832H616V932H515V842H433V932ZM161 832H423V570H161ZM250 396V295H351V396ZM283 363H318V328H283Z"},
    {static_cast<wchar_t>(230), 1132, L"M151 932V842H60V561H151V470H433V561H525V832H606V561H698V470H980V561H1071V660H970V570H707V650H889V751H707V832H970V740H1071V841H980V932H698V842H616V932H515V842H433V932ZM161 832H423V570H161Z"},
    {static_cast<wchar_t>(231), 586, L"M151 932V842H60V561H151V470H433V561H525V660H423V570H161V832H423V741H525V842H433V932ZM221 1120V1075H175V1020H231V1065H357V1030H311V985H266V883H321V975H367V1020H412V1075H367V1120Z"},
    {static_cast<wchar_t>(232), 586, L"M151 932V842H60V561H151V470H433V561H525V660H423V570H161V650H343V751H161V832H423V740H525V841H433V932ZM266 396V352H220V306H175V251H230V297H276V342H321V396Z"},
    {static_cast<wchar_t>(233), 586, L"M151 932V842H60V561H151V470H433V561H525V660H423V570H161V650H343V751H161V832H423V740H525V841H433V932ZM205 396V342H250V297H296V251H351V306H306V352H260V396Z"},
    {static_cast<wchar_t>(234), 586, L"M151 932V842H60V561H151V470H433V561H525V660H423V570H161V650H343V751H161V832H423V740H525V841H433V932ZM220 396V341H265V295H321V341H366V396H311V350H275V396Z"},
    {static_cast<wchar_t>(235), 586, L"M151 932V842H60V561H151V470H433V561H525V660H423V570H161V650H343V751H161V832H423V740H525V841H433V932ZM140 396V295H242V396ZM353 396V295H454V396Z"},
    {static_cast<wchar_t>(236), 222, L"M60 932V470H161V932ZM84 396V352H38V306H-7V251H48V297H94V342H139V396Z"},
    {static_cast<wchar_t>(237), 222, L"M60 932V470H161V932ZM23 396V342H68V297H114V251H169V306H124V352H78V396Z"},
    {static_cast<wchar_t>(238), 222, L"M60 932V470H161V932ZM38 396V341H83V295H139V341H184V396H129V350H93V396Z"},
    {static_cast<wchar_t>(239), 222, L"M60 932V470H161V932ZM-16 396V295H86V396ZM137 396V295H238V396Z"},
    {static_cast<wchar_t>(240), 605, L"M152 932V842H60V650H152V561H424V390H162V480H60V380H152V289H434V380H525V842H434V932ZM162 832H424V660H162ZM259 500 459 220H560L360 500Z"},
    {static_cast<wchar_t>(241), 586, L"M60 932V470H433V561H525V932H423V570H161V932ZM162 396V293H314V339H351V293H409V396H256V349H219V396Z"},
    {static_cast<wchar_t>(242), 586, L"M151 932V842H60V561H151V470H433V561H525V842H433V932ZM161 832H423V570H161ZM266 396V352H220V306H175V251H230V297H276V342H321V396Z"},
    {static_cast<wchar_t>(243), 586, L"M151 932V842H60V561H151V470H433V561H525V842H433V932ZM161 832H423V570H161ZM205 396V342H250V297H296V251H351V306H306V352H260V396Z"},
    {static_cast<wchar_t>(244), 586, L"M151 932V842H60V561H151V470H433V561H525V842H433V932ZM161 832H423V570H161ZM220 396V341H265V295H321V341H366V396H311V350H275V396Z"},
    {static_cast<wchar_t>(245), 586, L"M151 932V842H60V561H151V470H433V561H525V842H433V932ZM161 832H423V570H161ZM162 396V293H314V339H351V293H409V396H256V349H219V396Z"},
    {static_cast<wchar_t>(246), 586, L"M151 932V842H60V561H151V470H433V561H525V842H433V932ZM161 832H423V570H161ZM140 396V295H242V396ZM353 396V295H454V396Z"},
    {static_cast<wchar_t>(247), 600, L"M109 679V578H490V679ZM249 839V738H350V839ZM249 513V413H350V513Z"},
    {static_cast<wchar_t>(248), 586, L"M21 1009 451 409H553L123 1009ZM151 932V842H60V561H151V470H433V561H525V842H433V932ZM161 832H423V570H161Z"},
    {static_cast<wchar_t>(249), 586, L"M151 932V842H60V470H161V832H423V470H525V842H433V932ZM266 396V352H220V306H175V251H230V297H276V342H321V396Z"},
    {static_cast<wchar_t>(250), 586, L"M151 932V842H60V470H161V832H423V470H525V842H433V932ZM205 396V342H250V297H296V251H351V306H306V352H260V396Z"},
    {static_cast<wchar_t>(251), 586, L"M151 932V842H60V470H161V832H423V470H525V842H433V932ZM220 396V341H265V295H321V341H366V396H311V350H275V396Z"},
    {static_cast<wchar_t>(252), 586, L"M151 932V842H60V470H161V832H423V470H525V842H433V932ZM140 396V295H242V396ZM353 396V295H454V396Z"},
    {static_cast<wchar_t>(253), 586, L"M151 1113V1023H60V923H161V1013H424V842H151V751H60V470H161V741H424V470H525V1023H434V1113ZM205 396V342H250V297H296V251H351V306H306V352H260V396Z"},
    {static_cast<wchar_t>(254), 586, L"M60 932V289H161V416H433V507H525V789H433V878H161V932ZM161 779H423V517H161Z"},
    {static_cast<wchar_t>(255), 586, L"M151 1113V1023H60V923H161V1013H424V842H151V751H60V470H161V741H424V470H525V1023H434V1113ZM140 396V295H242V396ZM353 396V295H454V396Z"},
    {static_cast<wchar_t>(256), 586, L"M60 932V470H151V380H241V289H343V380H433V470H525V932H423V751H161V932ZM161 650H423V480H333V390H251V480H161ZM155 216V136H437V216Z"},
    {static_cast<wchar_t>(257), 677, L"M151 932V842H60V561H151V470H433V561H525V832H616V932H515V842H433V932ZM161 832H423V570H161ZM164 396V316H446V396Z"},
    {static_cast<wchar_t>(258), 586, L"M60 932V470H151V380H241V289H343V380H433V470H525V932H423V751H161V932ZM161 650H423V480H333V390H251V480H161ZM169 216V115H224V161H358V115H412V216Z"},
    {static_cast<wchar_t>(259), 677, L"M151 932V842H60V561H151V470H433V561H525V832H616V932H515V842H433V932ZM161 832H423V570H161ZM178 396V295H233V341H367V295H421V396Z"},
    {static_cast<wchar_t>(260), 586, L"M60 932V470H151V380H241V289H343V380H433V470H525V932H423V751H161V932ZM161 650H423V480H333V390H251V480H161ZM404 1132V1087H358V1032H404V987H449V894H504V997H459V1042H414V1077H540V1032H595V1087H550V1132Z"},
    {static_cast<wchar_t>(261), 677, L"M151 932V842H60V561H151V470H433V561H525V832H616V932H515V842H433V932ZM161 832H423V570H161ZM494 1120V1075H448V1020H494V975H539V882H594V985H549V1030H504V1065H630V1020H685V1075H640V1120Z"},
    {static_cast<wchar_t>(262), 586, L"M151 932V842H60V380H151V289H433V380H525V570H423V390H161V832H423V650H525V842H433V932ZM205 215V161H250V116H296V70H351V125H306V171H260V215Z"},
    {static_cast<wchar_t>(263), 586, L"M151 932V842H60V561H151V470H433V561H525V660H423V570H161V832H423V741H525V842H433V932ZM205 396V342H250V297H296V251H351V306H306V352H260V396Z"},
    {static_cast<wchar_t>(264), 586, L"M151 932V842H60V380H151V289H433V380H525V570H423V390H161V832H423V650H525V842H433V932ZM220 215V160H265V114H321V160H366V215H311V169H275V215Z"},
    {static_cast<wchar_t>(265), 586, L"M151 932V842H60V561H151V470H433V561H525V660H423V570H161V832H423V741H525V842H433V932ZM220 396V341H265V295H321V341H366V396H311V350H275V396Z"},
    {static_cast<wchar_t>(266), 586, L"M151 932V842H60V380H151V289H433V380H525V570H423V390H161V832H423V650H525V842H433V932ZM242 215V114H344V215Z"},
    {static_cast<wchar_t>(267), 586, L"M151 932V842H60V561H151V470H433V561H525V660H423V570H161V832H423V741H525V842H433V932ZM242 396V295H344V396Z"},
    {static_cast<wchar_t>(268), 586, L"M151 932V842H60V380H151V289H433V380H525V570H423V390H161V832H423V650H525V842H433V932ZM265 215V169H220V114H275V160H311V114H366V169H321V215Z"},
    {static_cast<wchar_t>(269), 586, L"M151 932V842H60V561H151V470H433V561H525V660H423V570H161V832H423V741H525V842H433V932ZM265 396V350H220V295H275V341H311V295H366V350H321V396Z"},
    {static_cast<wchar_t>(270), 586, L"M60 932V289H343V380H433V470H525V751H433V842H343V932ZM161 832H333V741H423V480H333V390H161ZM222 215V169H177V114H232V160H268V114H323V169H278V215Z"},
    {static_cast<wchar_t>(271), 586, L"M151 932V842H60V561H151V470H424V289H525V842H433V932ZM161 832H423V570H161ZM591 479V423H637V389H546V288H647V379H692V433H647V479Z"},
    {static_cast<wchar_t>(272), 586, L"M60 932V679H22V578H60V289H343V380H433V470H525V751H433V842H343V932ZM161 832H333V741H423V480H333V390H161V578H254V679H161Z"},
    {static_cast<wchar_t>(273), 585, L"M151 932V842H60V561H151V470H424V425H221V325H424V289H526V325H552V425H526V842H433V932ZM161 832H423V570H161Z"},
    {static_cast<wchar_t>(274), 586, L"M151 932V842H60V380H151V289H433V380H525V480H423V390H161V561H343V660H161V832H423V741H525V842H433V932ZM155 215V135H437V215Z"},
    {static_cast<wchar_t>(275), 586, L"M151 932V842H60V561H151V470H433V561H525V660H423V570H161V650H343V751H161V832H423V740H525V841H433V932ZM155 396V316H437V396Z"},
    {static_cast<wchar_t>(276), 586, L"M151 932V842H60V380H151V289H433V380H525V480H423V390H161V561H343V660H161V832H423V741H525V842H433V932ZM169 215V114H224V160H358V114H412V215Z"},
    {static_cast<wchar_t>(277), 586, L"M151 932V842H60V561H151V470H433V561H525V660H423V570H161V650H343V751H161V832H423V740H525V841H433V932ZM169 396V295H224V341H358V295H412V396Z"},
    {static_cast<wchar_t>(278), 586, L"M151 932V842H60V380H151V289H433V380H525V480H423V390H161V561H343V660H161V832H423V741H525V842H433V932ZM242 215V114H344V215Z"},
    {static_cast<wchar_t>(279), 586, L"M151 932V842H60V561H151V470H433V561H525V660H423V570H161V650H343V751H161V832H423V740H525V841H433V932ZM242 396V295H344V396Z"},
    {static_cast<wchar_t>(280), 586, L"M151 932V842H60V380H151V289H433V380H525V480H423V390H161V561H343V660H161V832H423V741H525V842H433V932ZM221 1132V1087H175V1032H221V987H266V894H321V997H276V1042H231V1077H357V1032H412V1087H367V1132Z"},
    {static_cast<wchar_t>(281), 586, L"M151 932V842H60V561H151V470H433V561H525V660H423V570H161V650H343V751H161V832H423V740H525V841H433V932ZM222 1120V1075H176V1020H222V975H267V882H322V985H277V1030H232V1065H358V1020H413V1075H368V1120Z"},
    {static_cast<wchar_t>(282), 586, L"M151 932V842H60V380H151V289H433V380H525V480H423V390H161V561H343V660H161V832H423V741H525V842H433V932ZM265 215V169H220V114H275V160H311V114H366V169H321V215Z"},
    {static_cast<wchar_t>(283), 586, L"M151 932V842H60V561H151V470H433V561H525V660H423V570H161V650H343V751H161V832H423V740H525V841H433V932ZM265 396V350H220V295H275V341H311V295H366V350H321V396Z"},
    {static_cast<wchar_t>(284), 586, L"M151 932V842H60V380H151V289H433V380H525V570H423V390H161V832H423V751H333V660H241V561H343V650H525V842H433V932ZM220 215V160H265V114H321V160H366V215H311V169H275V215Z"},
    {static_cast<wchar_t>(285), 586, L"M151 1113V1023H60V923H161V1013H424V842H151V751H60V561H151V470H434V561H525V1023H434V1113ZM161 741H424V570H161ZM220 396V341H265V295H321V341H366V396H311V350H275V396Z"},
    {static_cast<wchar_t>(286), 586, L"M151 932V842H60V380H151V289H433V380H525V570H423V390H161V832H423V751H333V660H241V561H343V650H525V842H433V932ZM169 215V114H224V160H358V114H412V215Z"},
    {static_cast<wchar_t>(287), 586, L"M151 1113V1023H60V923H161V1013H424V842H151V751H60V561H151V470H434V561H525V1023H434V1113ZM161 741H424V570H161ZM169 396V295H224V341H358V295H412V396Z"},
    {static_cast<wchar_t>(288), 586, L"M151 932V842H60V380H151V289H433V380H525V570H423V390H161V832H423V751H333V660H241V561H343V650H525V842H433V932ZM242 215V114H344V215Z"},
    {static_cast<wchar_t>(289), 586, L"M151 1113V1023H60V923H161V1013H424V842H151V751H60V561H151V470H434V561H525V1023H434V1113ZM161 741H424V570H161ZM242 396V295H344V396Z"},
    {static_cast<wchar_t>(290), 586, L"M151 932V842H60V380H151V289H433V380H525V570H423V390H161V832H423V751H333V660H241V561H343V650H525V842H433V932ZM280 1164V1108H326V1074H235V973H336V1064H381V1118H336V1164Z"},
    {static_cast<wchar_t>(291), 586, L"M151 1113V1023H60V923H161V1013H424V842H151V751H60V561H151V470H434V561H525V1023H434V1113ZM161 741H424V570H161ZM265 396V305H220V251H265V205H321V261H275V295H366V396Z"},
    {static_cast<wchar_t>(292), 586, L"M60 932V289H161V561H241V470H343V561H423V289H525V932H423V660H161V932ZM220 215V160H265V114H321V160H366V215H311V169H275V215Z"},
    {static_cast<wchar_t>(293), 586, L"M60 932V289H161V561H241V470H433V561H525V932H423V570H251V660H161V932ZM220 215V160H265V114H321V160H366V215H311V169H275V215Z"},
    {static_cast<wchar_t>(294), 586, L"M60 932V289H161V326H423V289H525V932H423V660H161V932ZM161 561H241V470H343V561H423V426H161Z"},
    {static_cast<wchar_t>(295), 586, L"M60 932V422H30V322H60V289H161V322H262V422H161V561H241V470H433V561H525V932H423V570H251V660H161V932Z"},
    {static_cast<wchar_t>(296), 404, L"M60 932V832H151V390H60V289H343V390H251V832H343V932ZM71 215V112H223V158H260V112H318V215H165V168H128V215Z"},
    {static_cast<wchar_t>(297), 222, L"M60 932V470H161V932ZM-20 396V293H132V339H169V293H227V396H74V349H37V396Z"},
    {static_cast<wchar_t>(298), 404, L"M60 932V832H151V390H60V289H343V390H251V832H343V932ZM64 215V135H346V215Z"},
    {static_cast<wchar_t>(299), 222, L"M60 932V470H161V932ZM-27 396V316H255V396Z"},
    {static_cast<wchar_t>(300), 404, L"M60 932V832H151V390H60V289H343V390H251V832H343V932ZM78 215V114H133V160H267V114H321V215Z"},
    {static_cast<wchar_t>(301), 222, L"M60 932V470H161V932ZM-13 380V279H42V325H176V279H231V380Z"},
    {static_cast<wchar_t>(302), 404, L"M60 932V832H151V390H60V289H343V390H251V832H343V932ZM126 1132V1087H80V1032H126V987H171V894H226V997H181V1042H136V1077H262V1032H317V1087H272V1132Z"},
    {static_cast<wchar_t>(303), 222, L"M60 932V470H161V932ZM60 396V295H162V396ZM38 1132V1087H-8V1032H38V987H83V894H138V997H93V1042H48V1077H174V1032H229V1087H184V1132Z"},
    {static_cast<wchar_t>(304), 404, L"M60 932V832H151V390H60V289H343V390H251V832H343V932ZM151 215V114H253V215Z"},
    {static_cast<wchar_t>(305), 222, L"M60 932V470H161V932Z"},
    {static_cast<wchar_t>(306), 990, L"M60 932V832H151V390H60V289H343V390H251V832H343V932ZM555 932V842H464V650H565V832H827V390H555V289H837V380H929V842H837V932Z"},
    {static_cast<wchar_t>(307), 697, L"M353 933V842H262V741H363V832H535V470H636V842H545V933ZM60 932V470H161V932Z"},
    {static_cast<wchar_t>(308), 586, L"M151 932V842H60V650H161V832H423V390H151V289H433V380H525V842H433V932ZM220 215V160H265V114H321V160H366V215H311V169H275V215Z"},
    {static_cast<wchar_t>(309), 495, L"M151 933V842H60V741H161V832H333V470H434V842H343V933ZM308 396V341H353V295H409V341H454V396H399V350H363V396Z"},
    {static_cast<wchar_t>(310), 586, L"M60 932V380H151V289H251V390H161V561H241V470H333V380H423V289H525V390H433V480H343V570H251V650H343V741H433V832H525V932H423V842H333V751H241V660H161V932ZM280 1164V1108H326V1074H235V973H336V1064H381V1118H336V1164Z"},
    {static_cast<wchar_t>(311), 586, L"M60 932V289H161V650H241V561H423V470H525V570H433V660H251V741H433V832H525V932H423V842H241V751H161V932ZM280 1164V1108H326V1074H235V973H336V1064H381V1118H336V1164Z"},
    {static_cast<wchar_t>(313), 586, L"M151 932V842H60V289H161V832H423V741H525V842H433V932ZM205 215V161H250V116H296V70H351V125H306V171H260V215Z"},
    {static_cast<wchar_t>(314), 222, L"M60 932V289H161V932ZM23 215V161H68V116H114V70H169V125H124V171H78V215Z"},
    {static_cast<wchar_t>(315), 586, L"M151 932V842H60V289H161V832H423V741H525V842H433V932ZM280 1164V1108H326V1074H235V973H336V1064H381V1118H336V1164Z"},
    {static_cast<wchar_t>(316), 222, L"M60 932V289H161V932ZM98 1164V1108H144V1074H53V973H154V1064H199V1118H154V1164Z"},
    {static_cast<wchar_t>(317), 586, L"M151 932V842H60V289H161V832H423V741H525V842H433V932ZM374 480V424H420V390H329V289H430V380H475V434H430V480Z"},
    {static_cast<wchar_t>(318), 222, L"M60 932V289H161V932ZM227 480V424H273V390H182V289H283V380H328V434H283V480Z"},
    {static_cast<wchar_t>(319), 586, L"M151 932V842H60V289H161V832H423V741H525V842H433V932ZM243 504V403H344V504Z"},
    {static_cast<wchar_t>(320), 222, L"M60 932V289H161V932ZM60 920V819H161V920Z"},
    {static_cast<wchar_t>(321), 587, L"M153 932V842H62V726L31 768L-24 726L62 613V289H163V478L226 395L280 435L163 591V832H425V741H527V842H435V932Z"},
    {static_cast<wchar_t>(322), 225, L"M62 932V760L38 800L-21 766L62 629V289H163V459L188 416L246 451L163 590V932Z"},
    {static_cast<wchar_t>(323), 586, L"M60 932V289H161V380H251V470H343V741H423V289H525V932H423V842H333V751H241V480H161V932ZM205 215V161H250V116H296V70H351V125H306V171H260V215Z"},
    {static_cast<wchar_t>(324), 586, L"M60 932V470H433V561H525V932H423V570H161V932ZM205 396V342H250V297H296V251H351V306H306V352H260V396Z"},
    {static_cast<wchar_t>(325), 586, L"M60 932V289H161V380H251V470H343V741H423V289H525V932H423V842H333V751H241V480H161V932ZM280 1164V1108H326V1074H235V973H336V1064H381V1118H336V1164Z"},
    {static_cast<wchar_t>(326), 586, L"M60 932V470H433V561H525V932H423V570H161V932ZM280 1164V1108H326V1074H235V973H336V1064H381V1118H336V1164Z"},
    {static_cast<wchar_t>(327), 586, L"M60 932V289H161V380H251V470H343V741H423V289H525V932H423V842H333V751H241V480H161V932ZM265 215V169H220V114H275V160H311V114H366V169H321V215Z"},
    {static_cast<wchar_t>(328), 586, L"M60 932V470H433V561H525V932H423V570H161V932ZM265 396V350H220V295H275V341H311V295H366V350H321V396Z"},
    {static_cast<wchar_t>(329), 673, L"M147 932V470H520V561H612V932H510V570H248V932ZM55 480V424H101V390H10V289H111V380H156V434H111V480Z"},
    {static_cast<wchar_t>(330), 586, L"M241 1168V1077H150V976H251V1067H423V842H333V751H241V480H161V932H60V289H161V380H251V470H343V741H423V289H525V1077H433V1168Z"},
    {static_cast<wchar_t>(331), 586, L"M241 1167V1076H150V975H251V1066H423V570H161V932H60V470H433V561H525V1076H433V1167Z"},
    {static_cast<wchar_t>(332), 586, L"M151 932V842H60V380H151V289H433V380H525V842H433V932ZM161 832H423V390H161ZM155 215V135H437V215Z"},
    {static_cast<wchar_t>(333), 586, L"M151 932V842H60V561H151V470H433V561H525V842H433V932ZM161 832H423V570H161ZM155 396V316H437V396Z"},
    {static_cast<wchar_t>(334), 586, L"M151 932V842H60V380H151V289H433V380H525V842H433V932ZM161 832H423V390H161ZM169 215V114H224V160H358V114H412V215Z"},
    {static_cast<wchar_t>(335), 586, L"M151 932V842H60V561H151V470H433V561H525V842H433V932ZM161 832H423V570H161ZM169 396V295H224V341H358V295H412V396Z"},
    {static_cast<wchar_t>(336), 586, L"M151 932V842H60V380H151V289H433V380H525V842H433V932ZM161 832H423V390H161ZM367 215V171H321V125H276V70H331V116H377V161H422V215ZM248 215V171H202V125H157V70H212V116H258V161H303V215Z"},
    {static_cast<wchar_t>(337), 586, L"M151 932V842H60V561H151V470H433V561H525V842H433V932ZM161 832H423V570H161ZM367 396V352H321V306H276V251H331V297H377V342H422V396ZM248 396V352H202V306H157V251H212V297H258V342H303V396Z"},
    {static_cast<wchar_t>(338), 949, L"M151 932V842H60V380H151V289H433V380H515V289H797V380H888V480H787V390H525V561H706V660H525V832H787V741H888V842H797V932H515V842H433V932ZM161 832H423V390H161Z"},
    {static_cast<wchar_t>(339), 949, L"M151 932V842H60V561H151V470H433V561H515V470H797V561H888V660H787V570H525V650H706V751H525V832H787V740H888V841H797V932H515V842H433V932ZM161 832H423V570H161Z"},
    {static_cast<wchar_t>(340), 586, L"M60 932V380H151V289H433V380H525V660H433V751H389V832H479V932H379V842H287V751H161V932ZM161 650H423V390H161ZM205 215V161H250V116H296V70H351V125H306V171H260V215Z"},
    {static_cast<wchar_t>(341), 586, L"M60 932V561H151V470H433V561H525V660H423V570H161V932ZM205 396V342H250V297H296V251H351V306H306V352H260V396Z"},
    {static_cast<wchar_t>(342), 586, L"M60 932V380H151V289H433V380H525V660H433V751H389V832H479V932H379V842H287V751H161V932ZM161 650H423V390H161ZM280 1164V1108H326V1074H235V973H336V1064H381V1118H336V1164Z"},
    {static_cast<wchar_t>(343), 586, L"M60 932V561H151V470H433V561H525V660H423V570H161V932ZM280 1164V1108H326V1074H235V973H336V1064H381V1118H336V1164Z"},
    {static_cast<wchar_t>(344), 586, L"M60 932V380H151V289H433V380H525V660H433V751H389V832H479V932H379V842H287V751H161V932ZM161 650H423V390H161ZM265 215V169H220V114H275V160H311V114H366V169H321V215Z"},
    {static_cast<wchar_t>(345), 586, L"M60 932V561H151V470H433V561H525V660H423V570H161V932ZM265 396V350H220V295H275V341H311V295H366V350H321V396Z"},
    {static_cast<wchar_t>(346), 586, L"M151 932V842H60V741H161V832H423V660H151V570H60V380H151V289H433V380H525V480H423V390H161V561H433V650H525V842H433V932ZM205 215V161H250V116H296V70H351V125H306V171H260V215Z"},
    {static_cast<wchar_t>(347), 586, L"M60 932V832H423V751H151V660H60V561H151V470H525V570H161V650H433V741H525V842H433V932ZM245 396V342H290V297H336V251H391V306H346V352H300V396Z"},
    {static_cast<wchar_t>(348), 586, L"M151 932V842H60V741H161V832H423V660H151V570H60V380H151V289H433V380H525V480H423V390H161V561H433V650H525V842H433V932ZM220 215V160H265V114H321V160H366V215H311V169H275V215Z"},
    {static_cast<wchar_t>(349), 586, L"M60 932V832H423V751H151V660H60V561H151V470H525V570H161V650H433V741H525V842H433V932ZM260 396V341H305V295H361V341H406V396H351V350H315V396Z"},
    {static_cast<wchar_t>(350), 586, L"M151 932V842H60V741H161V832H423V660H151V570H60V380H151V289H433V380H525V480H423V390H161V561H433V650H525V842H433V932ZM221 1120V1075H175V1020H231V1065H357V1030H311V985H266V883H321V975H367V1020H412V1075H367V1120Z"},
    {static_cast<wchar_t>(351), 586, L"M60 932V832H423V751H151V660H60V561H151V470H525V570H161V650H433V741H525V842H433V932ZM221 1120V1075H175V1020H231V1065H357V1030H311V985H266V883H321V975H367V1020H412V1075H367V1120Z"},
    {static_cast<wchar_t>(352), 586, L"M151 932V842H60V741H161V832H423V660H151V570H60V380H151V289H433V380H525V480H423V390H161V561H433V650H525V842H433V932ZM265 215V169H220V114H275V160H311V114H366V169H321V215Z"},
    {static_cast<wchar_t>(353), 586, L"M60 932V832H423V751H151V660H60V561H151V470H525V570H161V650H433V741H525V842H433V932ZM305 396V350H260V295H315V341H351V295H406V350H361V396Z"},
    {static_cast<wchar_t>(354), 586, L"M241 932V390H161V480H60V380H151V289H433V380H525V480H423V390H343V932ZM221 1120V1075H175V1020H231V1065H357V1030H311V985H266V883H321V975H367V1020H412V1075H367V1120Z"},
    {static_cast<wchar_t>(355), 677, L"M241 932V842H151V570H60V470H151V289H251V470H525V570H251V832H515V741H616V842H525V932ZM298 1120V1075H252V1020H308V1065H434V1030H388V985H343V883H398V975H444V1020H489V1075H444V1120Z"},
    {static_cast<wchar_t>(356), 586, L"M241 932V390H161V480H60V380H151V289H433V380H525V480H423V390H343V932ZM265 215V169H220V114H275V160H311V114H366V169H321V215Z"},
    {static_cast<wchar_t>(357), 677, L"M241 932V842H151V570H60V470H151V289H251V470H525V570H251V832H515V741H616V842H525V932ZM582 480V424H628V390H537V289H638V380H683V434H638V480Z"},
    {static_cast<wchar_t>(358), 586, L"M241 932V624H151V523H241V390H161V480H60V380H151V289H433V380H525V480H423V390H343V523H433V624H343V932Z"},
    {static_cast<wchar_t>(359), 677, L"M241 932V842H151V765H60V664H151V570H60V470H151V289H251V470H525V570H251V664H342V765H251V832H515V741H616V842H525V932Z"},
    {static_cast<wchar_t>(360), 586, L"M151 932V842H60V289H161V832H423V289H525V842H433V932ZM162 215V112H314V158H351V112H409V215H256V168H219V215Z"},
    {static_cast<wchar_t>(361), 586, L"M151 932V842H60V470H161V832H423V470H525V842H433V932ZM162 396V293H314V339H351V293H409V396H256V349H219V396Z"},
    {static_cast<wchar_t>(362), 586, L"M151 932V842H60V289H161V832H423V289H525V842H433V932ZM155 215V135H437V215Z"},
    {static_cast<wchar_t>(363), 586, L"M151 932V842H60V470H161V832H423V470H525V842H433V932ZM155 396V316H437V396Z"},
    {static_cast<wchar_t>(364), 586, L"M151 932V842H60V289H161V832H423V289H525V842H433V932ZM169 215V114H224V160H358V114H412V215Z"},
    {static_cast<wchar_t>(365), 586, L"M151 932V842H60V470H161V832H423V470H525V842H433V932ZM169 396V295H224V341H358V295H412V396Z"},
    {static_cast<wchar_t>(366), 586, L"M151 932V842H60V289H161V832H423V289H525V842H433V932ZM241 215V114H342V215ZM274 182H309V147H274Z"},
    {static_cast<wchar_t>(367), 586, L"M151 932V842H60V470H161V832H423V470H525V842H433V932ZM241 396V295H342V396ZM274 363H309V328H274Z"},
    {static_cast<wchar_t>(368), 586, L"M151 932V842H60V289H161V832H423V289H525V842H433V932ZM367 215V171H321V125H276V70H331V116H377V161H422V215ZM248 215V171H202V125H157V70H212V116H258V161H303V215Z"},
    {static_cast<wchar_t>(369), 586, L"M151 932V842H60V470H161V832H423V470H525V842H433V932ZM367 396V352H321V306H276V251H331V297H377V342H422V396ZM248 396V352H202V306H157V251H212V297H258V342H303V396Z"},
    {static_cast<wchar_t>(370), 586, L"M151 932V842H60V289H161V832H423V289H525V842H433V932ZM219 1132V1087H173V1032H219V987H264V894H319V997H274V1042H229V1077H355V1032H410V1087H365V1132Z"},
    {static_cast<wchar_t>(371), 586, L"M151 932V842H60V470H161V832H423V470H525V842H433V932ZM225 1132V1087H179V1032H225V987H270V894H325V997H280V1042H235V1077H361V1032H416V1087H371V1132Z"},
    {static_cast<wchar_t>(372), 767, L"M151 932V842H60V289H161V832H241V650H333V380H433V650H525V832H606V289H706V842H616V932H515V842H423V660H343V842H251V932ZM311 215V160H356V114H412V160H457V215H402V169H366V215Z"},
    {static_cast<wchar_t>(373), 767, L"M60 932V470H161V832H333V470H433V832H606V470H706V842H616V932H423V842H343V932ZM311 396V341H356V295H412V341H457V396H402V350H366V396Z"},
    {static_cast<wchar_t>(374), 586, L"M241 932V570H151V480H60V289H161V470H251V561H333V470H423V289H525V480H433V570H343V932ZM220 215V160H265V114H321V160H366V215H311V169H275V215Z"},
    {static_cast<wchar_t>(375), 586, L"M151 1113V1023H60V923H161V1013H424V842H151V751H60V470H161V741H424V470H525V1023H434V1113ZM220 396V341H265V295H321V341H366V396H311V350H275V396Z"},
    {static_cast<wchar_t>(376), 586, L"M241 932V570H151V480H60V289H161V470H251V561H333V470H423V289H525V480H433V570H343V932ZM140 215V114H242V215ZM353 215V114H454V215Z"},
    {static_cast<wchar_t>(377), 586, L"M151 932V842H60V650H151V561H423V390H161V480H60V289H433V380H525V570H433V660H161V832H423V741H525V932ZM205 215V161H250V116H296V70H351V125H306V171H260V215Z"},
    {static_cast<wchar_t>(378), 586, L"M60 932V832H151V741H241V650H333V570H60V470H525V570H433V660H343V751H251V832H525V932ZM205 396V342H250V297H296V251H351V306H306V352H260V396Z"},
    {static_cast<wchar_t>(379), 586, L"M151 932V842H60V650H151V561H423V390H161V480H60V289H433V380H525V570H433V660H161V832H423V741H525V932ZM242 215V114H344V215Z"},
    {static_cast<wchar_t>(380), 586, L"M60 932V832H151V741H241V650H333V570H60V470H525V570H433V660H343V751H251V832H525V932ZM242 396V295H344V396Z"},
    {static_cast<wchar_t>(381), 586, L"M151 932V842H60V650H151V561H423V390H161V480H60V289H433V380H525V570H433V660H161V832H423V741H525V932ZM265 215V169H220V114H275V160H311V114H366V169H321V215Z"},
    {static_cast<wchar_t>(382), 586, L"M60 932V832H151V741H241V650H333V570H60V470H525V570H433V660H343V751H251V832H525V932ZM265 396V350H220V295H275V341H311V295H366V350H321V396Z"},
    {static_cast<wchar_t>(402), 586, L"M-32 1023V922H60V380H151V289H433V380H525V480H423V390H161V561H343V660H161V1023Z"},
    {static_cast<wchar_t>(508), 949, L"M60 932V470H151V380H241V289H343V380H515V289H797V380H888V480H787V390H525V561H706V660H525V832H787V741H888V842H797V932H423V751H161V932ZM161 650H423V480H333V390H251V480H161ZM412 228V174H457V129H503V83H558V138H513V184H467V228Z"},
    {static_cast<wchar_t>(509), 1132, L"M151 932V842H60V561H151V470H433V561H525V832H606V561H698V470H980V561H1071V660H970V570H707V650H889V751H707V832H970V740H1071V841H980V932H698V842H616V932H515V842H433V932ZM161 832H423V570H161ZM537 396V342H582V297H628V251H683V306H638V352H592V396Z"},
    {static_cast<wchar_t>(510), 623, L"M60 972 460 250H562L162 972ZM168 932V842H77V380H168V289H450V380H542V842H450V932ZM178 832H440V390H178ZM224 146V92H269V47H315V1H370V56H325V102H279V146Z"},
    {static_cast<wchar_t>(511), 586, L"M21 1009 451 409H553L123 1009ZM151 932V842H60V561H151V470H433V561H525V842H433V932ZM161 832H423V570H161ZM205 396V342H250V297H296V251H351V306H306V352H260V396Z"},
    {static_cast<wchar_t>(536), 586, L"M151 932V842H60V741H161V832H423V660H151V570H60V380H151V289H433V380H525V480H423V390H161V561H433V650H525V842H433V932ZM280 1164V1108H326V1074H235V973H336V1064H381V1118H336V1164Z"},
    {static_cast<wchar_t>(537), 586, L"M60 932V832H423V751H151V660H60V561H151V470H525V570H161V650H433V741H525V842H433V932ZM280 1164V1108H326V1074H235V973H336V1064H381V1118H336V1164Z"},
    {static_cast<wchar_t>(538), 586, L"M241 932V390H161V480H60V380H151V289H433V380H525V480H423V390H343V932ZM280 1164V1108H326V1074H235V973H336V1064H381V1118H336V1164Z"},
    {static_cast<wchar_t>(539), 677, L"M241 932V842H151V570H60V470H151V289H251V470H525V570H251V832H515V741H616V842H525V932ZM357 1164V1108H403V1074H312V973H413V1064H458V1118H413V1164Z"},
    {static_cast<wchar_t>(567), 495, L"M151 933V842H60V741H161V832H333V470H434V842H343V933Z"},
    {static_cast<wchar_t>(710), 300, L"M77 396V341H122V295H178V341H223V396H168V350H132V396Z"},
    {static_cast<wchar_t>(711), 300, L"M122 396V350H77V295H132V341H168V295H223V350H178V396Z"},
    {static_cast<wchar_t>(728), 300, L"M24 380V279H79V325H213V279H267V380Z"},
    {static_cast<wchar_t>(729), 300, L"M108 379V278H210V379Z"},
    {static_cast<wchar_t>(730), 300, L"M94 379V278H195V379ZM127 346H162V311H127Z"},
    {static_cast<wchar_t>(731), 300, L"M78 1120V1075H32V1020H78V975H123V882H178V985H133V1030H88V1065H214V1020H269V1075H224V1120Z"},
    {static_cast<wchar_t>(732), 300, L"M27 396V293H179V339H216V293H274V396H121V349H84V396Z"},
    {static_cast<wchar_t>(733), 300, L"M222 379V335H176V289H131V234H186V280H232V325H277V379ZM103 379V335H57V289H12V234H67V280H113V325H158V379Z"},
    {static_cast<wchar_t>(768), 0, L"M151 396V352H105V306H60V251H115V297H161V342H206V396Z"},
    {static_cast<wchar_t>(769), 0, L"M77 396V342H122V297H168V251H223V306H178V352H132V396Z"},
    {static_cast<wchar_t>(770), 0, L"M77 396V341H122V295H178V341H223V396H168V350H132V396Z"},
    {static_cast<wchar_t>(771), 0, L"M27 396V293H179V339H216V293H274V396H121V349H84V396Z"},
    {static_cast<wchar_t>(772), 0, L"M9 396V316H291V396Z"},
    {static_cast<wchar_t>(774), 0, L"M29 396V295H84V341H218V295H272V396Z"},
    {static_cast<wchar_t>(775), 0, L"M99 396V295H201V396Z"},
    {static_cast<wchar_t>(776), 0, L"M-7 396V295H95V396ZM206 396V295H307V396Z"},
    {static_cast<wchar_t>(778), 0, L"M100 396V295H201V396ZM133 363H168V328H133Z"},
    {static_cast<wchar_t>(779), 0, L"M228 396V352H182V306H137V251H192V297H238V342H283V396ZM109 396V352H63V306H18V251H73V297H119V342H164V396Z"},
    {static_cast<wchar_t>(780), 0, L"M122 396V350H77V295H132V341H168V295H223V350H178V396Z"},
    {static_cast<wchar_t>(786), 0, L"M122 396V305H77V251H122V205H178V261H132V295H223V396Z"},
    {static_cast<wchar_t>(806), 0, L"M133 1164V1108H179V1074H88V973H189V1064H234V1118H189V1164Z"},
    {static_cast<wchar_t>(807), 0, L"M77 1120V1075H31V1020H87V1065H213V1030H167V985H122V883H177V975H223V1020H268V1075H223V1120Z"},
    {static_cast<wchar_t>(808), 0, L"M77 1120V1075H31V1020H77V975H122V882H177V985H132V1030H87V1065H213V1020H268V1075H223V1120Z"},
    {static_cast<wchar_t>(902), 586, L"M60 932V470H151V380H241V289H343V380H433V470H525V932H423V751H161V932ZM161 650H423V480H333V390H251V480H161ZM263 229V177L356 83H407V135L315 229Z"},
    {static_cast<wchar_t>(904), 586, L"M151 932V842H60V380H151V289H433V380H525V480H423V390H161V561H343V660H161V832H423V741H525V842H433V932ZM263 229V177L356 83H407V135L315 229Z"},
    {static_cast<wchar_t>(905), 586, L"M60 932V289H161V561H241V470H343V561H423V289H525V932H423V660H161V932ZM265 229V128H320V229Z"},
    {static_cast<wchar_t>(906), 404, L"M60 932V832H151V390H60V289H343V390H251V832H343V932ZM176 229V128H231V229Z"},
    {static_cast<wchar_t>(908), 586, L"M151 932V842H60V380H151V289H433V380H525V842H433V932ZM161 832H423V390H161ZM265 229V128H320V229Z"},
    {static_cast<wchar_t>(910), 586, L"M241 932V570H151V480H60V289H161V470H251V561H333V470H423V289H525V480H433V570H343V932ZM265 229V128H320V229Z"},
    {static_cast<wchar_t>(911), 585, L"M0 933V832H60V380H151V290H433V380H525V832H584V933H423V390H161V933ZM263 229V177L356 83H407V135L315 229Z"},
    {static_cast<wchar_t>(913), 586, L"M60 932V470H151V380H241V289H343V380H433V470H525V932H423V751H161V932ZM161 650H423V480H333V390H251V480H161Z"},
    {static_cast<wchar_t>(914), 586, L"M151 932V842H60V380H151V289H433V380H525V570H433V650H525V842H433V932ZM161 832H423V660H241V561H423V390H161Z"},
    {static_cast<wchar_t>(915), 494, L"M60 932V289H433V390H161V932Z"},
    {static_cast<wchar_t>(917), 586, L"M151 932V842H60V380H151V289H433V380H525V480H423V390H161V561H343V660H161V832H423V741H525V842H433V932Z"},
    {static_cast<wchar_t>(918), 586, L"M151 932V842H60V650H151V561H423V390H161V480H60V289H433V380H525V570H433V660H161V832H423V741H525V932Z"},
    {static_cast<wchar_t>(919), 586, L"M60 932V289H161V561H241V470H343V561H423V289H525V932H423V660H161V932Z"},
    {static_cast<wchar_t>(920), 586, L"M151 932V842H60V380H151V289H433V380H525V842H433V932ZM161 832H423V390H161ZM223 651V586H362V651Z"},
    {static_cast<wchar_t>(921), 404, L"M60 932V832H151V390H60V289H343V390H251V832H343V932Z"},
    {static_cast<wchar_t>(922), 586, L"M60 932V380H151V289H251V390H161V561H241V470H333V380H423V289H525V390H433V480H343V570H251V650H343V741H433V832H525V932H423V842H333V751H241V660H161V932Z"},
    {static_cast<wchar_t>(923), 586, L"M60 932V561H151V379H241V289H343V379H433V561H525V932H423V570H333V389H251V570H161V932Z"},
    {static_cast<wchar_t>(924), 767, L"M60 932V380H151V289H251V380H343V561H423V380H515V289H616V380H706V932H606V390H525V570H433V842H333V570H241V390H161V932Z"},
    {static_cast<wchar_t>(925), 586, L"M60 932V289H161V380H251V470H343V741H423V289H525V932H423V842H333V751H241V480H161V932Z"},
    {static_cast<wchar_t>(926), 403, L"M60 390V289H342V390ZM60 932V831H342V932ZM60 661V561H342V661Z"},
    {static_cast<wchar_t>(927), 586, L"M151 932V842H60V380H151V289H433V380H525V842H433V932ZM161 832H423V390H161Z"},
    {static_cast<wchar_t>(928), 586, L"M60 932V291H525V932H423V392H161V932Z"},
    {static_cast<wchar_t>(929), 586, L"M60 932V380H151V289H433V380H525V660H433V751H161V932ZM161 650H423V390H161Z"},
    {static_cast<wchar_t>(931), 767, L"M151 933V842H60V742H151V651H333V571H151V481H60V380H151V290H706V390H161V471H343V562H616V661H343V752H161V832H706V933Z"},
    {static_cast<wchar_t>(932), 586, L"M241 932V390H161V480H60V380H151V289H433V380H525V480H423V390H343V932Z"},
    {static_cast<wchar_t>(933), 586, L"M241 932V570H151V480H60V289H161V470H251V561H333V470H423V289H525V480H433V570H343V932Z"},
    {static_cast<wchar_t>(934), 586, L"M238 931V852H151V762H60V481H151V390H238V288H340V390H433V481H525V762H433V852H340V931ZM248 842H330V762H248ZM161 752H238V490H161ZM340 752H423V490H340ZM248 481H330V400H248Z"},
    {static_cast<wchar_t>(935), 586, L"M60 932V832H151V650H241V570H151V390H60V289H161V380H251V561H333V380H423V289H525V390H433V570H343V650H433V832H525V932H423V842H333V660H251V842H161V932Z"},
    {static_cast<wchar_t>(936), 586, L"M238 932V714H151V624H60V291H161V614H238V289H340V614H423V291H525V624H433V714H340V932Z"},
    {static_cast<wchar_t>(938), 404, L"M60 932V832H151V390H60V289H343V390H251V832H343V932ZM47 217V116H148V217ZM259 217V116H361V217Z"},
    {static_cast<wchar_t>(939), 586, L"M241 932V570H151V480H60V289H161V470H251V561H333V470H423V289H525V480H433V570H343V932ZM166 229V128H221V229ZM365 229V128H419V229Z"},
    {static_cast<wchar_t>(940), 586, L"M423 999V932H151V842H60V561H151V470H433V561H525V999ZM161 832H423V570H161ZM265 392V291H320V392Z"},
    {static_cast<wchar_t>(941), 586, L"M151 932V842H60V561H151V470H433V561H525V660H423V570H161V650H343V751H161V832H423V740H525V841H433V932ZM265 392V291H320V392Z"},
    {static_cast<wchar_t>(942), 586, L"M423 1113V570H161V932H60V470H433V561H525V1113ZM265 392V291H320V392Z"},
    {static_cast<wchar_t>(943), 404, L"M60 932V832H151V390H60V289H343V390H251V832H343V932ZM176 229V128H231V229Z"},
    {static_cast<wchar_t>(945), 586, L"M423 999V932H151V842H60V561H151V470H433V561H525V999ZM161 832H423V570H161Z"},
    {static_cast<wchar_t>(946), 586, L"M60 1113V289H433V380H525V570H433V650H525V842H433V932H241V832H423V660H241V561H423V390H161V1113Z"},
    {static_cast<wchar_t>(947), 576, L"M232 932V714H141V532H50V470H151V522H241V704H323V522H413V470H515V532H423V714H333V932Z"},
    {static_cast<wchar_t>(948), 586, L"M151 932V842H60V561H151V470H238V380H330V289H430V390H340V470H433V561H525V842H433V932ZM161 832H423V570H161Z"},
    {static_cast<wchar_t>(949), 586, L"M151 932V842H60V561H151V470H433V561H525V660H423V570H161V650H343V751H161V832H423V740H525V841H433V932Z"},
    {static_cast<wchar_t>(950), 586, L"M332 1109V1010H423V932H151V841H60V650H151V561H423V389H151V289H433V379H525V570H433V660H161V831H423V741H525V1109Z"},
    {static_cast<wchar_t>(951), 586, L"M423 1113V570H161V932H60V470H433V561H525V1113Z"},
    {static_cast<wchar_t>(952), 586, L"M151 932V842H60V561H151V470H433V561H525V842H433V932ZM161 832H423V570H161ZM225 739V676H360V739Z"},
    {static_cast<wchar_t>(953), 222, L"M60 932V470H161V932ZM60 390V289H161V390Z"},
    {static_cast<wchar_t>(954), 586, L"M60 932V289H161V650H241V561H423V470H525V570H433V660H251V741H433V832H525V932H423V842H241V751H161V932Z"},
    {static_cast<wchar_t>(955), 586, L"M60 932V741H151V650H241V289H343V650H433V741H525V932H423V751H333V660H251V751H161V932Z"},
    {static_cast<wchar_t>(956), 767, L"M60 932V380H151V289H251V380H343V561H423V380H515V289H616V380H706V932H606V390H525V570H433V842H333V570H241V390H161V932Z"},
    {static_cast<wchar_t>(957), 576, L"M232 932V842H141V660H50V470H151V650H241V832H323V650H413V470H515V660H423V842H333V932Z"},
    {static_cast<wchar_t>(958), 585, L"M241 1111V1012H423V932H151V842H60V650H151V570H60V390H0V290H433V380H525V480H423V390H161V561H433V660H161V832H423V741H525V1111Z"},
    {static_cast<wchar_t>(959), 586, L"M151 932V842H60V561H151V470H433V561H525V842H433V932ZM161 832H423V570H161Z"},
    {static_cast<wchar_t>(960), 949, L"M241 932V571H161V661H60V562H151V471H433V562H515V471H797V562H888V661H787V571H706V932H605V571H525V661H423V571H343V932Z"},
    {static_cast<wchar_t>(961), 586, L"M60 1108V561H151V470H433V561H525V842H433V932H161V1108ZM161 832H423V570H161Z"},
    {static_cast<wchar_t>(963), 585, L"M151 932V842H60V561H151V470H583V570H525V842H433V932ZM161 832H423V570H161Z"},
    {static_cast<wchar_t>(964), 586, L"M241 932V570H161V660H60V561H151V470H433V561H525V660H423V570H343V932Z"},
    {static_cast<wchar_t>(965), 586, L"M151 932V842H60V470H161V832H423V470H525V842H433V932ZM263 407V354L356 260H407V313L315 407Z"},
    {static_cast<wchar_t>(966), 586, L"M238 932V841H151V751H60V470H151V379H238V289H340V379H433V470H525V751H433V841H340V932ZM161 741H238V480H161ZM340 741H423V480H340Z"},
    {static_cast<wchar_t>(967), 586, L"M60 932V832H151V741H241V660H151V570H60V470H161V561H251V650H333V561H423V470H525V570H433V660H343V741H433V832H525V932H423V842H333V751H251V842H161V932Z"},
    {static_cast<wchar_t>(968), 586, L"M238 932V842H151V752H60V380H161V742H238V289H340V742H423V380H525V752H433V842H340V932Z"},
    {static_cast<wchar_t>(969), 767, L"M60 932V470H161V832H333V470H433V832H606V470H706V842H616V932H423V842H343V932Z"},
    {static_cast<wchar_t>(970), 404, L"M60 932V832H151V390H60V289H343V390H251V832H343V932ZM47 217V116H148V217ZM259 217V116H361V217Z"},
    {static_cast<wchar_t>(971), 586, L"M241 932V570H151V480H60V289H161V470H251V561H333V470H423V289H525V480H433V570H343V932ZM166 229V128H221V229ZM365 229V128H419V229Z"},
    {static_cast<wchar_t>(972), 586, L"M151 932V842H60V561H151V470H433V561H525V842H433V932ZM161 832H423V570H161ZM265 392V291H320V392Z"},
    {static_cast<wchar_t>(973), 586, L"M151 932V842H60V470H161V832H423V470H525V842H433V932ZM265 392V291H320V392Z"},
    {static_cast<wchar_t>(974), 767, L"M60 932V470H161V832H333V470H433V832H606V470H706V842H616V932H423V842H343V932ZM355 392V291H409V392Z"},
    {static_cast<wchar_t>(1025), 586, L"M151 932V842H60V380H151V289H433V380H525V480H423V390H161V561H343V660H161V832H423V741H525V842H433V932ZM166 244V143H221V244ZM365 244V143H419V244Z"},
    {static_cast<wchar_t>(1026), 585, L"M151 932V842H60V390H-40V289H262V390H161V470H433V561H525V842H433V932ZM161 832H423V570H161Z"},
    {static_cast<wchar_t>(1027), 494, L"M60 932V291H433V392H161V932Z"},
    {static_cast<wchar_t>(1028), 586, L"M151 932V842H60V650H151V570H60V380H151V289H433V380H525V480H423V390H161V561H433V660H161V832H423V741H525V842H433V932Z"},
    {static_cast<wchar_t>(1029), 586, L"M60 932V832H423V751H151V660H60V561H151V470H525V570H161V650H433V741H525V842H433V932Z"},
    {static_cast<wchar_t>(1031), 280, L"M89 932V471H191V932ZM-20 391V290H82V391ZM199 391V290H300V391Z"},
    {static_cast<wchar_t>(1032), 586, L"M151 932V842H60V650H161V832H423V390H151V289H433V380H525V842H433V932Z"},
    {static_cast<wchar_t>(1033), 676, L"M60 932V831H150V291H615V932H514V392H251V932Z"},
    {static_cast<wchar_t>(1034), 949, L"M60 932V289H161V561H423V289H525V470H797V561H888V842H797V932H423V660H161V932ZM525 832H787V570H525Z"},
    {static_cast<wchar_t>(1035), 585, L"M60 932V392H-40V291H262V392H161V561H241V470H433V561H525V932H423V570H251V660H161V932Z"},
    {static_cast<wchar_t>(1036), 586, L"M60 932V380H151V289H251V390H161V561H241V470H333V380H423V289H525V390H433V480H343V570H251V650H343V741H433V832H525V932H423V842H333V751H241V660H161V932Z"},
    {static_cast<wchar_t>(1038), 586, L"M241 932V570H151V480H60V289H161V470H251V561H333V470H423V289H525V480H433V570H343V932ZM169 229V128H224V174H368V128H422V229Z"},
    {static_cast<wchar_t>(1039), 586, L"M240 1023V932H60V290H161V831H423V290H525V932H341V1023Z"},
    {static_cast<wchar_t>(1040), 586, L"M60 932V470H151V380H241V289H343V380H433V470H525V932H423V751H161V932ZM161 650H423V480H333V390H251V480H161Z"},
    {static_cast<wchar_t>(1041), 586, L"M151 932V842H60V289H433V380H525V480H423V390H161V561H433V650H525V842H433V932ZM161 832H423V660H161Z"},
    {static_cast<wchar_t>(1042), 586, L"M151 932V842H60V380H151V289H433V380H525V570H433V650H525V842H433V932ZM161 832H423V660H241V561H423V390H161Z"},
    {static_cast<wchar_t>(1043), 494, L"M60 932V291H433V392H161V932Z"},
    {static_cast<wchar_t>(1044), 676, L"M60 932V649H150V291H615V932H514V841H161V932ZM251 740H514V392H251Z"},
    {static_cast<wchar_t>(1045), 586, L"M151 932V842H60V380H151V289H433V380H525V480H423V390H161V561H343V660H161V832H423V741H525V842H433V932Z"},
    {static_cast<wchar_t>(1046), 949, L"M60 932V831H151V741H241V650H333V570H241V480H151V390H60V289H161V380H251V470H343V561H423V289H525V561H605V470H697V380H787V289H888V390H797V480H706V570H615V650H706V741H797V831H888V932H787V841H697V751H605V660H525V932H423V660H343V751H251V841H161V932Z"},
    {static_cast<wchar_t>(1047), 586, L"M151 932V842H60V741H161V832H423V660H151V561H423V390H161V480H60V380H151V289H433V380H525V570H433V650H525V842H433V932Z"},
    {static_cast<wchar_t>(1048), 586, L"M60 932V289H161V741H241V470H333V380H423V289H525V932H423V480H343V751H251V842H161V932Z"},
    {static_cast<wchar_t>(1049), 586, L"M60 932V289H161V741H241V470H333V380H423V289H525V932H423V480H343V751H251V842H161V932ZM169 229V128H224V174H358V128H412V229Z"},
    {static_cast<wchar_t>(1050), 586, L"M60 932V380H151V289H251V390H161V561H241V470H333V380H423V289H525V390H433V480H343V570H251V650H343V741H433V832H525V932H423V842H333V751H241V660H161V932ZM263 229V174H308V128H353V83H407V138H363V183H318V229Z"},
    {static_cast<wchar_t>(1051), 676, L"M60 932V831H150V291H615V932H514V392H251V932Z"},
    {static_cast<wchar_t>(1052), 767, L"M60 932V380H151V289H251V380H343V561H423V380H515V289H616V380H706V932H606V390H525V570H433V842H333V570H241V390H161V932Z"},
    {static_cast<wchar_t>(1053), 586, L"M60 932V289H161V561H423V289H525V932H423V660H161V932Z"},
    {static_cast<wchar_t>(1056), 586, L"M60 932V380H151V289H433V380H525V660H433V751H161V932ZM161 650H423V390H161Z"},
    {static_cast<wchar_t>(1057), 586, L"M151 932V842H60V380H151V289H433V380H525V570H423V390H161V832H423V650H525V842H433V932Z"},
    {static_cast<wchar_t>(1058), 586, L"M241 932V390H161V480H60V380H151V289H433V380H525V480H423V390H343V932Z"},
    {static_cast<wchar_t>(1059), 586, L"M241 932V570H151V480H60V289H161V470H251V561H333V470H423V289H525V480H433V570H343V932Z"},
    {static_cast<wchar_t>(1060), 586, L"M238 932V843H151V753H60V472H151V381H238V289H340V381H433V472H525V753H433V843H340V932ZM161 743H238V482H161ZM340 743H423V482H340Z"},
    {static_cast<wchar_t>(1061), 586, L"M60 932V832H151V650H241V570H151V390H60V289H161V380H251V561H333V380H423V289H525V390H433V570H343V650H433V832H525V932H423V842H333V660H251V842H161V932Z"},
    {static_cast<wchar_t>(1062), 586, L"M469 1022V966H515V932H60V291H161V831H423V291H525V922H569V976H525V1022Z"},
    {static_cast<wchar_t>(1063), 586, L"M424 933V662H151V571H60V290H161V562H424V290H525V933Z"},
    {static_cast<wchar_t>(1064), 767, L"M151 932V842H60V289H161V832H241V650H333V380H433V650H525V832H606V289H706V842H616V932H515V842H423V660H343V842H251V932Z"},
    {static_cast<wchar_t>(1065), 767, L"M651 1022V966H697V932H515V842H423V660H343V842H251V932H151V842H60V289H161V832H241V650H333V380H433V650H525V832H606V289H706V922H751V976H706V1022Z"},
    {static_cast<wchar_t>(1066), 585, L"M151 932V842H60V390H-40V289H262V390H161V470H433V561H525V842H433V932ZM161 832H423V570H161Z"},
    {static_cast<wchar_t>(1067), 767, L"M151 932V842H60V289H161V562H433V652H525V842H433V932ZM161 832H423V662H161ZM605 932V470H706V932Z"},
    {static_cast<wchar_t>(1068), 586, L"M151 932V842H60V289H161V470H433V561H525V842H433V932ZM161 832H423V570H161Z"},
    {static_cast<wchar_t>(1069), 586, L"M151 932V842H60V741H161V832H423V660H151V561H423V390H161V480H60V380H151V289H433V380H525V570H433V650H525V842H433V932Z"},
    {static_cast<wchar_t>(1070), 850, L"M60 932V289H161V561H324V380H415V289H698V380H789V842H698V932H415V842H324V660H161V932ZM425 832H688V390H425Z"},
    {static_cast<wchar_t>(1071), 586, L"M105 932V832H196V751H151V660H60V380H151V289H433V380H525V932H423V751H297V842H206V932ZM161 650H423V390H161Z"},
    {static_cast<wchar_t>(1072), 677, L"M151 932V842H60V614H423V570H60V470H433V561H525V832H616V932H515V842H433V932ZM161 832H423V715H161Z"},
    {static_cast<wchar_t>(1073), 586, L"M151 932V842H60V380H151V289H433V380H525V480H423V390H161V561H433V650H525V842H433V932ZM161 832H423V660H161Z"},
    {static_cast<wchar_t>(1074), 586, L"M151 932V842H60V561H151V470H433V561H525V662H433V742H525V842H433V932ZM161 832H423V752H241V652H423V570H161Z"},
    {static_cast<wchar_t>(1075), 494, L"M60 932V470H433V570H161V932Z"},
    {static_cast<wchar_t>(1076), 676, L"M60 932V649H150V470H615V932H514V841H161V932ZM251 740H514V570H251Z"},
    {static_cast<wchar_t>(1077), 586, L"M151 932V842H60V561H151V470H433V561H525V660H423V570H161V650H343V751H161V832H423V740H525V841H433V932Z"},
    {static_cast<wchar_t>(1078), 760, L"M60 932V863H119V803H180V750H238V651H179V592H118V533H60V470H128V523H189V582H248V642H329V470H430V642H511V582H570V523H631V470H700V533H641V592H580V651H521V750H579V803H640V863H700V932H630V873H569V813H511V760H430V932H329V760H248V813H190V873H129V932Z"},
    {static_cast<wchar_t>(1079), 586, L"M151 932V841H60V740H161V832H423V751H241V650H423V570H161V660H60V561H151V470H433V561H525V842H433V932Z"},
    {static_cast<wchar_t>(1080), 556, L"M60 932V470H161V741H241V650H333V561H423V470H525V932H423V660H343V751H251V842H161V932Z"},
    {static_cast<wchar_t>(1081), 586, L"M60 932V469H161V741H241V649H333V560H423V469H525V932H423V659H343V751H251V842H161V932ZM179 409V308H234V353H358V308H412V409Z"},
    {static_cast<wchar_t>(1082), 586, L"M60 932V470H161V650H241V561H423V470H525V570H433V660H251V741H433V832H525V932H423V842H241V751H161V932Z"},
    {static_cast<wchar_t>(1083), 676, L"M60 932V831H150V471H615V932H514V571H251V932Z"},
    {static_cast<wchar_t>(1084), 767, L"M60 932V561H151V470H251V561H343V741H423V561H515V470H616V561H706V932H606V570H525V751H433V842H333V751H241V570H161V932Z"},
    {static_cast<wchar_t>(1085), 586, L"M60 932V469H161V661H423V469H525V932H423V761H161V932Z"},
    {static_cast<wchar_t>(1086), 586, L"M151 932V842H60V561H151V470H433V561H525V842H433V932ZM161 832H423V570H161Z"},
    {static_cast<wchar_t>(1087), 586, L"M60 932V470H525V932H423V570H161V932Z"},
    {static_cast<wchar_t>(1088), 586, L"M60 1108V561H151V470H433V561H525V842H433V932H161V1108ZM161 832H423V570H161Z"},
    {static_cast<wchar_t>(1089), 586, L"M151 932V842H60V561H151V470H433V561H525V660H423V570H161V832H423V741H525V842H433V932Z"},
    {static_cast<wchar_t>(1090), 586, L"M241 932V569H161V659H60V560H151V469H433V560H525V659H423V569H343V932Z"},
    {static_cast<wchar_t>(1091), 586, L"M151 1113V1023H60V923H161V1013H424V842H151V751H60V470H161V741H424V470H525V1023H434V1113Z"},
    {static_cast<wchar_t>(1092), 504, L"M183 932V817H111V737H30V488H111V407H183V289H284V407H362V488H443V737H362V817H284V932ZM121 727H183V497H121ZM284 727H352V497H284Z"},
    {static_cast<wchar_t>(1093), 586, L"M60 932V832H151V741H241V660H151V570H60V470H161V561H251V650H333V561H423V470H525V570H433V660H343V741H433V832H525V932H423V842H333V751H251V842H161V932Z"},
    {static_cast<wchar_t>(1094), 599, L"M468 1022V966H514V932H60V470H161V831H423V470H525V922H568V976H524V1022Z"},
    {static_cast<wchar_t>(1095), 586, L"M424 933V842H151V751H60V470H161V741H424V470H525V933Z"},
    {static_cast<wchar_t>(1096), 767, L"M60 932V470H161V832H333V470H433V832H606V470H706V932H423V842H343V932Z"},
    {static_cast<wchar_t>(1097), 767, L"M651 1022V966H697V932H423V842H343V932H60V470H161V832H333V470H433V832H606V470H706V922H751V976H706V1022Z"},
    {static_cast<wchar_t>(1098), 585, L"M151 932V842H60V390H0V289H161V470H433V561H525V842H433V932ZM161 832H423V570H161Z"},
    {static_cast<wchar_t>(1099), 767, L"M151 932V842H60V289H161V470H433V561H525V842H433V932ZM161 832H423V570H161ZM605 932V289H706V932Z"},
    {static_cast<wchar_t>(1100), 585, L"M91 932V842H0V289H101V470H373V561H465V842H373V932ZM101 832H363V570H101Z"},
    {static_cast<wchar_t>(1101), 586, L"M151 932V841H60V740H161V832H423V751H241V650H423V570H161V660H60V561H151V470H433V561H525V842H433V932Z"},
    {static_cast<wchar_t>(1102), 849, L"M60 932V469H161V661H323V561H414V470H697V561H788V842H697V932H414V842H323V761H161V932ZM424 832H687V570H424Z"},
    {static_cast<wchar_t>(1103), 586, L"M105 932V832H196V750H151V659H60V561H151V470H433V561H525V932H423V750H297V842H206V932ZM161 649H423V570H161Z"},
    {static_cast<wchar_t>(1105), 586, L"M151 932V842H60V561H151V470H433V561H525V660H423V570H161V650H343V751H161V832H423V740H525V841H433V932ZM134 368V267H236V368ZM347 368V267H448V368Z"},
    {static_cast<wchar_t>(1106), 615, L"M271 1167V1076H180V975H281V1066H453V570H281V660H191V932H89V422H60V322H89V289H191V322H292V422H191V561H271V470H463V561H554V1076H463V1167Z"},
    {static_cast<wchar_t>(1107), 586, L"M60 932V561H151V470H433V561H525V660H423V570H161V932ZM221 380V325H265V279H310V234H365V289H320V335H275V380Z"},
    {static_cast<wchar_t>(1108), 586, L"M151 932V842H60V561H151V470H433V561H525V660H423V570H161V650H343V751H161V832H423V740H525V841H433V932Z"},
    {static_cast<wchar_t>(1109), 586, L"M60 932V832H423V751H151V660H60V561H151V470H525V570H161V650H433V741H525V842H433V932Z"},
    {static_cast<wchar_t>(1110), 222, L"M60 932V470H161V932ZM60 390V289H161V390Z"},
    {static_cast<wchar_t>(1111), 340, L"M119 932V650H221V932ZM10 570V470H111V570ZM229 570V470H330V570Z"},
    {static_cast<wchar_t>(1112), 495, L"M151 933V842H60V741H161V832H333V470H434V842H343V933Z"},
    {static_cast<wchar_t>(1113), 939, L"M60 932V831H150V471H515V562H787V652H878V842H787V932H413V571H251V932ZM515 832H777V662H515Z"},
    {static_cast<wchar_t>(1114), 850, L"M60 932V469H161V661H324V469H425V562H698V652H789V842H698V932H324V761H161V932ZM425 832H688V662H425Z"},
    {static_cast<wchar_t>(1115), 586, L"M60 932V291H262V392H161V561H241V470H433V561H525V932H423V570H251V660H161V932Z"},
    {static_cast<wchar_t>(1116), 586, L"M60 932V470H161V650H241V561H423V470H525V570H433V660H251V741H433V832H525V932H423V842H241V751H161V932ZM221 380V325H265V279H310V234H365V289H320V335H275V380Z"},
    {static_cast<wchar_t>(1118), 586, L"M151 1113V1023H60V923H161V1013H424V842H151V751H60V470H161V741H424V470H525V1023H434V1113ZM169 380V279H224V325H368V279H422V380Z"},
    {static_cast<wchar_t>(1119), 586, L"M240 1023V932H60V471H161V831H423V471H525V932H341V1023Z"},
    {static_cast<wchar_t>(1168), 494, L"M60 932V291H333V219H433V392H161V932Z"},
    {static_cast<wchar_t>(1169), 586, L"M241 932V570H151V480H60V289H161V470H251V561H333V470H423V289H525V480H433V570H343V932Z"},
    {static_cast<wchar_t>(7808), 767, L"M151 932V842H60V289H161V832H241V650H333V380H433V650H525V832H606V289H706V842H616V932H515V842H423V660H343V842H251V932ZM357 215V171H311V125H266V70H321V116H367V161H412V215Z"},
    {static_cast<wchar_t>(7809), 767, L"M60 932V470H161V832H333V470H433V832H606V470H706V842H616V932H423V842H343V932ZM357 396V352H311V306H266V251H321V297H367V342H412V396Z"},
    {static_cast<wchar_t>(7810), 767, L"M151 932V842H60V289H161V832H241V650H333V380H433V650H525V832H606V289H706V842H616V932H515V842H423V660H343V842H251V932ZM296 215V161H341V116H387V70H442V125H397V171H351V215Z"},
    {static_cast<wchar_t>(7811), 767, L"M60 932V470H161V832H333V470H433V832H606V470H706V842H616V932H423V842H343V932ZM296 396V342H341V297H387V251H442V306H397V352H351V396Z"},
    {static_cast<wchar_t>(7812), 767, L"M151 932V842H60V289H161V832H241V650H333V380H433V650H525V832H606V289H706V842H616V932H515V842H423V660H343V842H251V932ZM231 215V114H333V215ZM444 215V114H545V215Z"},
    {static_cast<wchar_t>(7813), 767, L"M60 932V470H161V832H333V470H433V832H606V470H706V842H616V932H423V842H343V932ZM231 396V295H333V396ZM444 396V295H545V396Z"},
    {static_cast<wchar_t>(7838), 655, L"M221 932V832H493V660H321V561H493V390H121V289H503V380H595V570H503V650H595V842H503V932ZM60 932V289H161V932Z"},
    {static_cast<wchar_t>(7922), 586, L"M241 932V570H151V480H60V289H161V470H251V561H333V470H423V289H525V480H433V570H343V932ZM266 215V171H220V125H175V70H230V116H276V161H321V215Z"},
    {static_cast<wchar_t>(7923), 586, L"M151 1113V1023H60V923H161V1013H424V842H151V751H60V470H161V741H424V470H525V1023H434V1113ZM266 396V352H220V306H175V251H230V297H276V342H321V396Z"},
    {static_cast<wchar_t>(8211), 403, L"M60 679V578H342V679Z"},
    {static_cast<wchar_t>(8212), 502, L"M60 679V578H441V679Z"},
    {static_cast<wchar_t>(8216), 267, L"M104 480V389H60V335H104V289H160V344H114V379H206V480Z"},
    {static_cast<wchar_t>(8217), 267, L"M105 480V424H151V390H60V289H161V380H206V434H161V480Z"},
    {static_cast<wchar_t>(8218), 267, L"M105 1010V954H151V920H60V819H161V910H206V964H161V1010Z"},
    {static_cast<wchar_t>(8220), 465, L"M303 480V389H258V335H303V289H359V344H313V379H404V480ZM104 480V389H60V335H104V289H160V344H114V379H206V480Z"},
    {static_cast<wchar_t>(8221), 465, L"M304 480V424H350V390H258V289H360V380H404V434H360V480ZM105 480V424H151V390H60V289H161V380H206V434H161V480Z"},
    {static_cast<wchar_t>(8222), 465, L"M304 1010V954H350V920H258V819H360V910H404V964H360V1010ZM105 1010V954H151V920H60V819H161V910H206V964H161V1010Z"},
    {static_cast<wchar_t>(8224), 403, L"M150 932V517H60V416H150V289H251V416H342V517H251V932Z"},
    {static_cast<wchar_t>(8226), 258, L"M60 672V536H197V672Z"},
    {static_cast<wchar_t>(8230), 643, L"M270 920V819H372V920ZM60 920V819H161V920ZM481 920V819H582V920Z"},
    {static_cast<wchar_t>(8240), 642, L"M106 574V528H60V286H106V240H256V286H303V528H256V574ZM106 982V936H60V802H106V756H256V802H303V936H256V982ZM116 926H246V812H116ZM385 982V936H338V802H385V756H535V802H581V936H535V982ZM394 926H525V812H394ZM30 673V628H535V673ZM116 518H246V296H116Z"},
    {static_cast<wchar_t>(8249), 556, L"M423 1027V936H333V845H241V755H151V664H60V565H151V474H241V383H333V293H423V202H525V303H433V393H343V484H251V574H161V654H251V745H343V835H433V926H525V1027Z"},
    {static_cast<wchar_t>(8250), 556, L"M30 1027V926H121V835H212V745H303V654H394V574H303V484H212V393H121V303H30V202H131V293H222V383H313V474H403V565H495V664H403V755H313V845H222V936H131V1027Z"},
    {static_cast<wchar_t>(8252), 431, L"M268 751V289H370V751ZM60 932V832H161V932ZM268 932V832H370V932ZM60 751V289H161V751Z"},
    {static_cast<wchar_t>(8304), 364, L"M106 624V577H60V336H106V289H256V336H303V577H256V624ZM116 567H246V345H116Z"},
    {static_cast<wchar_t>(8308), 365, L"M246 625V532H60V429H106V382H153V336H201V289H304V625ZM116 475H246V345H211V392H163V439H116Z"},
    {static_cast<wchar_t>(8309), 365, L"M106 625V578H60V522H116V568H246V532H153V485H106V439H60V336H106V289H256V336H304V439H246V345H116V429H163V475H256V522H304V578H256V625Z"},
    {static_cast<wchar_t>(8310), 365, L"M106 625V578H60V336H106V289H256V336H304V392H246V345H116V429H256V475H304V578H256V625ZM116 568H246V485H116Z"},
    {static_cast<wchar_t>(8311), 365, L"M246 625V345H116V392H60V336H106V289H256V336H304V625Z"},
    {static_cast<wchar_t>(8312), 365, L"M106 625V578H60V475H106V439H60V336H106V289H256V336H304V439H256V475H304V578H256V625ZM116 568H246V485H116ZM116 429H246V345H116Z"},
    {static_cast<wchar_t>(8313), 365, L"M106 625V578H60V522H116V568H246V485H106V439H60V336H106V289H256V336H304V578H256V625ZM116 429H246V345H116Z"},
    {static_cast<wchar_t>(8320), 364, L"M106 933V886H60V645H106V598H256V645H303V886H256V933ZM116 876H246V654H116Z"},
    {static_cast<wchar_t>(8321), 271, L"M153 933V654H116V701H60V645H106V598H163V645H210V933Z"},
    {static_cast<wchar_t>(8322), 364, L"M106 933V887H60V784H106V737H246V654H116V701H60V645H106V598H256V645H303V747H256V794H116V877H246V830H303V887H256V933Z"},
    {static_cast<wchar_t>(8323), 364, L"M107 933V886H60V830H117V876H245V793H107V737H245V655H117V701H60V645H107V598H255V645H303V747H255V783H303V886H255V933Z"},
    {static_cast<wchar_t>(8324), 365, L"M246 933V840H60V737H106V690H153V644H201V597H304V933ZM116 783H246V653H211V700H163V747H116Z"},
    {static_cast<wchar_t>(8325), 365, L"M106 933V886H60V830H116V876H246V840H153V793H106V747H60V644H106V597H256V644H304V747H246V653H116V737H163V783H256V830H304V886H256V933Z"},
    {static_cast<wchar_t>(8326), 365, L"M106 933V886H60V644H106V597H256V644H304V700H246V653H116V737H256V783H304V886H256V933ZM116 876H246V793H116Z"},
    {static_cast<wchar_t>(8327), 365, L"M246 933V653H116V700H60V644H106V597H256V644H304V933Z"},
    {static_cast<wchar_t>(8328), 365, L"M106 933V886H60V783H106V747H60V644H106V597H256V644H304V747H256V783H304V886H256V933ZM116 876H246V793H116ZM116 737H246V653H116Z"},
    {static_cast<wchar_t>(8329), 365, L"M106 933V886H60V830H116V876H246V793H106V747H60V644H106V597H256V644H304V886H256V933ZM116 737H246V653H116Z"},
    {static_cast<wchar_t>(8353), 586, L"M162 991 171 932H151V842H60V380H151V289H266L275 230H327L318 289H356L365 230H416L407 289H433V380H525V570H423V390H392L327 832H423V650H525V842H433V932H312L303 991H251L260 932H223L214 991ZM161 832H186L250 390H161ZM238 832H275L340 390H302Z"},
    {static_cast<wchar_t>(8355), 585, L"M22 932V831H60V289H433V380H525V480H423V390H161V561H343V660H161V831H205V932Z"},
    {static_cast<wchar_t>(8356), 676, L"M60 932V832H151V380H241V289H525V380H615V480H515V390H251V511H433V611H251V669H433V769H251V832H515V741H615V842H525V932Z"},
    {static_cast<wchar_t>(8359), 1195, L"M60 932V380H151V289H433V380H525V660H433V751H161V932ZM161 650H423V390H161ZM664 932V887H619V755H574V700H619V612H674V700H808V755H674V877H798V833H853V887H808V932ZM900 932V877H1079V843H946V799H900V745H946V700H1134V755H956V789H1089V833H1134V887H1089V932Z"},
    {static_cast<wchar_t>(8364), 586, L"M151 932V842H60V380H151V289H433V380H525V570H423V390H161V492H344V593H161V642H344V742H161V832H423V650H525V842H433V932Z"},
    {static_cast<wchar_t>(8470), 1160, L"M60 932V289H161V380H251V470H343V741H423V289H525V932H423V842H333V751H241V480H161V932ZM705 933V832H1027V933ZM725 753V663H634V382H725V291H1008V382H1099V663H1008V753ZM735 653H998V392H735Z"},
    {static_cast<wchar_t>(8482), 789, L"M393 623V336H440V289H496V336H542V427H579V336H625V289H683V336H728V623H673V345H635V437H589V576H532V437H486V345H450V623ZM153 623V345H116V392H60V336H106V289H256V336H302V392H246V345H209V623Z"},
    {static_cast<wchar_t>(8486), 585, L"M0 933V832H60V380H151V290H433V380H525V832H584V933H423V390H161V933Z"},
    {static_cast<wchar_t>(8531), 881, L"M217 932V764H296V606H376V527H455V368H536V289H625V457H546V537H465V695H386V774H306V932ZM153 624V345H116V392H60V336H106V289H163V336H210V624ZM624 933V886H576V830H634V876H762V793H624V737H762V655H634V701H576V645H624V598H772V645H820V747H772V783H820V886H772V933Z"},
    {static_cast<wchar_t>(8532), 910, L"M266 932V764H346V606H425V527H505V368H585V289H675V457H595V537H515V695H435V774H356V932ZM106 626V580H60V477H106V430H246V347H116V394H60V337H106V291H256V337H303V440H256V487H116V570H246V523H303V580H256V626ZM654 933V886H606V830H664V876H792V793H654V737H792V655H664V701H606V645H654V598H802V645H850V747H802V783H850V886H802V933Z"},
    {static_cast<wchar_t>(8539), 872, L"M217 932V764H296V606H376V527H455V368H536V289H625V457H546V537H465V695H386V774H306V932ZM153 624V345H116V392H60V336H106V289H163V336H210V624ZM613 933V886H566V783H613V747H566V644H613V597H763V644H811V747H763V783H811V886H763V933ZM623 876H753V793H623ZM623 737H753V653H623Z"},
    {static_cast<wchar_t>(8540), 941, L"M286 932V764H366V606H445V527H525V368H605V289H695V457H615V537H535V695H455V774H376V932ZM107 624V577H60V521H117V567H245V484H107V428H245V346H117V392H60V336H107V289H255V336H303V438H255V474H303V577H255V624ZM693 876H823V793H693ZM683 933V886H636V783H683V747H636V644H683V597H833V644H880V747H833V783H880V886H833V933ZM693 737H823V653H693Z"},
    {static_cast<wchar_t>(8541), 971, L"M316 932V764H395V606H475V527H554V368H635V289H724V457H645V537H564V695H485V774H405V932ZM106 625V578H60V522H116V568H246V532H153V485H106V439H60V336H106V289H256V336H304V439H246V345H116V429H163V475H256V522H304V578H256V625ZM722 876H853V793H722ZM712 933V886H666V783H712V747H666V644H712V597H862V644H910V747H862V783H910V886H862V933ZM722 737H853V653H722Z"},
    {static_cast<wchar_t>(8542), 991, L"M336 932V764H415V606H495V527H574V368H655V289H744V457H665V537H584V695H505V774H425V932ZM246 625V345H116V392H60V336H106V289H256V336H304V625ZM732 933V886H686V783H732V747H686V644H732V597H882V644H930V747H882V783H930V886H882V933ZM742 876H872V793H742ZM742 737H872V653H742Z"},
    {static_cast<wchar_t>(8710), 586, L"M60 932V470H151V380H241V289H343V380H433V470H525V932ZM161 831H423V480H333V390H251V480H161Z"},
    {static_cast<wchar_t>(8719), 586, L"M60 932V289H525V932H424V390H161V932Z"},
    {static_cast<wchar_t>(8721), 767, L"M150 932V841H59V741H150V650H332V570H150V480H59V379H150V289H705V389H160V470H342V561H615V660H342V751H160V831H705V932Z"},
    {static_cast<wchar_t>(8722), 600, L"M109 679V578H490V679Z"},
    {static_cast<wchar_t>(8725), 529, L"M60 932V764H139V606H219V527H298V368H379V289H468V457H389V537H308V695H229V774H149V932Z"},
    {static_cast<wchar_t>(8734), 949, L"M151 932V842H60V561H151V470H433V561H515V470H797V561H888V842H797V932H515V842H433V932ZM161 832H423V570H161ZM525 832H787V570H525Z"},
    {static_cast<wchar_t>(8776), 600, L"M176 630V527H328V573H365V527H423V630H270V583H233V630ZM176 788V685H328V731H365V685H423V788H270V741H233V788Z"},
    {static_cast<wchar_t>(8800), 600, L"M170 910 319 409H420L271 910ZM109 785V685H491V785ZM109 630V529H491V630Z"},
    {static_cast<wchar_t>(8804), 600, L"M394 814V747H327V680H260V614H193V547H126V470H193V403H260V336H327V269H394V202H472V279H404V345H337V413H270V480H203V537H270V604H337V670H404V737H472V814ZM126 980V916H473V980Z"},
    {static_cast<wchar_t>(8805), 600, L"M127 814V737H195V670H262V604H330V537H396V480H330V413H262V345H195V279H127V202H205V269H272V336H339V403H406V470H474V547H406V614H339V680H272V747H205V814ZM126 980V916H474V980Z"},
    {static_cast<wchar_t>(64256), 949, L"M60 932V380H151V289H433V380H515V289H797V380H888V480H787V390H525V561H706V660H525V932H423V390H161V561H343V660H161V932Z"},
    {static_cast<wchar_t>(64257), 586, L"M60 932V380H151V289H433V380H525V480H423V390H161V561H525V932H423V660H161V932Z"},
    {static_cast<wchar_t>(64258), 586, L"M60 932V380H151V289H433V380H525V932H423V390H161V561H343V660H161V932Z"},
    {static_cast<wchar_t>(64259), 949, L"M60 932V380H151V289H433V380H515V289H797V380H888V480H787V390H525V561H888V932H786V660H525V932H423V390H161V561H343V660H161V932Z"},
    {static_cast<wchar_t>(64260), 949, L"M60 932V380H151V289H433V380H515V289H797V380H888V932H787V390H525V561H706V660H525V932H423V390H161V561H343V660H161V932Z"},
    {static_cast<wchar_t>(64262), 1124, L"M60 932V832H423V751H151V660H60V561H151V470H598V289H699V470H972V570H699V832H962V741H1063V842H972V932H689V842H598V570H161V650H433V741H525V842H433V932Z"},
};

// Pixel themes render licensed font outlines as native XAML paths. No external
// font install, private font file access, dispatcher callbacks or new hooks.
struct StudioPixelTheme {
    const wchar_t* key;
    const wchar_t* anchor;
    unsigned long foreground, accent, shadow;
    double x, y;
    bool dateAbove, twoTone, strip;
};
static const StudioPixelTheme kStudioPixelThemes[] = {
    {L"pixelCoffee", L"bottomLeft", 0xFFFFF3DF, 0xFFD6B58B, 0xDD211724, 80,-65, true,false,false},
    {L"pixelDusk", L"topLeft", 0xFFF2EDE7, 0xFFCDA69B, 0xDD201E2A, 80,65, false,false,false},
    {L"pixelSky", L"topRight", 0xFFFFFFFF, 0xFFFFE5B0, 0xAA547E94,-80,110, true,false,false},
    {L"pixelNight", L"topLeft", 0xFFF3EDF8, 0xFF56C5DC, 0xDD252237, 80,65, false,true,false},
    {L"pixelSakura", L"topLeft", 0xFF384654, 0xFFAE718B, 0x77FFFFFF, 80,65, false,false,false},
    {L"pixelRain", L"topLeft", 0xFFE4EEFF, 0xFF83B7DB, 0xEE101526, 80,65, false,false,false},
    {L"pixelHollow", L"bottom", 0xFFECE8DC, 0xFFAFAB9D, 0xEE111110, 140,-45, false,false,true},
    {L"pixelSunset", L"topLeft", 0xFFFFE2B5, 0xFFEEBCA4, 0xAA75465D, 80,65, false,false,false},
};
StudioPixelTheme const& CurrentStudioPixelTheme() {
    static const StudioPixelTheme legacy{L"pixel",L"center",0xFFFFFFFF,0xFFDDE9FF,0xDD101526,0,0,false,false,false};
    if (g_settings.clockStyle==L"pixel") return legacy;
    for (auto const& theme : kStudioPixelThemes)
        if (g_settings.clockStyle == theme.key) return theme;
    return kStudioPixelThemes[0];
}
[[clang::no_destroy]] wuxc::Grid studioPixelClock{nullptr};
[[clang::no_destroy]] wuxc::Grid studioPixelDate{nullptr};
std::wstring studioPixelLastTime, studioPixelLastDate;
void ResetStudioPixel() {
    studioPixelClock = nullptr;
    studioPixelDate = nullptr;
    studioPixelLastTime.clear();
    studioPixelLastDate.clear();
}
StudioPixelGlyph const* FindStudioPixelGlyph(wchar_t c) {
    auto first = std::begin(kStudioPixelGlyphs), last = std::end(kStudioPixelGlyphs);
    auto it = std::lower_bound(first, last, c,
        [](StudioPixelGlyph const& glyph, wchar_t character) { return glyph.character < character; });
    return it != last && it->character == c ? it : nullptr;
}
std::wstring StudioPixelHex(wu::Color const& color) {
    wchar_t buffer[10];
    swprintf_s(buffer, L"#%02X%02X%02X%02X", color.A, color.R, color.G, color.B);
    return buffer;
}
void RenderStudioPixel(wuxc::Grid const& host, std::wstring const& text,
                       double size, wu::Color foreground, wu::Color accent,
                       bool twoTone, double tracking,std::wstring const& fontOverride=L"") {
    auto const& theme = CurrentStudioPixelTheme();
    host.Children().Clear();
    if (!fontOverride.empty()) {
        wuxc::TextBlock custom;
        custom.Text(text);
        custom.FontSize(size);
        custom.FontFamily(wuxm::FontFamily(fontOverride));
        custom.Foreground(wuxm::SolidColorBrush(foreground));
        auto separator=text.find_first_of(L":|");
        if (twoTone && separator!=std::wstring::npos) {
            custom.Text(L"");
            wuxd::Run hours; hours.Text(text.substr(0,separator));
            custom.Inlines().Append(hours);
            wuxd::Run divider; divider.Text(text.substr(separator,1));
            divider.Foreground(wuxm::SolidColorBrush(StudioColor(L"",0xFFBE8DAE)));
            custom.Inlines().Append(divider);
            wuxd::Run minutes; minutes.Text(text.substr(separator+1));
            minutes.Foreground(wuxm::SolidColorBrush(accent));
            custom.Inlines().Append(minutes);
        }
        custom.CharacterSpacing(static_cast<int>(tracking*1000/(std::max)(size,1.0)));
        host.Children().Append(custom);
        custom.Measure({100000,100000});
        auto desired=custom.DesiredSize();
        host.Width((std::max)(1.0,static_cast<double>(desired.Width)));
        host.Height((std::max)(1.0,static_cast<double>(desired.Height)));
        return;
    }
    // Unsupported scripts use a readable native text fallback, never missing
    // date glyphs. All mapped font characters (including Cyrillic) are embedded.
    for (wchar_t c : text) if (!FindStudioPixelGlyph(c)) {
        wuxc::TextBlock fallback;
        fallback.Text(text);
        fallback.FontSize(size);
        fallback.Foreground(wuxm::SolidColorBrush(foreground));
        host.Children().Append(fallback);
        fallback.Measure({100000,100000});
        auto desired=fallback.DesiredSize();
        host.Width((std::max)(1.0,static_cast<double>(desired.Width)));
        host.Height((std::max)(1.0,static_cast<double>(desired.Height)));
        return;
    }
    double width = 0;
    auto ampm = text.ends_with(L" AM") || text.ends_with(L" PM") ? text.size()-3 : std::wstring::npos;
    for (size_t i = 0; i < text.size(); ++i) {
        double scale = size / kStudioPixelEm;
        if (size > studio.dateSize && ampm != std::wstring::npos && i >= ampm) scale *= 0.4;
        width += FindStudioPixelGlyph(text[i])->advance * scale + tracking;
    }
    width = (std::max)(1.0, width - (text.empty() ? 0 : tracking));
    double height = size * kStudioPixelHeight / kStudioPixelEm;
    double shadow = studio.pixelShadow;
    std::wstring xaml = L"<Canvas xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation' Width='" +
        std::to_wstring(width + shadow) + L"' Height='" + std::to_wstring(height + shadow) + L"'>";
    auto split = text.find_first_of(L":|");
    for (int layer = shadow > 0 ? 0 : 1; layer < 2; ++layer) {
        double cursor = 0;
        for (size_t i = 0; i < text.size(); ++i) {
            auto glyph = FindStudioPixelGlyph(text[i]);
            double scale = size / kStudioPixelEm;
            bool small = size > studio.dateSize && ampm != std::wstring::npos && i >= ampm;
            if (small) scale *= 0.4;
            wu::Color color = foreground;
            if (twoTone && split != std::wstring::npos && i > split) color = accent;
            if (twoTone && i == split) color = StudioColor(L"", 0xFFBE8DAE);
            if (layer == 0) color = StudioColor(L"", theme.shadow);
            double offset = layer == 0 ? shadow : 0;
            double top = offset + (small ? size * 0.45 : 0);
            if (*glyph->data) {
                xaml += L"<Path Stretch='None' Canvas.Left='" + std::to_wstring(cursor + offset) +
                    L"' Canvas.Top='" + std::to_wstring(top) + L"' Fill='" + StudioPixelHex(color) +
                    L"' Data='F1 " + glyph->data + L"'><Path.RenderTransform><ScaleTransform ScaleX='" +
                    std::to_wstring(scale) + L"' ScaleY='" + std::to_wstring(scale) +
                    L"'/></Path.RenderTransform></Path>";
            }
            cursor += glyph->advance * scale + tracking;
        }
    }
    xaml += L"</Canvas>";
    auto canvas = wux::Markup::XamlReader::Load(xaml).as<wuxc::Canvas>();
    host.Width(width + shadow);
    host.Height(height + shadow);
    host.Children().Append(canvas);
}
void UpdateStudioPixelTime() {
    if (!studioPixelClock) return;
    SYSTEMTIME now;
    GetLocalTime(&now);
    bool twelve = g_settings.timeFormat == L"12h";
    int hour = twelve ? (now.wHour % 12 == 0 ? 12 : now.wHour % 12) : now.wHour;
    wchar_t buffer[48];
    swprintf_s(buffer, twelve ? L"%d:%02d" : L"%02d:%02d", hour, now.wMinute);
    std::wstring time = buffer;
    if (g_settings.showSeconds) {
        swprintf_s(buffer, L":%02d", now.wSecond);
        time += buffer;
    }
    if (twelve) time += now.wHour >= 12 ? L" PM" : L" AM";
    auto const& theme = CurrentStudioPixelTheme();
    if (theme.twoTone) time[time.find(L':')] = L'|';
    auto fg = StudioColor(studio.foreground, theme.foreground);
    auto accent = StudioColor(studio.accent, theme.accent);
    bool layoutChanged = time != studioPixelLastTime;
    if (layoutChanged) {
        RenderStudioPixel(studioPixelClock, time, g_settings.clockFontSize, fg, accent, theme.twoTone, 0,studio.clockFont);
        studioPixelLastTime = time;
    }
    if (studioPixelDate && g_settings.showDateCapsule) {
        wchar_t dateBuffer[256]{};
        PCWSTR locale = studio.pixelDateLocale.empty() || studio.pixelDateLocale == L"system"
            ? LOCALE_NAME_USER_DEFAULT : studio.pixelDateLocale.c_str();
        PCWSTR format = studio.dateFormat.empty() ? nullptr : studio.dateFormat.c_str();
        if (!GetDateFormatEx(locale, format ? 0 : DATE_LONGDATE, &now, format,
                             dateBuffer, ARRAYSIZE(dateBuffer), nullptr)) {
            GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_LONGDATE, &now, nullptr,
                            dateBuffer, ARRAYSIZE(dateBuffer), nullptr);
        }
        CharUpperBuffW(dateBuffer, static_cast<DWORD>(wcslen(dateBuffer)));
        std::wstring date = dateBuffer;
        if (date != studioPixelLastDate) {
            std::wstring dateFont=studio.dateFont;
            if (dateFont.empty() && g_settings.clockStyle==L"pixel") dateFont=L"Segoe UI Variable Text";
            RenderStudioPixel(studioPixelDate, date, studio.dateSize, accent, accent, false, studio.pixelDateTracking,dateFont);
            studioPixelLastDate = date;
            layoutChanged = true;
        }
    }
    if (theme.strip && layoutChanged && studioPixelDate &&
        (studio.clockAnchor.empty() || studio.clockAnchor == L"theme") &&
        (studio.dateAnchor.empty() || studio.dateAnchor == L"theme")) {
        double gap = 36;
        StudioPlace(studioPixelClock, L"bottom", (studioPixelDate.Width() + gap) / 2 + studio.clockX,
                    theme.y + studio.clockY);
        StudioPlace(studioPixelDate, L"bottom", -(studioPixelClock.Width() + gap) / 2 + studio.clockX + studio.dateX,
                    theme.y + studio.clockY - (studioPixelClock.Height() - studioPixelDate.Height()) / 2 + studio.dateY);
    }
    if (g_batteryStatusText && g_settings.showBatteryStatus) {
        auto battery = GetCurrentBatteryInfo();
        if (battery.hasBattery && (battery.percentage != g_timeCache.lastBatteryPct ||
                                   battery.isCharging != g_timeCache.lastIsCharging)) {
            swprintf_s(buffer, L"%d%%", battery.percentage);
            g_batteryStatusText.Text(buffer);
            if (g_batteryIconText) g_batteryIconText.Text(battery.isCharging ? L"\u26A1 " : L"\U0001F50B ");
            g_timeCache.lastBatteryPct = battery.percentage;
            g_timeCache.lastIsCharging = battery.isCharging;
        }
    }
}
wux::UIElement BuildStudioPixel() {
    ResetStudioPixel();
    auto const& theme = CurrentStudioPixelTheme();
    wuxc::Grid root;
    root.Name(L"WindhawkStudioPixelRoot");
    root.IsHitTestVisible(false);
    wuxc::Grid::SetRowSpan(root, 100);
    wuxc::Grid::SetColumnSpan(root, 100);
    wuxc::Canvas::SetZIndex(root, 100);
    root.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(
        static_cast<uint8_t>(255 * studio.dim / 100), 0, 0, 0)));
    auto anchor = studio.clockAnchor == L"theme" || studio.clockAnchor.empty()
        ? std::wstring(theme.anchor) : studio.clockAnchor;
    bool useTheme = studio.clockAnchor == L"theme" || studio.clockAnchor.empty();
    double x = (useTheme ? theme.x : 0) + studio.clockX;
    double y = (useTheme ? theme.y : 0) + studio.clockY;
    wuxc::Grid clock;
    StudioPlace(clock, anchor, x, y);
    root.Children().Append(clock);
    studioPixelClock = clock;
    if (g_settings.showDateCapsule) {
        wuxc::Grid date;
        bool follows = studio.dateAnchor == L"theme" || studio.dateAnchor.empty();
        double clockHeight = g_settings.clockFontSize * kStudioPixelHeight / kStudioPixelEm;
        double dateHeight = studio.dateSize * kStudioPixelHeight / kStudioPixelEm;
        bool bottom = anchor == L"bottom" || anchor == L"bottomLeft" || anchor == L"bottomRight";
        double dateOffset = theme.dateAbove ? -(bottom ? clockHeight + 16 : dateHeight + 12)
                                            : (bottom ? dateHeight + 12 : clockHeight + 12);
        double dateX = follows ? x : 0;
        double dateY = follows ? y + dateOffset : 0;
        if (theme.strip && follows) { dateX = -150 + studio.clockX; dateY = y - 35; }
        StudioPlace(date, follows ? anchor : studio.dateAnchor, dateX + studio.dateX, dateY + studio.dateY);
        root.Children().Append(date);
        studioPixelDate = date;
    }
    if (!studio.text.empty()) {
        wuxc::Grid text;
        RenderStudioPixel(text, studio.text, studio.textSize,
            StudioColor(studio.foreground, theme.foreground), StudioColor(studio.accent, theme.accent), false, 1);
        StudioPlace(text, studio.textAnchor, studio.textX, studio.textY);
        root.Children().Append(text);
    }
    if (g_settings.showBatteryStatus) {
        auto battery = CreateBatteryCapsule(false, true);
        if (battery) {
            auto element = battery.as<wux::FrameworkElement>();
            element.Margin(wux::Thickness{0,0,0,0});
            StudioPlace(element, studio.batteryAnchor, studio.batteryX, studio.batteryY);
            root.Children().Append(element);
        }
    }
    UpdateStudioPixelTime();
    return root;
}

wux::UIElement BuildStudio() {
    if (IsStudioPixelStyle(g_settings.clockStyle)) return BuildStudioPixel();
    auto style = g_settings.clockStyle;
    unsigned long fg = 0xFFECEFF4, accent = 0xFF88C0D0, bg = 0xFF2E3440;
    if (style == L"catppuccin") { fg=0xFFCDD6F4; accent=0xFFCBA6F7; bg=0xFF1E1E2E; }
    if (style == L"gruvbox") { fg=0xFFEBDBB2; accent=0xFFFABD2F; bg=0xFF282828; }
    if (style == L"hyprland") { fg=0xFFCFFAFE; accent=0xFF2DD4BF; bg=0xFF0F172A; }
    if (style == L"kde") { fg=0xFFEFF0F1; accent=0xFF3DAEE9; bg=0xFF232629; }
    if (style == L"gnome") { fg=0xFFFFFFFF; accent=0xFFFFFFFF; bg=0xFF202020; }
    auto foreground = wuxm::SolidColorBrush(StudioColor(studio.foreground, fg));
    auto highlight = wuxm::SolidColorBrush(StudioColor(studio.accent, accent));
    auto panelColor = StudioColor(studio.panel, bg);
    panelColor.A = static_cast<uint8_t>(panelColor.A * studio.opacity / 100);
    wuxc::Grid root;
    root.Name(L"WindhawkStudioRoot");
    root.IsHitTestVisible(false);
    // Independent overlay spans the parent, including Grid row/column layouts.
    wuxc::Grid::SetRowSpan(root, 100);
    wuxc::Grid::SetColumnSpan(root, 100);
    wuxc::Canvas::SetZIndex(root, 100);
    root.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(
        static_cast<uint8_t>(255 * studio.dim / 100), 0, 0, 0)));
    std::wstring anchor = studio.clockAnchor;
    bool terminal = style == L"hyprland";
    if (anchor == L"theme" || anchor.empty()) anchor = terminal ? L"left" : L"center";
    double themeX = terminal ? 100 : 0;
    wuxc::Border card;
    card.Padding(wux::Thickness{28, 8, 28, 8});
    card.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(studio.radius));
    if (style != L"gnome" || !studio.panel.empty())
        card.Background(wuxm::SolidColorBrush(panelColor));
    if (terminal || style == L"kde") {
        card.BorderBrush(highlight);
        card.BorderThickness(wux::Thickness{1,1,1,1});
    }
    wuxc::TextBlock clock;
    clock.Name(L"StudioClock");
    clock.FontSize(g_settings.clockFontSize);
    clock.FontWeight(winrt::Windows::UI::Text::FontWeight{static_cast<uint16_t>(studio.weight)});
    std::wstring font = studio.font.empty() ?
        (terminal ? L"Cascadia Code" : L"Segoe UI Variable Display") : studio.font;
    clock.FontFamily(wuxm::FontFamily(studio.clockFont.empty() ? font : studio.clockFont));
    clock.Foreground(foreground);
    card.Child(clock);
    StudioPlace(card, anchor, themeX + studio.clockX, studio.clockY);
    root.Children().Append(card);
    g_auraTimeTextBlock = clock;
    if (g_settings.showDateCapsule) {
        wuxc::TextBlock date;
        date.Name(L"StudioDate");
        date.FontSize(studio.dateSize);
        date.FontFamily(wuxm::FontFamily(studio.dateFont.empty() ? font : studio.dateFont));
        date.Foreground(highlight);
        bool follows = studio.dateAnchor == L"theme" || studio.dateAnchor.empty();
        StudioPlace(date, follows ? anchor : studio.dateAnchor,
                    (follows ? themeX : 0) + studio.dateX,
                    (follows ? g_settings.clockFontSize * 0.85 + studio.clockY : 0) + studio.dateY);
        root.Children().Append(date);
        g_dateTextBlock = date;
    }
    if (!studio.text.empty()) {
        wuxc::TextBlock text;
        text.Text(studio.text);
        text.FontSize(studio.textSize);
        text.FontFamily(wuxm::FontFamily(font));
        text.Foreground(highlight);
        StudioPlace(text, studio.textAnchor, studio.textX, studio.textY);
        root.Children().Append(text);
    }
    if (g_settings.showBatteryStatus) {
        auto battery = CreateBatteryCapsule(false, true);
        if (battery) {
            auto element = battery.as<wux::FrameworkElement>();
            element.Margin(wux::Thickness{0,0,0,0});
            StudioPlace(element, studio.batteryAnchor, studio.batteryX, studio.batteryY);
            root.Children().Append(element);
        }
    }
    return root;
}
void ApplyStudioClockFonts(wux::DependencyObject const& node,int depth=0) {
    if (!node || depth>96) return;
    if (auto text=node.try_as<wuxc::TextBlock>()) {
        bool date=text==g_dateTextBlock || text==g_macosDateTextBlock || text==g_macosDateShadowBlock;
        bool clock=text==g_hourTextBlock || text==g_hourBotTextBlock || text==g_minTextBlock ||
            text==g_minBotTextBlock || text==g_secTextBlock || text==g_ampmTextBlock ||
            text==g_auraTimeTextBlock || text==g_auraShadowTextBlock ||
            text==g_macosTimeTextBlock || text==g_macosShadowTextBlock;
        auto const& family=date ? studio.dateFont : studio.clockFont;
        if ((date || clock) && !family.empty()) text.FontFamily(wuxm::FontFamily(family));
    }
    int count=wuxm::VisualTreeHelper::GetChildrenCount(node);
    for(int i=0;i<count;++i) ApplyStudioClockFonts(wuxm::VisualTreeHelper::GetChild(node,i),depth+1);
}
struct StudioNativeValue {
    wux::FrameworkElement element{nullptr};
    wux::DependencyProperty property{nullptr};
    winrt::Windows::Foundation::IInspectable value{nullptr};
    unsigned long long scan = 0;
};
[[clang::no_destroy]] std::vector<StudioNativeValue> studioNative;
unsigned long long studioNativeScan = 0;
[[clang::no_destroy]] wux::FrameworkElement studioWidget{nullptr};
[[clang::no_destroy]] wux::FrameworkElement studioWidgetMotion{nullptr};
bool studioWidgetLogged = false;
bool studioMediaLogged = false;
std::vector<std::wstring> studioLoggedNames;
void ResetStudioMediaRefresh();
void ResetStudioCardOverlays();
void FinishStudioCardOverlays();
void ResetStudioWidgets();
void RestoreStudioNative() {
    ResetStudioMediaRefresh();
    ResetStudioWidgets();
    ResetStudioCardOverlays();
    for (auto const& item : studioNative) try {
        if (item.value == wux::DependencyProperty::UnsetValue()) item.element.ClearValue(item.property);
        else item.element.SetValue(item.property, item.value);
    } catch (...) {}
    studioNative.clear();
    studioWidget = nullptr;
    studioWidgetMotion = nullptr;
    studioWidgetLogged = false;
    studioMediaLogged = false;
    studioLoggedNames.clear();
}
void StudioSave(wux::FrameworkElement const& e, wux::DependencyProperty const& p) {
    for (auto& item : studioNative) if (item.element == e && item.property == p) {
        item.scan = studioNativeScan;
        return;
    }
    studioNative.push_back({e, p, e.ReadLocalValue(p), studioNativeScan});
}
void FinishStudioNativeScan() {
    FinishStudioCardOverlays();
    // A fallback container can be replaced by WidgetGroupPanel later. Restore
    // the old target so offsets never accumulate on both parent and child.
    studioNative.erase(std::remove_if(studioNative.begin(), studioNative.end(),
        [](StudioNativeValue const& item) {
            if (item.scan == studioNativeScan) return false;
            try {
                if (item.value == wux::DependencyProperty::UnsetValue()) item.element.ClearValue(item.property);
                else item.element.SetValue(item.property, item.value);
            } catch (...) {}
            return true;
        }), studioNative.end());
}
// Prefer the actual cards panel over an outer canvas that Windows can animate
// or clip. Select once per scan so only one group receives the offset.
void FindStudioWidget(wux::DependencyObject const& node, int depth, int& bestRank) {
    if (!node || depth > 96 || (g_activeClockElement && node == g_activeClockElement)) return;
    if (auto e = node.try_as<wux::FrameworkElement>()) {
        auto name = e.Name();
        int rank = 100;
        if (!studio.widgetTarget.empty() && studio.widgetTarget != L"auto") {
            if (name == studio.widgetTarget) rank = 0;
        } else {
            if (name == L"WidgetGroupPanel") rank = 0;
            else if (name == L"LockWidgetCanvas") rank = 1;
            else if (name == L"AdaptiveCardDetailContainer") rank = 2;
            else if (name == L"DetailTextPanel") rank = 3;
        }
        if (rank < bestRank) { studioWidget = e; bestRank = rank; }
    }
    int count = wuxm::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i) try {
        FindStudioWidget(wuxm::VisualTreeHelper::GetChild(node, i), depth + 1, bestRank);
    } catch (...) {}
}
bool StudioWidgetOnlyBranch(wux::DependencyObject const& node,wux::DependencyObject const& branch,int depth,int& budget) {
    if (!node || depth>96 || --budget<0) return false;
    if (node==branch) return true;
    if (g_activeClockElement && node==g_activeClockElement) return false;
    if (auto e=node.try_as<wux::FrameworkElement>()) {
        auto name=e.Name();
        if (name==L"Time" || name==L"Date" || name==L"TimeAndDatePanel" ||
            name==L"MediaControls" || name==L"MediaTransportControls") return false;
        if (e.try_as<winrt::Windows::UI::Xaml::Controls::Primitives::ScrollBar>()) return true;
        if (depth>0 && (e.try_as<wuxc::TextBlock>() || e.try_as<wuxc::Image>() ||
            e.try_as<wuxc::IconElement>() || e.try_as<wuxc::Control>())) return false;
        wuxm::Brush background{nullptr};
        if (auto grid=e.try_as<wuxc::Panel>()) background=grid.Background();
        else if (auto border=e.try_as<wuxc::Border>()) background=border.Background();
        else if (auto control=e.try_as<wuxc::Control>()) background=control.Background();
        if (background && background.try_as<wuxm::ImageBrush>()) return false;
    }
    int count=wuxm::VisualTreeHelper::GetChildrenCount(node);
    for(int i=0;i<count;++i)
        if (!StudioWidgetOnlyBranch(wuxm::VisualTreeHelper::GetChild(node,i),branch,depth+1,budget)) return false;
    return true;
}
void SelectStudioWidget(wux::DependencyObject const& content) {
    auto previous = studioWidget;
    auto previousMotion = studioWidgetMotion;
    studioWidget = nullptr;
    int bestRank = 100;
    FindStudioWidget(content, 0, bestRank);
    studioWidgetMotion = studioWidget;
    if (studioWidget && studio.widgetMoveContainer != L"panel") {
        wux::DependencyObject branch=studioWidget;
        auto parent = wuxm::VisualTreeHelper::GetParent(studioWidget);
        for (int i = 0; parent && i < 96 && parent != content; ++i) {
            if (auto element = parent.try_as<wux::FrameworkElement>()) {
                auto name = element.Name();
                if (name == L"LockRootGrid" || name == L"MainPageContainer") break;
                if (name == L"LockWidgetCanvas" || name == L"AdaptiveCardDetailContainer" || name == L"DetailTextPanel")
                    studioWidgetMotion = element;
                else {
                    int budget=4096;
                    if (!StudioWidgetOnlyBranch(parent,branch,0,budget)) break;
                    studioWidgetMotion=element;
                }
            }
            branch=parent;
            parent = wuxm::VisualTreeHelper::GetParent(parent);
        }
    }
    if (previousMotion && previousMotion != studioWidgetMotion) {
        studioNative.erase(std::remove_if(studioNative.begin(),studioNative.end(),
            [&previousMotion](StudioNativeValue const& saved) {
                if (saved.element != previousMotion || saved.property != wux::UIElement::RenderTransformProperty()) return false;
                if (saved.value == wux::DependencyProperty::UnsetValue()) saved.element.ClearValue(saved.property);
                else saved.element.SetValue(saved.property,saved.value);
                return true;
            }),studioNative.end());
    }
    if (studioWidget != previous) studioWidgetLogged = false;
    if (!studioWidgetLogged && (studio.widgetX || studio.widgetY || studio.diagnostics)) {
        if (studioWidget) {
            Wh_Log(L"Studio widgets: target=%s, offset=(%d,%d)",
                studioWidget.Name().c_str(), studio.widgetX, studio.widgetY);
            if (studioWidgetMotion) Wh_Log(L"Studio widget motion host: %s (%s)",
                studioWidgetMotion.Name().c_str(),winrt::get_class_name(studioWidgetMotion).c_str());
            studioWidgetLogged = true;
        } else if (!studioDumpDone) {
            Wh_Log(L"Studio widgets: no matching group yet; retrying. Enable diagnostics to inspect names.");
        }
    }
}
void StudioSet(wux::FrameworkElement const& e, wux::DependencyProperty const& p,
               winrt::Windows::Foundation::IInspectable const& value) {
    StudioSave(e, p);
    // Fast media refresh must not continually invalidate unchanged visuals.
    auto current = e.GetValue(p);
    if (current == value) return;
    if (auto requested=value.try_as<wuxm::FontFamily>()) {
        if (auto actual=current.try_as<wuxm::FontFamily>())
            if (requested.Source()==actual.Source()) return;
    }
    if (auto requested = value.try_as<wuxm::SolidColorBrush>()) {
        if (auto actual = current.try_as<wuxm::SolidColorBrush>())
            if (requested.Color() == actual.Color() && requested.Opacity() == actual.Opacity()) return;
    }
    if (auto requested = value.try_as<winrt::Windows::Foundation::IReference<double>>()) {
        if (auto actual = current.try_as<winrt::Windows::Foundation::IReference<double>>())
            if (requested.Value() == actual.Value()) return;
    }
    if (auto requested = value.try_as<winrt::Windows::Foundation::IReference<wux::Thickness>>()) {
        if (auto actual = current.try_as<winrt::Windows::Foundation::IReference<wux::Thickness>>())
            if (requested.Value() == actual.Value()) return;
    }
    if (auto requested = value.try_as<winrt::Windows::Foundation::IReference<wux::CornerRadius>>()) {
        if (auto actual = current.try_as<winrt::Windows::Foundation::IReference<wux::CornerRadius>>())
            if (requested.Value() == actual.Value()) return;
    }
    e.SetValue(p, value);
}
bool StudioMediaName(winrt::hstring const& name) {
    return name == L"MediaControls" || name == L"MediaTransportControls";
}
bool StudioInsideMedia(wux::DependencyObject node) {
    for (int depth = 0; node && depth < 96; ++depth) {
        node = wuxm::VisualTreeHelper::GetParent(node);
        if (auto parent = node.try_as<wux::FrameworkElement>())
            if (StudioMediaName(parent.Name())) return true;
    }
    return false;
}
// Pure placement policy, also exercised by tools/test-widget-placement.cpp.
#include <algorithm>
#include <cmath>
namespace MitsuPlacement {
inline double ClampOffset(double start, double extent, double viewport, double padding, double requested) {
    if (!std::isfinite(start) || !std::isfinite(extent) || !std::isfinite(viewport) ||
        !std::isfinite(requested) || extent <= 0 || viewport <= 0) return 0;
    padding = (std::clamp)(padding, 0.0, viewport / 2.0);
    double low = padding - start;
    double high = viewport - padding - extent - start;
    // If the entire row cannot fit, center it so both ends remain reachable.
    if (low > high) return (viewport - extent) / 2.0 - start;
    return (std::clamp)(requested, low, high);
}
struct Offset { double x,y; };
inline Offset KeepVisible(double left,double top,double width,double height,
    double viewportWidth,double viewportHeight,double padding,Offset previous,
    Offset requested,Offset xAxis,Offset yAxis) {
    double determinant=xAxis.x*yAxis.y-yAxis.x*xAxis.y;
    if (!std::isfinite(determinant) || std::abs(determinant)<1e-6) return {0,0};
    double baseX=left-previous.x*xAxis.x-previous.y*yAxis.x;
    double baseY=top-previous.x*xAxis.y-previous.y*yAxis.y;
    double x=ClampOffset(baseX,width,viewportWidth,padding,requested.x*xAxis.x+requested.y*yAxis.x);
    double y=ClampOffset(baseY,height,viewportHeight,padding,requested.x*xAxis.y+requested.y*yAxis.y);
    return {(x*yAxis.y-y*yAxis.x)/determinant,(y*xAxis.x-x*xAxis.y)/determinant};
}
}

// Shared native-card skins. Data, album art, weather icons and native controls
// remain owned by Windows. This is a XAML skin, not a replacement data service.
bool StudioCardSkinName(std::wstring const& name) {
    return name == L"pixel" || name == L"macos" || name == L"macosDark" ||
        name == L"smoke" || name == L"minimal" || name == L"nord" ||
        name == L"catppuccin" || name == L"gruvbox";
}
struct StudioCardPalette { unsigned long background, foreground, accent; };
StudioCardPalette StudioCardColors(std::wstring const& name) {
    if (name == L"pixel") return {0xFF241D2D,0xFFFFE5C2,0xFFE6B890};
    if (name == L"macos") return {0xFFF5F5F7,0xFF1D1D1F,0xFF007AFF};
    if (name == L"macosDark") return {0xFF1C1C1E,0xFFF5F5F7,0xFF0A84FF};
    if (name == L"nord") return {0xFF2E3440,0xFFECEFF4,0xFF88C0D0};
    if (name == L"catppuccin") return {0xFF1E1E2E,0xFFCDD6F4,0xFFCBA6F7};
    if (name == L"gruvbox") return {0xFF282828,0xFFEBDBB2,0xFFFABD2F};
    return {0xFF1C2030,0xFFF1F5F9,0xFF94A3B8};
}
void StyleStudioCardSurface(wux::FrameworkElement const& e, bool widget, bool outer = true) {
    auto const& theme = widget ? studio.widgetTheme : studio.mediaTheme;
    auto colors = StudioCardColors(theme);
    auto const& bg = widget ? studio.widgetBackground : studio.mediaBackground;
    auto background = StudioColor(bg, colors.background);
    background.A = static_cast<uint8_t>(background.A * (widget ? studio.widgetOpacity : studio.mediaOpacity) / 100);
    if (theme == L"minimal" && bg.empty()) background.A = 0;
    // The card owner draws the only background and outline. Template layers
    // below it must not retain either Windows' border or another themed border.
    if (!outer) background.A = 0;
    auto brush = wuxm::SolidColorBrush(background);
    auto borderBrush = wuxm::SolidColorBrush(StudioColor(widget ? studio.widgetAccent : studio.mediaAccent, colors.accent));
    auto foreground = wuxm::SolidColorBrush(StudioColor(widget ? studio.widgetForeground : studio.mediaForeground, colors.foreground));
    double radius = theme == L"pixel" ? 0 : (widget ? studio.widgetRadius : studio.mediaRadius);
    auto corners = wux::CornerRadiusHelper::FromUniformRadius(radius);
    auto thickness = wux::ThicknessHelper::FromUniformLength(outer ? (theme == L"pixel" ? 2 : 1) : 0);
    if (!outer) borderBrush = wuxm::SolidColorBrush(wu::Colors::Transparent());
    if (e.try_as<wuxc::Border>()) {
        StudioSet(e, wuxc::Border::BackgroundProperty(), brush);
        StudioSet(e, wuxc::Border::BorderBrushProperty(), borderBrush);
        StudioSet(e, wuxc::Border::BorderThicknessProperty(), winrt::box_value(thickness));
        StudioSet(e, wuxc::Border::CornerRadiusProperty(), winrt::box_value(corners));
    } else if (e.try_as<wuxc::Control>()) {
        StudioSet(e, wuxc::Control::BackgroundProperty(), brush);
        StudioSet(e, wuxc::Control::ForegroundProperty(), foreground);
        StudioSet(e, wuxc::Control::BorderBrushProperty(), borderBrush);
        StudioSet(e, wuxc::Control::BorderThicknessProperty(), winrt::box_value(thickness));
        StudioSet(e, wuxc::Control::CornerRadiusProperty(), winrt::box_value(corners));
    } else if (e.try_as<wuxc::Grid>()) {
        StudioSet(e, wuxc::Panel::BackgroundProperty(), brush);
        StudioSet(e, wuxc::Grid::BorderBrushProperty(), borderBrush);
        StudioSet(e, wuxc::Grid::BorderThicknessProperty(), winrt::box_value(thickness));
        StudioSet(e, wuxc::Grid::CornerRadiusProperty(), winrt::box_value(corners));
    } else if (e.try_as<wuxc::ContentPresenter>()) {
        StudioSet(e, wuxc::ContentPresenter::BackgroundProperty(), brush);
        StudioSet(e, wuxc::ContentPresenter::BorderBrushProperty(), borderBrush);
        StudioSet(e, wuxc::ContentPresenter::BorderThicknessProperty(), winrt::box_value(thickness));
        StudioSet(e, wuxc::ContentPresenter::CornerRadiusProperty(), winrt::box_value(corners));
    }
}
bool StudioCardSurfaceType(wux::FrameworkElement const& e) {
    return e.try_as<wuxc::Border>() || e.try_as<wuxc::Control>() ||
        e.try_as<wuxc::Grid>() || e.try_as<wuxc::ContentPresenter>();
}
wux::FrameworkElement FindStudioCardPart(wux::DependencyObject const& node, bool frame, int depth, int& budget) {
    if (!node || depth>32 || --budget<0) return nullptr;
    if (auto element=node.try_as<wux::FrameworkElement>()) {
        if (element.Name()==L"WindhawkStudioCardPixelLayer") return nullptr;
        if (frame) {
            auto type=std::wstring(winrt::get_class_name(element));
            if (type.find(L"LockCanvasWidgetFrame")!=std::wstring::npos) return element;
        } else if (element.Name()==L"WidgetFrameGrid" && element.try_as<wuxc::Grid>()) return element;
    }
    int count=wuxm::VisualTreeHelper::GetChildrenCount(node);
    for (int i=0;i<count && budget>0;++i) try {
        auto result=FindStudioCardPart(wuxm::VisualTreeHelper::GetChild(node,i),frame,depth+1,budget);
        if (result) return result;
    } catch (...) {}
    return nullptr;
}
wux::FrameworkElement StudioCardOwner(wux::FrameworkElement const& e, bool widget) {
    wux::FrameworkElement frame{nullptr}, fallback{nullptr};
    wux::DependencyObject node=e;
    for (int depth=0;node && depth<96;++depth) {
        if (widget && node==studioWidget) break;
        if (auto element=node.try_as<wux::FrameworkElement>()) {
            auto name=element.Name();
            if (name==L"LockRootGrid" || name==L"MainPageContainer") break;
            if (widget) {
                // Prefer the actual per-card frame, not both it and its inner
                // WidgetFrameGrid. Fallback picks the individual slot surface.
                auto type=std::wstring(winrt::get_class_name(element));
                if (type.find(L"LockCanvasWidgetFrame")!=std::wstring::npos) frame=element;
                if (StudioCardSurfaceType(element)) fallback=element;
            } else if (StudioMediaName(name)) frame=element;
        }
        node=wuxm::VisualTreeHelper::GetParent(node);
    }
    if (widget) {
        // Slot presenters above a frame must also resolve to that frame's
        // surface. Otherwise both slot and frame would claim their own border.
        if (!frame) { int budget=512; frame=FindStudioCardPart(e,true,0,budget); }
        if (frame) {
            int budget=512;
            auto grid=FindStudioCardPart(frame,false,0,budget);
            return grid ? grid : frame;
        }
    }
    // A legacy single-card root can itself be the selected widget container.
    if (widget && !frame && !fallback && studioWidget && e==studioWidget &&
        e.Name()!=L"WidgetGroupPanel" && StudioCardSurfaceType(e)) return e;
    return frame ? frame : fallback;
}
void StyleStudioCardChrome(wux::FrameworkElement const& e, bool widget) {
    if (!StudioCardSurfaceType(e)) return;
    auto owner=StudioCardOwner(e,widget);
    if (!owner) return;
    if (e==owner) { StyleStudioCardSurface(e,widget,true); return; }
    auto name=e.Name();
    bool wrapper=name==L"WidgetFrameGrid" || name==L"RootGrid" ||
        name==L"BackgroundBorder" || name==L"LayoutRoot";
    // Also suppress unnamed same-size template chrome. Small controls, album
    // art and content panels are not given full-card outlines or backgrounds.
    bool fullCard=owner.ActualWidth()>0 && owner.ActualHeight()>0 &&
        e.ActualWidth()>=owner.ActualWidth()*0.75 &&
        e.ActualHeight()>=owner.ActualHeight()*0.75;
    if (wrapper || fullCard) StyleStudioCardSurface(e,widget,false);
}
bool StudioInsideWidget(wux::DependencyObject node) {
    for (int i = 0; node && i < 96; ++i) {
        if (studioWidget && node == studioWidget) return true;
        node = wuxm::VisualTreeHelper::GetParent(node);
    }
    return false;
}
bool StudioWidgetWrapper(wux::FrameworkElement const& e) {
    if (!studioWidget || !studioWidgetMotion || studioWidget==studioWidgetMotion) return false;
    auto node=studioWidget ? wuxm::VisualTreeHelper::GetParent(studioWidget) : nullptr;
    for (int i=0;node && i<96;++i) {
        if (node==e) return true;
        if (studioWidgetMotion && node==studioWidgetMotion) break;
        node=wuxm::VisualTreeHelper::GetParent(node);
    }
    return false;
}
struct StudioPixelLayer { wuxc::Grid parent{nullptr}; wuxc::Canvas canvas{nullptr}; };
struct StudioPixelLabel {
    winrt::weak_ref<wuxc::TextBlock> source;
    wuxc::Canvas layer{nullptr};
    wuxc::Grid image{nullptr};
    std::wstring text, color;
    double size = 0;
    unsigned long long scan = 0;
};
[[clang::no_destroy]] std::vector<StudioPixelLayer> studioPixelLayers;
[[clang::no_destroy]] std::vector<StudioPixelLabel> studioPixelLabels;
void ResetStudioCardOverlays() {
    studioPixelLabels.clear();
    for (auto& layer : studioPixelLayers) try {
        uint32_t index;
        if (layer.parent.Children().IndexOf(layer.canvas, index)) layer.parent.Children().RemoveAt(index);
    } catch (...) {}
    studioPixelLayers.clear();
}
void FinishStudioCardOverlays() {
    studioPixelLabels.erase(std::remove_if(studioPixelLabels.begin(), studioPixelLabels.end(),
        [](StudioPixelLabel const& label) {
            if (label.scan == studioNativeScan && label.source.get()) return false;
            try { uint32_t i; if (label.layer.Children().IndexOf(label.image, i)) label.layer.Children().RemoveAt(i); } catch (...) {}
            return true;
        }), studioPixelLabels.end());
    studioPixelLayers.erase(std::remove_if(studioPixelLayers.begin(), studioPixelLayers.end(),
        [](StudioPixelLayer const& layer) {
            if (layer.canvas.Children().Size()) return false;
            try { uint32_t i; if (layer.parent.Children().IndexOf(layer.canvas, i)) layer.parent.Children().RemoveAt(i); } catch (...) {}
            return true;
        }), studioPixelLayers.end());
}
void DropStudioPixelLabel(wuxc::TextBlock const& text) {
    studioPixelLabels.erase(std::remove_if(studioPixelLabels.begin(),studioPixelLabels.end(),
        [&text](StudioPixelLabel const& label) {
            if (label.source.get() != text) return false;
            uint32_t index;
            if (label.layer.Children().IndexOf(label.image,index)) label.layer.Children().RemoveAt(index);
            return true;
        }),studioPixelLabels.end());
    studioNative.erase(std::remove_if(studioNative.begin(),studioNative.end(),
        [&text](StudioNativeValue const& saved) {
            if (saved.element != text || saved.property != wux::UIElement::OpacityProperty()) return false;
            if (saved.value == wux::DependencyProperty::UnsetValue()) text.ClearValue(saved.property);
            else text.SetValue(saved.property,saved.value);
            return true;
        }),studioNative.end());
}
void StudioPixelizeLabel(wuxc::TextBlock const& text, wu::Color color) {
    // Keep wrapping paragraphs native. Single-line values (temperature, city,
    // title, artist) get pixel outlines without replacing their data bindings.
    if (text.Text().empty() || text.TextWrapping() != wux::TextWrapping::NoWrap ||
        text.Visibility() != wux::Visibility::Visible ||
        text.ActualWidth() <= 0 || text.ActualHeight() <= 0) {
        DropStudioPixelLabel(text);
        return;
    }
    wuxc::Grid parent{nullptr};
    auto node = wuxm::VisualTreeHelper::GetParent(text);
    for (int i = 0; node && i < 32; ++i) {
        if ((parent = node.try_as<wuxc::Grid>())) break;
        node = wuxm::VisualTreeHelper::GetParent(node);
    }
    if (!parent) return;
    wuxc::Canvas layer{nullptr};
    for (auto const& candidate : studioPixelLayers)
        if (candidate.parent == parent) { layer = candidate.canvas; break; }
    if (!layer) {
        layer = wuxc::Canvas();
        layer.Name(L"WindhawkStudioCardPixelLayer");
        layer.IsHitTestVisible(false);
        wuxc::Grid::SetRowSpan(layer,100);
        wuxc::Grid::SetColumnSpan(layer,100);
        wuxc::Canvas::SetZIndex(layer,99);
        parent.Children().Append(layer);
        studioPixelLayers.push_back({parent,layer});
    }
    auto found = std::find_if(studioPixelLabels.begin(),studioPixelLabels.end(),
        [&text](StudioPixelLabel const& label) { return label.source.get() == text; });
    if (found != studioPixelLabels.end() && found->layer != layer) {
        uint32_t index;
        if (found->layer.Children().IndexOf(found->image,index)) found->layer.Children().RemoveAt(index);
        studioPixelLabels.erase(found);
        found = studioPixelLabels.end();
    }
    if (found == studioPixelLabels.end()) {
        wuxc::Grid image;
        layer.Children().Append(image);
        studioPixelLabels.push_back({winrt::make_weak(text),layer,image});
        found = studioPixelLabels.end()-1;
    }
    found->scan = studioNativeScan;
    auto hex = StudioPixelHex(color);
    if (found->text != text.Text() || found->size != text.FontSize() || found->color != hex) {
        RenderStudioPixel(found->image,std::wstring(text.Text()),text.FontSize(),color,color,false,0);
        found->text = text.Text(); found->size = text.FontSize(); found->color = hex;
    }
    auto bounds = text.TransformToVisual(layer).TransformBounds(
        winrt::Windows::Foundation::Rect{0,0,static_cast<float>(text.ActualWidth()),static_cast<float>(text.ActualHeight())});
    double scale = (std::min)({1.0,bounds.Width/found->image.Width(),bounds.Height/found->image.Height()});
    if (!std::isfinite(scale) || scale <= 0) return;
    auto old = found->image.RenderTransform().try_as<wuxm::ScaleTransform>();
    if (!old || old.ScaleX() != scale) {
        wuxm::ScaleTransform transform; transform.ScaleX(scale); transform.ScaleY(scale);
        found->image.RenderTransform(transform);
    }
    double x = bounds.X;
    if (text.TextAlignment() == wux::TextAlignment::Center) x += (bounds.Width-found->image.Width()*scale)/2;
    if (text.TextAlignment() == wux::TextAlignment::Right) x += bounds.Width-found->image.Width()*scale;
    wuxc::Canvas::SetLeft(found->image,x);
    wuxc::Canvas::SetTop(found->image,bounds.Y+(bounds.Height-found->image.Height()*scale)/2);
    StudioSet(text,wux::UIElement::OpacityProperty(),winrt::box_value(0.0));
}
void StyleStudioCardText(wuxc::TextBlock const& text,bool widget) {
    auto family = text.FontFamily().Source();
    if (family == L"Segoe Fluent Icons" || family == L"Segoe MDL2 Assets" || family == L"Segoe UI Symbol") return;
    auto const& theme = widget ? studio.widgetTheme : studio.mediaTheme;
    auto const& font=widget ? studio.widgetFont : studio.mediaFont;
    if (!font.empty()) {
        DropStudioPixelLabel(text);
        StudioSet(text,wuxc::TextBlock::FontFamilyProperty(),wuxm::FontFamily(font));
    }
    if (!StudioCardSkinName(theme)) return;
    auto palette = StudioCardColors(theme);
    auto color = StudioColor(widget ? studio.widgetForeground : studio.mediaForeground,palette.foreground);
    StudioSet(text,wuxc::TextBlock::ForegroundProperty(),wuxm::SolidColorBrush(color));
    int size = widget ? studio.widgetTextSize : studio.mediaTextSize;
    if (size) StudioSet(text,wuxc::TextBlock::FontSizeProperty(),winrt::box_value(static_cast<double>((std::max)(10,size))));
    if (theme == L"pixel" && font.empty()) StudioPixelizeLabel(text,color);
}
void CollectStudioWidgetFrames(wux::DependencyObject const& node,std::vector<wux::FrameworkElement>& frames,int depth,int& budget) {
    if (!node || depth>96 || --budget<0 || frames.size()>=8) return;
    if (auto e=node.try_as<wux::FrameworkElement>()) {
        if (e.Name()==L"WindhawkStudioCardPixelLayer" || e.Name()==L"WindhawkStudioWidgets") return;
        auto type=std::wstring(winrt::get_class_name(e));
        if (type.find(L"LockCanvasWidgetFrame")!=std::wstring::npos || e.Name()==L"WidgetFrameGrid") {
            int remaining=512;
            auto grid=FindStudioCardPart(e,false,0,remaining);
            frames.push_back(grid ? grid : e);
            return;
        }
    }
    int count=wuxm::VisualTreeHelper::GetChildrenCount(node);
    for(int i=0;i<count && budget>0;++i) try {
        CollectStudioWidgetFrames(wuxm::VisualTreeHelper::GetChild(node,i),frames,depth+1,budget);
    } catch (...) {}
}
std::vector<wux::FrameworkElement> StudioWidgetFrames() {
    std::vector<wux::FrameworkElement> frames;
    int budget=8192;
    if (studioWidget) CollectStudioWidgetFrames(studioWidget,frames,0,budget);
    if (frames.empty() && studioWidget) frames.push_back(studioWidget);
    return frames;
}
void ClampStudioWidgetOffset(double& x,double& y) {
    if (!studio.widgetKeepOnScreen || !studioWidget || !studioWidgetMotion) return;
    auto window = wux::Window::Current();
    auto content = window ? window.Content().try_as<wux::FrameworkElement>() : nullptr;
    if (!content || content.ActualWidth() <= 0 || content.ActualHeight() <= 0) return;
    winrt::Windows::Foundation::Rect bounds{};
    bool first=true;
    for(auto const& frame:StudioWidgetFrames()) {
        if (frame.ActualWidth()<=0 || frame.ActualHeight()<=0) continue;
        auto rect=frame.TransformToVisual(content).TransformBounds(
            winrt::Windows::Foundation::Rect{0,0,static_cast<float>(frame.ActualWidth()),static_cast<float>(frame.ActualHeight())});
        if (first) { bounds=rect; first=false; }
        else {
            float right=(std::max)(bounds.X+bounds.Width,rect.X+rect.Width);
            float bottom=(std::max)(bounds.Y+bounds.Height,rect.Y+rect.Height);
            bounds.X=(std::min)(bounds.X,rect.X); bounds.Y=(std::min)(bounds.Y,rect.Y);
            bounds.Width=right-bounds.X; bounds.Height=bottom-bounds.Y;
        }
    }
    if (bounds.Width <= 0 || bounds.Height <= 0) { x=0; y=0; return; }
    // Include shared ancestor viewports, not just the display edges.
    double visibleLeft=0,visibleTop=0,visibleRight=content.ActualWidth(),visibleBottom=content.ActualHeight();
    auto parent=wuxm::VisualTreeHelper::GetParent(studioWidgetMotion);
    for(int depth=0;parent && depth<96;++depth) {
        if (auto e=parent.try_as<wux::FrameworkElement>()) {
            auto clip=e.Clip();
            bool viewport=e.try_as<wuxc::ScrollViewer>() || e.try_as<wuxc::ScrollContentPresenter>();
            if (clip || viewport) {
                auto local=clip ? clip.Rect() : winrt::Windows::Foundation::Rect{
                    0,0,static_cast<float>(e.ActualWidth()),static_cast<float>(e.ActualHeight())};
                if (clip && clip.Transform()) local=clip.Transform().TransformBounds(local);
                auto rect=e.TransformToVisual(content).TransformBounds(local);
                visibleLeft=(std::max)(visibleLeft,static_cast<double>(rect.X));
                visibleTop=(std::max)(visibleTop,static_cast<double>(rect.Y));
                visibleRight=(std::min)(visibleRight,static_cast<double>(rect.X+rect.Width));
                visibleBottom=(std::min)(visibleBottom,static_cast<double>(rect.Y+rect.Height));
            }
        }
        if (parent==content) break;
        parent=wuxm::VisualTreeHelper::GetParent(parent);
    }
    if (visibleRight<=visibleLeft || visibleBottom<=visibleTop) { x=0; y=0; return; }
    double previousX=0,previousY=0;
    for (auto const& saved : studioNative) {
        if (saved.element != studioWidgetMotion || saved.property != wux::UIElement::RenderTransformProperty()) continue;
        auto group = studioWidgetMotion.RenderTransform().try_as<wuxm::TransformGroup>();
        if (group && group.Children().Size())
            if (auto last = group.Children().GetAt(group.Children().Size()-1).try_as<wuxm::TranslateTransform>()) {
                previousX=last.X(); previousY=last.Y();
            }
        break;
    }
    auto transform=studioWidgetMotion.TransformToVisual(content);
    auto origin=transform.TransformPoint({0,0});
    auto unitX=transform.TransformPoint({1,0});
    auto unitY=transform.TransformPoint({0,1});
    auto safe=MitsuPlacement::KeepVisible(bounds.X-visibleLeft,bounds.Y-visibleTop,bounds.Width,bounds.Height,
        visibleRight-visibleLeft,visibleBottom-visibleTop,studio.widgetScreenPadding,
        {previousX,previousY},{x,y},{unitX.X-origin.X,unitX.Y-origin.Y},{unitY.X-origin.X,unitY.Y-origin.Y});
    x=safe.x; y=safe.y;
}

void InspectStudioNative(wux::FrameworkElement const& e, int depth) {
    auto name = e.Name();
    if (auto text=e.try_as<wuxc::TextBlock>()) {
        if (name==L"Time" && !studio.clockFont.empty())
            StudioSet(text,wuxc::TextBlock::FontFamilyProperty(),wuxm::FontFamily(studio.clockFont));
        if (name==L"Date" && !studio.dateFont.empty())
            StudioSet(text,wuxc::TextBlock::FontFamilyProperty(),wuxm::FontFamily(studio.dateFont));
    }
    if (studio.diagnostics && !name.empty()) {
        std::wstring key = std::wstring(name) + L" (" + std::wstring(winrt::get_class_name(e)) + L")";
        if (std::find(studioLoggedNames.begin(), studioLoggedNames.end(), key) == studioLoggedNames.end()) {
            studioLoggedNames.push_back(key);
            Wh_Log(L"Studio tree [%d]: %s", depth, key.c_str());
        }
    }
    // Match one outer container per feature to avoid applying offsets twice.
    bool widget = studio.widgetMode!=L"studio" && studioWidgetMotion && e == studioWidgetMotion;
    bool insideWidget = StudioInsideWidget(e);
    bool insideMedia = StudioInsideMedia(e);
    bool media = StudioMediaName(name) && !insideMedia;
    bool skinMedia = StudioCardSkinName(studio.mediaTheme);
    if ((skinMedia || !studio.mediaFont.empty()) && (media || insideMedia)) {
        if (skinMedia) StyleStudioCardChrome(e,false);
        if (auto text = e.try_as<wuxc::TextBlock>()) {
            StyleStudioCardText(text,false);
        }
    }
    if (studio.widgetMode!=L"studio" && insideWidget && (StudioCardSkinName(studio.widgetTheme) || !studio.widgetFont.empty())) {
        if (StudioCardSkinName(studio.widgetTheme)) StyleStudioCardChrome(e,true);
        if (auto text = e.try_as<wuxc::TextBlock>()) StyleStudioCardText(text,true);
    }
    if (studio.widgetMode!=L"studio" && !insideWidget && StudioCardSkinName(studio.widgetTheme) && StudioWidgetWrapper(e))
        StyleStudioCardChrome(e,true);
    if (media && !studioMediaLogged) {
        Wh_Log(L"Studio media: target=%s, theme=%s, offset=(%d,%d)",
               name.c_str(), studio.mediaTheme.c_str(), studio.mediaX, studio.mediaY);
        studioMediaLogged = true;
    }
    StudioRule const* rule = nullptr;
    // Protect layout hosts from accidentally collapsing the entire screen.
    if (name != L"LockRootGrid" && name != L"MainPageContainer")
        for (auto const& candidate : studioRules)
            if (candidate.name == name) { rule = &candidate; break; }
    if (!widget && !media && !rule) return;
    double x = widget ? studio.widgetX : studio.mediaX;
    double y = widget ? studio.widgetY : studio.mediaY;
    if (rule) { x = rule->x; y = rule->y; }
    if (widget && !rule) ClampStudioWidgetOffset(x,y);
    if (x || y || widget) {
        auto prop = wux::UIElement::RenderTransformProperty();
        StudioSave(e, prop);
        wuxm::Transform original{nullptr};
        for (auto const& item : studioNative)
            if (item.element == e && item.property == prop && item.value != wux::DependencyProperty::UnsetValue())
                original = item.value.try_as<wuxm::Transform>();
        wuxm::TransformGroup group;
        if (original) group.Children().Append(original);
        wuxm::TranslateTransform t; t.X(x); t.Y(y);
        group.Children().Append(t);
        bool unchanged = false;
        if (auto current = e.RenderTransform().try_as<wuxm::TransformGroup>()) {
            auto children = current.Children();
            if (children.Size() == (original ? 2u : 1u)) {
                auto last = children.GetAt(children.Size() - 1).try_as<wuxm::TranslateTransform>();
                unchanged = last && last.X() == x && last.Y() == y &&
                    (!original || children.GetAt(0) == original);
            }
        }
        if (!unchanged) e.RenderTransform(group);
    }
    if ((media && studio.hideMedia) || (rule && rule->hide)) {
        StudioSave(e, wux::UIElement::VisibilityProperty());
        e.Visibility(wux::Visibility::Collapsed);
    }
    if (rule) {
        StudioSave(e, wux::UIElement::OpacityProperty());
        e.Opacity(rule->opacity / 100.0);
        if (auto text = e.try_as<wuxc::TextBlock>()) {
            if (rule->fontSize > 0) {
                StudioSave(e, wuxc::TextBlock::FontSizeProperty());
                text.FontSize(rule->fontSize);
            }
            if (!rule->color.empty()) {
                StudioSave(e, wuxc::TextBlock::ForegroundProperty());
                text.Foreground(wuxm::SolidColorBrush(StudioColor(rule->color, 0xFFFFFFFF)));
            }
        }
    }
}
void WalkStudioNative(wux::DependencyObject const& node, int depth) {
    if (!node || depth > 96 || (g_activeClockElement && node == g_activeClockElement)) return;
    if (auto e = node.try_as<wux::FrameworkElement>()) {
        if (e.Name() == L"WindhawkStudioCardPixelLayer" || e.Name()==L"WindhawkStudioWidgets") return;
        try { InspectStudioNative(e, depth); }
        catch (winrt::hresult_error const& error) {
            if (studio.diagnostics) Wh_Log(L"Studio native element %s failed: %s", e.Name().c_str(), error.message().c_str());
        }
    }
    int count = wuxm::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i) try {
        WalkStudioNative(wuxm::VisualTreeHelper::GetChild(node, i), depth + 1);
    } catch (...) {}
}
// Root-level cards mirror the native widget text without reparenting Windows'
// controls. Native bindings remain active; no external weather API is called.
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
struct StudioWidgetSnapshot {
    std::vector<std::wstring> lines;
    std::vector<std::wstring> conditionLabels;
    wuxm::ImageSource image{nullptr};
    double imageArea=0;
};
struct StudioHiddenWidget {
    wux::FrameworkElement source{nullptr};
    winrt::Windows::Foundation::IInspectable opacity{nullptr},hitTest{nullptr};
};
bool StudioTemperature(std::wstring const& value) {
    bool number=false;
    for(wchar_t c:value) if(c>=L'0' && c<=L'9') number=true;
    return number && value.find(L'\x00B0')!=std::wstring::npos;
}
void NormalizeStudioWidgetLines(std::vector<std::wstring>& lines) {
    std::vector<std::wstring> normalized;
    for(auto const& line:lines) {
        bool unit=line==L"\x00B0" L"C" || line==L"\x00B0" L"F" || line==L"\x00B0";
        bool numeric=!normalized.empty() && !normalized.back().empty();
        if (numeric) for(wchar_t c:normalized.back())
            numeric=numeric && ((c>=L'0' && c<=L'9') || c==L'-' || c==L'+' || c==L'.' || c==L'\x2212');
        if (unit && numeric) normalized.back()+=L" "+line;
        else normalized.push_back(line);
    }
    lines=std::move(normalized);
}
[[clang::no_destroy]] StudioHiddenWidget studioHiddenWidget;
[[clang::no_destroy]] wuxc::Grid studioWidgetsRoot{nullptr};
[[clang::no_destroy]] wuxc::Grid studioWidgetsParent{nullptr};
[[clang::no_destroy]] wuxc::Canvas studioWidgetsHost{nullptr};
std::wstring studioWidgetsSignature;
double studioWidgetsNaturalWidth=0,studioWidgetsNaturalHeight=0;
void RestoreStudioWidgetSource() {
    if (!studioHiddenWidget.source) return;
    auto restore=[](wux::FrameworkElement const& e,wux::DependencyProperty const& property,
                    winrt::Windows::Foundation::IInspectable const& value) {
        if (value==wux::DependencyProperty::UnsetValue()) e.ClearValue(property);
        else e.SetValue(property,value);
    };
    try { restore(studioHiddenWidget.source,wux::UIElement::OpacityProperty(),studioHiddenWidget.opacity); } catch (...) {}
    try { restore(studioHiddenWidget.source,wux::UIElement::IsHitTestVisibleProperty(),studioHiddenWidget.hitTest); } catch (...) {}
    studioHiddenWidget.source=nullptr;
    studioHiddenWidget.opacity=nullptr;
    studioHiddenWidget.hitTest=nullptr;
}
void HideStudioWidgetSource() {
    if (!studioWidget) return;
    if (studioHiddenWidget.source!=studioWidget) {
        RestoreStudioWidgetSource();
        studioHiddenWidget.opacity=studioWidget.ReadLocalValue(wux::UIElement::OpacityProperty());
        studioHiddenWidget.hitTest=studioWidget.ReadLocalValue(wux::UIElement::IsHitTestVisibleProperty());
        studioHiddenWidget.source=studioWidget;
    }
    if (studioWidget.Opacity()!=0) studioWidget.Opacity(0);
    if (studioWidget.IsHitTestVisible()) studioWidget.IsHitTestVisible(false);
}
void ResetStudioWidgets() {
    RestoreStudioWidgetSource();
    if (studioWidgetsRoot && studioWidgetsParent) try {
        uint32_t index;
        if (studioWidgetsParent.Children().IndexOf(studioWidgetsRoot,index))
            studioWidgetsParent.Children().RemoveAt(index);
    } catch (...) {}
    studioWidgetsHost=nullptr;
    studioWidgetsRoot=nullptr;
    studioWidgetsParent=nullptr;
    studioWidgetsSignature.clear();
    studioWidgetsNaturalWidth=studioWidgetsNaturalHeight=0;
}
void ReadStudioWidgetSnapshot(wux::DependencyObject const& node,StudioWidgetSnapshot& snapshot,int depth,int& budget) {
    if (!node || depth>96 || --budget<0) return;
    if (auto e=node.try_as<wux::FrameworkElement>()) {
        if (e.Name()==L"WindhawkStudioCardPixelLayer" || e.Visibility()!=wux::Visibility::Visible) return;
        auto add=[&](winrt::hstring const& label) {
            std::wstring value(label);
            if(!value.empty() && value.size()<=512 && snapshot.conditionLabels.size()<128 &&
               std::find(snapshot.conditionLabels.begin(),snapshot.conditionLabels.end(),value)==snapshot.conditionLabels.end())
                snapshot.conditionLabels.push_back(value);
        };
        try {
            add(wux::Automation::AutomationProperties::GetName(e));
            add(wux::Automation::AutomationProperties::GetHelpText(e));
            // Weather artwork commonly exposes its condition only as an
            // accessibility name, rather than a visible TextBlock.
            if(e.try_as<wuxc::Image>() || e.try_as<wuxc::IconElement>()) {
                auto peer=wux::Automation::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(e);
                if(peer) { add(peer.GetName()); add(peer.GetHelpText()); }
            }
        } catch (...) {}
    }
    if (auto text=node.try_as<wuxc::TextBlock>()) {
        auto family=std::wstring(text.FontFamily().Source());
        auto value=std::wstring(text.Text());
        bool glyph=family.find(L"Icons")!=std::wstring::npos || family.find(L"MDL2")!=std::wstring::npos;
        value.erase(0,value.find_first_not_of(L" \t\r\n"));
        auto last=value.find_last_not_of(L" \t\r\n");
        if (last!=std::wstring::npos) value.erase(last+1);
        if (!glyph && !value.empty() && value.size()<=256 && value!=L"\u2026" && value!=L"...") {
            bool privateGlyph=false;
            for(wchar_t c:value) if(c>=0xE000 && c<=0xF8FF) privateGlyph=true;
            if (!privateGlyph && (snapshot.lines.empty() || snapshot.lines.back()!=value)) snapshot.lines.push_back(value);
        }
    }
    if (auto image=node.try_as<wuxc::Image>()) {
        double area=image.ActualWidth()*image.ActualHeight();
        if (image.Source() && image.ActualWidth()>=24 && image.ActualHeight()>=24 && area>snapshot.imageArea) {
            snapshot.image=image.Source(); snapshot.imageArea=area;
        }
    }
    int count=wuxm::VisualTreeHelper::GetChildrenCount(node);
    for(int i=0;i<count && budget>0;++i) try {
        ReadStudioWidgetSnapshot(wuxm::VisualTreeHelper::GetChild(node,i),snapshot,depth+1,budget);
    } catch (...) {}
}
wux::UIElement StudioWidgetLabel(std::wstring const& value,double size,wu::Color color,double width,bool bold=false) {
    if (studio.widgetTheme==L"pixel" && studio.widgetFont.empty() && value.size()<=40) {
        wuxc::Grid pixel;
        RenderStudioPixel(pixel,value,size,color,color,false,0);
        double scale=(std::min)(1.0,width/pixel.Width());
        wuxc::Canvas box;
        box.Width(pixel.Width()*scale); box.Height(pixel.Height()*scale);
        wuxm::ScaleTransform transform; transform.ScaleX(scale); transform.ScaleY(scale);
        pixel.RenderTransform(transform);
        box.Children().Append(pixel);
        return box;
    }
    wuxc::TextBlock label;
    label.Text(value); label.FontSize(size);
    label.Foreground(wuxm::SolidColorBrush(color));
    label.FontFamily(wuxm::FontFamily(studio.widgetFont.empty() ? L"Segoe UI Variable Text" : studio.widgetFont));
    if (bold) label.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    label.TextWrapping(wux::TextWrapping::Wrap);
    label.MaxWidth(width);
    return label;
}
// Original vector artwork: no icon font, image download or weather service.
// Unknown/localized conditions use a neutral thermometer, never guessed weather.
#include <winrt/Windows.UI.Xaml.Automation.h>
std::wstring StudioWeatherCondition(StudioWidgetSnapshot const& snapshot) {
    if(!studio.weatherIconCondition.empty() && studio.weatherIconCondition!=L"auto")
        return studio.weatherIconCondition;
    std::wstring text;
    auto labels=snapshot.conditionLabels;
    labels.insert(labels.end(),snapshot.lines.begin(),snapshot.lines.end());
    for (auto line:labels) {
        for (auto& c:line) c=static_cast<wchar_t>(towlower(c));
        if(line.find(L"warning")!=std::wstring::npos || line.find(L"alert")!=std::wstring::npos ||
           line.find(L"advisory")!=std::wstring::npos || line.find(L"forecast")!=std::wstring::npos) continue;
        text+=line; text+=L" ";
    }
    auto has=[&](wchar_t const* value) { return text.find(value)!=std::wstring::npos; };
    if(has(L"thunder") || has(L"lightning")) return L"storm";
    if(has(L"sleet") || has(L"freezing rain") || has(L"wintry mix")) return L"sleet";
    if(has(L"snow") || has(L"blizzard") || has(L"flurr")) return L"snow";
    if(has(L"rain") || has(L"drizzle") || has(L"shower")) return L"rain";
    if(has(L"fog") || has(L"mist") || has(L"haze")) return L"fog";
    bool night=has(L"night") || has(L"moon");
    if(has(L"partly") || has(L"mostly sunny") || has(L"mostly clear")) return night ? L"partlyNight" : L"partly";
    if(has(L"cloud") || has(L"overcast")) return L"cloud";
    if(night) return L"night";
    if(has(L"sunny") || has(L"clear")) return L"sun";
    if(has(L"wind") || has(L"breezy")) return L"wind";
    return L"unknown";
}
wux::UIElement BuildStudioWeatherIcon(StudioWidgetSnapshot const& snapshot,
                                      std::wstring const& theme,wu::Color accent,wu::Color foreground) {
    auto condition=StudioWeatherCondition(snapshot);
    bool pixel=theme==L"pixel",outline=theme==L"minimal";
    // Each theme has its own finish: square pixel art, fine outline, two-tone
    // Nordic, pastel Catppuccin, warm Gruvbox, or layered macOS-style color.
    auto hex=[](wu::Color c) {
        wchar_t value[10]; swprintf_s(value,L"#%02X%02X%02X%02X",c.A,c.R,c.G,c.B);
        return std::wstring(value);
    };
    auto warm=StudioColor(L"",theme==L"catppuccin" ? 0xFFF9E2AF :
        theme==L"gruvbox" ? 0xFFFABD2F : theme==L"nord" ? 0xFFEBCB8B :
        theme==L"pixel" ? 0xFFFFCF7D : 0xFFFFBD59);
    auto cool=StudioColor(L"",theme==L"catppuccin" ? 0xFF89B4FA :
        theme==L"gruvbox" ? 0xFF83A598 : theme==L"nord" ? 0xFF81A1C1 : 0xFF70BFFF);
    std::wstring xaml=L"<Canvas xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation' Width='72' Height='72'>";
    auto path=[&](wchar_t const* data,wu::Color color,bool fill=true) {
        xaml+=L"<Path Data='"+std::wstring(data)+L"' Stroke='"+hex(color)+L"' StrokeThickness='"+
            std::wstring(pixel ? L"2" : outline ? L"1.5" : L"2")+L"' Fill='"+
            std::wstring(fill && !outline ? hex(color) : L"Transparent")+L"'/>";
    };
    bool cloudy=condition==L"cloud" || condition==L"partly" || condition==L"partlyNight" || condition==L"rain" ||
        condition==L"storm" || condition==L"snow" || condition==L"sleet";
    if(condition==L"sun" || condition==L"partly") {
        path(pixel ? L"M22 10H38V14H42V30H38V34H22V30H18V14H22Z" :
            L"M42 22A12 12 0 1 1 18 22A12 12 0 1 1 42 22Z",warm);
        path(L"M30 2V6M30 38V42M10 22H14M46 22H50M15 7L18 10M42 34L45 37M15 37L18 34M42 10L45 7",warm,false);
    }
    if(condition==L"night" || condition==L"partlyNight") path(pixel ? L"M36 8H24V14H18V36H24V42H48V36H54V30H36V24H30V14H36Z" :
        L"M38 8C19 5 12 27 24 39C35 49 51 43 56 31C39 36 27 23 38 8Z",cool);
    if(cloudy) {
        path(pixel ? L"M10 34H16V24H22V18H36V24H42V28H56V34H62V46H56V50H10V46H6V38H10Z" :
            L"M16 48C2 48 3 30 17 30C17 12 42 12 44 29C64 25 70 48 54 48Z",foreground);
        if(theme==L"macos" || theme==L"macosDark" || theme==L"smoke")
            path(L"M17 44H53",accent,false);
    }
    if(condition==L"rain" || condition==L"sleet")
        path(pixel ? L"M18 54V62H22V54ZM34 54V66H38V54ZM50 54V62H54V54Z" :
            L"M22 55L18 63M38 55L34 65M54 55L50 63",cool,pixel);
    if(condition==L"snow" || condition==L"sleet")
        path(L"M23 55V67M17 61H29M19 57L27 65M19 65L27 57M48 55V67M42 61H54",accent,false);
    if(condition==L"storm") path(pixel ? L"M34 48H46L38 56H46L28 70L34 58H28Z" :
        L"M35 48H46L38 57H45L28 70L33 59H27Z",warm);
    if(condition==L"storm") path(pixel ? L"M14 54H18V62H14ZM52 54H56V62H52Z" :
        L"M17 54C9 64 22 66 17 54ZM54 54C46 64 59 66 54 54Z",cool);
    if(condition==L"fog" || condition==L"wind")
        path(pixel ? L"M10 22H50V26H10ZM18 36H62V40H18ZM8 50H48V54H8Z" :
            condition==L"wind" ? L"M9 25H45C60 25 56 10 48 15M17 37H58M8 49H38C53 49 51 63 43 59" :
            L"M14 24H53M8 36H63M18 48H55",accent,pixel);
    if(condition==L"unknown") {
        path(pixel ? L"M28 12H40V44H46V62H40V66H28V62H22V44H28Z" :
            L"M29 43V17C29 7 41 7 41 17V43C54 50 49 66 35 66C21 66 16 50 29 43Z",foreground,false);
        path(L"M35 24V53M30 55H40",accent,false);
    }
    xaml+=L"</Canvas>";
    auto icon=wux::Markup::XamlReader::Load(xaml).as<wuxc::Canvas>();
    wux::Automation::AutomationProperties::SetName(icon,
        condition==L"unknown" ? L"Weather" : L"Weather: "+condition);
    return icon;
}

wux::FrameworkElement BuildStudioWidgetCard(StudioWidgetSnapshot const& snapshot) {
    auto theme=StudioCardSkinName(studio.widgetTheme) ? studio.widgetTheme : L"smoke";
    auto palette=StudioCardColors(theme);
    bool custom=studio.widgetCustomStyle;
    auto fg=StudioColor(custom ? studio.widgetForeground : L"",palette.foreground);
    auto accent=StudioColor(custom ? studio.widgetAccent : L"",palette.accent);
    auto bg=StudioColor(custom ? studio.widgetBackground : L"",palette.background);
    bg.A=static_cast<uint8_t>(bg.A*(custom ? studio.widgetOpacity : theme==L"smoke" ? 78 : 100)/100);
    if (theme==L"minimal" && (!custom || studio.widgetBackground.empty())) bg.A=0;
    double radius=custom ? studio.widgetRadius :
        theme==L"macos" || theme==L"macosDark" ? 24 : theme==L"catppuccin" ? 20 :
        theme==L"nord" ? 8 : theme==L"gruvbox" ? 4 : theme==L"minimal" ? 12 : 18;
    auto borderColor=accent;
    double borderWidth=1;
    if(theme==L"macos" || theme==L"macosDark") {
        borderColor=StudioColor(L"",theme==L"macos" ? 0xFFD2D2D7 : 0xFF38383A);
        borderWidth=0.5;
    } else if(theme==L"nord") borderColor=StudioColor(L"",0xFF4C566A);
    else if(theme==L"catppuccin") borderColor=StudioColor(L"",0xFF45475A);
    else if(theme==L"gruvbox") borderColor=StudioColor(L"",0xFF928374);
    if(custom && !studio.widgetAccent.empty()) borderColor=accent;
    wuxc::Border card;
    card.Width(studio.widgetCardWidth);
    card.Padding(wux::ThicknessHelper::FromUniformLength(18));
    card.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(theme==L"pixel" ? 0 : radius));
    card.BorderThickness(wux::ThicknessHelper::FromUniformLength(theme==L"pixel" ? 0 : borderWidth));
    card.BorderBrush(wuxm::SolidColorBrush(borderColor));
    card.Background(wuxm::SolidColorBrush(bg));
    wuxc::StackPanel body; body.Spacing(8);
    double width=studio.widgetCardWidth-36;
    size_t temperature=snapshot.lines.size();
    for(size_t i=0;i<snapshot.lines.size();++i) if(StudioTemperature(snapshot.lines[i])) {
        temperature=i; break;
    }
    if(temperature<snapshot.lines.size() && studio.weatherIconVisible &&
       (studio.weatherIconPosition==L"left" || studio.weatherIconPosition==L"right"))
        width=(std::max)(40.0,width-(std::min)(double(studio.weatherIconSize),width-52)-12);
    size_t titleIndex=snapshot.lines.size();
    for(size_t i=0;i<snapshot.lines.size();++i) if(i!=temperature) { titleIndex=i; break; }
    auto title=titleIndex<snapshot.lines.size() ? snapshot.lines[titleIndex] :
        std::wstring(temperature<snapshot.lines.size() ? L"Weather" : L"Windows widget");
    auto titleColor=theme==L"macos" || theme==L"macosDark" ? fg : accent;
    body.Children().Append(StudioWidgetLabel(title,studio.widgetTextSize ? studio.widgetTextSize : 18,titleColor,width,true));
    bool weather=temperature<snapshot.lines.size();
    if (!weather && snapshot.image) {
        wuxc::Image icon; icon.Source(snapshot.image); icon.Width(64); icon.Height(64);
        icon.Stretch(wuxm::Stretch::Uniform);
        icon.HorizontalAlignment(wux::HorizontalAlignment::Left);
        body.Children().Append(icon);
    }
    // Preserve values verbatim. No guessed weather condition or invented data.
    size_t shown=0;
    for(size_t i=0;i<snapshot.lines.size() && shown<7;++i) {
        if(i==titleIndex) continue;
        auto value=snapshot.lines[i];
        if (value==L"See full forecast" || value==L"See more" || value==L"Learn more") continue;
        bool numeric=!value.empty();
        for(wchar_t c:value) numeric=numeric && ((c>=L'0' && c<=L'9') || c==L'-' || c==L'+' || c==L'.');
        if (numeric && i+1<snapshot.lines.size() && snapshot.lines[i+1].find(L'\x00B0')!=std::wstring::npos) {
            value+=L" "+snapshot.lines[++i];
            temperature=i;
        }
        double size=i==temperature ? 42 : (studio.widgetTextSize ? studio.widgetTextSize : 16);
        body.Children().Append(StudioWidgetLabel(value,size,fg,width,i==temperature));
        ++shown;
    }
    if(weather && studio.weatherIconVisible) {
        auto condition=StudioWeatherCondition(snapshot);
        wux::UIElement artwork{nullptr};
        if(condition==L"unknown" && snapshot.image) {
            wuxc::Image native; native.Source(snapshot.image); native.Width(72); native.Height(72);
            native.Stretch(wuxm::Stretch::Uniform); artwork=native;
        } else artwork=BuildStudioWeatherIcon(snapshot,theme,accent,fg);
        wuxc::Canvas icon;
        double size=studio.weatherIconSize;
        if(studio.weatherIconPosition==L"left" || studio.weatherIconPosition==L"right")
            size=(std::min)(size,double(studio.widgetCardWidth-36-52));
        else size=(std::min)(size,double(studio.widgetCardWidth-36));
        icon.Width(size); icon.Height(size);
        wuxm::ScaleTransform scale; scale.ScaleX(size/72); scale.ScaleY(size/72);
        artwork.RenderTransform(scale); icon.Children().Append(artwork);
        wuxm::TranslateTransform offset; offset.X(studio.weatherIconX); offset.Y(studio.weatherIconY);
        icon.RenderTransform(offset);
        auto position=studio.weatherIconPosition;
        bool side=position==L"left" || position==L"right";
        bool bottom=position.find(L"bottom")==0;
        bool right=position==L"right" || position==L"topRight" || position==L"bottomRight";
        icon.HorizontalAlignment(right ? wux::HorizontalAlignment::Right :
            position==L"top" || position==L"bottom" ? wux::HorizontalAlignment::Center : wux::HorizontalAlignment::Left);
        icon.VerticalAlignment(side ? wux::VerticalAlignment::Center : wux::VerticalAlignment::Top);
        wuxc::Grid layout;
        if(side) {
            wuxc::ColumnDefinition a,b;
            a.Width({right ? 1.0 : size+12,right ? wux::GridUnitType::Star : wux::GridUnitType::Pixel});
            b.Width({right ? size+12 : 1.0,right ? wux::GridUnitType::Pixel : wux::GridUnitType::Star});
            layout.ColumnDefinitions().Append(a); layout.ColumnDefinitions().Append(b);
            wuxc::Grid::SetColumn(icon,right ? 1 : 0); wuxc::Grid::SetColumn(body,right ? 0 : 1);
            double textWidth=width;
            body.Width(textWidth);
            // Re-measure wrapping and pixel labels in the remaining text column.
            for(auto const& child:body.Children()) if(auto label=child.try_as<wuxc::TextBlock>()) label.MaxWidth(textWidth);
        } else {
            wuxc::RowDefinition a,b;
            a.Height({1,wux::GridUnitType::Auto}); b.Height({1,wux::GridUnitType::Auto});
            layout.RowDefinitions().Append(a); layout.RowDefinitions().Append(b);
            wuxc::Grid::SetRow(icon,bottom ? 1 : 0); wuxc::Grid::SetRow(body,bottom ? 0 : 1);
            icon.Margin(wux::ThicknessHelper::FromLengths(0,bottom ? 12 : 0,0,bottom ? 0 : 12));
        }
        layout.Children().Append(body); layout.Children().Append(icon); card.Child(layout);
    } else card.Child(body);
    if(theme==L"pixel") {
        // One stepped silhouette paints both fill and outline. The content
        // Border is transparent, so square Windows/theme outlines cannot stack.
        card.Background(wuxm::SolidColorBrush(wu::Colors::Transparent()));
        card.Measure({static_cast<float>(studio.widgetCardWidth),100000});
        double w=studio.widgetCardWidth,h=card.DesiredSize().Height;
        auto n=[](double v) { return std::to_wstring(v); };
        auto hex=[](wu::Color c) { wchar_t s[10]; swprintf_s(s,L"#%02X%02X%02X%02X",c.A,c.R,c.G,c.B); return std::wstring(s); };
        auto data=L"M9,1 H"+n(w-9)+L" V5 H"+n(w-5)+L" V9 H"+n(w-1)+
            L" V"+n(h-9)+L" H"+n(w-5)+L" V"+n(h-5)+L" H"+n(w-9)+L" V"+n(h-1)+
            L" H9 V"+n(h-5)+L" H5 V"+n(h-9)+L" H1 V9 H5 V5 H9 Z";
        auto xml=L"<Path xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation' Data='"+
            data+L"' Fill='"+hex(bg)+L"' Stroke='"+hex(accent)+L"' StrokeThickness='2'/>";
        wuxc::Grid frame; frame.Width(w); frame.Height(h);
        frame.Children().Append(wux::Markup::XamlReader::Load(xml).as<wux::UIElement>());
        frame.Children().Append(card);
        return frame;
    }
    return card;
}
void PlaceStudioWidgets() {
    if (!studioWidgetsHost || !studioWidgetsRoot || !studioWidgetsParent) return;
    double w=studioWidgetsParent.ActualWidth(),h=studioWidgetsParent.ActualHeight();
    if (w<=0 || h<=0) return;
    double padding=studio.widgetScreenPadding;
    double scale=(std::min)({1.0,(std::max)(1.0,w-2*padding)/studioWidgetsNaturalWidth,
        (std::max)(1.0,h-2*padding)/studioWidgetsNaturalHeight});
    double width=studioWidgetsNaturalWidth*scale,height=studioWidgetsNaturalHeight*scale;
    studioWidgetsHost.Width(width); studioWidgetsHost.Height(height);
    auto row=studioWidgetsHost.Children().GetAt(0).as<wuxc::StackPanel>();
    auto previous=row.RenderTransform().try_as<wuxm::ScaleTransform>();
    if (!previous || previous.ScaleX()!=scale) {
        wuxm::ScaleTransform transform; transform.ScaleX(scale); transform.ScaleY(scale);
        row.RenderTransform(transform);
    }
    auto anchor=studio.widgetAnchor.empty() ? std::wstring(L"bottom") : studio.widgetAnchor;
    bool left=anchor==L"left" || anchor==L"topLeft" || anchor==L"bottomLeft";
    bool right=anchor==L"right" || anchor==L"topRight" || anchor==L"bottomRight";
    bool top=anchor==L"top" || anchor==L"topLeft" || anchor==L"topRight";
    bool bottom=anchor==L"bottom" || anchor==L"bottomLeft" || anchor==L"bottomRight";
    double baseX=left ? 64 : right ? w-width-64 : (w-width)/2;
    double baseY=top ? 64 : bottom ? h-height-64 : (h-height)/2;
    double x=studio.widgetX,y=studio.widgetY;
    if (studio.widgetKeepOnScreen) {
        x=MitsuPlacement::ClampOffset(baseX,width,w,padding,x);
        y=MitsuPlacement::ClampOffset(baseY,height,h,padding,y);
    }
    // Canvas positioning directly in the root bypasses the native widget slot.
    studioWidgetsHost.HorizontalAlignment(wux::HorizontalAlignment::Left);
    studioWidgetsHost.VerticalAlignment(wux::VerticalAlignment::Top);
    auto old=studioWidgetsHost.RenderTransform().try_as<wuxm::TranslateTransform>();
    if (!old || old.X()!=baseX+x || old.Y()!=baseY+y) {
        wuxm::TranslateTransform transform; transform.X(baseX+x); transform.Y(baseY+y);
        studioWidgetsHost.RenderTransform(transform);
    }
}
void UpdateStudioWidgets() {
    if (studio.widgetMode!=L"studio" || studio.widgetTheme==L"stock") {
        if(studioWidgetsRoot) ResetStudioWidgets(); return;
    }
    if (!studioWidget || !g_lockRootGrid) return;
    std::vector<StudioWidgetSnapshot> snapshots;
    std::wstring signature;
    for(auto const& frame:StudioWidgetFrames()) {
        StudioWidgetSnapshot snapshot;
        int budget=4096;
        ReadStudioWidgetSnapshot(frame,snapshot,0,budget);
        if (snapshot.lines.empty()) continue;
        NormalizeStudioWidgetLines(snapshot.lines);
        signature+=L"[card]";
        for(auto const& line:snapshot.lines) signature+=std::to_wstring(line.size())+L":"+line;
        for(auto const& label:snapshot.conditionLabels) signature+=L"[condition]"+std::to_wstring(label.size())+L":"+label;
        signature+=std::to_wstring(reinterpret_cast<uintptr_t>(winrt::get_abi(snapshot.image)));
        snapshots.push_back(std::move(snapshot));
        if (snapshots.size()==4) break;
    }
    if (snapshots.empty()) { ResetStudioWidgets(); return; }
    bool attached=false;
    if (studioWidgetsRoot && studioWidgetsParent==g_lockRootGrid) {
        uint32_t index; attached=g_lockRootGrid.Children().IndexOf(studioWidgetsRoot,index);
    }
    if (!attached || signature!=studioWidgetsSignature) {
        if(studio.diagnostics) for(auto const& snapshot:snapshots)
            Wh_Log(L"Studio weather detection: %s; %u accessibility labels. Unknown means no recognized condition.",
                StudioWeatherCondition(snapshot).c_str(),static_cast<unsigned>(snapshot.conditionLabels.size()));
        // Build the replacement before hiding the original. A failed build
        // leaves the Windows card available instead of producing an empty UI.
        wuxc::Grid root;
        root.Name(L"WindhawkStudioWidgets"); root.IsHitTestVisible(false);
        wuxc::Grid::SetRowSpan(root,100); wuxc::Grid::SetColumnSpan(root,100);
        wuxc::Canvas::SetZIndex(root,110);
        wuxc::StackPanel row; row.Orientation(wuxc::Orientation::Horizontal); row.Spacing(16);
        row.HorizontalAlignment(wux::HorizontalAlignment::Left);
        row.VerticalAlignment(wux::VerticalAlignment::Top);
        for(auto const& snapshot:snapshots) row.Children().Append(BuildStudioWidgetCard(snapshot));
        row.Measure({100000,100000});
        auto desired=row.DesiredSize();
        if (desired.Width<=0 || desired.Height<=0) return;
        row.Width(desired.Width); row.Height(desired.Height);
        wuxc::Canvas host; host.Children().Append(row); root.Children().Append(host);
        ResetStudioWidgets();
        g_lockRootGrid.Children().Append(root);
        studioWidgetsRoot=root; studioWidgetsHost=host; studioWidgetsParent=g_lockRootGrid;
        studioWidgetsNaturalWidth=desired.Width; studioWidgetsNaturalHeight=desired.Height;
        studioWidgetsSignature=signature;
        Wh_Log(L"Studio replacement widgets: %u card(s), independent root layout.",static_cast<unsigned>(snapshots.size()));
    }
    PlaceStudioWidgets();
    HideStudioWidgetSource();
}


// All work stays on LockApp's UI thread. Weak references cannot keep an old
// media tree alive and no event handlers can survive DLL unload.
[[clang::no_destroy]] winrt::weak_ref<wux::FrameworkElement> studioFastMedia;
ULONGLONG studioLastMediaRefresh = 0;
bool studioWasVisible = false;
ULONGLONG studioLastWidgetDiscovery = 0;
void ResetStudioMediaRefresh() {
    studioFastMedia = {};
    studioLastMediaRefresh = 0;
    studioWasVisible = false;
    studioLastWidgetDiscovery = 0;
}
wux::FrameworkElement FindStudioMedia(wux::DependencyObject const& node, int depth, int& budget) {
    if (!node || depth > 96 || --budget < 0 ||
        (g_activeClockElement && node == g_activeClockElement)) return nullptr;
    if (auto element = node.try_as<wux::FrameworkElement>())
        if (StudioMediaName(element.Name())) return element;
    int count = wuxm::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count && budget > 0; ++i) try {
        auto match = FindStudioMedia(wuxm::VisualTreeHelper::GetChild(node, i), depth + 1, budget);
        if (match) return match;
    } catch (...) {}
    return nullptr;
}
bool StudioMediaAttached(wux::DependencyObject node, wux::DependencyObject const& content) {
    for (int i = 0; node && i < 96; ++i) {
        if (node == content) return true;
        node = wuxm::VisualTreeHelper::GetParent(node);
    }
    return false;
}
void RefreshStudioMedia(ULONGLONG now) {
    auto window = wux::Window::Current();
    if (!window || !window.Visible()) {
        studioWasVisible = false;
        return;
    }
    bool justVisible = !studioWasVisible;
    studioWasVisible = true;
    if (!justVisible && now - studioLastMediaRefresh < 125) return;
    studioLastMediaRefresh = now;
    auto content = window.Content();
    if (!content) return;
    bool widgetsWanted = studio.widgetMode==L"studio" || StudioCardSkinName(studio.widgetTheme) || !studio.widgetFont.empty() || studio.widgetX || studio.widgetY;
    if (widgetsWanted) {
        bool attached = studioWidget && StudioMediaAttached(studioWidget,content);
        if (!attached || justVisible || now-studioLastWidgetDiscovery >= 500) {
            SelectStudioWidget(content);
            studioLastWidgetDiscovery = now;
        }
        if (studioWidget) {
            if (studio.widgetMode==L"studio") UpdateStudioWidgets();
            else {
                if (studioWidgetMotion && studioWidgetMotion != studioWidget) InspectStudioNative(studioWidgetMotion,0);
                WalkStudioNative(studioWidget,0);
            }
        }
    }
    bool themed = StudioCardSkinName(studio.mediaTheme);
    if (!themed && studio.mediaFont.empty() && !studio.hideMedia && !studio.mediaX && !studio.mediaY) return;
    auto media = studioFastMedia.get();
    if (!media || !StudioMediaAttached(media, content)) {
        int budget = 8192;
        media = FindStudioMedia(content, 0, budget);
        studioFastMedia = media ? winrt::make_weak(media) : winrt::weak_ref<wux::FrameworkElement>{};
    }
    if (media) {
        // Do not finish/prune a full native scan after a subtree-only pass:
        // widget and other rule targets must keep their saved original values.
        WalkStudioNative(media, 0);
    }
}

// Helper to apply root horizontal and vertical alignments (supports bottomLeft Sonoma style)
void ApplyAlignmentAndMargin(wuxc::Grid& rootContainer) {
    if (g_settings.clockAlignment == L"left") {
        rootContainer.HorizontalAlignment(wux::HorizontalAlignment::Left);
        rootContainer.VerticalAlignment(wux::VerticalAlignment::Center);
        rootContainer.Margin(wux::Thickness{ 80.0, (double)g_settings.verticalOffset, 0.0, 0.0 });
    } else if (g_settings.clockAlignment == L"right") {
        rootContainer.HorizontalAlignment(wux::HorizontalAlignment::Right);
        rootContainer.VerticalAlignment(wux::VerticalAlignment::Center);
        rootContainer.Margin(wux::Thickness{ 0.0, (double)g_settings.verticalOffset, 80.0, 0.0 });
    } else if (g_settings.clockAlignment == L"bottomLeft") {
        rootContainer.HorizontalAlignment(wux::HorizontalAlignment::Left);
        rootContainer.VerticalAlignment(wux::VerticalAlignment::Bottom);
        rootContainer.Margin(wux::Thickness{ 80.0, 0.0, 0.0, 80.0 + (double)g_settings.verticalOffset });
    } else {
        rootContainer.HorizontalAlignment(wux::HorizontalAlignment::Center);
        rootContainer.VerticalAlignment(wux::VerticalAlignment::Center);
        rootContainer.Margin(wux::Thickness{ 0.0, (double)g_settings.verticalOffset, 0.0, 0.0 });
    }
}

// Update single pixel digit dots (7x11 matrix) with dirty-checking
void SetPixelDigitValue(PixelDigitGrid& digitRef, int val) {
    if (val < 0 || val > 9 || digitRef.dots.size() != 77 || !g_brushActiveDot || !g_brushInactiveDot) {
        return;
    }
    if (digitRef.lastVal == val) {
        return; // Value unchanged; skip re-applying brushes to 77 Ellipses
    }
    digitRef.lastVal = val;

    const uint8_t* rows = FONT_7x11[val];
    for (int r = 0; r < 11; r++) {
        uint8_t rowBits = rows[r];
        for (int c = 0; c < 7; c++) {
            bool active = (rowBits & (1 << (6 - c))) != 0;
            int idx = r * 7 + c;
            digitRef.dots[idx].Fill(active ? g_brushActiveDot : g_brushInactiveDot);
        }
    }
}

// Update Time and Date with dirty-checking cache (eliminates 95%+ redundant DirectWrite passes)
void UpdateTimeAndDate() {
    if (IsStudioPixelStyle(g_settings.clockStyle) || (g_settings.clockStyle == L"pixel" && !studio.clockFont.empty())) { UpdateStudioPixelTime(); return; }
    SYSTEMTIME st;
    GetLocalTime(&st);

    bool is12h = (g_settings.timeFormat == L"12h");
    int rawHour = st.wHour;
    int displayHour = rawHour;
    bool isPM = (rawHour >= 12);
    const wchar_t* ampmStr = isPM ? L"PM" : L"AM";
    if (is12h) {
        displayHour = rawHour % 12;
        if (displayHour == 0) displayHour = 12;
    }

    bool hourChanged = (displayHour != g_timeCache.lastDisplayHour);
    bool minuteChanged = (st.wMinute != g_timeCache.lastMinute);
    bool secondChanged = (st.wSecond != g_timeCache.lastSecond);
    bool ampmChanged = (g_timeCache.lastAmPm != ampmStr);
    bool dateChanged = (st.wDay != g_timeCache.lastDay || st.wMonth != g_timeCache.lastMonth || st.wYear != g_timeCache.lastYear);

    if (g_ampmTextBlock && ampmChanged) {
        g_ampmTextBlock.Text(ampmStr);
    }
    // Update the cache even when the current style renders AM/PM inside its
    // main text. Otherwise "ampmChanged" stays true and defeats dirty checking.
    g_timeCache.lastAmPm = ampmStr;

    if (g_settings.clockStyle == L"pixel") {
        int h1 = displayHour / 10;
        int h2 = displayHour % 10;
        int m1 = st.wMinute / 10;
        int m2 = st.wMinute % 10;

        SetPixelDigitValue(g_pixelDigits[0], h1);
        SetPixelDigitValue(g_pixelDigits[1], h2);
        SetPixelDigitValue(g_pixelDigits[2], m1);
        SetPixelDigitValue(g_pixelDigits[3], m2);

        if (g_settings.showSeconds) {
            int s1 = st.wSecond / 10;
            int s2 = st.wSecond % 10;
            SetPixelDigitValue(g_pixelDigits[4], s1);
            SetPixelDigitValue(g_pixelDigits[5], s2);
        }
    } else if (g_settings.clockStyle == L"macos") {
        bool shouldUpdateTime = g_settings.showSeconds ? secondChanged : (minuteChanged || hourChanged || ampmChanged);
        if (g_macosTimeTextBlock && (shouldUpdateTime || g_timeCache.lastMacosTime.empty())) {
            WCHAR timeBuf[32];
            if (is12h) {
                if (g_settings.showSeconds) {
                    swprintf_s(timeBuf, L"%d:%02d:%02d %s", displayHour, st.wMinute, st.wSecond, ampmStr);
                } else {
                    swprintf_s(timeBuf, L"%d:%02d %s", displayHour, st.wMinute, ampmStr);
                }
            } else {
                if (g_settings.showSeconds) {
                    swprintf_s(timeBuf, L"%02d:%02d:%02d", displayHour, st.wMinute, st.wSecond);
                } else {
                    swprintf_s(timeBuf, L"%02d:%02d", displayHour, st.wMinute);
                }
            }
            g_macosTimeTextBlock.Text(timeBuf);
            if (g_macosShadowTextBlock) {
                g_macosShadowTextBlock.Text(timeBuf);
            }
            g_timeCache.lastMacosTime = timeBuf;
        }

        if (g_macosDateTextBlock && (dateChanged || g_timeCache.lastMacosDate.empty())) {
            WCHAR dateBuf[128];
            if (GetDateFormatW(LOCALE_USER_DEFAULT, 0, &st, L"dddd, d MMMM", dateBuf, ARRAYSIZE(dateBuf)) == 0) {
                GetDateFormatW(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, nullptr, dateBuf, ARRAYSIZE(dateBuf));
            }
            g_macosDateTextBlock.Text(dateBuf);
            if (g_macosDateShadowBlock) {
                g_macosDateShadowBlock.Text(dateBuf);
            }
            g_timeCache.lastMacosDate = dateBuf;
        }
    } else if (g_settings.clockStyle == L"aura" || IsStudioStyle(g_settings.clockStyle)) {
        bool shouldUpdateAura = g_settings.showSeconds ? secondChanged : (minuteChanged || hourChanged || ampmChanged);
        if (g_auraTimeTextBlock && (shouldUpdateAura || g_timeCache.lastAuraTime.empty())) {
            WCHAR timeBuf[32];
            if (is12h) {
                if (g_settings.showSeconds) {
                    swprintf_s(timeBuf, L"%d:%02d:%02d %s", displayHour, st.wMinute, st.wSecond, ampmStr);
                } else {
                    swprintf_s(timeBuf, L"%d:%02d %s", displayHour, st.wMinute, ampmStr);
                }
            } else {
                if (g_settings.showSeconds) {
                    swprintf_s(timeBuf, L"%02d:%02d:%02d", displayHour, st.wMinute, st.wSecond);
                } else {
                    swprintf_s(timeBuf, L"%02d:%02d", displayHour, st.wMinute);
                }
            }
            g_auraTimeTextBlock.Text(timeBuf);
            if (g_auraShadowTextBlock) {
                g_auraShadowTextBlock.Text(timeBuf);
            }
            g_timeCache.lastAuraTime = timeBuf;
        }
    } else {
        // Flip Clock, Retro Vintage, Cyberpunk HUD
        if (g_hourTextBlock && hourChanged) {
            WCHAR hourBuf[8];
            // Text-based 12-hour clocks omit the leading zero consistently.
            if (is12h) {
                swprintf_s(hourBuf, L"%d", displayHour);
            } else {
                swprintf_s(hourBuf, L"%02d", displayHour);
            }
            g_hourTextBlock.Text(hourBuf);
            if (g_hourBotTextBlock) {
                g_hourBotTextBlock.Text(hourBuf);
            }
        }

        if (g_minTextBlock && minuteChanged) {
            WCHAR minBuf[8];
            swprintf_s(minBuf, L"%02d", st.wMinute);
            g_minTextBlock.Text(minBuf);
            if (g_minBotTextBlock) {
                g_minBotTextBlock.Text(minBuf);
            }
        }

        if (g_secTextBlock && g_settings.showSeconds && secondChanged) {
            WCHAR secBuf[8];
            swprintf_s(secBuf, L"%02d", st.wSecond);
            g_secTextBlock.Text(secBuf);
        }

        // Cyberpunk HUD Tactical Separator Pulse
        if (g_settings.clockStyle == L"cyberpunk" && secondChanged) {
            bool pulseState = (st.wSecond % 2 == 0);
            if (g_cyberSlash1) {
                g_cyberSlash1.Opacity(pulseState ? 1.0 : 0.35);
            }
            if (g_cyberSlash2) {
                g_cyberSlash2.Opacity(pulseState ? 1.0 : 0.35);
            }
            if (g_cyberLiveLabel) {
                g_cyberLiveLabel.Opacity(pulseState ? 1.0 : 0.55);
            }
        }
    }

    // Update the date cache even when the current style has no date capsule.
    // Otherwise "dateChanged" stays true forever for the Sonoma style, which
    // would format and assign the date on every tick.
    if (dateChanged) {
        g_timeCache.lastDay = st.wDay;
        g_timeCache.lastMonth = st.wMonth;
        g_timeCache.lastYear = st.wYear;
    }

    if (g_dateTextBlock && g_settings.showDateCapsule && (dateChanged || g_timeCache.lastDateStr.empty())) {
        WCHAR dateBuf[128];
        if (GetDateFormatW(LOCALE_USER_DEFAULT, studio.dateFormat.empty() ? DATE_LONGDATE : 0, &st, studio.dateFormat.empty() ? nullptr : studio.dateFormat.c_str(), dateBuf, ARRAYSIZE(dateBuf)) > 0) {
            g_dateTextBlock.Text(dateBuf);
            g_timeCache.lastDateStr = dateBuf;
        }
    }

    // Battery percentage and power status update
    if (g_batteryStatusText && g_settings.showBatteryStatus && (minuteChanged || g_timeCache.lastBatteryPct == -1)) {
        auto bInfo = GetCurrentBatteryInfo();
        if (bInfo.hasBattery) {
            if (bInfo.percentage != g_timeCache.lastBatteryPct || bInfo.isCharging != g_timeCache.lastIsCharging) {
                WCHAR bBuf[32];
                if (g_settings.clockStyle == L"cyberpunk") {
                    swprintf_s(bBuf, bInfo.isCharging ? L"PWR: %d%% [CHG]" : L"BAT: %d%%", bInfo.percentage);
                } else {
                    swprintf_s(bBuf, L"%d%%", bInfo.percentage);
                }
                g_batteryStatusText.Text(bBuf);
                if (g_batteryIconText) {
                    g_batteryIconText.Text(bInfo.isCharging ? L"⚡ " : L"🔋 ");
                    g_batteryIconText.Foreground(bInfo.isCharging
                        ? wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 252, 238, 10))
                        : wuxm::SolidColorBrush(wu::Colors::White()));
                }
                g_timeCache.lastBatteryPct = bInfo.percentage;
                g_timeCache.lastIsCharging = bInfo.isCharging;
            }
        }
    }

    g_timeCache.lastDisplayHour = displayHour;
    g_timeCache.lastMinute = st.wMinute;
    g_timeCache.lastSecond = st.wSecond;
}

// Build Single Flip Card Element (Two physically split upper/lower flaps with clean scissors-cut gap)
wuxc::Grid CreateFlipCard(double width, double height, double cornerRadius, double fontSize,
                          wuxc::TextBlock& outTopText, wuxc::TextBlock& outBotText,
                          const wchar_t* textName, bool isHourCard = false) {
    wuxc::Grid card;
    card.Width(width);
    card.Height(height);

    double seamGap = 4.0;
    double halfHeight = (height - seamGap) / 2.0;

    // Optical centering offset for Segoe UI Variable ExtraBold: aligns the
    // digit to the split line so both halves are visually 50/50.
    double opticalOffset = fontSize * 0.045;

    // --- 1. TOP FLAP (upper half card & digits) ---
    wuxc::Grid topContainer;
    topContainer.Width(width);
    topContainer.Height(height);

    wuxm::RectangleGeometry topClip;
    topClip.Rect(winrt::Windows::Foundation::Rect{ 0.0f, 0.0f, (float)width, (float)halfHeight });
    topContainer.Clip(topClip);

    // Top flap acrylic body & border: rounded top corners, 2 px inner cut,
    // open bottom edge.
    wuxc::Border topFlap;
    topFlap.Width(width);
    topFlap.Height(halfHeight);
    topFlap.VerticalAlignment(wux::VerticalAlignment::Top);
    topFlap.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb((uint8_t)g_settings.glassOpacity, 255, 255, 255)));
    topFlap.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(80, 255, 255, 255)));
    topFlap.BorderThickness(wux::Thickness{ 1.5, 1.5, 1.5, 0.0 });
    topFlap.CornerRadius(wux::CornerRadiusHelper::FromRadii(cornerRadius, cornerRadius, 2.0, 2.0));
    topContainer.Children().Append(topFlap);

    // Top digit: centered in the card; the lower half is clipped at the split.
    wuxc::TextBlock topText;
    topText.Name(textName);
    topText.FontSize(fontSize);
    topText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    topText.FontWeight(winrt::Windows::UI::Text::FontWeights::ExtraBold());
    topText.Foreground(wuxm::SolidColorBrush(wu::Colors::White()));
    topText.HorizontalAlignment(wux::HorizontalAlignment::Center);
    topText.VerticalAlignment(wux::VerticalAlignment::Center);
    topText.Margin(wux::Thickness{ 0.0, opticalOffset, 0.0, 0.0 });
    wuxd::Typography::SetNumeralAlignment(topText, wux::FontNumeralAlignment::Tabular);
    topContainer.Children().Append(topText);

    // AM/PM badge on the clock card (12-hour format, top-left of the top flap).
    if (isHourCard && g_settings.timeFormat == L"12h") {
        wuxc::Border ampmBadge;
        ampmBadge.HorizontalAlignment(wux::HorizontalAlignment::Left);
        ampmBadge.VerticalAlignment(wux::VerticalAlignment::Top);
        ampmBadge.Margin(wux::Thickness{ 14.0, 12.0, 0.0, 0.0 });
        ampmBadge.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(45, 255, 255, 255)));
        ampmBadge.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(75, 255, 255, 255)));
        ampmBadge.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
        ampmBadge.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(6.0));
        ampmBadge.Padding(wux::Thickness{ 7.0, 2.0, 7.0, 2.0 });

        wuxc::TextBlock ampmText;
        ampmText.Name(L"WindhawkFlipAmPmText");
        ampmText.FontSize(12.0);
        ampmText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Text"));
        ampmText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        ampmText.Foreground(wuxm::SolidColorBrush(wu::Colors::White()));
        ampmBadge.Child(ampmText);

        topContainer.Children().Append(ampmBadge);
        g_ampmTextBlock = ampmText;
    }

    // --- 2. BOTTOM FLAP (lower half card & digits) ---
    wuxc::Grid botContainer;
    botContainer.Width(width);
    botContainer.Height(height);

    wuxm::RectangleGeometry botClip;
    botClip.Rect(winrt::Windows::Foundation::Rect{ 0.0f, (float)(halfHeight + seamGap), (float)width, (float)halfHeight });
    botContainer.Clip(botClip);

    // Bottom flap acrylic body & border: 2 px inner cut, rounded bottom
    // corners, open top edge.
    wuxc::Border bottomFlap;
    bottomFlap.Width(width);
    bottomFlap.Height(halfHeight);
    bottomFlap.VerticalAlignment(wux::VerticalAlignment::Bottom);
    bottomFlap.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb((uint8_t)g_settings.glassOpacity, 255, 255, 255)));
    bottomFlap.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(80, 255, 255, 255)));
    bottomFlap.BorderThickness(wux::Thickness{ 1.5, 0.0, 1.5, 1.5 });
    bottomFlap.CornerRadius(wux::CornerRadiusHelper::FromRadii(2.0, 2.0, cornerRadius, cornerRadius));
    botContainer.Children().Append(bottomFlap);

    // Bottom digit: same center coordinates as the top digit; the upper half
    // is clipped at the split.
    std::wstring botName = std::wstring(textName) + L"_Bot";
    wuxc::TextBlock botText;
    botText.Name(botName);
    botText.FontSize(fontSize);
    botText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    botText.FontWeight(winrt::Windows::UI::Text::FontWeights::ExtraBold());
    botText.Foreground(wuxm::SolidColorBrush(wu::Colors::White()));
    botText.HorizontalAlignment(wux::HorizontalAlignment::Center);
    botText.VerticalAlignment(wux::VerticalAlignment::Center);
    botText.Margin(wux::Thickness{ 0.0, opticalOffset, 0.0, 0.0 });
    wuxd::Typography::SetNumeralAlignment(botText, wux::FontNumeralAlignment::Tabular);
    botContainer.Children().Append(botText);

    card.Children().Append(topContainer);
    card.Children().Append(botContainer);

    outTopText = topText;
    outBotText = botText;
    return card;
}

// Build Flip Clock UI Element
wux::UIElement BuildFlipClockElement() {
    wuxc::Grid rootContainer;
    rootContainer.Name(L"WindhawkFlipClockContainer");
    ApplyAlignmentAndMargin(rootContainer);

    wuxc::StackPanel verticalLayout;
    verticalLayout.Orientation(wuxc::Orientation::Vertical);
    verticalLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);

    wuxc::StackPanel cardsLayout;
    cardsLayout.Orientation(wuxc::Orientation::Horizontal);
    cardsLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);
    cardsLayout.Spacing(22.0);

    // Near-square large chunky card proportions (aspect ratio ~0.99)
    double cardWidth = (double)g_settings.clockFontSize * 2.02;
    double cardHeight = (double)g_settings.clockFontSize * 2.05;
    double flipFontSize = (double)g_settings.clockFontSize * 1.72;
    double cornerRad = 22.0;

    // 1. Hour Card
    auto hourCard = CreateFlipCard(cardWidth, cardHeight, cornerRad, flipFontSize, g_hourTextBlock, g_hourBotTextBlock, L"WindhawkFlipHourText", true);
    cardsLayout.Children().Append(hourCard);

    // 2. Minute Card
    auto minCard = CreateFlipCard(cardWidth, cardHeight, cornerRad, flipFontSize, g_minTextBlock, g_minBotTextBlock, L"WindhawkFlipMinText", false);
    cardsLayout.Children().Append(minCard);

    verticalLayout.Children().Append(cardsLayout);

    // Date Capsule
    if (g_settings.showDateCapsule) {
        wuxc::Border dateCapsule;
        dateCapsule.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(35, 255, 255, 255)));
        dateCapsule.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(60, 255, 255, 255)));
        dateCapsule.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
        dateCapsule.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(12.0));
        dateCapsule.Padding(wux::Thickness{ 20.0, 8.0, 20.0, 8.0 });
        dateCapsule.Margin(wux::Thickness{ 0.0, 18.0, 0.0, 0.0 });
        dateCapsule.HorizontalAlignment(wux::HorizontalAlignment::Center);

        wuxc::TextBlock dateText;
        dateText.Name(L"WindhawkFlipDateText");
        dateText.FontSize(18.0);
        dateText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Text"));
        dateText.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        dateText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(240, 255, 255, 255)));
        dateText.HorizontalAlignment(wux::HorizontalAlignment::Center);

        dateCapsule.Child(dateText);
        verticalLayout.Children().Append(dateCapsule);
        g_dateTextBlock = dateText;
    }

    // Battery Capsule (if enabled and laptop battery present)
    if (g_settings.showBatteryStatus) {
        auto batteryCapsule = CreateBatteryCapsule(false);
        if (batteryCapsule) {
            verticalLayout.Children().Append(batteryCapsule);
        }
    }

    rootContainer.Children().Append(verticalLayout);
    UpdateTimeAndDate();

    return rootContainer;
}

// Build Single Vintage Flip Card Element (Retro Mechanical Airport/Station Solari Style)
wuxc::Grid CreateVintageFlipCard(double width, double height, double cornerRadius, double fontSize, wuxc::TextBlock& outText, const wchar_t* textName, bool isHourCard = false) {
    wuxc::Grid card;
    card.Width(width);
    card.Height(height);

    // Fliqlo matte charcoal black flap body (#0e0e0e) - frameless matte finish.
    wuxc::Border flapBackplate;
    flapBackplate.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(242, 14, 14, 14)));
    flapBackplate.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(cornerRadius));

    // Fliqlo Original Matte Light Silver Digit Typography (#b9b9b9 / rgb(185, 185, 185))
    wuxc::TextBlock cardText;
    cardText.Name(textName);
    cardText.FontSize(fontSize);
    cardText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    cardText.FontWeight(winrt::Windows::UI::Text::FontWeights::ExtraBold());
    cardText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 185, 185, 185)));
    cardText.HorizontalAlignment(wux::HorizontalAlignment::Center);
    cardText.VerticalAlignment(wux::VerticalAlignment::Center);
    double opticalOffset = fontSize * 0.045;
    cardText.Margin(wux::Thickness{ 0.0, opticalOffset, 0.0, 0.0 });
    wuxd::Typography::SetNumeralAlignment(cardText, wux::FontNumeralAlignment::Tabular);

    // Center Mechanical Split: Upper shadow + Lower light catch bevel + Side axle hinge clips
    wuxc::Grid splitSeam;
    splitSeam.Height(10.0);
    splitSeam.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    splitSeam.VerticalAlignment(wux::VerticalAlignment::Center);

    wuxc::StackPanel seamLines;
    seamLines.Orientation(wuxc::Orientation::Vertical);
    seamLines.VerticalAlignment(wux::VerticalAlignment::Center);

    // Deep shadow groove between top and bottom flaps
    wuxs::Rectangle shadowGroove;
    shadowGroove.Height(2.0);
    shadowGroove.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    shadowGroove.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 0, 0, 0)));
    shadowGroove.Margin(wux::Thickness{ 4.0, 0.0, 4.0, 0.0 });

    // Subtle edge highlight on bottom flap catching ambient overhead light
    wuxs::Rectangle bevelHighlight;
    bevelHighlight.Height(1.0);
    bevelHighlight.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    bevelHighlight.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(40, 255, 255, 255)));
    bevelHighlight.Margin(wux::Thickness{ 4.0, 0.0, 4.0, 0.0 });

    seamLines.Children().Append(shadowGroove);
    seamLines.Children().Append(bevelHighlight);
    splitSeam.Children().Append(seamLines);

    // Left and Right Axle Clips (Mechanical hinge pins)
    wuxs::Rectangle leftClip;
    leftClip.Width(4.0);
    leftClip.Height(9.0);
    leftClip.RadiusX(1.0);
    leftClip.RadiusY(1.0);
    leftClip.HorizontalAlignment(wux::HorizontalAlignment::Left);
    leftClip.VerticalAlignment(wux::VerticalAlignment::Center);
    leftClip.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(240, 8, 8, 8)));

    wuxs::Rectangle rightClip;
    rightClip.Width(4.0);
    rightClip.Height(9.0);
    rightClip.RadiusX(1.0);
    rightClip.RadiusY(1.0);
    rightClip.HorizontalAlignment(wux::HorizontalAlignment::Right);
    rightClip.VerticalAlignment(wux::VerticalAlignment::Center);
    rightClip.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(240, 8, 8, 8)));

    splitSeam.Children().Append(leftClip);
    splitSeam.Children().Append(rightClip);

    card.Children().Append(flapBackplate);
    card.Children().Append(cardText);
    card.Children().Append(splitSeam);
    outText = cardText;

    // Vintage Mechanical AM/PM Tag on Hour Card (12h format)
    if (isHourCard && g_settings.timeFormat == L"12h") {
        wuxc::Border ampmBadge;
        ampmBadge.HorizontalAlignment(wux::HorizontalAlignment::Left);
        ampmBadge.VerticalAlignment(wux::VerticalAlignment::Top);
        ampmBadge.Margin(wux::Thickness{ 12.0, 10.0, 0.0, 0.0 });
        ampmBadge.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(245, 14, 14, 14)));
        ampmBadge.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(4.0));
        ampmBadge.Padding(wux::Thickness{ 6.0, 1.5, 6.0, 1.5 });

        wuxc::TextBlock ampmText;
        ampmText.Name(L"WindhawkVintageAmPmText");
        ampmText.FontSize(11.0);
        ampmText.FontFamily(wuxm::FontFamily(L"Consolas"));
        ampmText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        ampmText.Foreground(wuxm::SolidColorBrush(wu::Colors::White()));
        ampmBadge.Child(ampmText);

        card.Children().Append(ampmBadge);
        g_ampmTextBlock = ampmText;
    }

    return card;
}

// Build Retro Vintage Flip Clock UI Element
wux::UIElement BuildVintageClockElement() {
    wuxc::Grid rootContainer;
    rootContainer.Name(L"WindhawkVintageClockContainer");
    ApplyAlignmentAndMargin(rootContainer);

    wuxc::StackPanel verticalLayout;
    verticalLayout.Orientation(wuxc::Orientation::Vertical);
    verticalLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);

    wuxc::StackPanel cardsLayout;
    cardsLayout.Orientation(wuxc::Orientation::Horizontal);
    cardsLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);
    cardsLayout.Spacing(20.0);

    // Near-square large chunky card proportions (aspect ratio ~0.99)
    double cardWidth = (double)g_settings.clockFontSize * 2.02;
    double cardHeight = (double)g_settings.clockFontSize * 2.05;
    double flipFontSize = (double)g_settings.clockFontSize * 1.72;
    double cornerRad = 18.0;

    // 1. Hour Card
    auto hourCard = CreateVintageFlipCard(cardWidth, cardHeight, cornerRad, flipFontSize, g_hourTextBlock, L"WindhawkVintageHourText", true);
    cardsLayout.Children().Append(hourCard);

    // 2. Minute Card
    auto minCard = CreateVintageFlipCard(cardWidth, cardHeight, cornerRad, flipFontSize, g_minTextBlock, L"WindhawkVintageMinText", false);
    cardsLayout.Children().Append(minCard);

    verticalLayout.Children().Append(cardsLayout);

    // Vintage Date Plate (Fliqlo Matte Dark Style)
    if (g_settings.showDateCapsule) {
        wuxc::Border dateCapsule;
        dateCapsule.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(220, 14, 14, 14)));
        dateCapsule.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(8.0));
        dateCapsule.Padding(wux::Thickness{ 22.0, 7.0, 22.0, 7.0 });
        dateCapsule.Margin(wux::Thickness{ 0.0, 18.0, 0.0, 0.0 });
        dateCapsule.HorizontalAlignment(wux::HorizontalAlignment::Center);

        wuxc::TextBlock dateText;
        dateText.Name(L"WindhawkVintageDateText");
        dateText.FontSize(18.0);
        dateText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Text"));
        dateText.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        dateText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(240, 185, 185, 185)));
        dateText.HorizontalAlignment(wux::HorizontalAlignment::Center);

        dateCapsule.Child(dateText);
        verticalLayout.Children().Append(dateCapsule);
        g_dateTextBlock = dateText;
    }

    // Battery Capsule (if enabled and laptop battery present)
    if (g_settings.showBatteryStatus) {
        auto batteryCapsule = CreateBatteryCapsule(false, true);
        if (batteryCapsule) {
            verticalLayout.Children().Append(batteryCapsule);
        }
    }

    rootContainer.Children().Append(verticalLayout);
    UpdateTimeAndDate();

    return rootContainer;
}

// Build Cyberpunk Sci-Fi HUD Terminal Clock Element (Authentic Netrunner / Tactical Cyberpunk HUD)
wux::UIElement BuildCyberpunkClockElement() {
    wuxc::Grid rootContainer;
    rootContainer.Name(L"WindhawkCyberpunkHUDContainer");
    ApplyAlignmentAndMargin(rootContainer);

    wuxc::StackPanel verticalLayout;
    verticalLayout.Orientation(wuxc::Orientation::Vertical);
    verticalLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);

    // Outer Tactical HUD Bracket Frame
    wuxc::Grid hudFrame;
    hudFrame.HorizontalAlignment(wux::HorizontalAlignment::Center);
    hudFrame.VerticalAlignment(wux::VerticalAlignment::Center);

    // 1. Dark Obsidian Glass Chassis with Neon Cyan Edge
    wuxc::Border chassis;
    chassis.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(215, 8, 10, 18)));
    chassis.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(130, 0, 245, 255))); // Electric Neon Cyan
    chassis.BorderThickness(wux::Thickness{ 1.5, 1.5, 1.5, 1.5 });
    chassis.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(6.0));
    chassis.Padding(wux::Thickness{ 32.0, 18.0, 32.0, 20.0 });

    wuxc::StackPanel hudContent;
    hudContent.Orientation(wuxc::Orientation::Vertical);
    hudContent.HorizontalAlignment(wux::HorizontalAlignment::Center);

    // --- TOP CYBER HEADER BAR ---
    wuxc::Grid topBar;
    topBar.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    topBar.Margin(wux::Thickness{ 0.0, 0.0, 0.0, 14.0 });

    // Left Tag: [ SYS.LOC // TIME_SYNC ]
    wuxc::TextBlock topTag;
    topTag.Text(L"SYS.LOC // TIME_SYNC");
    topTag.FontSize(11.0);
    topTag.FontFamily(wuxm::FontFamily(L"Consolas"));
    topTag.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
    topTag.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(220, 0, 245, 255)));
    topTag.HorizontalAlignment(wux::HorizontalAlignment::Left);
    topTag.VerticalAlignment(wux::VerticalAlignment::Center);

    // Right Telemetry Blocks: 3 Cyan + 1 Acid Yellow
    wuxc::StackPanel telemetryPills;
    telemetryPills.Orientation(wuxc::Orientation::Horizontal);
    telemetryPills.HorizontalAlignment(wux::HorizontalAlignment::Right);
    telemetryPills.VerticalAlignment(wux::VerticalAlignment::Center);

    wuxc::TextBlock liveLabel;
    liveLabel.Text(L"ONLINE ");
    liveLabel.FontSize(10.0);
    liveLabel.FontFamily(wuxm::FontFamily(L"Consolas"));
    liveLabel.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
    liveLabel.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(220, 252, 238, 10))); // Acid Yellow
    liveLabel.VerticalAlignment(wux::VerticalAlignment::Center);
    telemetryPills.Children().Append(liveLabel);
    g_cyberLiveLabel = liveLabel;

    for (int i = 0; i < 3; i++) {
        wuxs::Rectangle pill;
        pill.Width(6.0);
        pill.Height(9.0);
        pill.RadiusX(1.0);
        pill.RadiusY(1.0);
        pill.Margin(wux::Thickness{ 2.0, 0.0, 0.0, 0.0 });
        pill.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(230, 0, 245, 255)));
        telemetryPills.Children().Append(pill);
    }
    wuxs::Rectangle yellowPill;
    yellowPill.Width(6.0);
    yellowPill.Height(9.0);
    yellowPill.RadiusX(1.0);
    yellowPill.RadiusY(1.0);
    yellowPill.Margin(wux::Thickness{ 2.0, 0.0, 0.0, 0.0 });
    yellowPill.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(250, 252, 238, 10))); // Acid Yellow
    telemetryPills.Children().Append(yellowPill);

    // Tactical laptop battery telemetry (if battery present)
    auto bInfo = GetCurrentBatteryInfo();
    if (g_settings.showBatteryStatus && bInfo.hasBattery) {
        wuxc::Border cyberBat;
        cyberBat.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(40, 0, 245, 255)));
        cyberBat.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(90, 0, 245, 255)));
        cyberBat.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
        cyberBat.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(3.0));
        cyberBat.Padding(wux::Thickness{ 6.0, 1.0, 6.0, 1.0 });
        cyberBat.Margin(wux::Thickness{ 6.0, 0.0, 0.0, 0.0 });
        cyberBat.VerticalAlignment(wux::VerticalAlignment::Center);

        wuxc::TextBlock batText;
        batText.Name(L"WindhawkCyberBatteryText");
        WCHAR bBuf[32];
        swprintf_s(bBuf, bInfo.isCharging ? L"PWR: %d%% [CHG]" : L"BAT: %d%%", bInfo.percentage);
        batText.Text(bBuf);
        batText.FontSize(9.5);
        batText.FontFamily(wuxm::FontFamily(L"Consolas"));
        batText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        batText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(240, 0, 245, 255)));
        cyberBat.Child(batText);
        telemetryPills.Children().Append(cyberBat);
        g_batteryStatusText = batText;
    }

    topBar.Children().Append(topTag);
    topBar.Children().Append(telemetryPills);
    hudContent.Children().Append(topBar);

    // Top Header Divider (Neon Cyan hair line)
    wuxs::Rectangle headerDivider;
    headerDivider.Height(1.0);
    headerDivider.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    headerDivider.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(70, 0, 245, 255)));
    headerDivider.Margin(wux::Thickness{ 0.0, 0.0, 0.0, 16.0 });
    hudContent.Children().Append(headerDivider);

    // --- CENTER DIGITS DISPLAY ---
    wuxc::StackPanel digitsPanel;
    digitsPanel.Orientation(wuxc::Orientation::Horizontal);
    digitsPanel.HorizontalAlignment(wux::HorizontalAlignment::Center);
    digitsPanel.VerticalAlignment(wux::VerticalAlignment::Center);

    double numFontSize = (double)g_settings.clockFontSize * 1.05;
    double digitBlockWidth = numFontSize * 1.28;

    // Hour
    wuxc::TextBlock hourText;
    hourText.Name(L"WindhawkCyberHourText");
    hourText.FontSize(numFontSize);
    hourText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    hourText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
    hourText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 235, 252, 255))); // Electric White-Cyan
    hourText.Width(digitBlockWidth);
    hourText.TextAlignment(wux::TextAlignment::Center);
    wuxd::Typography::SetNumeralAlignment(hourText, wux::FontNumeralAlignment::Tabular);
    digitsPanel.Children().Append(hourText);
    g_hourTextBlock = hourText;

    // Slashes 1 (//) in Acid Yellow
    wuxc::TextBlock slash1;
    slash1.Text(L" // ");
    slash1.FontSize(numFontSize * 0.7);
    slash1.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    slash1.FontWeight(winrt::Windows::UI::Text::FontWeights::Light());
    slash1.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(230, 252, 238, 10))); // Acid Yellow
    slash1.VerticalAlignment(wux::VerticalAlignment::Center);
    slash1.Margin(wux::Thickness{ 4.0, 0.0, 4.0, 0.0 });
    digitsPanel.Children().Append(slash1);
    g_cyberSlash1 = slash1;

    // Minute
    wuxc::TextBlock minText;
    minText.Name(L"WindhawkCyberMinText");
    minText.FontSize(numFontSize);
    minText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    minText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
    minText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 235, 252, 255)));
    minText.Width(digitBlockWidth);
    minText.TextAlignment(wux::TextAlignment::Center);
    wuxd::Typography::SetNumeralAlignment(minText, wux::FontNumeralAlignment::Tabular);
    digitsPanel.Children().Append(minText);
    g_minTextBlock = minText;

    // Seconds (if enabled)
    if (g_settings.showSeconds) {
        wuxc::TextBlock slash2;
        slash2.Text(L" // ");
        slash2.FontSize(numFontSize * 0.7);
        slash2.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
        slash2.FontWeight(winrt::Windows::UI::Text::FontWeights::Light());
        slash2.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(240, 255, 0, 128))); // Hot Magenta
        slash2.VerticalAlignment(wux::VerticalAlignment::Center);
        slash2.Margin(wux::Thickness{ 4.0, 0.0, 4.0, 0.0 });
        digitsPanel.Children().Append(slash2);
        g_cyberSlash2 = slash2;

        wuxc::TextBlock secText;
        secText.Name(L"WindhawkCyberSecText");
        secText.FontSize(numFontSize);
        secText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
        secText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        secText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 235, 252, 255)));
        secText.Width(digitBlockWidth);
        secText.TextAlignment(wux::TextAlignment::Center);
        wuxd::Typography::SetNumeralAlignment(secText, wux::FontNumeralAlignment::Tabular);
        digitsPanel.Children().Append(secText);
        g_secTextBlock = secText;
    }

    // 12-Hour AM/PM Tactical Indicator
    if (g_settings.timeFormat == L"12h") {
        wuxc::Border ampmBadge;
        ampmBadge.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(50, 0, 245, 255)));
        ampmBadge.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(180, 0, 245, 255)));
        ampmBadge.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
        ampmBadge.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(4.0));
        ampmBadge.Padding(wux::Thickness{ 8.0, 4.0, 8.0, 4.0 });
        ampmBadge.Margin(wux::Thickness{ 12.0, 0.0, 0.0, 0.0 });
        ampmBadge.VerticalAlignment(wux::VerticalAlignment::Center);

        wuxc::TextBlock ampmText;
        ampmText.Name(L"WindhawkCyberAmPmText");
        ampmText.FontSize(numFontSize * 0.32);
        ampmText.FontFamily(wuxm::FontFamily(L"Consolas"));
        ampmText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        ampmText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 0, 245, 255)));
        ampmBadge.Child(ampmText);
        digitsPanel.Children().Append(ampmBadge);
        g_ampmTextBlock = ampmText;
    }

    hudContent.Children().Append(digitsPanel);

    // Bottom Scanline Bar (Dotted / Segmented Laser Track)
    wuxs::Rectangle scanline;
    scanline.Height(2.0);
    scanline.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    scanline.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(90, 0, 245, 255)));
    scanline.Margin(wux::Thickness{ 0.0, 14.0, 0.0, 14.0 });
    hudContent.Children().Append(scanline);

    // --- BOTTOM TELEMETRY DATE BAR ---
    if (g_settings.showDateCapsule) {
        wuxc::StackPanel bottomBar;
        bottomBar.Orientation(wuxc::Orientation::Horizontal);
        bottomBar.HorizontalAlignment(wux::HorizontalAlignment::Center);

        // Acid Yellow Telemetry Tag Badge
        wuxc::Border tagBadge;
        tagBadge.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(235, 252, 238, 10))); // Cyber Yellow
        tagBadge.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(3.0));
        tagBadge.Padding(wux::Thickness{ 7.0, 2.0, 7.0, 2.0 });
        tagBadge.Margin(wux::Thickness{ 0.0, 0.0, 10.0, 0.0 });

        wuxc::TextBlock tagText;
        tagText.Text(L"DATE.LOG");
        tagText.FontSize(11.0);
        tagText.FontFamily(wuxm::FontFamily(L"Consolas"));
        tagText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        tagText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 10, 12, 18))); // Deep Black
        tagBadge.Child(tagText);
        bottomBar.Children().Append(tagBadge);

        // Date String (Cyan glow)
        wuxc::TextBlock dateText;
        dateText.Name(L"WindhawkCyberDateText");
        dateText.FontSize(15.0);
        dateText.FontFamily(wuxm::FontFamily(L"Consolas"));
        dateText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        dateText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(240, 0, 245, 255))); // Electric Cyan
        dateText.VerticalAlignment(wux::VerticalAlignment::Center);
        bottomBar.Children().Append(dateText);
        g_dateTextBlock = dateText;

        hudContent.Children().Append(bottomBar);
    }

    chassis.Child(hudContent);
    hudFrame.Children().Append(chassis);

    // --- TACTICAL HUD CORNER ACCENTS (4 Tactical Chamfer Brackets) ---
    // 1. Top-Left L-bracket (Acid Yellow)
    wuxc::Grid cornerTL;
    cornerTL.HorizontalAlignment(wux::HorizontalAlignment::Left);
    cornerTL.VerticalAlignment(wux::VerticalAlignment::Top);
    cornerTL.Margin(wux::Thickness{ -2.0, -2.0, 0.0, 0.0 });
    wuxs::Rectangle tlH;
    tlH.Width(16.0); tlH.Height(3.0);
    tlH.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 252, 238, 10)));
    tlH.HorizontalAlignment(wux::HorizontalAlignment::Left);
    tlH.VerticalAlignment(wux::VerticalAlignment::Top);
    wuxs::Rectangle tlV;
    tlV.Width(3.0); tlV.Height(16.0);
    tlV.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 252, 238, 10)));
    tlV.HorizontalAlignment(wux::HorizontalAlignment::Left);
    tlV.VerticalAlignment(wux::VerticalAlignment::Top);
    cornerTL.Children().Append(tlH);
    cornerTL.Children().Append(tlV);
    hudFrame.Children().Append(cornerTL);

    // 2. Top-Right Accent (Hot Magenta)
    wuxc::Grid cornerTR;
    cornerTR.HorizontalAlignment(wux::HorizontalAlignment::Right);
    cornerTR.VerticalAlignment(wux::VerticalAlignment::Top);
    cornerTR.Margin(wux::Thickness{ 0.0, -2.0, -2.0, 0.0 });
    wuxs::Rectangle trH;
    trH.Width(16.0); trH.Height(3.0);
    trH.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 255, 0, 128)));
    trH.HorizontalAlignment(wux::HorizontalAlignment::Right);
    trH.VerticalAlignment(wux::VerticalAlignment::Top);
    wuxs::Rectangle trV;
    trV.Width(3.0); trV.Height(16.0);
    trV.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 255, 0, 128)));
    trV.HorizontalAlignment(wux::HorizontalAlignment::Right);
    trV.VerticalAlignment(wux::VerticalAlignment::Top);
    cornerTR.Children().Append(trH);
    cornerTR.Children().Append(trV);
    hudFrame.Children().Append(cornerTR);

    // 3. Bottom-Left Accent (Electric Cyan)
    wuxc::Grid cornerBL;
    cornerBL.HorizontalAlignment(wux::HorizontalAlignment::Left);
    cornerBL.VerticalAlignment(wux::VerticalAlignment::Bottom);
    cornerBL.Margin(wux::Thickness{ -2.0, 0.0, 0.0, -2.0 });
    wuxs::Rectangle blH;
    blH.Width(16.0); blH.Height(3.0);
    blH.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 0, 245, 255)));
    blH.HorizontalAlignment(wux::HorizontalAlignment::Left);
    blH.VerticalAlignment(wux::VerticalAlignment::Bottom);
    wuxs::Rectangle blV;
    blV.Width(3.0); blV.Height(16.0);
    blV.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 0, 245, 255)));
    blV.HorizontalAlignment(wux::HorizontalAlignment::Left);
    blV.VerticalAlignment(wux::VerticalAlignment::Bottom);
    cornerBL.Children().Append(blH);
    cornerBL.Children().Append(blV);
    hudFrame.Children().Append(cornerBL);

    // 4. Bottom-Right Accent (Electric Cyan)
    wuxc::Grid cornerBR;
    cornerBR.HorizontalAlignment(wux::HorizontalAlignment::Right);
    cornerBR.VerticalAlignment(wux::VerticalAlignment::Bottom);
    cornerBR.Margin(wux::Thickness{ 0.0, 0.0, -2.0, -2.0 });
    wuxs::Rectangle brH;
    brH.Width(16.0); brH.Height(3.0);
    brH.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 0, 245, 255)));
    brH.HorizontalAlignment(wux::HorizontalAlignment::Right);
    brH.VerticalAlignment(wux::VerticalAlignment::Bottom);
    wuxs::Rectangle brV;
    brV.Width(3.0); brV.Height(16.0);
    brV.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 0, 245, 255)));
    brV.HorizontalAlignment(wux::HorizontalAlignment::Right);
    brV.VerticalAlignment(wux::VerticalAlignment::Bottom);
    cornerBR.Children().Append(brH);
    cornerBR.Children().Append(brV);
    hudFrame.Children().Append(cornerBR);

    verticalLayout.Children().Append(hudFrame);
    rootContainer.Children().Append(verticalLayout);

    UpdateTimeAndDate();

    return rootContainer;
}

// Build Pixel Digit Grid Helper (7 cols x 11 rows, 77 dots)
wuxc::Grid CreatePixelDigitGrid(PixelDigitGrid& digitRef, double dotSize, double spacing) {
    wuxc::Grid grid;
    for (int c = 0; c < 7; c++) {
        wuxc::ColumnDefinition colDef;
        colDef.Width(wux::GridLengthHelper::FromPixels(dotSize + spacing));
        grid.ColumnDefinitions().Append(colDef);
    }
    for (int r = 0; r < 11; r++) {
        wuxc::RowDefinition rowDef;
        rowDef.Height(wux::GridLengthHelper::FromPixels(dotSize + spacing));
        grid.RowDefinitions().Append(rowDef);
    }

    digitRef.dots.clear();
    digitRef.dots.reserve(77);

    for (int r = 0; r < 11; r++) {
        for (int c = 0; c < 7; c++) {
            wuxs::Ellipse dot;
            dot.Width(dotSize);
            dot.Height(dotSize);
            dot.Fill(g_brushInactiveDot);
            dot.HorizontalAlignment(wux::HorizontalAlignment::Center);
            dot.VerticalAlignment(wux::VerticalAlignment::Center);
            wuxc::Grid::SetRow(dot, r);
            wuxc::Grid::SetColumn(dot, c);
            grid.Children().Append(dot);
            digitRef.dots.push_back(dot);
        }
    }
    return grid;
}

// Build Pixel Colon Helper (2 cols x 11 rows, 4 rows x 2 dots active per colon point)
wuxc::Grid CreatePixelColon(double dotSize, double spacing) {
    wuxc::Grid grid;
    for (int c = 0; c < 2; c++) {
        wuxc::ColumnDefinition colDef;
        colDef.Width(wux::GridLengthHelper::FromPixels(dotSize + spacing));
        grid.ColumnDefinitions().Append(colDef);
    }
    for (int r = 0; r < 11; r++) {
        wuxc::RowDefinition rowDef;
        rowDef.Height(wux::GridLengthHelper::FromPixels(dotSize + spacing));
        grid.RowDefinitions().Append(rowDef);
    }

    for (int r = 0; r < 11; r++) {
        for (int c = 0; c < 2; c++) {
            wuxs::Ellipse dot;
            dot.Width(dotSize);
            dot.Height(dotSize);
            bool isColonDot = (r == 2 || r == 3 || r == 7 || r == 8);
            dot.Fill(isColonDot ? g_brushActiveDot : g_brushInactiveDot);
            dot.HorizontalAlignment(wux::HorizontalAlignment::Center);
            dot.VerticalAlignment(wux::VerticalAlignment::Center);
            wuxc::Grid::SetRow(dot, r);
            wuxc::Grid::SetColumn(dot, c);
            grid.Children().Append(dot);
        }
    }
    return grid;
}

// Build Retro Pixel / LED Dot Matrix Clock Element
wux::UIElement BuildPixelClockElement() {
    wuxc::Grid rootContainer;
    rootContainer.Name(L"WindhawkPixelClockContainer");
    ApplyAlignmentAndMargin(rootContainer);

    // High-contrast brushes: 100% active white, subtle 6% ambient inactive dots
    g_brushActiveDot = wuxm::SolidColorBrush(wu::Colors::White());
    g_brushInactiveDot = wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(15, 255, 255, 255));

    // Dark frosted acrylic backplate for perfect contrast on any wallpaper
    wuxc::Border darkBackplate;
    darkBackplate.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(32, 10, 14, 22)));
    darkBackplate.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(15, 255, 255, 255)));
    darkBackplate.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
    darkBackplate.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(20.0));
    darkBackplate.Padding(wux::Thickness{ 26.0, 24.0, 26.0, 24.0 });
    darkBackplate.HorizontalAlignment(wux::HorizontalAlignment::Center);

    wuxc::StackPanel verticalLayout;
    verticalLayout.Orientation(wuxc::Orientation::Vertical);
    verticalLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);

    wuxc::StackPanel matrixLayout;
    matrixLayout.Orientation(wuxc::Orientation::Horizontal);
    matrixLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);
    matrixLayout.VerticalAlignment(wux::VerticalAlignment::Center);

    // Scale dot size appropriately for 7x11 double-pixel thickness
    double dotSize = (double)g_settings.clockFontSize * 0.095;
    double spacing = dotSize * 0.35;
    double digitGap = dotSize * 1.3;
    double colonGap = dotSize * 1.0;

    // H1 & H2
    auto h1 = CreatePixelDigitGrid(g_pixelDigits[0], dotSize, spacing);
    auto h2 = CreatePixelDigitGrid(g_pixelDigits[1], dotSize, spacing);
    h1.Margin(wux::Thickness{ 0.0, 0.0, digitGap, 0.0 });
    h2.Margin(wux::Thickness{ 0.0, 0.0, colonGap, 0.0 });
    matrixLayout.Children().Append(h1);
    matrixLayout.Children().Append(h2);

    // Colon 1 (between hours and minutes)
    auto colon1 = CreatePixelColon(dotSize, spacing);
    colon1.Margin(wux::Thickness{ 0.0, 0.0, colonGap, 0.0 });
    matrixLayout.Children().Append(colon1);

    // M1 & M2
    auto m1 = CreatePixelDigitGrid(g_pixelDigits[2], dotSize, spacing);
    auto m2 = CreatePixelDigitGrid(g_pixelDigits[3], dotSize, spacing);
    m1.Margin(wux::Thickness{ 0.0, 0.0, digitGap, 0.0 });
    matrixLayout.Children().Append(m1);
    matrixLayout.Children().Append(m2);

    // Optional Seconds (HH:MM:SS format at the same scale and alignment)
    if (g_settings.showSeconds) {
        m2.Margin(wux::Thickness{ 0.0, 0.0, colonGap, 0.0 });

        // Colon 2 (between minutes and seconds)
        auto colon2 = CreatePixelColon(dotSize, spacing);
        colon2.Margin(wux::Thickness{ 0.0, 0.0, colonGap, 0.0 });
        matrixLayout.Children().Append(colon2);

        // S1 & S2
        auto s1 = CreatePixelDigitGrid(g_pixelDigits[4], dotSize, spacing);
        auto s2 = CreatePixelDigitGrid(g_pixelDigits[5], dotSize, spacing);
        s1.Margin(wux::Thickness{ 0.0, 0.0, digitGap, 0.0 });
        matrixLayout.Children().Append(s1);
        matrixLayout.Children().Append(s2);
    }

    // Optional 12-Hour AM/PM Indicator
    if (g_settings.timeFormat == L"12h") {
        wuxc::Border ampmBadge;
        ampmBadge.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(35, 255, 255, 255)));
        ampmBadge.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(60, 255, 255, 255)));
        ampmBadge.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
        ampmBadge.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(6.0));
        ampmBadge.Padding(wux::Thickness{ 8.0, 3.0, 8.0, 3.0 });
        ampmBadge.Margin(wux::Thickness{ digitGap, 0.0, 0.0, 0.0 });
        ampmBadge.VerticalAlignment(wux::VerticalAlignment::Center);

        wuxc::TextBlock ampmText;
        ampmText.Name(L"WindhawkPixelAmPmText");
        ampmText.FontSize(dotSize * 2.8);
        ampmText.FontFamily(wuxm::FontFamily(L"Consolas"));
        ampmText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        ampmText.Foreground(wuxm::SolidColorBrush(wu::Colors::White()));
        ampmBadge.Child(ampmText);
        matrixLayout.Children().Append(ampmBadge);
        g_ampmTextBlock = ampmText;
    }

    verticalLayout.Children().Append(matrixLayout);

    // Date display
    if (g_settings.showDateCapsule) {
        wuxc::TextBlock dateText;
        dateText.Name(L"WindhawkPixelDateText");
        dateText.FontSize(19.0);
        dateText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Text"));
        dateText.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        dateText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(235, 255, 255, 255)));
        dateText.HorizontalAlignment(wux::HorizontalAlignment::Center);
        dateText.Margin(wux::Thickness{ 0.0, 22.0, 0.0, 0.0 });

        verticalLayout.Children().Append(dateText);
        g_dateTextBlock = dateText;
    }

    // Battery Capsule (if enabled and laptop battery present)
    if (g_settings.showBatteryStatus) {
        auto batteryCapsule = CreateBatteryCapsule(false);
        if (batteryCapsule) {
            verticalLayout.Children().Append(batteryCapsule);
        }
    }

    darkBackplate.Child(verticalLayout);
    rootContainer.Children().Append(darkBackplate);

    UpdateTimeAndDate();

    return rootContainer;
}

// Build Minimalist Aura / Floating Clock Element
wux::UIElement BuildAuraClockElement() {
    wuxc::Grid rootContainer;
    rootContainer.Name(L"WindhawkAuraClockContainer");
    ApplyAlignmentAndMargin(rootContainer);

    wuxc::StackPanel verticalLayout;
    verticalLayout.Orientation(wuxc::Orientation::Vertical);
    verticalLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);

    // Layered floating time container with soft ambient drop-shadow
    wuxc::Grid timeLayerGrid;
    timeLayerGrid.HorizontalAlignment(wux::HorizontalAlignment::Center);

    double fontSize = (double)(g_settings.clockFontSize * 1.35);

    // 1. Ambient Drop-Shadow Aura Layer (Behind foreground text for contrast on any wallpaper)
    wuxc::TextBlock shadowText;
    shadowText.FontSize(fontSize);
    shadowText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    shadowText.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    shadowText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(140, 0, 0, 0)));
    shadowText.HorizontalAlignment(wux::HorizontalAlignment::Center);
    shadowText.TextAlignment(wux::TextAlignment::Center);
    shadowText.Margin(wux::Thickness{ 0.0, 4.0, 0.0, 0.0 });
    wuxd::Typography::SetNumeralAlignment(shadowText, wux::FontNumeralAlignment::Tabular);
    timeLayerGrid.Children().Append(shadowText);
    g_auraShadowTextBlock = shadowText;

    // 2. Foreground Crisp Pure White Floating Text Layer
    wuxc::TextBlock timeText;
    timeText.FontSize(fontSize);
    timeText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    timeText.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    timeText.Foreground(wuxm::SolidColorBrush(wu::Colors::White()));
    timeText.HorizontalAlignment(wux::HorizontalAlignment::Center);
    timeText.TextAlignment(wux::TextAlignment::Center);
    wuxd::Typography::SetNumeralAlignment(timeText, wux::FontNumeralAlignment::Tabular);
    timeLayerGrid.Children().Append(timeText);
    g_auraTimeTextBlock = timeText;

    verticalLayout.Children().Append(timeLayerGrid);

    // Floating Aura Date Pill
    if (g_settings.showDateCapsule) {
        wuxc::Border auraDatePill;
        auraDatePill.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(38, 255, 255, 255)));
        auraDatePill.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(65, 255, 255, 255)));
        auraDatePill.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
        auraDatePill.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(18.0));
        auraDatePill.Padding(wux::Thickness{ 22.0, 7.0, 22.0, 7.0 });
        auraDatePill.Margin(wux::Thickness{ 0.0, 14.0, 0.0, 0.0 });
        auraDatePill.HorizontalAlignment(wux::HorizontalAlignment::Center);

        wuxc::TextBlock dateText;
        dateText.Name(L"WindhawkAuraDateText");
        dateText.FontSize(18.0);
        dateText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Text"));
        dateText.FontWeight(winrt::Windows::UI::Text::FontWeights::Medium());
        dateText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(245, 255, 255, 255)));
        dateText.HorizontalAlignment(wux::HorizontalAlignment::Center);

        auraDatePill.Child(dateText);
        verticalLayout.Children().Append(auraDatePill);
        g_dateTextBlock = dateText;
    }

    // Battery Capsule (if enabled and laptop battery present)
    if (g_settings.showBatteryStatus) {
        auto batteryCapsule = CreateBatteryCapsule(false);
        if (batteryCapsule) {
            verticalLayout.Children().Append(batteryCapsule);
        }
    }

    rootContainer.Children().Append(verticalLayout);
    UpdateTimeAndDate();

    return rootContainer;
}

// Build Minimalist Sonoma (Clean Typography) Clock Element
wux::UIElement BuildMacOSClockElement() {
    wuxc::Grid rootContainer;
    rootContainer.Name(L"WindhawkMacOSClockContainer");
    ApplyAlignmentAndMargin(rootContainer);

    bool isBottomLeft = (g_settings.clockAlignment == L"bottomLeft");

    wuxc::StackPanel verticalLayout;
    verticalLayout.Orientation(wuxc::Orientation::Vertical);
    verticalLayout.HorizontalAlignment(isBottomLeft ? wux::HorizontalAlignment::Left : wux::HorizontalAlignment::Center);

    // 1. macOS Date Header (Positioned elegantly ABOVE the time)
    if (g_settings.showDateCapsule) {
        wuxc::Grid dateLayerGrid;
        dateLayerGrid.HorizontalAlignment(isBottomLeft ? wux::HorizontalAlignment::Left : wux::HorizontalAlignment::Center);
        dateLayerGrid.Margin(wux::Thickness{ 0.0, 0.0, 0.0, 6.0 });

        double dateFontSize = (double)g_settings.clockFontSize * 0.23;
        if (dateFontSize < 18.0) dateFontSize = 18.0;

        // Atmospheric Shadow Layer for Date
        wuxc::TextBlock dateShadow;
        dateShadow.FontSize(dateFontSize);
        dateShadow.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
        dateShadow.FontWeight(winrt::Windows::UI::Text::FontWeights::Normal());
        dateShadow.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(70, 0, 0, 0)));
        dateShadow.HorizontalAlignment(isBottomLeft ? wux::HorizontalAlignment::Left : wux::HorizontalAlignment::Center);
        dateShadow.TextAlignment(isBottomLeft ? wux::TextAlignment::Left : wux::TextAlignment::Center);
        dateShadow.Margin(wux::Thickness{ 0.0, 1.5, 0.0, 0.0 });
        dateLayerGrid.Children().Append(dateShadow);
        g_macosDateShadowBlock = dateShadow;

        // Foreground Date Text
        wuxc::TextBlock dateText;
        dateText.Name(L"WindhawkMacOSDateText");
        dateText.FontSize(dateFontSize);
        dateText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
        dateText.FontWeight(winrt::Windows::UI::Text::FontWeights::Normal());
        dateText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(240, 255, 255, 255)));
        dateText.HorizontalAlignment(isBottomLeft ? wux::HorizontalAlignment::Left : wux::HorizontalAlignment::Center);
        dateText.TextAlignment(isBottomLeft ? wux::TextAlignment::Left : wux::TextAlignment::Center);
        dateLayerGrid.Children().Append(dateText);
        g_macosDateTextBlock = dateText;

        verticalLayout.Children().Append(dateLayerGrid);
    }

    // 2. Giant macOS Typography Time Display (Thin/Light, San Francisco aesthetic via Segoe UI Variable Display Light)
    wuxc::Grid timeLayerGrid;
    timeLayerGrid.HorizontalAlignment(isBottomLeft ? wux::HorizontalAlignment::Left : wux::HorizontalAlignment::Center);

    double timeFontSize = (double)g_settings.clockFontSize * 1.55;

    // Atmospheric Soft Shadow Layer for Time
    wuxc::TextBlock timeShadow;
    timeShadow.FontSize(timeFontSize);
    timeShadow.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    timeShadow.FontWeight(winrt::Windows::UI::Text::FontWeights::Light());
    timeShadow.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(80, 0, 0, 0)));
    timeShadow.HorizontalAlignment(isBottomLeft ? wux::HorizontalAlignment::Left : wux::HorizontalAlignment::Center);
    timeShadow.TextAlignment(isBottomLeft ? wux::TextAlignment::Left : wux::TextAlignment::Center);
    timeShadow.Margin(wux::Thickness{ 0.0, 3.5, 0.0, 0.0 });
    wuxd::Typography::SetNumeralAlignment(timeShadow, wux::FontNumeralAlignment::Tabular);
    timeLayerGrid.Children().Append(timeShadow);
    g_macosShadowTextBlock = timeShadow;

    // Foreground Pure White Clean Time Text
    wuxc::TextBlock timeText;
    timeText.Name(L"WindhawkMacOSTimeText");
    timeText.FontSize(timeFontSize);
    timeText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    timeText.FontWeight(winrt::Windows::UI::Text::FontWeights::Light());
    timeText.Foreground(wuxm::SolidColorBrush(wu::Colors::White()));
    timeText.HorizontalAlignment(isBottomLeft ? wux::HorizontalAlignment::Left : wux::HorizontalAlignment::Center);
    timeText.TextAlignment(isBottomLeft ? wux::TextAlignment::Left : wux::TextAlignment::Center);
    wuxd::Typography::SetNumeralAlignment(timeText, wux::FontNumeralAlignment::Tabular);
    timeLayerGrid.Children().Append(timeText);
    g_macosTimeTextBlock = timeText;

    verticalLayout.Children().Append(timeLayerGrid);

    // 3. macOS Sleek Battery Capsule (Underneath time if enabled)
    if (g_settings.showBatteryStatus) {
        auto batteryCapsule = CreateBatteryCapsule(isBottomLeft);
        if (batteryCapsule) {
            verticalLayout.Children().Append(batteryCapsule);
        }
    }

    rootContainer.Children().Append(verticalLayout);
    UpdateTimeAndDate();

    return rootContainer;
}

// Factory function for selected clock style
wux::UIElement BuildClockElementCore() {
    if (g_settings.clockStyle == L"pixel" && !studio.clockFont.empty()) return BuildStudioPixel();
    if (IsStudioStyle(g_settings.clockStyle)) return BuildStudio();
    if (g_settings.clockStyle == L"macos") {
        return BuildMacOSClockElement();
    } else if (g_settings.clockStyle == L"pixel") {
        return BuildPixelClockElement();
    } else if (g_settings.clockStyle == L"flip") {
        return BuildFlipClockElement();
    } else if (g_settings.clockStyle == L"vintage") {
        return BuildVintageClockElement();
    } else if (g_settings.clockStyle == L"cyberpunk") {
        return BuildCyberpunkClockElement();
    } else if (g_settings.clockStyle == L"aura" || IsStudioStyle(g_settings.clockStyle)) {
        return BuildAuraClockElement();
    }
    return BuildPixelClockElement();
}

// ---------------------------------------------------------------------------
wux::UIElement BuildClockElement() {
    auto element = BuildClockElementCore();
    ApplyStudioClockFonts(element);
    return element;
}
// UI thread discovery and synchronous dispatch
// ---------------------------------------------------------------------------

std::atomic<HWND> g_coreWindow{ nullptr };
std::atomic<HHOOK> g_getMessageHook{ nullptr };
std::atomic<bool> g_settingsDirty{ false };
HANDLE g_stopEvent = nullptr;
HANDLE g_workerThread = nullptr;

UINT g_runOnUiThreadMessage = 0;

using RunOnUiThreadProc = void(WINAPI*)(PVOID);

struct RunOnUiThreadParam {
    RunOnUiThreadProc proc;
    PVOID param;
    bool ran;
};

// WH_CALLWNDPROC hook used by RunOnUiThread(). It runs on the target UI thread
// before the window procedure sees the message.
LRESULT CALLBACK RunOnUiThreadHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (cwp && cwp->message == g_runOnUiThreadMessage) {
            auto* p = reinterpret_cast<RunOnUiThreadParam*>(cwp->lParam);
            if (p && p->proc) {
                try {
                    p->proc(p->param);
                    p->ran = true;
                } catch (winrt::hresult_error const& e) {
                    Wh_Log(L"UI task failed: %s", e.message().c_str());
                } catch (...) {
                    Wh_Log(L"UI task failed with an unknown error.");
                }
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// Runs proc on the thread that owns hWnd and returns only after it has
// finished. Mixing this with queued dispatcher callbacks is not allowed: the
// synchronous path is the only way to guarantee that no mod code is left
// behind when the DLL is unloaded.
bool RunOnUiThread(HWND hWnd, RunOnUiThreadProc proc, PVOID param) {
    if (!hWnd || !proc) {
        return false;
    }

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!threadId) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        proc(param);
        return true;
    }

    static std::mutex s_runMutex;
    std::lock_guard<std::mutex> lock(s_runMutex);

    if (!g_runOnUiThreadMessage) {
        return false;
    }

    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, RunOnUiThreadHookProc, nullptr, threadId);
    if (!hook) {
        Wh_Log(L"SetWindowsHookEx(WH_CALLWNDPROC) failed: %u", GetLastError());
        return false;
    }

    RunOnUiThreadParam p{ proc, param, false };
    SendMessageW(hWnd, g_runOnUiThreadMessage, 0, reinterpret_cast<LPARAM>(&p));
    UnhookWindowsHookEx(hook);
    return p.ran;
}

// ---------------------------------------------------------------------------
// Visual tree helpers
// ---------------------------------------------------------------------------

bool IsClockAttached() {
    if (!g_activeClockElement) {
        return false;
    }
    try {
        return wuxm::VisualTreeHelper::GetParent(g_activeClockElement) != nullptr;
    } catch (...) {
        return false;
    }
}

// Restores the local Visibility value that existed before this mod hid an
// element. ClearValue() is used when the element had no local value, which
// keeps Windows' own bindings and visual states intact.
void RestoreTrackedElements(std::vector<TrackedElement>& tracked) {
    auto prop = wux::UIElement::VisibilityProperty();
    for (auto& trackedElement : tracked) {
        if (!trackedElement.element) {
            continue;
        }
        try {
            if (trackedElement.hadLocalValue) {
                trackedElement.element.SetValue(prop, trackedElement.originalValue);
            } else {
                trackedElement.element.ClearValue(prop);
            }
        } catch (...) {
            // If restoring the saved value fails, at least remove our local
            // value so Windows can re-evaluate its own bindings and states.
            try {
                trackedElement.element.ClearValue(prop);
            } catch (...) {}
        }
    }
    tracked.clear();
    tracked.shrink_to_fit();
}

// Keeps the tracking list in sync with the current tree and hides only the
// elements that are actually supposed to be hidden.
void SyncTrackedElements(std::vector<TrackedElement>& tracked,
                         std::vector<wux::FrameworkElement> const& found,
                         bool shouldHide) {
    tracked.erase(
        std::remove_if(tracked.begin(), tracked.end(),
                       [&found](TrackedElement const& trackedElement) {
                           if (!trackedElement.element) {
                               return true;
                           }
                           return std::none_of(
                               found.begin(), found.end(),
                               [&trackedElement](wux::FrameworkElement const& element) {
                                   return element == trackedElement.element;
                               });
                       }),
        tracked.end());

    if (!shouldHide) {
        RestoreTrackedElements(tracked);
        return;
    }

    auto prop = wux::UIElement::VisibilityProperty();
    for (auto const& element : found) {
        if (!element) {
            continue;
        }

        bool alreadyTracked = std::any_of(
            tracked.begin(), tracked.end(),
            [&element](TrackedElement const& trackedElement) {
                return trackedElement.element == element;
            });
        if (alreadyTracked) {
            continue;
        }

        TrackedElement trackedElement;
        trackedElement.element = element;
        try {
            auto localValue = element.ReadLocalValue(prop);
            trackedElement.hadLocalValue =
                !(localValue == wux::DependencyProperty::UnsetValue());
            if (trackedElement.hadLocalValue) {
                trackedElement.originalValue = localValue;
            }
        } catch (...) {
            trackedElement.hadLocalValue = false;
        }

        tracked.push_back(trackedElement);
        try {
            element.Visibility(wux::Visibility::Collapsed);
        } catch (...) {}
    }

    // Re-assert the value on every scan. Local values normally win over visual
    // states, but re-applying is cheap and keeps the element hidden if Windows
    // replaces the tree or resets the property.
    for (auto& trackedElement : tracked) {
        try {
            trackedElement.element.Visibility(wux::Visibility::Collapsed);
        } catch (...) {}
    }
}

// ---------------------------------------------------------------------------
// Clock element management
// ---------------------------------------------------------------------------

void RemoveActiveClockElement() {
    if (!g_activeClockElement) {
        return;
    }
    if (g_lockRootGrid) {
        uint32_t index = 0;
        try {
            if (g_lockRootGrid.Children().IndexOf(g_activeClockElement, index)) {
                g_lockRootGrid.Children().RemoveAt(index);
            }
        } catch (...) {}
    }
    g_activeClockElement = nullptr;
}

// Releases every strong XAML reference. Must run on the lock screen UI thread.
void ResetClockElementReferences() {
    ResetStudioPixel();
    g_hourTextBlock = nullptr;
    g_hourBotTextBlock = nullptr;
    g_minTextBlock = nullptr;
    g_minBotTextBlock = nullptr;
    g_secTextBlock = nullptr;
    g_ampmTextBlock = nullptr;
    g_auraTimeTextBlock = nullptr;
    g_auraShadowTextBlock = nullptr;
    g_macosTimeTextBlock = nullptr;
    g_macosShadowTextBlock = nullptr;
    g_macosDateTextBlock = nullptr;
    g_macosDateShadowBlock = nullptr;
    g_batteryCapsule = nullptr;
    g_batteryIconText = nullptr;
    g_batteryStatusText = nullptr;
    g_cyberSlash1 = nullptr;
    g_cyberSlash2 = nullptr;
    g_cyberLiveLabel = nullptr;
    g_dateTextBlock = nullptr;

    for (int i = 0; i < 6; i++) {
        g_pixelDigits[i].dots.clear();
        g_pixelDigits[i].dots.shrink_to_fit();
        g_pixelDigits[i].lastVal = -1;
    }
    g_brushActiveDot = nullptr;
    g_brushInactiveDot = nullptr;
}

void RestoreDefaultElements() {
    RestoreStudioNative();
    RestoreTrackedElements(g_hiddenClockElements);
    RestoreTrackedElements(g_hiddenWidgetElements);
}

struct ScanResult {
    wuxc::Grid root{ nullptr };
    bool hasLockRoot = false;
    bool hasMainPage = false;
    std::vector<wux::FrameworkElement> clockElements;
    std::vector<wux::FrameworkElement> widgetElements;
};

void ScanNode(wux::DependencyObject const& node, ScanResult& result, int depth) {
    if (!node || depth > 96) {
        return;
    }

    // Our own clock contains hundreds of Ellipses in the pixel style. There is
    // nothing to find inside it, so skip the whole subtree.
    if (g_activeClockElement && node == g_activeClockElement) {
        return;
    }

    if (auto element = node.try_as<wux::FrameworkElement>()) {
        auto name = element.Name();

        if (name == L"LockRootGrid") {
            if (!result.hasLockRoot) {
                if (auto grid = element.try_as<wuxc::Grid>()) {
                    result.root = grid;
                    result.hasLockRoot = true;
                }
            }
        } else if (name == L"MainPageContainer") {
            if (!result.hasMainPage) {
                if (auto grid = element.try_as<wuxc::Grid>()) {
                    if (!result.hasLockRoot) {
                        result.root = grid;
                    }
                    result.hasMainPage = true;
                }
            }
        } else if (name == L"TimeAndDatePanel" || name == L"Time" || name == L"Date") {
            result.clockElements.push_back(element);
        } else if (name == L"AdaptiveCardDetailContainer" ||
                   name == L"DetailTextPanel" || name == L"LockWidgetCanvas" || name == L"WidgetGroupPanel") {
            result.widgetElements.push_back(element);
        }
    }

    int childCount = 0;
    try {
        childCount = wuxm::VisualTreeHelper::GetChildrenCount(node);
    } catch (...) {
        return;
    }
    for (int i = 0; i < childCount; i++) {
        try {
            ScanNode(wuxm::VisualTreeHelper::GetChild(node, i), result, depth + 1);
        } catch (...) {}
    }
}

// Finds the lock screen root and the stock clock/widget elements, then injects
// or refreshes the selected clock. Must run on the lock screen UI thread.
void AttachToTree() {
    auto window = wux::Window::Current();
    if (!window) return;
    auto content = window.Content();
    if (!content) {
        return;
    }

    ScanResult scan;
    ScanNode(content, scan, 0);
    if (!scan.root) {
        // The tree is still being built; the next tick will retry.
        return;
    }

    bool sameRoot = (g_lockRootGrid == scan.root);
    if (!sameRoot) {
        RestoreDefaultElements();
        RemoveActiveClockElement();
        ResetClockElementReferences();
        g_lockRootGrid = scan.root;
        g_timeCache.Invalidate();
        Wh_Log(L"Attached to the lock screen visual tree.");
    }

    ++studioNativeScan;
    SelectStudioWidget(content);
    WalkStudioNative(content, 0);
    UpdateStudioWidgets();
    FinishStudioNativeScan();
    bool hideClock =
        g_settings.hideDefaultClock && (g_settings.clockStyle != L"defaultOnly");

    SyncTrackedElements(g_hiddenWidgetElements, scan.widgetElements,
                        g_settings.hideDefaultWidgets && studio.widgetMode != L"studio");

    if (g_settings.clockStyle == L"defaultOnly") {
        RemoveActiveClockElement();
        g_timeCache.Invalidate();
        return;
    }

    if (!g_activeClockElement || !IsClockAttached()) {
        RemoveActiveClockElement();
        ResetClockElementReferences();
        g_timeCache.Invalidate();

        try {
            g_activeClockElement = BuildClockElement();
            if (g_activeClockElement) {
                g_lockRootGrid.Children().Append(g_activeClockElement);
                Wh_Log(L"Clock injected: style=%s, seconds=%d",
                       g_settings.clockStyle.c_str(), g_settings.showSeconds);
            }
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"Failed to inject clock: %s", e.message().c_str());
            g_activeClockElement = nullptr;
        } catch (...) {
            Wh_Log(L"Failed to inject clock: unknown error.");
            g_activeClockElement = nullptr;
        }
    }

    SyncTrackedElements(g_hiddenClockElements, scan.clockElements, hideClock && IsClockAttached());
    studioDumpDone = true;
    UpdateTimeAndDate();
}

// Reloads settings and rebuilds the clock on the UI thread.
void WINAPI RebuildOnUiThread(PVOID) {
    g_settingsDirty = false;
    LoadSettings();
    RestoreDefaultElements();
    studioDumpDone = false;
    RemoveActiveClockElement();
    ResetClockElementReferences();
    g_timeCache.Invalidate();
    g_lockRootGrid = nullptr;
    AttachToTree();
    Wh_Log(L"Settings applied on the lock screen UI thread.");
}

// ---------------------------------------------------------------------------
// Periodic UI work
// ---------------------------------------------------------------------------

ULONGLONG g_lastUiTick = 0;
ULONGLONG g_lastFullScan = 0;

void OnUiThreadTick() {
    ULONGLONG now = GetTickCount64();
    if (g_settingsDirty.exchange(false)) {
        try {
            RebuildOnUiThread(nullptr);
            g_lastFullScan = now;
        } catch (...) {
            g_settingsDirty = true;
            throw;
        }
        return;
    }

    // Runs independently of clock rendering. Cached media subtree only;
    // discovery is needed only while the card is absent or replaced.
    RefreshStudioMedia(now);

    int intervalMs = g_settings.showSeconds ? 500 : 1000;
    if ((now - g_lastUiTick) < static_cast<ULONGLONG>(intervalMs)) return;
    g_lastUiTick = now;
    bool hasClockStyle = (g_settings.clockStyle != L"defaultOnly");
    bool needScan = (!g_lockRootGrid || (hasClockStyle && !g_activeClockElement) ||
                     (now - g_lastFullScan) >= 3000);
    if (needScan) {
        g_lastFullScan = now;
        AttachToTree();
    } else if (hasClockStyle) {
        UpdateTimeAndDate();
    }
}
LRESULT CALLBACK GetMessageHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        try {
            OnUiThreadTick();
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"UI tick failed: %s", e.message().c_str());
        } catch (...) {
            Wh_Log(L"UI tick failed with an unknown error.");
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Teardown and worker thread
// ---------------------------------------------------------------------------

void WINAPI TeardownOnUiThread(PVOID) {
    // Unhook from the UI thread: no WH_GETMESSAGE callback can be executing at
    // the same time on this thread, so it is safe for the DLL to unload next.
    HHOOK hook = g_getMessageHook.exchange(nullptr);
    if (hook) {
        UnhookWindowsHookEx(hook);
    }

    RestoreDefaultElements();
    studioDumpDone = false;
    RemoveActiveClockElement();
    ResetClockElementReferences();
    g_lockRootGrid = nullptr;
    g_timeCache.Invalidate();
    Wh_Log(L"UI teardown complete.");
}

BOOL CALLBACK FindCoreWindowProc(HWND hWnd, LPARAM lParam) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    if (processId != GetCurrentProcessId()) {
        return TRUE;
    }

    WCHAR className[128];
    if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) > 0 &&
        _wcsicmp(className, L"Windows.UI.Core.CoreWindow") == 0) {
        *reinterpret_cast<HWND*>(lParam) = hWnd;
        return FALSE;
    }
    return TRUE;
}

HWND FindCoreWindow() {
    HWND found = nullptr;
    EnumWindows(FindCoreWindowProc, reinterpret_cast<LPARAM>(&found));
    if (!found) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (snapshot != INVALID_HANDLE_VALUE) {
            THREADENTRY32 entry{sizeof(entry)};
            if (Thread32First(snapshot, &entry)) do {
                if (entry.th32OwnerProcessID == GetCurrentProcessId())
                    EnumThreadWindows(entry.th32ThreadID, FindCoreWindowProc,
                                      reinterpret_cast<LPARAM>(&found));
                if (found) break;
            } while (Thread32Next(snapshot, &entry));
            CloseHandle(snapshot);
        }
    }
    return found;
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    // Wait for LockApp's CoreWindow, then install the message hook on its
    // thread. Retry until the window exists or the mod is unloaded.
    while (WaitForSingleObject(g_stopEvent, 250) == WAIT_TIMEOUT) {
        if (g_getMessageHook.load()) {
            break;
        }

        HWND window = FindCoreWindow();
        if (!window) {
            continue;
        }

        DWORD threadId = GetWindowThreadProcessId(window, nullptr);
        HHOOK hook = SetWindowsHookExW(WH_GETMESSAGE, GetMessageHookProc, nullptr, threadId);
        if (hook) {
            g_coreWindow = window;
            g_getMessageHook = hook;
            Wh_Log(L"Installed WH_GETMESSAGE hook on UI thread %u (hwnd=%p).",
                   threadId, window);
            break;
        }
        Wh_Log(L"SetWindowsHookEx(WH_GETMESSAGE) failed: %u", GetLastError());
    }

    // Wake the UI thread periodically. The hook itself throttles updates to
    // 500 ms when seconds are shown and to 1000 ms otherwise.
    while (WaitForSingleObject(g_stopEvent,
            IsWindowVisible(g_coreWindow.load()) ? 125 : 500) == WAIT_TIMEOUT) {
        HWND window = g_coreWindow.load();
        if (!window) {
            continue;
        }
        if (!IsWindow(window)) {
            Wh_Log(L"The lock screen window disappeared; stopping the worker.");
            break;
        }
        PostMessageW(window, WM_NULL, 0, 0);
    }

    return 0;
}

// ---------------------------------------------------------------------------
// Windhawk lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Lock Screen Studio initializing (Wh_ModInit)...");

    LoadSettings();

    g_runOnUiThreadMessage =
        RegisterWindowMessageW(L"Windhawk_LockScreenStudio_RunOnUiThread_v1");
    if (!g_runOnUiThreadMessage) {
        Wh_Log(L"RegisterWindowMessageW failed: %u", GetLastError());
        return FALSE;
    }

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"CreateEventW failed: %u", GetLastError());
        return FALSE;
    }

    g_coreWindow = nullptr;
    g_getMessageHook = nullptr;
    g_settingsDirty = false;

    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Lock Screen Studio uninitializing (Wh_ModUninit)...");

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    HWND window = g_coreWindow.load();
    HHOOK hook = g_getMessageHook.load();
    if (window && hook && IsWindow(window)) {
        // Synchronous. When this returns, TeardownOnUiThread has finished and
        // the WH_GETMESSAGE hook has been removed from the UI thread.
        if (!RunOnUiThread(window, TeardownOnUiThread, nullptr)) {
            Wh_Log(L"Failed to run teardown on the UI thread.");
            HHOOK remainingHook = g_getMessageHook.exchange(nullptr);
            if (remainingHook) {
                UnhookWindowsHookEx(remainingHook);
            }
            // XAML objects are intentionally left alone here. They belong to
            // the UI thread and releasing them from this thread would be unsafe.
        }
    } else {
        HHOOK remainingHook = g_getMessageHook.exchange(nullptr);
        if (remainingHook) {
            UnhookWindowsHookEx(remainingHook);
        }
    }

    g_coreWindow = nullptr;
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }

    Wh_Log(L"Lock Screen Studio uninitialized.");
}

void Wh_ModSettingsChanged() {
    g_settingsDirty = true;
    HWND window = g_coreWindow.load();
    if (window) PostMessageW(window, WM_NULL, 0, 0);
}